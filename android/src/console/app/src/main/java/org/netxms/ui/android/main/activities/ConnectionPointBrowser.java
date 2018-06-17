package org.netxms.ui.android.main.activities;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import org.json.JSONArray;
import org.json.JSONException;
import org.netxms.client.MacAddress;
import org.netxms.client.MacAddressFormatException;
import org.netxms.client.NXCException;
import org.netxms.client.NXCSession;
import org.netxms.client.objects.Interface;
import org.netxms.client.objects.Node;
import org.netxms.client.topology.ConnectionPoint;
import org.netxms.ui.android.R;
import org.netxms.ui.android.main.adapters.ConnectionPointListAdapter;
import org.netxms.ui.android.tools.BarcodeScannerIntegrator;
import org.netxms.ui.android.tools.BarcodeScannerIntentResult;

import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.res.Resources;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.IBinder;
import android.preference.PreferenceManager;
import android.util.Log;
import android.view.ContextMenu;
import android.view.ContextMenu.ContextMenuInfo;
import android.view.KeyEvent;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnKeyListener;
import android.widget.AdapterView;
import android.widget.AdapterView.AdapterContextMenuInfo;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.TextView;

/**
 * MAC address search result browser
 * 
 * @author Marco Incalcaterra (marco.incalcaterra@thinksoft.it)
 *
 */

public class ConnectionPointBrowser extends AbstractClientActivity
{
	private static final String TAG = "nxclient/ConnectionPointBrowser";
	private static final String CPLIST_KEY = "ConnectionPointList";
	private final int maxConnectionPoints = 50;
	private List<String> cpList = new ArrayList<String>(0);
	private EditText editText;
	private ListView listView;
	private ConnectionPointListAdapter adapter;
	private int nodeId = 0;
	private ProgressDialog dialog;
	private Resources r;

	/* (non-Javadoc)
	 * @see org.netxms.ui.android.main.activities.AbstractClientActivity#onCreateStep2(android.os.Bundle)
	 */
	@Override
	protected void onCreateStep2(Bundle savedInstanceState)
	{
		dialog = new ProgressDialog(this);
		r = getResources();
		setContentView(R.layout.connection_point_view);

		TextView title = (TextView)findViewById(R.id.ScreenTitlePrimary);
		title.setText(R.string.connection_point_title);
		// keeps current list of alarms as datasource for listview
		adapter = new ConnectionPointListAdapter(this);
		listView = (ListView)findViewById(R.id.ConnectionPointList);
		listView.setAdapter(adapter);
		registerForContextMenu(listView);

		editText = (EditText)findViewById(R.id.MacAddressToSearch);
		editText.setOnKeyListener(new OnKeyListener()
		{
			@Override
			public boolean onKey(View v, int keyCode, KeyEvent event)
			{
				if ((event.getAction() == KeyEvent.ACTION_DOWN) && (keyCode == KeyEvent.KEYCODE_ENTER)) // If the event is a key-down event on the "enter" button
				{
					nodeId = 0;
					startSearch();
					return true;
				}
				return false;
			}
		});

		final Button scanButton = (Button)findViewById(R.id.ScanBarcode);
		scanButton.setOnClickListener(new OnClickListener()
		{
			@Override
			public void onClick(View v)
			{
				nodeId = 0;
				BarcodeScannerIntegrator integrator = new BarcodeScannerIntegrator(ConnectionPointBrowser.this);
				integrator.initiateScan();
			}
		});

		nodeId = getIntent().getIntExtra("nodeId", 0);
		cpList = retrievePreferences(this, CPLIST_KEY, maxConnectionPoints);
		adapter.setConnectionPoint(cpList);
		adapter.notifyDataSetChanged();
	}

	/* (non-Javadoc)
	 * @see android.app.Activity#onActivityResult(int, int, android.content.Intent)
	 */
	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data)
	{
		Log.w(TAG, "onActivityResult: rq=" + requestCode + " result=" + resultCode);
		BarcodeScannerIntentResult scanResult = BarcodeScannerIntegrator.parseActivityResult(requestCode, resultCode, data);
		if (scanResult != null)
		{
			editText.setText(scanResult.getContents());
			startSearch();
		}
		super.onActivityResult(requestCode, resultCode, data);
	}

	/* (non-Javadoc)
	 * @see android.content.ServiceConnection#onServiceConnected(android.content.ComponentName, android.os.IBinder)
	 */
	@Override
	public void onServiceConnected(ComponentName name, IBinder binder)
	{
		super.onServiceConnected(name, binder);
		if (nodeId != 0)
			startSearch();
	}

	/* (non-Javadoc)
	 * @see android.content.ServiceConnection#onServiceDisconnected(android.content.ComponentName)
	 */
	@Override
	public void onServiceDisconnected(ComponentName name)
	{
		super.onServiceDisconnected(name);
	}

	/* (non-Javadoc)
	 * @see android.app.Activity#onCreateContextMenu(android.view.ContextMenu, android.view.View, android.view.ContextMenu.ContextMenuInfo)
	 */
	@Override
	public void onCreateContextMenu(ContextMenu menu, View v, ContextMenuInfo menuInfo)
	{
		android.view.MenuInflater inflater = getMenuInflater();
		inflater.inflate(R.menu.connection_point_actions, menu);
	}

	/* (non-Javadoc)
	 * @see android.app.Activity#onContextItemSelected(android.view.MenuItem)
	 */
	@Override
	public boolean onContextItemSelected(MenuItem item)
	{
		// get selected item
		AdapterView.AdapterContextMenuInfo info = (AdapterContextMenuInfo)item.getMenuInfo();
		final int position = info.position;

		if (item.getItemId() == R.id.connection_point_delete_one)
		{
			new AlertDialog.Builder(this).setIcon(android.R.drawable.ic_dialog_alert).setTitle(R.string.confirm_tool_execution).setMessage(r.getString(R.string.connection_point_confirm_delete_one)).setCancelable(true).setPositiveButton(
					R.string.yes,
					new DialogInterface.OnClickListener()
					{
						@Override
						public void onClick(DialogInterface dialog, int which)
						{
							cpList.remove(position);
							refreshList();
						}
					}).setNegativeButton(R.string.no, null).show();
		}
		else if (item.getItemId() == R.id.connection_point_delete_all)
		{
			new AlertDialog.Builder(this).setIcon(android.R.drawable.ic_dialog_alert).setTitle(R.string.confirm_tool_execution).setMessage(r.getString(R.string.connection_point_confirm_delete_all)).setCancelable(true).setPositiveButton(
					R.string.yes,
					new DialogInterface.OnClickListener()
					{
						@Override
						public void onClick(DialogInterface dialog, int which)
						{
							cpList.clear();
							refreshList();
						}
					}).setNegativeButton(R.string.no, null).show();
		}
		else
		{
			return super.onContextItemSelected(item);
		}
		return true;
	}

	/* (non-Javadoc)
	 * @see android.app.Activity#onDestroy()
	 */
	@Override
	protected void onDestroy()
	{
		super.onDestroy();
	}

	/* (non-Javadoc)
	 * @see android.app.Activity#onStop()
	 */
	@Override
	protected void onStop()
	{
		super.onStop();
		storePreferences(this, CPLIST_KEY, cpList, maxConnectionPoints);
	}

	private void refreshList()
	{
		adapter.setConnectionPoint(cpList);
		adapter.notifyDataSetChanged();
	}

	/**
	 * Store connection points result list
	 */
	private void storePreferences(Context context, String key, List<String> values, int maxValues)
	{
		SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(context);
		SharedPreferences.Editor editor = prefs.edit();
		JSONArray a = new JSONArray();
		for (int i = 0; i < Math.min(values.size(), maxValues); i++)
			a.put(values.get(i));
		if (!values.isEmpty())
			editor.putString(key, a.toString());
		else
			editor.putString(key, null);
		editor.commit();
	}

	/**
	 * Retrieve connection points result list
	 */
	private ArrayList<String> retrievePreferences(Context context, String key, int maxValues)
	{
		SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(context);
		String json = prefs.getString(key, null);
		ArrayList<String> values = new ArrayList<String>();
		if (json != null)
			try
			{
				JSONArray a = new JSONArray(json);
				for (int i = 0; i < Math.min(a.length(), maxValues); i++)
					values.add(a.optString(i));
			}
			catch (JSONException e)
			{
				Log.e(TAG, "JSONException in retrievePreferences...", e);
				e.printStackTrace();
			}
		return values;
	}

	/**
	 * Start MAC address search
	 */
	private void startSearch()
	{
		new SearchConnectionPointTask(nodeId, editText.getText().toString()).execute();
	}

	/**
	 * Internal task for loading info for MAC address search
	 */
	private class SearchConnectionPointTask extends AsyncTask<Object, Void, String>
	{
		private final int nodeId;
		private final String macAddress;

		SearchConnectionPointTask(int nodeId, String macAddress)
		{
			this.nodeId = nodeId;
			this.macAddress = macAddress;
		}

		/* (non-Javadoc)
		 * @see android.os.AsyncTask#onPreExecute()
		 */
		@Override
		protected void onPreExecute()
		{
			if (dialog != null)
			{
				dialog.setMessage(getString(R.string.progress_gathering_data));
				dialog.setIndeterminate(true);
				dialog.setCancelable(false);
				dialog.show();
			}
		}

		/* (non-Javadoc)
		 * @see android.os.AsyncTask#doInBackground(Params[])
		 */
		@Override
		protected String doInBackground(Object... params)
		{
			String string = nodeId != 0 ? r.getString(R.string.connection_point_notfound) : r.getString(R.string.connection_point_macaddress_notfound, macAddress);
			if (service != null)
			{
				NXCSession session = service.getSession();
				if (session != null)
				{
					ConnectionPoint cp = null;
					Node host = null;
					Node bridge = null;
					Interface iface = null;
					try
					{
						cp = nodeId != 0 ? session.findConnectionPoint(nodeId) : session.findConnectionPoint(MacAddress.parseMacAddress(macAddress));
						if (cp != null)
						{
							session.syncMissingObjects(new long[] { cp.getLocalNodeId(), cp.getNodeId(), cp.getInterfaceId() }, false, NXCSession.OBJECT_SYNC_WAIT);
							host = (Node)session.findObjectById(cp.getLocalNodeId());
							bridge = (Node)session.findObjectById(cp.getNodeId());
							iface = (Interface)session.findObjectById(cp.getInterfaceId());
						}
					}
					catch (MacAddressFormatException e)
					{
						Log.e(TAG, "MacAddressFormatException while executing searchMacAddress", e);
						string = r.getString(R.string.connection_point_invalid, macAddress);
					}
					catch (NXCException e)
					{
						Log.e(TAG, "NXCException while executing syncMissingObjects", e);
					}
					catch (IOException e)
					{
						Log.e(TAG, "IOException while executing syncMissingObjects", e);
					}

					if ((host != null) && (bridge != null) && (iface != null))
						string = r.getString(R.string.connection_point_info, host != null ? " " + host.getObjectName() : "", cp.getLocalMacAddress().toString(), bridge.getObjectName(), iface.getObjectName());
					else
						string = r.getString(R.string.connection_point_info_notfound);
				}
			}
			return string;
		}

		/* (non-Javadoc)
		 * @see android.os.AsyncTask#onPostExecute(java.lang.Object)
		 */
		@Override
		protected void onPostExecute(String result)
		{
			if (dialog != null)
				dialog.cancel();
			if (result != null)
			{
				cpList.add(0, result);
				if (cpList.size() > maxConnectionPoints)
					cpList.remove(maxConnectionPoints);
				refreshList();
			}
		}
	}
}
