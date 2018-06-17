/* 
** Windows 2000+ NetXMS subagent
** Copyright (C) 2003-2014 Victor Kirhenshtein
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
** File: system.cpp
** WinNT+ specific system information parameters
**/

#include "winnt_subagent.h"

/**
 * Handler for System.ServiceState parameter
 */
LONG H_ServiceState(const TCHAR *cmd, const TCHAR *arg, TCHAR *value, AbstractCommSession *session)
{
   SC_HANDLE hManager, hService;
   TCHAR szServiceName[MAX_PATH];
   LONG iResult = SYSINFO_RC_SUCCESS;

   if (!AgentGetParameterArg(cmd, 1, szServiceName, MAX_PATH))
      return SYSINFO_RC_UNSUPPORTED;

   hManager = OpenSCManager(NULL, NULL, GENERIC_READ);
   if (hManager == NULL)
   {
      return SYSINFO_RC_ERROR;
   }

   hService = OpenService(hManager, szServiceName, SERVICE_QUERY_STATUS);
   if (hService == NULL)
   {
      iResult = SYSINFO_RC_UNSUPPORTED;
   }
   else
   {
      SERVICE_STATUS status;

      if (QueryServiceStatus(hService, &status))
      {
         int i;
         static DWORD dwStates[7]={ SERVICE_RUNNING, SERVICE_PAUSED, SERVICE_START_PENDING,
                                    SERVICE_PAUSE_PENDING, SERVICE_CONTINUE_PENDING,
                                    SERVICE_STOP_PENDING, SERVICE_STOPPED };

         for(i = 0; i < 7; i++)
            if (status.dwCurrentState == dwStates[i])
               break;
         ret_uint(value, i);
      }
      else
      {
         ret_uint(value, 255);    // Unable to retrieve information
      }
      CloseServiceHandle(hService);
   }

   CloseServiceHandle(hManager);
   return iResult;
}

/**
 * Handler for System.Services list
 */
LONG H_ServiceList(const TCHAR *pszCmd, const TCHAR *pArg, StringList *value, AbstractCommSession *session)
{
   SC_HANDLE hManager = OpenSCManager(NULL, NULL, GENERIC_READ);
   if (hManager == NULL)
   {
      return SYSINFO_RC_ERROR;
   }

   LONG rc = SYSINFO_RC_ERROR;
   DWORD bytes, count;
   EnumServicesStatusEx(hManager, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_STATE_ALL, NULL, 0, &bytes, &count, NULL, NULL);
   if (GetLastError() == ERROR_MORE_DATA)
   {
      ENUM_SERVICE_STATUS_PROCESS *services = (ENUM_SERVICE_STATUS_PROCESS *)malloc(bytes);
      if (EnumServicesStatusEx(hManager, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_STATE_ALL, (BYTE *)services, bytes, &bytes, &count, NULL, NULL))
      {
         for(DWORD i = 0; i < count; i++)
            value->add(services[i].lpServiceName);
         rc = SYSINFO_RC_SUCCESS;
      }
      free(services);
   }

   CloseServiceHandle(hManager);
   return rc;
}

/**
 * Handler for System.Services table
 */
LONG H_ServiceTable(const TCHAR *pszCmd, const TCHAR *pArg, Table *value, AbstractCommSession *session)
{
   SC_HANDLE hManager = OpenSCManager(NULL, NULL, GENERIC_READ);
   if (hManager == NULL)
   {
      return SYSINFO_RC_ERROR;
   }

   LONG rc = SYSINFO_RC_ERROR;
   DWORD bytes, count;
   EnumServicesStatusEx(hManager, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_STATE_ALL, NULL, 0, &bytes, &count, NULL, NULL);
   if (GetLastError() == ERROR_MORE_DATA)
   {
      ENUM_SERVICE_STATUS_PROCESS *services = (ENUM_SERVICE_STATUS_PROCESS *)malloc(bytes);
      if (EnumServicesStatusEx(hManager, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_STATE_ALL, (BYTE *)services, bytes, &bytes, &count, NULL, NULL))
      {
         value->addColumn(_T("NAME"), DCI_DT_STRING, _T("Name"), true);
         value->addColumn(_T("DISPNAME"), DCI_DT_STRING, _T("Display name"));
         value->addColumn(_T("TYPE"), DCI_DT_STRING, _T("Type"));
         value->addColumn(_T("STATE"), DCI_DT_STRING, _T("State"));
         value->addColumn(_T("STARTUP"), DCI_DT_STRING, _T("Startup"));
         value->addColumn(_T("PID"), DCI_DT_UINT, _T("PID"));
         value->addColumn(_T("BINARY"), DCI_DT_STRING, _T("Binary"));
         value->addColumn(_T("DEPENDENCIES"), DCI_DT_STRING, _T("Dependencies"));
         for(DWORD i = 0; i < count; i++)
         {
            value->addRow();
            value->set(0, services[i].lpServiceName);
            value->set(1, services[i].lpDisplayName);
            value->set(2, (services[i].ServiceStatusProcess.dwServiceType == SERVICE_WIN32_SHARE_PROCESS) ? _T("Shared") : _T("Own"));
            switch(services[i].ServiceStatusProcess.dwCurrentState)
            {
               case SERVICE_CONTINUE_PENDING:
                  value->set(3, _T("Continue Pending"));
                  break;
               case SERVICE_PAUSE_PENDING:
                  value->set(3, _T("Pausing"));
                  break;
               case SERVICE_PAUSED:
                  value->set(3, _T("Paused"));
                  break;
               case SERVICE_RUNNING:
                  value->set(3, _T("Running"));
                  break;
               case SERVICE_START_PENDING:
                  value->set(3, _T("Starting"));
                  break;
               case SERVICE_STOP_PENDING:
                  value->set(3, _T("Stopping"));
                  break;
               case SERVICE_STOPPED:
                  value->set(3, _T("Stopped"));
                  break;
               default:
                  value->set(3, (UINT32)services[i].ServiceStatusProcess.dwCurrentState);
                  break;
            }
            if (services[i].ServiceStatusProcess.dwProcessId != 0)
               value->set(5, (UINT32)services[i].ServiceStatusProcess.dwProcessId);

            SC_HANDLE hService = OpenService(hManager, services[i].lpServiceName, SERVICE_QUERY_CONFIG);
            if (hService != NULL)
            {
               BYTE buffer[8192];
               QUERY_SERVICE_CONFIG *cfg = (QUERY_SERVICE_CONFIG *)&buffer;
               if (QueryServiceConfig(hService, cfg, 8192, &bytes))
               {
                  switch(cfg->dwStartType)
                  {
                     case SERVICE_AUTO_START:
                        value->set(4, _T("Auto"));
                        break;
                     case SERVICE_BOOT_START:
                        value->set(4, _T("Boot"));
                        break;
                     case SERVICE_DEMAND_START:
                        value->set(4, _T("Manual"));
                        break;
                     case SERVICE_DISABLED:
                        value->set(4, _T("Disabled"));
                        break;
                     case SERVICE_SYSTEM_START:
                        value->set(4, _T("System"));
                        break;
                     default:
                        value->set(4, (UINT32)cfg->dwStartType);
                        break;
                  }
                  value->set(6, cfg->lpBinaryPathName);
                  value->set(7, cfg->lpDependencies);
               }
               CloseServiceHandle(hService);
            }
         }
         rc = SYSINFO_RC_SUCCESS;
      }
      free(services);
   }

   CloseServiceHandle(hManager);
   return rc;
}

/**
 * Handler for System.ThreadCount
 */
LONG H_ThreadCount(const TCHAR *cmd, const TCHAR *arg, TCHAR *value, AbstractCommSession *session)
{
   PERFORMANCE_INFORMATION pi;
   pi.cb = sizeof(PERFORMANCE_INFORMATION);
   if (!GetPerformanceInfo(&pi, sizeof(PERFORMANCE_INFORMATION)))
      return SYSINFO_RC_ERROR;
   ret_uint(value, pi.ThreadCount);
   return SYSINFO_RC_SUCCESS;
}

/**
 * Handler for System.HandleCount
 */
LONG H_HandleCount(const TCHAR *cmd, const TCHAR *arg, TCHAR *value, AbstractCommSession *session)
{
   PERFORMANCE_INFORMATION pi;
   pi.cb = sizeof(PERFORMANCE_INFORMATION);
   if (!GetPerformanceInfo(&pi, sizeof(PERFORMANCE_INFORMATION)))
      return SYSINFO_RC_ERROR;
   ret_uint(value, pi.HandleCount);
   return SYSINFO_RC_SUCCESS;
}

/**
 * Handler for System.ConnectedUsers parameter
 */
LONG H_ConnectedUsers(const TCHAR *pszCmd, const TCHAR *pArg, TCHAR *pValue, AbstractCommSession *session)
{
   LONG nRet;
   WTS_SESSION_INFO *pSessionList;
   DWORD i, dwNumSessions, dwCount;

   if (WTSEnumerateSessions(WTS_CURRENT_SERVER_HANDLE, 0, 1, &pSessionList, &dwNumSessions))
   {
      for(i = 0, dwCount = 0; i < dwNumSessions; i++)
         if ((pSessionList[i].State == WTSActive) ||
             (pSessionList[i].State == WTSConnected))
            dwCount++;
      WTSFreeMemory(pSessionList);
      ret_uint(pValue, dwCount);
      nRet = SYSINFO_RC_SUCCESS;
   }
   else
   {
      nRet = SYSINFO_RC_ERROR;
   }
   return nRet;
}

/**
 * Handler for System.ActiveUserSessions enum
 */
LONG H_ActiveUserSessions(const TCHAR *pszCmd, const TCHAR *pArg, StringList *value, AbstractCommSession *session)
{
   LONG nRet;
   WTS_SESSION_INFO *pSessionList;
   DWORD i, dwNumSessions, dwBytes;
   TCHAR *pszClientName, *pszUserName, szBuffer[1024];

   if (WTSEnumerateSessions(WTS_CURRENT_SERVER_HANDLE, 0, 1, &pSessionList, &dwNumSessions))
   {
      for(i = 0; i < dwNumSessions; i++)
		{
         if ((pSessionList[i].State == WTSActive) ||
             (pSessionList[i].State == WTSConnected))
         {
            if (!WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE, pSessionList[i].SessionId,
                                            WTSClientName, &pszClientName, &dwBytes))
            {
               pszClientName = NULL;
            }
            if (!WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE, pSessionList[i].SessionId,
                                            WTSUserName, &pszUserName, &dwBytes))
            {
               pszUserName = NULL;
            }

            _sntprintf(szBuffer, 1024, _T("\"%s\" \"%s\" \"%s\""),
                       pszUserName == NULL ? _T("UNKNOWN") : pszUserName,
                       pSessionList[i].pWinStationName,
                       pszClientName == NULL ? _T("UNKNOWN") : pszClientName);
            value->add(szBuffer);

            if (pszUserName != NULL)
               WTSFreeMemory(pszUserName);
            if (pszClientName != NULL)
               WTSFreeMemory(pszClientName);
         }
		}
      WTSFreeMemory(pSessionList);
      nRet = SYSINFO_RC_SUCCESS;
   }
   else
   {
      nRet = SYSINFO_RC_ERROR;
   }
   return nRet;
}

/**
 * Callback for window stations enumeration
 */
static BOOL CALLBACK WindowStationsEnumCallback(LPTSTR lpszWindowStation, LPARAM lParam)
{
   ((StringList *)lParam)->add(lpszWindowStation);
   return TRUE;
}

/**
 * Handler for System.WindowStations list
 */
LONG H_WindowStations(const TCHAR *cmd, const TCHAR *arg, StringList *value, AbstractCommSession *session)
{
   return EnumWindowStations(WindowStationsEnumCallback, (LONG_PTR)value) ? SYSINFO_RC_SUCCESS : SYSINFO_RC_ERROR;
}

/**
 * Callback for desktop enumeration
 */
static BOOL CALLBACK DesktopsEnumCallback(LPTSTR lpszDesktop, LPARAM lParam)
{
   ((StringList *)lParam)->add(lpszDesktop);
   return TRUE;
}

/**
 * Handler for System.Desktops list
 */
LONG H_Desktops(const TCHAR *cmd, const TCHAR *arg, StringList *value, AbstractCommSession *session)
{
   TCHAR wsName[256];
   AgentGetParameterArg(cmd, 1, wsName, 256);
   HWINSTA ws = OpenWindowStation(wsName, FALSE, WINSTA_ENUMDESKTOPS);
   if (ws == NULL)
      return SYSINFO_RC_ERROR;

   LONG rc = EnumDesktops(ws, DesktopsEnumCallback, (LONG_PTR)value) ? SYSINFO_RC_SUCCESS : SYSINFO_RC_ERROR;
   CloseWindowStation(ws);
   return rc;
}

/**
 * Handler for Agent.Desktop parameter
 */
LONG H_AgentDesktop(const TCHAR *cmd, const TCHAR *arg, TCHAR *value, AbstractCommSession *session)
{
   HWINSTA ws = GetProcessWindowStation();
   if (ws == NULL)
      return SYSINFO_RC_ERROR;

   HDESK desk = GetThreadDesktop(GetCurrentThreadId());
   if (desk == NULL)
      return SYSINFO_RC_ERROR;

   TCHAR wsName[64], deskName[64];
   DWORD size;
   if (GetUserObjectInformation(ws, UOI_NAME, wsName, 64 * sizeof(TCHAR), &size) &&
       GetUserObjectInformation(desk, UOI_NAME, deskName, 64 * sizeof(TCHAR), &size))
   {
      DWORD sid;
      if (ProcessIdToSessionId(GetCurrentProcessId(), &sid))
      {
         TCHAR *sessionName;
         if (WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE, sid, WTSWinStationName, &sessionName, &size))
         {
            _sntprintf(value, MAX_RESULT_LENGTH, _T("/%s/%s/%s"), sessionName, wsName, deskName);
            WTSFreeMemory(sessionName);
         }
         else
         {
            _sntprintf(value, MAX_RESULT_LENGTH, _T("/%u/%s/%s"), sid, wsName, deskName);
         }
      }
      else
      {
         _sntprintf(value, MAX_RESULT_LENGTH, _T("/?/%s/%s"), wsName, deskName);
      }
      return SYSINFO_RC_SUCCESS;
   }
   else
   {
      return SYSINFO_RC_ERROR;
   }
}

/**
 * Handler for System.AppAddressSpace
 */
LONG H_AppAddressSpace(const TCHAR *pszCmd, const TCHAR *pArg, TCHAR *pValue, AbstractCommSession *session)
{
	SYSTEM_INFO si;

	GetSystemInfo(&si);
	DWORD_PTR size = (DWORD_PTR)si.lpMaximumApplicationAddress - (DWORD_PTR)si.lpMinimumApplicationAddress;
	ret_uint(pValue, (DWORD)(size / 1024 / 1024));
	return SYSINFO_RC_SUCCESS;
}

/**
 * Handler for System.Update.*Time parameters
 */
LONG H_SysUpdateTime(const TCHAR *cmd, const TCHAR *arg, TCHAR *value, AbstractCommSession *session)
{
   TCHAR buffer[MAX_PATH];
   _sntprintf(buffer, MAX_PATH, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WindowsUpdate\\Auto Update\\Results\\%s"), arg);

   HKEY hKey;
   if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, buffer, 0, KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS)
      return SYSINFO_RC_ERROR;

   LONG rc;
   DWORD size = MAX_PATH * sizeof(TCHAR);
   if (RegQueryValueEx(hKey, _T("LastSuccessTime"), NULL, NULL, (BYTE *)buffer, &size) == ERROR_SUCCESS)
   {
      // Date stored as YYYY-mm-dd HH:MM:SS in UTC
      if (_tcslen(buffer) == 19)
      {
         struct tm t;
         memset(&t, 0, sizeof(struct tm));
         t.tm_isdst = 0;

         buffer[4] = 0;
         t.tm_year = _tcstol(buffer, NULL, 10) - 1900;

         buffer[7] = 0;
         t.tm_mon = _tcstol(&buffer[5], NULL, 10) - 1;

         buffer[10] = 0;
         t.tm_mday = _tcstol(&buffer[8], NULL, 10);

         buffer[13] = 0;
         t.tm_hour = _tcstol(&buffer[11], NULL, 10);

         buffer[16] = 0;
         t.tm_min = _tcstol(&buffer[14], NULL, 10);

         t.tm_sec = _tcstol(&buffer[17], NULL, 10);

         ret_int64(value, timegm(&t));
         rc = SYSINFO_RC_SUCCESS;
      }      
   }
   else
   {
      rc = SYSINFO_RC_ERROR;
   }
   RegCloseKey(hKey);
	return rc;
}
