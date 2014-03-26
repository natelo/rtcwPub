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

		// Auto shuffle if enabled and treshhold is reached
		if (g_autoShuffle.integer)
		{
			if (shuffleTracking.integer >= g_autoShuffle.integer)
			{
				trap_SendConsoleCommand(EXEC_APPEND, "shuffle @print\n");
				trap_Cvar_Set("shuffleTracking", 0);
				AP("chat \"console: Teams were ^3Auto shuffled^7!\n\"");
			}
			// Notify that shuffle will occur the next round..
			else if ((g_autoShuffle.integer > 2) &&
				(shuffleTracking.integer == (g_autoShuffle.integer - 1)))
			{
				AP(("chat \"^3Notice: ^7Teams will be ^3Auto Shuffled ^7the next round^3!\n\""));
			}
		}
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

/*
=================
Match Info

Basically just some info prints..
=================
*/
// Gracefully taken from s4ndmod :p
char *GetLevelTime(void) {
	int Objseconds, Objmins, Objtens;

	Objseconds = (((g_timelimit.value * 60 * 1000) - ((level.time - level.startTime))) / 1000); // begin martin - this line was a bitch :-)
																								// L0 - I know, that's why I took it. :p
	Objmins = Objseconds / 60;
	Objseconds -= Objmins * 60;
	Objtens = Objseconds / 10;
	Objseconds -= Objtens * 10;

	if (Objseconds < 0) {
		Objseconds = 0;
	}
	if (Objtens < 0) {
		Objtens = 0;
	}
	if (Objmins < 0) {
		Objmins = 0;
	}

	return va("%i:%i%i", Objmins, Objtens, Objseconds);  //end martin
}
// Prints stuff
void matchInfo(unsigned int type, char *msg) {

	// End of Match info
	if (type == MT_EI)
	{
		if (g_gametype.integer == GT_WOLF_STOPWATCH) {
			if (g_currentRound.integer == 1) {
				AP(va("print \"*** ^3Clock set to: ^7%d:%02d\n\"",
					g_nextTimeLimit.integer,
					(int)(60.0 * (float)(g_nextTimeLimit.value - g_nextTimeLimit.integer))));
			}
			else {
				float val = (float)((level.timeCurrent - (level.startTime + level.time - level.intermissiontime)) / 60000.0);
				if (val < g_timelimit.value)
				{
					AP(va("print \"*** ^3Objective reached at ^7%d:%02d ^3(original: ^7%d:%02d^3)\n\"",
						(int)val,
						(int)(60.0 * (val - (int)val)),
						g_timelimit.integer,
						(int)(60.0 * (float)(g_timelimit.value - g_timelimit.integer))));
				}
				else {
					AP(va("print \"*** ^3Objective NOT reached in time (^7%d:%02d^3)\n\"",
						g_timelimit.integer,
						(int)(60.0 * (float)(g_timelimit.value - g_timelimit.integer))));
				}
			}
		}
	}
	// Match events
	else if (type == MT_ME)
	{
		if (g_printMatchInfo.integer)
			AP(va("print \"[%s] ^3%s \n\"", GetLevelTime(), msg));
		else
			AP(va("cp \"%s \n\"", msg));
	}
}

