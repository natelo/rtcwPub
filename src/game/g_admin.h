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

//
// g_admin_cmds.c
//
void cmd_incognito(gentity_t *ent);

#endif // __ADMIN_H