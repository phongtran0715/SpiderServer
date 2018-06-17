/**
 * 
 */
package org.netxms.ui.android.service;

import java.text.DateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.netxms.base.Logger;
import org.netxms.client.NXCSession;
import org.netxms.client.SessionListener;
import org.netxms.client.SessionNotification;
import org.netxms.client.constants.Severity;
import org.netxms.client.datacollection.DciValue;
import org.netxms.client.events.Alarm;
import org.netxms.client.objects.AbstractObject;
import org.netxms.client.objecttools.ObjectTool;
import org.netxms.ui.android.NXApplication;
import org.netxms.ui.android.R;
import org.netxms.ui.android.helpers.SafeParser;
import org.netxms.ui.android.main.activities.DashboardBrowser;
import org.netxms.ui.android.main.activities.GraphBrowser;
import org.netxms.ui.android.main.activities.HomeScreen;
import org.netxms.ui.android.main.activities.NodeBrowser;
import org.netxms.ui.android.main.fragments.AlarmBrowserFragment;
import org.netxms.ui.android.receivers.AlarmIntentReceiver;
import org.netxms.ui.android.service.helpers.AndroidLoggingFacility;
import org.netxms.ui.android.service.tasks.ConnectTask;
import org.netxms.ui.android.service.tasks.ExecObjectToolTask;

import android.app.AlarmManager;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.content.res.Resources;
import android.net.Uri;
import android.os.Binder;
import android.os.Handler;
import android.os.IBinder;
import android.preference.PreferenceManager;
import android.support.v4.app.NotificationCompat;
import android.support.v4.content.Loader;
import android.util.Log;
import android.widget.Toast;

/**
 * Background communication service for NetXMS client.
 * 
 * @author Victor Kirhenshtein
 * @author Marco Incalcaterra (marco.incalcaterra@thinksoft.it)
 * 
 */

public class ClientConnectorService extends Service implements SessionListener
{
	public enum ConnectionStatus
	{
		CS_NOCONNECTION, CS_INPROGRESS, CS_ALREADYCONNECTED, CS_CONNECTED, CS_DISCONNECTED, CS_ERROR
	};

	public static final String ACTION_CONNECT = "org.netxms.ui.android.ACTION_CONNECT";
	public static final String ACTION_FORCE_CONNECT = "org.netxms.ui.android.ACTION_FORCE_CONNECT";
	public static final String ACTION_DISCONNECT = "org.netxms.ui.android.ACTION_DISCONNECT";
	public static final String ACTION_FORCE_DISCONNECT = "org.netxms.ui.android.ACTION_FORCE_DISCONNECT";
	public static final String ACTION_RESCHEDULE = "org.netxms.ui.android.ACTION_RESCHEDULE";
	public static final String ACTION_RECONNECT_ON_CONFIGURE = "org.netxms.ui.android.RECONNECT_ON_CONFIGURE";
	public static final String ACTION_ALARM_ACKNOWLEDGE = "org.netxms.ui.android.ACTION_ALARM_ACKNOWLEDGE";
	public static final String ACTION_ALARM_RESOLVE = "org.netxms.ui.android.ACTION_ALARM_RESOLVE";
	public static final String ACTION_ALARM_TERMINATE = "org.netxms.ui.android.ACTION_ALARM_TERMINATE";
	public static final String ACTION_EXIT = "org.netxms.ui.android.ACTION_EXIT";
	public static final String INTENTIONAL_EXIT_KEY = "IntentionalExit";

	private static final String TAG = "nxclient/ClientConnectorService";
	private static final String LASTALARM_KEY = "LastALarmIdNotified";
	private static final int NOTIFY_ALARM = 1;
	private static final int NOTIFY_STATUS = 2;
	private static final int NOTIFY_STATUS_NEVER = 0;
	private static final int NOTIFY_STATUS_ON_CONNECT = 1;
	private static final int NOTIFY_STATUS_ON_DISCONNECT = 2;
	private static final int NOTIFY_STATUS_ALWAYS = 3;
	private static final int ONE_DAY_MINUTES = 24 * 60;
	private static final int NETXMS_REQUEST_CODE = 123456;

	private final Object mutex = new Object();
	private final Binder binder = new ClientConnectorBinder();
	private Handler uiThreadHandler;
	private NotificationManager notificationManager;
	private NXCSession session = null;
	private ConnectionStatus connectionStatus = ConnectionStatus.CS_DISCONNECTED;
	private String connectionStatusText = "";
	private int connectionStatusColor = 0;
	private Map<Long, Alarm> alarms = null;
	private HomeScreen homeScreen = null;
	private NodeBrowser nodeBrowser = null;
	private GraphBrowser graphBrowser = null;
	private Alarm unknownAlarm = null;
	private long lastAlarmIdNotified;
	private List<ObjectTool> objectTools = null;
	private BroadcastReceiver receiver = null;
	private DashboardBrowser dashboardBrowser;
	private SharedPreferences sp;
	private final List<Loader<Alarm[]>> alarmLoaders = new ArrayList<Loader<Alarm[]>>(0);
	private final List<Loader<DciValue[]>> dciValueLoaders = new ArrayList<Loader<DciValue[]>>(0);
	private final List<Loader<AbstractObject>> genericObjectLoaders = new ArrayList<Loader<AbstractObject>>(0);
	private final List<Loader<Set<AbstractObject>>> genericObjectChildrenLoaders = new ArrayList<Loader<Set<AbstractObject>>>(0);
	private String server = "";
	private int port = 4701;
	private String login = "";
	private String password = "";
	private boolean encrypt = false;
	private boolean enabled = false;
	private boolean notifyAlarm = false;
	private boolean notifyAlarmInStatusBar = false;
	private boolean notifyAlarmBySound = false;
	private boolean notifyAlarmByLed = false;
	private boolean notifyAlarmByVibration = false;
	private boolean notifyAlarmCritical = false;
	private boolean notifyAlarmCriticalInStatusBar = false;
	private boolean notifyAlarmMajor = false;
	private boolean notifyAlarmMajorInStatusBar = false;
	private boolean notifyAlarmMinor = false;
	private boolean notifyAlarmMinorInStatusBar = false;
	private boolean notifyAlarmWarning = false;
	private boolean notifyAlarmWarningInStatusBar = false;
	private boolean notifyAlarmNormal = false;
	private boolean notifyAlarmNormalInStatusBar = false;
	private int notificationType = NOTIFY_STATUS_NEVER;
	private boolean notifyIcon = false;
	private boolean notifyToast = false;
	private int schedulerPostpone = 0;
	private int schedulerDuration = 0;
	private int schedulerInterval = 0;
	private boolean schedulerDailyEnabled = false;
	private int schedulerDailyOn = 0;
	private int schedulerDailyOff = 0;

	/**
	 * Class for clients to access. Because we know this service always runs in
	 * the same process as its clients, we don't need to deal with IPC.
	 */
	public class ClientConnectorBinder extends Binder
	{
		public ClientConnectorService getService()
		{
			return ClientConnectorService.this;
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see android.app.Service#onCreate()
	 */
	@Override
	public void onCreate()
	{
		super.onCreate();

		Logger.setLoggingFacility(new AndroidLoggingFacility());
		uiThreadHandler = new Handler(getMainLooper());
		notificationManager = (NotificationManager)getSystemService(NOTIFICATION_SERVICE);

		showToast(getString(R.string.notify_started));

		sp = PreferenceManager.getDefaultSharedPreferences(this);
		lastAlarmIdNotified = sp.getInt(LASTALARM_KEY, 0);
		hasToReconnect();

		receiver = new BroadcastReceiver()
		{
			@Override
			public void onReceive(Context context, Intent intent)
			{
				Intent i = new Intent(context, ClientConnectorService.class);
				i.setAction(ACTION_RESCHEDULE);
				context.startService(i);
			}
		};
		registerReceiver(receiver, new IntentFilter(Intent.ACTION_TIME_TICK));

		if (NXApplication.isActivityVisible())
			reconnect(false);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see android.app.Service#onStartCommand(android.content.Intent, int, int)
	 */
	@Override
	public int onStartCommand(Intent intent, int flags, int startId)
	{
		if ((intent != null) && (intent.getAction() != null))
		{
			Log.d(TAG, "onStartCommand: " + intent.getAction());
			if (intent.getAction().equals(ACTION_CONNECT))
				reconnect(false);
			else if (intent.getAction().equals(ACTION_FORCE_CONNECT))
				reconnect(true);
			else if (intent.getAction().equals(ACTION_DISCONNECT))
				disconnect(false);
			else if (intent.getAction().equals(ACTION_FORCE_DISCONNECT))
				disconnect(true);
			else if (intent.getAction().equals(ACTION_RESCHEDULE))
				if (NXApplication.isActivityVisible())
					reconnect(false);
				else
					disconnect(false);
			else if (intent.getAction().equals(ACTION_RECONNECT_ON_CONFIGURE))
			{
				if (hasToReconnect())
				{
					// Reset last alarm notified in case of app reconfiguration and force reconnection
					lastAlarmIdNotified = 0;
					clearNotifications();
					reconnect(true);
				}
				else
					reconnect(false);
			}
			else if (intent.getAction().equals(ACTION_ALARM_ACKNOWLEDGE))
			{
				acknowledgeAlarm(intent.getLongExtra("alarmId", 0), false);
				hideNotification(NOTIFY_ALARM);
			}
			else if (intent.getAction().equals(ACTION_ALARM_RESOLVE))
			{
				resolveAlarm(intent.getLongExtra("alarmId", 0));
				hideNotification(NOTIFY_ALARM);
			}
			else if (intent.getAction().equals(ACTION_ALARM_TERMINATE))
			{
				terminateAlarm(intent.getLongExtra("alarmId", 0));
				hideNotification(NOTIFY_ALARM);
			}
			else if (intent.getAction().equals(ACTION_EXIT))
				exitApp();
		}
		return super.onStartCommand(intent, flags, startId);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see android.app.Service#onBind(android.content.Intent)
	 */
	@Override
	public IBinder onBind(Intent intent)
	{
		return binder;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see android.app.Service#onDestroy()
	 */
	@Override
	public void onDestroy()
	{
		super.onDestroy();
	}

	/**
	 * Shutdown background service
	 */
	public void shutdown()
	{
		cancelSchedule();
		clearNotifications();
		saveSatusOnExit();
		unregisterReceiver(receiver);
		stopSelf();
	}

	/**
	 * Save status for intentional exit from app 
	 */
	public void saveSatusOnExit()
	{
		SharedPreferences.Editor editor = sp.edit();
		editor.putInt(LASTALARM_KEY, (int)lastAlarmIdNotified);
		editor.putBoolean(ClientConnectorService.INTENTIONAL_EXIT_KEY, true);
		editor.commit();
	}

	/**
	 * Show alarm notification (status bar, LED, sound and vibration)
	 * 
	 * @param alarm	alarm object
	 * @param text	notification text
	 */
	public void alarmNotification(Alarm alarm, String text)
	{
		Severity severity = alarm.getCurrentSeverity();
		if (NotifyAlarm(severity))
		{
			NotificationCompat.Builder nb = new NotificationCompat.Builder(getApplicationContext());
			final String sound = GetAlarmSound(severity);
			if ((sound != null) && (sound.length() > 0))
				nb.setSound(Uri.parse(sound));
			final int color = GetAlarmColor(severity);
			if ((color & 0x00FFFFFF) != 0) // Alpha channel is always set, check for black to disable signalling by LED
				nb.setLights(color, GetAlarmColorTimeOn(), GetAlarmColorTimeOff());
			nb.setVibrate(GetAlarmVibrationPattern());
			Notification notification;
			if (NotifyAlarmInStatusBar(severity))
			{
				Intent notifyIntent = new Intent(getApplicationContext(), AlarmBrowserFragment.class);
				PendingIntent intent = PendingIntent.getActivity(getApplicationContext(), 0, notifyIntent, Intent.FLAG_ACTIVITY_NEW_TASK);
				nb.setSmallIcon(getAlarmIcon(severity));
				nb.setWhen(System.currentTimeMillis());
				nb.setContentText(text);
				nb.setContentTitle(getString(R.string.notification_title)).setContentIntent(intent);
				nb.addAction(R.drawable.alarm_acknowledged, getString(R.string.alarm_acknowledge), createPendingIntent(ACTION_ALARM_ACKNOWLEDGE, alarm.getId()));
				nb.addAction(R.drawable.alarm_resolved, getString(R.string.alarm_resolve), createPendingIntent(ACTION_ALARM_RESOLVE, alarm.getId()));
				nb.addAction(R.drawable.alarm_terminated, getString(R.string.alarm_terminate), createPendingIntent(ACTION_ALARM_TERMINATE, alarm.getId()));
				notification = new NotificationCompat.BigTextStyle(nb).bigText(text).build();
			}
			else
				notification = new NotificationCompat.BigTextStyle(nb).build();
			notificationManager.notify(NOTIFY_ALARM, notification);
		}
	}

	/**
	 * Creates pending intent for notification area buttons
	 * 
	 * @param action	intent action
	 * @param id	alarm id on which to execute the action
	 */
	private PendingIntent createPendingIntent(String action, long id)
	{
		return PendingIntent.getService(getApplicationContext(), 0, new Intent(getApplicationContext(), ClientConnectorService.class).setAction(action).putExtra("alarmId", id), PendingIntent.FLAG_UPDATE_CURRENT);
	}

	/**
	 * Show status notification
	 * 
	 * @param status	connection status
	 * @param extra	extra text to add at the end of the toast
	 */
	public void statusNotification(ConnectionStatus status, String extra)
	{
		int icon = -1;
		String text = "";
		switch (status)
		{
			case CS_CONNECTED:
				if (notificationType == NOTIFY_STATUS_ON_CONNECT || notificationType == NOTIFY_STATUS_ALWAYS)
				{
					icon = R.drawable.ic_stat_connected;
				}
				text = getString(R.string.notify_connected, extra);
				break;
			case CS_DISCONNECTED:
				if (notificationType == NOTIFY_STATUS_ON_DISCONNECT || notificationType == NOTIFY_STATUS_ALWAYS)
				{
					icon = R.drawable.ic_stat_disconnected;
				}
				text = getString(R.string.notify_disconnected) + getNextConnectionRetry();
				break;
			case CS_ERROR:
				if (notificationType == NOTIFY_STATUS_ON_DISCONNECT || notificationType == NOTIFY_STATUS_ALWAYS)
				{
					icon = R.drawable.ic_stat_disconnected;
				}
				text = getString(R.string.notify_connection_failed, extra);
				break;
			case CS_NOCONNECTION:
			case CS_INPROGRESS:
			case CS_ALREADYCONNECTED:
			default:
				return;
		}
		if (icon == -1)
			hideNotification(NOTIFY_STATUS);
		else
		{
			if (notifyToast)
				showToast(text);
			if (notifyIcon)
			{
				Intent notifyIntent = new Intent(getApplicationContext(), HomeScreen.class);
				PendingIntent intent = PendingIntent.getActivity(getApplicationContext(), 0, notifyIntent, Intent.FLAG_ACTIVITY_NEW_TASK);
				NotificationCompat.Builder nb = new NotificationCompat.Builder(getApplicationContext());
				nb.setSmallIcon(icon);
				nb.setWhen(System.currentTimeMillis());
				nb.setAutoCancel(false);
				nb.setOnlyAlertOnce(true);
				nb.setOngoing(true);
				nb.setContentText(text);
				nb.setContentTitle(getString(R.string.notification_title));
				nb.setContentIntent(intent);
				nb.addAction(android.R.drawable.ic_menu_revert, getString(R.string.reconnect), createPendingIntent(ACTION_FORCE_CONNECT, 0));
				nb.addAction(android.R.drawable.ic_menu_close_clear_cancel, getString(R.string.exit), createPendingIntent(ACTION_EXIT, 0));
				notificationManager.notify(NOTIFY_STATUS, nb.build());
			}
		}
	}

	/**
	 * Hide notification
	 * 
	 * @param id notification id
	 */
	private void hideNotification(int id)
	{
		notificationManager.cancel(id);
	}

	/**
	 * Clear all notifications
	 */
	public void clearNotifications()
	{
		notificationManager.cancelAll();
	}

	/**
	 * Check if it is necessary to reconnect depending if a 
	 * particular subset of parameters have been changed.
	 * 
	 * @return true If it is necessary to force a reconnection
	 */
	private boolean hasToReconnect()
	{
		boolean needsToReconnect = enabled != sp.getBoolean("global.scheduler.enable", false);
		needsToReconnect |= !server.equalsIgnoreCase(sp.getString("connection.server", ""));
		needsToReconnect |= port != SafeParser.parseInt(sp.getString("connection.port", "4701"), 4701);
		needsToReconnect |= !login.equals(sp.getString("connection.login", ""));
		needsToReconnect |= !password.equals(sp.getString("connection.password", ""));
		needsToReconnect |= encrypt != sp.getBoolean("connection.encrypt", false);
		needsToReconnect |= notifyAlarm != sp.getBoolean("global.notification.alarm", true);
		needsToReconnect |= notifyAlarmInStatusBar != sp.getBoolean("global.statusbar.alarm", true);
		needsToReconnect |= notifyAlarmBySound != sp.getBoolean("global.sound.alarm", false);
		needsToReconnect |= notifyAlarmByLed != sp.getBoolean("global.led.alarm", false);
		needsToReconnect |= notifyAlarmByVibration != sp.getBoolean("global.vibration.alarm", false);
		needsToReconnect |= notifyAlarmCritical != sp.getBoolean("alarm.notification.critical", true);
		needsToReconnect |= notifyAlarmCriticalInStatusBar != sp.getBoolean("alarm.statusbar.critical", true);
		needsToReconnect |= notifyAlarmMajor != sp.getBoolean("alarm.notification.major", true);
		needsToReconnect |= notifyAlarmMajorInStatusBar != sp.getBoolean("alarm.statusbar.major", true);
		needsToReconnect |= notifyAlarmMinor != sp.getBoolean("alarm.notification.minor", true);
		needsToReconnect |= notifyAlarmMinorInStatusBar != sp.getBoolean("alarm.statusbar.minor", true);
		needsToReconnect |= notifyAlarmWarning != sp.getBoolean("alarm.notification.warning", true);
		needsToReconnect |= notifyAlarmWarningInStatusBar != sp.getBoolean("alarm.statusbar.warning", true);
		needsToReconnect |= notifyAlarmNormal != sp.getBoolean("alarm.notification.normal", true);
		needsToReconnect |= notifyAlarmNormalInStatusBar != sp.getBoolean("alarm.statusbar.normal", true);
		needsToReconnect |= notificationType != Integer.parseInt(sp.getString("global.notification.status", "0"));
		needsToReconnect |= notifyIcon != sp.getBoolean("global.notification.icon", false);
		needsToReconnect |= notifyToast != sp.getBoolean("global.notification.toast", true);
		needsToReconnect |= schedulerPostpone != Integer.parseInt(sp.getString("global.scheduler.postpone", "1"));
		needsToReconnect |= schedulerDuration != Integer.parseInt(sp.getString("global.scheduler.duration", "1"));
		needsToReconnect |= schedulerInterval != Integer.parseInt(sp.getString("global.scheduler.interval", "15"));
		needsToReconnect |= schedulerDailyEnabled != sp.getBoolean("global.scheduler.daily.enable", false);
		needsToReconnect |= schedulerDailyOn != getMinutes("global.scheduler.daily.on");
		needsToReconnect |= schedulerDailyOff != getMinutes("global.scheduler.daily.off");

		enabled = sp.getBoolean("global.scheduler.enable", false);
		server = sp.getString("connection.server", "");
		port = SafeParser.parseInt(sp.getString("connection.port", "4701"), 4701);
		login = sp.getString("connection.login", "");
		password = sp.getString("connection.password", "");
		encrypt = sp.getBoolean("connection.encrypt", false);
		notifyAlarm = sp.getBoolean("global.notification.alarm", true);
		notifyAlarmInStatusBar = sp.getBoolean("global.statusbar.alarm", true);
		notifyAlarmBySound = sp.getBoolean("global.sound.alarm", false);
		notifyAlarmByLed = sp.getBoolean("global.led.alarm", false);
		notifyAlarmByVibration = sp.getBoolean("global.vibration.alarm", false);
		notifyAlarmCritical = sp.getBoolean("alarm.notification.critical", true);
		notifyAlarmCriticalInStatusBar = sp.getBoolean("alarm.statusbar.critical", true);
		notifyAlarmMajor = sp.getBoolean("alarm.notification.major", true);
		notifyAlarmMajorInStatusBar = sp.getBoolean("alarm.statusbar.major", true);
		notifyAlarmMinor = sp.getBoolean("alarm.notification.minor", true);
		notifyAlarmMinorInStatusBar = sp.getBoolean("alarm.statusbar.minor", true);
		notifyAlarmWarning = sp.getBoolean("alarm.notification.warning", true);
		notifyAlarmWarningInStatusBar = sp.getBoolean("alarm.statusbar.warning", true);
		notifyAlarmNormal = sp.getBoolean("alarm.notification.normal", true);
		notifyAlarmNormalInStatusBar = sp.getBoolean("alarm.statusbar.normal", true);
		notificationType = Integer.parseInt(sp.getString("global.notification.status", "0"));
		notifyIcon = sp.getBoolean("global.notification.icon", false);
		notifyToast = sp.getBoolean("global.notification.toast", true);
		schedulerPostpone = Integer.parseInt(sp.getString("global.scheduler.postpone", "1"));
		schedulerDuration = Integer.parseInt(sp.getString("global.scheduler.duration", "1"));
		schedulerInterval = Integer.parseInt(sp.getString("global.scheduler.interval", "15"));
		schedulerDailyEnabled = sp.getBoolean("global.scheduler.daily.enable", false);
		schedulerDailyOn = getMinutes("global.scheduler.daily.on");
		schedulerDailyOff = getMinutes("global.scheduler.daily.off");

		return needsToReconnect;
	}

	/**
	 * Reconnect to server.
	 * 
	 * @param forceReconnect if set to true forces disconnection before connecting
	 */
	public void reconnect(boolean force)
	{
		if (force || (isScheduleExpired() || NXApplication.isActivityVisible()) && connectionStatus != ConnectionStatus.CS_CONNECTED && connectionStatus != ConnectionStatus.CS_ALREADYCONNECTED)
		{
			Log.i(TAG, "Reconnecting...");
			synchronized (mutex)
			{
				if (connectionStatus != ConnectionStatus.CS_INPROGRESS)
				{
					setConnectionStatus(ConnectionStatus.CS_INPROGRESS, "");
					statusNotification(ConnectionStatus.CS_INPROGRESS, "");
					new ConnectTask(this).execute(server, port, login, password, encrypt, force);
				}
			}
		}
	}

	/**
	 * Disconnect from server. Only when scheduler is enabled and connected.
	 * 
	 */
	public void disconnect(boolean force)
	{
		if (force || enabled && !NXApplication.isActivityVisible() && (connectionStatus == ConnectionStatus.CS_CONNECTED || connectionStatus == ConnectionStatus.CS_ALREADYCONNECTED))
		{
			Log.i(TAG, "Disconnecting...");
			nullifySession();
			setConnectionStatus(ConnectionStatus.CS_DISCONNECTED, "");
			statusNotification(ConnectionStatus.CS_DISCONNECTED, "");
		}
	}

	/**
	 * Called by connect task after successful connection
	 * 
	 * @param session
	 *           new session object
	 */
	public void onConnect(NXCSession session, Map<Long, Alarm> alarms)
	{
		synchronized (mutex)
		{
			if (session != null)
			{
				schedule(ACTION_DISCONNECT);
				this.session = session;
				this.alarms = alarms;
				session.addListener(this);
				setConnectionStatus(ConnectionStatus.CS_CONNECTED, session.getServerAddress());
				statusNotification(ConnectionStatus.CS_CONNECTED, session.getServerAddress());
				if (alarms != null)
				{
					long id = -1;
					Alarm alarm = null;
					// Find the newest alarm received when we were offline
					for (Alarm itAlarm : alarms.values())
						if (itAlarm.getId() > id)
						{
							alarm = itAlarm;
							id = itAlarm.getId();
						}
					if (alarm != null && alarm.getId() > lastAlarmIdNotified)
						processAlarmChange(alarm);
				}
			}
		}
	}

	/**
	 * Called by connect task or session notification listener after unsuccessful
	 * connection or disconnect
	 */
	public void onDisconnect()
	{
		schedule(ACTION_CONNECT);
		nullifySession();
		setConnectionStatus(ConnectionStatus.CS_DISCONNECTED, "");
		statusNotification(ConnectionStatus.CS_DISCONNECTED, "");
	}

	/**
	 * Called by connect task on error during connection
	 */
	public void onError(String error)
	{
		nullifySession();
		setConnectionStatus(ConnectionStatus.CS_ERROR, error);
		statusNotification(ConnectionStatus.CS_ERROR, error);
	}

	/**
	 * Check for expired pending connection schedule
	 */
	private boolean isScheduleExpired()
	{
		if (enabled)
		{
			Calendar cal = Calendar.getInstance();// get a Calendar object with current time
			return cal.getTimeInMillis() > sp.getLong("global.scheduler.next_activation", 0);
		}
		return true;
	}

	/**
	 * Gets stored time settings in minutes
	 */
	private int getMinutes(String time)
	{
		String[] vals = sp.getString(time, "00:00").split(":");
		return Integer.parseInt(vals[0]) * 60 + Integer.parseInt(vals[1]);
	}

	/**
	 * Sets the offset used to compute the next schedule
	 */
	private void setDayOffset(Calendar cal, int minutes)
	{
		cal.set(Calendar.HOUR_OF_DAY, 0);
		cal.set(Calendar.MINUTE, 0);
		cal.set(Calendar.SECOND, 0);
		cal.set(Calendar.MILLISECOND, 0);
		cal.add(Calendar.MINUTE, minutes);
	}

	/**
	 * Schedule a new connection/disconnection
	 */
	public void schedule(String action)
	{
		Log.i(TAG, "Schedule: " + action);
		if (!enabled)
			cancelSchedule();
		else
		{
			Calendar cal = Calendar.getInstance();// get a Calendar object with current time
			if (action.equals(ACTION_RESCHEDULE))
				cal.add(Calendar.MINUTE, schedulerPostpone);
			if (action.equals(ACTION_DISCONNECT))
				cal.add(Calendar.MINUTE, schedulerDuration);
			else if (!schedulerDailyEnabled)
				cal.add(Calendar.MINUTE, schedulerInterval);
			else
			{
				int on = schedulerDailyOn;
				int off = schedulerDailyOff;
				if (off < on)
					off += ONE_DAY_MINUTES;// Next day!
				Calendar calOn = (Calendar)cal.clone();
				setDayOffset(calOn, on);
				Calendar calOff = (Calendar)cal.clone();
				setDayOffset(calOff, off);
				cal.add(Calendar.MINUTE, schedulerInterval);
				if (cal.before(calOn))
				{
					cal = (Calendar)calOn.clone();
					Log.i(TAG, "schedule (before): rescheduled for daily interval");
				}
				else if (cal.after(calOff))
				{
					cal = (Calendar)calOn.clone();
					setDayOffset(cal, on + ONE_DAY_MINUTES);// Move to the next activation of the excluded range
					Log.i(TAG, "schedule (after): rescheduled for daily interval");
				}
			}
			setSchedule(cal.getTimeInMillis(), action);
		}
	}

	/**
	 * Set a connection schedule
	 */
	private void setSchedule(long milliseconds, String action)
	{
		Intent intent = new Intent(this, AlarmIntentReceiver.class);
		intent.putExtra("action", action);
		PendingIntent sender = PendingIntent.getBroadcast(this, NETXMS_REQUEST_CODE, intent, PendingIntent.FLAG_UPDATE_CURRENT);
		((AlarmManager)getSystemService(ALARM_SERVICE)).set(AlarmManager.RTC_WAKEUP, milliseconds, sender);
		// Update status
		long last = sp.getLong("global.scheduler.next_activation", 0);
		Editor e = sp.edit();
		e.putLong("global.scheduler.last_activation", last);
		e.putLong("global.scheduler.next_activation", milliseconds);
		e.commit();
	}

	/**
	 * Cancel a pending connection schedule (if any)
	 */
	public void cancelSchedule()
	{
		Intent intent = new Intent(this, AlarmIntentReceiver.class);
		PendingIntent sender = PendingIntent.getBroadcast(this, NETXMS_REQUEST_CODE, intent, PendingIntent.FLAG_UPDATE_CURRENT);
		((AlarmManager)getSystemService(ALARM_SERVICE)).cancel(sender);
		if (sp.getLong("global.scheduler.next_activation", 0) != 0)
		{
			Editor e = sp.edit();
			e.putLong("global.scheduler.next_activation", 0);
			e.commit();
		}
	}

	/**
	 * Release internal resources nullifying current session (if any)
	 */
	private void nullifySession()
	{
		synchronized (mutex)
		{
			if (session != null)
			{
				session.disconnect();
				session.removeListener(this);
				session = null;
			}
			alarms = null;
			unknownAlarm = null;
		}
	}

	/**
	 * Process alarm change
	 * 
	 * @param alarm
	 */
	private void processAlarmChange(Alarm alarm)
	{
		AbstractObject object = findObjectById(alarm.getSourceObjectId());
		synchronized (mutex)
		{
			if (alarms != null)
			{
				lastAlarmIdNotified = alarm.getId();
				alarms.put(lastAlarmIdNotified, alarm);
			}
			unknownAlarm = object == null ? alarm : null;
			alarmNotification(alarm, ((object != null) ? object.getObjectName() : getString(R.string.node_unknown)) + ": " + alarm.getMessage());
			refreshAlarmBrowser();
		}
	}

	/**
	 * Process alarm change
	 * 
	 * @param alarm
	 */
	private void processAlarmDelete(long id)
	{
		synchronized (mutex)
		{
			if (alarms != null)
				alarms.remove(id);
			if (lastAlarmIdNotified == id)
				hideNotification(NOTIFY_ALARM);
			refreshAlarmBrowser();
		}
	}

	/**
	 * Synchronize information about specific object in background
	 * 
	 * @param objectId object ID
	 */
	private void doBackgroundObjectSync(final long objectId)
	{
		new Thread("Background object sync")
		{
			@Override
			public void run()
			{
				try
				{
					if (session != null)
						session.syncObjectSet(new long[] { objectId }, false, NXCSession.OBJECT_SYNC_NOTIFY);
				}
				catch (Exception e)
				{
					Log.e(TAG, "Exception in doBackgroundObjectSync", e);
				}
			}
		}.start();
	}

	/**
	 * @param objectId
	 * @return
	 */
	public AbstractObject findObjectById(long objectId)
	{
		return findObjectById(objectId, AbstractObject.class);
	}

	/**
	 * Find object by ID with class filter
	 * 
	 * @param objectId
	 * @param classFilter
	 * @return
	 */
	public AbstractObject findObjectById(long objectId, Class<? extends AbstractObject> classFilter)
	{
		// we can't search without active session
		if (session == null)
			return null;

		AbstractObject object = session.findObjectById(objectId, classFilter);
		// if we don't have object - probably we never synced it
		// request object synchronization in that case
		if (object == null)
		{
			doBackgroundObjectSync(objectId);
		}
		return object;
	}

	/**
	 * @param object
	 */
	private void processObjectUpdate(AbstractObject object)
	{
		synchronized (mutex)
		{
			if (unknownAlarm != null && unknownAlarm.getSourceObjectId() == object.getObjectId()) // Update <Unknown> notification
			{
				alarmNotification(unknownAlarm, object.getObjectName() + ": " + unknownAlarm.getMessage());
				unknownAlarm = null;
			}
			refreshHomeScreen();
			refreshAlarmBrowser();
			refreshNodeBrowser();
			refreshDashboardBrowser();
		}
	}

	/**
	 * Exit intentionally from App
	 */
	private void exitApp()
	{
		if (homeScreen != null)
		{
			homeScreen.runOnUiThread(new Runnable()
			{
				@Override
				public void run()
				{
					homeScreen.exit();
				}
			});
		}
		else	// Home app is no more running
			shutdown();
	}

	/**
	 * Refresh homescreen activity
	 */
	private void refreshHomeScreen()
	{
		if (homeScreen != null)
		{
			homeScreen.runOnUiThread(new Runnable()
			{
				@Override
				public void run()
				{
					homeScreen.refreshActivityStatus();
				}
			});
		}
	}

	/**
	 * Refresh the alarms related activities
	 */
	private void refreshAlarmBrowser()
	{
		if (homeScreen != null)
		{
			homeScreen.runOnUiThread(new Runnable()
			{
				@Override
				public void run()
				{
					homeScreen.refreshPendingAlarms();
				}
			});
		}
		for (Loader<Alarm[]> l : alarmLoaders)
			l.forceLoad();
	}

	/**
	 * Refresh the node browser activity
	 */
	private void refreshNodeBrowser()
	{
		if (nodeBrowser != null)
		{
			nodeBrowser.runOnUiThread(new Runnable()
			{
				@Override
				public void run()
				{
					nodeBrowser.updateNodeList();
				}
			});
		}
		for (Loader<DciValue[]> l : dciValueLoaders)
			l.forceLoad();
		for (Loader<AbstractObject> l : genericObjectLoaders)
			l.forceLoad();
		for (Loader<Set<AbstractObject>> l : genericObjectChildrenLoaders)
			l.forceLoad();
	}

	/**
	 * Refresh dashboard browser activity
	 */
	private void refreshDashboardBrowser()
	{
		if (dashboardBrowser != null)
		{
			dashboardBrowser.runOnUiThread(new Runnable()
			{
				@Override
				public void run()
				{
					dashboardBrowser.updateDashboardList();
				}
			});
		}
	}

	/**
	 * Process graph update event
	 */
	private void processGraphUpdate()
	{
		synchronized (mutex)
		{
			if (this.graphBrowser != null)
			{
				graphBrowser.runOnUiThread(new Runnable()
				{
					@Override
					public void run()
					{
						graphBrowser.refreshList();
					}
				});
			}
		}
	}

	/**
	 * Determine the notification of alarm based on alarm severity
	 * 
	 * @param severity
	 */
	private boolean NotifyAlarm(Severity severity)
	{
		if (notifyAlarm)
			switch (severity)
			{
				case NORMAL:// Normal
					return notifyAlarmNormal;
				case WARNING:// Warning
					return notifyAlarmWarning;
				case MINOR:// Minor
					return notifyAlarmMinor;
				case MAJOR:// Major
					return notifyAlarmMajor;
				case CRITICAL:// Critical
					return notifyAlarmCritical;
//				case UNKNOWN:// Unknown
//				case TERMINATE:// Terminate
//				case RESOLVE:// Resolve
				default:
					break;
			}
		return false;
	}

	/**
	 * Determine the notification in status bar based on alarm severity
	 * 
	 * @param severity
	 */
	private boolean NotifyAlarmInStatusBar(Severity severity)
	{
		if (notifyAlarmInStatusBar)
			switch (severity)
			{
				case NORMAL:// Normal
					return notifyAlarmNormalInStatusBar;
				case WARNING:// Warning
					return notifyAlarmWarningInStatusBar;
				case MINOR:// Minor
					return notifyAlarmMinorInStatusBar;
				case MAJOR:// Major
					return notifyAlarmMajorInStatusBar;
				case CRITICAL:// Critical
					return notifyAlarmCriticalInStatusBar;
//				case UNKNOWN:// Unknown
//				case TERMINATE:// Terminate
//				case RESOLVE:// Resolve
				default:
					break;
			}
		return false;
	}

	/**
	 * Get alarm color based on alarm severity
	 * 
	 * @param severity
	 */
	int GetAlarmColor(Severity severity)
	{
		if (sp.getBoolean("global.led.alarm", false))
		{
			switch (severity)
			{
				case NORMAL:// Normal
					return sp.getInt("alarm.led.normal", 0);
				case WARNING:// Warning
					return sp.getInt("alarm.led.warning", 0);
				case MINOR:// Minor
					return sp.getInt("alarm.led.minor", 0);
				case MAJOR:// Major
					return sp.getInt("alarm.led.major", 0);
				case CRITICAL:// Critical
					return sp.getInt("alarm.led.critical", 0);
				case UNKNOWN:// Unknown
					return sp.getInt("alarm.led.unknown", 0);
				case TERMINATE:// Terminate
					return sp.getInt("alarm.led.terminate", 0);
				case RESOLVE:// Resolve
					return sp.getInt("alarm.led.resolve", 0);
			}
		}
		return 0x0;
	}

	/**
	 * Get alarm led time for on period
	 */
	int GetAlarmColorTimeOn()
	{
		return sp.getInt("alarm.led.time.on", 1000);// msecs
	}

	/**
	 * Get alarm led time for off period
	 */
	int GetAlarmColorTimeOff()
	{
		return sp.getInt("alarm.led.time.off", 2000);// msecs
	}

	/**
	 * Get alarm vibration pattern 
	 */
	private long[] GetAlarmVibrationPattern()
	{
		if (sp.getBoolean("global.vibration.alarm", false))
		{
			// In Morse Code, "s" = "dot-dot-dot", "o" = "dash-dash-dash" 
			int dot = 100;// Length of a Morse Code "dot" in milliseconds (real is 200) 
			int dash = 250;// Length of a Morse Code "dash" in milliseconds (real is 500)
			int short_gap = 100;// Length of Gap Between dots/dashes (real is 200)
			int medium_gap = 250;// Length of Gap Between Letters (real is 500)
			//int long_gap = 500;// Length of Gap Between Words (real is 1000)
			long[] pattern = {
					0, // Start immediately 
					dot,
					short_gap,
					dot,
					short_gap,
					dot, // s 
					medium_gap,
					dash,
					short_gap,
					dash,
					short_gap,
					dash, // o 
					medium_gap,
					dot,
					short_gap,
					dot,
					short_gap,
					dot// s 
			};
			return pattern;
		}
		return null;
	}

	/**
	 * Get alarm sound based on alarm severity
	 * 
	 * @param severity
	 */
	private String GetAlarmSound(Severity severity)
	{
		if (sp.getBoolean("global.sound.alarm", false))
			switch (severity)
			{
				case NORMAL:// Normal
					return sp.getString("alarm.sound.normal", "");
				case WARNING:// Warning
					return sp.getString("alarm.sound.warning", "");
				case MINOR:// Minor
					return sp.getString("alarm.sound.minor", "");
				case MAJOR:// Major
					return sp.getString("alarm.sound.major", "");
				case CRITICAL:// Critical
					return sp.getString("alarm.sound.critical", "");
				case UNKNOWN:// Unknown
					return sp.getString("alarm.sound.unknown", "");
				case TERMINATE:// Terminate
					return sp.getString("alarm.sound.terminate", "");
				case RESOLVE:// Resolve
					return sp.getString("alarm.sound.resolve", "");
			}
		return "";
	}

	/**
	 * Get alarm icon based on alarm severity
	 * 
	 * @param severity
	 */
	private int getAlarmIcon(Severity severity)
	{
		switch (severity)
		{
			case NORMAL:// Normal
				return R.drawable.status_normal;
			case WARNING:// Warning
				return R.drawable.status_warning;
			case MINOR:// Minor
				return R.drawable.status_minor;
			case MAJOR:// Major
				return R.drawable.status_major;
			case CRITICAL:// Critical
				return R.drawable.status_critical;
			case UNKNOWN:// Unknown
				return R.drawable.status_unknown;
			case TERMINATE:// Terminate
				return R.drawable.status_terminate;
			case RESOLVE:// Resolve
				return R.drawable.status_resolve;
		}
		return android.R.drawable.stat_notify_sdcard;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * org.netxms.api.client.SessionListener#notificationHandler(org.netxms.api
	 * .client.SessionNotification)
	 */
	@Override
	public void notificationHandler(SessionNotification n)
	{
		switch (n.getCode())
		{
			case SessionNotification.CONNECTION_BROKEN:
			case SessionNotification.SERVER_SHUTDOWN:
				onDisconnect();
				break;
			case SessionNotification.NEW_ALARM:
			case SessionNotification.ALARM_CHANGED:
				processAlarmChange((Alarm)n.getObject());
				break;
			case SessionNotification.ALARM_DELETED:
			case SessionNotification.ALARM_TERMINATED:
				processAlarmDelete(((Alarm)n.getObject()).getId());
				break;
			case SessionNotification.OBJECT_CHANGED:
			case SessionNotification.OBJECT_SYNC_COMPLETED:
				processObjectUpdate((AbstractObject)n.getObject());
				break;
			case SessionNotification.PREDEFINED_GRAPHS_CHANGED:
				processGraphUpdate();
				break;
			default:
				break;
		}
	}

	/**
	 * Get list of active alarms
	 * 
	 * @return list of active alarms
	 */
	public Alarm[] getAlarms()
	{
		Alarm[] a;
		synchronized (mutex)
		{
			if (alarms != null)
				a = alarms.values().toArray(new Alarm[alarms.size()]);
			else
				a = new Alarm[0];
		}
		return a;
	}

	/**
	 * @param id	id of alarm
	 * @param sticky	true for sticky acknowledge 
	 */
	public void acknowledgeAlarm(long id, boolean sticky)
	{
		try
		{
			if (session != null)
				session.acknowledgeAlarm(id, sticky, 0);
		}
		catch (Exception e)
		{
			Log.e(TAG, "Exception while executing session.acknowledgeAlarm", e);
		}
	}

	/**
	 * @param ids	list of id
	 */
	public void acknowledgeAlarms(ArrayList<Long> ids, boolean sticky)
	{
		for (int i = 0; i < ids.size(); i++)
			acknowledgeAlarm(ids.get(i).longValue(), sticky);
	}

	/**
	 * @param id	id of alarm
	 */
	public void resolveAlarm(long id)
	{
		try
		{
			if (session != null)
				session.resolveAlarm(id);
		}
		catch (Exception e)
		{
			Log.e(TAG, "Exception while executing session.resolveAlarm", e);
		}
	}

	/**
	 * @param ids	list of id
	 */
	public void resolveAlarms(ArrayList<Long> ids)
	{
		for (int i = 0; i < ids.size(); i++)
			resolveAlarm(ids.get(i).longValue());
	}

	/**
	 * @param id	id of alarm
	 */
	public void terminateAlarm(long id)
	{
		try
		{
			if (session != null)
				session.terminateAlarm(id);
		}
		catch (Exception e)
		{
			Log.e(TAG, "Exception while executing session.terminateAlarm", e);
		}
	}

	/**
	 * @param ids	list of id
	 */
	public void terminateAlarms(ArrayList<Long> ids)
	{
		for (int i = 0; i < ids.size(); i++)
			terminateAlarm(ids.get(i).longValue());
	}

	/**
	 * @param id
	 * @param state
	 */
	public void setObjectMgmtState(long id, boolean state)
	{
		try
		{
			if (session != null)
				session.setObjectManaged(id, state);
		}
		catch (Exception e)
		{
			Log.e(TAG, "Exception while executing session.setObjectManaged", e);
		}
	}

	/**
	 * 
	 */
	public void loadTools()
	{
		try
		{
			this.objectTools = session.getObjectTools();
		}
		catch (Exception e)
		{
			this.objectTools = null;
			Log.d(TAG, "Exception while executing session.getObjectTools", e);
		}
	}

	/**
	 * Execute object tool. Communication with server will be done in separate worker thread.
	 * 
	 * @param nodeId
	 * @param tool
	 */
	public void executeObjectTool(long nodeId, ObjectTool tool)
	{
		new ExecObjectToolTask().execute(new Object[] { session, nodeId, tool, this });
	}

	/**
	 * @return
	 */
	public List<ObjectTool> getTools()
	{
		return this.objectTools;
	}

	/**
	 * @param homeScreen
	 */
	public void registerHomeScreen(HomeScreen homeScreen)
	{
		this.homeScreen = homeScreen;
	}

	/**
	 * @param browser
	 */
	public void registerNodeBrowser(NodeBrowser browser)
	{
		nodeBrowser = browser;
	}

	/**
	 * @param browser
	 */
	public void registerGraphBrowser(GraphBrowser browser)
	{
		graphBrowser = browser;
	}

	/**
	 * @param browser
	 */
	public void registerDashboardBrowser(DashboardBrowser browser)
	{
		dashboardBrowser = browser;
	}

	public void registerAlarmLoader(Loader<Alarm[]> loader)
	{
		if (!alarmLoaders.contains(loader))
			alarmLoaders.add(loader);
	}

	public void unregisterAlarmLoader(Loader<Alarm[]> loader)
	{
		alarmLoaders.remove(loader);
	}

	public void registerDciValueLoader(Loader<DciValue[]> loader)
	{
		if (!dciValueLoaders.contains(loader))
			dciValueLoaders.add(loader);
	}

	public void unregisterDciValueLoader(Loader<DciValue[]> loader)
	{
		dciValueLoaders.remove(loader);
	}

	public void registerGenericObjectLoader(Loader<AbstractObject> loader)
	{
		if (!genericObjectLoaders.contains(loader))
			genericObjectLoaders.add(loader);
	}

	public void unregisterGenericObjectLoader(Loader<AbstractObject> loader)
	{
		genericObjectLoaders.remove(loader);
	}

	public void registerGenericObjectChildrenLoader(Loader<Set<AbstractObject>> loader)
	{
		if (!genericObjectChildrenLoaders.contains(loader))
			genericObjectChildrenLoaders.add(loader);
	}

	public void unregisterGenericObjectChildrenLoader(Loader<Set<AbstractObject>> loader)
	{
		genericObjectChildrenLoaders.remove(loader);
	}

	/**
	 * @return the connectionStatus
	 */
	public ConnectionStatus getConnectionStatus()
	{
		return connectionStatus;
	}

	/**
	 * @return the connectionStatusText
	 */
	public String getConnectionStatusText()
	{
		return connectionStatusText;
	}

	/**
	 * @return the connectionStatusColor
	 */
	public int getConnectionStatusColor()
	{
		return connectionStatusColor;
	}

	/**
	 * @param connectionStatus
	 *           the connectionStatus to set
	 */
	public void setConnectionStatus(ConnectionStatus connectionStatus, String extra)
	{
		Resources r = getResources();
		this.connectionStatus = connectionStatus;
		switch (connectionStatus)
		{
			case CS_NOCONNECTION:
				connectionStatusText = getString(R.string.notify_no_connection);
				connectionStatusColor = r.getColor(R.color.notify_no_connection);
				break;
			case CS_INPROGRESS:
				connectionStatusText = getString(R.string.notify_connecting);
				connectionStatusColor = r.getColor(R.color.notify_connecting);
				break;
			case CS_ALREADYCONNECTED:
				connectionStatusText = getString(R.string.notify_connected, extra);
				connectionStatusColor = r.getColor(R.color.notify_connected);
				break;
			case CS_CONNECTED:
				connectionStatusText = getString(R.string.notify_connected, extra);
				connectionStatusColor = r.getColor(R.color.notify_connected);
				break;
			case CS_DISCONNECTED:
				connectionStatusText = getString(R.string.notify_disconnected) + getNextConnectionRetry();
				connectionStatusColor = r.getColor(R.color.notify_disconnected);
				break;
			case CS_ERROR:
				connectionStatusText = getString(R.string.notify_connection_failed, extra);
				connectionStatusColor = r.getColor(R.color.notify_connection_failed);
				break;
			default:
				break;
		}
		if (homeScreen != null)
		{
			homeScreen.runOnUiThread(new Runnable()
			{
				@Override
				public void run()
				{
					homeScreen.setStatusText(connectionStatusText, connectionStatusColor);
					homeScreen.refreshActivityStatus();
				}
			});
		}
	}

	public String getNextConnectionRetry()
	{
		long next = sp.getLong("global.scheduler.next_activation", 0);
		if (next != 0)
		{
			Calendar cal = Calendar.getInstance();// get a Calendar object with current time
			if (cal.getTimeInMillis() < next)
			{
				cal.setTimeInMillis(next);
				return " " + getString(R.string.notify_next_connection_schedule, DateFormat.getDateTimeInstance().format(cal.getTime()));
			}
		}
		return "";
	}

	/**
	 * @return the session
	 */
	public NXCSession getSession()
	{
		return session;
	}

	/**
	 * Show toast with given text
	 * 
	 * @param text message text
	 */
	public void showToast(final String text)
	{
		uiThreadHandler.post(new Runnable()
		{
			@Override
			public void run()
			{
				Toast.makeText(getApplicationContext(), text, Toast.LENGTH_SHORT).show();
			}
		});
	}
}
