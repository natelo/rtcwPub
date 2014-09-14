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
void cmd_custom(gentity_t *ent, char *cmd) {
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
		admLog(log);

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
		CP(va("print \"Too many people with ^1%s ^7in their name^1!\n\"", ent->client->pers.cmd2));
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
		log = va("Player %s (IP: %s) has ignored user %s.",
			ent->client->pers.netname, clientIP(ent, qtrue), g_entities[nums[i]].client->pers.netname);
		admLog(log);
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
		CP(va("print \"Too many people with ^1%s ^7in their name^1!\n\"", ent->client->pers.cmd2));
		return;
	}

	for (i = 0; i < count; i++){
		if (!g_entities[nums[i]].client->sess.ignored){
			CP(va("print \"Player %s ^7is already unignored^1!\n\"", g_entities[nums[i]].client->pers.netname));
			return;
		}
		else
			g_entities[nums[i]].client->sess.ignored = 0;

		AP(va("chat \"console: %s has unignored player %s^1!\n\"", tag, g_entities[nums[i]].client->pers.netname));

		// Log it
		log = va("Player %s (IP: %s) has unignored user %s.",
			ent->client->pers.netname, clientIP(ent, qtrue), g_entities[nums[i]].client->pers.netname);
		admLog(log);
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
	admLog(log);

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
	admLog(log);
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
		CP(va("print \"Too many people with ^1%s ^7in their name^1!\n\"", ent->client->pers.cmd2));
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
		admLog(log);
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
	admLog(log);

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

	if ((!g_entities[clientid].client) || (level.clients[clientid].pers.connected != CON_CONNECTED)) {
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
			admLog(log2);
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
			admLog(log2);
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


	if ((clientid < 0) || (clientid >= MAX_CLIENTS)) {
		CP("print \"Invalid client number^1!\n\"");
		return;
	}

	if ((!g_entities[clientid].client) || (level.clients[clientid].pers.connected != CON_CONNECTED)) {
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
		admLog(log2);
	return;
}

/*
===========
Force user to spectators
===========
*/
void cmd_specs(gentity_t *ent) {
	int count = 0;
	int i;
	int nums[MAX_CLIENTS];
	char *tag, *log;

	tag = sortTag(ent);

	count = ClientNumberFromNameMatch(ent->client->pers.cmd2, nums);
	if (count == 0) {
		CP("print \"Client not on server^1!\n\"");
		return;
	}
	else if (count > 1) {
		CP(va("print \"Too many people with ^1%s ^7in their name^1!\n\"", ent->client->pers.cmd2));
		return;
	}

	for (i = 0; i < count; i++) {
		if (g_entities[nums[i]].client->sess.sessionTeam == TEAM_SPECTATOR) {
			CP(va("print \"Player %s ^7is already a spectator^1!\n\"", g_entities[nums[i]].client->pers.netname));
			return;
		}
		else
			SetTeam(&g_entities[nums[i]], "spectator", qtrue);
		AP(va("chat \"console: %s has forced player %s ^7to ^3spectators^7!\n\"", tag, g_entities[nums[i]].client->pers.netname));

		// Log it
		log = va("Player %s (IP: %s) has forced user %s to spectators.",
			ent->client->pers.netname, clientIP(ent, qtrue), g_entities[nums[i]].client->pers.netname);
		if (g_extendedLog.integer >= 2) // Only log this if it is set to 2+
			admLog(log);
	}
	return;
}

/*
===========
Force user to Axis
===========
*/
void cmd_axis(gentity_t *ent) {
	int count = 0;
	int i;
	int nums[MAX_CLIENTS];
	char *tag, *log;

	tag = sortTag(ent);

	count = ClientNumberFromNameMatch(ent->client->pers.cmd2, nums);
	if (count == 0) {
		CP("print \"Client not on server^1!\n\"");
		return;
	}
	else if (count > 1) {
		CP(va("print \"Too many people with ^1%s ^7in their name^1!\n\"", ent->client->pers.cmd2));
		return;
	}

	for (i = 0; i < count; i++) {
		if (g_entities[nums[i]].client->sess.sessionTeam == TEAM_RED) {
			CP(va("print \"Player %s ^7is already an ^1Axis ^7recruit!\n\"", g_entities[nums[i]].client->pers.netname));
			return;
		}
		else
			SetTeam(&g_entities[nums[i]], "red", qtrue);
		AP(va("chat \"console: %s has forced player %s ^7to ^1Axis ^7team!\n\"", tag, g_entities[nums[i]].client->pers.netname));

		// Log it
		log = va("Player %s (IP: %s) has forced user %s into Axis team.",
			ent->client->pers.netname, clientIP(ent, qtrue), g_entities[nums[i]].client->pers.netname);
		if (g_extendedLog.integer >= 2) // Only log this if it is set to 2+
			admLog(log);
	}
	return;
}

/*
===========
Force user to Allied
===========
*/
void cmd_allied(gentity_t *ent) {
	int count = 0;
	int i;
	int nums[MAX_CLIENTS];
	char *tag, *log;

	tag = sortTag(ent);

	count = ClientNumberFromNameMatch(ent->client->pers.cmd2, nums);
	if (count == 0) {
		CP("print \"Client not on server^1!\n\"");
		return;
	}
	else if (count > 1) {
		CP(va("print \"Too many people with ^1%s ^7in their name^1!\n\"", ent->client->pers.cmd2));
		return;
	}

	for (i = 0; i < count; i++) {
		if (g_entities[nums[i]].client->sess.sessionTeam == TEAM_BLUE) {
			CP(va("print \"Player %s ^7is already an ^4Allied ^7recruit!\n\"", g_entities[nums[i]].client->pers.netname));
			return;
		}
		else
			SetTeam(&g_entities[nums[i]], "blue", qtrue);
		AP(va("chat \"console: %s has forced player %s ^7to ^4Allied ^7team!\n\"", tag, g_entities[nums[i]].client->pers.netname));

		// Log it
		log = va("Player %s (IP: %s) has forced user %s into Axis team.",
			ent->client->pers.netname, clientIP(ent, qtrue), g_entities[nums[i]].client->pers.netname);
		if (g_extendedLog.integer >= 2) // Only log this if it is set to 2+
			admLog(log);
	}
	return;
}

/*
===========
Execute command
===========
*/
void cmd_exec(gentity_t *ent) {
	char *tag, *log;

	tag = sortTag(ent);

	if (!strcmp(ent->client->pers.cmd3, "@"))
		CP(va("print \"^3Info: ^7%s has been executed^3!\n\"", ent->client->pers.cmd2));
	else
		AP(va("print \"console: %s has executed ^3%s^3!\n\"", tag, ent->client->pers.cmd2));

	trap_SendConsoleCommand(EXEC_INSERT, va("exec \"%s\"", ent->client->pers.cmd2));

	// Log it
	log = va("Player %s (IP: %s) has executed %s config.",
		ent->client->pers.netname, clientIP(ent, qtrue), ent->client->pers.cmd2);
	admLog(log);

	return;
}

/*
===========
Nextmap
===========
*/
void cmd_nextmap(gentity_t *ent) {
	char *tag, *log;

	tag = sortTag(ent);
	AP(va("chat \"console: %s has set the ^3nextmap ^7in rotation^3!\n\"", tag));
	trap_SendConsoleCommand(EXEC_APPEND, va("vstr nextmap"));

	// Log it
	log = va("Player %s (IP: %s) has set nextmap.",
		ent->client->pers.netname, clientIP(ent, qtrue));
	if (g_extendedLog.integer >= 2) // Only log this if it is set to 2+
		admLog(log);

	return;
}

/*
===========
Load map
===========
*/
void cmd_map(gentity_t *ent) {
	char *tag, *log;

	tag = sortTag(ent);
	AP(va("chat \"console: %s has loaded the map ^3%s^3! \n\"", tag, ent->client->pers.cmd2));
	trap_SendConsoleCommand(EXEC_APPEND, va("map %s", ent->client->pers.cmd2));

	// Log it
	log = va("Player %s (IP: %s) has loaded %s map.",
		ent->client->pers.netname, clientIP(ent, qtrue), g_entities->client->pers.cmd2);
	admLog(log);

	return;
}

/*
===========
Vstr

Loads next map in rotation (if any)
===========
*/
void cmd_vstr(gentity_t *ent) {
	char *tag, *log;

	if (!Q_stricmp(ent->client->pers.cmd2, ""))
	{
		CP("print \"^1Error: ^7Please set a Marker (second command) so it can jump to that part of rotation!\n\"");
		return;
	}

	tag = sortTag(ent);
	AP(va("chat \"console: %s set ^3vstr ^7to ^3%s^7.\n\"", tag, ent->client->pers.cmd2));
	trap_SendConsoleCommand(EXEC_APPEND, va("vstr %s", ent->client->pers.cmd2));

	// Log it
	log = va("Player %s (IP: %s) has set vstr to %s",
		ent->client->pers.netname, clientIP(ent, qtrue), g_entities->client->pers.cmd2);
	admLog(log);

	return;
}

/*
===========
Center prints message to all
===========
*/
void cmd_cpa(gentity_t *ent) {
	char *s, *log;

	s = ConcatArgs(2);
	AP(va("cp \"^1ADMIN WARNING^7! \n%s\n\"", s));

	// Log it
	log = va("Player %s (IP: %s) issued CPA warning: %s",
		ent->client->pers.netname, clientIP(ent, qtrue), s);

	// Only log this if it is set to 2+
	if (g_extendedLog.integer >= 2) 
		admLog(log);

	return;
}

/*
===========
Shows message to selected user in center print
===========
*/
void cmd_cp(gentity_t *ent) {
	int	player_id;
	gentity_t	*targetclient;
	char *s, *log;

	s = ConcatArgs(3);

	player_id = ClientNumberFromString(ent, ent->client->pers.cmd2);
	if (player_id == -1) {
		return;
	}

	targetclient = g_entities + player_id;

	// CP to user	
	CPx(targetclient - g_entities, va("cp \"^1ADMIM WARNING^7! \n%s\n\"2", s));

	// Log it
	log = va("Player %s (IP: %s) issued to user %s a CP warning: %s",
		ent->client->pers.netname, clientIP(ent, qtrue), targetclient->client->pers.netname, s);

	// Only log this if it is set to 2+
	if (g_extendedLog.integer >= 2) 
		admLog(log);

	return;
}

/*
===========
Shows message to all in console and center print
===========
*/
void cmd_warn(gentity_t *ent) {
	char *s, *log;

	s = ConcatArgs(2);
	AP(va("cp \"^1ADMIM WARNING^7: \n%s\n\"2", s));
	AP(va("chat \"^1ADMIM WARNING^7: \n%s\n\"", s));

	// Log it
	log = va("Player %s (IP: %s) issued global warning: %s",
		ent->client->pers.netname, clientIP(ent, qtrue), s);

	// Only log this if it is set to 2+
	if (g_extendedLog.integer >= 2) 
		admLog(log);

	return;
}

/*
===========
Shows message to all in console
===========
*/
void cmd_chat(gentity_t *ent) {
	char *s, *log;

	s = ConcatArgs(2);
	AP(va("chat \"^1ADMIM WARNING^7: \n%s\n\"", s));

	// Log it
	log = va("Player %s (IP: %s) issued CHAT warning: %s",
		ent->client->pers.netname, clientIP(ent, qtrue), s);

	// Only log this if it is set to 2+
	if (g_extendedLog.integer >= 2) 
		admLog(log);

	return;
}
/*
===========
Cancels any vote in progress
===========
*/
void cmd_cancelvote(gentity_t *ent) {
	char *tag, *log;

	tag = sortTag(ent);

	if (level.voteTime) {
		level.voteNo = level.numConnectedClients;
		CheckVote();
		AP(va("cp \"%s has ^3Cancelled the vote.\n\"2", tag));
		AP("chat \"console: Turns out everyone voted No^1!\n\"");

		// Log it
		log = va("Player %s (IP: %s) cancelled a vote.",
			ent->client->pers.netname, clientIP(ent, qtrue));

		// Only log this if it is set to 2+
		if (g_extendedLog.integer >= 2) 
			admLog(log);

		return;
	}
	return;
}

/*
===========
Passes any vote in progress
===========
*/
void cmd_passvote(gentity_t *ent) {
	char *tag, *log;

	tag = sortTag(ent);

	if (level.voteTime) {
		level.voteYes = level.numConnectedClients;
		CheckVote();
		AP(va("cp \"%s has ^3Passed the vote.\n\"2", tag));
		AP("chat \"console: Turns out everyone voted Yes^2!\n\"");

		// Log it
		log = va("Player %s (IP: %s) passed a vote.",
			ent->client->pers.netname, clientIP(ent, qtrue));

		// Only log this if it is set to 2+
		if (g_extendedLog.integer >= 2) 
			admLog(log);

		return;
	}
	return;
}

/*
===========
Map restart
===========
*/
void cmd_restart(gentity_t *ent) {
	char *tag, *log;
	char *s;
	int num;
	s = ConcatArgs(2);

	tag = sortTag(ent);
	AP(va("chat \"console: %s has ^3restarted ^7the map^3.\n\"", tag));

	if (strlen(s)) {
		num = atoi(s);
		trap_SendConsoleCommand(EXEC_APPEND, va("map_restart %i", num));
	}
	else
		trap_SendConsoleCommand(EXEC_APPEND, va("map_restart"));

	// Log it
	log = va("Player %s (IP: %s) has restarted the map.",
		ent->client->pers.netname, clientIP(ent, qtrue));

	// Only log this if it is set to 2+
	if (g_extendedLog.integer >= 2)
		admLog(log);

	return;
}

/*
===========
Reset match
===========
*/
void cmd_resetmatch(gentity_t *ent) {
	char *tag, *log;

	tag = sortTag(ent);
	AP(va("chat \"console: %s has ^3reset ^7match^3.\n\"", tag));
	trap_SendConsoleCommand(EXEC_APPEND, va("reset_match"));

	// Log it
	log = va("Player %s (IP: %s) has reset match.",
		ent->client->pers.netname, clientIP(ent, qtrue));

	// Only log this if it is set to 2+
	if (g_extendedLog.integer >= 2) 
		admLog(log);

	return;
}

/*
===========
Swap teams
===========
*/
void cmd_swap(gentity_t *ent) {
	char *tag, *log;

	tag = sortTag(ent);
	AP(va("chat \"console: %s has ^3swapped ^7the teams^3.\n\"", tag));
	trap_SendConsoleCommand(EXEC_APPEND, va("swap_teams"));

	// Log it
	log = va("Player %s (IP: %s) has swapped teams.",
		ent->client->pers.netname, clientIP(ent, qtrue));

	// Only log this if it is set to 2+
	if (g_extendedLog.integer >= 2)
		admLog(log);

	return;
}

/*
===========
Shuffle
===========
*/
void cmd_shuffle(gentity_t *ent) {
	char *tag, *log;
	int count = 0, tmpCount, i;
	int players[MAX_CLIENTS];

	tag = sortTag(ent);
	memset(players, -1, sizeof(players));

	for (i = 0; i < MAX_CLIENTS; i++)
	{
		if ((!g_entities[i].inuse) || (level.clients[i].pers.connected != CON_CONNECTED))
			continue;

		// Ignore spectators
		if ((level.clients[i].sess.sessionTeam != TEAM_RED) && (level.clients[i].sess.sessionTeam != TEAM_BLUE))
			continue;

		players[count] = i;
		count++;
	}

	tmpCount = count;

	for (i = 0; i < count; i++)
	{
		int j;

		do {
			j = (rand() % count);
		} while (players[j] == -1);

		if (i & 1)
			level.clients[players[j]].sess.sessionTeam = TEAM_BLUE;
		else
			level.clients[players[j]].sess.sessionTeam = TEAM_RED;

		ClientUserinfoChanged(players[j]);
		ClientBegin(players[j]);

		players[j] = players[tmpCount - 1];
		players[tmpCount - 1] = -1;
		tmpCount--;
	}

	AP(va("print \"console: %s has ^3shuffled ^7the teams^3.\n\"", tag));
	trap_SendConsoleCommand(EXEC_APPEND, va("reset_match %i\n", GS_WARMUP));

	// Log it
	log = va("Player %s (IP: %s) has shuffled the teams.",
		ent->client->pers.netname, clientIP(ent, qtrue));

	// Only log this if it is set to 2+
	if (g_extendedLog.integer >= 2)
		admLog(log);

	return;
}

/*
==================
Move lagged out or downloading clients to spectators
==================
*/
qboolean cmd_specs999(gentity_t *ent) {
	int i;
	qboolean moved = qfalse;
	char *tag, *log;

	tag = sortTag(ent);
	for (i = 0; i < level.maxclients; i++) {
		ent = &g_entities[i];
		if (!ent->client) continue;
		if (ent->client->pers.connected != CON_CONNECTED) continue;
		if (ent->client->ps.ping >= 999) {
			SetTeam(ent, "s", qtrue);
			moved = qtrue;
		}
	}

	if (moved == qtrue)
		AP(va("chat \"console: %s moved all lagged out players to spectators^3!\n\"", tag));
	else
		CP("print \"No one to move to spectators^1!\n\"");

	// Log it
	log = va("Player %s (IP: %s) forced all 999 to spectators.",
		ent->client->pers.netname, clientIP(ent, qtrue));

	// Only log this if it is set to 2+
	if (g_extendedLog.integer >= 2) 
		admLog(log);

	return qtrue;
}

/*
==================
Reveal location of a player.
==================
*/
void cmd_revealCamper(gentity_t *ent) {
	char location[64];
	int	clientNum;
	char *tag, *log, *log2;

	tag = sortTag(ent);
	log2 = va("Player %s (IP: %s)",	ent->client->pers.netname, clientIP(ent, qtrue));
	clientNum = ClientNumberFromString(ent, ent->client->pers.cmd2);

	//check if target is not a client
	if (clientNum == -1) {
		return; 
	}

	// Give values to these
	ent = g_entities + clientNum;

	Team_GetLocationMsg(ent, location, sizeof(location), qtrue);
	AP(va("chat \"console: %s has releaved that player %s ^7is hiding at ^3%s^7.\n\"", tag, ent->client->pers.netname, location));

	// Log it
	log = va("%s has revealed %s location.", log2, ent->client->pers.netname);

	// Only log this if it is set to 2+
	if (g_extendedLog.integer >= 2)
		admLog(log);

	return;
}

/*
===========
Rename player
===========
*/
void cmd_rename(gentity_t *ent) {
	int          clientNum;
	gclient_t	 *client;
	char *tag, *log;
	char userinfo[MAX_INFO_STRING];

	tag = sortTag(ent);

	clientNum = ClientNumberFromString(ent, ent->client->pers.cmd2);

	if (clientNum == -1) {
		return;
	}

	// Ugly..
	log = va("Player %s (IP: %s) has renamed user",
		ent->client->pers.netname, clientIP(ent, qtrue));

	ent = g_entities + clientNum;
	client = ent->client;

	log = va("%s %s to", log, client->pers.netname);

	// Print first..
	AP(va("chat \"console: %s has renamed player %s ^7to %s^3!\n\"", tag, client->pers.netname, ConcatArgs(3)));

	// Rename..
	trap_GetUserinfo(client->ps.clientNum, userinfo, sizeof(userinfo));
	Info_SetValueForKey(userinfo, "name", ConcatArgs(3));
	trap_SetUserinfo(client->ps.clientNum, userinfo);
	ClientUserinfoChanged(client->ps.clientNum);

	// Log it
	log = va("%s %s", log, ConcatArgs(3));

	// Not vital..
	if (g_extendedLog.integer > 1)
		admLog(log);

	return;
}

/*
===========
Renameon/renameoff

Takes or restores ability to rename from client.
NOTE: Taking ability to rename lasts only for that round..
===========
*/
void cmd_nameHandle(gentity_t *ent, qboolean revoke) {
	int	player_id;
	gentity_t	*targetclient;
	char *tag, *log, *action;

	tag = sortTag(ent);

	player_id = ClientNumberFromString(ent, ent->client->pers.cmd2);
	if (player_id == -1) {
		return;
	}

	targetclient = g_entities + player_id;

	if (revoke && targetclient->client->pers.nameLocked) {
		CP(va("print \"^1Error: ^7%s ^7is already name locked!\n\"", targetclient->client->pers.netname));
		return;
	}
	else if (!revoke && !targetclient->client->pers.nameLocked) {
		CP(va("print \"^1Error: ^7%s ^7already can rename!\n\"", targetclient->client->pers.netname));
		return;
	}

	action = revoke ? "revoked" : "restored";
	AP(va("chat \"console:^7 %s has %s %s^7's ability to rename.\n\"", tag, action, targetclient->client->pers.netname));
	targetclient->client->pers.nameLocked = revoke;

	// Log it
	log = va("Player %s (IP: %s) has %s %s^7's ability to rename",
		ent->client->pers.netname, clientIP(ent, qtrue), action, targetclient->client->pers.netname);
	admLog(log);

	return;
}

/*
===========
Lock/Unlock team

NOTE: Somewhat messy due verbosity
TODO: Clean this eventually..
===========
*/
qboolean canTeamBeLocked(int team)
{
	if (team == TEAM_RED && level.axisPlayers < 1)
		return qfalse;
	else if (team == TEAM_BLUE && level.alliedPlayers < 1)
		return qfalse;
	else
		return qtrue;
}
// Lock/Unlock
void cmd_handleTeamLock( gentity_t *ent, qboolean tLock ) {
	char *tag = sortTag(ent);	
	char *cmd = ent->client->pers.cmd2;
	char *action = (tLock ? "Lock" : "Unlock");
	int team = TEAM_NUM_TEAMS;
	char *teamTag = "^3Both^7";
	char *log;

	if (!strcmp(cmd, "")) {
		CP(va("print \"^1Error: ^7Please select which team you wish to %s!\n\"", action));
		return;
	}	

	// Axis
	if (!strcmp(cmd, "red") || !strcmp(cmd, "axis") || !strcmp(cmd, "r")) {
		team = TEAM_RED;
		teamTag = "^1Axis^7";
	}
	// Allies
	else if (!strcmp(cmd, "blue") || !strcmp(cmd, "allies") || !strcmp(cmd, "allied") || !strcmp(cmd, "b"))	{
		team = TEAM_BLUE;
		teamTag = "^4Allied^7";
	} 
	// Both
	else if (!(strcmp(cmd, "both") == 0))
	{
		CP(va("print \"^1Error: ^7Please select which team you wish to %s!\n\"", action));
		return;
	}

	if (team != TEAM_NUM_TEAMS)
	{
		if (teamInfo[team].team_lock == tLock)
		{	
			CP(va("print \"Error: %s team is already %sed!  \n\"", teamTag, action));
			return;
		}
		else
		{
			if (!canTeamBeLocked(team))
			{
				CP(va("print \"Error: %s team is empty!\n\"", teamTag));
				return;
			}

			AP(va("chat \"console: %s has %sed %s team!\n\"", tag, action, teamTag));
			teamInfo[team].team_lock = tLock;
		}
	}
	else
	{	
		if (teamInfo[TEAM_RED].team_lock != tLock || teamInfo[TEAM_BLUE].team_lock != tLock)
		{
			teamInfo[TEAM_RED].team_lock = tLock;
			teamInfo[TEAM_BLUE].team_lock = tLock;
			AP(va("chat \"console: %s has %sed %s teams!\n\"", tag, action, teamTag));
		}
		else
			CP(va("print \"Error: Both teams are already %sed!  \n\"", action));
		return;
	}

	// Log it
	log = va("Player %s (IP: %s) has issued %s for %s",
		ent->client->pers.netname, clientIP(ent, qtrue), action, 
		(team == TEAM_RED ? "Axis team" : (team == TEAM_BLUE ? "Allied team" : "Both teams" )));
	admLog(log);

	return;
}

/*
===========
Ban ip
===========
*/
void cmd_ban(gentity_t *ent) {
	int count = 0;
	int i;
	int nums[MAX_CLIENTS];
	char *tag, *log;

	tag = sortTag(ent);
	count = ClientNumberFromNameMatch(ent->client->pers.cmd2, nums);

	if (count == 0){
		CP("print \"Client not on server^3!\n\"");
		return;
	}
	else if (count > 1) {
		CP(va("print \"Too many people with ^3%s ^7in their name^3!\n\"", ent->client->pers.cmd2));
		return;
	}

	for (i = 0; i < count; i++){
		// Ban player			
		trap_SendConsoleCommand(EXEC_APPEND, va("addip %i.%i.%i.%i/32|none|%s|%s|none",
			g_entities[nums[i]].client->sess.ip[0], g_entities[nums[i]].client->sess.ip[1],
			g_entities[nums[i]].client->sess.ip[2], g_entities[nums[i]].client->sess.ip[3],
			g_entities[nums[i]].client->pers.netname, getDate()));

		// Kick player now
		trap_DropClient(nums[i], va("banned by ^3%s \n^7%s", tag, ent->client->pers.cmd3));
		AP(va("chat \"console: %s has banned player %s^3! ^3%s\n\"", tag, g_entities[nums[i]].client->pers.netname, ent->client->pers.cmd3));

		// Log it
		log = va("Player %s (IP: %s) has (IP)banned user %s",
			ent->client->pers.netname, clientIP(ent, qtrue), g_entities[nums[i]].client->pers.netname);
		admLog(log);
	}

	return;
}

/*
===========
RangeBan ip
===========
*/
void cmd_rangeBan(gentity_t *ent) {
	int count = 0;
	int i;
	int nums[MAX_CLIENTS];
	char *tag, *log;
	char *range = ent->client->pers.cmd3;

	tag = sortTag(ent);
	count = ClientNumberFromNameMatch(ent->client->pers.cmd2, nums);

	if (count == 0){
		CP("print \"Client not on server^3!\n\"");
		return;
	}
	else if (count > 1) {
		CP(va("print \"Too many people with ^3%s ^7in their name^3!\n\"", ent->client->pers.cmd2));
		return;
	}

	// Figure out what we're about
	if (Q_stricmp(range, "32") && Q_stricmp(range, "24") && Q_stricmp(range, "16") && Q_stricmp(range, "8")) {
		if (Q_stricmp(range, ""))
			CP(va("print \"^3Error: ^7%s is not a valid Subnet Range!\n^3Valid input is: ^7!rangeban <player> <8/16/24/32>\n", range));
		else
			CP(va("print \"^3Error: ^7Missing Subnet!\n^3Valid input is: ^7!rangeban <player> <8/16/24/32>\n", range));
		return;
	}

	for (i = 0; i < count; i++){
		// Ban player			
		trap_SendConsoleCommand(EXEC_APPEND, va("addip %i.%i.%i.%i/%s|none|%s|%s|none",
			g_entities[nums[i]].client->sess.ip[0], g_entities[nums[i]].client->sess.ip[1],
			g_entities[nums[i]].client->sess.ip[2], g_entities[nums[i]].client->sess.ip[3],
			range, g_entities[nums[i]].client->pers.netname, getDate()));

		// Kick player now
		trap_DropClient(nums[i], va("banned by ^3%s", tag));
		AP(va("chat \"console: %s has banned player %s^3!\n\"", tag, g_entities[nums[i]].client->pers.netname));

		// Log it
		log = va("Player %s (IP: %s) has (IP)rangeBanned user %s",
			ent->client->pers.netname, clientIP(ent, qtrue), g_entities[nums[i]].client->pers.netname);
		admLog(log);
	}

	return;
}

/*
===========
tempBan ip
===========
*/
void cmd_tempBan(gentity_t *ent) {
	int count = 0;
	int i;
	int nums[MAX_CLIENTS];
	char *tag, *log;
	int time;

	tag = sortTag(ent);
	count = ClientNumberFromNameMatch(ent->client->pers.cmd2, nums);
	if (count == 0){
		CP("print \"Client not on server^3!\n\"");
		return;
	}
	else if (count > 1) {
		CP(va("print \"To many people with ^3%s ^7in their name^3!\n\"", ent->client->pers.cmd2));
		return;
	}

	// Don't allow Tempban to be higher than a week..let them use banning for that..
	time = (atoi(ent->client->pers.cmd3) > 10080 ? 10080 : (int)ent->client->pers.cmd3);

	for (i = 0; i < count; i++){
		// TempBan player			
		trap_SendConsoleCommand(EXEC_APPEND, va("tempban %i %d", nums[i], time));

		// Kick player now
		trap_DropClient(nums[i], va("Temporarily banned by ^3%s \n^7Tempban will expire in ^3%d ^7minutes", tag, time));
		AP(va("chat \"console: %s has tempbanned player %s ^7for ^3%d ^7minutes^3!\n\"", tag, g_entities[nums[i]].client->pers.netname, time));

		// Log it
		log = va("Player %s (IP: %s) tempbanned user %s by IP for %d minutes.",
			ent->client->pers.netname, clientIP(ent, qtrue), g_entities[nums[i]].client->pers.netname, time);
		admLog(log);
	}
	return;
}

/*
===========
Add IP

FIXME: 
- Convert * to subranges (multiply *(8) by times : *.* = 16 )
- Add some input checks..
===========
*/
void cmd_addIp(gentity_t *ent) {
	char *tag, *log;

	tag = sortTag(ent);

	// Note that this blindly accepts what ever user inputs. Not ideal..
	trap_SendConsoleCommand(EXEC_APPEND, va("addip %s", ent->client->pers.cmd2));
	AP(va("chat \"console: %s has added IP ^3%s ^7to banned file.\n\"", tag, ent->client->pers.cmd2));

	// Log it
	log = va("Player %s (IP: %s) added IP %s to banned file.",
		ent->client->pers.netname, clientIP(ent, qtrue), ent->client->pers.cmd2);
	admLog(log);

	return;
}
