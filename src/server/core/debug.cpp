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
** File: debug.cpp
**
**/

#include "nxcore.h"

#ifdef _WIN32
#include <dbghelp.h>
#endif

/**
 * Test read/write lock state and print to stdout
 */
void DbgTestRWLock(RWLOCK hLock, const TCHAR *szName, CONSOLE_CTX console)
{
   ConsolePrintf(console, _T("  %s: "), szName);
   if (RWLockWriteLock(hLock, 100))
   {
      ConsolePrintf(console, _T("unlocked\n"));
      RWLockUnlock(hLock);
   }
   else
   {
      if (RWLockReadLock(hLock, 100))
      {
         ConsolePrintf(console, _T("locked for reading\n"));
         RWLockUnlock(hLock);
      }
      else
      {
         ConsolePrintf(console, _T("locked for writing\n"));
      }
   }
}

/**
 * Print message to console, either local or remote
 */
void ConsolePrintf(CONSOLE_CTX console, const TCHAR *pszFormat, ...)
{
   va_list args;
   TCHAR szBuffer[8192];

   va_start(args, pszFormat);
   _vsntprintf(szBuffer, 8191, pszFormat, args);
	szBuffer[8191] = 0;
   va_end(args);

	if ((console->hSocket == -1) && (console->session == NULL) && (console->output == NULL))
   {
		WriteToTerminal(szBuffer);
   }
   else if (console->output != NULL)
   {
      // remove possible escape sequences
      for(int i = 0; szBuffer[i] != 0; i++)
      {
         if (szBuffer[i] == 27)
         {
            int start = i++;
            if (szBuffer[i] == '[')
            {
               for(i++; (szBuffer[i] != 0) && (szBuffer[i] != 'm'); i++);
               if (szBuffer[i] == 'm')
                  i++;
            }
            memmove(&szBuffer[start], &szBuffer[i], (_tcslen(&szBuffer[i]) + 1) * sizeof(TCHAR));
            i = start - 1;
         }
      }

      MutexLock(console->socketMutex);
      (*console->output).append(szBuffer);
      MutexUnlock(console->socketMutex);
   }
   else
   {
      console->pMsg->setField(VID_MESSAGE, szBuffer);
		if (console->session != NULL)
		{
			console->session->postMessage(console->pMsg);
		}
		else
		{
			NXCP_MESSAGE *pRawMsg = console->pMsg->serialize();
			SendEx(console->hSocket, pRawMsg, ntohl(pRawMsg->size), 0, console->socketMutex);
			free(pRawMsg);
		}
   }
}

/**
 * Print message to console, either local or remote
 */
void ConsoleWrite(CONSOLE_CTX console, const TCHAR *text)
{
	if ((console->hSocket == -1) && (console->session == NULL) && (console->output == NULL))
   {
		WriteToTerminal(text);
   }
   else if (console->output != NULL)
   {
      // remove possible escape sequences
      TCHAR *temp = _tcsdup(text);
      for(int i = 0; temp[i] != 0; i++)
      {
         if (temp[i] == 27)
         {
            int start = i++;
            if (temp[i] == '[')
            {
               for(i++; (temp[i] != 0) && (temp[i] != 'm'); i++);
               if (temp[i] == 'm')
                  i++;
            }
            memmove(&temp[start], &temp[i], (_tcslen(&temp[i]) + 1) * sizeof(TCHAR));
            i = start - 1;
         }
      }

      MutexLock(console->socketMutex);
      (*console->output).appendPreallocated(temp);
      MutexUnlock(console->socketMutex);
   }
   else
   {
      console->pMsg->setField(VID_MESSAGE, text);
		if (console->session != NULL)
		{
			console->session->postMessage(console->pMsg);
		}
		else
		{
			NXCP_MESSAGE *pRawMsg = console->pMsg->serialize();
			SendEx(console->hSocket, pRawMsg, ntohl(pRawMsg->size), 0, console->socketMutex);
			free(pRawMsg);
		}
   }
}

/**
 * Show server statistics
 */
static void DciCountCallback(NetObj *object, void *data)
{
	*((int *)data) += (int)((Node *)object)->getItemCount();
}

void ShowServerStats(CONSOLE_CTX console)
{
	int dciCount = 0;
	g_idxNodeById.forEach(DciCountCallback, &dciCount);
   ConsolePrintf(console, _T("Total number of objects:     %d\n")
                          _T("Number of monitored nodes:   %d\n")
                          _T("Number of collectable DCIs:  %d\n\n"),
	              g_idxObjectById.size(), g_idxNodeById.size(), dciCount);
}

/**
 * Show queue stats
 */
void ShowQueueStats(CONSOLE_CTX console, Queue *pQueue, const TCHAR *pszName)
{
   if (pQueue != NULL)
      ConsolePrintf(console, _T("%-32s : %d\n"), pszName, pQueue->size());
}

/**
 * Show queue stats
 */
void ShowThreadPoolPendingQueue(CONSOLE_CTX console, ThreadPool *p, const TCHAR *pszName)
{
   ThreadPoolInfo info;
   ThreadPoolGetInfo(p, &info);
   int size = (info.activeRequests > info.curThreads) ? info.activeRequests - info.curThreads : 0;
   ConsolePrintf(console, _T("%-32s : %d\n"), pszName, size);
}

/**
 * Show thread pool stats
 */
void ShowThreadPool(CONSOLE_CTX console, ThreadPool *p)
{
   ThreadPoolInfo info;
   ThreadPoolGetInfo(p, &info);
   ConsolePrintf(console, _T("\x1b[1m%s\x1b[0m\n")
                          _T("   Threads:      %d (%d/%d)\n")
                          _T("   Load average: %0.2f %0.2f %0.2f\n")
                          _T("   Current load: %d%%\n")
                          _T("   Usage:        %d%%\n")
                          _T("   Requests:     %d\n\n"),
                 info.name, info.curThreads, info.minThreads, info.maxThreads, 
                 info.loadAvg[0], info.loadAvg[1], info.loadAvg[2],
                 info.load, info.usage, info.activeRequests);
}

/**
 * Get thread pool stat (for internal DCI)
 */
LONG GetThreadPoolStat(ThreadPoolStat stat, const TCHAR *param, TCHAR *value)
{
   TCHAR poolName[64], options[64];
   if (!AgentGetParameterArg(param, 1, poolName, 64) ||
       !AgentGetParameterArg(param, 2, options, 64))
      return SYSINFO_RC_UNSUPPORTED;

   ThreadPoolInfo info;
   if (!ThreadPoolGetInfo(poolName, &info))
      return SYSINFO_RC_UNSUPPORTED;

   switch(stat)
   {
      case THREAD_POOL_CURR_SIZE:
         ret_int(value, info.curThreads);
         break;
      case THREAD_POOL_LOAD:
         ret_int(value, info.load);
         break;
      case THREAD_POOL_LOADAVG_1:
         if ((options[0] != 0) && _tcstol(options, NULL, 10))
            ret_double(value, info.loadAvg[0] / info.maxThreads, 2);
         else
            ret_double(value, info.loadAvg[0], 2);
         break;
      case THREAD_POOL_LOADAVG_5:
         if ((options[0] != 0) && _tcstol(options, NULL, 10))
            ret_double(value, info.loadAvg[1] / info.maxThreads, 2);
         else
            ret_double(value, info.loadAvg[1], 2);
         break;
      case THREAD_POOL_LOADAVG_15:
         if ((options[0] != 0) && _tcstol(options, NULL, 10))
            ret_double(value, info.loadAvg[2] / info.maxThreads, 2);
         else
            ret_double(value, info.loadAvg[2], 2);
         break;
      case THREAD_POOL_MAX_SIZE:
         ret_int(value, info.maxThreads);
         break;
      case THREAD_POOL_MIN_SIZE:
         ret_int(value, info.minThreads);
         break;
      case THREAD_POOL_REQUESTS:
         ret_int(value, info.activeRequests);
         break;
      case THREAD_POOL_USAGE:
         ret_int(value, info.usage);
         break;
      default:
         return SYSINFO_RC_UNSUPPORTED;
   }
   return SYSINFO_RC_SUCCESS;
}

/**
 * Write process coredump
 */
#ifdef _WIN32

void DumpProcess(CONSOLE_CTX console)
{
	ConsolePrintf(console, _T("Dumping process to disk...\n"));

	TCHAR cmdLine[MAX_PATH + 64];
	_sntprintf(cmdLine, MAX_PATH + 64, _T("netxmsd.exe --dump-dir \"%s\" --dump %d"), g_szDumpDir, GetCurrentProcessId());

	PROCESS_INFORMATION pi;
   STARTUPINFO si;
	memset(&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	if (CreateProcess(NULL, cmdLine, NULL, NULL, FALSE,
            (g_flags & AF_DAEMON) ? CREATE_NO_WINDOW : 0, NULL, NULL, &si, &pi))
	{
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		
		ConsolePrintf(console, _T("Done.\n"));
	}
	else
	{
		TCHAR buffer[256];
		ConsolePrintf(console, _T("Dump error: CreateProcess() failed (%s)\n"), GetSystemErrorText(GetLastError(), buffer, 256));
	}
}

#else

void DumpProcess(CONSOLE_CTX console)
{
	ConsolePrintf(console, _T("DUMP command is not supported for current operating system\n"));
}

#endif
