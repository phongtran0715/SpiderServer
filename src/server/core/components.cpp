/* 
** NetXMS - Network Management System
** Copyright (C) 2003-2016 Victor Kirhenshtein
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
** File: components.cpp
**
**/

#include "nxcore.h"

/**
 * Component tree constructor
 */
ComponentTree::ComponentTree(Component *root)
{
	m_root = root;
}

/**
 * Componnet tree destructor
 */
ComponentTree::~ComponentTree()
{
	delete m_root;
}

/**
 * Fill NXCP message with tree data
 */
void ComponentTree::fillMessage(NXCPMessage *msg, UINT32 baseId)
{
	if (m_root != NULL)
		m_root->fillMessage(msg, baseId);
}

/**
 * Constructor
 */
Component::Component(UINT32 index, const TCHAR *name) : m_childs(0, 16, true)
{
	m_index = index;
	m_class = 2; // unknown
	m_ifIndex = 0;
	m_name = _tcsdup(name);
	m_description = NULL;
	m_model = NULL;
	m_serial = NULL;
	m_vendor = NULL;
	m_firmware = NULL;
	m_parentIndex = 0;
}

/**
 * Destructor
 */
Component::~Component()
{
	safe_free(m_name);
	safe_free(m_description);
	safe_free(m_model);
	safe_free(m_serial);
	safe_free(m_vendor);
	safe_free(m_firmware);
}

/**
 * Update component from SNMP
 */
UINT32 Component::updateFromSnmp(SNMP_Transport *snmp)
{
	UINT32 oid[16] = { 1, 3, 6, 1, 2, 1, 47, 1, 1, 1, 1, 0, 0 };
	UINT32 rc;
	TCHAR buffer[256];

	oid[12] = m_index;

	oid[11] = 5;	// entPhysicalClass
	if ((rc = SnmpGet(snmp->getSnmpVersion(), snmp, NULL, oid, 13, &m_class, sizeof(UINT32), 0)) != SNMP_ERR_SUCCESS)
		return rc;

	oid[11] = 4;	// entPhysicalContainedIn
	if ((rc = SnmpGet(snmp->getSnmpVersion(), snmp, NULL, oid, 13, &m_parentIndex, sizeof(UINT32), 0)) != SNMP_ERR_SUCCESS)
		return rc;

	oid[11] = 7;	// entPhysicalDescr
	if (SnmpGet(snmp->getSnmpVersion(), snmp, NULL, oid, 13, buffer, sizeof(buffer), 0) == SNMP_ERR_SUCCESS)
	   m_description = _tcsdup(buffer);
	else
      m_description = _tcsdup(_T(""));

	oid[11] = 13;	// entPhysicalModelName
	if (SnmpGet(snmp->getSnmpVersion(), snmp, NULL, oid, 13, buffer, sizeof(buffer), 0) == SNMP_ERR_SUCCESS)
	   m_model = _tcsdup(buffer);
	else
      m_model = _tcsdup(_T(""));

	oid[11] = 11;	// entPhysicalSerialNum
	if (SnmpGet(snmp->getSnmpVersion(), snmp, NULL, oid, 13, buffer, sizeof(buffer), 0) == SNMP_ERR_SUCCESS)
	   m_serial = _tcsdup(buffer);
	else
      m_serial = _tcsdup(_T(""));

	oid[11] = 12;	// entPhysicalMfgName
	if (SnmpGet(snmp->getSnmpVersion(), snmp, NULL, oid, 13, buffer, sizeof(buffer), 0) == SNMP_ERR_SUCCESS)
	   m_vendor = _tcsdup(buffer);
	else
      m_vendor = _tcsdup(_T(""));

	oid[11] = 9;	// entPhysicalFirmwareRev
	if (SnmpGet(snmp->getSnmpVersion(), snmp, NULL, oid, 13, buffer, sizeof(buffer), 0) == SNMP_ERR_SUCCESS)
	   m_firmware = _tcsdup(buffer);
	else
	   m_firmware = _tcsdup(_T(""));

	return SNMP_ERR_SUCCESS;
}

/**
 * Build element tree
 */
void Component::buildTree(ObjectArray<Component> *elements)
{
	for(int i = 0; i < elements->size(); i++)
	{
		Component *e = elements->get(i);
		if (e->m_parentIndex == m_index)
		{
			m_childs.add(e);
			e->buildTree(elements);
		}
	}
}

/**
 * Print element tree to given console
 */
void Component::print(CONSOLE_CTX console, int level)
{
	ConsolePrintf(console, _T("%*s\x1b[1m%d\x1b[0m \x1b[32;1m%-32s\x1b[0m %s\n"), level * 4, _T(""), (int)m_index, m_name, m_description);
	for(int i = 0; i < m_childs.size(); i++)
		m_childs.get(i)->print(console, level + 1);
}

/**
 * Fill NXCP message
 */
UINT32 Component::fillMessage(NXCPMessage *msg, UINT32 baseId)
{
	msg->setField(baseId, m_index);
	msg->setField(baseId + 1, m_parentIndex);
	msg->setField(baseId + 2, m_class);
	msg->setField(baseId + 3, m_ifIndex);
	msg->setField(baseId + 4, m_name);
	msg->setField(baseId + 5, m_description);
	msg->setField(baseId + 6, m_model);
	msg->setField(baseId + 7, m_serial);
	msg->setField(baseId + 8, m_vendor);
	msg->setField(baseId + 9, m_firmware);
	msg->setField(baseId + 10, (UINT32)m_childs.size());

	UINT32 varId = baseId + 11;
	for(int i = 0; i < m_childs.size(); i++)
		varId = m_childs.get(i)->fillMessage(msg, varId);

	return varId;
}

/*
 * Get child components as NXSL Array
 */
NXSL_Array *Component::getChildrenForNXSL()
{
   NXSL_Array *components = new NXSL_Array();

   for(int i = 0; i < m_childs.size(); i++)
   {
      components->set(i, new NXSL_Value(new NXSL_Object(&g_nxslComponentClass, m_childs.get(i))));
   }

   return components;
}

/**
 * Physical entity tree walk callback
 */
static UINT32 EntityWalker(SNMP_Variable *var, SNMP_Transport *transport, void *arg)
{
	TCHAR buffer[256];
	Component *element = new Component(var->getName().getElement(12), var->getValueAsString(buffer, 256));
	UINT32 rc = element->updateFromSnmp(transport);
	if (rc != SNMP_ERR_SUCCESS)
	{
		delete element;
		return rc;
	}	
	((ObjectArray<Component> *)arg)->add(element);
	return SNMP_ERR_SUCCESS;
}

/**
 * Build components tree for given node
 */
ComponentTree *BuildComponentTree(Node *node, SNMP_Transport *snmp)
{
	DbgPrintf(5, _T("Building component tree for node %s [%d]"), node->getName(), (int)node->getId());
	ObjectArray<Component> elements(16, 16);
	ComponentTree *tree = NULL;
	if (SnmpWalk(snmp, _T(".1.3.6.1.2.1.47.1.1.1.1.7"), EntityWalker, &elements) == SNMP_ERR_SUCCESS)
	{
		DbgPrintf(6, _T("BuildComponentTree(%s [%d]): %d elements found"), node->getName(), (int)node->getId(), elements.size());

		Component *root = NULL;
		for(int i = 0; i < elements.size(); i++)
			if (elements.get(i)->getParentIndex() == 0)
			{
				root = elements.get(i);
				break;
			}

		if (root != NULL)
		{
			root->buildTree(&elements);
			tree = new ComponentTree(root);
		}
		else
		{
			DbgPrintf(6, _T("BuildComponentTree(%s [%d]): root element not found"), node->getName(), (int)node->getId());
			elements.setOwner(true);	// cause element destruction on exit
		}
	}
	else
	{
		DbgPrintf(6, _T("BuildComponentTree(%s [%d]): SNMP WALK failed"), node->getName(), (int)node->getId());
		elements.setOwner(true);	// cause element destruction on exit
	}
	DbgPrintf(5, _T("BuildComponentTree(%s [%d]): %p"), node->getName(), (int)node->getId(), tree);
	return tree;
}
