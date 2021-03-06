/*/
===========================================================================
L0 / rtcwPub :: g_admin.c

	Main Admin functionality.

Created: 24. Mar / 2014
===========================================================================
*/
#include "g_admin.h"

/*
===========
Admin Log

Just a wrapper with syntax fixed..
===========
*/
void admLog(char *info) {
	
	if (!g_extendedLog.integer)
		return;

	logEntry(ADMACT, va("Time: %s\n%s%s", getTime(), info, LOGLINE));
}
// Admin Activity


/*
===========
Sort tag
===========
*/
char *sortTag(gentity_t *ent) {
	char n1[MAX_STRING_TOKENS]; // Nobo - this needs to be atleast 256.. matching vmCvar's string (otherwise crash if > 36(previous size))

	if (ent->client->sess.admin == ADM_1)
		SanitizeString(a1_tag.string, n1, qtrue);
	else if (ent->client->sess.admin == ADM_2)
		SanitizeString(a2_tag.string, n1, qtrue);
	else if (ent->client->sess.admin == ADM_3)
		SanitizeString(a3_tag.string, n1, qtrue);
	else if (ent->client->sess.admin == ADM_4)
		SanitizeString(a4_tag.string, n1, qtrue);
	else if (ent->client->sess.admin == ADM_5)
		SanitizeString(a5_tag.string, n1, qtrue);
	else
		Q_strncpyz(n1, "", sizeof(n1));

	// Has to be done twice to cope with double carrots..
	Q_CleanStr(n1);
	//n2[10] = 0; Nobo - It may look nicer, but the size should be left to the descretion of the server owner.

	return va("%s", n1);
}

/*
===========
Login
===========
*/
void cmd_do_login(gentity_t *ent, qboolean silent) {
	char str[MAX_TOKEN_CHARS];
	qboolean error;
	char *log;

	error = qfalse;
	trap_Argv(1, str, sizeof(str));

	// Make sure user is not already logged in.
	if (ent->client->sess.admin != ADM_NONE) {
		CP("print \"You are already logged in^1!\n\"");
		return;
	}

	// Prevent bogus logins	
	if ((!Q_stricmp(str, "\0"))
		|| (!Q_stricmp(str, ""))
		|| (!Q_stricmp(str, "\""))
		|| (!Q_stricmp(str, "none")))
	{
		CP("print \"Incorrect password^1!\n\"");
		// No log here to avoid login by error..
		return;
	}

	// Else let's see if there's a password match.
	if ((Q_stricmp(str, a1_pass.string) == 0)
		|| (Q_stricmp(str, a2_pass.string) == 0)
		|| (Q_stricmp(str, a3_pass.string) == 0)
		|| (Q_stricmp(str, a4_pass.string) == 0)
		|| (Q_stricmp(str, a5_pass.string) == 0))
	{
		// Always start with lower level as if owner screws it up 
		// and sets the same passes for more levels, the lowest is the safest bet.
		if (Q_stricmp(str, a1_pass.string) == 0) {
			ent->client->sess.admin = ADM_1;
		}
		else if (Q_stricmp(str, a2_pass.string) == 0) {
			ent->client->sess.admin = ADM_2;
		}
		else if (Q_stricmp(str, a3_pass.string) == 0) {
			ent->client->sess.admin = ADM_3;
		}
		else if (Q_stricmp(str, a4_pass.string) == 0) {
			ent->client->sess.admin = ADM_4;
		}
		else if (Q_stricmp(str, a5_pass.string) == 0) {
			ent->client->sess.admin = ADM_5;
		}
		else {
			error = qtrue;
		}
		// Something went to hell..
		if (error == qtrue) {
			// User shouldn't be anything but regular so make sure..
			ent->client->sess.admin = ADM_NONE;
			CP("print \"Error has occured while trying to log you in^1!\n\"");
			return;
		}

		// We came so far so go with it..
		if (silent) {
			CP("print \"Silent Login successful^2!\n\"");
			ent->client->sess.incognito = 1; // Hide them

			// Log it
			log = va("Time: %s\nPlayer %s (IP: %s) has silently logged in as %s.%s",
				getTime(),
				ent->client->pers.netname,
				clientIP(ent, qtrue),
				sortTag(ent),
				LOGLINE);

			if (g_extendedLog.integer)
				logEntry(ADMLOG, log);
		}
		else {
			AP(va("chat \"^7console: %s ^7has logged in as %s^7!\n\"", ent->client->pers.netname, sortTag(ent)));

			// Log it
			log = va("Time: %s\nPlayer %s (IP: %s) has logged in as %s.%s",
				getTime(),ent->client->pers.netname, clientIP(ent, qtrue), sortTag(ent), LOGLINE);

			if (g_extendedLog.integer)
				logEntry(ADMLOG, log);
		}
		return;
	}
	else
	{
		CP("print \"Incorrect password^1!\n\"");

		// Log it
		log = va("Time: %s\nPlayer %s (IP: %s) has tried to login using password: %s%s",
			getTime(), ent->client->pers.netname, clientIP(ent, qtrue), str, LOGLINE );

		if (g_extendedLog.integer)
			logEntry(PASSLOG, log);

		return;
	}
}

/*
===========
Logout
===========
*/
void cmd_do_logout(gentity_t *ent) {
	// If user is not logged in do nothing
	if (ent->client->sess.admin == ADM_NONE) {
		return;
	}
	else {
		// Admin is hidden so don't print 
		if (ent->client->sess.incognito)
			CP("print \"You have successfully logged out^3!\n\"");
		else
			AP(va("chat \"console: %s ^7has logged out^3!\n\"", ent->client->pers.netname));

		// Log them out now
		ent->client->sess.admin = ADM_NONE;

		// Set incognito to visible..
		ent->client->sess.incognito = 0;

		return;
	}
}

/*
===========
Get client number from name
===========
*/
int ClientNumberFromNameMatch(char *name, int *matches) {
	int i, textLen;
	char nm[32];
	char c;
	int index = 0;

	Q_strncpyz(nm, name, sizeof(nm));
	Q_CleanStr(nm);
	textLen = strlen(nm);
	c = *nm;

	for (i = 0; i < level.maxclients; i++)
	{
		int j, len;
		char playerName[32];

		if ((!g_entities[i].client) || (g_entities[i].client->pers.connected != CON_CONNECTED))
			continue;

		Q_strncpyz(playerName, g_entities[i].client->pers.netname, sizeof(playerName));
		Q_CleanStr(playerName);
		len = strlen(playerName);

		for (j = 0; j < len; j++)
		{
			if (tolower(c) == tolower(playerName[j]))
			{
				if (!Q_stricmpn(nm, playerName + j, textLen))
				{
					matches[index] = i;
					index++;
					break;
				}
			}
		}
	}
	return index;
}

/*
===========
Deals with ! & ?
===========
*/
void admCmds(const char *strCMD1, char *strCMD2, char *strCMD3, qboolean cmd){

	int i = 0, j = 0;
	int foundcolon = 0;

	while (strCMD1[i] != 0)
	{
		if (!foundcolon)
		{
			if (cmd) {
				if (strCMD1[i] == '?') {
					foundcolon = 1;
					strCMD2[i] = 0;
				}
				else
					strCMD2[i] = strCMD1[i];
				i++;
			}
			else {
				if (strCMD1[i] == '!') {
					foundcolon = 1;
					strCMD2[i] = 0;
				}
				else
					strCMD2[i] = strCMD1[i];
				i++;
			}
		}
		else
		{
			strCMD3[j++] = strCMD1[i++];
		}
	}
	if (!foundcolon)
		strCMD2[i] = 0;
	strCMD3[j] = 0;
}

/*
===========
Parse string (if I recall right this bit is from S4NDMoD)
===========
*/
void ParseAdmStr(const char *strInput, char *strCmd, char *strArgs)
{
	int i = 0, j = 0;
	int foundspace = 0;

	while (strInput[i] != 0){
		if (!foundspace){
			if (strInput[i] == ' '){
				foundspace = 1;
				strCmd[i] = 0;
			}
			else
				strCmd[i] = strInput[i];
			i++;
		}
		else{
			strArgs[j++] = strInput[i++];
		}
	}
	if (!foundspace)
		strCmd[i] = 0;

	strArgs[j] = 0;
}

/*
===========
Can't use command msg..
===========
*/
void cantUse(gentity_t *ent) {
	char alt[128];
	char cmd[128];

	admCmds(ent->client->pers.cmd1, alt, cmd, qfalse);

	CP(va("print \"Command ^1%s ^7is not allowed for your level^1!\n\"", cmd));
	return;
}

/*
===========
Determine if admin level allows command
===========
*/
qboolean canUse(gentity_t *ent, qboolean isCmd) {
	char *list = "";
	char alt[128];
	char cmd[128];

	switch (ent->client->sess.admin) {
	case ADM_NONE:
		return qfalse;
		break;
	case ADM_1:
		list = a1_cmds.string;
		break;
	case ADM_2:
		list = a2_cmds.string;
		break;
	case ADM_3:
		list = a3_cmds.string;
		break;
	case ADM_4:
		list = a4_cmds.string;
		break;
	case ADM_5:
		if (a5_allowAll.integer && isCmd) // Return true if allowAll is enabled and is command.
			return qtrue;
		else
			list = a5_cmds.string;  // Otherwise just loop thru string and see if there's a match.
		break;
	}

	admCmds(ent->client->pers.cmd1, alt, cmd, qfalse);

	return Q_FindToken(list, cmd);
}

/*
===========
Check banned

Checks if user is banned or tempbanned..
===========
*/
int checkBanned(char *data, char * password) {
	char		port[MAX_INFO_STRING];
	char		ip[20];

	GetIP(data, ip, port);

	if (Banned(ip, password))
		return 1;
	else if (TempBanned(ip))
		return 2;
	else 
		return 0;
}

/*
===========
Password bypass

Basically we re-use existing stuff..
===========
*/
qboolean bypassing(char *password) {
	return Q_FindToken(g_bypassPasswords.string, password);
}

/*
===========
List commands
===========
*/
void cmd_listCmds(gentity_t *ent) {
	char *cmds;

	if (!adm_help.integer) {
		CP("print \"Admin commands list is disabled on this server^1!\n\"");
		return;
	}

	// Keep an eye on this..so it's not to big..
	cmds = "incognito cmds ignore unignore clientignore clientunignore kick clientkick slap kill "
		"lock unlock specs axis allies exec nextmap map cpa cp chat warn cancelvote passvote restart "
		"reset swap shuffle spec999 whereis pause unpause rename renameon renameoff vstr ban tempban addip";

	if (ent->client->sess.admin == ADM_1)
		CP(va("print \"^3Available commands are:^7\n%s\n^3Use ? for help with command. E.g. ?incognito.\n\"", a1_cmds.string));
	else if (ent->client->sess.admin == ADM_2)
		CP(va("print \"^3Available commands are:^7\n%s\n^3Use ? for help with command. E.g. ?incognito.\n\"", a2_cmds.string));
	else if (ent->client->sess.admin == ADM_3)
		CP(va("print \"^3Available commands are:^7\n%s\n^3Use ? for help with command. E.g. ?incognito.\n\"", a3_cmds.string));
	else if (ent->client->sess.admin == ADM_4)
		CP(va("print \"^3Available commands are:^7\n%s\n^3Use ? for help with command. E.g. ?incognito.\n\"", a4_cmds.string));
	else if (ent->client->sess.admin == 5 && !a5_allowAll.integer)
		CP(va("print \"^3Available commands are:^7\n%s\n^3Use ? for help with command. E.g. ?incognito.\n\"", a5_cmds.string));
	else if (ent->client->sess.admin == 5 && a5_allowAll.integer)
		CP(va("print \"^3Available commands are:^7\n%s\n^5Additional server commands:^7\n%s\n^3Use ? for help with command. E.g. ?incognito.\n\"", cmds, a5_cmds.string));

	return;
}

/*
===========
Admin commands
===========
*/
qboolean do_cmds(gentity_t *ent) {
	char alt[128];
	char cmd[128];

	admCmds(ent->client->pers.cmd1, alt, cmd, qfalse);

	if (!strcmp(cmd, "incognito"))			{ if (canUse(ent, qtrue)) cmd_incognito(ent); else cantUse(ent);	return qtrue; }
	else if (!strcmp(cmd, "cmds"))			{ cmd_listCmds(ent);	return qtrue; }
	else if (!strcmp(cmd, "ignore"))		{ if (canUse(ent, qtrue)) cmd_ignore(ent);	else cantUse(ent); return qtrue; }
	else if (!strcmp(cmd, "unignore"))		{ if (canUse(ent, qtrue)) cmd_unignore(ent); else cantUse(ent); return qtrue; }
	else if (!strcmp(cmd, "clientignore"))	{ if (canUse(ent, qtrue)) cmd_clientIgnore(ent); else cantUse(ent); return qtrue; }
	else if (!strcmp(cmd, "clientunignore")){ if (canUse(ent, qtrue)) cmd_clientUnignore(ent); else cantUse(ent); return qtrue; }
	else if (!strcmp(cmd, "kick"))			{ if (canUse(ent, qtrue)) cmd_kick(ent); else cantUse(ent); return qtrue; }
	else if (!strcmp(cmd, "clientkick"))	{ if (canUse(ent, qtrue)) cmd_clientkick(ent);	else cantUse(ent); return qtrue; }
	else if (!strcmp(cmd, "slap"))			{ if (canUse(ent, qtrue)) cmd_slap(ent); else cantUse(ent); return qtrue; }
	else if (!strcmp(cmd, "kill"))			{ if (canUse(ent, qtrue)) cmd_kill(ent); else cantUse(ent); return qtrue; }
	else if (!strcmp(cmd, "specs"))			{ if (canUse(ent, qtrue)) cmd_specs(ent); else cantUse(ent); return qtrue; }
	else if (!strcmp(cmd, "axis"))			{ if (canUse(ent, qtrue)) cmd_axis(ent); else cantUse(ent); return qtrue; }
	else if (!strcmp(cmd, "allies"))		{ if (canUse(ent, qtrue)) cmd_allied(ent); else cantUse(ent); return qtrue; }
	else if (!strcmp(cmd, "exec"))			{ if (canUse(ent, qtrue)) cmd_exec(ent); else cantUse(ent); return qtrue; }
	else if (!strcmp(cmd, "nextmap"))		{ if (canUse(ent, qtrue)) cmd_nextmap(ent); else cantUse(ent); return qtrue; }
	else if (!strcmp(cmd, "map"))			{ if (canUse(ent, qtrue)) cmd_map(ent); else cantUse(ent); return qtrue; }
	else if (!strcmp(cmd, "vstr"))			{ if (canUse(ent, qtrue)) cmd_vstr(ent); else cantUse(ent); return qtrue; }
	else if (!strcmp(cmd, "cpa"))			{ if (canUse(ent, qtrue)) cmd_cpa(ent); else cantUse(ent); return qtrue; }
	else if (!strcmp(cmd, "cp"))			{ if (canUse(ent, qtrue)) cmd_cp(ent); else cantUse(ent); return qtrue; }
	else if (!strcmp(cmd, "chat"))			{ if (canUse(ent, qtrue)) cmd_chat(ent); else cantUse(ent); return qtrue; }
	else if (!strcmp(cmd, "warn"))			{ if (canUse(ent, qtrue)) cmd_warn(ent); else cantUse(ent); return qtrue; }
	else if (!strcmp(cmd, "cancelvote"))	{ if (canUse(ent, qtrue)) cmd_cancelvote(ent); else cantUse(ent); return qtrue; }
	else if (!strcmp(cmd, "passvote"))		{ if (canUse(ent, qtrue)) cmd_passvote(ent); else cantUse(ent); return qtrue; }
	else if (!strcmp(cmd, "restart"))		{ if (canUse(ent, qtrue)) cmd_restart(ent); else cantUse(ent); return qtrue; }
	else if (!strcmp(cmd, "reset"))			{ if (canUse(ent, qtrue)) cmd_resetmatch(ent); else cantUse(ent); return qtrue; }
	else if (!strcmp(cmd, "swap"))			{ if (canUse(ent, qtrue)) cmd_swap(ent); else cantUse(ent); return qtrue; }
	else if (!strcmp(cmd, "shuffle"))		{ if (canUse(ent, qtrue)) cmd_shuffle(ent); else cantUse(ent); return qtrue; }
	else if (!strcmp(cmd, "spec999"))		{ if (canUse(ent, qtrue)) cmd_specs999(ent); else cantUse(ent); return qtrue; }
	else if (!strcmp(cmd, "whereis"))		{ if (canUse(ent, qtrue)) cmd_revealCamper(ent); else cantUse(ent); return qtrue; }
	else if (!strcmp(cmd, "rename"))		{ if (canUse(ent, qtrue)) cmd_rename(ent); else cantUse(ent); return qtrue; }
	else if (!strcmp(cmd, "renameon"))		{ if (canUse(ent, qtrue)) cmd_nameHandle(ent, qfalse); else cantUse(ent); return qtrue; }
	else if (!strcmp(cmd, "renameoff"))		{ if (canUse(ent, qtrue)) cmd_nameHandle(ent, qtrue); else cantUse(ent); return qtrue; }
	else if (!strcmp(cmd, "lock"))			{ if (canUse(ent, qtrue)) cmd_handleTeamLock(ent, qtrue); else cantUse(ent); return qtrue; }
	else if (!strcmp(cmd, "unlock"))		{ if (canUse(ent, qtrue)) cmd_handleTeamLock(ent, qfalse); else cantUse(ent); return qtrue; }
	else if (!strcmp(cmd, "ban"))			{ if (canUse(ent, qtrue)) cmd_ban(ent); else cantUse(ent); return qtrue; }
	else if (!strcmp(cmd, "rangeban"))		{ if (canUse(ent, qtrue)) cmd_rangeBan(ent); else cantUse(ent); return qtrue; }
	else if (!strcmp(cmd, "tempban"))		{ if (canUse(ent, qtrue)) cmd_tempBan(ent); else cantUse(ent); return qtrue; }
	else if (!strcmp(cmd, "addip"))			{ if (canUse(ent, qtrue)) cmd_addIp(ent); else cantUse(ent); return qtrue; }
	// Any other command
	else if (canUse(ent, qfalse))			{ cmd_custom(ent, cmd); return qtrue; }

	// It failed on all checks..
	else { CP(va("print \"Command ^1%s ^7was not found^1!\n\"", cmd)); return qfalse; }

}

/*
===========
Admin help
===========
*/
typedef struct {
	char *command;
	char *help;
	char *usage;
} helpCmd_reference_t;

#define _HELP(x,y,z) {x, y, z},
/**
* Fairly straight forward approach _HELP(COMMAND, DESCRIPTION, USAGE)
* Alternatively, usage can be empty.
* Add new as needed..
*/
static const helpCmd_reference_t helpInfo[] = {
	_HELP("help", "Prints help about specific command.", "?COMMAND")
	_HELP("login", "Logs you in as Administrator.", NULL)
	_HELP("@login", "Silently logs you in as Administrator.", NULL)
	_HELP("logout", "Removes you from Administrator status.", NULL)
	_HELP("incognito", "Toggles your Admin status from hidden to visible or other way around.", NULL)
	_HELP("getstatus", "Shows basic info of all connected players.", NULL)
	_HELP("cmds", "Shows all available commands for your level.", NULL)
	_HELP("ignore", "Takes player's ability to chat, use vsay or callvotes.", "Uses unique part of name!")
	_HELP("unignore", "Restores player's ability to chat, use vsay or call votes.", "Uses unique part of name!")
	_HELP("clientignore", "Takes player's ability to chat, callvotes or use vsay.", "Uses client slot!")
	_HELP("clientunignore", "Restores player's ability to chat, callvotes or use vsay.", "Uses client slot!")
	_HELP("kick", "Kicks player from server.", "Uses unique part of name! Optionally you can add a message.")
	_HELP("clientkick", "Kicks player from server.", "Uses client slot number! Optionally you can add a message.")
	_HELP("slap", "Slaps player and takes 20hp.", "Uses client slot number!")
	_HELP("kill", "Kills player on spot.", "Uses client slot number!")
	_HELP("lock", "Locks the team(s) so players can't join.", "Usage !lock <red/axis> <blue/allied> <both>")
	_HELP("unlock", "Unlocks the team(s) so players can join.", "Usage !unlock <red/axis/r> <blue/allied/b> <both>")
	_HELP("specs", "Forces player to spectators.", "Uses unique part of name!")
	_HELP("axis", "Forces player to Axis team.", "Uses unique part of name!")
	_HELP("allies", "Forces player to Allied team.", "Uses unique part of name!")
	_HELP("exec", "Executes server config file.", "You can use @ at the end to silently execute file, e.g. !exec server @")
	_HELP("nextmap", "Loads the nextmap.", NULL)
	_HELP("exec", "Executes config on a server. Note! Write full name.", "E.g. !exec server.cfg")
	_HELP("map", "Loads the map of your choice.", "!map mp_base")
	_HELP("cpa", "Center Prints Admin message to everyone.", "!cpa <msg>")
	_HELP("cp", "Center Prints Admin message to selected user.", "Uses client slot number!")
	_HELP("chat", "Shows warning message to all in global chat.", "!chat <msg>")
	_HELP("warn", "Shows warning message to all in global chat and center print.", "!warn <msg>")
	_HELP("cancelvote", "Cancels any vote in progress.", NULL)
	_HELP("passvote", "Passes any vote in progress.", NULL)
	_HELP("restart", "Restarts the round.", NULL)
	_HELP("reset", "Resets the match.", NULL)
	_HELP("swap", "Swaps the teams.", NULL)
	_HELP("shuffle", "Shuffles the teams.", NULL)
	_HELP("spec999", "Moves all lagged (999) players to spectators.", NULL)
	_HELP("whereis", "Reveals players location to all.", "Uses client slot number!")
	_HELP("ban", "Bans player.", "!ban <unique part of name>")
	_HELP("rangeban", "Range Bans player.", "!rangeban <unique part of name> <8/16/24/32>")
	_HELP("tempban", "Temporarily Bans player.", "!tempban <unique part of name> <mins>")
	_HELP("addip", "Adds IP to banned file. You can also use subranges.", "example - !addip 100.100.100.100/32")
	_HELP("rename", "Renames players.", "!rename <client slot> <new name>")
	_HELP("vstr", "Loads a level from rotation file. Note - You need to know rotation labels..", "!vstr map1")
	_HELP("renameon", "Restores ability to rename from client.", "!renameon <client number>")
	_HELP("renameoff", "Revokes ability to rename from client (lasts only that round).", "!renameoff <client number>")
	// --> Add new ones after this line..

	{
		NULL, NULL, NULL
	}
};

qboolean do_help(gentity_t *ent) {
	char alt[128];
	char cmd[128];
	unsigned int i, \
		aHelp = ARRAY_LEN(helpInfo);
	const helpCmd_reference_t *hCM;
	qboolean wasUsed = qfalse;

	admCmds(ent->client->pers.cmd1, alt, cmd, qtrue);

	for (i = 0; i < aHelp; i++) {
		hCM = &helpInfo[i];
		if (NULL != hCM->command && 0 == Q_stricmp(cmd, hCM->command)) {
			CP(va("print \"^3%s %s %s\n\"",
				va(hCM->usage ? "Help ^7:" : "Help^7:"),
				hCM->help,
				va("%s", (hCM->usage ? va("\n^3Usage^7: %s\n", hCM->usage) : ""))));
			wasUsed = qtrue;
		}
	}
	return wasUsed;
}

/*
===========
Commands
===========
*/
qboolean cmds_admin(char cmd[MAX_TOKEN_CHARS], gentity_t *ent) {

	// We're dealing with command
	if (Q_stricmp(cmd, "!") == 0) {
		return do_cmds(ent);
	}
	// We're dealing with help
	else if (Q_stricmp(cmd, "?") == 0) {
		return do_help(ent);
	}

	return qfalse;
}
