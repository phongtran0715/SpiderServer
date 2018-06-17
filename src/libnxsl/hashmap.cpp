/* 
** NetXMS - Network Management System
** NetXMS Scripting Language Interpreter
** Copyright (C) 2003-2016 Victor Kirhenshtein
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 3 of the License, or
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
** File: hashmap.cpp
**
**/

#include "libnxsl.h"

/**
 * Create empty hash map
 */
NXSL_HashMap::NXSL_HashMap() : NXSL_HandleCountObject()
{
   m_values = new StringObjectMap<NXSL_Value>(true);
}

/**
 * Create copy of existing hash map
 */
NXSL_HashMap::NXSL_HashMap(const NXSL_HashMap *src) : NXSL_HandleCountObject()
{
   m_values = new StringObjectMap<NXSL_Value>(true);
   StructArray<KeyValuePair> *values = src->m_values->toArray();
   for(int i = 0; i < values->size(); i++)
   {
      KeyValuePair *p = values->get(i);
      m_values->set(p->key, new NXSL_Value((const NXSL_Value *)p->value));
   }
   delete values;
}

/**
 * Destructor
 */
NXSL_HashMap::~NXSL_HashMap()
{
   delete m_values;
}

/**
 * Get keys as array
 */
NXSL_Value *NXSL_HashMap::getKeys() const
{
   NXSL_Array *array = new NXSL_Array();
   StructArray<KeyValuePair> *values = m_values->toArray();
   for(int i = 0; i < values->size(); i++)
   {
      KeyValuePair *p = values->get(i);
      array->append(new NXSL_Value(p->key));
   }
   delete values;
   return new NXSL_Value(array);
}

/**
 * Get values as array
 */
NXSL_Value *NXSL_HashMap::getValues() const
{
   NXSL_Array *array = new NXSL_Array();
   StructArray<KeyValuePair> *values = m_values->toArray();
   for(int i = 0; i < values->size(); i++)
   {
      KeyValuePair *p = values->get(i);
      array->append(new NXSL_Value((const NXSL_Value *)p->value));
   }
   delete values;
   return new NXSL_Value(array);
}

/**
 * Get hash map as string map
 * Resulting string map is dynamically allocated and should be destroyed by caller
 */
StringMap *NXSL_HashMap::toStringMap() const
{
   StringMap *map = new StringMap();
   StructArray<KeyValuePair> *values = m_values->toArray();
   for(int i = 0; i < values->size(); i++)
   {
      KeyValuePair *p = values->get(i);
      const TCHAR *s = ((NXSL_Value *)p->value)->getValueAsCString();
      if (s != NULL)
         map->set(p->key, s);
   }
   delete values;
   return map;
}
