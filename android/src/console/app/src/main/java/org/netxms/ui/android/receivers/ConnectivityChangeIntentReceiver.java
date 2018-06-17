package org.netxms.ui.android.receivers;

import org.netxms.ui.android.service.ClientConnectorService;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.preference.PreferenceManager;

/**
 * Intent receiver for connectivity change broadcast message
 * 
 * @author Marco Incalcaterra (marco.incalcaterra@thinksoft.it)
 * 
 */

public class ConnectivityChangeIntentReceiver extends BroadcastReceiver
{
	@SuppressWarnings("deprecation")
	@Override
	public void onReceive(Context context, Intent intent)
	{
		SharedPreferences sp = PreferenceManager.getDefaultSharedPreferences(context);
		if (!sp.getBoolean(ClientConnectorService.INTENTIONAL_EXIT_KEY, false))
			if (intent.getExtras() != null)
			{
				if (!sp.getBoolean("global.scheduler.enable", false)) // Try to connect only when the scheduler is disabled
				{
					NetworkInfo ni = (NetworkInfo)intent.getExtras().get(ConnectivityManager.EXTRA_NETWORK_INFO);
					if (ni != null && ni.getState() == NetworkInfo.State.CONNECTED)
					{
						Intent i = new Intent(context, ClientConnectorService.class);
						i.setAction(ClientConnectorService.ACTION_FORCE_CONNECT);
						context.startService(i);
					}
				}
				if (intent.getExtras().getBoolean(ConnectivityManager.EXTRA_NO_CONNECTIVITY, Boolean.FALSE))
				{
					Intent i = new Intent(context, ClientConnectorService.class);
					i.setAction(ClientConnectorService.ACTION_FORCE_DISCONNECT);
					context.startService(i);
				}
			}
	}
}
