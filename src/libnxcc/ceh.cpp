/* 
** NetXMS - Network Management System
** Copyright (C) 2003-2015 Victor Kirhenshtein
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** File: ceh.cpp
**
*/

#include "libnxcc.h"

/**
 * Constructor
 */
ClusterEventHandler::ClusterEventHandler()
{
}

/**
 * Destructor
 */
ClusterEventHandler::~ClusterEventHandler()
{
}

/**
 * Node join handler
 */
void ClusterEventHandler::onNodeJoin(UINT32 nodeId)
{
}

/**
 * Node up handler
 */
void ClusterEventHandler::onNodeUp(UINT32 nodeId)
{
}

/**
 * Node disconnect handler
 */
void ClusterEventHandler::onNodeDisconnect(UINT32 nodeId)
{
}

/**
 * Shutdown handler
 */
void ClusterEventHandler::onShutdown()
{
}

/**
 * Split brain condition handler
 */
void ClusterEventHandler::onSplitBrain()
{
}

/**
 * Incoming message handler
 * Possible return values:
 *    CLUSTER_MSG_PROCESSED - message is processed and should be deleted
 *    CLUSTER_MSG_QUEUED - message is queued for deferred processing and should not be deleted by caller
 *    CLUSTER_MSG_IGNORED - message is ignored and should be placed into wait queue
 */
ClusterMessageProcessingResult ClusterEventHandler::onMessage(NXCPMessage *msg, UINT32 sourceNodeId)
{
   return CLUSTER_MSG_IGNORED;
}

/**
 * Incoming notification handler
 */
void ClusterEventHandler::onNotification(int code, UINT32 sourceNodeId)
{
}
