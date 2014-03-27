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

/*
===========
Censoring penalty
===========
*/
void SB_chatWarn(gentity_t *ent) {
	int n = rand() % 4; // Randomize messages

	if (!sb_system.integer || (sb_system.integer && !sb_censorPenalty.integer))
		return;

	// Only for non logged in users..
	// Chat will still get censored they just wont get kicked or ignored for it..
	if (ent->client->sess.admin != ADM_NONE)
		return;

	if (ent->client->pers.sb_chatWarned == 0) {
		if (n == 0)
			CP("chat \"^3Strike one! ^7You should really wash your mouth.\n\"");
		else if (n == 1)
			CP("chat \"^3Strike one! ^7You got warned for foul language..\n\"");
		else if (n == 2)
			CP("chat \"^3Strike one! ^7This is not your local pub..\n\"");
		else
			CP("chat \"^3Strike one! ^7Cursing is not allowed here.\n\"");
	}
	else if (ent->client->pers.sb_chatWarned == 1) {
		if (n == 0)
			CP("chat \"^3Strike two! ^7Don't push it..\n\"");
		else if (n == 1)
			CP("chat \"^3Strike two! ^7You where warned..\n\"");
		else if (n == 2)
			CP("chat \"^3Strike two! ^7Do you talk to your parents like this?\n\"");
		else
			CP("chat \"^3Strike two! ^7Foul language is not allowed here.\n\"");
	}
	else if (ent->client->pers.sb_chatWarned == 2) {
		if (n == 0)
			CP("chat \"^3Strike three! ^7Last warning!\n\"");
		else if (n == 1)
			CP("chat \"^3Strike three! ^7There wont be strike four..\n\"");
		else if (n == 2)
			CP("chat \"^3Strike three! ^7There's no more warnings after this one.\n\"");
		else
			CP("chat \"^3Strike three! ^7Care to see how strike four looks like?\n\"");
	}
	else {
		if (sb_censorPenalty.integer == 1) {
			ent->client->sess.ignored = 1;
			AP(va("chat \"^3SB^7: %s ^7has been ignored due foul language^3!\n\"", ent->client->pers.netname));
		}
		else {

			// Tempban
			if (sb_censorPentalityTempbanMin.integer)
			{	
				trap_SendConsoleCommand(EXEC_APPEND, va("tempban %i %i", ent->client->ps.clientNum, sb_censorPentalityTempbanMin.integer));
			}

			AP(va("chat \"^3SB^7: %s ^7got kicked for foul language^3!\n\"", ent->client->pers.netname));
			trap_DropClient(ent - g_entities, va("^3kicked for ^3Foul ^3Language!"));
		}
		return;
	}

	// Count it..
	ent->client->pers.sb_chatWarned++;
	// Use sound if they have sound pack..
	CPS(ent, "rtcwpub/sound/client/warn.wav");

	return;
}


