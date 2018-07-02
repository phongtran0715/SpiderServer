#include "nxcore.h"

#ifdef HAVE_STD
#  include <iostream>
using namespace std;
#else
#  include <iostream.h>
#endif

void AgentSide_i::onDownloadStartup(const ::CORBA::WChar* downloadClusterId)
{
	DbgPrintf(5, _T("AgentSide_i::[onDownloadStartup] Download cluster id : %s"), downloadClusterId);
	TCHAR query [MAX_DB_STRING];
	DB_RESULT hResult;
	UINT32 i, dwNumRecords;
	DB_HANDLE hdb = DBConnectionPoolAcquireConnection();
	_sntprintf(query, sizeof query, _T("SELECT id, time_interval_sync")
	           _T(" FROM mapping_list WHERE status_sync = 1 AND download_cluster = '%s'"), (const TCHAR*)downloadClusterId);
	DbgPrintf(5, _T("AgentSide_i::[onDownloadStartup] SQL query = %s"), query);
	DB_STATEMENT hStmt = DBPrepare(hdb, query);
	if (hStmt != NULL)
	{
		hResult = DBSelectPrepared(hStmt);
		if (hResult != NULL)
		{
			dwNumRecords = DBGetNumRows(hResult);
			DbgPrintf(5, _T("AgentSide_i::[onDownloadStartup] numRecord = %d"), dwNumRecords);
			for (i = 0; i < dwNumRecords; i++)
			{
				INT32 jobId = DBGetFieldInt64(hResult, i, 0);
				INT64 timeSync = DBGetFieldInt64(hResult, i, 1);
				SpiderDownloadClient* downloadClient = new SpiderDownloadClient((const TCHAR*)downloadClusterId);
				if (downloadClient->initSuccess)
				{
					if (downloadClient->mDownloadRef != NULL)
					{
						try
						{
							downloadClient->mDownloadRef->createDownloadTimer(jobId, timeSync);
						}
						catch (CORBA::TRANSIENT&) {
							DbgPrintf(1, _T("AgentSide_i::[onDownloadStartup] : Caught system exception TRANSIENT -- unable to contact the server"));
						}
						catch (CORBA::SystemException& ex) {
							DbgPrintf(1, _T("AgentSide_i::[onDownloadStartup] : Caught a CORBA:: %s"), ex._name());
						}
						catch (CORBA::Exception& ex)
						{
							DbgPrintf(1, _T("AgentSide_i::[onDownloadStartup] : Caught a CORBA:: %s"), ex._name());
						}
					}
				} else {
				}
			}
			DBFreeResult(hResult);
		}
	}
	DBConnectionPoolReleaseConnection(hdb);
}

void AgentSide_i::onRenderStartup(const ::CORBA::WChar* renderClusterId)
{
	DbgPrintf(5, _T("AgentSide_i::[onRenderStartup] >>>> renderClusterId = %s"), renderClusterId);
	DB_RESULT hResult;
	TCHAR query [MAX_DB_STRING];
	UINT32 i, dwNumRecords;
	SpiderRenderClient* renderClient = new SpiderRenderClient((const TCHAR*)renderClusterId);
	DB_HANDLE hdb = DBConnectionPoolAcquireConnection();
	_sntprintf(query, sizeof query,  _T("SELECT id, video_id, title, description, tag, thumbnail, downloaded_path, ")
	           _T(" process_status, license, mapping_list_id FROM video_container WHERE mapping_list_id IN ")
	           _T(" (SELECT id FROM mapping_list WHERE render_cluster = '%s') AND process_status = 1"), (TCHAR*) renderClusterId);
	DbgPrintf(5, _T("AgentSide_i::[onRenderStartup] SQL query = %s"), query);

	DB_STATEMENT hStmt = DBPrepare(hdb, query);
	if (hStmt != NULL)
	{
		hResult = DBSelectPrepared(hStmt);
		if (hResult != NULL)
		{
			dwNumRecords = DBGetNumRows(hResult);
			DbgPrintf(5, _T("AgentSide_i::[onRenderStartup] numRecord = %d"), dwNumRecords);

			for (i = 0; i < dwNumRecords; i++)
			{
				::SpiderCorba::SpiderDefine::VideoInfo vInfo;

				INT32 jobId = DBGetFieldInt64(hResult, i, 0);
				vInfo.videoId = ::CORBA::wstring_dup(DBGetField(hResult, i, 1, NULL, 0));
				vInfo.title = ::CORBA::wstring_dup(DBGetField(hResult, i, 2, NULL, 0));
				vInfo.description = ::CORBA::wstring_dup(DBGetField(hResult, i, 3, NULL, 0));
				vInfo.tags = ::CORBA::wstring_dup(DBGetField(hResult, i, 4, NULL, 0));
				vInfo.thumbnail = ::CORBA::wstring_dup(DBGetField(hResult, i, 5, NULL, 0));
				vInfo.vDownloadPath = ::CORBA::wstring_dup(DBGetField(hResult, i, 6, NULL, 0));
				vInfo.vRenderPath = ::CORBA::wstring_dup(_T(""));
				vInfo.processStatus = DBGetFieldInt64(hResult, i, 7);
				vInfo.license = DBGetFieldInt64(hResult, i, 8);
				vInfo.mappingId = DBGetFieldInt64(hResult, i, 9);

				if (renderClient->initSuccess)
				{
					if (renderClient->mRenderRef != NULL)
					{
						try
						{
							renderClient->mRenderRef->createRenderJob(jobId, vInfo);
							DbgPrintf(5, _T("AgentSide_i::[onRenderStartup] create render job id : %d OK"), jobId);
						}
						catch (CORBA::TRANSIENT&) {
							DbgPrintf(1, _T("AgentSide_i::[onRenderStartup] : Caught system exception TRANSIENT -- unable to contact the server"));
						}
						catch (CORBA::SystemException& ex) {
							DbgPrintf(1, _T("AgentSide_i::[onRenderStartup] : Caught a CORBA:: %s"), ex._name());
						}
						catch (CORBA::Exception& ex)
						{
							DbgPrintf(1, _T("AgentSide_i::[onRenderStartup] : Caught a CORBA:: %s"), ex._name());
						}
					}
				} else {
				}
			}
			DBFreeResult(hResult);
		}
	}
	DBConnectionPoolReleaseConnection(hdb);
	DbgPrintf(5, _T("AgentSide_i::[onRenderStartup] <<<< "));
}

void AgentSide_i::onUploadStartup(const ::CORBA::WChar* uploadClusterId)
{
	DbgPrintf(5, _T("AgentSide_i::onUploadStartup : uploadId = %s"), uploadClusterId);
	createUploadTimerByMapping(uploadClusterId);
	createUploadJobByMapping(uploadClusterId);
}

void AgentSide_i::createUploadTimerByMapping(const ::CORBA::WChar* uploadClusterId)
{
	DbgPrintf(5, _T("AgentSide_i::createUploadTimerByMapping : uploadClusterId = %s"), uploadClusterId);
	DB_RESULT hResult;
	UINT32 i, dwNumRecords;
	SpiderUploadClient* uploadClient = new SpiderUploadClient((const TCHAR*)uploadClusterId);
	DB_HANDLE hdb = DBConnectionPoolAcquireConnection();
	TCHAR query [MAX_DB_STRING];
	_sntprintf(query, sizeof query, _T("SELECT DISTINCT home_channel_id FROM mapping_list WHERE status_sync = 1 AND upload_cluster = '%s'"), uploadClusterId);
	DbgPrintf(5, _T("AgentSide_i::[createUploadTimerByMapping] SQL query = %s"), query);
	DB_STATEMENT hStmt = DBPrepare(hdb, query);
	if (hStmt != NULL)
	{
		hResult = DBSelectPrepared(hStmt);
		if (hResult != NULL)
		{
			dwNumRecords = DBGetNumRows(hResult);
			DbgPrintf(5, _T("AgentSide_i::[createUploadTimerByMapping] dwNumRecords = %d"), dwNumRecords);
			for (i = 0; i < dwNumRecords; i++)
			{
				TCHAR* cHomeId = DBGetField(hResult, i, 0, NULL, 0);
				if (uploadClient->initSuccess)
				{
					if (uploadClient->mUploadRef != NULL)
					{
						try
						{
							uploadClient->mUploadRef->createUploadTimer(cHomeId);
						}
						catch (CORBA::TRANSIENT&) {
							DbgPrintf(1, _T("AgentSide_i::[createUploadTimerByMapping] : Caught system exception TRANSIENT -- unable to contact the server"));
						}
						catch (CORBA::SystemException& ex) {
							DbgPrintf(1, _T("AgentSide_i::[createUploadTimerByMapping] : Caught a CORBA:: %s"), ex._name());
						}
						catch (CORBA::Exception& ex)
						{
							DbgPrintf(1, _T("AgentSide_i::[createUploadTimerByMapping] : Caught a CORBA:: %s"), ex._name());
						}
					}
				} else {
					DbgPrintf(1, _T("AgentSide_i::[createUploadTimerByMapping] : Can not create corba connection"));
				}
			}
			DBFreeResult(hResult);
		}
	}
	DBConnectionPoolReleaseConnection(hdb);
}

void AgentSide_i::createUploadJobByMapping(const ::CORBA::WChar* uploadClusterId)
{
	DbgPrintf(5, _T("AgentSide_i::[createUploadJobByMapping] SQL query : uploadClusterId = %s "), uploadClusterId);
	DB_RESULT hResult;
	UINT32 i, dwNumRecords;
	DB_HANDLE hdb = DBConnectionPoolAcquireConnection();
	TCHAR query [MAX_DB_STRING];
	_sntprintf(query, sizeof query,  _T("SELECT id, video_id, title, description, tag, thumbnail, ")
	           _T(" rendered_path, mapping_list_id FROM video_container WHERE mapping_list_id IN ")
	           _T(" (SELECT id FROM mapping_list WHERE upload_cluster = '%s') AND process_status = 2"),
	           (const TCHAR*)uploadClusterId);
	DbgPrintf(5, _T("AgentSide_i::[createUploadJobByMapping] SQL query : %s"), query);

	DB_STATEMENT hStmt = DBPrepare(hdb, query);
	if (hStmt != NULL)
	{
		hResult = DBSelectPrepared(hStmt);
		if (hResult != NULL)
		{
			dwNumRecords = DBGetNumRows(hResult);
			DbgPrintf(5, _T("AgentSide_i::onUploadStartup : number record = %d"), dwNumRecords);

			for (i = 0; i < dwNumRecords; i++)
			{
				INT32 id = DBGetFieldInt64(hResult, i, 0);
				TCHAR* videoId = DBGetField(hResult, i, 1, NULL, 0);
				TCHAR* vTitle = DBGetField(hResult, i, 2, NULL, 0);
				TCHAR* vDesc = DBGetField(hResult, i, 3, NULL, 0);
				TCHAR* vTags = DBGetField(hResult, i, 4, NULL, 0);
				TCHAR* vThumb = DBGetField(hResult, i, 5, NULL, 0);
				TCHAR* vRenderedPath = DBGetField(hResult, i, 6, NULL, 0);
				INT32 mappingId = DBGetFieldInt64(hResult, i, 7);

				SpiderUploadClient* uploadClient = new SpiderUploadClient((const TCHAR*)uploadClusterId);
				if (uploadClient->initSuccess)
				{
					if (uploadClient->mUploadRef != NULL)
					{
						try
						{
							::SpiderCorba::SpiderDefine::VideoInfo vInfo;
							vInfo.videoId = ::CORBA::wstring_dup(videoId);
							vInfo.title = ::CORBA::wstring_dup(vTitle);
							vInfo.title = ::CORBA::wstring_dup(vTitle);
							vInfo.tags = ::CORBA::wstring_dup(vTags);
							vInfo.description = ::CORBA::wstring_dup(vDesc);
							vInfo.thumbnail = ::CORBA::wstring_dup(vThumb);
							vInfo.vRenderPath = ::CORBA::wstring_dup(vRenderedPath);
							vInfo.mappingId = mappingId;
							TCHAR* cHomeId = getHomeChannelId(mappingId);
							if (cHomeId != nullptr)
							{
								uploadClient->mUploadRef->createUploadJob(id, vInfo, ::CORBA::wstring_dup(cHomeId));
							} else {
								DbgPrintf(1, _T("AgentSide_i::[createUploadJobByMapping] : home channel ID is NULL"));
							}
						}
						catch (CORBA::TRANSIENT&) {
							DbgPrintf(1, _T("AgentSide_i::[createUploadJobByMapping] : Caught system exception TRANSIENT -- unable to contact the server"));
						}
						catch (CORBA::SystemException& ex) {
							DbgPrintf(1, _T("AgentSide_i::[createUploadJobByMapping] : Caught a CORBA:: %s"), ex._name());
						}
						catch (CORBA::Exception& ex)
						{
							DbgPrintf(1, _T("AgentSide_i::[createUploadJobByMapping] : Caught a CORBA:: %s"), ex._name());
						}
					}
				} else {
				}
			}
			DBFreeResult(hResult);
		}
	}
	DBConnectionPoolReleaseConnection(hdb);
}

TCHAR* AgentSide_i::getHomeChannelId(INT32 mappingId)
{
	TCHAR* cHomeId = nullptr;
	DB_RESULT hResult;
	UINT32 dwNumRecords;
	DB_HANDLE hdb = DBConnectionPoolAcquireConnection();
	TCHAR query [MAX_DB_STRING];
	_sntprintf(query, sizeof query,  _T("SELECT home_channel_id FROM mapping_list WHERE id = %d "), mappingId);
	DbgPrintf(5, _T("AgentSide_i::[getHomeChannelId] SQL query : %s"), query);
	DB_STATEMENT hStmt = DBPrepare(hdb, query);
	if (hStmt != NULL)
	{
		hResult = DBSelectPrepared(hStmt);
		if (hResult != NULL)
		{
			dwNumRecords = DBGetNumRows(hResult);
			DbgPrintf(5, _T("AgentSide_i::[getHomeChannelId] : number record = %d"), dwNumRecords);
			if (dwNumRecords > 0)
			{
				cHomeId = DBGetField(hResult, 0, 0, NULL, 0);
				DbgPrintf(5, _T("AgentSide_i::[getHomeChannelId] : home channel ID = %s"), cHomeId);
			}
			DBFreeResult(hResult);
		}
	}
	DBConnectionPoolReleaseConnection(hdb);
	return cHomeId;
}

::CORBA::LongLong AgentSide_i::getLastSyncTime(::CORBA::Long mappingId)
{
	DbgPrintf(5, _T("AgentSide_i::getLastSyncTime : mappingId = %ld"), mappingId);
	INT32 result = 0;
	DB_RESULT hResult;
	TCHAR query [MAX_DB_STRING];
	UINT32 dwNumRecords;
	DB_HANDLE hdb = DBConnectionPoolAcquireConnection();

	_sntprintf(query, sizeof query, _T("SELECT last_sync_time FROM mapping_list WHERE id = %d"), (INT32)mappingId);
	DbgPrintf(5, _T("AgentSide_i::getLastSyncTime : SQL query = %s"), query);
	DB_STATEMENT hStmt = DBPrepare(hdb, query);
	if (hStmt != NULL)
	{
		hResult = DBSelectPrepared(hStmt);
		if (hResult != NULL)
		{
			dwNumRecords = DBGetNumRows(hResult);
			if (dwNumRecords > 0)
			{
				result = DBGetFieldInt64(hResult, 0, 0);
				DbgPrintf(1, _T("AgentSide_i::getLastSyncTime : last sync time = %d"), result);
			}
			DBFreeResult(hResult);
		}
	}
	DBConnectionPoolReleaseConnection(hdb);
	return result;
}

void AgentSide_i::updateLastSyntime(::CORBA::Long mappingId, ::CORBA::LongLong lastSyncTime)
{
	DbgPrintf(5, _T("AgentSide_i::[updateLastSyntime]"));
	TCHAR query [MAX_DB_STRING];
	DB_HANDLE hdb = DBConnectionPoolAcquireConnection();
	DB_STATEMENT hStmt;
	if (hdb != NULL)
	{
		_sntprintf(query, sizeof query, _T("UPDATE mapping_list SET last_sync_time = %d WHERE id = %d"), (INT32)lastSyncTime , (INT32)mappingId);
		DbgPrintf(5, _T("AgentSide_i::[updateLastSyntime] SQL query = %s"), query);
		hStmt = DBPrepare(hdb, query);
		DBExecute(hStmt);
	}
	DBConnectionPoolReleaseConnection(hdb);
}

::SpiderCorba::SpiderDefine::CustomVideoInfor* AgentSide_i::getCustomVideo(const ::CORBA::WChar* downloadClusterId)
{
	DbgPrintf(5, _T("AgentSide_i::[getCustomVideo]"));
	::SpiderCorba::SpiderDefine::CustomVideoInfor* customVideo = new ::SpiderCorba::SpiderDefine::CustomVideoInfor();
	DB_RESULT hResult;
	TCHAR query [MAX_DB_STRING];
	UINT32 dwNumRecords;
	DB_HANDLE hdb = DBConnectionPoolAcquireConnection();

	_sntprintf(query, sizeof query, _T("SELECT id, video_id, mapping_list_id FROM video_container WHERE mapping_list_id IN (SELECT id FROM mapping_list ")
	           _T("WHERE download_cluster = '%s' AND status_sync = 1) AND process_status = 0 LIMIT 1"), (const TCHAR*) downloadClusterId);
	DbgPrintf(5, _T("AgentSide_i::getCustomVideo : SQL query = %s"), query);
	DB_STATEMENT hStmt = DBPrepare(hdb, query);
	if (hStmt != NULL)
	{
		hResult = DBSelectPrepared(hStmt);
		if (hResult != NULL)
		{
			dwNumRecords = DBGetNumRows(hResult);
			if (dwNumRecords > 0)
			{
				customVideo->id = DBGetFieldInt64(hResult, 0, 0);
				customVideo->videoId = CORBA::wstring_dup(DBGetField(hResult, 0, 1, NULL, 0));
				customVideo->mappingId = DBGetFieldInt64(hResult, 0, 2);
			}
			DBFreeResult(hResult);
		}
	}
	DBConnectionPoolReleaseConnection(hdb);
	return customVideo;
}

void AgentSide_i::insertDownloadedVideo(const ::SpiderCorba::SpiderDefine::VideoInfo& vInfo)
{
	DbgPrintf(5, _T("AgentSide_i::[updateDownloadedVideo]"));
	DB_HANDLE hdb = DBConnectionPoolAcquireConnection();
	DB_STATEMENT hStmt;

	if (hdb != NULL)
	{
		hStmt = DBPrepare(hdb, _T("INSERT INTO video_container (video_id, title, description, ")
		                  _T(" tag, thumbnail, downloaded_path, process_status,")
		                  _T(" license, mapping_list_id) VALUES (?,?,?,?,?,?,?,?,?)"));

		DBBind(hStmt, 1, DB_SQLTYPE_VARCHAR, (const TCHAR *)CORBA::wstring_dup(vInfo.videoId), DB_BIND_TRANSIENT);
		DBBind(hStmt, 2, DB_SQLTYPE_VARCHAR, (const TCHAR *)CORBA::wstring_dup(vInfo.title), DB_BIND_TRANSIENT);
		DBBind(hStmt, 3, DB_SQLTYPE_VARCHAR, (const TCHAR *)CORBA::wstring_dup(vInfo.description), DB_BIND_TRANSIENT);
		DBBind(hStmt, 4, DB_SQLTYPE_VARCHAR, (const TCHAR *)CORBA::wstring_dup(vInfo.tags), DB_BIND_TRANSIENT);
		DBBind(hStmt, 5, DB_SQLTYPE_VARCHAR, (const TCHAR *)CORBA::wstring_dup(vInfo.thumbnail), DB_BIND_TRANSIENT);
		DBBind(hStmt, 6, DB_SQLTYPE_VARCHAR, (const TCHAR *)CORBA::wstring_dup(vInfo.vDownloadPath), DB_BIND_TRANSIENT);
		DBBind(hStmt, 7, DB_SQLTYPE_INTEGER, (INT32)vInfo.processStatus);
		DBBind(hStmt, 8, DB_SQLTYPE_INTEGER, (INT32)vInfo.license);
		DBBind(hStmt, 9, DB_SQLTYPE_INTEGER, (INT32)vInfo.mappingId);

		bool success = DBExecute(hStmt);
		if (success == true)
		{
			//notify to render app
			TCHAR* renderClusterId = getClusterId((INT32)vInfo.mappingId, TYPE_RENDERED);
			SpiderRenderClient* renderClient = new SpiderRenderClient(renderClusterId);
			if (renderClient->initSuccess)
			{
				if (renderClient->mRenderRef != NULL)
				{
					try
					{
						INT32 jobId = getMaxId(_T("video_container"));
						renderClient->mRenderRef->createRenderJob(jobId, vInfo);
					}
					catch (CORBA::TRANSIENT&) {
						DbgPrintf(1, _T("AgentSide_i::[] : Caught system exception TRANSIENT -- unable to contact the server"));
					}
					catch (CORBA::SystemException& ex) {
						DbgPrintf(1, _T("AgentSide_i::[] : Caught a CORBA:: %s"), ex._name());
					}
					catch (CORBA::Exception& ex)
					{
						DbgPrintf(1, _T("AgentSide_i::[] : Caught a CORBA:: %s"), ex._name());
					}
				}
			} else
			{
			}
		} else
		{
			DbgPrintf(1, _T("AgentSide_i::updateDownloadedVideo : insert new video info FALSE"));
		}
	}

	DBConnectionPoolReleaseConnection(hdb);
}

void AgentSide_i::updateDownloadedVideo(::CORBA::Long jobId, const ::SpiderCorba::SpiderDefine::VideoInfo& vInfo)
{
	DbgPrintf(5, _T("AgentSide_i::[updateDownloadedVideo] jobId = %d"), (INT32)jobId);
	DB_HANDLE hdb = DBConnectionPoolAcquireConnection();
	DB_STATEMENT hStmt;
	if (hdb != NULL)
	{
		hStmt = DBPrepare(hdb, _T("UPDATE video_container SET title = ?, description = ?, tag = ?, thumbnail = ?, ")
		                  _T(" downloaded_path = ?, process_status = ?, license = ? WHERE id = ?"));
		DBBind(hStmt, 1, DB_SQLTYPE_VARCHAR, (const TCHAR*)vInfo.title, DB_BIND_TRANSIENT);
		DBBind(hStmt, 2, DB_SQLTYPE_VARCHAR, (const TCHAR *)vInfo.description, DB_BIND_TRANSIENT);
		DBBind(hStmt, 3, DB_SQLTYPE_VARCHAR, (const TCHAR *)vInfo.tags, DB_BIND_TRANSIENT);
		DBBind(hStmt, 4, DB_SQLTYPE_VARCHAR, (const TCHAR *)vInfo.thumbnail, DB_BIND_TRANSIENT);
		DBBind(hStmt, 5, DB_SQLTYPE_VARCHAR, (const TCHAR *)vInfo.vDownloadPath, DB_BIND_TRANSIENT);
		DBBind(hStmt, 6, DB_SQLTYPE_INTEGER, vInfo.processStatus);
		DBBind(hStmt, 7, DB_SQLTYPE_INTEGER, vInfo.license);
		DBBind(hStmt, 8, DB_SQLTYPE_INTEGER, jobId);
		bool success = DBExecute(hStmt);
		if (success == true)
		{
			//notify to render app
			TCHAR* renderClusterId = getClusterId((INT32)vInfo.mappingId, TYPE_RENDERED);
			SpiderRenderClient* renderClient = new SpiderRenderClient(renderClusterId);
			if (renderClient->initSuccess)
			{
				if (renderClient->mRenderRef != NULL)
				{
					try
					{
						renderClient->mRenderRef->createRenderJob(jobId, vInfo);
					}
					catch (CORBA::TRANSIENT&) {
						DbgPrintf(1, _T("AgentSide_i::[] : Caught system exception TRANSIENT -- unable to contact the server"));
					}
					catch (CORBA::SystemException& ex) {
						DbgPrintf(1, _T("AgentSide_i::[] : Caught a CORBA:: %s"), ex._name());
					}
					catch (CORBA::Exception& ex)
					{
						DbgPrintf(1, _T("AgentSide_i::[] : Caught a CORBA:: %s"), ex._name());
					}
				}
			} else
			{
			}
		} else
		{
			DbgPrintf(1, _T("AgentSide_i::updateDownloadedVideo : insert new video info FALSE"));
		}
	}
	DBConnectionPoolReleaseConnection(hdb);
}

TCHAR* AgentSide_i::getClusterId(INT32 mappingId, INT32 clusterType)
{
	DbgPrintf(5, _T("AgentSide_i::[getClusterId] : mappingId = %d - mapping type = %d"), mappingId);
	TCHAR* clusterName = nullptr;
	TCHAR query [MAX_DB_STRING];
	DB_RESULT hResult;
	UINT32 dwNumRecords;
	DB_HANDLE hdb = DBConnectionPoolAcquireConnection();
	const TCHAR* tbField;
	switch (clusterType)
	{
	case TYPE_DOWNLOADED:
		tbField = _T("download_cluster");
		break;
	case TYPE_RENDERED:
		tbField = _T("render_cluster");
		break;
	case TYPE_UPLOADED:
		tbField = _T("upload_cluster");
		break;
	default:
		break;
	}
	_sntprintf(query, sizeof query, _T("SELECT %s FROM mapping_list WHERE id = %d"), tbField, mappingId);
	DbgPrintf(5, _T("AgentSide_i::[getClusterId] : SQL query = %s"), query);
	DB_STATEMENT hStmt = DBPrepare(hdb, query);
	if (hStmt != NULL)
	{
		hResult = DBSelectPrepared(hStmt);
		if (hResult != NULL)
		{
			dwNumRecords = DBGetNumRows(hResult);
			if (dwNumRecords > 0)
			{
				clusterName = DBGetField(hResult, 0, 0, NULL, 0);
				DbgPrintf(1, _T("AgentSide_i::getClusterId : cluster id = %s"), clusterName);
			}
			DBFreeResult(hResult);
		}
	}
	DBConnectionPoolReleaseConnection(hdb);
	return clusterName;
}

void AgentSide_i::updateRenderedVideo(::CORBA::Long jobId, const ::SpiderCorba::SpiderDefine::VideoInfo & vInfo)
{
	DbgPrintf(5, _T("AgentSide_i::[updateRenderedVideo] jobId = %d"), (INT32)jobId);
	DB_HANDLE hdb = DBConnectionPoolAcquireConnection();
	DB_STATEMENT hStmt;

	if (hdb != NULL)
	{
		hStmt = DBPrepare(hdb, _T("UPDATE video_container SET process_status = ?, rendered_path = ? WHERE id = ?"));
		DBBind(hStmt, 1, DB_SQLTYPE_INTEGER, (INT32)vInfo.processStatus);
		DBBind(hStmt, 2, DB_SQLTYPE_VARCHAR, (const TCHAR *)vInfo.vRenderPath, DB_BIND_TRANSIENT);
		DBBind(hStmt, 3, DB_SQLTYPE_INTEGER, (INT32)jobId);
		bool success = DBExecute(hStmt);
		if (success == true)
		{
			//notify to upload app
			TCHAR* uploadClusterId = getClusterId((INT32)vInfo.mappingId, TYPE_UPLOADED);
			SpiderUploadClient* uploadClient = new SpiderUploadClient((const TCHAR*)uploadClusterId);
			if (uploadClient->initSuccess)
			{
				if (uploadClient->mUploadRef != NULL)
				{
					try
					{
						TCHAR* cHomeId = getHomeChannelId((INT32)vInfo.mappingId);
						if (cHomeId != nullptr)
						{
							uploadClient->mUploadRef->createUploadJob(jobId, vInfo, ::CORBA::wstring_dup(cHomeId));
						} else {
							DbgPrintf(5, _T("AgentSide_i::[updateRenderedVideo] home channel ID is NULL "));
						}
					}
					catch (CORBA::TRANSIENT&) {
						DbgPrintf(1, _T("AgentSide_i::[] : Caught system exception TRANSIENT -- unable to contact the server"));
					}
					catch (CORBA::SystemException& ex) {
						DbgPrintf(1, _T("AgentSide_i::[] : Caught a CORBA:: %s"), ex._name());
					}
					catch (CORBA::Exception& ex)
					{
						DbgPrintf(1, _T("AgentSide_i::[] : Caught a CORBA:: %s"), ex._name());
					}
				}
			} else {
			}
		}
	}
	DBConnectionPoolReleaseConnection(hdb);
}

AgentCorbaServer::AgentCorbaServer(): initSuccess(false)
{
	//Default constructor
}

void AgentCorbaServer::initCorba()
{
	try {
		int param1 = 1;
		char* param2[] = { ""};
		CORBA::ORB_var          orb = CORBA::ORB_init(param1, param2);
		CORBA::Object_var       obj = orb->resolve_initial_references("RootPOA");
		PortableServer::POA_var poa = PortableServer::POA::_narrow(obj);

		PortableServer::Servant_var<AgentSide_i> myecho = new AgentSide_i();

		PortableServer::ObjectId_var myechoid = poa->activate_object(myecho);

		// Obtain a reference to the object, and register it in
		// the naming service.
		obj = myecho->_this();

		CORBA::String_var sior(orb->object_to_string(obj));
		cout << sior << endl;

		if (!bindObjectToName(orb, obj))
		{
			DbgPrintf(1, _T("AgentCorbaServer::initCorba: ERROR ! scan not bind object to name..."));
			return;
		}

		PortableServer::POAManager_var pman = poa->the_POAManager();
		pman->activate();
		DbgPrintf(1, _T("server is running and waiting connection from client..."));

		orb->run();
	}
	catch (CORBA::SystemException& ex) {
		DbgPrintf(1, _T("AgentCorbaServer::initCorba: Caught CORBA:: %s"), ex._name());
	}
	catch (CORBA::Exception& ex) {
		DbgPrintf(1, _T("AgentCorbaServer::initCorba: Caught CORBA::Exception: %s"), ex._name());
	}
}


CORBA::Boolean
AgentCorbaServer::bindObjectToName(CORBA::ORB_ptr orb, CORBA::Object_ptr objref)
{
	CosNaming::NamingContext_var rootContext;

	try {
		// Obtain a reference to the root context of the Name service:
		CORBA::Object_var obj = orb->resolve_initial_references("NameService");

		// Narrow the reference returned.
		rootContext = CosNaming::NamingContext::_narrow(obj);
		if (CORBA::is_nil(rootContext)) {
			DbgPrintf(1, _T("Failed to narrow the root naming context."));
			return false;
		}
	}
	catch (CORBA::NO_RESOURCES&) {
		DbgPrintf(1, _T("Caught NO_RESOURCES exception. You must configure omniORB "));
		DbgPrintf(1, _T("with the location of the naming service."));
		return false;
	}
	catch (CORBA::ORB::InvalidName&) {
		// This should not happen!
		DbgPrintf(1, _T("Service required is invalid [does not exist]."));
		return false;
	}

	try {
		// Bind objref with name Echo to the testContext:
		CosNaming::Name objectName;
		objectName.length(1);
		objectName[0].id   = (const char*) "AgentSide";   // string copied
		objectName[0].kind = (const char*) ""; // string copied

		try {
			rootContext->bind(objectName, objref);
		}
		catch (CosNaming::NamingContext::AlreadyBound& ex) {
			rootContext->rebind(objectName, objref);
		}
		// Note: Using rebind() will overwrite any Object previously bound
		//       to /test/Echo with obj.
		//       Alternatively, bind() can be used, which will raise a
		//       CosNaming::NamingContext::AlreadyBound exception if the name
		//       supplied is already bound to an object.
	}
	catch (CORBA::TRANSIENT& ex) {
		DbgPrintf(1, _T("Caught system exception TRANSIENT -- unable to contact the naming service."));
		DbgPrintf(1, _T(" Make sure the naming server is running and that omniORB is configured correctly."));
		return false;
	}
	catch (CORBA::SystemException& ex) {
		DbgPrintf(1, _T("Caught a CORBA::"), ex._name());
		return false;
	}
	return true;
}

INT32 AgentSide_i::getMaxId(const TCHAR * tbName)
{
	INT32 result = -1;
	DB_RESULT hResult;
	DB_HANDLE hdb = DBConnectionPoolAcquireConnection();
	TCHAR query [MAX_DB_STRING];
	_sntprintf(query, sizeof query, _T("SELECT MAX(id) FROM %s"), tbName);
	DB_STATEMENT hStmt = DBPrepare(hdb, query);
	if (hStmt != NULL)
	{
		hResult = DBSelectPrepared(hStmt);
		if (hResult != NULL)
		{
			result = DBGetFieldInt64(hResult, 0, 0);
			DBFreeResult(hResult);
		}
	}
	DBConnectionPoolReleaseConnection(hdb);
	return result;
}

void AgentSide_i::updateUploadedVideo(::CORBA::Long jobId)
{
	DbgPrintf(5, _T("AgentSide_i::[updateUploadedVideo] jobId = %ld"), jobId);
	DB_HANDLE hdb = DBConnectionPoolAcquireConnection();
	DB_STATEMENT hStmt;

	if (hdb != NULL)
	{
		hStmt = DBPrepare(hdb, _T("DELETE FROM video_container WHERE Id = ?"));
		DBBind(hStmt, 1, DB_SQLTYPE_INTEGER, (INT32)jobId);
		bool success = DBExecute(hStmt);
		if (success == true)
		{
			DbgPrintf(5, _T("AgentSide_i::[updateUploadedVideo] delete video record successful"));
			//TODO: delete video on disk
		}
		else {
			DbgPrintf(1, _T("AgentSide_i::[updateUploadedVideo] ERROR! Can not delete vide record for jobId: %ld"), jobId);
		}
	}
	DBConnectionPoolReleaseConnection(hdb);
}

::SpiderCorba::SpiderDefine::DownloadConfig* AgentSide_i::getDownloadConfig(::CORBA::Long mappingId)
{
	DbgPrintf(5, _T(" AgentSide_i::[getDownloadConfig] mappingId = %d"), mappingId);
	::SpiderCorba::SpiderDefine::DownloadConfig* downloadCfg = new ::SpiderCorba::SpiderDefine::DownloadConfig();
	DB_RESULT hResult;
	INT32 dwNumRecords;
	DB_HANDLE hdb = DBConnectionPoolAcquireConnection();
	TCHAR query [MAX_DB_STRING];
	_sntprintf(query, sizeof query, _T("SELECT home_channel_id, monitor_content , mapping_type ")
	           _T(" FROM mapping_list WHERE id = %d "), (INT32)mappingId);
	DbgPrintf(5, _T(" AgentSide_i::[getDownloadConfig] SQL query = %s"), query);
	DB_STATEMENT hStmt = DBPrepare(hdb, query);
	if (hStmt != NULL)
	{
		hResult = DBSelectPrepared(hStmt);
		if (hResult != NULL)
		{
			dwNumRecords = DBGetNumRows(hResult);
			DbgPrintf(5, _T(" AgentSide_i::[getDownloadConfig] Num ber record = %d"), dwNumRecords);
			if (dwNumRecords > 0)
			{
				downloadCfg->cHomeId = CORBA::wstring_dup(DBGetField(hResult, 0, 0, NULL, 0));
				downloadCfg->monitorContent = CORBA::wstring_dup(DBGetField(hResult, 0, 1, NULL, 0));
				downloadCfg->mappingType = DBGetFieldInt64(hResult, 0, 2);
			} else {
				DbgPrintf(1, _T(" AgentSide_i::[getDownloadConfig] Not found download config data in database with mappingId = %d "), mappingId);
			}

			DBFreeResult(hResult);
		}
		else {
			DbgPrintf(1, _T(" AgentSide_i::[getDownloadConfig] result is NULL"));
		}
	}
	else {
		DbgPrintf(1, _T(" AgentSide_i::[getDownloadConfig] mappingId = Can not prepare query command"));
	}
	DBConnectionPoolReleaseConnection(hdb);
	return downloadCfg;
}

::SpiderCorba::SpiderDefine::RenderConfig* AgentSide_i::getRenderConfig(::CORBA::Long mappingId)
{
	DbgPrintf(5, _T(" AgentSide_i::[getRenderConfig] mappingId = %d"), mappingId);
	::SpiderCorba::SpiderDefine::RenderConfig* renderCfg = new ::SpiderCorba::SpiderDefine::RenderConfig();
	DB_RESULT hResult;
	INT32 dwNumRecords;
	DB_HANDLE hdb = DBConnectionPoolAcquireConnection();
	TCHAR query [MAX_DB_STRING];
	_sntprintf(query, sizeof query, _T("SELECT video_intro, video_outro , logo, enable_intro, enable_outro, enable_logo ")
	           _T(" FROM mapping_config WHERE mapping_list_id = %d "), (INT32)mappingId);
	DbgPrintf(5, _T(" AgentSide_i::[getRenderConfig] SQL query = %s"), query);
	DB_STATEMENT hStmt = DBPrepare(hdb, query);
	if (hStmt != NULL)
	{
		hResult = DBSelectPrepared(hStmt);
		if (hResult != NULL)
		{
			dwNumRecords = DBGetNumRows(hResult);
			DbgPrintf(5, _T(" AgentSide_i::[getRenderConfig] Num ber record = %d"), dwNumRecords);
			if (dwNumRecords > 0)
			{
				renderCfg->vIntroTemp = CORBA::wstring_dup(DBGetField(hResult, 0, 0, NULL, 0));
				renderCfg->vOutroTemp = CORBA::wstring_dup(DBGetField(hResult, 0, 1, NULL, 0));
				renderCfg->vLogoTemp = CORBA::wstring_dup(DBGetField(hResult, 0, 2, NULL, 0));
				renderCfg->enableIntro = DBGetFieldInt64(hResult, 0, 3) == 1;
				renderCfg->enableOutro = DBGetFieldInt64(hResult, 0, 4) == 1;
				renderCfg->enableLogo = DBGetFieldInt64(hResult, 0, 5) == 1;
			} else {
				DbgPrintf(1, _T(" AgentSide_i::[getRenderConfig] Not found render config data in database with mappingId = %d "), mappingId);
			}

			DBFreeResult(hResult);
		}
		else {
			DbgPrintf(1, _T(" AgentSide_i::[getRenderConfig] result is NULL"));
		}
	}
	else {
		DbgPrintf(1, _T(" AgentSide_i::[getRenderConfig] mappingId = Can not prepare query command"));
	}
	DBConnectionPoolReleaseConnection(hdb);
	return renderCfg;
}

::SpiderCorba::SpiderDefine::UploadConfig* AgentSide_i::getUploadConfig(::CORBA::Long mappingId)
{
	DbgPrintf(5, _T(" AgentSide_i::[getUploadConfig] mappingId = %d"), mappingId);
	::SpiderCorba::SpiderDefine::UploadConfig* uploadCfg = new ::SpiderCorba::SpiderDefine::UploadConfig();
	DB_RESULT hResult;
	INT32 dwNumRecords;
	DB_HANDLE hdb = DBConnectionPoolAcquireConnection();
	TCHAR query [MAX_DB_STRING];
	_sntprintf(query, sizeof query, _T("SELECT title_template, desc_template , tag_template, enable_title, ")
	           _T(" enable_desc, enable_tag FROM mapping_config WHERE mapping_list_id = %d "), mappingId);
	DbgPrintf(5, _T(" AgentSide_i::[getUploadConfig] SQL query = %s"), query);
	DB_STATEMENT hStmt = DBPrepare(hdb, query);
	if (hStmt != NULL)
	{
		hResult = DBSelectPrepared(hStmt);
		if (hResult != NULL)
		{
			dwNumRecords = DBGetNumRows(hResult);
			DbgPrintf(5, _T(" AgentSide_i::[getUploadConfig] number record = %d"), dwNumRecords);
			if (dwNumRecords > 0)
			{
				uploadCfg->vTitleTemp = CORBA::wstring_dup(DBGetField(hResult, 0, 0, NULL, 0));
				uploadCfg->vDescTemp = CORBA::wstring_dup(DBGetField(hResult, 0, 1, NULL, 0));
				uploadCfg->vTagsTemp = CORBA::wstring_dup(DBGetField(hResult, 0, 2, NULL, 0));
				uploadCfg->enableTitle = DBGetFieldInt64(hResult, 0, 3) == 1;
				uploadCfg->enableDes = DBGetFieldInt64(hResult, 0, 4) == 1;
				uploadCfg->enableTags = DBGetFieldInt64(hResult, 0, 5) == 1;
			}
			DBFreeResult(hResult);
		}
		else {
			DbgPrintf(1, _T(" AgentSide_i::[getUploadConfig] result is NULL"));
		}
	}
	else {
		DbgPrintf(1, _T(" AgentSide_i::[getUploadConfig] mappingId = Can not prepare query command"));
	}
	DBConnectionPoolReleaseConnection(hdb);
	return uploadCfg;
}

::SpiderCorba::SpiderDefine::ClusterInfo* AgentSide_i::getClusterInfo(::CORBA::Long mappingId, ::CORBA::Long clusterType)
{
	DbgPrintf(5, _T(" Function [getClusterInfo] mappingId = %ld"), mappingId);
	::SpiderCorba::SpiderDefine::ClusterInfo* clusterInfo = new ::SpiderCorba::SpiderDefine::ClusterInfo();
	DB_RESULT hResult;
	UINT32 dwNumRecords;
	TCHAR query [MAX_DB_STRING];

	DB_HANDLE hdb = DBConnectionPoolAcquireConnection();
	_sntprintf(query, sizeof query, _T("SELECT render_cluster FROM mapping_list WHERE id = %d"), mappingId);
	DB_STATEMENT hStmt = DBPrepare(hdb, query);
	if (hStmt != NULL)
	{
		DBBind(hStmt, 1, DB_SQLTYPE_INTEGER, (INT32)mappingId);
		hResult = DBSelectPrepared(hStmt);
		if (hResult != NULL)
		{
			dwNumRecords = DBGetNumRows(hResult);
			if (dwNumRecords > 0)
			{
				TCHAR* clusterId  = DBGetField(hResult, 0, 0, NULL, 0);
				_sntprintf(query, sizeof query, _T("SELECT ip_address FROM cluster_info WHERE cluster_id = '%s'"), clusterId);
				hStmt = DBPrepare(hdb, query);
				if (hStmt != NULL)
				{
					hResult = DBSelectPrepared(hStmt);
					if (hResult != NULL) {
						dwNumRecords = DBGetNumRows(hResult);
						if (dwNumRecords > 0)
						{
							clusterInfo->clusterIp = DBGetField(hResult, 0, 0, NULL, 0);
							clusterInfo->userName = _T("phongtran0715");
							clusterInfo->password = _T("123456aA@");
							DbgPrintf(5, _T(" Function [getClusterInfo] cluster IP = %d"), clusterInfo->clusterIp);
						}
					}
				}
			}
			DBFreeResult(hResult);
		}
	}
	DBConnectionPoolReleaseConnection(hdb);
	return clusterInfo;
}


::SpiderCorba::SpiderDefine::AuthenInfo* AgentSide_i::getAuthenInfo(::CORBA::Long mappingId)
{
	DbgPrintf(5, _T(" Function [getAuthenInfo] mappingId = %ld"), mappingId);
	::SpiderCorba::SpiderDefine::AuthenInfo* authenInfo = new ::SpiderCorba::SpiderDefine::AuthenInfo();
	DB_RESULT hResult;
	UINT32 dwNumRecords;
	TCHAR query [MAX_DB_STRING];

	DB_HANDLE hdb = DBConnectionPoolAcquireConnection();
	_sntprintf(query, sizeof query, _T("SELECT home_channel_id FROM mapping_list WHERE id = ?"), mappingId);
	DbgPrintf(5, _T(" Function [getAuthenInfo] mappingId = SQL query = %s"), query);
	DB_STATEMENT hStmt = DBPrepare(hdb, query);
	if (hStmt != NULL)
	{
		DBBind(hStmt, 1, DB_SQLTYPE_INTEGER, (INT32)mappingId);
		hResult = DBSelectPrepared(hStmt);
		if (hResult != NULL)
		{
			dwNumRecords = DBGetNumRows(hResult);
			if (dwNumRecords > 0)
			{
				TCHAR* cHomeId  = DBGetField(hResult, 0, 0, NULL, 0);
				hStmt = DBPrepare(hdb, _T("SELECT user_name, api, client_secret, client_id FROM google_account WHERE user_name IN ")
				                  _T(" (SELECT google_account_user_name FROM home_channel WHERE channel_id = ?)"));
				if (hStmt != NULL)
				{
					DBBind(hStmt, 1, DB_SQLTYPE_VARCHAR, cHomeId, DB_BIND_TRANSIENT);
					hResult = DBSelectPrepared(hStmt);
					if (hResult != NULL) {
						dwNumRecords = DBGetNumRows(hResult);
						if (dwNumRecords > 0)
						{
							authenInfo->userName = CORBA::wstring_dup(DBGetField(hResult, 0, 0, NULL, 0));
							authenInfo->apiKey = CORBA::wstring_dup(DBGetField(hResult, 0, 1, NULL, 0));
							authenInfo->clientSecret = CORBA::wstring_dup(DBGetField(hResult, 0, 2, NULL, 0));
							authenInfo->clientId = CORBA::wstring_dup(DBGetField(hResult, 0, 3, NULL, 0));
						}
					}
				}
			}
			DBFreeResult(hResult);
		}
	}
	DBConnectionPoolReleaseConnection(hdb);
	return authenInfo;
}

AgentCorbaServer::~AgentCorbaServer()
{
}

