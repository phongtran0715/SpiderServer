/* 
** NetXMS - Network Management System
** Driver for Nortel WLAN Security Switch series
** Copyright (C) 2013-2016 Raden Solutions
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
**/

#include "ntws.h"
#include <math.h>

/**
 * Static data
 */
static TCHAR s_driverName[] = _T("NTWS");
static TCHAR s_driverVersion[] = NETXMS_VERSION_STRING;

/**
 * Get driver name
 */
const TCHAR *NtwsDriver::getName()
{
   return s_driverName;
}

/**
 * Get driver version
 */
const TCHAR *NtwsDriver::getVersion()
{
   return s_driverVersion;
}

/**
 * Check if given device can be potentially supported by driver
 *
 * @param oid Device OID
 */
int NtwsDriver::isPotentialDevice(const TCHAR *oid)
{
   return (_tcsncmp(oid, _T(".1.3.6.1.4.1.45.6.1.3.1."), 24) == 0) ? 127 : 0;
}

/**
 * Check if given device is supported by driver
 *
 * @param snmp SNMP transport
 * @param oid Device OID
 */
bool NtwsDriver::isDeviceSupported(SNMP_Transport *snmp, const TCHAR *oid)
{
   TCHAR buffer[1024];
   return SnmpGet(snmp->getSnmpVersion(), snmp, _T(".1.3.6.1.4.1.45.6.1.4.2.1.1.0"), NULL, 0, buffer, sizeof(buffer), 0) == SNMP_ERR_SUCCESS;
}

/**
 * Do additional checks on the device required by driver.
 * Driver can set device's custom attributes from within
 * this function.
 *
 * @param snmp SNMP transport
 * @param attributes Node's custom attributes
 */
void NtwsDriver::analyzeDevice(SNMP_Transport *snmp, const TCHAR *oid, StringMap *attributes, DriverData **driverData)
{
}

/**
 * Get cluster mode for device (standalone / active / standby)
 *
 * @param snmp SNMP transport
 * @param attributes Node custom attributes
 * @param driverData optional pointer to user data
 */
int NtwsDriver::getClusterMode(SNMP_Transport *snmp, StringMap *attributes, DriverData *driverData)
{
   return CLUSTER_MODE_UNKNOWN;
}

/*
 * Check switch for wireless capabilities
 *
 * @param snmp SNMP transport
 * @param attributes Node custom attributes
 * @param driverData optional pointer to user data
 */
bool NtwsDriver::isWirelessController(SNMP_Transport *snmp, StringMap *attributes, DriverData *driverData)
{
   return true;
}

/**
 * Handler for access point enumeration - unadopted
 */
static UINT32 HandlerAccessPointListUnadopted(SNMP_Variable *var, SNMP_Transport *transport, void *arg)
{
   ObjectArray<AccessPointInfo> *apList = (ObjectArray<AccessPointInfo> *)arg;

   TCHAR model[128];
   AccessPointInfo *info = new AccessPointInfo(0, (const BYTE *)"\x00\x00\x00\x00\x00\x00", InetAddress::INVALID, AP_UNADOPTED, NULL, NULL, var->getValueAsString(model, 128), NULL);
   apList->add(info);

   return SNMP_ERR_SUCCESS;
}

/**
 * Handler for access point enumeration - adopted
 */
static UINT32 HandlerAccessPointListAdopted(SNMP_Variable *var, SNMP_Transport *transport, void *arg)
{
   int ret = SNMP_ERR_SUCCESS;

   ObjectArray<AccessPointInfo> *apList = (ObjectArray<AccessPointInfo> *)arg;

   const SNMP_ObjectId& name = var->getName();
   size_t nameLen = name.length();

   UINT32 oid[128];
   memcpy(oid, name.value(), nameLen * sizeof(UINT32));

   // get serial number - it's encoded in OID as <length>.<serial>
   TCHAR serial[128];
   UINT32 slen = oid[16];
   for(UINT32 i = 0; i < slen; i++)
      serial[i] = oid[i + 17];
   serial[slen] = 0;

   SNMP_PDU *request = new SNMP_PDU(SNMP_GET_REQUEST, SnmpNewRequestId(), transport->getSnmpVersion());
   
   oid[15] = 6;  // ntwsApStatApStatusModel
   request->bindVariable(new SNMP_Variable(oid, nameLen));

   oid[15] = 8;  // ntwsApStatApStatusApName
   request->bindVariable(new SNMP_Variable(oid, nameLen));

   oid[15] = 10; // ntwsApStatApStatusIpAddress
   request->bindVariable(new SNMP_Variable(oid, nameLen));

   oid[15] = 13; // ntwsApStatApStatusManufacturerId
   request->bindVariable(new SNMP_Variable(oid, nameLen));

   oid[15] = 19; // ntwsApStatApStatusApNum
   request->bindVariable(new SNMP_Variable(oid, nameLen));

   SNMP_PDU *response;
   if (transport->doRequest(request, &response, SnmpGetDefaultTimeout(), 3) == SNMP_ERR_SUCCESS)
   {
      if (response->getNumVariables() >= 5)
      {
         TCHAR model[256], name[256], vendor[256], ipAddr[32];
         AccessPointInfo *ap = 
            new AccessPointInfo(
               response->getVariable(4)->getValueAsUInt(),
               var->getValue(),
               ntohl(_t_inet_addr(response->getVariable(2)->getValueAsString(ipAddr, 32))), 
               AP_ADOPTED, 
               response->getVariable(1)->getValueAsString(name, 256),
               response->getVariable(3)->getValueAsString(vendor, 256),
               response->getVariable(0)->getValueAsString(model, 256),
               serial);
         apList->add(ap);
      }
      delete response;
   }
   delete request;

   return ret;
}

/**
 * Handler for radios enumeration
 */
static UINT32 HandlerRadioList(SNMP_Variable *var, SNMP_Transport *transport, void *arg)
{
   AccessPointInfo *ap = (AccessPointInfo *)arg;

   const SNMP_ObjectId& name = var->getName();
   size_t nameLen = name.length();

   UINT32 oid[128];
   memcpy(oid, name.value(), nameLen * sizeof(UINT32));

   RadioInterfaceInfo rif;
   memcpy(rif.macAddr, var->getValue(), MAC_ADDR_LENGTH);
   rif.index = (int)oid[nameLen - 1];
   _sntprintf(rif.name, sizeof(rif.name) / sizeof(TCHAR), _T("Radio%d"), rif.index);
   
   SNMP_PDU *request = new SNMP_PDU(SNMP_GET_REQUEST, SnmpNewRequestId(), transport->getSnmpVersion());
   
   oid[15] = 6;  // ntwsApStatRadioStatusCurrentPowerLevel
   request->bindVariable(new SNMP_Variable(oid, nameLen));

   oid[15] = 7;  // ntwsApStatRadioStatusCurrentChannelNum
   request->bindVariable(new SNMP_Variable(oid, nameLen));

   SNMP_PDU *response;
   if (transport->doRequest(request, &response, SnmpGetDefaultTimeout(), 3) == SNMP_ERR_SUCCESS)
   {
      if (response->getNumVariables() >= 2)
      {
         rif.powerDBm = response->getVariable(0)->getValueAsInt();
         rif.powerMW = (int)pow(10.0, (double)rif.powerDBm / 10.0);
         rif.channel = response->getVariable(1)->getValueAsUInt();
         ap->addRadioInterface(&rif);
      }
      delete response;
   }
   delete request;
   return SNMP_ERR_SUCCESS;
}

/*
 * Get access points
 *
 * @param snmp SNMP transport
 * @param attributes Node custom attributes
 * @param driverData optional pointer to user data
 */
ObjectArray<AccessPointInfo> *NtwsDriver::getAccessPoints(SNMP_Transport *snmp, StringMap *attributes, DriverData *driverData)
{
   ObjectArray<AccessPointInfo> *apList = new ObjectArray<AccessPointInfo>(0, 16, true);

   // Adopted
   if (SnmpWalk(snmp, _T(".1.3.6.1.4.1.45.6.1.4.5.1.1.2.1.2"),
                HandlerAccessPointListAdopted, apList) == SNMP_ERR_SUCCESS)
   {
      // Read radios by walking .1.3.6.1.4.1.45.6.1.4.5.1.1.4.1.3.<ap serial> (ntwsApStatRadioStatusBaseMac)
      for(int i = 0; i < apList->size(); i++)
      {
         AccessPointInfo *ap = apList->get(i);

         UINT32 oid[256] = { 1, 3, 6, 1, 4, 1, 45, 6, 1, 4, 5, 1, 1, 4, 1, 3 };
         const TCHAR *serial = ap->getSerial();
         oid[16] = (UINT32)_tcslen(serial);
         for(UINT32 i = 0; i < oid[16]; i++)
            oid[i + 17] = (UINT32)serial[i];
         SnmpWalk(snmp, oid, oid[16] + 17, HandlerRadioList, ap);
      }
   }
   else
   {
      delete apList;
      return NULL;
   }

   // Unadopted
   if (SnmpWalk(snmp, _T(".1.3.6.1.4.1.45.6.1.4.15.1.2.1.2"),
                HandlerAccessPointListUnadopted, apList) != SNMP_ERR_SUCCESS)
   {
      delete apList;
      return NULL;
   }

   return apList;
}

/**
 * Handler for mobile units enumeration
 */
static UINT32 HandlerWirelessStationList(SNMP_Variable *var, SNMP_Transport *transport, void *arg)
{
   int ret = SNMP_ERR_SUCCESS;

   ObjectArray<WirelessStationInfo> *wsList = (ObjectArray<WirelessStationInfo> *)arg;

   const SNMP_ObjectId& name = var->getName();
   size_t nameLen = name.length();
   const UINT32 *value = name.value();

   UINT32 oid[32];
   memcpy(oid, value, nameLen * sizeof(value[0]));

   UINT32 ipAddr;
   oid[14] = 6; // wsCcRfMuIpAddr
   ret = SnmpGetEx(transport, NULL, oid, sizeof(oid) / sizeof(oid[0]), &ipAddr, sizeof(ipAddr), 0, NULL);
   UINT32 vlanInfex;
   if (ret == SNMP_ERR_SUCCESS)
   {
      oid[14] = 4; // wsCcRfMuVlanIndex
      ret = SnmpGetEx(transport, NULL, oid, sizeof(oid) / sizeof(oid[0]), &vlanInfex, sizeof(vlanInfex), 0, NULL);
   }

   UINT32 wlanInfex;
   if (ret == SNMP_ERR_SUCCESS)
   {
      oid[14] = 2; // wsCcRfMuWlanIndex
      ret = SnmpGetEx(transport, NULL, oid, sizeof(oid) / sizeof(oid[0]), &wlanInfex, sizeof(vlanInfex), 0, NULL);
   }

   UINT32 rfIndex;
   if (ret == SNMP_ERR_SUCCESS)
   {
      oid[14] = 3; // wsCcRfMuRadioIndex
      ret = SnmpGetEx(transport, NULL, oid, sizeof(oid) / sizeof(oid[0]), &rfIndex, sizeof(rfIndex), 0, NULL);
   }

   TCHAR ssid[MAX_OBJECT_NAME];
   if (ret == SNMP_ERR_SUCCESS)
   {
      UINT32 wlanOid[] = { 1, 3, 6, 1, 4, 1, 388, 14, 3, 2, 1, 14, 1, 1, 4, 0 };
      wlanOid[(sizeof(wlanOid) / sizeof(wlanOid[0])) - 1] = wlanInfex;

      ret = SnmpGetEx(transport, NULL, wlanOid, sizeof(wlanOid) / sizeof(wlanOid[0]), ssid, sizeof(ssid), 0, NULL);
   }

   if (ret == SNMP_ERR_SUCCESS)
   {
      WirelessStationInfo *info = new WirelessStationInfo;
      memset(info, 0, sizeof(WirelessStationInfo));

      memcpy(info->macAddr, var->getValue(), MAC_ADDR_LENGTH);
      info->ipAddr = ipAddr;
      info->vlan = vlanInfex;
      nx_strncpy(info->ssid, ssid, MAX_OBJECT_NAME);
      info->rfIndex = rfIndex;
      info->apMatchPolicy = AP_MATCH_BY_RFINDEX;

      wsList->add(info);
   }

   return ret;
}

/*
 *
 * @param snmp SNMP transport
 * @param attributes Node custom attributes
 * @param driverData optional pointer to user data
 */
ObjectArray<WirelessStationInfo> *NtwsDriver::getWirelessStations(SNMP_Transport *snmp, StringMap *attributes, DriverData *driverData)
{
   ObjectArray<WirelessStationInfo> *wsList = new ObjectArray<WirelessStationInfo>(0, 16, true);

   if (SnmpWalk(snmp, _T(".1.3.6.1.4.1.388.14.3.2.1.12.3.1.1"), // wsCcRfMuMac
                HandlerWirelessStationList, wsList) != SNMP_ERR_SUCCESS)
   {
      delete wsList;
      wsList = NULL;
   }

   return wsList;
}

/**
 * Driver entry point
 */
DECLARE_NDD_ENTRY_POINT(s_driverName, NtwsDriver);

/**
 * DLL entry point
 */
#ifdef _WIN32

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
   if (dwReason == DLL_PROCESS_ATTACH)
   {
      DisableThreadLibraryCalls(hInstance);
   }

   return TRUE;
}

#endif
