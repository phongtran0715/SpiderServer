/*
** NetXMS - Network Management System
** NetXMS Foundation Library
** Copyright (C) 2003-2017 Victor Kirhenshtein
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published
** by the Free Software Foundation; either version 3 of the License, or
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
** File: nxproc_win32.cpp
**
**/

#include "libnetxms.h"
#include <nxproc.h>
#include <aclapi.h>

/**
 * Create listener end for named pipe
 */
NamedPipeListener *NamedPipeListener::create(const TCHAR *name, NamedPipeRequestHandler reqHandler, void *userArg, const TCHAR *user)
{
   NamedPipeListener *listener = NULL;

	PSID sidEveryone = NULL;
	ACL *acl = NULL;
	PSECURITY_DESCRIPTOR sd = NULL;

	TCHAR errorText[1024];

	// Create a well-known SID for the Everyone group.
	SID_IDENTIFIER_AUTHORITY sidAuthWorld = SECURITY_WORLD_SID_AUTHORITY;
	if (!AllocateAndInitializeSid(&sidAuthWorld, 1, SECURITY_WORLD_RID, 0, 0, 0, 0, 0, 0, 0, &sidEveryone))
	{
		nxlog_debug(2, _T("NamedPipeListener(%s): AllocateAndInitializeSid failed (%s)"), name, GetSystemErrorText(GetLastError(), errorText, 1024));
		goto cleanup;
	}

	// Initialize an EXPLICIT_ACCESS structure for an ACE.
	// The ACE will allow either Everyone or given user to access pipe
	EXPLICIT_ACCESS ea;
	ZeroMemory(&ea, sizeof(EXPLICIT_ACCESS));
	ea.grfAccessPermissions = (FILE_GENERIC_READ | FILE_GENERIC_WRITE) & ~FILE_CREATE_PIPE_INSTANCE;
	ea.grfAccessMode = SET_ACCESS;
	ea.grfInheritance = NO_INHERITANCE;
	if ((user == NULL) || (user[0] == 0) || !_tcscmp(user, _T("*")))
	{
		ea.Trustee.TrusteeForm = TRUSTEE_IS_SID;
		ea.Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
		ea.Trustee.ptstrName  = (LPTSTR)sidEveryone;
	}
	else
	{
		ea.Trustee.TrusteeForm = TRUSTEE_IS_NAME;
		ea.Trustee.TrusteeType = TRUSTEE_IS_USER;
		ea.Trustee.ptstrName  = (LPTSTR)user;
		nxlog_debug(2, _T("NamedPipeListener(%s): will allow connections only for user %s"), name, user);
	}

	// Create a new ACL that contains the new ACEs.
	if (SetEntriesInAcl(1, &ea, NULL, &acl) != ERROR_SUCCESS)
	{
		nxlog_debug(2, _T("NamedPipeListener(%s): SetEntriesInAcl failed (%s)"), name, GetSystemErrorText(GetLastError(), errorText, 1024));
		goto cleanup;
	}

	sd = (PSECURITY_DESCRIPTOR)LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
	if (sd == NULL)
	{
		nxlog_debug(2, _T("NamedPipeListener(%s): LocalAlloc failed (%s)"), name, GetSystemErrorText(GetLastError(), errorText, 1024));
		goto cleanup;
	}

	if (!InitializeSecurityDescriptor(sd, SECURITY_DESCRIPTOR_REVISION))
	{
		nxlog_debug(2, _T("NamedPipeListener(%s): InitializeSecurityDescriptor failed (%s)"), name, GetSystemErrorText(GetLastError(), errorText, 1024));
		goto cleanup;
	}

	// Add the ACL to the security descriptor. 
   if (!SetSecurityDescriptorDacl(sd, TRUE, acl, FALSE))
	{
		nxlog_debug(2, _T("NamedPipeListener(%s): SetSecurityDescriptorDacl failed (%s)"), name, GetSystemErrorText(GetLastError(), errorText, 1024));
		goto cleanup;
	}

   TCHAR path[MAX_PATH];
   _sntprintf(path, MAX_PATH, _T("\\\\.\\pipe\\%s"), name);

	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = FALSE;
	sa.lpSecurityDescriptor = sd;
	HANDLE hPipe = CreateNamedPipe(path, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE, 1, 8192, 8192, 0, &sa);
	if (hPipe == INVALID_HANDLE_VALUE)
	{
		nxlog_debug(2, _T("NamedPipeListener(%s): CreateNamedPipe failed (%s)"), name, GetSystemErrorText(GetLastError(), errorText, 1024));
		goto cleanup;
	}

   listener = new NamedPipeListener(name, hPipe, reqHandler, userArg, user);

cleanup:
	if (sd != NULL)
		LocalFree(sd);

	if (acl != NULL)
		LocalFree(acl);

	if (sidEveryone != NULL)
		FreeSid(sidEveryone);

   return listener;
}

/**
 * Pipe destructor
 */
NamedPipeListener::~NamedPipeListener()
{
   CloseHandle(m_handle);
   stop();
}

/**
 * Named pipe server thread
 */
void NamedPipeListener::serverThread()
{
   nxlog_debug(2, _T("NamedPipeListener(%s): waiting for connection"), m_name);
   while(!m_stop)
   {
      BOOL connected = ConnectNamedPipe(m_handle, NULL);
		if (connected || (GetLastError() == ERROR_PIPE_CONNECTED))
		{
         nxlog_debug(5, _T("NamedPipeListener(%s): accepted connection"), m_name);
         NamedPipe *pipe = new NamedPipe(m_name, m_handle, NULL);
         m_reqHandler(pipe, m_userArg);
         delete pipe;
		}
		else
		{
         TCHAR errorText[1024];
			nxlog_debug(2, _T("NamedPipeListener(%s): ConnectNamedPipe failed (%s)"), m_name, GetSystemErrorText(GetLastError(), errorText, 1024));
         ThreadSleep(5);
		}
   }
}

/**
 * Pipe destructor
 */
NamedPipe::~NamedPipe()
{
   DWORD flags = 0;
   GetNamedPipeInfo(m_handle, &flags, NULL, NULL, NULL);
   if (flags & PIPE_SERVER_END)
		DisconnectNamedPipe(m_handle);
   else
      CloseHandle(m_handle);
   MutexDestroy(m_writeLock);
}

/**
 * Create client end for named pipe
 */
NamedPipe *NamedPipe::connect(const TCHAR *name, UINT32 timeout)
{
   TCHAR path[MAX_PATH];
   _sntprintf(path, MAX_PATH, _T("\\\\.\\pipe\\%s"), name);

reconnect:
   HANDLE h = CreateFile(path, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (h == INVALID_HANDLE_VALUE)
	{
		if (GetLastError() == ERROR_PIPE_BUSY)
		{
			if (WaitNamedPipe(path, timeout))
				goto reconnect;
		}
		return NULL;
	}

	DWORD pipeMode = PIPE_READMODE_MESSAGE;
	SetNamedPipeHandleState(h, &pipeMode, NULL, NULL);
   return new NamedPipe(name, h, false);
}

/**
 * Write to pipe
 */
bool NamedPipe::write(const void *data, size_t size)
{
	DWORD bytes;
	if (!WriteFile(m_handle, data, (DWORD)size, &bytes, NULL))
		return false;
   return bytes == (DWORD)size;
}

/**
 * Get user name
 */
const TCHAR *NamedPipe::user()
{
   if (m_user[0] == 0)
   {
      if (!GetNamedPipeHandleState(m_handle, NULL, NULL, NULL, NULL, m_user, 64))
      {
         if (GetLastError() != ERROR_CANNOT_IMPERSONATE)
         {
            TCHAR errorText[1024];
			   nxlog_debug(5, _T("NamedPipeListener(%s): GetNamedPipeHandleState failed (%s)"), m_name, GetSystemErrorText(GetLastError(), errorText, 1024));
            _tcscpy(m_user, _T("[unknown]"));
         }
      }
   }
   return m_user;
}
