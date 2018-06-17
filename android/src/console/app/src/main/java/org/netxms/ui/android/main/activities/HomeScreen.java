package org.netxms.ui.android.main.activities;

import java.util.ArrayList;

import org.netxms.base.NXCommon;
import org.netxms.client.objects.AbstractObject;
import org.netxms.client.objects.GenericObject;
import org.netxms.ui.android.R;
import org.netxms.ui.android.main.adapters.ActivityListAdapter;
import org.netxms.ui.android.main.fragments.AlarmBrowserFragment;
import org.netxms.ui.android.service.ClientConnectorService;
import org.netxms.ui.android.service.ClientConnectorService.ConnectionStatus;

import android.annotation.SuppressLint;
import android.content.ComponentName;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.res.Configuration;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.preference.PreferenceManager;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.GridView;
import android.widget.TextView;
import android.widget.Toast;

/**
 * Home screen activity
 * 
 * @author Victor Kirhenshtein
 * @author Marco Incalcaterra (marco.incalcaterra@thinksoft.it)
 * 
 */

public class HomeScreen extends AbstractClientActivity implements OnItemClickListener
{
	public static final int ACTIVITY_ALARMS = 1;
	public static final int ACTIVITY_DASHBOARDS = 2;
	public static final int ACTIVITY_NODES = 3;
	public static final int ACTIVITY_ENTIRENETWORK = 4;
	public static final int ACTIVITY_GRAPHS = 5;
	public static final int ACTIVITY_MACADDRESS = 6;

	private ActivityListAdapter adapter;
	private TextView statusText;

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.netxms.ui.android.main.activities.AbstractClientActivity#onCreateStep2(android.os.Bundle)
	 */
	@Override
	public void onCreateStep2(Bundle savedInstanceState)
	{
		setIntentionalExit(false);// Allow autorestart on change connectivity status for premature exit
		setContentView(R.layout.homescreen);

		GridView gridview = (GridView)findViewById(R.id.ActivityList);
		if (getResources().getConfiguration().orientation == Configuration.ORIENTATION_LANDSCAPE)
			gridview.setNumColumns(5);
		else
			gridview.setNumColumns(2);
		adapter = new ActivityListAdapter(this);
		gridview.setAdapter(adapter);
		gridview.setOnItemClickListener(this);

		statusText = (TextView)findViewById(R.id.ScreenTitleSecondary);

		TextView buildName = (TextView)findViewById(R.id.MainScreenVersion);
		buildName.setText(getString(R.string.version) + " " + NXCommon.VERSION + " (" + getString(R.string.build_number) + ")");
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.netxms.ui.android.main.activities.AbstractClientActivity#onServiceConnected(android.content.ComponentName,
	 * android.os.IBinder)
	 */
	@Override
	public void onServiceConnected(ComponentName name, IBinder binder)
	{
		super.onServiceConnected(name, binder);
		service.registerHomeScreen(this);
		setStatusText(service.getConnectionStatusText(), service.getConnectionStatusColor());
		refreshActivityStatus();
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see android.app.Activity#onCreateOptionsMenu(android.view.Menu)
	 */
	@SuppressLint("InlinedApi")
	@Override
	public boolean onCreateOptionsMenu(Menu menu)
	{
		super.onCreateOptionsMenu(menu);
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.HONEYCOMB)
			menu.removeItem(android.R.id.home);
		menu.add(Menu.NONE, R.string.reconnect, Menu.NONE, getString(R.string.reconnect)).setIcon(android.R.drawable.ic_menu_revert);
		menu.add(Menu.NONE, R.string.exit, Menu.NONE, getString(R.string.exit)).setIcon(android.R.drawable.ic_menu_close_clear_cancel);
		return true;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see android.app.Activity#onOptionsItemSelected(android.view.MenuItem)
	 */
	@Override
	public boolean onOptionsItemSelected(MenuItem item)
	{
		if (item.getItemId() == R.string.reconnect)
		{
			if (service != null)
				service.reconnect(true);
			return true;
		}
		else if (item.getItemId() == R.string.exit)
		{
			exit();
			return true;
		}
		return super.onOptionsItemSelected(item);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see android.widget.AdapterView.OnItemClickListener#onItemClick(android.widget.AdapterView, android.view.View, int, long)
	 */
	@Override
	public void onItemClick(AdapterView<?> parent, View view, int position, long id)
	{
		// Avoid starting activity if no connection
		if (service != null && (service.getConnectionStatus() == ConnectionStatus.CS_CONNECTED || service.getConnectionStatus() == ConnectionStatus.CS_ALREADYCONNECTED))
			switch ((int)id)
			{
				case ACTIVITY_ALARMS:
					startActivity(new Intent(this, AlarmBrowserFragment.class));
					break;
				case ACTIVITY_NODES:
					startActivity(new Intent(this, NodeBrowser.class).putExtra("parentId", GenericObject.SERVICEROOT));
					break;
				case ACTIVITY_ENTIRENETWORK:
					startActivity(new Intent(this, NodeBrowser.class).putExtra("parentId", GenericObject.NETWORK));
					break;
				case ACTIVITY_GRAPHS:
					startActivity(new Intent(this, GraphBrowser.class));
					break;
				case ACTIVITY_MACADDRESS:
					startActivity(new Intent(this, ConnectionPointBrowser.class));
					break;
				case ACTIVITY_DASHBOARDS:
					startActivity(new Intent(this, DashboardBrowser.class));
					break;
				default:
					break;
			}
		else
			showToast(getString(R.string.notify_disconnected));
	}

	/**
	 * @param text
	 */
	public void setStatusText(String text, int color)
	{
		statusText.setTextColor(color);
		statusText.setText(text);
	}

	public void showToast(final String text)
	{
		new Handler(getMainLooper()).post(new Runnable()
		{
			@Override
			public void run()
			{
				Toast.makeText(getApplicationContext(), text, Toast.LENGTH_SHORT).show();
			}
		});
	}

	/**
	 * Exit from app and shutdown service
	 */
	public void exit()
	{
		if (service != null)
			service.shutdown();
		else
			setIntentionalExit(true);// Avoid autorestart on change connectivity status for intentional exit (shutdown will do itself)
		moveTaskToBack(true);
		System.exit(0);
	}

	/**
	 * Refresh pending status of alarms, called from service
	 */
	public void refreshPendingAlarms()
	{
		adapter.setPendingAlarms(service.getAlarms().length);
		adapter.notifyDataSetChanged();
	}

	/**
	 * Refresh the activity status, called from service
	 */
	public void refreshActivityStatus()
	{
		refreshPendingAlarms();
		ArrayList<AbstractObject> objList = new ArrayList<AbstractObject>();
		objList.add(service.findObjectById(GenericObject.NETWORK));
		objList.add(service.findObjectById(GenericObject.SERVICEROOT));
		objList.add(service.findObjectById(GenericObject.DASHBOARDROOT));
		adapter.setTopNodes(objList);
		adapter.notifyDataSetChanged();
	}

	/**
	 * Set a flag to inform about an intentional exit to avoid
	 * automatic reconnection on change connectivity status
	 * 
	 * @param flag true to signal an intentional exit
	 */
	private void setIntentionalExit(boolean flag)
	{
		SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(this);
		SharedPreferences.Editor editor = prefs.edit();
		editor.putBoolean(ClientConnectorService.INTENTIONAL_EXIT_KEY, flag);
		editor.commit();
	}
}
