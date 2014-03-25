/*/
===========================================================================
L0 / rtcwPub :: g_shared.c

	Game logic shared functionality

Created: 24. Mar / 2014
===========================================================================
*/
#include "g_local.h"

/*
==================
Ported from et: NQ
DecolorString

Remove color characters
==================
*/
void DecolorString(char *in, char *out)
{
	while (*in) {
		if (*in == 27 || *in == '^') {
			in++;		// skip color code
			if (*in) in++;
			continue;
		}
		*out++ = *in++;
	}
	*out = 0;
}

/*
==================
Time

Returns current time.
==================
*/
extern int trap_RealTime(qtime_t * qtime);
const char *months[12] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
// Returns current time
char *getTime( void )
{
	qtime_t		ct;
	trap_RealTime(&ct);

	return va("%02d:%02d:%02d/%02d %s %d",
		ct.tm_hour, ct.tm_min, ct.tm_sec, ct.tm_mday,
		months[ct.tm_mon], 1900 + ct.tm_year);
}

/*
==================
Check if string is numeric
==================
*/
int is_numeric(const char *p) {
	if (*p) {
		char c;
		while ((c = *p++)) {
			if (!isdigit(c)) return 0;
		}
		return 1;
	}
	return 0;
}


