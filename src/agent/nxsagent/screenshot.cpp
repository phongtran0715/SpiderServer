/*
** NetXMS Session Agent
** Copyright (C) 2003-2014 Victor Kirhenshtein
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
** File: screenshot.cpp
**
**/

#include "nxsagent.h"

/**
 * Take screenshot
 */
void TakeScreenshot(NXCPMessage *response)
{
   UINT32 rcc = ERR_INTERNAL_ERROR;

   HDC dc = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
   if (dc != NULL)
   {
      HDC memdc = CreateCompatibleDC(dc);
      if (memdc != NULL)
      {
         int x = GetSystemMetrics(SM_XVIRTUALSCREEN);
         int y = GetSystemMetrics(SM_YVIRTUALSCREEN);
         int cx = GetSystemMetrics(SM_CXVIRTUALSCREEN);
         int cy = GetSystemMetrics(SM_CYVIRTUALSCREEN);

         HBITMAP bitmap = CreateCompatibleBitmap(dc, cx, cy);
         if (bitmap != NULL)
         {
            SelectObject(memdc, bitmap);
            BitBlt(memdc, 0, 0, cx, cy, dc, x, y, SRCCOPY | CAPTUREBLT);
         }

         DeleteDC(memdc);

         TCHAR tempPath[MAX_PATH];
         GetTempPath(MAX_PATH, tempPath);

         TCHAR tempFile[MAX_PATH];
         GetTempFileName(tempPath, _T("nx"), 0, tempFile);
         if (SaveBitmapToPng(bitmap, tempFile))
         {
            rcc = ERR_SUCCESS;
            response->setFieldFromFile(VID_FILE_DATA, tempFile);
         }
         DeleteObject(bitmap);
         DeleteFile(tempFile);
      }
      DeleteDC(dc);
   }
   
   response->setField(VID_RCC, rcc);
}
