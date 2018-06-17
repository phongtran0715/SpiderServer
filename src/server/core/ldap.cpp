/*
** NetXMS - Network Management System
** Copyright (C) 2003-2016 Raden Solutions
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
** File: ldap.cpp
**
**/

#include "nxcore.h"

#ifndef _WIN32

#define ldap_first_attributeA    ldap_first_attribute
#define ldap_next_attributeA     ldap_next_attribute
#define ldap_memfreeA            ldap_memfree
#define ldap_get_values_lenA     ldap_get_values_len

#endif

#define LDAP_DEFAULT 0
#define LDAP_USER 1
#define LDAP_GROUP 2

#ifndef LDAP_SSL_PORT
#define LDAP_SSL_PORT 636
#endif

#define LDAP_PAGE_OID "1.2.840.113556.1.4.319"

/**
 * LDAP entry constructor
 */
Entry::Entry()
{
   m_type = LDAP_DEFAULT;
   m_loginName = NULL;
   m_fullName = NULL;
   m_description = NULL;
   m_id = NULL;
   m_memberList = new StringSet();
}

/**
 * LDAP entry destructor
 */
Entry::~Entry()
{
   free(m_loginName);
   free(m_fullName);
   free(m_description);
   free(m_id);
   delete m_memberList;
}

#if WITH_LDAP

#if !defined(_WIN32) && !HAVE_LDAP_CONTROL_CREATE && !HAVE_LDAP_CREATE_CONTROL && HAVE_NSLDAPI_BUILD_CONTROL

#if !HAVE_DECL_NSLDAPI_BUILD_CONTROL
extern "C" int nsldapi_build_control(const char *oid, BerElement *ber, int freeber, char iscritical, LDAPControl **ctrlp);
#endif

static int ldap_create_control(const char *oid, BerElement *ber, int iscritical, LDAPControl **ctrlp)
{
	return nsldapi_build_control(oid, ber, 0, iscritical, ctrlp);
}

#endif

#if !HAVE_LDAP_CREATE_PAGE_CONTROL && !defined(_WIN32)
int ldap_create_page_control(LDAP *ldap, ber_int_t pagesize,
			     struct berval *cookie, char isCritical,
			     LDAPControl **output)
{
	BerElement *ber;
	int rc;

	if (!ldap || !output)
		return LDAP_PARAM_ERROR;

	ber = ber_alloc_t(LBER_USE_DER);
	if (!ber)
		return LDAP_NO_MEMORY;

	if (ber_printf(ber, "{io}", pagesize,
			(cookie && cookie->bv_val) ? cookie->bv_val : "",
			(cookie && cookie->bv_val) ? cookie->bv_len : 0)
				== -1)
	{
		ber_free(ber, 1);
		return LDAP_ENCODING_ERROR;
	}

#if HAVE_LDAP_CONTROL_CREATE
   rc = ldap_control_create(LDAP_PAGE_OID, isCritical, cookie, 0, output);
#else
	rc = ldap_create_control(LDAP_PAGE_OID, ber, isCritical, output);
#endif

	return rc;
}
#endif /* HAVE_LDAP_CREATE_PAGE_CONTROL */

#if !HAVE_LDAP_PARSE_PAGE_CONTROL && !defined(_WIN32)
int ldap_parse_page_control(LDAP *ldap, LDAPControl **controls,
			    ber_int_t *totalcount, struct berval **cookie)
{
	BerElement *theBer;
	LDAPControl *listCtrlp;

	for(int i = 0; controls[i] != NULL; i++) {
		if (strcmp(controls[i]->ldctl_oid, LDAP_PAGE_OID) == 0) {
			listCtrlp = controls[i];

			theBer = ber_init(&listCtrlp->ldctl_value);
			if (!theBer)
				return LDAP_NO_MEMORY;

			if (ber_scanf(theBer, "{iO}", totalcount, cookie) == LBER_ERROR)
			{
				ber_free(theBer, 1);
				return LDAP_DECODING_ERROR;
			}

			ber_free(theBer, 1);
			return LDAP_SUCCESS;
		}
	}

	return LDAP_CONTROL_NOT_FOUND;
}
#endif /* HAVE_LDAP_PARSE_PAGE_CONTROL */

/**
 * Lists
 */
StringObjectMap<Entry> *userIdEntryList;
StringObjectMap<Entry> *userDnEntryList;
StringObjectMap<Entry> *groupIdEntryList;
StringObjectMap<Entry> *groupDnEntryList;

/**
 * Correctly formats ldap connection string (according to OS)
 */
void LDAPConnection::prepareStringForInit(LDAP_CHAR *connectionLine)
{
   LDAP_CHAR *comma;
   LDAP_CHAR *lastSlash;
   LDAP_CHAR *nearestSpace;

   comma=ldap_strchr(connectionLine,_TLDAP(','));
   while(comma != NULL)
   {
      *comma = _TLDAP(' ');
      comma=ldap_strchr(connectionLine,_TLDAP(','));
   }

   if(ldap_strstr(connectionLine,_TLDAP("ldaps://")))
   {
      m_secure = 1;
   }

   lastSlash=ldap_strchr(connectionLine, _TLDAP('/'));
   while(lastSlash != NULL)
   {
      *lastSlash = 0;
      lastSlash++;

      nearestSpace=ldap_strchr(connectionLine,_TLDAP(' '));
      if(nearestSpace == NULL)
      {
         nearestSpace = connectionLine;
      }
      else
      {
         nearestSpace++;
      }
      *nearestSpace = 0;
      ldap_strcat(connectionLine, lastSlash);

      lastSlash=ldap_strchr(connectionLine, _TLDAP('/'));
   }
}

/**
 * Init connection with LDAP(init search line, start checking thread, init check interval)
 */
void LDAPConnection::initLDAP()
{
   DbgPrintf(4, _T("LDAPConnection::initLDAP(): Connecting to LDAP server"));
#if HAVE_LDAP_INITIALIZE
   int errorCode = ldap_initialize(&m_ldapConn, m_connList);
   if (errorCode != LDAP_SUCCESS)
#else
   prepareStringForInit(m_connList);
   int port = m_secure ? LDAP_SSL_PORT : LDAP_PORT;
#ifdef _WIN32
   if (m_connList[0] == 0)
   {
      m_ldapConn = ldap_sslinit(NULL, port, m_secure);
   }
   else
   {
      DbgPrintf(4, _T("LDAPConnection::initLDAP(): servers=\"%s\" port=%d secure=%s"), m_connList, port, m_secure ? _T("yes") : _T("no"));
      m_ldapConn = ldap_sslinit(m_connList, port, m_secure);
   }
#else
   if(m_secure)
   {
#if HAVE_LDAPSSL_INIT
      ldapssl_init(m_connList, port, m_secure);
#else
      DbgPrintf(4, _T("LDAPConnection::initLDAP(): Your LDAP library does not support secure connection."));
#endif //HAVE_LDAPSSL_INIT
   }
   else
   {
      m_ldapConn = ldap_init(m_connList, LDAP_PORT);
   }
#endif // _WIN32

#ifdef _WIN32
   ULONG errorCode = LdapGetLastError();
#else
   int errorCode = errno;
#endif // _WIN32
   if (m_ldapConn == NULL)
#endif // HAVE_LDAP_INITIALIZE
   {
      TCHAR *error = getErrorString(errorCode);
      DbgPrintf(4, _T("LDAPConnection::initLDAP(): LDAP session initialization failed (%s)"), error);
      safe_free(error);
      return;
   }
   //set all LDAP options
   int version = LDAP_VERSION3;
   ldap_set_option(m_ldapConn, LDAP_OPT_PROTOCOL_VERSION, &version); //default verion 2, it's recomended to use version 3
   ldap_set_option(m_ldapConn, LDAP_OPT_REFERRALS, LDAP_OPT_OFF);
}

/**
 * Reads required synchronization parameters
 */
void LDAPConnection::getAllSyncParameters()
{
   TCHAR tmpPwd[MAX_PASSWORD];
   TCHAR tmpLogin[MAX_CONFIG_VALUE];
   ConfigReadStr(_T("LdapSyncUserPassword"), tmpPwd, MAX_PASSWORD, _T(""));
   ConfigReadStr(_T("LdapSyncUser"), tmpLogin, MAX_CONFIG_VALUE, _T(""));
   DecryptPassword(tmpLogin, tmpPwd, tmpPwd, MAX_PASSWORD);
   LdapConfigRead(_T("LdapConnectionString"), m_connList, MAX_CONFIG_VALUE, _TLDAP(""));
   LdapConfigRead(_T("LdapSyncUser"), m_userDN, MAX_CONFIG_VALUE, _TLDAP(""));
   LdapConfigRead(_T("LdapSearchBase"), m_searchBase, MAX_CONFIG_VALUE, _TLDAP(""));
   LdapConfigRead(_T("LdapSearchFilter"), m_searchFilter, MAX_CONFIG_VALUE, _TLDAP("(objectClass=*)"));
   if (m_searchFilter[0] == 0)
      ldap_strcpy(m_searchFilter, _TLDAP("(objectClass=*)"));
#ifdef _WIN32
   _tcsncpy(m_userPassword, tmpPwd, MAX_PASSWORD);
#else

#ifdef UNICODE
   char *utf8Password = UTF8StringFromWideString(tmpPwd);
   strcpy(m_userPassword, utf8Password);
   safe_free(utf8Password);
#else
   char *utf8Password = UTF8StringFromMBString(tmpPwd);
   strcpy(m_userPassword, utf8Password);
   safe_free(utf8Password);
#endif // UNICODE

#endif //win32
   ConfigReadStrUTF8(_T("LdapMappingName"), m_ldapLoginNameAttr, MAX_CONFIG_VALUE, "");
   ConfigReadStrUTF8(_T("LdapMappingFullName"), m_ldapFullNameAttr, MAX_CONFIG_VALUE, "");
   ConfigReadStrUTF8(_T("LdapMappingDescription"), m_ldapDescriptionAttr, MAX_CONFIG_VALUE, "");
   ConfigReadStrUTF8(_T("LdapUserUniqueId"), m_ldapUsreIdAttr, MAX_CONFIG_VALUE, "");
   ConfigReadStrUTF8(_T("LdapGroupUniqueId"), m_ldapGroupIdAttr, MAX_CONFIG_VALUE, "");
   ConfigReadStr(_T("LdapGroupClass"), m_groupClass, MAX_CONFIG_VALUE, _T(""));
   ConfigReadStr(_T("LdapUserClass"), m_userClass, MAX_CONFIG_VALUE, _T(""));
   m_action = ConfigReadInt(_T("LdapUserDeleteAction"), 1); //default value - to disable user(value=1)
   m_pageSize = ConfigReadInt(_T("LdapPageSize"), 1000); //default value - 1000
}

/**
 * Get users, according to search line & insetr in netxms DB missing
 */
void LDAPConnection::syncUsers()
{
   getAllSyncParameters();
   initLDAP();
   if (loginLDAP() != RCC_SUCCESS)
   {
      DbgPrintf(6, _T("LDAPConnection::syncUsers(): Could not login."));
      return;
   }

   struct ldap_timeval timeOut = { 10, 0 }; // 10 second connecion/search timeout
   LDAPMessage *searchResult;
   userDnEntryList = new StringObjectMap<Entry>(true); //as unique string ID is used dn
   userIdEntryList = new StringObjectMap<Entry>(false); //as unique string ID is used id
   groupDnEntryList= new StringObjectMap<Entry>(true); //as unique string ID is used dn
   groupIdEntryList= new StringObjectMap<Entry>(false); //as unique string ID is used id

   //Parse search base string. As separater is used ';' if this symbol is not escaped
   LDAP_CHAR *tmp  = ldap_strdup(m_searchBase);
   LDAP_CHAR *separator = tmp;
   LDAP_CHAR *base = tmp;
   size_t size = ldap_strlen(tmp);
   int rc = LDAP_SUCCESS;

   while (separator != NULL)
   {
      while (true)
      {
         separator = ldap_strchr(separator, ';');
         if(separator != NULL)
         {
            if ((separator - tmp) > 0)
            {
               if(separator[-1] != '\\')
               {
                  separator[0] = 0;
               }
               else
               {
                  separator++;
                  continue;
               }
            }
            else
            {
               base++;
               separator++;
               continue;
            }
         }
         break;
      }

      DbgPrintf(6, _T("LDAPConnection::syncUsers(): Search Base DN: ") LDAP_TFMT, base);
      rc = ldap_search_ext_s(
               m_ldapConn,		// LDAP session handle
               base,	// Search Base
               LDAP_SCOPE_SUBTREE,	// Search Scope – everything below o=Acme
               m_searchFilter, // Search Filter – only inetOrgPerson objects
               NULL,	// returnAllAttributes – NULL means Yes
               0,		// attributesOnly – False means we want values
               NULL,	// Server controls – There are none
               NULL,	// Client controls – There are none
               &timeOut,	// search Timeout
               LDAP_NO_LIMIT,	// no size limit
               &searchResult );

      if (rc != LDAP_SUCCESS)
      {
         if (rc == LDAP_SIZELIMIT_EXCEEDED)
         {
            rc = readInPages(base);
         }
         else
         {
            TCHAR* error = getErrorString(rc);
            DbgPrintf(1, _T("LDAPConnection::syncUsers(): LDAP could not get search results. Error code: %s"), error);
            safe_free(error);
         }
      }
      else
      {
         fillLists(searchResult);
      }

      ldap_msgfree(searchResult);
      if ((separator != NULL) && ((size_t)(separator - tmp) < size))
      {
         separator++;
         base = separator;
      }
      else
      {
         separator = NULL;
      }
   }

   safe_free(tmp);
   closeLDAPConnection();
   if(rc == LDAP_SUCCESS)
   {
      //compare new LDAP list with old users
      compareUserLists();
      compareGroupList();
   }

   delete userDnEntryList;
   delete userIdEntryList;
   delete groupDnEntryList;
   delete groupIdEntryList;
}

/**
 * Reads and process each page
 */
int LDAPConnection::readInPages(LDAP_CHAR *base)
{
   DbgPrintf(7, _T("LDAPConnection::readInPages(): Getting LDAP results as a pages."));
   LDAPControl *pageControl=NULL, *controls[2] = { NULL, NULL };
	LDAPControl **returnedControls = NULL;
#if defined(__sun)
   unsigned int pageSize = m_pageSize;
   unsigned int totalCount = 0;
#elif defined(_WIN32)
   ULONG pageSize = m_pageSize;
   ULONG totalCount = 0;
#else
   ber_int_t pageSize = m_pageSize;
   ber_int_t totalCount = 0;
#endif
   char pagingCriticality = 'T';
   struct berval *cookie = NULL;

   LDAPMessage *searchResult;
   struct ldap_timeval timeOut = { 10, 0 }; // 10 second connecion/search timeout
   int rc;

   do {
      rc = ldap_create_page_control(m_ldapConn, pageSize, cookie, pagingCriticality, &pageControl);
      if (rc != LDAP_SUCCESS)
      {
         TCHAR* error = getErrorString(rc);
         DbgPrintf(1, _T("LDAPConnection::readInPages(): LDAP could not create page control. Error code: %s"), error);
         break;
      }

      /* Insert the control into a list to be passed to the search. */
      controls[0] = pageControl;

      DbgPrintf(6, _T("LDAPConnection::syncUsers(): Search Base DN: ") LDAP_TFMT, base);
      /* Search for entries in the directory using the parmeters. */
      rc = ldap_search_ext_s(
               m_ldapConn,		// LDAP session handle
               base,	// Search Base
               LDAP_SCOPE_SUBTREE,	// Search Scope – everything below o=Acme
               m_searchFilter, // Search Filter – only inetOrgPerson objects
               NULL,	// returnAllAttributes – NULL means Yes
               0,		// attributesOnly – False means we want values
               controls,	// Server controls – Page controls
               NULL,	// Client controls – There are none
               &timeOut,	// search Timeout
               LDAP_NO_LIMIT,	// no size limit
               &searchResult );

      if ((rc != LDAP_SUCCESS) && (rc != LDAP_PARTIAL_RESULTS)) {
         TCHAR* error = getErrorString(rc);
         DbgPrintf(1, _T("LDAPConnection::readInPages(): Not possible to get result page. Error code: %s"), error);
         ldap_control_free(pageControl);
         break;
      }

      /* Parse the results to retrieve the contols being returned. */
      rc = ldap_parse_result(m_ldapConn, searchResult, NULL, NULL, NULL, NULL, &returnedControls, FALSE);
      fillLists(searchResult);
      ldap_msgfree(searchResult);

      /* Clear cookie */
      if (cookie != NULL) {
         ber_bvfree(cookie);
         cookie = NULL;
      }

      /*
       * Parse the page control returned to get the cookie and
       * determine whether there are more pages.
       */
      rc = ldap_parse_page_control(m_ldapConn, returnedControls, &totalCount, &cookie);

      /* Cleanup the controls used. */
      if (returnedControls)
         ldap_controls_free(returnedControls);

      ldap_control_free(pageControl);

   } while(cookie && cookie->bv_val && strlen(cookie->bv_val));
   if (cookie != NULL) {
      ber_bvfree(cookie);
   }
   return rc;
}

TCHAR *LDAPConnection::ldap_internal_get_dn(LDAP *conn, LDAPMessage *entry)
{
#ifdef _WIN32
   TCHAR *_dn = ldap_get_dn(m_ldapConn, entry);
   TCHAR *dn = _tcsdup(_dn);
#else
   char *_dn = ldap_get_dn(m_ldapConn, entry);
#ifdef UNICODE
   WCHAR *dn = WideStringFromUTF8String(_dn);
#else
   char *dn = MBStringFromUTF8String(_dn);
#endif
#endif
   ldap_memfree(_dn);
   return dn;
}

/**
 * Fills lists of users and groups from search results
 */
void LDAPConnection::fillLists(LDAPMessage *searchResult)
{
   LDAPMessage *entry;
   char *attribute;
   BerElement *ber;
	int i;
   DbgPrintf(4, _T("LDAPConnection::fillLists(): Found entry count: %d"), ldap_count_entries(m_ldapConn, searchResult));
   for (entry = ldap_first_entry(m_ldapConn, searchResult); entry != NULL; entry = ldap_next_entry(m_ldapConn, entry))
   {
      Entry *newObj = new Entry();
      TCHAR *dn = ldap_internal_get_dn(m_ldapConn, entry);
      DbgPrintf(4, _T("LDAPConnection::fillLists(): Found dn: %s"), dn);
      TCHAR *value;
      for(i = 0, value = getAttrValue(entry, "objectClass", i); value != NULL; value = getAttrValue(entry, "objectClass", ++i))
      {
         if(_tcscmp(value, m_userClass) == 0)
         {
            newObj->m_type = LDAP_USER;
            safe_free(value);
            break;
         }
         if(_tcscmp(value, m_groupClass) == 0)
         {
            newObj->m_type = LDAP_GROUP;
            safe_free(value);
            break;
         }
         safe_free(value);
      }

      if (newObj->m_type == LDAP_DEFAULT)
      {
         DbgPrintf(4, _T("LDAPConnection::fillLists(): %s is not a user nor a group"), dn);
         free(dn);
         delete newObj;
         continue;
      }

      for(attribute = ldap_first_attributeA(m_ldapConn, entry, &ber); attribute != NULL; attribute = ldap_next_attributeA(m_ldapConn, entry, ber))
      {
         // We get values only for those attributes that are used for user/group creation
         if (!strcmp(attribute, m_ldapFullNameAttr))
         {
            newObj->m_fullName = getAttrValue(entry, attribute);
         }
         if (!strcmp(attribute, m_ldapLoginNameAttr))
         {
            newObj->m_loginName = getAttrValue(entry, attribute);
         }
         if (!strcmp(attribute, m_ldapDescriptionAttr))
         {
            newObj->m_description = getAttrValue(entry, attribute);
         }
         if (m_ldapUsreIdAttr[0] != 0 && !strcmp(attribute, m_ldapUsreIdAttr) && newObj->m_type == LDAP_USER)
         {
            newObj->m_id = getIdAttrValue(entry, attribute);
         }
         if (m_ldapGroupIdAttr[0] != 0 && !strcmp(attribute, m_ldapGroupIdAttr) && newObj->m_type == LDAP_GROUP)
         {
            newObj->m_id = getIdAttrValue(entry, attribute);
         }
         if (!strcmp(attribute, "member"))
         {
            i = 0;
            TCHAR *value = getAttrValue(entry, attribute, i);
            while(value != NULL)
            {
               DbgPrintf(4, _T("LDAPConnection::fillLists(): member: %s"), value);
               newObj->m_memberList->addPreallocated(value);
               value = getAttrValue(entry, attribute, ++i);
            }
         }
         if (!strncmp(attribute, "member;range=", 13))
         {
            DbgPrintf(4, _T("LDAPConnection::fillLists(): found member attr: %hs"), attribute);
            DbgPrintf(4, _T("LDAPConnection::fillLists(): there are more members, than can be provided in one request"));
            //There are more members than can be provided in one request
#if !defined(_WIN32) && defined(UNICODE)
            char *tmpDn = UTF8StringFromWideString(dn);
            updateMembers(newObj->m_memberList, attribute, entry, tmpDn);
            free(tmpDn);
#else
            updateMembers(newObj->m_memberList, attribute, entry, dn);
#endif
         }
         ldap_memfreeA(attribute);
      }
      ber_free(ber, 0);

      // entry is added only if it was of a correct type
      if (newObj->m_type == LDAP_USER && newObj->m_loginName != NULL)
      {
         DbgPrintf(4, _T("LDAPConnection::fillLists(): User added: dn: %s, login name: %s, full name: %s, description: %s"), dn, newObj->m_loginName, CHECK_NULL(newObj->m_fullName), CHECK_NULL(newObj->m_description));
         userDnEntryList->set(dn, newObj);
         if(m_ldapUsreIdAttr[0] != 0 && newObj->m_id != NULL)
            userIdEntryList->set(newObj->m_id, newObj);
      }
      else if (newObj->m_type == LDAP_GROUP && newObj->m_loginName != NULL)
      {
         DbgPrintf(4, _T("LDAPConnection::fillLists(): Group added: dn: %s, login name: %s, full name: %s, description: %s"), dn, newObj->m_loginName, CHECK_NULL(newObj->m_fullName), CHECK_NULL(newObj->m_description));
         groupDnEntryList->set(dn, newObj);
         if(m_ldapGroupIdAttr[0] != 0 && newObj->m_id != NULL)
            groupIdEntryList->set(newObj->m_id, newObj);
      }
      else
      {
         DbgPrintf(4, _T("LDAPConnection::fillLists(): Unknown object is not added: dn: %s, login name: %s, full name: %s, description: %s"), dn, CHECK_NULL(newObj->m_loginName), CHECK_NULL(newObj->m_fullName), CHECK_NULL(newObj->m_description));
         delete newObj;
      }
      free(dn);
   }
}

/**
 * Get attribute's value
 */
TCHAR *LDAPConnection::getAttrValue(LDAPMessage *entry, const char *attr, UINT32 i)
{
   TCHAR *result = NULL;
   berval **values = ldap_get_values_lenA(m_ldapConn, entry, (char *)attr);   // cast needed for Windows LDAP library
   if (ldap_count_values_len(values) > i)
   {
#ifdef UNICODE
      result = WideStringFromUTF8String(values[i]->bv_val);
#else
      result = MBStringFromUTF8String(values[i]->bv_val);
#endif /* UNICODE */
   }
   ldap_value_free_len(values);
   return result;
}

/**
 * Get attribute's value
 */
TCHAR *LDAPConnection::getIdAttrValue(LDAPMessage *entry, const char *attr)
{
   BYTE hash[SHA256_DIGEST_SIZE];
   BYTE tmp[1024];
   memset(tmp, 0, 1024);
   berval **values = ldap_get_values_lenA(m_ldapConn, entry, (char *)attr);   // cast needed for Windows LDAP library
   int count = (int)ldap_count_values_len(values);
   int i,pos;
   for(i = 0, pos = 0; i < count; i++)
   {
      if (pos + values[i]->bv_len > 1024)
         break;
      memcpy(tmp+pos,values[i]->bv_val,values[i]->bv_len);
      pos += values[i]->bv_len;
   }
   ldap_value_free_len(values);
   if (i == 0)
      return _tcsdup(_T(""));

   CalculateSHA256Hash(tmp, pos, hash);
   TCHAR *result = (TCHAR *)malloc(sizeof(TCHAR) * (SHA256_DIGEST_SIZE * 2 + 1));
   BinToStr(hash, SHA256_DIGEST_SIZE, result);
   return result;
}

/**
 * Parse range information from attribute
 */
static void ParseRange(const char *attr, int *start, int *end)
{
   *end  = -1;
   *start = -1;

   const char *tmpS = strchr(attr, '=');
   if (tmpS == NULL)
      return;

   char *tmpAttr = strdup(tmpS + 1);
   char *tmpE = strchr(tmpAttr, '-');
   if (tmpE == NULL)
   {
      free(tmpAttr);
      return;
   }
   *tmpE = 0;
   tmpE++;
   *start = atoi(tmpAttr);
   if (tmpE[0] != '*')
   {
      *end = atoi(tmpE);
   }
   free(tmpAttr);
}

/**
 * Update group members
 */
void LDAPConnection::updateMembers(StringSet *memberList, const char *firstAttr, LDAPMessage *firstEntry, const LDAP_CHAR *dn)
{
   int start, end;

   // get start, end member count
   ParseRange(firstAttr, &start, &end);

   // add received members
   int i = 0;
   TCHAR *value = getAttrValue(firstEntry, firstAttr, i);
   while(value != NULL)
   {
      DbgPrintf(4, _T("LDAPConnection::updateMembers(): member: %s"), value);
      memberList->addPreallocated(value);
      value = getAttrValue(firstEntry, firstAttr, ++i);
   }

   LDAPMessage *searchResult;
   LDAPMessage *entry;
   char *attribute;
   BerElement *ber;
   LDAP_CHAR *requiredAttr[2];
   LDAP_CHAR memberAttr[32];
   requiredAttr[1] = NULL;

   const LDAP_CHAR *filter = _TLDAP("(objectClass=*)");

   while(true)
   {
      // request next members
      struct ldap_timeval timeOut = { 10, 0 }; // 10 second connecion/search timeout
      ldap_snprintf(memberAttr, 32, _TLDAP("member;range=%d-*"), end + 1);
      requiredAttr[0] = memberAttr;
      DbgPrintf(4, _T("LDAPConnection::updateMembers(): request members id ") LDAP_TFMT _T(" group: ") LDAP_TFMT, dn, memberAttr);

      int rc = ldap_search_ext_s(
               m_ldapConn,		// LDAP session handle
               (LDAP_CHAR *)dn,	// Search Base
               LDAP_SCOPE_SUBTREE,	// Search Scope – everything below o=Acme
               (LDAP_CHAR *)filter, // Search Filter – only inetOrgPerson objects
               requiredAttr,	// returnAllAttributes – NULL means Yes
               0,		// attributesOnly – False means we want values
               NULL,	// Server controls – There are none
               NULL,	// Client controls – There are none
               &timeOut,	// search Timeout
               LDAP_NO_LIMIT,	// no size limit
               &searchResult);

      if(rc != LDAP_SUCCESS)
      {
         TCHAR* error = getErrorString(rc);
         DbgPrintf(1, _T("LDAPConnection::syncUsers(): LDAP could not get search results. Error code: %s"), error);
         safe_free(error);
         break;
      }

      bool found = false;
      DbgPrintf(4, _T("LDAPConnection::fillLists(): Found entry count: %d"), ldap_count_entries(m_ldapConn, searchResult));
      for(entry = ldap_first_entry(m_ldapConn, searchResult); entry != NULL; entry = ldap_next_entry(m_ldapConn, entry))
      {
         for(attribute = ldap_first_attributeA(m_ldapConn, entry, &ber); attribute != NULL; attribute = ldap_next_attributeA(m_ldapConn, entry, ber))
         {
            if (!strncmp(attribute, "member;range=", 13))
            {
               // add received members
               i = 0;
               TCHAR *value = getAttrValue(entry, attribute, i);
               if(value != NULL)
               {
                  ParseRange(attribute, &start, &end);
                  found = true;
               }

               while(value != NULL)
               {
                  DbgPrintf(4, _T("LDAPConnection::updateMembers(): member: %s"), value);
                  memberList->addPreallocated(value);
                  value = getAttrValue(entry, attribute, ++i);
               }
            }
            ldap_memfreeA(attribute);
         }
         ber_free(ber, 0);
      }
      ldap_msgfree(searchResult);

      if (end == -1 || !found)
         break;

      if (start == -1)
      {
         DbgPrintf(4, _T("LDAPConnection::updateMembers(): member start interval returned as: %d"), start);
         break;
      }
   }
}

/**
 * Update user callback
 */
static EnumerationCallbackResult UpdateUserCallback(const TCHAR *key, const void *value, void *data)
{
   UpdateLDAPUser(key, (Entry *)value);
   return _CONTINUE;
}

/**
 * Updates user list according to newly recievd user list
 */
void LDAPConnection::compareUserLists()
{
   userDnEntryList->forEach(UpdateUserCallback, NULL);
   RemoveDeletedLDAPEntries(userDnEntryList, userIdEntryList, m_action, true);
}

/**
 * Update group callback
 */
static EnumerationCallbackResult UpdateGroupCallback(const TCHAR *key, const void *value, void *data)
{
   UpdateLDAPGroup(key, (Entry *)value);
   return _CONTINUE;
}

/**
 * Updates group list according to newly recievd user list
 */
void LDAPConnection::compareGroupList()
{
   groupDnEntryList->forEach(UpdateGroupCallback, NULL);
   RemoveDeletedLDAPEntries(groupDnEntryList, groupIdEntryList, m_action, false);
}

/**
 * Coverts given parameters to correct encoding, login to ldap and close connection.
 * This function should be used to check connection. As name should be given user dn.
 */
UINT32 LDAPConnection::ldapUserLogin(const TCHAR *name, const TCHAR *password)
{
   getAllSyncParameters();
   initLDAP();
   UINT32 result;
#ifdef UNICODE
#ifdef _WIN32
   nx_strncpy(m_userDN, name, MAX_CONFIG_VALUE);
   nx_strncpy(m_userPassword, password, MAX_CONFIG_VALUE);
#else
   char *utf8Name = UTF8StringFromWideString(name);
   strcpy(m_userDN, utf8Name);
   safe_free(utf8Name);
   char *utf8Password = UTF8StringFromWideString(password);
   strcpy(m_userPassword, utf8Password);
   safe_free(utf8Password);
#endif
#else
   strcpy(m_userDN, name);
   strcpy(m_userPassword, password);
#endif // UNICODE
   result = loginLDAP();
   closeLDAPConnection();
   return result;
}

/**
 * Autentificate LDAP user
 */
 UINT32 LDAPConnection::loginLDAP()
 {
   int ldap_error;

// Prevent empty password, bind against ADS will succeed with
// empty password by default.
   if(ldap_strlen(m_userPassword) == 0)
   {
      return RCC_ACCESS_DENIED;
   }

   if (m_ldapConn != NULL)
   {
#ifdef _WIN32
      ldap_error = ldap_simple_bind_s(m_ldapConn, m_userDN, m_userPassword);
#else
      struct berval cred;
      cred.bv_val = m_userPassword;
      cred.bv_len = (int)strlen(m_userPassword);

      ldap_error = ldap_sasl_bind_s(m_ldapConn, m_userDN, LDAP_SASL_SIMPLE, &cred, NULL, NULL, NULL);
#endif
      if (ldap_error == LDAP_SUCCESS)
      {
         return RCC_SUCCESS;
      }
      else
      {
         TCHAR *error = getErrorString(ldap_error);
         DbgPrintf(4, _T("LDAPConnection::loginLDAP(): cannot login to LDAP server (%s)"), error);
         free(error);
      }
   }
   else
   {
      return RCC_NO_LDAP_CONNECTION;
   }
   return RCC_ACCESS_DENIED;
 }

 /**
  * Converts error string and returns in right format.
  * Memory should be released by caller.
  */
TCHAR *LDAPConnection::getErrorString(int ldap_error)
{
#ifdef _WIN32
   return _tcsdup(ldap_err2string(ldap_error));
#else
#ifdef UNICODE
   return WideStringFromUTF8String(ldap_err2string(ldap_error));
#else
   return MBStringFromUTF8String(ldap_err2string(ldap_error));
#endif
#endif
}

/**
 * Close LDAP connection
 */
void LDAPConnection::closeLDAPConnection()
{
   DbgPrintf(4, _T("LDAPConnection::closeLDAPConnection(): Disconnect form LDAP server"));
   if(m_ldapConn != NULL)
   {
#ifdef _WIN32
      ldap_unbind_s(m_ldapConn);
#else
      ldap_unbind_ext(m_ldapConn, NULL, NULL);
#endif
      m_ldapConn = NULL;
   }
}

/**
 * Defeult LDAPConnection class construtor
 */
LDAPConnection::LDAPConnection()
{
   m_ldapConn = NULL;
   m_action = 1;
   m_secure = 0;
   m_pageSize = 1000;
}

/**
 * Destructor
 */
LDAPConnection::~LDAPConnection()
{
}

#else	/* WITH_LDAP */

/**
 * Synchronize users - stub for server without LDAP support
 */
void LDAPConnection::syncUsers()
{
    DbgPrintf(4, _T("LDAPConnection::syncUsers(): FAILED - server was compiled without LDAP support"));
}

/**
 * Login via LDAP - stub for server without LDAP support
 */
UINT32 LDAPConnection::ldapUserLogin(const TCHAR *name, const TCHAR *password)
{
   DbgPrintf(4, _T("LDAPConnection::ldapUserLogin(): FAILED - server was compiled without LDAP support"));
   return RCC_INTERNAL_ERROR;
}

#endif

/**
 * Get users, according to search line & insetr in netxms DB missing
 */
THREAD_RESULT THREAD_CALL SyncLDAPUsers(void *arg)
{
   ThreadSetName("LDAPSync");
   UINT32 syncInterval = ConfigReadInt(_T("LdapSyncInterval"), 0);
   if (syncInterval == 0)
   {
      DbgPrintf(1, _T("SyncLDAPUsers: sync thread will not start because LDAP sync is disabled"));
      return THREAD_OK;
   }

   DbgPrintf(1, _T("SyncLDAPUsers: sync thread started, interval %d minutes"), syncInterval);
   syncInterval *= 60;
   while(!SleepAndCheckForShutdown(syncInterval))
   {
      LDAPConnection conn;
      conn.syncUsers();
   }
   DbgPrintf(1, _T("SyncLDAPUsers: sync thread stopped"));
   return THREAD_OK;
}
