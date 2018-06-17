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
** File: actions.cpp
**
**/

#include "nxcore.h"

/**
 * Static data
 */
static UINT32 m_dwNumActions = 0;
static NXC_ACTION *m_pActionList = NULL;
static RWLOCK m_rwlockActionListAccess;
static UINT32 m_dwUpdateCode;

/**
 * Send updates to all connected clients
 */
static void SendActionDBUpdate(ClientSession *pSession, void *pArg)
{
   pSession->onActionDBUpdate(m_dwUpdateCode, (NXC_ACTION *)pArg);
}

/**
 * Destroy action list
 */
static void DestroyActionList()
{
   UINT32 i;

   RWLockWriteLock(m_rwlockActionListAccess, INFINITE);
   if (m_pActionList != NULL)
   {
      for(i = 0; i < m_dwNumActions; i++)
         safe_free(m_pActionList[i].pszData);
      free(m_pActionList);
      m_pActionList = NULL;
      m_dwNumActions = 0;
   }
   RWLockUnlock(m_rwlockActionListAccess);
}

/**
 * Load actions list from database
 */
static BOOL LoadActions()
{
   DB_RESULT hResult;
   BOOL bResult = FALSE;
   UINT32 i;

   DB_HANDLE hdb = DBConnectionPoolAcquireConnection();
   hResult = DBSelect(hdb, _T("SELECT action_id,action_name,action_type,")
                                 _T("is_disabled,rcpt_addr,email_subject,action_data ")
                                 _T("FROM actions ORDER BY action_id"));
   if (hResult != NULL)
   {
      DestroyActionList();
      m_dwNumActions = (UINT32)DBGetNumRows(hResult);
      m_pActionList = (NXC_ACTION *)malloc(sizeof(NXC_ACTION) * m_dwNumActions);
      memset(m_pActionList, 0, sizeof(NXC_ACTION) * m_dwNumActions);
      for(i = 0; i < m_dwNumActions; i++)
      {
         m_pActionList[i].dwId = DBGetFieldULong(hResult, i, 0);
         DBGetField(hResult, i, 1, m_pActionList[i].szName, MAX_OBJECT_NAME);
         m_pActionList[i].iType = DBGetFieldLong(hResult, i, 2);
         m_pActionList[i].bIsDisabled = DBGetFieldLong(hResult, i, 3);
         DBGetField(hResult, i, 4, m_pActionList[i].szRcptAddr, MAX_RCPT_ADDR_LEN);
         DBGetField(hResult, i, 5, m_pActionList[i].szEmailSubject, MAX_EMAIL_SUBJECT_LEN);
         m_pActionList[i].pszData = DBGetField(hResult, i, 6, NULL, 0);
      }
      DBFreeResult(hResult);
      bResult = TRUE;
   }
   else
   {
      nxlog_write(MSG_ACTIONS_LOAD_FAILED, EVENTLOG_ERROR_TYPE, NULL);
   }
   DBConnectionPoolReleaseConnection(hdb);
   return bResult;
}

/**
 * Initialize action-related stuff
 */
BOOL InitActions()
{
   BOOL bSuccess = FALSE;

   m_rwlockActionListAccess = RWLockCreate();
   if (m_rwlockActionListAccess != NULL)
      bSuccess = LoadActions();
   return bSuccess;
}

/**
 * Cleanup action-related stuff
 */
void CleanupActions()
{
   DestroyActionList();
   RWLockDestroy(m_rwlockActionListAccess);
}

/**
 * Save action record to database
 */
static void SaveActionToDB(NXC_ACTION *pAction)
{
	DB_HANDLE hdb = DBConnectionPoolAcquireConnection();

   // Prepare and execute INSERT or UPDATE query
   TCHAR szQuery[8192];
   if (IsDatabaseRecordExist(hdb, _T("actions"), _T("action_id"), pAction->dwId))
      _sntprintf(szQuery, 8192,
		           _T("UPDATE actions SET action_name=%s,action_type=%d,is_disabled=%d,")
                 _T("rcpt_addr=%s,email_subject=%s,action_data=%s ")
                 _T("WHERE action_id=%d"),
              (const TCHAR *)DBPrepareString(hdb, pAction->szName, 63), pAction->iType, pAction->bIsDisabled,
              (const TCHAR *)DBPrepareString(hdb, pAction->szRcptAddr, 255),
				  (const TCHAR *)DBPrepareString(hdb, pAction->szEmailSubject, 255),
				  (const TCHAR *)DBPrepareString(hdb, CHECK_NULL_EX(pAction->pszData)), pAction->dwId);
   else
      _sntprintf(szQuery, 8192,
		           _T("INSERT INTO actions (action_id,action_name,action_type,")
                 _T("is_disabled,rcpt_addr,email_subject,action_data) VALUES")
                 _T(" (%d,%s,%d,%d,%s,%s,%s)"),
              pAction->dwId, (const TCHAR *)DBPrepareString(hdb, pAction->szName, 63),
				  pAction->iType, pAction->bIsDisabled,
              (const TCHAR *)DBPrepareString(hdb, pAction->szRcptAddr, 255),
				  (const TCHAR *)DBPrepareString(hdb, pAction->szEmailSubject, 255),
				  (const TCHAR *)DBPrepareString(hdb, CHECK_NULL_EX(pAction->pszData)));
	DBQuery(hdb, szQuery);

	DBConnectionPoolReleaseConnection(hdb);
}

/**
 * Compare action's id for bsearch()
 */
static int CompareId(const void *key, const void *elem)
{
   return CAST_FROM_POINTER(key, UINT32) < ((NXC_ACTION *)elem)->dwId ? -1 :
            (CAST_FROM_POINTER(key, UINT32) > ((NXC_ACTION *)elem)->dwId ? 1 : 0);
}

/**
 * Compare action id for qsort()
 */
static int CompareElements(const void *p1, const void *p2)
{
   return ((NXC_ACTION *)p1)->dwId < ((NXC_ACTION *)p2)->dwId ? -1 :
            (((NXC_ACTION *)p1)->dwId > ((NXC_ACTION *)p2)->dwId ? 1 : 0);
}

/**
 * Execute remote action
 */
static BOOL ExecuteRemoteAction(TCHAR *pszTarget, TCHAR *pszAction)
{
   AgentConnection *pConn;
   UINT32 dwError;
   int i, nLen, nState, nCount = 0;
   TCHAR *pCmd[128], *pTmp;

   if (pszTarget[0] == '@')
   {
      //Resolve name of node to connection to it. Name shuld be in @name format.
      Node *node = (Node *)FindObjectByName(pszTarget+1, OBJECT_NODE);
      if(node == NULL)
         return FALSE;
      pConn = node->createAgentConnection();
      if (pConn == NULL)
         return FALSE;
   }
   else
   {
      // Resolve hostname
      InetAddress addr = InetAddress::resolveHostName(pszTarget);
      if (!addr.isValid())
         return FALSE;

      Node *node = FindNodeByIP(0, addr.getAddressV4());	/* TODO: add possibility to specify action target by object id */
      if (node != NULL)
      {
         pConn = node->createAgentConnection();
         if (pConn == NULL)
            return FALSE;
      }
      else
      {
         // Target node is not in our database, try default communication settings
         pConn = new AgentConnection(addr, AGENT_LISTEN_PORT, AUTH_NONE, _T(""));
         pConn->setCommandTimeout(g_agentCommandTimeout);
         if (!pConn->connect(g_pServerKey))
         {
            pConn->decRefCount();
            return FALSE;
         }
      }
   }

	pTmp = _tcsdup(pszAction);
	pCmd[0] = pTmp;
	nLen = (int)_tcslen(pTmp);
	for(i = 0, nState = 0, nCount = 1; (i < nLen) && (nCount < 127); i++)
	{
		switch(pTmp[i])
		{
			case _T(' '):
				if (nState == 0)
				{
					pTmp[i] = 0;
					if (pTmp[i + 1] != 0)
					{
						pCmd[nCount++] = pTmp + i + 1;
					}
				}
				break;
			case _T('"'):
				nState == 0 ? nState++ : nState--;

				memmove(pTmp + i, pTmp + i + 1, (nLen - i) * sizeof(TCHAR));
				i--;
				break;
			case _T('\\'):
				if (pTmp[i + 1] == _T('"'))
				{
					memmove(pTmp + i, pTmp + i + 1, (nLen - i) * sizeof(TCHAR));
				}
				break;
			default:
				break;
		}
	}
	pCmd[nCount] = NULL;

   dwError = pConn->execAction(pCmd[0], nCount - 1, &pCmd[1]);
   pConn->disconnect();
   pConn->decRefCount();
   free(pTmp);
   return dwError == ERR_SUCCESS;
}

/**
 * Run external command via system()
 */
static void RunCommand(void *arg)
{
   if (ConfigReadInt(_T("EscapeLocalCommands"), 0))
   {
      String s = (TCHAR *)arg;
#ifdef _WIN32
      s.replace(_T("\t"), _T("\\t"));
      s.replace(_T("\n"), _T("\\n"));
      s.replace(_T("\r"), _T("\\r"));
#else
      s.replace(_T("\t"), _T("\\\\t"));
      s.replace(_T("\n"), _T("\\\\n"));
      s.replace(_T("\r"), _T("\\\\r"));
#endif
      free(arg);
      arg = _tcsdup(s.getBuffer());
   }
   DbgPrintf(3, _T("*actions* Executing command \"%s\""), (TCHAR *)arg);
	if (_tsystem((TCHAR *)arg) == -1)
	   DbgPrintf(5, _T("RunCommandThread: failed to execute command \"%s\""), (TCHAR *)arg);
	free(arg);
}

/**
 * Forward event to other server
 */
static bool ForwardEvent(const TCHAR *server, Event *event)
{
   InetAddress addr = InetAddress::resolveHostName(server);
	if (!addr.isValidUnicast())
	{
		nxlog_debug(2, _T("ForwardEvent: host name %s is invalid or cannot be resolved"), server);
		return false;
	}

	ISC *isc = new ISC(addr);
	UINT32 rcc = isc->connect(ISC_SERVICE_EVENT_FORWARDER);
	if (rcc == ISC_ERR_SUCCESS)
	{
		NXCPMessage msg;
		NetObj *object;

		msg.setId(1);
		msg.setCode(CMD_FORWARD_EVENT);
		object = FindObjectById(event->getSourceId());
		if (object != NULL)
		{
         if (object->getObjectClass() == OBJECT_NODE)
         {
			   msg.setField(VID_IP_ADDRESS, ((Node *)object)->getIpAddress());
         }
			msg.setField(VID_EVENT_CODE, event->getCode());
			msg.setField(VID_EVENT_NAME, event->getName());
			if (event->getUserTag() != NULL)
				msg.setField(VID_USER_TAG, event->getUserTag());
			msg.setField(VID_NUM_ARGS, (WORD)event->getParametersCount());
			for(int i = 0; i < event->getParametersCount(); i++)
				msg.setField(VID_EVENT_ARG_BASE + i, event->getParameter(i));

			if (isc->sendMessage(&msg))
			{
				rcc = isc->waitForRCC(1, 10000);
			}
			else
			{
				rcc = ISC_ERR_CONNECTION_BROKEN;
			}
		}
		else
		{
			rcc = ISC_ERR_INTERNAL_ERROR;
		}
		isc->disconnect();
	}
	delete isc;
	if (rcc != ISC_ERR_SUCCESS)
		nxlog_write(MSG_EVENT_FORWARD_FAILED, EVENTLOG_WARNING_TYPE, "ss", server, ISCErrorCodeToText(rcc));
	return rcc == ISC_ERR_SUCCESS;
}

/**
 * Execute NXSL script
 */
static BOOL ExecuteActionScript(const TCHAR *scriptName, Event *event)
{
	BOOL success = FALSE;
	NXSL_VM *vm = CreateServerScriptVM(scriptName);
	if (vm != NULL)
	{
		NetObj *object = FindObjectById(event->getSourceId());
		if (object != NULL)
		{
         vm->setGlobalVariable(_T("$object"), object->createNXSLObject());
		   if (object->getObjectClass() == OBJECT_NODE)
	         vm->setGlobalVariable(_T("$node"), object->createNXSLObject());
		}
		vm->setGlobalVariable(_T("$event"), new NXSL_Value(new NXSL_Object(&g_nxslEventClass, event)));

		// Pass event's parameters as arguments
		NXSL_Value **ppValueList = (NXSL_Value **)malloc(sizeof(NXSL_Value *) * event->getParametersCount());
		memset(ppValueList, 0, sizeof(NXSL_Value *) * event->getParametersCount());
		for(int i = 0; i < event->getParametersCount(); i++)
			ppValueList[i] = new NXSL_Value(event->getParameter(i));

		if (vm->run(event->getParametersCount(), ppValueList))
		{
			DbgPrintf(4, _T("ExecuteActionScript: script %s successfully executed"), scriptName);
			success = TRUE;
		}
		else
		{
			DbgPrintf(4, _T("ExecuteActionScript: Script %s execution error: %s"), scriptName, vm->getErrorText());
			PostEvent(EVENT_SCRIPT_ERROR, g_dwMgmtNode, "ssd", scriptName, vm->getErrorText(), 0);
		}
	   free(ppValueList);
      delete vm;
	}
	else
	{
		DbgPrintf(4, _T("ExecuteActionScript(): Cannot find script %s"), scriptName);
	}
	return success;
}

/**
 * Execute action on specific event
 */
BOOL ExecuteAction(UINT32 dwActionId, Event *pEvent, const TCHAR *alarmMsg, const TCHAR *alarmKey)
{
   static const TCHAR *actionType[] = { _T("EXEC"), _T("REMOTE"), _T("SEND EMAIL"), _T("SEND SMS"), _T("FORWARD EVENT"), _T("NXSL SCRIPT"), _T("XMPP MESSAGE") };

   NXC_ACTION *pAction;
   BOOL bSuccess = FALSE;

   RWLockReadLock(m_rwlockActionListAccess, INFINITE);
   pAction = (NXC_ACTION *)bsearch(CAST_TO_POINTER(dwActionId, void *), m_pActionList,
                                   m_dwNumActions, sizeof(NXC_ACTION), CompareId);
   if (pAction != NULL)
   {
      if (pAction->bIsDisabled)
      {
         DbgPrintf(3, _T("*actions* Action %d (%s) is disabled and will not be executed"), dwActionId, pAction->szName);
         bSuccess = TRUE;
      }
      else
      {
         DbgPrintf(3, _T("*actions* Executing action %d (%s) of type %s"),
            dwActionId, pAction->szName, actionType[pAction->iType]);

         TCHAR *pszExpandedData, *pszExpandedSubject, *pszExpandedRcpt, *curr, *next;

         pszExpandedData = pEvent->expandText(CHECK_NULL_EX(pAction->pszData), alarmMsg, alarmKey);
         StrStrip(pszExpandedData);

         pszExpandedRcpt = pEvent->expandText(pAction->szRcptAddr, alarmMsg, alarmKey);
         StrStrip(pszExpandedRcpt);

         switch(pAction->iType)
         {
            case ACTION_EXEC:
               if (pszExpandedData[0] != 0)
               {
                  ThreadPoolExecute(g_mainThreadPool, RunCommand, _tcsdup(pszExpandedData));
               }
               else
               {
                  DbgPrintf(3, _T("*actions* Empty command - nothing to execute"));
               }
					bSuccess = TRUE;
               break;
            case ACTION_SEND_EMAIL:
               if (pszExpandedRcpt[0] != 0)
               {
                  DbgPrintf(3, _T("*actions* Sending mail to %s: \"%s\""), pszExpandedRcpt, pszExpandedData);
                  pszExpandedSubject = pEvent->expandText(pAction->szEmailSubject, alarmMsg, alarmKey);
					   curr = pszExpandedRcpt;
					   do
					   {
						   next = _tcschr(curr, _T(';'));
						   if (next != NULL)
							   *next = 0;
						   StrStrip(curr);
						   PostMail(curr, pszExpandedSubject, pszExpandedData);
						   curr = next + 1;
					   } while(next != NULL);
                  free(pszExpandedSubject);
               }
               else
               {
                  DbgPrintf(3, _T("*actions* Empty recipients list - mail will not be sent"));
               }
               bSuccess = TRUE;
               break;
            case ACTION_SEND_SMS:
               if (pszExpandedRcpt[0] != 0)
               {
                  DbgPrintf(3, _T("*actions* Sending SMS to %s: \"%s\""), pszExpandedRcpt, pszExpandedData);
					   curr = pszExpandedRcpt;
					   do
					   {
						   next = _tcschr(curr, _T(';'));
						   if (next != NULL)
							   *next = 0;
						   StrStrip(curr);
	                  PostSMS(curr, pszExpandedData);
						   curr = next + 1;
					   } while(next != NULL);
               }
               else
               {
                  DbgPrintf(3, _T("*actions* Empty recipients list - SMS will not be sent"));
               }
               bSuccess = TRUE;
               break;
            case ACTION_XMPP_MESSAGE:
               if (pszExpandedRcpt[0] != 0)
               {
#if XMPP_SUPPORTED
                  DbgPrintf(3, _T("*actions* Sending XMPP message to %s: \"%s\""), pszExpandedRcpt, pszExpandedData);
					   curr = pszExpandedRcpt;
					   do
					   {
						   next = _tcschr(curr, _T(';'));
						   if (next != NULL)
							   *next = 0;
						   StrStrip(curr);
	                  SendXMPPMessage(curr, pszExpandedData);
						   curr = next + 1;
					   } while(next != NULL);
#else
                  DbgPrintf(3, _T("*actions* cannot send XMPP message to %s (server compiled without XMPP support)"), pszExpandedRcpt);
#endif
               }
               else
               {
                  DbgPrintf(3, _T("*actions* Empty recipients list - XMPP message will not be sent"));
               }
               bSuccess = TRUE;
               break;
            case ACTION_REMOTE:
               if (pszExpandedRcpt[0] != 0)
               {
                  DbgPrintf(3, _T("*actions* Executing on \"%s\": \"%s\""), pszExpandedRcpt, pszExpandedData);
                  bSuccess = ExecuteRemoteAction(pszExpandedRcpt, pszExpandedData);
               }
               else
               {
                  DbgPrintf(3, _T("*actions* Empty target list - remote action will not be executed"));
                  bSuccess = TRUE;
               }
               break;
				case ACTION_FORWARD_EVENT:
               if (pszExpandedRcpt[0] != 0)
               {
                  DbgPrintf(3, _T("*actions* Forwarding event to \"%s\""), pszExpandedRcpt);
                  bSuccess = ForwardEvent(pszExpandedRcpt, pEvent);
               }
               else
               {
                  DbgPrintf(3, _T("*actions* Empty destination - event will not be forwarded"));
                  bSuccess = TRUE;
               }
					break;
				case ACTION_NXSL_SCRIPT:
               if (pszExpandedRcpt[0] != 0)
               {
                  DbgPrintf(3, _T("*actions* Executing NXSL script \"%s\""), pszExpandedRcpt);
                  bSuccess = ExecuteActionScript(pszExpandedRcpt, pEvent);
               }
               else
               {
                  DbgPrintf(3, _T("*actions* Empty script name - nothing to execute"));
                  bSuccess = TRUE;
               }
					break;
            default:
               break;
         }
         free(pszExpandedRcpt);
         free(pszExpandedData);
      }
   }
   RWLockUnlock(m_rwlockActionListAccess);
   return bSuccess;
}

/**
 * Create new action
 */
UINT32 CreateNewAction(const TCHAR *pszName, UINT32 *pdwId)
{
   UINT32 i, dwResult = RCC_SUCCESS;

   RWLockWriteLock(m_rwlockActionListAccess, INFINITE);

   // Check for duplicate name
   for(i = 0; i < m_dwNumActions; i++)
      if (!_tcsicmp(m_pActionList[i].szName, pszName))
      {
         dwResult = RCC_OBJECT_ALREADY_EXISTS;
         break;
      }

   // If not exist, create it
   if (i == m_dwNumActions)
   {
      m_dwNumActions++;
      m_pActionList = (NXC_ACTION *)realloc(m_pActionList, sizeof(NXC_ACTION) * m_dwNumActions);
      m_pActionList[i].dwId = CreateUniqueId(IDG_ACTION);
      nx_strncpy(m_pActionList[i].szName, pszName, MAX_OBJECT_NAME);
      m_pActionList[i].bIsDisabled = TRUE;
      m_pActionList[i].iType = ACTION_EXEC;
      m_pActionList[i].szEmailSubject[0] = 0;
      m_pActionList[i].szRcptAddr[0] = 0;
      m_pActionList[i].pszData = NULL;

      qsort(m_pActionList, m_dwNumActions, sizeof(NXC_ACTION), CompareElements);

      SaveActionToDB(&m_pActionList[i]);
      m_dwUpdateCode = NX_NOTIFY_ACTION_CREATED;
      EnumerateClientSessions(SendActionDBUpdate, &m_pActionList[i]);
      *pdwId = m_pActionList[i].dwId;
   }

   RWLockUnlock(m_rwlockActionListAccess);
   return dwResult;
}

/**
 * Delete action
 */
UINT32 DeleteActionFromDB(UINT32 dwActionId)
{
   UINT32 i, dwResult = RCC_INVALID_ACTION_ID;
   TCHAR szQuery[256];

   DB_HANDLE hdb = DBConnectionPoolAcquireConnection();

   RWLockWriteLock(m_rwlockActionListAccess, INFINITE);

   for(i = 0; i < m_dwNumActions; i++)
      if (m_pActionList[i].dwId == dwActionId)
      {
         m_dwUpdateCode = NX_NOTIFY_ACTION_DELETED;
         EnumerateClientSessions(SendActionDBUpdate, &m_pActionList[i]);

         m_dwNumActions--;
         safe_free(m_pActionList[i].pszData);
         memmove(&m_pActionList[i], &m_pActionList[i + 1], sizeof(NXC_ACTION) * (m_dwNumActions - i));
         _sntprintf(szQuery, 256, _T("DELETE FROM actions WHERE action_id=%d"), dwActionId);
         DBQuery(hdb, szQuery);

         dwResult = RCC_SUCCESS;
         break;
      }

   RWLockUnlock(m_rwlockActionListAccess);
   DBConnectionPoolReleaseConnection(hdb);
   return dwResult;
}

/**
 * Modify action record from message
 */
UINT32 ModifyActionFromMessage(NXCPMessage *pMsg)
{
   UINT32 i, dwResult = RCC_INVALID_ACTION_ID;
   UINT32 dwActionId;
	TCHAR name[MAX_OBJECT_NAME];

   pMsg->getFieldAsString(VID_ACTION_NAME, name, MAX_OBJECT_NAME);
	if (!IsValidObjectName(name, TRUE))
		return RCC_INVALID_OBJECT_NAME;

   dwActionId = pMsg->getFieldAsUInt32(VID_ACTION_ID);
   RWLockWriteLock(m_rwlockActionListAccess, INFINITE);

   // Find action with given id
   for(i = 0; i < m_dwNumActions; i++)
      if (m_pActionList[i].dwId == dwActionId)
      {
         m_pActionList[i].bIsDisabled = pMsg->getFieldAsUInt16(VID_IS_DISABLED);
         m_pActionList[i].iType = pMsg->getFieldAsUInt16(VID_ACTION_TYPE);
         safe_free(m_pActionList[i].pszData);
         m_pActionList[i].pszData = pMsg->getFieldAsString(VID_ACTION_DATA);
         pMsg->getFieldAsString(VID_EMAIL_SUBJECT, m_pActionList[i].szEmailSubject, MAX_EMAIL_SUBJECT_LEN);
         pMsg->getFieldAsString(VID_RCPT_ADDR, m_pActionList[i].szRcptAddr, MAX_RCPT_ADDR_LEN);
			_tcscpy(m_pActionList[i].szName, name);

         SaveActionToDB(&m_pActionList[i]);

         m_dwUpdateCode = NX_NOTIFY_ACTION_MODIFIED;
         EnumerateClientSessions(SendActionDBUpdate, &m_pActionList[i]);

         dwResult = RCC_SUCCESS;
         break;
      }

   RWLockUnlock(m_rwlockActionListAccess);
   return dwResult;
}

/**
 * Fill CSCP message with action's data
 */
void FillActionInfoMessage(NXCPMessage *pMsg, NXC_ACTION *pAction)
{
   pMsg->setField(VID_IS_DISABLED, (WORD)pAction->bIsDisabled);
   pMsg->setField(VID_ACTION_TYPE, (WORD)pAction->iType);
   pMsg->setField(VID_ACTION_DATA, CHECK_NULL_EX(pAction->pszData));
   pMsg->setField(VID_EMAIL_SUBJECT, pAction->szEmailSubject);
   pMsg->setField(VID_ACTION_NAME, pAction->szName);
   pMsg->setField(VID_RCPT_ADDR, pAction->szRcptAddr);
}

/**
 * Send all actions to client
 */
void SendActionsToClient(ClientSession *pSession, UINT32 dwRqId)
{
   UINT32 i;
   NXCPMessage msg;

   // Prepare message
   msg.setCode(CMD_ACTION_DATA);
   msg.setId(dwRqId);

   RWLockReadLock(m_rwlockActionListAccess, INFINITE);

   for(i = 0; i < m_dwNumActions; i++)
   {
      msg.setField(VID_ACTION_ID, m_pActionList[i].dwId);
      FillActionInfoMessage(&msg, &m_pActionList[i]);
      pSession->sendMessage(&msg);
      msg.deleteAllFields();
   }

   RWLockUnlock(m_rwlockActionListAccess);

   // Send end-of-list flag
   msg.setField(VID_ACTION_ID, (UINT32)0);
   pSession->sendMessage(&msg);
}

/**
 * Export action configuration
 */
void CreateActionExportRecord(String &xml, UINT32 id)
{
   DB_HANDLE hdb = DBConnectionPoolAcquireConnection();
   DB_STATEMENT hStmt = DBPrepare(hdb, _T("SELECT action_name,action_type,")
                                       _T("rcpt_addr,email_subject,action_data ")
                                       _T("FROM actions WHERE action_id=?"));
   if (hStmt == NULL)
   {
      DBConnectionPoolReleaseConnection(hdb);
      return;
   }

   DBBind(hStmt, 1, DB_SQLTYPE_INTEGER, id);
   DB_RESULT hResult = DBSelectPrepared(hStmt);
   if (hResult != NULL && DBGetNumRows(hResult) > 0)
   {
      xml.append(_T("\t\t<action id=\""));
      xml.append(id);
      xml.append(_T("\">\n\t\t\t<name>"));
      xml.appendPreallocated(DBGetFieldForXML(hResult, 0, 0));
      xml.append(_T("</name>\n\t\t\t<type>"));
      xml.append(DBGetFieldULong(hResult, 0, 1));
      xml.append(_T("</type>\n\t\t\t<recipientAddress>"));
      xml.appendPreallocated(DBGetFieldForXML(hResult, 0, 2));
      xml.append(_T("</recipientAddress>\n\t\t\t<emailSubject>"));
      xml.appendPreallocated(DBGetFieldForXML(hResult, 0, 3));
      xml.append(_T("</emailSubject>\n\t\t\t<data>"));
      xml.appendPreallocated(DBGetFieldForXML(hResult, 0, 4));
      xml.append(_T("</data>\n"));
      xml.append(_T("\t\t</action>\n"));
      DBFreeResult(hResult);
   }
   DBFreeStatement(hStmt);
   DBConnectionPoolReleaseConnection(hdb);
}

/**
 * Import action configuration
 */
bool ImportAction(ConfigEntry *config)
{
   if (config->getSubEntryValue(_T("name")) == NULL)
   {
      DbgPrintf(4, _T("ImportAction: no name specified"));
      return false;
   }

   RWLockWriteLock(m_rwlockActionListAccess, INFINITE);

   UINT32 i;
   // Check for duplicate name
   for(i = 0; i < m_dwNumActions; i++)
   {
      if (!_tcsicmp(m_pActionList[i].szName, config->getSubEntryValue(_T("name"))))
      {
         DbgPrintf(4, _T("ImportAction: name already exists"));
         return false;
      }
   }

   // If not exist, create it
   m_dwNumActions++;
   m_pActionList = (NXC_ACTION *)realloc(m_pActionList, sizeof(NXC_ACTION) * m_dwNumActions);
   m_pActionList[i].dwId = CreateUniqueId(IDG_ACTION);
   nx_strncpy(m_pActionList[i].szName, config->getSubEntryValue(_T("name")), MAX_OBJECT_NAME);
   m_pActionList[i].bIsDisabled = FALSE;
   m_pActionList[i].iType = config->getSubEntryValueAsUInt(_T("type"), 0);
   if (config->getSubEntryValue(_T("emailSubject")) == NULL)
      m_pActionList[i].szEmailSubject[0] = 0;
   else
      nx_strncpy(m_pActionList[i].szEmailSubject, config->getSubEntryValue(_T("emailSubject")), MAX_EMAIL_SUBJECT_LEN);
   if (config->getSubEntryValue(_T("recipientAddress")) == NULL)
      m_pActionList[i].szRcptAddr[0] = 0;
   else
      nx_strncpy(m_pActionList[i].szRcptAddr, config->getSubEntryValue(_T("recipientAddress")), MAX_RCPT_ADDR_LEN);
   if (config->getSubEntryValue(_T("data")) == NULL)
      m_pActionList[i].pszData = NULL;
   else
      m_pActionList[i].pszData = _tcsdup(config->getSubEntryValue(_T("data")));

   qsort(m_pActionList, m_dwNumActions, sizeof(NXC_ACTION), CompareElements);

   SaveActionToDB(&m_pActionList[i]);
   m_dwUpdateCode = NX_NOTIFY_ACTION_CREATED;
   EnumerateClientSessions(SendActionDBUpdate, &m_pActionList[i]);

   RWLockUnlock(m_rwlockActionListAccess);
   return true;
}
