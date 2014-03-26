/*/
===========================================================================
L0 / rtcwPub :: g_admin.c

	Server (Admin) Bot (SB) functionality.

Created: 26. Mar / 2014
===========================================================================
*/
#include "g_local.h"

/*
===========
Kicks for teamkills
===========
*/
void SB_maxTeamKill(gentity_t *ent) {
	int count = ent->client->pers.teamKills + 1 - ent->client->pers.sb_TKforgiven;

	if (level.warmupTime || !sb_system.integer || sb_maxTKs.integer == (-1))
	{
		// Just count it for stats
		if (!level.warmupTime)
			ent->client->pers.teamKills++;
		return;
	}

	if (sb_maxTKs.integer - count == 1 && !ent->client->pers.sb_TKwarned)
	{
		AP(va("chat \"^3[WARNING]: ^7%s ^7gets kicked on next ^3Team Kill^7!\n\"", ent->client->pers.netname));
		ent->client->pers.sb_TKwarned = qtrue;
	}

	// Give them some time to make it up.. (ie tk-revive)
	ent->client->pers.sb_TKkillTime = level.time + 10000;
	ent->client->pers.teamKills++;
	return;
}

/*
===========
Kicks for bleeding
===========
*/
void SB_maxTeamBleed(gentity_t *ent) {
	int count = ent->client->pers.dmgTeam;

	if (level.warmupTime || !sb_system.integer || sb_maxTeamBleed.integer == (-1))
		return;

	if (sb_maxTeamBleed.integer - count == 10)
		AP(va("chat \"^3[WARNING]: ^7%s ^7is getting close to being kicked for ^3Bleeding^7!\n\"", ent->client->pers.netname));

	if ((count >= sb_maxTeamBleed.integer) && count) {

		// Tempban
		if (sb_maxTeamBleedTempbanMins.integer)
		{	
			trap_SendConsoleCommand(EXEC_APPEND, va("tempban %i %i", ent->client->ps.clientNum, sb_maxTeamBleedTempbanMins.integer));
		}

		trap_DropClient(ent - g_entities, "Kicked \n^3For Team Wounding.");
		AP(va("chat \"^3SB^7: %s ^7got kicked for ^3Team Wounding^7.\n\"", ent->client->pers.netname));
	}
	return;
}

/*
===========
Kicks for low score
===========
*/
void SB_minLowScore(gentity_t *ent) {
	// Positive values are ignored.
	if (level.warmupTime || !sb_system.integer || sb_minLowScore.integer >= 0)
		return;

	if (ent->client->ps.persistant[PERS_SCORE] < sb_minLowScore.integer){

		// Tempban
		if (sb_minLowScoreTempbanMins.integer)
		{
			trap_SendConsoleCommand(EXEC_APPEND, va("tempban %i %i", ent->client->ps.clientNum, sb_minLowScoreTempbanMins.integer));
		}

		AP(va("chat \"^3SB^7: %s ^7got kicked for ^3Low Score^7.\n\"", ent->client->pers.netname));
		trap_DropClient(ent->client->ps.clientNum, "Kicked \n^3For Low Score.");
	}
	return;
}

/*
===========
Kicks laggers
===========
*/
void SB_maxPingFlux(gclient_t *client) {
	int max = sb_maxPingFlux.integer;
	int times = sb_maxPingHits.integer;

	// 50 is sanity check..so someone doesn't screw it with sab_maxPingFlux "1"
	if (level.warmupTime || !sb_system.integer || max < 50 ||
		client->sess.sessionTeam != TEAM_SPECTATOR || client->ps.pm_type != PM_DEAD)
		return;

	if (client->pers.sb_ping >= times) {
		trap_DropClient(client - level.clients, "Kicked \nDue unstable ping or max ping limit.");
		AP(va("chat \"^3SB^7: %s ^7got kicked due ^3Unstable Ping^7.\n\"", client->pers.netname));
		return;

		// 1 hit per second
	}
	else if (client->ps.ping >= max && level.time > client->pers.sb_maxPing) {
		client->pers.sb_ping++;
		client->pers.sb_maxPing = level.time + 1000;
		return;
	}
	return;
}
