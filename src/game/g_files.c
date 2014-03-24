/*/
===========================================================================
L0 / rtcwPub :: g_files.c

	Deals with Files handling

Created: 24. Mar / 2014
===========================================================================
*/
#include "g_admin.h"

// Time
extern int trap_RealTime(qtime_t * qtime);
const char *months[12] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

/*
===========
Log Admin related stuff
===========
*/
void logEntry(char *filename, char *info)
{
	fileHandle_t	f;
	char *varLine;
	qtime_t		ct;
	trap_RealTime(&ct);

	if (!g_extendedLog.integer)
		return;

	strcat(info, "\r");
	trap_FS_FOpenFile(filename, &f, FS_APPEND);

	varLine = va("Time: %02d:%02d:%02d/%02d %s %d : %s \n",
		ct.tm_hour, ct.tm_min, ct.tm_sec, ct.tm_mday,
		months[ct.tm_mon], 1900 + ct.tm_year, info);

	trap_FS_Write(varLine, strlen(varLine), f);
	trap_FS_FCloseFile(f);
	return;
}

