/* 
** NetXMS subagent for OpenBSD
** Copyright (C) 2004-2009 Alex Kirhenshtein
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
**/

#include <nms_common.h>
#include <nms_agent.h>

#include <sys/param.h>
#include <sys/mount.h>

#include "disk.h"

LONG H_DiskInfo(const TCHAR *pszParam, const TCHAR *pArg, TCHAR *pValue, AbstractCommSession *session)
{
	int nRet = SYSINFO_RC_ERROR;
	char szArg[512] = {0};
	struct statfs s;

	AgentGetParameterArgA(pszParam, 1, szArg, sizeof(szArg));

	if (szArg[0] != 0 && statfs(szArg, &s) == 0)
	{
		nRet = SYSINFO_RC_SUCCESS;
		
		QWORD usedBlocks = (QWORD)(s.f_blocks - s.f_bfree);
		QWORD totalBlocks = (QWORD)s.f_blocks;
		QWORD blockSize = (QWORD)s.f_bsize;
		QWORD freeBlocks = (QWORD)s.f_bfree;
		QWORD availableBlocks = (QWORD)s.f_bavail;
		
		switch((long)pArg)
		{
			case DISK_TOTAL:
				ret_uint64(pValue, totalBlocks * blockSize);
				break;
			case DISK_USED:
				ret_uint64(pValue, usedBlocks * blockSize);
				break;
			case DISK_FREE:
				ret_uint64(pValue, freeBlocks * blockSize);
				break;
			case DISK_AVAIL:
				ret_uint64(pValue, availableBlocks * blockSize);
				break;
			case DISK_USED_PERC:
				ret_double(pValue, (usedBlocks * 100.0) / totalBlocks);
				break;
			case DISK_AVAIL_PERC:
				ret_double(pValue, (availableBlocks * 100.0) / totalBlocks);
				break;
			case DISK_FREE_PERC:
				ret_double(pValue, (freeBlocks * 100.0) / totalBlocks);
				break;
			default:
				nRet = SYSINFO_RC_ERROR;
				break;
		}
	}

	return nRet;
}
