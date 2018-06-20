// This file is generated by omniidl (C++ backend)- omniORB_4_2. Do not edit.
#ifndef __SpiderAgentAPI_hh__
#define __SpiderAgentAPI_hh__

#ifndef __CORBA_H_EXTERNAL_GUARD__
#include <omniORB4/CORBA.h>
#endif

#ifndef  USE_stub_in_nt_dll
# define USE_stub_in_nt_dll_NOT_DEFINED_SpiderAgentAPI
#endif
#ifndef  USE_core_stub_in_nt_dll
# define USE_core_stub_in_nt_dll_NOT_DEFINED_SpiderAgentAPI
#endif
#ifndef  USE_dyn_stub_in_nt_dll
# define USE_dyn_stub_in_nt_dll_NOT_DEFINED_SpiderAgentAPI
#endif






#ifdef USE_stub_in_nt_dll
# ifndef USE_core_stub_in_nt_dll
#  define USE_core_stub_in_nt_dll
# endif
# ifndef USE_dyn_stub_in_nt_dll
#  define USE_dyn_stub_in_nt_dll
# endif
#endif

#ifdef _core_attr
# error "A local CPP macro _core_attr has already been defined."
#else
# ifdef  USE_core_stub_in_nt_dll
#  define _core_attr _OMNIORB_NTDLL_IMPORT
# else
#  define _core_attr
# endif
#endif

#ifdef _dyn_attr
# error "A local CPP macro _dyn_attr has already been defined."
#else
# ifdef  USE_dyn_stub_in_nt_dll
#  define _dyn_attr _OMNIORB_NTDLL_IMPORT
# else
#  define _dyn_attr
# endif
#endif



_CORBA_MODULE SpiderCorba

_CORBA_MODULE_BEG

#ifndef __SpiderCorba_mSpiderDefine__
#define __SpiderCorba_mSpiderDefine__
  class SpiderDefine;
  class _objref_SpiderDefine;
  class _impl_SpiderDefine;
  
  typedef _objref_SpiderDefine* SpiderDefine_ptr;
  typedef SpiderDefine_ptr SpiderDefineRef;

  class SpiderDefine_Helper {
  public:
    typedef SpiderDefine_ptr _ptr_type;

    static _ptr_type _nil();
    static _CORBA_Boolean is_nil(_ptr_type);
    static void release(_ptr_type);
    static void duplicate(_ptr_type);
    static void marshalObjRef(_ptr_type, cdrStream&);
    static _ptr_type unmarshalObjRef(cdrStream&);
  };

  typedef _CORBA_ObjRef_Var<_objref_SpiderDefine, SpiderDefine_Helper> SpiderDefine_var;
  typedef _CORBA_ObjRef_OUT_arg<_objref_SpiderDefine,SpiderDefine_Helper > SpiderDefine_out;

#endif

  // interface SpiderDefine
  class SpiderDefine {
  public:
    // Declarations for this interface type.
    typedef SpiderDefine_ptr _ptr_type;
    typedef SpiderDefine_var _var_type;

    static _ptr_type _duplicate(_ptr_type);
    static _ptr_type _narrow(::CORBA::Object_ptr);
    static _ptr_type _unchecked_narrow(::CORBA::Object_ptr);
    
    static _ptr_type _nil();

    static inline void _marshalObjRef(_ptr_type, cdrStream&);

    static inline _ptr_type _unmarshalObjRef(cdrStream& s) {
      omniObjRef* o = omniObjRef::_unMarshal(_PD_repoId,s);
      if (o)
        return (_ptr_type) o->_ptrToObjRef(_PD_repoId);
      else
        return _nil();
    }

    static inline _ptr_type _fromObjRef(omniObjRef* o) {
      if (o)
        return (_ptr_type) o->_ptrToObjRef(_PD_repoId);
      else
        return _nil();
    }

    static _core_attr const char* _PD_repoId;

    // Other IDL defined within this scope.
    struct VideoInfo {
      typedef _CORBA_ConstrType_Variable_Var<VideoInfo> _var_type;

      
      ::CORBA::WString_member videoId;

      ::CORBA::WString_member title;

      ::CORBA::WString_member tags;

      ::CORBA::WString_member description;

      ::CORBA::WString_member thumbnail;

      ::CORBA::WString_member vDownloadPath;

      ::CORBA::WString_member vRenderPath;

      ::CORBA::Long mappingId;

      ::CORBA::Long processStatus;

      ::CORBA::Long license;

    

      void operator>>= (cdrStream &) const;
      void operator<<= (cdrStream &);
    };

    typedef VideoInfo::_var_type VideoInfo_var;

    typedef _CORBA_ConstrType_Variable_OUT_arg< VideoInfo,VideoInfo_var > VideoInfo_out;

    struct DownloadConfig {
      typedef _CORBA_ConstrType_Variable_Var<DownloadConfig> _var_type;

      
      ::CORBA::WString_member cHomeId;

      ::CORBA::WString_member monitorContent;

      ::CORBA::WString_member downloadClusterId;

      ::CORBA::Long mappingType;

      ::CORBA::Long timerInterval;

      ::CORBA::Long synStatus;

    

      void operator>>= (cdrStream &) const;
      void operator<<= (cdrStream &);
    };

    typedef DownloadConfig::_var_type DownloadConfig_var;

    typedef _CORBA_ConstrType_Variable_OUT_arg< DownloadConfig,DownloadConfig_var > DownloadConfig_out;

    struct RenderConfig {
      typedef _CORBA_ConstrType_Variable_Var<RenderConfig> _var_type;

      
      ::CORBA::WString_member vIntroTemp;

      ::CORBA::WString_member vOutroTemp;

      ::CORBA::WString_member vLogoTemp;

      ::CORBA::Boolean enableIntro;

      ::CORBA::Boolean enableOutro;

      ::CORBA::Boolean enableLogo;

    

      void operator>>= (cdrStream &) const;
      void operator<<= (cdrStream &);
    };

    typedef RenderConfig::_var_type RenderConfig_var;

    typedef _CORBA_ConstrType_Variable_OUT_arg< RenderConfig,RenderConfig_var > RenderConfig_out;

    struct UploadConfig {
      typedef _CORBA_ConstrType_Variable_Var<UploadConfig> _var_type;

      
      ::CORBA::WString_member vTitleTemp;

      ::CORBA::WString_member vDescTemp;

      ::CORBA::WString_member vTagsTemp;

      ::CORBA::Boolean enableTitle;

      ::CORBA::Boolean enableDes;

      ::CORBA::Boolean enableTags;

    

      void operator>>= (cdrStream &) const;
      void operator<<= (cdrStream &);
    };

    typedef UploadConfig::_var_type UploadConfig_var;

    typedef _CORBA_ConstrType_Variable_OUT_arg< UploadConfig,UploadConfig_var > UploadConfig_out;

    struct AuthenInfo {
      typedef _CORBA_ConstrType_Variable_Var<AuthenInfo> _var_type;

      
      ::CORBA::WString_member userName;

      ::CORBA::WString_member apiKey;

      ::CORBA::WString_member clientSecret;

      ::CORBA::WString_member clientId;

    

      void operator>>= (cdrStream &) const;
      void operator<<= (cdrStream &);
    };

    typedef AuthenInfo::_var_type AuthenInfo_var;

    typedef _CORBA_ConstrType_Variable_OUT_arg< AuthenInfo,AuthenInfo_var > AuthenInfo_out;

    struct ClusterInfo {
      typedef _CORBA_ConstrType_Variable_Var<ClusterInfo> _var_type;

      
      ::CORBA::WString_member clusterIp;

      ::CORBA::WString_member userName;

      ::CORBA::WString_member password;

    

      void operator>>= (cdrStream &) const;
      void operator<<= (cdrStream &);
    };

    typedef ClusterInfo::_var_type ClusterInfo_var;

    typedef _CORBA_ConstrType_Variable_OUT_arg< ClusterInfo,ClusterInfo_var > ClusterInfo_out;

  
  };

  class _objref_SpiderDefine :
    public virtual ::CORBA::Object,
    public virtual omniObjRef
  {
  public:
    // IDL operations
    

    // Constructors
    inline _objref_SpiderDefine()  { _PR_setobj(0); }  // nil
    _objref_SpiderDefine(omniIOR*, omniIdentity*);

  protected:
    virtual ~_objref_SpiderDefine();

    
  private:
    virtual void* _ptrToObjRef(const char*);

    _objref_SpiderDefine(const _objref_SpiderDefine&);
    _objref_SpiderDefine& operator = (const _objref_SpiderDefine&);
    // not implemented

    friend class SpiderDefine;
  };

  class _pof_SpiderDefine : public _OMNI_NS(proxyObjectFactory) {
  public:
    inline _pof_SpiderDefine() : _OMNI_NS(proxyObjectFactory)(SpiderDefine::_PD_repoId) {}
    virtual ~_pof_SpiderDefine();

    virtual omniObjRef* newObjRef(omniIOR*,omniIdentity*);
    virtual _CORBA_Boolean is_a(const char*) const;
  };

  class _impl_SpiderDefine :
    public virtual omniServant
  {
  public:
    virtual ~_impl_SpiderDefine();

    
    
  public:  // Really protected, workaround for xlC
    virtual _CORBA_Boolean _dispatch(omniCallHandle&);

  private:
    virtual void* _ptrToInterface(const char*);
    virtual const char* _mostDerivedRepoId();
    
  };


#ifndef __SpiderCorba_mDownloadSide__
#define __SpiderCorba_mDownloadSide__
  class DownloadSide;
  class _objref_DownloadSide;
  class _impl_DownloadSide;
  
  typedef _objref_DownloadSide* DownloadSide_ptr;
  typedef DownloadSide_ptr DownloadSideRef;

  class DownloadSide_Helper {
  public:
    typedef DownloadSide_ptr _ptr_type;

    static _ptr_type _nil();
    static _CORBA_Boolean is_nil(_ptr_type);
    static void release(_ptr_type);
    static void duplicate(_ptr_type);
    static void marshalObjRef(_ptr_type, cdrStream&);
    static _ptr_type unmarshalObjRef(cdrStream&);
  };

  typedef _CORBA_ObjRef_Var<_objref_DownloadSide, DownloadSide_Helper> DownloadSide_var;
  typedef _CORBA_ObjRef_OUT_arg<_objref_DownloadSide,DownloadSide_Helper > DownloadSide_out;

#endif

  // interface DownloadSide
  class DownloadSide {
  public:
    // Declarations for this interface type.
    typedef DownloadSide_ptr _ptr_type;
    typedef DownloadSide_var _var_type;

    static _ptr_type _duplicate(_ptr_type);
    static _ptr_type _narrow(::CORBA::Object_ptr);
    static _ptr_type _unchecked_narrow(::CORBA::Object_ptr);
    
    static _ptr_type _nil();

    static inline void _marshalObjRef(_ptr_type, cdrStream&);

    static inline _ptr_type _unmarshalObjRef(cdrStream& s) {
      omniObjRef* o = omniObjRef::_unMarshal(_PD_repoId,s);
      if (o)
        return (_ptr_type) o->_ptrToObjRef(_PD_repoId);
      else
        return _nil();
    }

    static inline _ptr_type _fromObjRef(omniObjRef* o) {
      if (o)
        return (_ptr_type) o->_ptrToObjRef(_PD_repoId);
      else
        return _nil();
    }

    static _core_attr const char* _PD_repoId;

    // Other IDL defined within this scope.
    
  };

  class _objref_DownloadSide :
    public virtual _objref_SpiderDefine
  {
  public:
    // IDL operations
    ::CORBA::Boolean createDownloadTimer(::CORBA::Long timerId, const ::SpiderCorba::SpiderDefine::DownloadConfig& downloadCfg);
    ::CORBA::Boolean modifyDownloadTimer(::CORBA::Long timerId, const ::SpiderCorba::SpiderDefine::DownloadConfig& downloadCfg);
    ::CORBA::Boolean deleteDowloadTimer(::CORBA::Long timerId);
    ::CORBA::Boolean deleteDownloadedVideo(::CORBA::Long jobId);

    // Constructors
    inline _objref_DownloadSide()  { _PR_setobj(0); }  // nil
    _objref_DownloadSide(omniIOR*, omniIdentity*);

  protected:
    virtual ~_objref_DownloadSide();

    
  private:
    virtual void* _ptrToObjRef(const char*);

    _objref_DownloadSide(const _objref_DownloadSide&);
    _objref_DownloadSide& operator = (const _objref_DownloadSide&);
    // not implemented

    friend class DownloadSide;
  };

  class _pof_DownloadSide : public _OMNI_NS(proxyObjectFactory) {
  public:
    inline _pof_DownloadSide() : _OMNI_NS(proxyObjectFactory)(DownloadSide::_PD_repoId) {}
    virtual ~_pof_DownloadSide();

    virtual omniObjRef* newObjRef(omniIOR*,omniIdentity*);
    virtual _CORBA_Boolean is_a(const char*) const;
  };

  class _impl_DownloadSide :
    public virtual _impl_SpiderDefine
  {
  public:
    virtual ~_impl_DownloadSide();

    virtual ::CORBA::Boolean createDownloadTimer(::CORBA::Long timerId, const ::SpiderCorba::SpiderDefine::DownloadConfig& downloadCfg) = 0;
    virtual ::CORBA::Boolean modifyDownloadTimer(::CORBA::Long timerId, const ::SpiderCorba::SpiderDefine::DownloadConfig& downloadCfg) = 0;
    virtual ::CORBA::Boolean deleteDowloadTimer(::CORBA::Long timerId) = 0;
    virtual ::CORBA::Boolean deleteDownloadedVideo(::CORBA::Long jobId) = 0;
    
  public:  // Really protected, workaround for xlC
    virtual _CORBA_Boolean _dispatch(omniCallHandle&);

  private:
    virtual void* _ptrToInterface(const char*);
    virtual const char* _mostDerivedRepoId();
    
  };


#ifndef __SpiderCorba_mRenderSide__
#define __SpiderCorba_mRenderSide__
  class RenderSide;
  class _objref_RenderSide;
  class _impl_RenderSide;
  
  typedef _objref_RenderSide* RenderSide_ptr;
  typedef RenderSide_ptr RenderSideRef;

  class RenderSide_Helper {
  public:
    typedef RenderSide_ptr _ptr_type;

    static _ptr_type _nil();
    static _CORBA_Boolean is_nil(_ptr_type);
    static void release(_ptr_type);
    static void duplicate(_ptr_type);
    static void marshalObjRef(_ptr_type, cdrStream&);
    static _ptr_type unmarshalObjRef(cdrStream&);
  };

  typedef _CORBA_ObjRef_Var<_objref_RenderSide, RenderSide_Helper> RenderSide_var;
  typedef _CORBA_ObjRef_OUT_arg<_objref_RenderSide,RenderSide_Helper > RenderSide_out;

#endif

  // interface RenderSide
  class RenderSide {
  public:
    // Declarations for this interface type.
    typedef RenderSide_ptr _ptr_type;
    typedef RenderSide_var _var_type;

    static _ptr_type _duplicate(_ptr_type);
    static _ptr_type _narrow(::CORBA::Object_ptr);
    static _ptr_type _unchecked_narrow(::CORBA::Object_ptr);
    
    static _ptr_type _nil();

    static inline void _marshalObjRef(_ptr_type, cdrStream&);

    static inline _ptr_type _unmarshalObjRef(cdrStream& s) {
      omniObjRef* o = omniObjRef::_unMarshal(_PD_repoId,s);
      if (o)
        return (_ptr_type) o->_ptrToObjRef(_PD_repoId);
      else
        return _nil();
    }

    static inline _ptr_type _fromObjRef(omniObjRef* o) {
      if (o)
        return (_ptr_type) o->_ptrToObjRef(_PD_repoId);
      else
        return _nil();
    }

    static _core_attr const char* _PD_repoId;

    // Other IDL defined within this scope.
    
  };

  class _objref_RenderSide :
    public virtual _objref_SpiderDefine
  {
  public:
    // IDL operations
    ::CORBA::Boolean createRenderJob(::CORBA::Long jobId, const ::SpiderCorba::SpiderDefine::VideoInfo& vInfo);
    ::CORBA::Boolean deleteRenderJob(::CORBA::Long jobId);
    void deleteRenderdVideo(const ::CORBA::WChar* vLocation);

    // Constructors
    inline _objref_RenderSide()  { _PR_setobj(0); }  // nil
    _objref_RenderSide(omniIOR*, omniIdentity*);

  protected:
    virtual ~_objref_RenderSide();

    
  private:
    virtual void* _ptrToObjRef(const char*);

    _objref_RenderSide(const _objref_RenderSide&);
    _objref_RenderSide& operator = (const _objref_RenderSide&);
    // not implemented

    friend class RenderSide;
  };

  class _pof_RenderSide : public _OMNI_NS(proxyObjectFactory) {
  public:
    inline _pof_RenderSide() : _OMNI_NS(proxyObjectFactory)(RenderSide::_PD_repoId) {}
    virtual ~_pof_RenderSide();

    virtual omniObjRef* newObjRef(omniIOR*,omniIdentity*);
    virtual _CORBA_Boolean is_a(const char*) const;
  };

  class _impl_RenderSide :
    public virtual _impl_SpiderDefine
  {
  public:
    virtual ~_impl_RenderSide();

    virtual ::CORBA::Boolean createRenderJob(::CORBA::Long jobId, const ::SpiderCorba::SpiderDefine::VideoInfo& vInfo) = 0;
    virtual ::CORBA::Boolean deleteRenderJob(::CORBA::Long jobId) = 0;
    virtual void deleteRenderdVideo(const ::CORBA::WChar* vLocation) = 0;
    
  public:  // Really protected, workaround for xlC
    virtual _CORBA_Boolean _dispatch(omniCallHandle&);

  private:
    virtual void* _ptrToInterface(const char*);
    virtual const char* _mostDerivedRepoId();
    
  };


#ifndef __SpiderCorba_mUploadSide__
#define __SpiderCorba_mUploadSide__
  class UploadSide;
  class _objref_UploadSide;
  class _impl_UploadSide;
  
  typedef _objref_UploadSide* UploadSide_ptr;
  typedef UploadSide_ptr UploadSideRef;

  class UploadSide_Helper {
  public:
    typedef UploadSide_ptr _ptr_type;

    static _ptr_type _nil();
    static _CORBA_Boolean is_nil(_ptr_type);
    static void release(_ptr_type);
    static void duplicate(_ptr_type);
    static void marshalObjRef(_ptr_type, cdrStream&);
    static _ptr_type unmarshalObjRef(cdrStream&);
  };

  typedef _CORBA_ObjRef_Var<_objref_UploadSide, UploadSide_Helper> UploadSide_var;
  typedef _CORBA_ObjRef_OUT_arg<_objref_UploadSide,UploadSide_Helper > UploadSide_out;

#endif

  // interface UploadSide
  class UploadSide {
  public:
    // Declarations for this interface type.
    typedef UploadSide_ptr _ptr_type;
    typedef UploadSide_var _var_type;

    static _ptr_type _duplicate(_ptr_type);
    static _ptr_type _narrow(::CORBA::Object_ptr);
    static _ptr_type _unchecked_narrow(::CORBA::Object_ptr);
    
    static _ptr_type _nil();

    static inline void _marshalObjRef(_ptr_type, cdrStream&);

    static inline _ptr_type _unmarshalObjRef(cdrStream& s) {
      omniObjRef* o = omniObjRef::_unMarshal(_PD_repoId,s);
      if (o)
        return (_ptr_type) o->_ptrToObjRef(_PD_repoId);
      else
        return _nil();
    }

    static inline _ptr_type _fromObjRef(omniObjRef* o) {
      if (o)
        return (_ptr_type) o->_ptrToObjRef(_PD_repoId);
      else
        return _nil();
    }

    static _core_attr const char* _PD_repoId;

    // Other IDL defined within this scope.
    
  };

  class _objref_UploadSide :
    public virtual _objref_SpiderDefine
  {
  public:
    // IDL operations
    ::CORBA::Boolean createUploadTimer(const ::CORBA::WChar* cHomeId);
    ::CORBA::Boolean deleteUploadTimer(const ::CORBA::WChar* cHomeId);
    ::CORBA::Boolean createUploadJob(::CORBA::Long jobId, const ::SpiderCorba::SpiderDefine::VideoInfo& vInfo, const ::CORBA::WChar* cHomeId);
    ::CORBA::Boolean deleteUploadJob(::CORBA::Long jobId, const ::CORBA::WChar* cHomeId);

    // Constructors
    inline _objref_UploadSide()  { _PR_setobj(0); }  // nil
    _objref_UploadSide(omniIOR*, omniIdentity*);

  protected:
    virtual ~_objref_UploadSide();

    
  private:
    virtual void* _ptrToObjRef(const char*);

    _objref_UploadSide(const _objref_UploadSide&);
    _objref_UploadSide& operator = (const _objref_UploadSide&);
    // not implemented

    friend class UploadSide;
  };

  class _pof_UploadSide : public _OMNI_NS(proxyObjectFactory) {
  public:
    inline _pof_UploadSide() : _OMNI_NS(proxyObjectFactory)(UploadSide::_PD_repoId) {}
    virtual ~_pof_UploadSide();

    virtual omniObjRef* newObjRef(omniIOR*,omniIdentity*);
    virtual _CORBA_Boolean is_a(const char*) const;
  };

  class _impl_UploadSide :
    public virtual _impl_SpiderDefine
  {
  public:
    virtual ~_impl_UploadSide();

    virtual ::CORBA::Boolean createUploadTimer(const ::CORBA::WChar* cHomeId) = 0;
    virtual ::CORBA::Boolean deleteUploadTimer(const ::CORBA::WChar* cHomeId) = 0;
    virtual ::CORBA::Boolean createUploadJob(::CORBA::Long jobId, const ::SpiderCorba::SpiderDefine::VideoInfo& vInfo, const ::CORBA::WChar* cHomeId) = 0;
    virtual ::CORBA::Boolean deleteUploadJob(::CORBA::Long jobId, const ::CORBA::WChar* cHomeId) = 0;
    
  public:  // Really protected, workaround for xlC
    virtual _CORBA_Boolean _dispatch(omniCallHandle&);

  private:
    virtual void* _ptrToInterface(const char*);
    virtual const char* _mostDerivedRepoId();
    
  };


#ifndef __SpiderCorba_mAgentSide__
#define __SpiderCorba_mAgentSide__
  class AgentSide;
  class _objref_AgentSide;
  class _impl_AgentSide;
  
  typedef _objref_AgentSide* AgentSide_ptr;
  typedef AgentSide_ptr AgentSideRef;

  class AgentSide_Helper {
  public:
    typedef AgentSide_ptr _ptr_type;

    static _ptr_type _nil();
    static _CORBA_Boolean is_nil(_ptr_type);
    static void release(_ptr_type);
    static void duplicate(_ptr_type);
    static void marshalObjRef(_ptr_type, cdrStream&);
    static _ptr_type unmarshalObjRef(cdrStream&);
  };

  typedef _CORBA_ObjRef_Var<_objref_AgentSide, AgentSide_Helper> AgentSide_var;
  typedef _CORBA_ObjRef_OUT_arg<_objref_AgentSide,AgentSide_Helper > AgentSide_out;

#endif

  // interface AgentSide
  class AgentSide {
  public:
    // Declarations for this interface type.
    typedef AgentSide_ptr _ptr_type;
    typedef AgentSide_var _var_type;

    static _ptr_type _duplicate(_ptr_type);
    static _ptr_type _narrow(::CORBA::Object_ptr);
    static _ptr_type _unchecked_narrow(::CORBA::Object_ptr);
    
    static _ptr_type _nil();

    static inline void _marshalObjRef(_ptr_type, cdrStream&);

    static inline _ptr_type _unmarshalObjRef(cdrStream& s) {
      omniObjRef* o = omniObjRef::_unMarshal(_PD_repoId,s);
      if (o)
        return (_ptr_type) o->_ptrToObjRef(_PD_repoId);
      else
        return _nil();
    }

    static inline _ptr_type _fromObjRef(omniObjRef* o) {
      if (o)
        return (_ptr_type) o->_ptrToObjRef(_PD_repoId);
      else
        return _nil();
    }

    static _core_attr const char* _PD_repoId;

    // Other IDL defined within this scope.
    
  };

  class _objref_AgentSide :
    public virtual _objref_SpiderDefine
  {
  public:
    // IDL operations
    void onDownloadStartup(const ::CORBA::WChar* downloadClusterId);
    ::CORBA::LongLong getLastSyncTime(::CORBA::Long mappingId);
    void updateLastSyntime(::CORBA::Long mappingId, ::CORBA::LongLong lastSyncTime);
    void updateDownloadedVideo(const ::SpiderCorba::SpiderDefine::VideoInfo& vInfo);
    void onRenderStartup(const ::CORBA::WChar* renderClusterId);
    SpiderDefine::RenderConfig* getRenderConfig(::CORBA::Long mappingId);
    void updateRenderedVideo(::CORBA::Long jobId, const ::SpiderCorba::SpiderDefine::VideoInfo& vInfo);
    void onUploadStartup(const ::CORBA::WChar* uploadClusterId);
    SpiderDefine::UploadConfig* getUploadConfig(::CORBA::Long mappingId);
    void updateUploadedVideo(::CORBA::Long jobId);
    SpiderDefine::AuthenInfo* getAuthenInfo(::CORBA::Long mappingId);
    SpiderDefine::ClusterInfo* getClusterInfo(::CORBA::Long mappingId, ::CORBA::Long clusterType);

    // Constructors
    inline _objref_AgentSide()  { _PR_setobj(0); }  // nil
    _objref_AgentSide(omniIOR*, omniIdentity*);

  protected:
    virtual ~_objref_AgentSide();

    
  private:
    virtual void* _ptrToObjRef(const char*);

    _objref_AgentSide(const _objref_AgentSide&);
    _objref_AgentSide& operator = (const _objref_AgentSide&);
    // not implemented

    friend class AgentSide;
  };

  class _pof_AgentSide : public _OMNI_NS(proxyObjectFactory) {
  public:
    inline _pof_AgentSide() : _OMNI_NS(proxyObjectFactory)(AgentSide::_PD_repoId) {}
    virtual ~_pof_AgentSide();

    virtual omniObjRef* newObjRef(omniIOR*,omniIdentity*);
    virtual _CORBA_Boolean is_a(const char*) const;
  };

  class _impl_AgentSide :
    public virtual _impl_SpiderDefine
  {
  public:
    virtual ~_impl_AgentSide();

    virtual void onDownloadStartup(const ::CORBA::WChar* downloadClusterId) = 0;
    virtual ::CORBA::LongLong getLastSyncTime(::CORBA::Long mappingId) = 0;
    virtual void updateLastSyntime(::CORBA::Long mappingId, ::CORBA::LongLong lastSyncTime) = 0;
    virtual void updateDownloadedVideo(const ::SpiderCorba::SpiderDefine::VideoInfo& vInfo) = 0;
    virtual void onRenderStartup(const ::CORBA::WChar* renderClusterId) = 0;
    virtual SpiderDefine::RenderConfig* getRenderConfig(::CORBA::Long mappingId) = 0;
    virtual void updateRenderedVideo(::CORBA::Long jobId, const ::SpiderCorba::SpiderDefine::VideoInfo& vInfo) = 0;
    virtual void onUploadStartup(const ::CORBA::WChar* uploadClusterId) = 0;
    virtual SpiderDefine::UploadConfig* getUploadConfig(::CORBA::Long mappingId) = 0;
    virtual void updateUploadedVideo(::CORBA::Long jobId) = 0;
    virtual SpiderDefine::AuthenInfo* getAuthenInfo(::CORBA::Long mappingId) = 0;
    virtual SpiderDefine::ClusterInfo* getClusterInfo(::CORBA::Long mappingId, ::CORBA::Long clusterType) = 0;
    
  public:  // Really protected, workaround for xlC
    virtual _CORBA_Boolean _dispatch(omniCallHandle&);

  private:
    virtual void* _ptrToInterface(const char*);
    virtual const char* _mostDerivedRepoId();
    
  };


_CORBA_MODULE_END



_CORBA_MODULE POA_SpiderCorba
_CORBA_MODULE_BEG

  class SpiderDefine :
    public virtual SpiderCorba::_impl_SpiderDefine,
    public virtual ::PortableServer::ServantBase
  {
  public:
    virtual ~SpiderDefine();

    inline ::SpiderCorba::SpiderDefine_ptr _this() {
      return (::SpiderCorba::SpiderDefine_ptr) _do_this(::SpiderCorba::SpiderDefine::_PD_repoId);
    }
  };

  class DownloadSide :
    public virtual SpiderCorba::_impl_DownloadSide,
    public virtual SpiderDefine
  {
  public:
    virtual ~DownloadSide();

    inline ::SpiderCorba::DownloadSide_ptr _this() {
      return (::SpiderCorba::DownloadSide_ptr) _do_this(::SpiderCorba::DownloadSide::_PD_repoId);
    }
  };

  class RenderSide :
    public virtual SpiderCorba::_impl_RenderSide,
    public virtual SpiderDefine
  {
  public:
    virtual ~RenderSide();

    inline ::SpiderCorba::RenderSide_ptr _this() {
      return (::SpiderCorba::RenderSide_ptr) _do_this(::SpiderCorba::RenderSide::_PD_repoId);
    }
  };

  class UploadSide :
    public virtual SpiderCorba::_impl_UploadSide,
    public virtual SpiderDefine
  {
  public:
    virtual ~UploadSide();

    inline ::SpiderCorba::UploadSide_ptr _this() {
      return (::SpiderCorba::UploadSide_ptr) _do_this(::SpiderCorba::UploadSide::_PD_repoId);
    }
  };

  class AgentSide :
    public virtual SpiderCorba::_impl_AgentSide,
    public virtual SpiderDefine
  {
  public:
    virtual ~AgentSide();

    inline ::SpiderCorba::AgentSide_ptr _this() {
      return (::SpiderCorba::AgentSide_ptr) _do_this(::SpiderCorba::AgentSide::_PD_repoId);
    }
  };

_CORBA_MODULE_END



_CORBA_MODULE OBV_SpiderCorba
_CORBA_MODULE_BEG

_CORBA_MODULE_END





#undef _core_attr
#undef _dyn_attr



inline void
SpiderCorba::SpiderDefine::_marshalObjRef(::SpiderCorba::SpiderDefine_ptr obj, cdrStream& s) {
  omniObjRef::_marshal(obj->_PR_getobj(),s);
}

inline void
SpiderCorba::DownloadSide::_marshalObjRef(::SpiderCorba::DownloadSide_ptr obj, cdrStream& s) {
  omniObjRef::_marshal(obj->_PR_getobj(),s);
}

inline void
SpiderCorba::RenderSide::_marshalObjRef(::SpiderCorba::RenderSide_ptr obj, cdrStream& s) {
  omniObjRef::_marshal(obj->_PR_getobj(),s);
}

inline void
SpiderCorba::UploadSide::_marshalObjRef(::SpiderCorba::UploadSide_ptr obj, cdrStream& s) {
  omniObjRef::_marshal(obj->_PR_getobj(),s);
}

inline void
SpiderCorba::AgentSide::_marshalObjRef(::SpiderCorba::AgentSide_ptr obj, cdrStream& s) {
  omniObjRef::_marshal(obj->_PR_getobj(),s);
}



#ifdef   USE_stub_in_nt_dll_NOT_DEFINED_SpiderAgentAPI
# undef  USE_stub_in_nt_dll
# undef  USE_stub_in_nt_dll_NOT_DEFINED_SpiderAgentAPI
#endif
#ifdef   USE_core_stub_in_nt_dll_NOT_DEFINED_SpiderAgentAPI
# undef  USE_core_stub_in_nt_dll
# undef  USE_core_stub_in_nt_dll_NOT_DEFINED_SpiderAgentAPI
#endif
#ifdef   USE_dyn_stub_in_nt_dll_NOT_DEFINED_SpiderAgentAPI
# undef  USE_dyn_stub_in_nt_dll
# undef  USE_dyn_stub_in_nt_dll_NOT_DEFINED_SpiderAgentAPI
#endif

#endif  // __SpiderAgentAPI_hh__

