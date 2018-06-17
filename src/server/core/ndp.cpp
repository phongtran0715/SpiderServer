/* 
** NetXMS - Network Management System
** Copyright (C) 2003-2017 Victor Kirhenshtein
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** File: ndp.cpp
**
**/

#include "nxcore.h"

/**
 * Read remote slot and port from s5EnMsTopNmmEosTable
 */
static WORD ReadRemoteSlotAndPort(Node *node, const SNMP_ObjectId& oid, SNMP_Transport *transport)
{
	// Read data from appropriate entry in s5EnMsTopNmmEosTable
	UINT32 eosEntryOID[64];
	memcpy(eosEntryOID, oid.value(), oid.length() * sizeof(UINT32));
	eosEntryOID[11] = 3;
	eosEntryOID[12] = 1;
	eosEntryOID[13] = 1;

   SNMP_PDU *pRqPDU = new SNMP_PDU(SNMP_GET_REQUEST, SnmpNewRequestId(), transport->getSnmpVersion());
	pRqPDU->bindVariable(new SNMP_Variable(eosEntryOID, oid.length()));

	WORD result = 0;
	SNMP_PDU *pRespPDU = NULL;
   UINT32 rcc = transport->doRequest(pRqPDU, &pRespPDU, SnmpGetDefaultTimeout(), 3);
	delete pRqPDU;
	if ((rcc == SNMP_ERR_SUCCESS) && (pRespPDU->getNumVariables() > 0) && (pRespPDU->getVariable(0)->getType() == ASN_OCTET_STRING))
   {
		BYTE eosEntry[128];
		pRespPDU->getVariable(0)->getRawValue(eosEntry, 128);
		result = (((WORD)eosEntry[7]) << 8) | (WORD)eosEntry[8];	// Slot in byte 7 and port in byte 8
	}
	delete pRespPDU;
	return result;
}

/**
 * Topology table walker's callback for NDP topology table
 */
static UINT32 NDPTopoHandler(SNMP_Variable *var, SNMP_Transport *transport, void *arg)
{
	Node *node = (Node *)((LinkLayerNeighbors *)arg)->getData();
	const SNMP_ObjectId& oid = var->getName();

	// Entries indexed by slot, port, IP address, and segment ID
	UINT32 slot = oid.getElement(14);
	UINT32 port = oid.getElement(15);

	// Table always contains record with slot=0 and port=0 which
	// represents local chassis. We should ignore this record.
	if ((slot == 0) && (port == 0))
		return SNMP_ERR_SUCCESS;

	UINT32 remoteIp;
	var->getRawValue((BYTE *)&remoteIp, sizeof(UINT32));
	remoteIp = ntohl(remoteIp);
	TCHAR ipAddrText[32];
	DbgPrintf(6, _T("NDP(%s [%d]): found peer at %d.%d IP address %s"), node->getName(), node->getId(), slot, port, IpToStr(remoteIp, ipAddrText));
	Node *remoteNode = FindNodeByIP(node->getZoneUIN(), remoteIp);
	if (remoteNode == NULL)
	{
		DbgPrintf(6, _T("NDP(%s [%d]): node object for IP %s not found"), node->getName(), node->getId(), ipAddrText);
		return SNMP_ERR_SUCCESS;
	}

	Interface *ifLocal = node->findInterfaceBySlotAndPort(slot, port);
	DbgPrintf(6, _T("NDP(%s [%d]): remote node is %s [%d], local interface object \"%s\""), node->getName(), node->getId(),
	          remoteNode->getName(), remoteNode->getId(), (ifLocal != NULL) ? ifLocal->getName() : _T("(null)"));
	if (ifLocal != NULL)
	{
		WORD rport = ReadRemoteSlotAndPort(node, oid, transport);
		DbgPrintf(6, _T("NDP(%s [%d]): remote slot/port is %04X"), node->getName(), node->getId(), rport);
		if (rport != 0)
		{
			Interface *ifRemote = remoteNode->findInterfaceBySlotAndPort(rport >> 8, rport & 0xFF);
			if (ifRemote != NULL)
			{
				LL_NEIGHBOR_INFO info;

				info.objectId = remoteNode->getId();
				info.ifRemote = ifRemote->getIfIndex();
				info.ifLocal = ifLocal->getIfIndex();
				info.isPtToPt = true;
				info.protocol = LL_PROTO_NDP;
            info.isCached = false;
				((LinkLayerNeighbors *)arg)->addConnection(&info);
			}
		}
	}

	return SNMP_ERR_SUCCESS;
}

/**
 * Add NDP-discovered neighbors
 */
void AddNDPNeighbors(Node *node, LinkLayerNeighbors *nbs)
{
	if (!(node->getFlags() & NF_IS_NDP))
		return;

	DbgPrintf(5, _T("NDP: collecting topology information for node %s [%d]"), node->getName(), node->getId());
	nbs->setData(node);
	node->callSnmpEnumerate(_T(".1.3.6.1.4.1.45.1.6.13.2.1.1.3"), NDPTopoHandler, nbs);
	DbgPrintf(5, _T("NDP: finished collecting topology information for node %s [%d]"), node->getName(), node->getId());
}
