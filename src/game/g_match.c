/*/
===========================================================================
L0 / rtcwPub :: g_match.c

	Holds match specific stuff	

Created: 25. Mar / 2014
===========================================================================
*/
#include "g_local.h"

/*
=================
Countdown
=================
*/
void CountDown(void) {
	char *index = "";

	if (level.cnStarted == qfalse) {
		return;
	}
	// Countdown...
	if (level.cnNum == 0)
	{
		index = "prepare.wav"; AP("cp \"Prepare to fight^2!\n\"2");
	}
	if (level.cnNum == 1)
	{
		index = "cn_5.wav"; /*AP( "cp \"Match starts in: ^25\n\"2" );*/
	}
	if (level.cnNum == 2)
	{
		index = "cn_4.wav"; /*AP( "cp \"Match starts in: ^24\n\"2" );*/
	}
	if (level.cnNum == 3)
	{
		index = "cn_3.wav"; /*AP( "cp \"Match starts in: ^23\n\"2" );*/
	}
	if (level.cnNum == 4)
	{
		index = "cn_2.wav"; /*AP( "cp \"Match starts in: ^22\n\"2" );*/
	}
	if (level.cnNum == 5)
	{
		index = "cn_1.wav"; /*AP( "cp \"Match starts in: ^21\n\"2" );*/
	}
	if (level.cnNum == 6)
	{
		index = "fight.wav"; AP("print \"^2Fight!\n\"");
	}

	// Prepare to fight takes 2 seconds..
	if (level.cnNum == 0){
		level.cnPush = level.time + 2000;

	} // Just enough to fix the bug and skip to action..
	else if (level.cnNum == 6) {
		level.cnPush = level.time + 200;
		// Otherwise, 1 second.
	}
	else {
		level.cnPush = level.time + 1000;
	}

	// We're done.. restart the game
	if (level.cnNum == 7) {

		level.warmupTime += 10000;
		trap_Cvar_Set("g_restarted", "1");
		trap_SendConsoleCommand(EXEC_APPEND, "map_restart 0\n");
		level.restarted = qtrue;

		return;
	}

	if (level.clients->pers.connected == CON_CONNECTED)
		APS(va("rtcwpub/sound/scenaric/countdown/%s", index));

	level.cnNum++;
}

/*
=================
Chicken Run

Originally from etPUB (i think)

TODO: Once stats are ported make sure it counts as kill for attacker.
=================
*/
gentity_t *G_FearCheck(gentity_t *ent) {
	qboolean fear = qfalse;
	gentity_t *attacker = &level.gentities[ent->client->lasthurt_client];

	if (g_chicken.integer && attacker && attacker->client &&
		(level.time - ent->client->lasthurt_time) < g_chicken.integer &&
		attacker->client->sess.sessionTeam != ent->client->sess.sessionTeam &&
		attacker->health > 0) {

		fear = qtrue;
	}

	return (fear ? attacker : NULL);
}

