package org.netxms.agent.android.main.activities;

import org.netxms.agent.android.NXApplication;
import org.netxms.agent.android.R;
import org.netxms.agent.android.service.AgentConnectorService;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;

/**
 * Abstract base class for all activities in the client. Implements
 * common functionality for connecting to service and handling common items
 * in options menu.
 */
public abstract class AbstractClientActivity extends Activity implements ServiceConnection
{
	private static final String TAG = "nxagent/AbstractClientActivity";

	protected AgentConnectorService service;

	/* (non-Javadoc)
	 * @see android.app.Activity#onCreate(android.os.Bundle)
	 */
	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		onCreateStep2(savedInstanceState);
		startService(new Intent(this, AgentConnectorService.class));
		bindService(new Intent(this, AgentConnectorService.class), this, 0);

		// the following is required if target API version is 14:
		//getActionBar().setHomeButtonEnabled(true);
	}

	/**
	 * Called by AbstractClientActivity.onCreate before service binding
	 * to allow inherited classes to do initialization before onServiceConnected call
	 * 
	 * @param savedInstanceState
	 */
	protected abstract void onCreateStep2(Bundle savedInstanceState);

	/* (non-Javadoc)
	 * @see android.app.Activity#onDestroy()
	 */
	@Override
	protected void onDestroy()
	{
		unbindService(this);
		super.onDestroy();
	}

	/* (non-Javadoc)
	 * @see android.app.Activity#onResume()
	 */
	@Override
	protected void onResume()
	{
		super.onResume();
		NXApplication.activityResumed();
		if (service != null)
			service.reconnect(false);
	}

	/* (non-Javadoc)
	 * @see android.app.Activity#onPause()
	 */
	@Override
	protected void onPause()
	{
		super.onStop();
		NXApplication.activityPaused();
	}

	/* (non-Javadoc)
	 * @see android.app.Activity#onCreateOptionsMenu(android.view.Menu)
	 */
	@Override
	public boolean onCreateOptionsMenu(Menu menu)
	{
		MenuInflater inflater = getMenuInflater();
		inflater.inflate(R.menu.main_menu, menu);
		return true;
	}

	/* (non-Javadoc)
	 * @see android.app.Activity#onOptionsItemSelected(android.view.MenuItem)
	 */
	@Override
	public boolean onOptionsItemSelected(MenuItem item)
	{
		Log.d(TAG, "onOptionsItemSelected: id=" + android.R.id.home);

		if (item.getItemId() == R.id.settings)
		{
			startActivity(new Intent(this, Preferences.class));
			return true;
		}
		return super.onOptionsItemSelected(item);
	}

	/* (non-Javadoc)
	 * @see android.content.ServiceConnection#onServiceConnected(android.content.ComponentName, android.os.IBinder)
	 */
	@Override
	public void onServiceConnected(ComponentName name, IBinder binder)
	{
		service = ((AgentConnectorService.AgentConnectorBinder)binder).getService();
		if (service != null)
			service.reconnect(false);
	}

	/* (non-Javadoc)
	 * @see android.content.ServiceConnection#onServiceDisconnected(android.content.ComponentName)
	 */
	@Override
	public void onServiceDisconnected(ComponentName name)
	{
	}
}
