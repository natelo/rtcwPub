/*/
===========================================================================
L0 / rtcwPub :: g_files.c

	Deals with Files handling

Created: 24. Mar / 2014
===========================================================================
*/
#include "g_admin.h"

/*
===========
Log Admin related stuff
===========
*/
void logEntry(char *filename, char *info)
{
	fileHandle_t	f;
	char *varLine;

	if (!g_extendedLog.integer)
		return;

	strcat(info, "\r");
	trap_FS_FOpenFile(filename, &f, FS_APPEND);

	varLine = va("Time: %s : %s \n", getTime(), info);

	trap_FS_Write(varLine, strlen(varLine), f);
	trap_FS_FCloseFile(f);
	return;
}

