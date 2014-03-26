/*
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
/*
	else if (mod == MOD_SELFKILL)
	{
		int r = rand() % 2; // randomize messages

		if (r == 0)
			AP(va("print \"%s ^7slit his throat.\n\"", self->client->pers.netname));
		else if (r == 1)
			AP(va("print \"%s ^7commited suicide.\n\"", self->client->pers.netname));

		return qtrue;
	}
	else if (mod == MOD_THROWKNIFE)
	{
		AP(va("print \"%s ^7was impaled by %s^7s throwing knife.\n\"", self->client->pers.netname, attacker->client->pers.netname));
		attacker->client->pers.knifeKills++;
		write_RoundStats(attacker->client->pers.netname, attacker->client->pers.knifeKills, ROUND_KNIFETHROW);

		return qtrue;
	}
	else if (mod == MOD_CHICKEN)
	{
		AP(va("print \"%s ^6was scared to death by ^7%s^7.\n\"", self->client->pers.netname, attacker->client->pers.netname));
		self->client->pers.chicken++;
		write_RoundStats(self->client->pers.netname, self->client->pers.chicken, ROUND_CHICKEN);

		// Give props to attacker
		attacker->client->pers.kills++;
		attacker->client->pers.lifeKills++;
		write_RoundStats(attacker->client->pers.netname, attacker->client->pers.kills, ROUND_KILLS);
		write_RoundStats(attacker->client->pers.netname, attacker->client->pers.lifeKills, ROUND_KILLPEAK);

		return qtrue;
	}
	else if (mod == MOD_POISONMED)
	{
		int r = rand() % 2;

		if (r == 0)
			AP(va("print \"%s ^7was poisoned by %s^7.\n\"", self->client->pers.netname, attacker->client->pers.netname));
		else if (r == 1)
			AP(va("print \"%s ^7tasted %s^7's poison.\n\"", self->client->pers.netname, attacker->client->pers.netname));

		// Stats
		attacker->client->pers.poison++;

		write_RoundStats(attacker->client->pers.netname, attacker->client->pers.poison, ROUND_POISON);
	}
*/
	return qfalse;
}