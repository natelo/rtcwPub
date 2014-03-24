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
L0 - Ported from et: NQ
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

