/*/
===========================================================================
L0 / rtcwPub :: g_admin.h

	Admin Prototypes

Created: 24. Mar / 2014
===========================================================================
*/
#ifndef __ADMIN_H
#define __ADMIN_H

#include "g_local.h"

// Logs
#define ADMLOG		"./logs/adminLogins.log"
#define PASSLOG		"./logs/adminLoginAttempts.log"
#define ADMACT		"./logs/adminActions.log"
#define BYPASSLOG	"./logs/banBypass.log"
#define SYSLOG		"./logs/systemEvents.log"

//
// g_admin.c
//
char *sortTag(gentity_t *ent);
void ParseAdmStr(const char *strInput, char *strCmd, char *strArgs);
qboolean cmds_admin(char cmd[MAX_TOKEN_CHARS], gentity_t *ent);
void cmd_do_login(gentity_t *ent, qboolean silent);
void cmd_do_logout(gentity_t *ent);
int ClientNumberFromNameMatch(char *name, int *matches);

//
// g_admin_cmds.c
//
void cmd_incognito(gentity_t *ent);
void cmdCustom(gentity_t *ent, char *cmd);
void cmd_ignore(gentity_t *ent);
void cmd_unignore(gentity_t *ent);
void cmd_clientIgnore(gentity_t *ent);
void cmd_clientUnignore(gentity_t *ent);

#endif // __ADMIN_H
