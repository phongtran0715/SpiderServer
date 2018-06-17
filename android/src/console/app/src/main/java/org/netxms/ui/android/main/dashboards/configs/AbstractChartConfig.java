/**
 * NetXMS - open source network management system
 * Copyright (C) 2003-2011 Victor Kirhenshtein
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
package org.netxms.ui.android.main.dashboards.configs;

import java.util.Map;
import java.util.Set;
import org.netxms.client.datacollection.GraphSettings;
import org.netxms.ui.android.main.activities.helpers.ChartDciConfig;
import org.simpleframework.xml.Element;
import org.simpleframework.xml.ElementArray;

/**
 * Base class for all chart widget configs
 */
public abstract class AbstractChartConfig extends DashboardElementConfig
{
	@ElementArray(required = true)
	private ChartDciConfig[] dciList = new ChartDciConfig[0];
	
	@Element(required = false)
	private String title = "";
	
	@Element(required = false)
	private int legendPosition = GraphSettings.POSITION_RIGHT;
	
	@Element(required = false)
	private boolean showLegend = true;
	
	@Element(required = false)
	private boolean showTitle = true;

	@Element(required = false)
	private int refreshRate = 30;

	/* (non-Javadoc)
	 * @see org.netxms.ui.eclipse.dashboard.widgets.internal.DashboardElementConfig#getObjects()
	 */
	@Override
	public Set<Long> getObjects()
	{
		Set<Long> objects = super.getObjects();
		for(ChartDciConfig dci : dciList)
			objects.add(dci.nodeId);
		return objects;
	}

	/* (non-Javadoc)
	 * @see org.netxms.ui.eclipse.dashboard.widgets.internal.DashboardElementConfig#getDataCollectionItems()
	 */
	@Override
	public Map<Long, Long> getDataCollectionItems()
	{
		Map<Long, Long> items =  super.getDataCollectionItems();
		for(ChartDciConfig dci : dciList)
			items.put(dci.dciId, dci.nodeId);
		return items;
	}

	/**
	 * @return the title
	 */
	public String getTitle()
	{
		return title;
	}

	/**
	 * @param title the title to set
	 */
	public void setTitle(String title)
	{
		this.title = title;
	}

	/**
	 * @return the dciList
	 */
	public ChartDciConfig[] getDciList()
	{
		return dciList;
	}

	/**
	 * @param dciList the dciList to set
	 */
	public void setDciList(ChartDciConfig[] dciList)
	{
		this.dciList = dciList;
	}

	/**
	 * @return the legendPosition
	 */
	public int getLegendPosition()
	{
		return legendPosition;
	}

	/**
	 * @param legendPosition the legendPosition to set
	 */
	public void setLegendPosition(int legendPosition)
	{
		this.legendPosition = legendPosition;
	}

	/**
	 * @return the showLegend
	 */
	public boolean isShowLegend()
	{
		return showLegend;
	}

	/**
	 * @param showLegend the showLegend to set
	 */
	public void setShowLegend(boolean showLegend)
	{
		this.showLegend = showLegend;
	}

	/**
	 * @return the refreshRate
	 */
	public int getRefreshRate()
	{
		return refreshRate;
	}

	/**
	 * @param refreshRate the refreshRate to set
	 */
	public void setRefreshRate(int refreshRate)
	{
		this.refreshRate = refreshRate;
	}
	
	/**
	 * @return the showTitle
	 */
	public boolean isShowTitle()
	{
		return showTitle;
	}

	/**
	 * @param showTitle the showTitle to set
	 */
	public void setShowTitle(boolean showTitle)
	{
		this.showTitle = showTitle;
	}
}
