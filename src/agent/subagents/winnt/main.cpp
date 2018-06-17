/*
** Windows platform subagent
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
** File: main.cpp
**
**/

#include "winnt_subagent.h"
#include "lm.h"

/**
 * Externlals
 */
LONG H_ActiveUserSessions(const TCHAR *cmd, const TCHAR *arg, StringList *value, AbstractCommSession *session);
LONG H_AgentDesktop(const TCHAR *cmd, const TCHAR *arg, TCHAR *value, AbstractCommSession *session);
LONG H_AppAddressSpace(const TCHAR *pszCmd, const TCHAR *pArg, TCHAR *pValue, AbstractCommSession *session);
LONG H_ArpCache(const TCHAR *cmd, const TCHAR *arg, StringList *value, AbstractCommSession *session);
LONG H_ConnectedUsers(const TCHAR *pszCmd, const TCHAR *pArg, TCHAR *pValue, AbstractCommSession *session);
LONG H_CpuContextSwitches(const TCHAR *param, const TCHAR *arg, TCHAR *value, AbstractCommSession *session);
LONG H_CpuInterrupts(const TCHAR *param, const TCHAR *arg, TCHAR *value, AbstractCommSession *session);
LONG H_CpuUsage(const TCHAR *param, const TCHAR *arg, TCHAR *value, AbstractCommSession *session);
LONG H_Desktops(const TCHAR *cmd, const TCHAR *arg, StringList *value, AbstractCommSession *session);
LONG H_HandleCount(const TCHAR *cmd, const TCHAR *arg, TCHAR *value, AbstractCommSession *session);
LONG H_InstalledProducts(const TCHAR *cmd, const TCHAR *arg, Table *value, AbstractCommSession *);
LONG H_InterfaceList(const TCHAR *cmd, const TCHAR *arg, StringList *value, AbstractCommSession *session);
LONG H_InterfaceNames(const TCHAR *cmd, const TCHAR *arg, StringList *value, AbstractCommSession *session);
LONG H_IPRoutingTable(const TCHAR *cmd, const TCHAR *arg, StringList *pValue, AbstractCommSession *session);
LONG H_NetInterface64bitSupport(const TCHAR *cmd, const TCHAR *arg, TCHAR *value, AbstractCommSession *session);
LONG H_NetInterfaceStats(const TCHAR *cmd, const TCHAR *arg, TCHAR *value, AbstractCommSession *session);
LONG H_NetIPStats(const TCHAR *cmd, const TCHAR *arg, TCHAR *value, AbstractCommSession *session);
LONG H_RemoteShareStatus(const TCHAR *pszCmd, const TCHAR *pArg, TCHAR *pValue, AbstractCommSession *session);
LONG H_ProcessList(const TCHAR *cmd, const TCHAR *arg, StringList *value, AbstractCommSession *session);
LONG H_ProcessTable(const TCHAR *cmd, const TCHAR *arg, Table *value, AbstractCommSession *);
LONG H_ProcCount(const TCHAR *cmd, const TCHAR *arg, TCHAR *value, AbstractCommSession *session);
LONG H_ProcCountSpecific(const TCHAR *cmd, const TCHAR *arg, TCHAR *value, AbstractCommSession *session);
LONG H_ProcInfo(const TCHAR *cmd, const TCHAR *arg, TCHAR *value, AbstractCommSession *session);
LONG H_ServiceList(const TCHAR *pszCmd, const TCHAR *pArg, StringList *value, AbstractCommSession *session);
LONG H_ServiceState(const TCHAR *cmd, const TCHAR *arg, TCHAR *value, AbstractCommSession *session);
LONG H_ServiceTable(const TCHAR *pszCmd, const TCHAR *pArg, Table *value, AbstractCommSession *session);
LONG H_SysUpdateTime(const TCHAR *cmd, const TCHAR *arg, TCHAR *value, AbstractCommSession *session);
LONG H_ThreadCount(const TCHAR *cmd, const TCHAR *arg, TCHAR *value, AbstractCommSession *session);
LONG H_WindowStations(const TCHAR *cmd, const TCHAR *arg, StringList *value, AbstractCommSession *session);

void StartCPUStatCollector();
void StopCPUStatCollector();

/**
 * Optional imports
 */
DWORD (__stdcall *imp_GetIfEntry2)(PMIB_IF_ROW2) = NULL;

/**
 * Windows XP/Windows Server 2003 flag
 */
bool g_isWin5 = false;

/**
 * Import symbols
 */
static void ImportSymbols()
{
   HMODULE hModule;

   // IPHLPAPI.DLL
   hModule = LoadLibrary(_T("IPHLPAPI.DLL"));
   if (hModule != NULL)
   {
      imp_GetIfEntry2 = (DWORD (__stdcall *)(PMIB_IF_ROW2))GetProcAddress(hModule, "GetIfEntry2");
   }
   else
   {
		AgentWriteLog(NXLOG_WARNING, _T("Unable to load IPHLPAPI.DLL"));
   }
}

/**
 * Set or clear current privilege
 */
static BOOL SetCurrentPrivilege(LPCTSTR pszPrivilege, BOOL bEnablePrivilege)
{
	HANDLE hToken;
	LUID luid;
	TOKEN_PRIVILEGES tp, tpPrevious;
	DWORD cbPrevious = sizeof(TOKEN_PRIVILEGES);
	BOOL bSuccess = FALSE;

	if (!LookupPrivilegeValue(NULL, pszPrivilege, &luid))
		return FALSE;

	if(!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &hToken))
		return FALSE;

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = 0;

	if (AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES),
				&tpPrevious, &cbPrevious))
	{
		tpPrevious.PrivilegeCount = 1;
		tpPrevious.Privileges[0].Luid = luid;

		if (bEnablePrivilege)
			tpPrevious.Privileges[0].Attributes |= SE_PRIVILEGE_ENABLED;
		else
			tpPrevious.Privileges[0].Attributes &= ~SE_PRIVILEGE_ENABLED;

		bSuccess = AdjustTokenPrivileges(hToken, FALSE, &tpPrevious, cbPrevious, NULL, NULL);
	}

	CloseHandle(hToken);

	return bSuccess;
}

/**
 * Shutdown system
 */
static LONG H_ActionShutdown(const TCHAR *action, StringList *args, const TCHAR *data, AbstractCommSession *session)
{
	LONG nRet = ERR_INTERNAL_ERROR;

	if (SetCurrentPrivilege(SE_SHUTDOWN_NAME, TRUE))
	{
		if (InitiateSystemShutdown(NULL, NULL, 0, TRUE, (*data == _T('R')) ? TRUE : FALSE))
			nRet = ERR_SUCCESS;
	}
	return nRet;
}

/**
 * Change user's password
 * Parameters: user new_password
 */
static LONG H_ChangeUserPassword(const TCHAR *action, StringList *args, const TCHAR *data, AbstractCommSession *session)
{
   if (args->size() < 2)
	   return ERR_INTERNAL_ERROR;

   USER_INFO_1003 ui;
   ui.usri1003_password = (TCHAR *)args->get(1);
   NET_API_STATUS status = NetUserSetInfo(NULL, args->get(0), 1003, (LPBYTE)&ui, NULL);
   AgentWriteDebugLog(2, _T("WINNT: change password for user %s status=%d"), args->get(0), status);
   return (status == NERR_Success) ? ERR_SUCCESS : ERR_INTERNAL_ERROR; 
}

/**
 * Subagent initialization
 */
static BOOL SubAgentInit(Config *config)
{
   StartCPUStatCollector();
   return true;
}

/**
 * Called by master agent at unload
 */
static void SubAgentShutdown()
{
   StopCPUStatCollector();
}

/**
 * Supported parameters
 */
static NETXMS_SUBAGENT_PARAM m_parameters[] =
{
   { _T("Agent.Desktop"), H_AgentDesktop, NULL, DCI_DT_STRING, _T("Desktop associated with agent process") },

   { _T("Net.Interface.64BitCounters"), H_NetInterface64bitSupport, NULL, DCI_DT_INT, DCIDESC_NET_INTERFACE_64BITCOUNTERS },
   { _T("Net.Interface.AdminStatus(*)"), H_NetInterfaceStats, (TCHAR *)NETINFO_IF_ADMIN_STATUS, DCI_DT_INT, DCIDESC_NET_INTERFACE_ADMINSTATUS },
   { _T("Net.Interface.BytesIn(*)"), H_NetInterfaceStats, (TCHAR *)NETINFO_IF_BYTES_IN, DCI_DT_UINT, DCIDESC_NET_INTERFACE_BYTESIN },
   { _T("Net.Interface.BytesIn64(*)"), H_NetInterfaceStats, (TCHAR *)NETINFO_IF_BYTES_IN_64, DCI_DT_UINT64, DCIDESC_NET_INTERFACE_BYTESIN },
   { _T("Net.Interface.BytesOut(*)"), H_NetInterfaceStats, (TCHAR *)NETINFO_IF_BYTES_OUT, DCI_DT_UINT, DCIDESC_NET_INTERFACE_BYTESOUT },
   { _T("Net.Interface.BytesOut64(*)"), H_NetInterfaceStats, (TCHAR *)NETINFO_IF_BYTES_OUT_64, DCI_DT_UINT64, DCIDESC_NET_INTERFACE_BYTESOUT },
   { _T("Net.Interface.Description(*)"), H_NetInterfaceStats, (TCHAR *)NETINFO_IF_DESCR, DCI_DT_STRING, DCIDESC_NET_INTERFACE_DESCRIPTION },
   { _T("Net.Interface.InErrors(*)"), H_NetInterfaceStats, (TCHAR *)NETINFO_IF_IN_ERRORS, DCI_DT_UINT, DCIDESC_NET_INTERFACE_INERRORS },
   { _T("Net.Interface.Link(*)"), H_NetInterfaceStats, (TCHAR *)NETINFO_IF_OPER_STATUS, DCI_DT_DEPRECATED, DCIDESC_DEPRECATED },
   { _T("Net.Interface.MTU(*)"), H_NetInterfaceStats, (TCHAR *)NETINFO_IF_MTU, DCI_DT_UINT, DCIDESC_NET_INTERFACE_MTU },
   { _T("Net.Interface.OperStatus(*)"), H_NetInterfaceStats, (TCHAR *)NETINFO_IF_OPER_STATUS, DCI_DT_INT, DCIDESC_NET_INTERFACE_OPERSTATUS },
   { _T("Net.Interface.OutErrors(*)"), H_NetInterfaceStats, (TCHAR *)NETINFO_IF_OUT_ERRORS, DCI_DT_UINT, DCIDESC_NET_INTERFACE_OUTERRORS },
   { _T("Net.Interface.PacketsIn(*)"), H_NetInterfaceStats, (TCHAR *)NETINFO_IF_PACKETS_IN, DCI_DT_UINT, DCIDESC_NET_INTERFACE_PACKETSIN },
   { _T("Net.Interface.PacketsIn64(*)"), H_NetInterfaceStats, (TCHAR *)NETINFO_IF_PACKETS_IN_64, DCI_DT_UINT64, DCIDESC_NET_INTERFACE_PACKETSIN },
   { _T("Net.Interface.PacketsOut(*)"), H_NetInterfaceStats, (TCHAR *)NETINFO_IF_PACKETS_OUT, DCI_DT_UINT, DCIDESC_NET_INTERFACE_PACKETSOUT },
   { _T("Net.Interface.PacketsOut64(*)"), H_NetInterfaceStats, (TCHAR *)NETINFO_IF_PACKETS_OUT_64, DCI_DT_UINT64, DCIDESC_NET_INTERFACE_PACKETSOUT },
   { _T("Net.Interface.Speed(*)"), H_NetInterfaceStats, (TCHAR *)NETINFO_IF_SPEED, DCI_DT_UINT64, DCIDESC_NET_INTERFACE_SPEED },

   { _T("Net.IP.Forwarding"), H_NetIPStats, (TCHAR *)NETINFO_IP_FORWARDING, DCI_DT_INT, DCIDESC_NET_IP_FORWARDING },

	{ _T("Net.RemoteShareStatus(*)"), H_RemoteShareStatus, _T("C"), DCI_DT_INT, _T("Status of remote shared resource") },
	{ _T("Net.RemoteShareStatusText(*)"), H_RemoteShareStatus, _T("T"), DCI_DT_STRING, _T("Status of remote shared resource as text") },

	{ _T("Process.Count(*)"), H_ProcCountSpecific, _T("N"), DCI_DT_INT, DCIDESC_PROCESS_COUNT },
	{ _T("Process.CountEx(*)"), H_ProcCountSpecific, _T("E"), DCI_DT_INT, DCIDESC_PROCESS_COUNTEX },
	{ _T("Process.CPUTime(*)"), H_ProcInfo, (TCHAR *)PROCINFO_CPUTIME, DCI_DT_UINT64, DCIDESC_PROCESS_CPUTIME },
	{ _T("Process.GDIObjects(*)"), H_ProcInfo, (TCHAR *)PROCINFO_GDI_OBJ, DCI_DT_UINT64, DCIDESC_PROCESS_GDIOBJ },
	{ _T("Process.Handles(*)"), H_ProcInfo, (TCHAR *)PROCINFO_HANDLES, DCI_DT_UINT, DCIDESC_PROCESS_HANDLES },
	{ _T("Process.IO.OtherB(*)"), H_ProcInfo, (TCHAR *)PROCINFO_IO_OTHER_B, DCI_DT_UINT64, DCIDESC_PROCESS_IO_OTHERB },
	{ _T("Process.IO.OtherOp(*)"), H_ProcInfo, (TCHAR *)PROCINFO_IO_OTHER_OP, DCI_DT_UINT64, DCIDESC_PROCESS_IO_OTHEROP },
	{ _T("Process.IO.ReadB(*)"), H_ProcInfo, (TCHAR *)PROCINFO_IO_READ_B, DCI_DT_UINT64, DCIDESC_PROCESS_IO_READB },
	{ _T("Process.IO.ReadOp(*)"), H_ProcInfo, (TCHAR *)PROCINFO_IO_READ_OP, DCI_DT_UINT64, DCIDESC_PROCESS_IO_READOP },
	{ _T("Process.IO.WriteB(*)"), H_ProcInfo, (TCHAR *)PROCINFO_IO_WRITE_B, DCI_DT_UINT64, DCIDESC_PROCESS_IO_WRITEB },
	{ _T("Process.IO.WriteOp(*)"), H_ProcInfo, (TCHAR *)PROCINFO_IO_WRITE_OP, DCI_DT_UINT64, DCIDESC_PROCESS_IO_WRITEOP },
	{ _T("Process.KernelTime(*)"), H_ProcInfo, (TCHAR *)PROCINFO_KTIME, DCI_DT_UINT64, DCIDESC_PROCESS_KERNELTIME },
	{ _T("Process.PageFaults(*)"), H_ProcInfo, (TCHAR *)PROCINFO_PF, DCI_DT_UINT64, DCIDESC_PROCESS_PAGEFAULTS },
	{ _T("Process.UserObjects(*)"), H_ProcInfo, (TCHAR *)PROCINFO_USER_OBJ, DCI_DT_UINT64, DCIDESC_PROCESS_USEROBJ },
	{ _T("Process.UserTime(*)"), H_ProcInfo, (TCHAR *)PROCINFO_UTIME, DCI_DT_UINT64, DCIDESC_PROCESS_USERTIME },
	{ _T("Process.VMSize(*)"), H_ProcInfo, (TCHAR *)PROCINFO_VMSIZE, DCI_DT_UINT64, DCIDESC_PROCESS_VMSIZE },
	{ _T("Process.WkSet(*)"), H_ProcInfo, (TCHAR *)PROCINFO_WKSET, DCI_DT_UINT64, DCIDESC_PROCESS_WKSET },

	{ _T("System.AppAddressSpace"), H_AppAddressSpace, NULL, DCI_DT_UINT, DCIDESC_SYSTEM_APPADDRESSSPACE },
	{ _T("System.ConnectedUsers"), H_ConnectedUsers, NULL, DCI_DT_INT, DCIDESC_SYSTEM_CONNECTEDUSERS },

	{ _T("System.CPU.Interrupts"), H_CpuInterrupts, _T("T"), DCI_DT_UINT, DCIDESC_SYSTEM_CPU_INTERRUPTS },
	{ _T("System.CPU.Interrupts(*)"), H_CpuInterrupts, _T("C"), DCI_DT_UINT, DCIDESC_SYSTEM_CPU_INTERRUPTS_EX },
	{ _T("System.CPU.ContextSwitches"), H_CpuContextSwitches, NULL, DCI_DT_UINT, DCIDESC_SYSTEM_CPU_CONTEXT_SWITCHES },
   
   { _T("System.CPU.CurrentUsage"), H_CpuUsage, _T("T0U"), DCI_DT_FLOAT, DCIDESC_SYSTEM_CPU_USAGECURR },
   { _T("System.CPU.CurrentUsage.Idle"), H_CpuUsage, _T("T0I"), DCI_DT_FLOAT, DCIDESC_SYSTEM_CPU_USAGECURR_IDLE },
   { _T("System.CPU.CurrentUsage.Irq"), H_CpuUsage, _T("T0q"), DCI_DT_FLOAT, DCIDESC_SYSTEM_CPU_USAGECURR_IRQ },
   { _T("System.CPU.CurrentUsage.System"), H_CpuUsage, _T("T0s"), DCI_DT_FLOAT, DCIDESC_SYSTEM_CPU_USAGECURR_SYSTEM },
   { _T("System.CPU.CurrentUsage.User"), H_CpuUsage, _T("T0u"), DCI_DT_FLOAT, DCIDESC_SYSTEM_CPU_USAGECURR_USER },

   { _T("System.CPU.CurrentUsage(*)"), H_CpuUsage, _T("C0U"), DCI_DT_FLOAT, DCIDESC_SYSTEM_CPU_USAGECURR_EX },
   { _T("System.CPU.CurrentUsage.Idle(*)"), H_CpuUsage, _T("C0I"), DCI_DT_FLOAT, DCIDESC_SYSTEM_CPU_USAGECURR_IDLE_EX },
   { _T("System.CPU.CurrentUsage.Irq(*)"), H_CpuUsage, _T("C0q"), DCI_DT_FLOAT, DCIDESC_SYSTEM_CPU_USAGECURR_IRQ_EX },
   { _T("System.CPU.CurrentUsage.System(*)"), H_CpuUsage, _T("C0s"), DCI_DT_FLOAT, DCIDESC_SYSTEM_CPU_USAGECURR_SYSTEM_EX },
   { _T("System.CPU.CurrentUsage.User(*)"), H_CpuUsage, _T("C0u"), DCI_DT_FLOAT, DCIDESC_SYSTEM_CPU_USAGECURR_USER_EX },

   { _T("System.CPU.Usage"), H_CpuUsage, _T("T1U"), DCI_DT_FLOAT, DCIDESC_SYSTEM_CPU_USAGE },
   { _T("System.CPU.Usage.Idle"), H_CpuUsage, _T("T1I"), DCI_DT_FLOAT, DCIDESC_SYSTEM_CPU_USAGE_IDLE },
   { _T("System.CPU.Usage.Irq"), H_CpuUsage, _T("T1q"), DCI_DT_FLOAT, DCIDESC_SYSTEM_CPU_USAGE_IRQ },
   { _T("System.CPU.Usage.System"), H_CpuUsage, _T("T1s"), DCI_DT_FLOAT, DCIDESC_SYSTEM_CPU_USAGE_SYSTEM },
   { _T("System.CPU.Usage.User"), H_CpuUsage, _T("T1u"), DCI_DT_FLOAT, DCIDESC_SYSTEM_CPU_USAGE_USER },
	
   { _T("System.CPU.Usage(*)"), H_CpuUsage, _T("C1U"), DCI_DT_FLOAT, DCIDESC_SYSTEM_CPU_USAGE_EX },
   { _T("System.CPU.Usage.Idle(*)"), H_CpuUsage, _T("C1I"), DCI_DT_FLOAT, DCIDESC_SYSTEM_CPU_USAGE_IDLE_EX },
   { _T("System.CPU.Usage.Irq(*)"), H_CpuUsage, _T("C1q"), DCI_DT_FLOAT, DCIDESC_SYSTEM_CPU_USAGE_IRQ_EX },
   { _T("System.CPU.Usage.System(*)"), H_CpuUsage, _T("C1s"), DCI_DT_FLOAT, DCIDESC_SYSTEM_CPU_USAGE_SYSTEM_EX },
   { _T("System.CPU.Usage.User(*)"), H_CpuUsage, _T("C1u"), DCI_DT_FLOAT, DCIDESC_SYSTEM_CPU_USAGE_USER_EX },
   
   { _T("System.CPU.Usage5"), H_CpuUsage, _T("T2U"), DCI_DT_FLOAT, DCIDESC_SYSTEM_CPU_USAGE5 },
   { _T("System.CPU.Usage5.Idle"), H_CpuUsage, _T("T2I"), DCI_DT_FLOAT, DCIDESC_SYSTEM_CPU_USAGE5_IDLE },
   { _T("System.CPU.Usage5.Irq"), H_CpuUsage, _T("T2q"), DCI_DT_FLOAT, DCIDESC_SYSTEM_CPU_USAGE5_IRQ },
   { _T("System.CPU.Usage5.System"), H_CpuUsage, _T("T2s"), DCI_DT_FLOAT, DCIDESC_SYSTEM_CPU_USAGE5_SYSTEM },
   { _T("System.CPU.Usage5.User"), H_CpuUsage, _T("T2u"), DCI_DT_FLOAT, DCIDESC_SYSTEM_CPU_USAGE5_USER },
   
   { _T("System.CPU.Usage5(*)"), H_CpuUsage, _T("C2U"), DCI_DT_FLOAT, DCIDESC_SYSTEM_CPU_USAGE5_EX },
   { _T("System.CPU.Usage5.Idle(*)"), H_CpuUsage, _T("C2I"), DCI_DT_FLOAT, DCIDESC_SYSTEM_CPU_USAGE5_IDLE_EX },
   { _T("System.CPU.Usage5.Irq(*)"), H_CpuUsage, _T("C2q"), DCI_DT_FLOAT, DCIDESC_SYSTEM_CPU_USAGE5_IRQ_EX },
   { _T("System.CPU.Usage5.System(*)"), H_CpuUsage, _T("C2s"), DCI_DT_FLOAT, DCIDESC_SYSTEM_CPU_USAGE5_SYSTEM_EX },
   { _T("System.CPU.Usage5.User(*)"), H_CpuUsage, _T("C2u"), DCI_DT_FLOAT, DCIDESC_SYSTEM_CPU_USAGE5_USER_EX },
   
   { _T("System.CPU.Usage15"), H_CpuUsage, _T("T3U"), DCI_DT_FLOAT, DCIDESC_SYSTEM_CPU_USAGE15 },
   { _T("System.CPU.Usage15.Idle"), H_CpuUsage, _T("T3I"), DCI_DT_FLOAT, DCIDESC_SYSTEM_CPU_USAGE15_IDLE },
   { _T("System.CPU.Usage15.Irq"), H_CpuUsage, _T("T3q"), DCI_DT_FLOAT, DCIDESC_SYSTEM_CPU_USAGE15_IRQ },
   { _T("System.CPU.Usage15.System"), H_CpuUsage, _T("T3s"), DCI_DT_FLOAT, DCIDESC_SYSTEM_CPU_USAGE15_SYSTEM },
   { _T("System.CPU.Usage15.User"), H_CpuUsage, _T("T3u"), DCI_DT_FLOAT, DCIDESC_SYSTEM_CPU_USAGE15_USER },
   
   { _T("System.CPU.Usage15(*)"), H_CpuUsage, _T("C3U"), DCI_DT_FLOAT, DCIDESC_SYSTEM_CPU_USAGE15_EX },
   { _T("System.CPU.Usage15.Idle(*)"), H_CpuUsage, _T("C3I"), DCI_DT_FLOAT, DCIDESC_SYSTEM_CPU_USAGE15_IDLE_EX },
   { _T("System.CPU.Usage15.Irq(*)"), H_CpuUsage, _T("C3q"), DCI_DT_FLOAT, DCIDESC_SYSTEM_CPU_USAGE15_IRQ_EX },
   { _T("System.CPU.Usage15.System(*)"), H_CpuUsage, _T("C3s"), DCI_DT_FLOAT, DCIDESC_SYSTEM_CPU_USAGE15_SYSTEM_EX },
   { _T("System.CPU.Usage15.User(*)"), H_CpuUsage, _T("C3u"), DCI_DT_FLOAT, DCIDESC_SYSTEM_CPU_USAGE15_USER_EX },
	
   { _T("System.HandleCount"), H_HandleCount, NULL, DCI_DT_UINT, DCIDESC_SYSTEM_HANDLECOUNT },
	{ _T("System.ProcessCount"), H_ProcCount, NULL, DCI_DT_UINT, DCIDESC_SYSTEM_PROCESSCOUNT },
	{ _T("System.ServiceState(*)"), H_ServiceState, NULL, DCI_DT_INT, DCIDESC_SYSTEM_SERVICESTATE },
	{ _T("System.ThreadCount"), H_ThreadCount, NULL, DCI_DT_UINT, DCIDESC_SYSTEM_THREADCOUNT },
   { _T("System.Update.LastDetectTime"), H_SysUpdateTime, _T("Detect"), DCI_DT_INT64, _T("System update: last detect time") },
   { _T("System.Update.LastDownloadTime"), H_SysUpdateTime, _T("Download"), DCI_DT_INT64, _T("System update: last download time") },
   { _T("System.Update.LastInstallTime"), H_SysUpdateTime, _T("Install"), DCI_DT_INT64, _T("System update: last install time") }
};

/**
 * Supported lists
 */
static NETXMS_SUBAGENT_LIST m_lists[] =
{
   { _T("Net.ArpCache"), H_ArpCache, NULL },
   { _T("Net.InterfaceList"), H_InterfaceList, NULL },
   { _T("Net.InterfaceNames"), H_InterfaceNames, NULL },
   { _T("Net.IP.RoutingTable"), H_IPRoutingTable, NULL },
	{ _T("System.ActiveUserSessions"), H_ActiveUserSessions, NULL },
	{ _T("System.Desktops(*)"), H_Desktops, NULL },
	{ _T("System.ProcessList"), H_ProcessList, NULL },
	{ _T("System.Services"), H_ServiceList, NULL },
	{ _T("System.WindowStations"), H_WindowStations, NULL }
};

/**
 * Supported tables
 */
static NETXMS_SUBAGENT_TABLE m_tables[] =
{
	{ _T("System.InstalledProducts"), H_InstalledProducts, NULL, _T("NAME"), DCTDESC_SYSTEM_INSTALLED_PRODUCTS },
	{ _T("System.Processes"), H_ProcessTable, NULL, _T("PID"), DCTDESC_SYSTEM_PROCESSES },
	{ _T("System.Services"), H_ServiceTable, NULL, _T("Name"), _T("Services") }
};

/**
 * Supported actions
 */
static NETXMS_SUBAGENT_ACTION m_actions[] =
{
	{ _T("System.Restart"), H_ActionShutdown, _T("R"), _T("Restart system") },
	{ _T("System.Shutdown"), H_ActionShutdown, _T("S"), _T("Shutdown system") },
	{ _T("User.ChangePassword"), H_ChangeUserPassword, NULL, _T("Change password for given user") }
};

/**
 * Subagent information
 */
static NETXMS_SUBAGENT_INFO m_info =
{
	NETXMS_SUBAGENT_INFO_MAGIC,
	_T("WinNT"), NETXMS_VERSION_STRING,
	SubAgentInit, SubAgentShutdown, NULL,
	sizeof(m_parameters) / sizeof(NETXMS_SUBAGENT_PARAM),
	m_parameters,
	sizeof(m_lists) / sizeof(NETXMS_SUBAGENT_LIST),
	m_lists,
	sizeof(m_tables) / sizeof(NETXMS_SUBAGENT_TABLE),
	m_tables,
	sizeof(m_actions) / sizeof(NETXMS_SUBAGENT_ACTION),
	m_actions,
	0, NULL	// push parameters
};

/**
 * Entry point for NetXMS agent
 */
DECLARE_SUBAGENT_ENTRY_POINT(WINNT)
{
	*ppInfo = &m_info;
	ImportSymbols();

   OSVERSIONINFO ver;
   ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
   if (GetVersionEx(&ver))
   {
      if (ver.dwMajorVersion < 6)
         g_isWin5 = true;
   }
	return TRUE;
}

/**
 * DLL entry point
 */
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
		DisableThreadLibraryCalls(hInstance);
	return TRUE;
}
