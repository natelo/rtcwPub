/*/
===========================================================================
L0 / rtcwPub :: g_hacks.c

	Functions that should be client-server sided but due server only mod
	they're more or less hacked into a game..

	NOTE: If you're ever planning to create a client-server mod,
		  then patch stuff accordingly and remove this file..

Created: 24. Mar / 2014
===========================================================================
*/
#include "g_local.h"

/*
===========
Custom MODs
===========
*/
qboolean isCustomMOD(int mod, gentity_t *self, gentity_t *attacker)
{
	// Killed by !kill command
	if (mod == MOD_ADMKILL)
	{	
		return qtrue;
	}
	return qfalse;
}