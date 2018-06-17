/*
** NetXMS SMS sending subagent
** Copyright (C) 2006-2014 Raden Solutions
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
** File: sender.cpp
**
**/

#include "sms.h"

bool SMSCreatePDUString(const char* phoneNumber, const char* message, char* pduBuffer);

/**
 * Static data
 */
static Serial s_serial;
static const char *s_eosMarks[] = { "OK", "ERROR", NULL };
static const char *s_eosMarksSend[] = { ">", "ERROR", NULL };
static enum { OM_TEXT, OM_PDU } s_operationMode = OM_TEXT;

/**
 * Read input to OK
 */
static bool ReadToOK(Serial *serial, char *data = NULL)
{
   char buffer[1024];
   memset(buffer, 0, 1024);
   while(true)
   {
      char *mark;
      int rc = serial->readToMark(buffer, 1024, s_eosMarks, &mark);
      if (rc <= 0)
      {
         AgentWriteDebugLog(5, _T("SMS: ReadToOK: readToMark returned %d"), rc);
         return false;
      }
      if (mark != NULL) 
      {
         if (data != NULL)
         {
            int len = (int)(mark - buffer);
            memcpy(data, buffer, len);
            data[len] = 0;
         }

         if (!strncmp(mark, "OK", 2))
            return true;
 
#ifdef UNICODE
      	AgentWriteDebugLog(5, _T("SMS: non-OK response (%hs)"), mark);
#else
      	AgentWriteDebugLog(5, _T("SMS: non-OK response (%s)"), mark);
#endif
         return false;
      }
   }
}

/**
 * Initialize modem
 */
static bool InitModem(Serial *serial)
{
	serial->write("\x1A\r\n", 3); // in case of pending send operation
   ReadToOK(serial);

	serial->write("ATZ\r\n", 5); // init modem
   if (!ReadToOK(serial))
      return false;
	AgentWriteDebugLog(5, _T("SMS: ATZ sent, got OK"));

   serial->write("ATE0\r\n", 6); // disable echo
   if (!ReadToOK(serial))
      return false;
	AgentWriteDebugLog(5, _T("SMS: ATE0 sent, got OK"));
   
   return true;
}

/**
 * Initialize sender
 * pszInitArgs format: portname,speed,databits,parity,stopbits
 */
bool InitSender(const TCHAR *pszInitArgs)
{
	TCHAR *portName;
	
	if (pszInitArgs == NULL || *pszInitArgs == 0)
	{
#ifdef _WIN32
		portName = _tcsdup(_T("COM1:"));
#else
		portName = _tcsdup(_T("/dev/ttyS0"));
#endif
	}
	else
	{
		portName = _tcsdup(pszInitArgs);
	}
	
	AgentWriteDebugLog(1, _T("SMS Sender: initializing GSM modem at %s"), pszInitArgs);
	
	TCHAR *p;
	const TCHAR *parityAsText;
	int portSpeed = 9600;
	int dataBits = 8;
	int parity = NOPARITY;
	int stopBits = ONESTOPBIT;
	
	if ((p = _tcschr(portName, _T(','))) != NULL)
	{
		*p = 0; p++;
		int tmp = _tcstol(p, NULL, 10);
		if (tmp != 0)
		{
			portSpeed = tmp;
			
			if ((p = _tcschr(p, _T(','))) != NULL)
			{
				*p = 0; p++;
				tmp = _tcstol(p, NULL, 10);
				if (tmp >= 5 && tmp <= 8)
				{
					dataBits = tmp;
					
					// parity
					if ((p = _tcschr(p, _T(','))) != NULL)
					{
						*p = 0; p++;
						switch (tolower((char)*p))
						{
							case 'n': // none
								parity = NOPARITY;
								break;
							case 'o': // odd
								parity = ODDPARITY;
								break;
							case 'e': // even
								parity = EVENPARITY;
								break;
						}
						
						// stop bits
						if ((p = _tcschr(p, _T(','))) != NULL)
						{
							*p = 0; p++;
							
							if (*p == _T('2'))
							{
								stopBits = TWOSTOPBITS;
							}

							// Text or PDU mode
							if ((p = _tcschr(p, _T(','))) != NULL)
							{
								*p = 0; p++;
								if (*p == _T('T'))
									s_operationMode = OM_TEXT;
								else if (*p == _T('P'))
									s_operationMode = OM_PDU;
							}
						}
					}
				}
			}
		}
	}
	
	switch (parity)
	{
		case ODDPARITY:
			parityAsText = _T("ODD");
			break;
		case EVENPARITY:
			parityAsText = _T("EVEN");
			break;
		default:
			parityAsText = _T("NONE");
			break;
	}
	AgentWriteDebugLog(1, _T("SMS: initialize for port=\"%s\", speed=%d, data=%d, parity=%s, stop=%d"),
	                portName, portSpeed, dataBits, parityAsText, stopBits == TWOSTOPBITS ? 2 : 1);
	
	if (s_serial.open(portName))
	{
		AgentWriteDebugLog(5, _T("SMS: port opened"));
		s_serial.setTimeout(2000);
		
      if (!s_serial.set(portSpeed, dataBits, parity, stopBits))
      {
   		AgentWriteDebugLog(5, _T("SMS: cannot set port parameters"));
         goto cleanup;
      }
		
      if (!InitModem(&s_serial))
         goto cleanup;

		// enter PIN: AT+CPIN="xxxx"
		// register network: AT+CREG1
		
		s_serial.write("ATI3\r\n", 6); // read vendor id
		char vendorId[1024];
      if (!ReadToOK(&s_serial, vendorId))
         goto cleanup;
		AgentWriteDebugLog(5, _T("SMS init: ATI3 sent, got OK"));
		
		char *sptr, *eptr;	
		for(sptr = vendorId; (*sptr != 0) && ((*sptr == '\r') || (*sptr == '\n') || (*sptr == ' ') || (*sptr == '\t')); sptr++);
		for(eptr = sptr; (*eptr != 0) && (*eptr != '\r') && (*eptr != '\n'); eptr++);
		*eptr = 0;
#ifdef UNICODE
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, sptr, -1, g_szDeviceModel, 256);
		g_szDeviceModel[255] = 0;
#else
		nx_strncpy(g_szDeviceModel, sptr, 256);
#endif
		AgentWriteLog(EVENTLOG_INFORMATION_TYPE, _T("SMS Sender: GSM modem initialized (Device=\"%s\" Model=\"%s\")"), pszInitArgs, g_szDeviceModel);
	}
	else
	{
		AgentWriteLog(EVENTLOG_WARNING_TYPE, _T("SMS Sender: Unable to open serial port (%s)"), pszInitArgs);
	}

cleanup:
	safe_free(portName);
   s_serial.close();
	return TRUE;   // return TRUE always to keep subagent in memory
}

/**
 * Send SMS
 */
bool SendSMS(const char *pszPhoneNumber, const char *pszText)
{
	if ((pszPhoneNumber == NULL) || (pszText == NULL))
      return false;

	AgentWriteDebugLog(3, _T("SMS: send to {%hs}: {%hs}"), pszPhoneNumber, pszText);
   if (!s_serial.restart())
   {
   	AgentWriteDebugLog(5, _T("SMS: failed to open port"));
      return false;
   }

   bool success = false;
   if (!InitModem(&s_serial))
      goto cleanup;
	
   if (s_operationMode == OM_PDU)
   {
	   s_serial.write("AT+CMGF=0\r\n", 11); // =0 - PDU message
      if (!ReadToOK(&s_serial))
         goto cleanup;
	   AgentWriteDebugLog(5, _T("SMS: AT+CMGF=0 sent, got OK"));

		char pduBuffer[PDU_BUFFER_SIZE];
		SMSCreatePDUString(pszPhoneNumber, pszText, pduBuffer);

      char buffer[256];
		snprintf(buffer, sizeof(buffer), "AT+CMGS=%d\r\n", (int)strlen(pduBuffer) / 2 - 1);
	   s_serial.write(buffer, (int)strlen(buffer));

      char *mark;
      if (s_serial.readToMark(buffer, sizeof(buffer), s_eosMarksSend, &mark) <= 0)
         goto cleanup;
      if ((mark == NULL) || (*mark != '>'))
      {
   	   AgentWriteDebugLog(5, _T("SMS: wrong response to AT+CMGS=\"%hs\" (%hs)"), pszPhoneNumber, mark);
         goto cleanup;
      }

      s_serial.write(pduBuffer, (int)strlen(pduBuffer)); // send PDU
      s_serial.write("\x1A\r\n", 3); // send ^Z
   }
   else
   {
	   s_serial.write("AT+CMGF=1\r\n", 11); // =1 - text message
      if (!ReadToOK(&s_serial))
         goto cleanup;
	   AgentWriteDebugLog(5, _T("SMS: AT+CMGF=1 sent, got OK"));

      char buffer[256];
      snprintf(buffer, sizeof(buffer), "AT+CMGS=\"%s\"\r\n", pszPhoneNumber);
	   s_serial.write(buffer, (int)strlen(buffer)); // set number

      char *mark;
      if (s_serial.readToMark(buffer, sizeof(buffer), s_eosMarksSend, &mark) <= 0)
         goto cleanup;
      if ((mark == NULL) || (*mark != '>'))
      {
   	   AgentWriteDebugLog(5, _T("SMS: wrong response to AT+CMGS=\"%hs\" (%hs)"), pszPhoneNumber, mark);
         goto cleanup;
      }
   	
      if (strlen(pszText) <= 160)
      {
         snprintf(buffer, sizeof(buffer), "%s\x1A\r\n", pszText);
      }
      else
      {
         strncpy(buffer, pszText, 160);
         strcpy(&buffer[160], "\x1A\r\n");
      }
	   s_serial.write(buffer, (int)strlen(buffer)); // send text, end with ^Z
   }

   s_serial.setTimeout(30000);
   if (!ReadToOK(&s_serial))
      goto cleanup;

   AgentWriteDebugLog(5, _T("SMS: AT+CMGS + message body sent, got OK"));
   success = true;

cleanup:
   s_serial.setTimeout(2000);
   s_serial.close();
	return success;
}
