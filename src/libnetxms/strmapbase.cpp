/* 
** NetXMS - Network Management System
** NetXMS Foundation Library
** Copyright (C) 2003-2014 Victor Kirhenshtein
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
** File: strmapbase.cpp
**
**/

#include "libnetxms.h"
#include "strmap-internal.h"

/**
 * Standard object destructor
 */
static void ObjectDestructor(void *object)
{
	free(object);
}

/**
 * Constructors
 */
StringMapBase::StringMapBase(bool objectOwner)
{
	m_data = NULL;
	m_objectOwner = objectOwner;
   m_ignoreCase = true;
	m_objectDestructor = ObjectDestructor;
}

/**
 * Destructor
 */
StringMapBase::~StringMapBase()
{
	clear();
}

/**
 * Clear map
 */
void StringMapBase::clear()
{
   StringMapEntry *entry, *tmp;
   HASH_ITER(hh, m_data, entry, tmp)
   {
      HASH_DEL(m_data, entry);
      free(entry->key);
      safe_free(entry->originalKey);
      if (m_objectOwner)
         destroyObject(entry->value);
      free(entry);
   }
}

/**
 * Find entry index by key
 */
StringMapEntry *StringMapBase::find(const TCHAR *key, int keyLen) const
{
	if (key == NULL)
		return NULL;

   StringMapEntry *entry;
   if (m_ignoreCase)
   {
#if HAVE_ALLOCA
      TCHAR *ukey = (TCHAR *)alloca(keyLen + sizeof(TCHAR));
#else
      TCHAR *ukey = (TCHAR *)malloc(keyLen + sizeof(TCHAR));
#endif
      memcpy(ukey, key, keyLen + sizeof(TCHAR));
      _tcsupr(ukey);
      HASH_FIND(hh, m_data, ukey, keyLen, entry);
#if !HAVE_ALLOCA
      free(ukey);
#endif
   }
   else
   {
      HASH_FIND(hh, m_data, key, keyLen, entry);
   }
   return entry;
}

/**
 * Set value
 */
void StringMapBase::setObject(TCHAR *key, void *value, bool keyPreAllocated)
{
   if (key == NULL)
      return;

	StringMapEntry *entry = find(key, (int)_tcslen(key) * sizeof(TCHAR));
	if (entry != NULL)
	{
		if (keyPreAllocated)
      {
         if (m_ignoreCase)
         {
            free(entry->originalKey);
            entry->originalKey = key;
         }
         else
         {
			   free(key);
         }
      }
      else if (m_ignoreCase)
      {
         free(entry->originalKey);
         entry->originalKey = _tcsdup(key);
      }
		if (m_objectOwner)
         destroyObject(entry->value);
      entry->value = value;
	}
	else
	{
      entry = (StringMapEntry *)malloc(sizeof(StringMapEntry));
      entry->key = keyPreAllocated ? key : _tcsdup(key);
      if (m_ignoreCase)
      {
         entry->originalKey = _tcsdup(entry->key);
         _tcsupr(entry->key);
      }
      else
      {
         entry->originalKey = NULL;
      }
      int keyLen = (int)(_tcslen(key) * sizeof(TCHAR));
      entry->value = value;
      HASH_ADD_KEYPTR(hh, m_data, entry->key, keyLen, entry);
	}
}

/**
 * Get value by key
 */
void *StringMapBase::getObject(const TCHAR *key) const
{
   if (key == NULL)
      return NULL;
	StringMapEntry *entry = find(key, (int)_tcslen(key) * sizeof(TCHAR));
   return (entry != NULL) ? entry->value : NULL;
}

/**
 * Get value by key
 */
void *StringMapBase::getObject(const TCHAR *key, size_t len) const
{
   if (key == NULL)
      return NULL;
   StringMapEntry *entry = find(key, (int)len * sizeof(TCHAR));
   return (entry != NULL) ? entry->value : NULL;
}

/**
 * Delete value
 */
void StringMapBase::remove(const TCHAR *key)
{
   StringMapEntry *entry = find(key, (int)_tcslen(key) * sizeof(TCHAR));
   if (entry != NULL)
   {
      HASH_DEL(m_data, entry);
      free(entry->key);
      free(entry->originalKey);
		if (m_objectOwner)
         destroyObject(entry->value);
      free(entry);
   }
}

/**
 * Enumerate entries
 * Returns _CONTINUE if whole map was enumerated and _STOP if enumeration was aborted by callback.
 */
EnumerationCallbackResult StringMapBase::forEach(EnumerationCallbackResult (*cb)(const TCHAR *, const void *, void *), void *userData) const
{
   EnumerationCallbackResult result = _CONTINUE;
   StringMapEntry *entry, *tmp;
   HASH_ITER(hh, m_data, entry, tmp)
   {
      if (cb(m_ignoreCase ? entry->originalKey : entry->key, entry->value, userData) == _STOP)
      {
         result = _STOP;
         break;
      }
   }
   return result;
}

/**
 * Find entry
 */
const void *StringMapBase::findElement(bool (*comparator)(const TCHAR *, const void *, void *), void *userData) const
{
   const void *result = NULL;
   StringMapEntry *entry, *tmp;
   HASH_ITER(hh, m_data, entry, tmp)
   {
      if (comparator(m_ignoreCase ? entry->originalKey : entry->key, entry->value, userData))
      {
         result = entry->value;
         break;
      }
   }
   return result;
}

/**
 * Filter elements (delete those for which filter callback returns false)
 */
void StringMapBase::filterElements(bool (*filter)(const TCHAR *, const void *, void *), void *userData)
{
   StringMapEntry *entry, *tmp;
   HASH_ITER(hh, m_data, entry, tmp)
   {
      if (!filter(m_ignoreCase ? entry->originalKey : entry->key, entry->value, userData))
      {
         HASH_DEL(m_data, entry);
         free(entry->key);
         safe_free(entry->originalKey);
         if (m_objectOwner)
            destroyObject(entry->value);
         free(entry);
      }
   }
}

/**
 * Convert to key/value array
 */
StructArray<KeyValuePair> *StringMapBase::toArray() const
{
   StructArray<KeyValuePair> *a = new StructArray<KeyValuePair>(size());
   StringMapEntry *entry, *tmp;
   HASH_ITER(hh, m_data, entry, tmp)
   {
      KeyValuePair p;
      p.key = m_ignoreCase ? entry->originalKey : entry->key;
      p.value = entry->value;
      a->add(&p);
   }
   return a;
}

/**
 * Get list of all keys
 */
StringList *StringMapBase::keys() const
{
   StringList *list = new StringList();
   StringMapEntry *entry, *tmp;
   HASH_ITER(hh, m_data, entry, tmp)
   {
      list->add(m_ignoreCase ? entry->originalKey : entry->key);
   }
   return list;
}

/**
 * Get size
 */
int StringMapBase::size() const
{
   return HASH_COUNT(m_data);
}

/**
 * Change case sensitivity mode
 */
void StringMapBase::setIgnoreCase(bool ignore)
{ 
   if (m_ignoreCase == ignore)
      return;  // No change required

   m_ignoreCase = ignore;
   if (m_data == NULL)
      return;  // Empty set

   StringMapEntry *data = NULL;
   StringMapEntry *entry, *tmp;
   if (m_ignoreCase)
   {
      // switching to case ignore mode
      HASH_ITER(hh, m_data, entry, tmp)
      {
         HASH_DEL(m_data, entry);
         entry->originalKey = _tcsdup(entry->key);
         _tcsupr(entry->key);
         int keyLen = (int)(_tcslen(entry->key) * sizeof(TCHAR));
         HASH_ADD_KEYPTR(hh, data, entry->key, keyLen, entry);
      }
   }
   else
   {
      // switching to case sensitive mode
      HASH_ITER(hh, m_data, entry, tmp)
      {
         HASH_DEL(m_data, entry);
         free(entry->key);
         entry->key = entry->originalKey;
         entry->originalKey = NULL;
         int keyLen = (int)(_tcslen(entry->key) * sizeof(TCHAR));
         HASH_ADD_KEYPTR(hh, data, entry->key, keyLen, entry);
      }
   }
   m_data = data;
}
