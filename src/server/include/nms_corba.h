#ifndef _nms_corba_h_
#define _nms_corba_h_

#include "SpiderAgentAPI.hh"

#ifdef _WIN32
#ifdef LIBCORBA_EXPORTS
#define LIBCORBA_EXPORTABLE __declspec(dllexport)
#else
#define LIBCORBA_EXPORTABLE __declspec(dllimport)
#endif
#else    /* _WIN32 */
#define LIBCORBA_EXPORTABLE
#endif

class LIBCORBA_EXPORTABLE YoutubeAgent
{
private:
   CORBA::Object_ptr getObjectReference(CORBA::ORB_ptr orb, const TCHAR* contextName);
public:
   SpiderCorba::YoutubeAgent_var mYtAgentRef;
   CORBA::ORB_var mOrb;
   bool initSuccess;
   YoutubeAgent(const TCHAR* contextName);
   ~YoutubeAgent();
};

class LIBCORBA_EXPORTABLE NetxmsCorbaClient
{
private:
   CORBA::Object_ptr getObjectReference(CORBA::ORB_ptr orb, const TCHAR* contextName);
public:
   SpiderCorba::SpiderBootSide_var netxmsClientRef;
   CORBA::ORB_var mOrb;
   bool initSuccess;
   NetxmsCorbaClient(const TCHAR* contextName);
   ~NetxmsCorbaClient();
};

class AgentCorbaServer
{
private:
   CORBA::Boolean bindObjectToName(CORBA::ORB_ptr, CORBA::Object_ptr);
   INT32 getMaxId(const TCHAR * tbName);
public:
   void initCorba();
   bool initSuccess;
   AgentCorbaServer();
   ~AgentCorbaServer();
};


class AgentSide_i : public POA_SpiderCorba::AgentSide
{
private:
   INT32 getMaxId(const TCHAR * tbName);
   TCHAR* getClusterId(INT32 mappingId, INT32 clusterType);
   void createUploadTimerByMapping(const ::CORBA::WChar* uploadClusterId);
   void createUploadJobByMapping(const ::CORBA::WChar* uploadClusterId);
   TCHAR* getHomeChannelId(INT32 mappingId);

public:
    // IDL operations
    void onDownloadStartup(const ::CORBA::WChar* downloadClusterId);
    ::CORBA::LongLong getLastSyncTime(::CORBA::Long mappingId);
    void updateLastSyntime(::CORBA::Long mappingId, ::CORBA::LongLong lastSyncTime);
    void insertDownloadedVideo(const ::SpiderCorba::SpiderDefine::VideoInfo& vInfo);
    void updateDownloadedVideo(::CORBA::Long jobId, const ::SpiderCorba::SpiderDefine::VideoInfo& vInfo);
    ::SpiderCorba::SpiderDefine::DownloadConfig* getDownloadConfig(::CORBA::Long mappingId);
    ::SpiderCorba::SpiderDefine::CustomVideoInfor* getCustomVideo(const ::CORBA::WChar* downloadClusterId, ::CORBA::Long timerId);
    void onRenderStartup(const ::CORBA::WChar* renderClusterId);
    ::SpiderCorba::SpiderDefine::RenderConfig* getRenderConfig(::CORBA::Long mappingId);
    void updateRenderedVideo(::CORBA::Long jobId, const ::SpiderCorba::SpiderDefine::VideoInfo& vInfo);
    void onUploadStartup(const ::CORBA::WChar* uploadClusterId);
    ::SpiderCorba::SpiderDefine::UploadConfig* getUploadConfig(::CORBA::Long mappingId);
    void updateUploadedVideo(::CORBA::Long jobId);
    ::SpiderCorba::SpiderDefine::AuthenInfo* getAuthenInfo(::CORBA::Long mappingId);
    ::SpiderCorba::SpiderDefine::ClusterInfo* getClusterInfo(::CORBA::Long mappingId, ::CORBA::Long clusterType);
};

#endif /* _nms_corba_h_ */
