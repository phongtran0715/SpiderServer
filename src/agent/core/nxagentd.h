/*
** NetXMS multiplatform core agent
** Copyright (C) 2003-2016 Victor Kirhenshtein
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be usefu,,
** but ITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** File: nxagentd.h
**
**/

#ifndef _nxagentd_h_
#define _nxagentd_h_

#include <nms_common.h>
#include <nms_util.h>
#include <nms_threads.h>
#include <nms_agent.h>
#include <nxcpapi.h>
#include <nxqueue.h>
#include <nxlog.h>
#include <nxcldefs.h>
#include <nxdbapi.h>
#include <nxproc.h>
#include "messages.h"
#include "nxsnmp.h"
#include "localdb.h"

#ifdef _NETWARE
#undef SEVERITY_CRITICAL
#endif

#ifdef _WIN32
#include <aclapi.h>
#endif

/**
 * Default files
 */
#if defined(_WIN32)
#define AGENT_DEFAULT_CONFIG     _T("{search}")
#define AGENT_DEFAULT_CONFIG_D   _T("{search}")
#define AGENT_DEFAULT_LOG        _T("C:\\nxagentd.log")
#define AGENT_DEFAULT_FILE_STORE _T("C:\\")
#define AGENT_DEFAULT_DATA_DIR   _T("{default}")
#elif defined(_NETWARE)
#define AGENT_DEFAULT_CONFIG     "SYS:ETC/nxagentd.conf"
#define AGENT_DEFAULT_CONFIG_D   "SYS:ETC/nxagentd.conf.d"
#define AGENT_DEFAULT_LOG        "SYS:ETC/nxagentd.log"
#define AGENT_DEFAULT_FILE_STORE "SYS:\\"
#define AGENT_DEFAULT_DATA_DIR   "SYS:ETC"
#elif defined(_IPSO)
#define AGENT_DEFAULT_CONFIG     _T("/opt/netxms/etc/nxagentd.conf")
#define AGENT_DEFAULT_CONFIG_D   _T("/opt/netxms/etc/nxagentd.conf.d")
#define AGENT_DEFAULT_LOG        _T("/opt/netxms/log/nxagentd.log")
#define AGENT_DEFAULT_FILE_STORE _T("/opt/netxms/store")
#define AGENT_DEFAULT_DATA_DIR   _T("/opt/netxms/data")
#else
#define AGENT_DEFAULT_CONFIG     _T("{search}")
#define AGENT_DEFAULT_CONFIG_D   _T("{search}")
#define AGENT_DEFAULT_LOG        _T("/var/log/nxagentd")
#define AGENT_DEFAULT_FILE_STORE _T("/tmp")
#define AGENT_DEFAULT_DATA_DIR   _T("{default}")
#endif

/**
 * Constants
 */
#ifdef _WIN32
#define DEFAULT_AGENT_SERVICE_NAME    _T("NetXMSAgentdW32")
#define DEFAULT_AGENT_EVENT_SOURCE    _T("NetXMS Win32 Agent")
#define NXAGENTD_SYSLOG_NAME          g_windowsEventSourceName
#else
#define NXAGENTD_SYSLOG_NAME          _T("nxagentd")
#endif

#define MAX_PSUFFIX_LENGTH 32
#define MAX_SERVERS        32
#define MAX_ESA_USER_NAME  64
#define MAX_AGENT_MSG_SIZE 4194304

#define AF_DAEMON                   0x00000001
#define AF_USE_SYSLOG               0x00000002
#define AF_SUBAGENT_LOADER          0x00000004
#define AF_REQUIRE_AUTH             0x00000008
#define AF_LOG_UNRESOLVED_SYMBOLS   0x00000010
#define AF_ENABLE_ACTIONS           0x00000020
#define AF_REQUIRE_ENCRYPTION       0x00000040
#define AF_HIDE_WINDOW              0x00000080
#define AF_ENABLE_AUTOLOAD          0x00000100
#define AF_ENABLE_PROXY             0x00000200
#define AF_CENTRAL_CONFIG           0x00000400
#define AF_ENABLE_SNMP_PROXY        0x00000800
#define AF_SHUTDOWN                 0x00001000
#define AF_INTERACTIVE_SERVICE      0x00002000
#define AF_REGISTER                 0x00004000
#define AF_ENABLE_WATCHDOG          0x00008000
#define AF_CATCH_EXCEPTIONS         0x00010000
#define AF_WRITE_FULL_DUMP          0x00020000
#define AF_ENABLE_CONTROL_CONNECTOR 0x00040000
#define AF_DISABLE_IPV4             0x00080000
#define AF_DISABLE_IPV6             0x00100000
#define AF_ENABLE_SNMP_TRAP_PROXY   0x00200000
#define AF_BACKGROUND_LOG_WRITER    0x00400000
#define AF_ENABLE_SYSLOG_PROXY      0x00800000

// Flags for component failures
#define FAIL_OPEN_LOG               0x00000001
#define FAIL_OPEN_DATABASE          0x00000002
#define FAIL_UPGRADE_DATABASE       0x00000004

#ifdef _WIN32

/**
 * Request types for H_MemoryInfo
 */
#define MEMINFO_PHYSICAL_FREE       1
#define MEMINFO_PHYSICAL_FREE_PCT   2
#define MEMINFO_PHYSICAL_TOTAL      3
#define MEMINFO_PHYSICAL_USED       4
#define MEMINFO_PHYSICAL_USED_PCT   5
#define MEMINFO_VIRTUAL_FREE        6
#define MEMINFO_VIRTUAL_FREE_PCT    7
#define MEMINFO_VIRTUAL_TOTAL       8
#define MEMINFO_VIRTUAL_USED        9
#define MEMINFO_VIRTUAL_USED_PCT    10

/**
 * Request types for H_DiskInfo
 */
#define DISKINFO_FREE_BYTES      1
#define DISKINFO_USED_BYTES      2
#define DISKINFO_TOTAL_BYTES     3
#define DISKINFO_FREE_SPACE_PCT  4
#define DISKINFO_USED_SPACE_PCT  5

#endif   /* _WIN32 */

/**
 * Thread pool stats
 */
enum ThreadPoolStat
{
   THREAD_POOL_CURR_SIZE,
   THREAD_POOL_MIN_SIZE,
   THREAD_POOL_MAX_SIZE,
   THREAD_POOL_REQUESTS,
   THREAD_POOL_LOAD,
   THREAD_POOL_USAGE,
   THREAD_POOL_LOADAVG_1,
   THREAD_POOL_LOADAVG_5,
   THREAD_POOL_LOADAVG_15
};

/**
 * Request types for H_DirInfo
 */
#define DIRINFO_FILE_COUNT       1
#define DIRINFO_FILE_SIZE        2
#define DIRINFO_FOLDER_COUNT     3
#define DIRINFO_FILE_LINE_COUNT  4

/**
 * Request types for H_FileTime
 */
#define FILETIME_ATIME           1
#define FILETIME_MTIME           2
#define FILETIME_CTIME           3

/**
 * Action types
 */
#define AGENT_ACTION_EXEC        1
#define AGENT_ACTION_SUBAGENT    2
#define AGENT_ACTION_SHELLEXEC   3

/**
 * Action definition structure
 */
typedef struct
{
   TCHAR szName[MAX_PARAM_NAME];
   int iType;
   union
   {
      TCHAR *pszCmdLine;      // to TCHAR
      struct __subagentAction
      {
         LONG (*fpHandler)(const TCHAR *, StringList *, const TCHAR *, AbstractCommSession *);
         const TCHAR *pArg;
         TCHAR szSubagentName[MAX_PATH];
      } sa;
   } handler;
   TCHAR szDescription[MAX_DB_STRING];
} ACTION;

/**
 * Loaded subagent information
 */
struct SUBAGENT
{
   HMODULE hModule;              // Subagent's module handle
   NETXMS_SUBAGENT_INFO *pInfo;  // Information provided by subagent
   TCHAR szName[MAX_PATH];        // Name of the module  // to TCHAR by LWX
#ifdef _NETWARE
   char szEntryPoint[256];       // Name of agent's entry point
#endif
};

/**
 * External subagent information
 */
class ExternalSubagent
{
private:
   TCHAR m_name[MAX_SUBAGENT_NAME];
   TCHAR m_user[MAX_ESA_USER_NAME];
   NamedPipeListener *m_listener;
   NamedPipe *m_pipe;
   bool m_connected;
   MsgWaitQueue *m_msgQueue;
   UINT32 m_requestId;

   bool sendMessage(NXCPMessage *msg);
   NXCPMessage *waitForMessage(WORD code, UINT32 id);
   UINT32 waitForRCC(UINT32 id);
   NETXMS_SUBAGENT_PARAM *getSupportedParameters(UINT32 *count);
   NETXMS_SUBAGENT_LIST *getSupportedLists(UINT32 *count);
   NETXMS_SUBAGENT_TABLE *getSupportedTables(UINT32 *count);

public:
   ExternalSubagent(const TCHAR *name, const TCHAR *user);
   ~ExternalSubagent();

   void startListener();
   void connect(NamedPipe *pipe);

   bool isConnected() { return m_connected; }
   const TCHAR *getName() { return m_name; }
   const TCHAR *getUserName() { return m_user; }

   UINT32 getParameter(const TCHAR *name, TCHAR *buffer);
   UINT32 getTable(const TCHAR *name, Table *value);
   UINT32 getList(const TCHAR *name, StringList *value);
   void listParameters(NXCPMessage *msg, UINT32 *baseId, UINT32 *count);
   void listParameters(StringList *list);
   void listLists(NXCPMessage *msg, UINT32 *baseId, UINT32 *count);
   void listLists(StringList *list);
   void listTables(NXCPMessage *msg, UINT32 *baseId, UINT32 *count);
   void listTables(StringList *list);
   void shutdown();
   void restart();
};

/**
 * Server information
 */
class ServerInfo
{
private:
   char *m_name;
   InetAddress m_address;
   bool m_control;
   bool m_master;
   time_t m_lastResolveTime;
   bool m_redoResolve;
   MUTEX m_mutex;

   void resolve();

public:
   ServerInfo(const TCHAR *name, bool control, bool master);
   ~ServerInfo();

   bool match(const InetAddress &addr);

   bool isMaster() { return m_master; }
   bool isControl() { return m_control; }
};

/**
 * Pending request information
 */
struct PendingRequest
{
   UINT32 id;
   INT64 expirationTime;
   bool completed;

   PendingRequest(NXCPMessage *msg, UINT32 timeout)
   {
      id = msg->getId();
      expirationTime = GetCurrentTimeMs() + (INT64)timeout;
      completed = false;
   }
};

/**
 * Communication session
 */
class CommSession : public AbstractCommSession
{
private:
   UINT32 m_id;
   UINT32 m_index;
   AbstractCommChannel *m_channel;
   Queue *m_sendQueue;
   Queue *m_processingQueue;
   THREAD m_hWriteThread;
   THREAD m_hProcessingThread;
   THREAD m_hProxyReadThread;
   UINT64 m_serverId;
   InetAddress m_serverAddr;        // IP address of connected host
   bool m_disconnected;
   bool m_authenticated;
   bool m_masterServer;
   bool m_controlServer;
   bool m_proxyConnection;
   bool m_acceptData;
   bool m_acceptTraps;
   bool m_acceptFileUpdates;
   bool m_ipv6Aware;
   bool m_bulkReconciliationSupported;
   HashMap<UINT32, DownloadFileInfo> m_downloadFileMap;
   bool m_allowCompression;   // allow compression for structured messages
   NXCPEncryptionContext *m_pCtx;
   time_t m_ts;               // Last activity timestamp
   SOCKET m_hProxySocket;     // Socket for proxy connection
   MUTEX m_socketWriteMutex;
   VolatileCounter m_requestId;
   MsgWaitQueue *m_responseQueue;

   bool sendRawMessage(NXCP_MESSAGE *msg, NXCPEncryptionContext *ctx);
   void authenticate(NXCPMessage *pRequest, NXCPMessage *pMsg);
   void getConfig(NXCPMessage *pMsg);
   void updateConfig(NXCPMessage *pRequest, NXCPMessage *pMsg);
   void getParameter(NXCPMessage *pRequest, NXCPMessage *pMsg);
   void getList(NXCPMessage *pRequest, NXCPMessage *pMsg);
   void getTable(NXCPMessage *pRequest, NXCPMessage *pMsg);
   void action(NXCPMessage *pRequest, NXCPMessage *pMsg);
   void recvFile(NXCPMessage *pRequest, NXCPMessage *pMsg);
   void getLocalFile(NXCPMessage *pRequest, NXCPMessage *pMsg);
   void cancelFileMonitoring(NXCPMessage *pRequest, NXCPMessage *pMsg);
   UINT32 upgrade(NXCPMessage *request);
   UINT32 setupProxyConnection(NXCPMessage *pRequest);

   void readThread();
   void writeThread();
   void processingThread();
   void proxyReadThread();
   void proxySnmpRequest(NXCPMessage *request);

   static THREAD_RESULT THREAD_CALL readThreadStarter(void *);
   static THREAD_RESULT THREAD_CALL writeThreadStarter(void *);
   static THREAD_RESULT THREAD_CALL processingThreadStarter(void *);
   static THREAD_RESULT THREAD_CALL proxyReadThreadStarter(void *);

public:
   CommSession(AbstractCommChannel *channel, const InetAddress &serverAddr, bool masterServer, bool controlServer);
   virtual ~CommSession();

   void run();
   void disconnect();

   virtual bool sendMessage(NXCPMessage *msg);
   virtual void postMessage(NXCPMessage *msg) { if (!m_disconnected) m_sendQueue->put(msg->serialize(m_allowCompression)); }
   virtual bool sendRawMessage(NXCP_MESSAGE *msg);
   virtual void postRawMessage(NXCP_MESSAGE *msg) { if (!m_disconnected) m_sendQueue->put(nx_memdup(msg, ntohl(msg->size))); }
   virtual bool sendFile(UINT32 requestId, const TCHAR *file, long offset, bool allowCompression, VolatileCounter *cancelationFlag);
   virtual UINT32 doRequest(NXCPMessage *msg, UINT32 timeout);
   virtual NXCPMessage *doRequestEx(NXCPMessage *msg, UINT32 timeout);
   virtual UINT32 generateRequestId();

   virtual UINT32 getId() { return m_id; };

   virtual UINT64 getServerId() { return m_serverId; }
   virtual const InetAddress& getServerAddress() { return m_serverAddr; }

   virtual bool isMasterServer() { return m_masterServer; }
   virtual bool isControlServer() { return m_controlServer; }
   virtual bool canAcceptData() { return m_acceptData; }
   virtual bool canAcceptTraps() { return m_acceptTraps; }
   virtual bool canAcceptFileUpdates() { return m_acceptFileUpdates; }
   virtual bool isBulkReconciliationSupported() { return m_bulkReconciliationSupported; }
   virtual bool isIPv6Aware() { return m_ipv6Aware; }

   virtual UINT32 openFile(TCHAR *nameOfFile, UINT32 requestId, time_t fileModTime = 0);

   virtual void debugPrintf(int level, const TCHAR *format, ...);

   UINT32 getIndex() { return m_index; }
   void setIndex(UINT32 index) { if (m_index == INVALID_INDEX) m_index = index; }

   time_t getTimeStamp() { return m_ts; }
   void updateTimeStamp() { m_ts = time(NULL); }
};

/**
 * Pseudo-session for cached data collection
 */
class VirtualSession : public AbstractCommSession
{
private:
   UINT32 m_id;
   UINT64 m_serverId;

public:
   VirtualSession(UINT64 serverId);

   virtual UINT32 getId() { return m_id; }

   virtual UINT64 getServerId() { return m_serverId; };
   virtual const InetAddress& getServerAddress() { return InetAddress::LOOPBACK; }

   virtual bool isMasterServer() { return false; }
   virtual bool isControlServer() { return false; }
   virtual bool canAcceptData() { return true; }
   virtual bool canAcceptTraps() { return true; }
   virtual bool canAcceptFileUpdates() { return false; }
   virtual bool isBulkReconciliationSupported() { return false; }
   virtual bool isIPv6Aware() { return true; }

   virtual bool sendMessage(NXCPMessage *pMsg) { return false; }
   virtual void postMessage(NXCPMessage *pMsg) { }
   virtual bool sendRawMessage(NXCP_MESSAGE *pMsg) { return false; }
   virtual void postRawMessage(NXCP_MESSAGE *pMsg) { }
   virtual bool sendFile(UINT32 requestId, const TCHAR *file, long offset, bool allowCompression, VolatileCounter *cancelationFlag) { return false; }
   virtual UINT32 doRequest(NXCPMessage *msg, UINT32 timeout) { return RCC_NOT_IMPLEMENTED; }
   virtual NXCPMessage *doRequestEx(NXCPMessage *msg, UINT32 timeout) { return NULL; }
   virtual UINT32 generateRequestId() { return 0; }
   virtual UINT32 openFile(TCHAR *fileName, UINT32 requestId, time_t fileModTime = 0) { return ERR_INTERNAL_ERROR; }
   virtual void debugPrintf(int level, const TCHAR *format, ...);
};

/**
 * Session agent connector
 */
class SessionAgentConnector : public RefCountObject
{
private:
   UINT32 m_id;
   SOCKET m_socket;
   MUTEX m_mutex;
   NXCP_BUFFER m_msgBuffer;
   MsgWaitQueue m_msgQueue;
   UINT32 m_sessionId;
   TCHAR *m_sessionName;
   INT16 m_sessionState;
   TCHAR *m_userName;
   VolatileCounter m_requestId;

   void readThread();
   bool sendMessage(NXCPMessage *msg);
   UINT32 nextRequestId() { return InterlockedIncrement(&m_requestId); }

   static THREAD_RESULT THREAD_CALL readThreadStarter(void *);

public:
   SessionAgentConnector(UINT32 id, SOCKET s);
   virtual ~SessionAgentConnector();

   void run();
   void disconnect();

   UINT32 getId() { return m_id; }
   UINT32 getSessionId() { return m_sessionId; }
   INT16 getSessionState() { return m_sessionState; }
   const TCHAR *getSessionName() { return CHECK_NULL(m_sessionName); }
   const TCHAR *getUserName() { return CHECK_NULL(m_userName); }

   bool testConnection();
   void takeScreenshot(NXCPMessage *msg);
};

/**
 * Class to reciever traps
 */
class SNMP_TrapProxyTransport : public SNMP_UDPTransport
{
public:
   SNMP_TrapProxyTransport(SOCKET hSocket);

   int readRawMessage(BYTE **rawData, UINT32 timeout, struct sockaddr *sender, socklen_t *addrSize);
};

/**
 * SNMP target
 */
class SNMPTarget : public RefCountObject
{
private:
   uuid m_guid;
   UINT64 m_serverId;
   InetAddress m_ipAddress;
   UINT16 m_port;
   BYTE m_snmpVersion;
   BYTE m_authType;
   BYTE m_encType;
   char *m_authName;
   char *m_authPassword;
   char *m_encPassword;
   SNMP_Transport *m_transport;

public:
   SNMPTarget(UINT64 serverId, NXCPMessage *msg, UINT32 baseId);
   SNMPTarget(DB_RESULT hResult, int row);
   virtual ~SNMPTarget();

   const uuid& getGuid() const { return m_guid; }
   SNMP_Transport *getTransport(UINT16 port);

   bool saveToDatabase();
};

/**
 * Functions
 */
BOOL Initialize();
void Shutdown();
void Main();

void ConsolePrintf(const TCHAR *format, ...)
#if !defined(UNICODE) && (defined(__GNUC__) || defined(__clang__))
__attribute__ ((format(printf, 1, 2)))
#endif
;

void DebugPrintf(int level, const TCHAR *format, ...)
#if !defined(UNICODE) && (defined(__GNUC__) || defined(__clang__))
__attribute__ ((format(printf, 2, 3)))
#endif
;

void BuildFullPath(TCHAR *pszFileName, TCHAR *pszFullPath);

BOOL DownloadConfig(TCHAR *pszServer);

BOOL InitParameterList();

void AddParameter(const TCHAR *szName, LONG (* fpHandler)(const TCHAR *, const TCHAR *, TCHAR *, AbstractCommSession *), const TCHAR *pArg,
                  int iDataType, const TCHAR *pszDescription);
void AddPushParameter(const TCHAR *name, int dataType, const TCHAR *description);
void AddList(const TCHAR *name, LONG (* handler)(const TCHAR *, const TCHAR *, StringList *, AbstractCommSession *), const TCHAR *arg);
void AddTable(const TCHAR *name, LONG (* handler)(const TCHAR *, const TCHAR *, Table *, AbstractCommSession *),
              const TCHAR *arg, const TCHAR *instanceColumns, const TCHAR *description,
              int numColumns, NETXMS_SUBAGENT_TABLE_COLUMN *columns);
bool AddExternalParameter(TCHAR *config, bool shellExec, bool isList);
bool AddExternalTable(TCHAR *config, bool shellExec);
UINT32 GetParameterValue(const TCHAR *param, TCHAR *value, AbstractCommSession *session);
UINT32 GetListValue(const TCHAR *param, StringList *value, AbstractCommSession *session);
UINT32 GetTableValue(const TCHAR *param, Table *value, AbstractCommSession *session);
void GetParameterList(NXCPMessage *pMsg);
void GetEnumList(NXCPMessage *pMsg);
void GetTableList(NXCPMessage *pMsg);
BOOL LoadSubAgent(TCHAR *szModuleName);
void UnloadAllSubAgents();
BOOL InitSubAgent(HMODULE hModule, const TCHAR *pszModuleName,
                  BOOL (* SubAgentInit)(NETXMS_SUBAGENT_INFO **, Config *),
                  const TCHAR *pszEntryPoint);
BOOL ProcessCmdBySubAgent(UINT32 dwCommand, NXCPMessage *pRequest, NXCPMessage *pResponse, AbstractCommSession *session);
BOOL AddAction(const TCHAR *pszName, int iType, const TCHAR *pArg,
               LONG (*fpHandler)(const TCHAR *, StringList *, const TCHAR *, AbstractCommSession *session),
               const TCHAR *pszSubAgent, const TCHAR *pszDescription);
BOOL AddActionFromConfig(TCHAR *pszLine, BOOL bShellExec);
UINT32 ExecAction(const TCHAR *action, StringList *args, AbstractCommSession *session);
UINT32 ExecActionWithOutput(CommSession *session, UINT32 requestId, const TCHAR *action, StringList *args);
UINT32 ExecuteCommand(TCHAR *pszCommand, StringList *pArgs, pid_t *pid);
UINT32 ExecuteShellCommand(TCHAR *pszCommand, StringList *pArgs);

void StartParamProvidersPoller();
bool AddParametersProvider(const TCHAR *line);
LONG GetParameterValueFromExtProvider(const TCHAR *name, TCHAR *buffer);
void ListParametersFromExtProviders(NXCPMessage *msg, UINT32 *baseId, UINT32 *count);
void ListParametersFromExtProviders(StringList *list);

bool AddExternalSubagent(const TCHAR *config);
UINT32 GetParameterValueFromExtSubagent(const TCHAR *name, TCHAR *buffer);
UINT32 GetTableValueFromExtSubagent(const TCHAR *name, Table *value);
UINT32 GetListValueFromExtSubagent(const TCHAR *name, StringList *value);
void ListParametersFromExtSubagents(NXCPMessage *msg, UINT32 *baseId, UINT32 *count);
void ListParametersFromExtSubagents(StringList *list);
void ListListsFromExtSubagents(NXCPMessage *msg, UINT32 *baseId, UINT32 *count);
void ListListsFromExtSubagents(StringList *list);
void ListTablesFromExtSubagents(NXCPMessage *msg, UINT32 *baseId, UINT32 *count);
void ListTablesFromExtSubagents(StringList *list);
bool SendMessageToMasterAgent(NXCPMessage *msg);
bool SendRawMessageToMasterAgent(NXCP_MESSAGE *msg);
void ShutdownExtSubagents();
void RestartExtSubagents();

void RegisterApplicationAgent(const TCHAR *name);
UINT32 GetParameterValueFromAppAgent(const TCHAR *name, TCHAR *buffer);

bool WaitForProcess(const TCHAR *name);

UINT32 UpgradeAgent(TCHAR *pszPkgFile);

void SendTrap(UINT32 dwEventCode, const TCHAR *eventName, int iNumArgs, TCHAR **ppArgList);
void SendTrap(UINT32 dwEventCode, const TCHAR *eventName, const char *pszFormat, ...);
void SendTrap(UINT32 dwEventCode, const TCHAR *eventName, const char *pszFormat, va_list args);
void ForwardTrap(NXCPMessage *msg);

void StartPushConnector();
bool PushData(const TCHAR *parameter, const TCHAR *value, UINT32 objectId, time_t timestamp);

void StartControlConnector();
bool SendControlMessage(NXCPMessage *msg);

void StartSessionAgentConnector();
SessionAgentConnector *AcquireSessionAgentConnector(const TCHAR *sessionName);

UINT32 GenerateMessageId();

void ConfigureDataCollection(UINT64 serverId, NXCPMessage *msg);

bool EnumerateSessions(EnumerationCallbackResult (* callback)(AbstractCommSession *, void* ), void *data);
AbstractCommSession *FindServerSession(UINT64 serverId);
AbstractCommSession *FindServerSession(bool (*comparator)(AbstractCommSession *, void *), void *userData);

#ifdef _WIN32

void InitService();
void InstallService(TCHAR *execName, TCHAR *confFile, int debugLevel);
void RemoveService();
void StartAgentService();
void StopAgentService();
bool WaitForService(DWORD dwDesiredState);
void InstallEventSource(const TCHAR *path);
void RemoveEventSource();

TCHAR *GetPdhErrorText(UINT32 dwError, TCHAR *pszBuffer, int iBufSize);

#endif


/**
 * Global variables
 */
extern UINT32 g_dwFlags;
extern UINT32 g_failFlags;
extern TCHAR g_szLogFile[];
extern TCHAR g_szSharedSecret[];
extern TCHAR g_szConfigFile[];
extern TCHAR g_szFileStore[];
extern TCHAR g_szConfigServer[];
extern TCHAR g_szRegistrar[];
extern TCHAR g_szListenAddress[];
extern TCHAR g_szConfigIncludeDir[];
extern TCHAR g_szConfigPolicyDir[];
extern TCHAR g_szLogParserDirectory[];
extern TCHAR g_certificateDirectory[];
extern TCHAR g_szDataDirectory[];
extern TCHAR g_masterAgent[];
extern TCHAR g_szSNMPTrapListenAddress[];
extern UINT16 g_wListenPort;
extern TCHAR g_systemName[];
extern ObjectArray<ServerInfo> g_serverList;
extern time_t g_tmAgentStartTime;
extern TCHAR g_szPlatformSuffix[];
extern UINT32 g_dwStartupDelay;
extern UINT32 g_dwIdleTimeout;
extern UINT32 g_dwMaxSessions;
extern UINT32 g_execTimeout;
extern UINT32 g_snmpTimeout;
extern UINT16 g_snmpTrapPort;
extern UINT32 g_longRunningQueryThreshold;
extern UINT16 g_sessionAgentPort;
extern UINT32 g_zoneUIN;
extern UINT32 g_tunnelKeepaliveInterval;
extern UINT16 g_syslogListenPort;

extern Config *g_config;

extern UINT32 g_acceptErrors;
extern UINT32 g_acceptedConnections;
extern UINT32 g_rejectedConnections;

extern CommSession **g_pSessionList;
extern MUTEX g_hSessionListAccess;
extern ThreadPool *g_snmpProxyThreadPool;
extern ThreadPool *g_commThreadPool;

#ifdef _WIN32
extern TCHAR g_windowsEventSourceName[];
#endif   /* _WIN32 */

#endif   /* _nxagentd_h_ */
