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
		CP("cp \"You are now incognito^z!\n\"2");
		return;
	}
	else {
		ent->client->sess.incognito = 0;
		CP("cp \"Your status is now set to visible^z!\n\"2");
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
void cmd_clientIgnore(gentity_t *ent)
{
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
void cmd_clientUnignore(gentity_t *ent)
{
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
