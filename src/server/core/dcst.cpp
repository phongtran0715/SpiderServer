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
** File: dcst.cpp
**
**/

#include "nxcore.h"

/**
 * Modify DCI summary table. Will create new table if id is 0.
 *
 * @return RCC ready to be sent to client
 */
UINT32 ModifySummaryTable(NXCPMessage *msg, LONG *newId)
{
   LONG id = msg->getFieldAsUInt32(VID_SUMMARY_TABLE_ID);
   if (id == 0)
   {
      id = CreateUniqueId(IDG_DCI_SUMMARY_TABLE);
   }
   *newId = id;

   DB_HANDLE hdb = DBConnectionPoolAcquireConnection();

   bool isNew = !IsDatabaseRecordExist(hdb, _T("dci_summary_tables"), _T("id"), (UINT32)id);
   DB_STATEMENT hStmt;
   if (isNew)
   {
      hStmt = DBPrepare(hdb, _T("INSERT INTO dci_summary_tables (menu_path,title,node_filter,flags,columns,table_dci_name,id,guid) VALUES (?,?,?,?,?,?,?,?)"));
   }
   else
   {
      hStmt = DBPrepare(hdb, _T("UPDATE dci_summary_tables SET menu_path=?,title=?,node_filter=?,flags=?,columns=?,table_dci_name=? WHERE id=?"));
   }

   UINT32 rcc;
   if (hStmt != NULL)
   {
      DBBind(hStmt, 1, DB_SQLTYPE_VARCHAR, msg->getFieldAsString(VID_MENU_PATH), DB_BIND_DYNAMIC);
      DBBind(hStmt, 2, DB_SQLTYPE_VARCHAR, msg->getFieldAsString(VID_TITLE), DB_BIND_DYNAMIC);
      DBBind(hStmt, 3, DB_SQLTYPE_TEXT, msg->getFieldAsString(VID_FILTER), DB_BIND_DYNAMIC);
      DBBind(hStmt, 4, DB_SQLTYPE_INTEGER, msg->getFieldAsUInt32(VID_FLAGS));
      DBBind(hStmt, 5, DB_SQLTYPE_TEXT, msg->getFieldAsString(VID_COLUMNS), DB_BIND_DYNAMIC);
      DBBind(hStmt, 6, DB_SQLTYPE_VARCHAR, msg->getFieldAsString(VID_DCI_NAME), DB_BIND_DYNAMIC);
      DBBind(hStmt, 7, DB_SQLTYPE_INTEGER, id);

      if (isNew)
      {
         DBBind(hStmt, 8, DB_SQLTYPE_VARCHAR, uuid::generate());
      }

      rcc = DBExecute(hStmt) ? RCC_SUCCESS : RCC_DB_FAILURE;
      if (rcc == RCC_SUCCESS)
         NotifyClientSessions(NX_NOTIFY_DCISUMTBL_CHANGED, (UINT32)id);

      DBFreeStatement(hStmt);
   }
   else
   {
      rcc = RCC_DB_FAILURE;
   }

   DBConnectionPoolReleaseConnection(hdb);
   return rcc;
}

/**
 * Delete DCI summary table
 */
UINT32 DeleteSummaryTable(LONG tableId)
{
   DB_HANDLE hdb = DBConnectionPoolAcquireConnection();
   UINT32 rcc = RCC_DB_FAILURE;
   DB_STATEMENT hStmt = DBPrepare(hdb, _T("DELETE FROM dci_summary_tables WHERE id=?"));
   if (hStmt != NULL)
   {
      DBBind(hStmt, 1, DB_SQLTYPE_INTEGER, tableId);
      if (DBExecute(hStmt))
      {
         rcc = RCC_SUCCESS;
         NotifyClientSessions(NX_NOTIFY_DCISUMTBL_DELETED, (UINT32)tableId);
      }
      DBFreeStatement(hStmt);
   }
   DBConnectionPoolReleaseConnection(hdb);
   return rcc;
}

/**
 * Create column definition from NXCP message
 */
SummaryTableColumn::SummaryTableColumn(NXCPMessage *msg, UINT32 baseId)
{
   msg->getFieldAsString(baseId, m_name, MAX_DB_STRING);
   msg->getFieldAsString(baseId + 1, m_dciName, MAX_PARAM_NAME);
   m_flags = msg->getFieldAsUInt32(baseId + 2);
   if (msg->isFieldExist(baseId + 3))
      msg->getFieldAsString(baseId + 3, m_separator, 16);
   else
      _tcscpy(m_separator, _T(";"));
}

/**
 * Create export record for column
 */
void SummaryTableColumn::createExportRecord(String &xml, int id)
{
   xml.append(_T("\t\t\t\t<column id=\""));
   xml.append(id);
   xml.append(_T("\">\n\t\t\t\t\t<name>"));
   xml.appendPreallocated(EscapeStringForXML(m_name, -1));
   xml.append(_T("</name>\n\t\t\t\t\t<dci>"));
   xml.appendPreallocated(EscapeStringForXML(m_dciName, -1));
   xml.append(_T("</dci>\n\t\t\t\t\t<flags>"));
   xml.append(m_flags);
   xml.append(_T("</flags>\n\t\t\t\t\t<separator>\n"));
   xml.append(m_separator);
   xml.append(_T("</separator>\n\t\t\t\t</column>\n"));
}

/**
 * Create column definition from configuration string
 */
SummaryTableColumn::SummaryTableColumn(TCHAR *configStr)
{
   TCHAR *ptr = _tcsstr(configStr, _T("^#^"));
   if (ptr != NULL)
   {
      *ptr = 0;
      ptr += 3;
      TCHAR *opt = _tcsstr(ptr, _T("^#^"));
      if (opt != NULL)
      {
         *opt = 0;
         opt += 3;
         TCHAR *sep = _tcsstr(opt, _T("^#^"));
         if (sep != NULL)
         {
            *sep = 0;
            sep += 3;
            nx_strncpy(m_separator, sep, 16);
         }
         else
         {
            _tcscpy(m_separator, _T(";"));
         }
         m_flags = _tcstoul(opt, NULL, 10);
      }
      else
      {
         m_flags = 0;
      }
      nx_strncpy(m_dciName, ptr, MAX_PARAM_NAME);
   }
   else
   {
      nx_strncpy(m_dciName, configStr, MAX_PARAM_NAME);
      m_flags = 0;
   }
   nx_strncpy(m_name, configStr, MAX_DB_STRING);
}

/**
 * Create ad-hoc summary table definition from NXCP message
 */
SummaryTable::SummaryTable(NXCPMessage *msg)
{
   m_id = 0;
   m_guid = uuid::generate();
   m_title[0] = 0;
   m_menuPath[0] = 0;
   m_flags = msg->getFieldAsUInt32(VID_FLAGS);
   m_filterSource = NULL;
   m_filter = NULL;
   m_aggregationFunction = (AggregationFunction)msg->getFieldAsInt16(VID_FUNCTION);
   m_periodStart = msg->getFieldAsTime(VID_TIME_FROM);
   m_periodEnd = msg->getFieldAsTime(VID_TIME_TO);

   int count = msg->getFieldAsInt32(VID_NUM_COLUMNS);
   m_columns = new ObjectArray<SummaryTableColumn>(count, 16, true);

   UINT32 id = VID_COLUMN_INFO_BASE;
   for(int i = 0; i < count; i++)
   {
      m_columns->add(new SummaryTableColumn(msg, id));
      id += 10;
   }
   msg->getFieldAsString(VID_DCI_NAME, m_tableDciName, MAX_PARAM_NAME);
}

/**
 * Create summary table definition from DB data
 */
SummaryTable::SummaryTable(INT32 id, DB_RESULT hResult)
{
   m_id = id;

   DBGetField(hResult, 0, 0, m_title, MAX_DB_STRING);
   m_flags = DBGetFieldULong(hResult, 0, 1);
   m_guid = DBGetFieldGUID(hResult, 0, 2);
   DBGetField(hResult, 0, 3, m_menuPath, MAX_DB_STRING);

   m_aggregationFunction = DCI_AGG_LAST;
   m_periodStart = 0;
   m_periodEnd = 0;

   // Filter script
   m_filterSource = DBGetField(hResult, 0, 4, NULL, 0);
   if (m_filterSource != NULL)
   {
      StrStrip(m_filterSource);
      if (*m_filterSource != 0)
      {
         TCHAR errorText[1024];
         m_filter = NXSLCompileAndCreateVM(m_filterSource, errorText, 1024, new NXSL_ServerEnv);
         if (m_filter == NULL)
         {
            nxlog_debug(4, _T("Error compiling filter script for DCI summary table: %s"), errorText);
         }
      }
      else
      {
         m_filter = NULL;
      }
   }
   else
   {
      m_filter = NULL;
   }

   // Columns
   m_columns = new ObjectArray<SummaryTableColumn>(16, 16, true);
   TCHAR *config = DBGetField(hResult, 0, 5, NULL, 0);
   if ((config != NULL) && (*config != 0))
   {
      TCHAR *curr = config;
      while(curr != NULL)
      {
         TCHAR *next = _tcsstr(curr, _T("^~^"));
         if (next != NULL)
         {
            *next = 0;
            next += 3;
         }
         m_columns->add(new SummaryTableColumn(curr));
         curr = next;
      }
      free(config);
   }
   DBGetField(hResult, 0, 6, m_tableDciName, MAX_PARAM_NAME);
}

/**
 * Load summary table object from database
 */
SummaryTable *SummaryTable::loadFromDB(INT32 id, UINT32 *rcc)
{
   nxlog_debug(4, _T("Loading configuration for DCI summary table %d"), id);
   SummaryTable *table = NULL;
   DB_HANDLE hdb = DBConnectionPoolAcquireConnection();
   DB_STATEMENT hStmt = DBPrepare(hdb, _T("SELECT title,flags,guid,menu_path,node_filter,columns,table_dci_name FROM dci_summary_tables WHERE id=?"));
   if (hStmt != NULL)
   {
      DBBind(hStmt, 1, DB_SQLTYPE_INTEGER, id);
      DB_RESULT hResult = DBSelectPrepared(hStmt);
      if (hResult != NULL)
      {
         if (DBGetNumRows(hResult) > 0)
         {
            table = new SummaryTable(id, hResult);
            *rcc = RCC_SUCCESS;
         }
         else
         {
            *rcc = RCC_INVALID_SUMMARY_TABLE_ID;
         }
         DBFreeResult(hResult);
      }
      else
      {
         *rcc = RCC_DB_FAILURE;
      }
      DBFreeStatement(hStmt);
   }
   else
   {
      *rcc = RCC_DB_FAILURE;
   }
   DBConnectionPoolReleaseConnection(hdb);
   nxlog_debug(4, _T("SummaryTable::loadFromDB(%d): object=%p, rcc=%d"), id, table, (int)*rcc);
   return table;
}

/**
 * Destructor
 */
SummaryTable::~SummaryTable()
{
   delete m_columns;
   delete m_filter;
   free(m_filterSource);
}

/**
 * Pass node through filter
 */
bool SummaryTable::filter(DataCollectionTarget *object)
{
   if (m_filter == NULL)
      return true;   // no filtering

   bool result = true;
   m_filter->setGlobalVariable(_T("$object"), object->createNXSLObject());
   if (object->getObjectClass() == OBJECT_NODE)
      m_filter->setGlobalVariable(_T("$node"), object->createNXSLObject());
   if (m_filter->run())
   {
      NXSL_Value *value = m_filter->getResult();
      if (value != NULL)
      {
         result = value->getValueAsInt32() ? true : false;
      }
   }
   else
   {
      nxlog_debug(4, _T("Error executing filter script for DCI summary table: %s"), m_filter->getErrorText());
   }
   return result;
}

/**
 * Create empty result table
 */
Table *SummaryTable::createEmptyResultTable()
{
   Table *result = new Table();
   result->setTitle(m_title);
   result->setExtendedFormat(true);
   result->addColumn(_T("Node"), DCI_DT_STRING);
   if (m_flags & SUMMARY_TABLE_MULTI_INSTANCE)
      result->addColumn(_T("Instance"), DCI_DT_STRING);

   if (!(m_flags & SUMMARY_TABLE_TABLE_DCI_SOURCE))
   {
      for(int i = 0; i < m_columns->size(); i++)
      {
         result->addColumn(m_columns->get(i)->m_name, DCI_DT_STRING);
      }
   }
   return result;
}

/**
 * Create export record
 */
void SummaryTable::createExportRecord(String &xml) const
{
   TCHAR buffer[64];

   xml.append(_T("\t\t<table id=\""));
   xml.append(m_id);
   xml.append(_T("\">\n\t\t\t<guid>"));
   xml.append(m_guid.toString(buffer));
   xml.append(_T("</guid>\n\t\t\t<title>"));
   xml.appendPreallocated(EscapeStringForXML(m_title, -1));
   xml.append(_T("</title>\n\t\t\t<flags>"));
   xml.append(m_flags);
   xml.append(_T("</flags>\n\t\t\t<path>"));
   xml.appendPreallocated(EscapeStringForXML(m_menuPath, -1));
   xml.append(_T("</path>\n\t\t\t<filter>"));
   xml.appendPreallocated(EscapeStringForXML(m_filterSource, -1));
   xml.append(_T("</filter>\n\t\t\t<tableDci>\n"));
   xml.appendPreallocated(EscapeStringForXML(m_tableDciName, -1));
   xml.append(_T("</tableDci>\n\t\t\t<columns>\n"));
   for(int i = 0; i < m_columns->size(); i++)
   {
      m_columns->get(i)->createExportRecord(xml, i + 1);
   }
   xml.append(_T("\t\t\t</columns>\n\t\t</table>\n"));
}

/**
 * Query summary table
 */
Table *QuerySummaryTable(LONG tableId, SummaryTable *adHocDefinition, UINT32 baseObjectId, UINT32 userId, UINT32 *rcc)
{
   NetObj *object = FindObjectById(baseObjectId);
   if (object == NULL)
   {
      *rcc = RCC_INVALID_OBJECT_ID;
      return NULL;
   }
   if (!object->checkAccessRights(userId, OBJECT_ACCESS_READ))
   {
      *rcc = RCC_ACCESS_DENIED;
      return NULL;
   }
   if ((object->getObjectClass() != OBJECT_CONTAINER) && (object->getObjectClass() != OBJECT_CLUSTER) &&
       (object->getObjectClass() != OBJECT_SERVICEROOT) && (object->getObjectClass() != OBJECT_SUBNET) &&
       (object->getObjectClass() != OBJECT_ZONE) && (object->getObjectClass() != OBJECT_NETWORK))
   {
      *rcc = RCC_INCOMPATIBLE_OPERATION;
      return NULL;
   }

   SummaryTable *tableDefinition = (adHocDefinition != NULL) ? adHocDefinition : SummaryTable::loadFromDB(tableId, rcc);
   if (tableDefinition == NULL)
      return NULL;

   ObjectArray<NetObj> *childObjects = object->getFullChildList(true, true);
   Table *tableData = tableDefinition->createEmptyResultTable();
   for(int i = 0; i < childObjects->size(); i++)
   {
      NetObj *obj = childObjects->get(i);
      if (!obj->isDataCollectionTarget() || !obj->checkAccessRights(userId, OBJECT_ACCESS_READ))
      {
         obj->decRefCount();
         continue;
      }

      if (tableDefinition->filter((DataCollectionTarget *)obj))
      {
         ((DataCollectionTarget *)obj)->getDciValuesSummary(tableDefinition, tableData, userId);
      }
      obj->decRefCount();
   }

   delete childObjects;
   delete tableDefinition;

   return tableData;
}

/**
 * Create export record for summary table
 */
bool CreateSummaryTableExportRecord(INT32 id, String &xml)
{
   UINT32 rcc;
   SummaryTable *t = SummaryTable::loadFromDB(id, &rcc);
   if (t == NULL)
      return false;
   t->createExportRecord(xml);
   delete t;
   return true;
}

/**
 * Build column list
 */
static TCHAR *BuildColumnList(ConfigEntry *root)
{
   if (root == NULL)
      return _tcsdup(_T(""));

   String s;
   ObjectArray<ConfigEntry> *columns = root->getOrderedSubEntries(_T("column#*"));
   for(int i = 0; i < columns->size(); i++)
   {
      if (i > 0)
         s.append(_T("^~^"));

      ConfigEntry *c = columns->get(i);
      s.append(c->getSubEntryValue(_T("name")));
      s.append(_T("^#^"));
      s.append(c->getSubEntryValue(_T("dci")));
      s.append(_T("^#^"));
      s.append(c->getSubEntryValueAsUInt(_T("flags")));
      s.append(_T("^#^"));
      s.append(c->getSubEntryValue(_T("separator")));
   }
   delete columns;
   return _tcsdup((const TCHAR *)s);
}

/**
 * Import failure exit
 */
static bool ImportFailure(DB_HANDLE hdb, DB_STATEMENT hStmt)
{
   if (hStmt != NULL)
      DBFreeStatement(hStmt);
   DBRollback(hdb);
   DBConnectionPoolReleaseConnection(hdb);
   DbgPrintf(4, _T("ImportObjectTool: database failure"));
   return false;
}

/**
 * Import summary table
 */
bool ImportSummaryTable(ConfigEntry *config)
{
   const TCHAR *guid = config->getSubEntryValue(_T("guid"));
   if (guid == NULL)
   {
      DbgPrintf(4, _T("ImportSummaryTable: missing GUID"));
      return false;
   }

   uuid_t temp;
   if (_uuid_parse(guid, temp) == -1)
   {
      DbgPrintf(4, _T("ImportSummaryTable: GUID (%s) is invalid"), guid);
      return false;
   }

   DB_HANDLE hdb = DBConnectionPoolAcquireConnection();

   // Step 1: find existing tool ID by GUID
   DB_STATEMENT hStmt = DBPrepare(hdb, _T("SELECT id FROM dci_summary_tables WHERE guid=?"));
   if (hStmt == NULL)
   {
      return ImportFailure(hdb, NULL);
   }

   DBBind(hStmt, 1, DB_SQLTYPE_VARCHAR, guid, DB_BIND_STATIC);
   DB_RESULT hResult = DBSelectPrepared(hStmt);
   if (hResult == NULL)
   {
      return ImportFailure(hdb, hStmt);
   }

   UINT32 id;
   if (DBGetNumRows(hResult) > 0)
   {
      id = DBGetFieldULong(hResult, 0, 0);
   }
   else
   {
      id = 0;
   }
   DBFreeResult(hResult);
   DBFreeStatement(hStmt);

   // Step 2: create or update summary table configuration record
   if (id == 0)
   {
      id = CreateUniqueId(IDG_DCI_SUMMARY_TABLE);
      hStmt = DBPrepare(hdb, _T("INSERT INTO dci_summary_tables (menu_path,title,node_filter,flags,columns,guid,id) VALUES (?,?,?,?,?,?,?)"));
   }
   else
   {
      hStmt = DBPrepare(hdb, _T("UPDATE dci_summary_tables SET menu_path=?,title=?,node_filter=?,flags=?,columns=?,guid=? WHERE id=?"));
   }
   if (hStmt == NULL)
   {
      return ImportFailure(hdb, NULL);
   }

   DBBind(hStmt, 1, DB_SQLTYPE_VARCHAR, config->getSubEntryValue(_T("path")), DB_BIND_STATIC);
   DBBind(hStmt, 2, DB_SQLTYPE_VARCHAR, config->getSubEntryValue(_T("title")), DB_BIND_STATIC);
   DBBind(hStmt, 3, DB_SQLTYPE_TEXT, config->getSubEntryValue(_T("filter")), DB_BIND_STATIC);
   DBBind(hStmt, 4, DB_SQLTYPE_INTEGER, config->getSubEntryValueAsUInt(_T("flags")));
   DBBind(hStmt, 5, DB_SQLTYPE_TEXT, BuildColumnList(config->findEntry(_T("columns"))), DB_BIND_DYNAMIC);
   DBBind(hStmt, 6, DB_SQLTYPE_VARCHAR, guid, DB_BIND_STATIC);
   DBBind(hStmt, 7, DB_SQLTYPE_INTEGER, id);

   if (!DBExecute(hStmt))
   {
      return ImportFailure(hdb, hStmt);
   }

   NotifyClientSessions(NX_NOTIFY_DCISUMTBL_CHANGED, (UINT32)id);

   DBFreeStatement(hStmt);
   DBConnectionPoolReleaseConnection(hdb);
   return true;
}
