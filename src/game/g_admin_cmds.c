/*/
===========================================================================
L0 / rtcwPub :: g_admin_cmds.c

Admin commands.

Created: 24. Mar / 2014
===========================================================================
*/
#include "g_admin.h"

/*
===========
Toggle incognito
===========
*/
void cmd_incognito(gentity_t *ent) {
	if (ent->client->sess.admin == ADM_NONE)
		return;

	if (ent->client->sess.incognito == 0) {
		ent->client->sess.incognito = 1;
		CP("cp \"You are now incognito^3!\n\"2");
		return;
	}
	else {
		ent->client->sess.incognito = 0;
		CP("cp \"Your status is now set to visible^3!\n\"2");
		return;
	}
}

/*
===========
Deals with customm commands
===========
*/
void cmdCustom(gentity_t *ent, char *cmd) {
	char *tag, *log;

	tag = sortTag(ent);

	if (!strcmp(ent->client->pers.cmd2, "")) {
		CP(va("print \"Command ^1%s ^7must have a value^1!\n\"", cmd));
		return;
	}
	else {
		// Rconpasswords or sensitve commands can be changed without public print..
		if (!strcmp(ent->client->pers.cmd3, "@"))
			CP(va("print \"Info: ^2%s ^7was silently changed to ^2%s^7!\n\"", cmd, ent->client->pers.cmd2));
		else
			AP(va("chat \"console: %s ^7changed ^3%s ^7to ^3%s %s\n\"", tag, cmd, ent->client->pers.cmd2, ent->client->pers.cmd3));
		// Change the stuff
		trap_SendConsoleCommand(EXEC_APPEND, va("%s %s %s", cmd, ent->client->pers.cmd2, ent->client->pers.cmd3));
		// Log it
		log = va("Player %s (IP: %s) has changed %s to %s %s.",
			ent->client->pers.netname, clientIP(ent, qtrue), cmd, ent->client->pers.cmd2, ent->client->pers.cmd3);
		logEntry(ADMACT, log);

		return;
	}
}

/*
===========
Ignore user
===========
*/
void cmd_ignore(gentity_t *ent) {
	int count = 0;
	int i;
	int nums[MAX_CLIENTS];
	char *tag, *log;

	tag = sortTag(ent);

	count = ClientNumberFromNameMatch(ent->client->pers.cmd2, nums);
	if (count == 0){
		CP("print \"Client not on server^1!\n\"");
		return;
	}
	else if (count > 1) {
		CP(va("print \"To many people with ^1%s ^7in their name^1!\n\"", ent->client->pers.cmd2));
		return;
	}

	for (i = 0; i < count; i++){
		if (g_entities[nums[i]].client->sess.ignored){
			CP(va("print \"Player %s ^7is already ignored^1!\n\"", g_entities[nums[i]].client->pers.netname));
			return;
		}
		else
			g_entities[nums[i]].client->sess.ignored = 1;

		AP(va("chat \"console: %s has ignored player %s^7!\n\"", tag, g_entities[nums[i]].client->pers.netname));

		// Log it
		log = va("Player %s (IP: %s) has Ignored user %s.",
			ent->client->pers.netname, clientIP(ent, qtrue), g_entities[nums[i]].client->pers.netname);
		logEntry(ADMACT, log);
	}
	return;
}

/*
===========
UnIgnore user
===========
*/
void cmd_unignore(gentity_t *ent) {
	int count = 0;
	int i;
	int nums[MAX_CLIENTS];
	char *tag, *log;

	tag = sortTag(ent);

	count = ClientNumberFromNameMatch(ent->client->pers.cmd2, nums);
	if (count == 0){
		CP("print \"Client not on server^1!\n\"");
		return;
	}
	else if (count > 1){
		CP(va("print \"To many people with ^1%s ^7in their name^1!\n\"", ent->client->pers.cmd2));
		return;
	}

	for (i = 0; i < count; i++){
		if (!g_entities[nums[i]].client->sess.ignored){
			CP(va("print \"Player %s ^7is already Unignored^1!\n\"", g_entities[nums[i]].client->pers.netname));
			return;
		}
		else
			g_entities[nums[i]].client->sess.ignored = 0;

		AP(va("chat \"console: %s has Unignored player %s^1!\n\"", tag, g_entities[nums[i]].client->pers.netname));

		// Log it
		log = va("Player %s (IP: %s) has unignored user %s.",
			ent->client->pers.netname, clientIP(ent, qtrue), g_entities[nums[i]].client->pers.netname);
		logEntry(ADMACT, log);
	}
	return;
}

/*
===========
Ignore user based upon client number
===========
*/
void cmd_clientIgnore(gentity_t *ent) {
	int	player_id;
	gentity_t	*targetclient;
	char *tag, *log;

	tag = sortTag(ent);

	player_id = ClientNumberFromString(ent, ent->client->pers.cmd2);
	if (player_id == -1) { // 
		return;
	}

	targetclient = g_entities + player_id;

	if (targetclient->client->sess.ignored) {
		CP(va("print \"Player %s ^7is already ignored^1!\"", targetclient->client->pers.netname));
		return;
	}

	targetclient->client->sess.ignored = 1;
	AP(va("chat \"console: %s has ignored player %s^7!\"", tag, targetclient->client->pers.netname));
	// Log it
	log = va("Player %s (IP: %s) has clientIgnored user %s.",
		ent->client->pers.netname, clientIP(ent, qtrue), targetclient->client->pers.netname);
	logEntry(ADMACT, log);

	return;
}

/*
===========
UnIgnore user based upon client number
===========
*/
void cmd_clientUnignore(gentity_t *ent) {
	int	player_id;
	gentity_t	*targetclient;
	char *tag, *log;

	tag = sortTag(ent);

	player_id = ClientNumberFromString(ent, ent->client->pers.cmd2);
	if (player_id == -1) { // 
		return;
	}

	targetclient = g_entities + player_id;

	if (targetclient->client->sess.ignored == 0) {
		CP(va("print \"Player %s ^7is already unignored^1!\"", targetclient->client->pers.netname));
		return;
	}

	targetclient->client->sess.ignored = 0;
	AP(va("chat \"console: %s has unignored player %s^1!\"", tag, targetclient->client->pers.netname));
	// Log it	
	log = va("Player %s (IP: %s) has clientUnignored user %s.",
		ent->client->pers.netname, clientIP(ent, qtrue), targetclient->client->pers.netname);
	logEntry(ADMACT, log);
	return;
}

/*
===========
Kick player + optional <msg>
===========
*/
void cmd_kick(gentity_t *ent) {
	int count = 0;
	int i;
	int nums[MAX_CLIENTS];
	char *tag, *log;

	tag = sortTag(ent);

	count = ClientNumberFromNameMatch(ent->client->pers.cmd2, nums);
	if (count == 0){
		CP("print \"Client not on server^1!\n\"");
		return;
	}
	else if (count > 1){
		CP(va("print \"To many people with ^1%s ^7in their name^1!\n\"", ent->client->pers.cmd2));
		return;
	}

	for (i = 0; i < count; i++){
		trap_DropClient(nums[i], va("^3kicked by ^3%s \n^7%s", tag, (ent->client->pers.cmd3 ? va("Reason: %s", ent->client->pers.cmd3) : "")));
		AP(va("chat \"console: %s has kicked player %s^7! %s\n\"", tag, g_entities[nums[i]].client->pers.netname, 
			(ent->client->pers.cmd3 ? va("^3Reason: ^7%s", ent->client->pers.cmd3) : "")));

		// Log it
		log = va("Player %s (IP: %s) has kicked user %s. %s",
			ent->client->pers.netname, clientIP(ent, qtrue), g_entities[nums[i]].client->pers.netname, 
			(ent->client->pers.cmd3 ? va("Reason: %s", ent->client->pers.cmd3) : ""));
		logEntry(ADMACT, log);
	}
	return;
}

/*
===========
Kick player based upon clientnumber + optional <msg>
===========
*/
void cmd_clientkick(gentity_t *ent) {
	int	player_id;
	gentity_t	*targetclient;
	char *tag, *log;

	tag = sortTag(ent);

	player_id = ClientNumberFromString(ent, ent->client->pers.cmd2);
	if (player_id == -1) {
		return;
	}

	targetclient = g_entities + player_id;

	//kick the client
	trap_DropClient(player_id, va("^3kicked by ^3%s \n^7%s", tag, (ent->client->pers.cmd3 ? va("Reason: %s", ent->client->pers.cmd3) : "")));
	AP(va("chat \"console: %s has kicked player %s^7! %s\n\"", tag, targetclient->client->pers.netname, 
		(ent->client->pers.cmd3 ? va("^3Reason: ^7%s", ent->client->pers.cmd3) : "")));

	// Log it
	log = va("Player %s (IP: %s) has clientKicked user %s. %s",
		ent->client->pers.netname, clientIP(ent, qtrue), targetclient->client->pers.netname, 
		(ent->client->pers.cmd3 ? va("Reason: %s", ent->client->pers.cmd3) : ""));
	logEntry(ADMACT, log);

	return;
}

/*
===========
Slap player
===========
*/
void cmd_slap(gentity_t *ent) {
	int clientid;
	int damagetodo;
	char *tag, *log, *log2;

	tag = sortTag(ent);
	// Sort log
	log = va("Player %s (IP: %s) has slapped ",
		ent->client->pers.netname, clientIP(ent, qtrue));

	clientid = atoi(ent->client->pers.cmd2);
	damagetodo = 20; // do 20 damage

	if ((clientid < 0) || (clientid >= MAX_CLIENTS))
	{
		CP("print \"Invalid client number^1!\n\"");
		return;
	}

	if ((!g_entities[clientid].client) || (level.clients[clientid].pers.connected != CON_CONNECTED))
	{
		CP("print \"Invalid client number^1!\n\"");
		return;
	}

	if (g_entities[clientid].client->sess.sessionTeam == TEAM_SPECTATOR) {
		CP("print \"You cannot slap a spectator^1!\n\"");
		return;
	}

	ent = &g_entities[clientid];

	if (ent->client->ps.stats[STAT_HEALTH] <= 20) {
		G_Damage(ent, NULL, NULL, NULL, NULL, damagetodo, DAMAGE_NO_PROTECTION, MOD_ADMKILL);
		AP(va("chat \"console: %s ^7was slapped to death by %s^3!\n\"", ent->client->pers.netname, tag));
		player_die(ent, ent, ent, 100000, MOD_ADMKILL);

		// Log it
		log2 = va("%s to death player %s.", log, ent->client->pers.netname);
		if (g_extendedLog.integer >= 2) // Only log this if it is set to 2+
			logEntry(ADMACT, log2);
		return;
	}
	else {
		G_Damage(ent, NULL, NULL, NULL, NULL, damagetodo, DAMAGE_NO_PROTECTION, MOD_ADMKILL);
		AP(va("chat \"console: %s ^7was slapped by %s^3!\n\"", ent->client->pers.netname, tag));
		// it's broadcasted globaly but only to near by players	
		G_AddEvent(ent, EV_GENERAL_SOUND, G_SoundIndex("sound/multiplayer/vo_revive.wav")); // L0 - TODO: Add sound in pack...

		// Log it
		log2 = va("%s player %s.", log, ent->client->pers.netname);
		if (g_extendedLog.integer >= 2) // Only log this if it is set to 2+
			logEntry(ADMACT, log2);
		return;
	}
}

/*
===========
Kill player
===========
*/
void cmd_kill(gentity_t *ent) {
	int clientid;
	int damagetodo;
	char *tag, *log, *log2;

	tag = sortTag(ent);
	// Sort log
	log = va("Player %s (IP: %s) has killed ",
		ent->client->pers.netname, clientIP(ent, qtrue));

	clientid = atoi(ent->client->pers.cmd2);
	damagetodo = 250; // Kill the player on spot


	if ((clientid < 0) || (clientid >= MAX_CLIENTS))
	{
		CP("print \"Invalid client number^1!\n\"");
		return;
	}

	if ((!g_entities[clientid].client) || (level.clients[clientid].pers.connected != CON_CONNECTED))
	{
		CP("print \"Invalid client number^1!\n\"");
		return;
	}

	if (!g_entities[clientid].client->ps.stats[STAT_HEALTH] > 0) {
		CP("print \"Player is already dead^1!\n\"");
		return;
	}

	if (g_entities[clientid].client->sess.sessionTeam == TEAM_SPECTATOR) {
		CP("print \"You cannot kill a spectator^1!\n\"");
		return;
	}

	ent = &g_entities[clientid];
	G_Damage(ent, NULL, NULL, NULL, NULL, damagetodo, DAMAGE_NO_PROTECTION, MOD_ADMKILL);
	AP(va("chat \"console: %s ^7was killed by %s^3!\n\"", ent->client->pers.netname, tag));
	player_die(ent, ent, ent, 100000, MOD_ADMKILL);

	// Log it
	log2 = va("%s user %s.", log, ent->client->pers.netname);
	if (g_extendedLog.integer >= 2) // Only log this if it is set to 2+
		logEntry(ADMACT, log2);
	return;
}
