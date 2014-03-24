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
void cmd_custom(gentity_t *ent, char *cmd);
void cmd_ignore(gentity_t *ent);
void cmd_unignore(gentity_t *ent);
void cmd_clientIgnore(gentity_t *ent);
void cmd_clientUnignore(gentity_t *ent);
void cmd_kick(gentity_t *ent);
void cmd_clientkick(gentity_t *ent);
void cmd_slap(gentity_t *ent);
void cmd_kill(gentity_t *ent);
void cmd_specs(gentity_t *ent);
void cmd_axis(gentity_t *ent);
void cmd_allied(gentity_t *ent);
void cmd_exec(gentity_t *ent);
void cmd_nextmap(gentity_t *ent);
void cmd_map(gentity_t *ent);
void cmd_vstr(gentity_t *ent);

void cmd_cpa(gentity_t *ent);
void cmd_cp(gentity_t *ent);
void cmd_warn(gentity_t *ent);
void cmd_chat(gentity_t *ent);
void cmd_cancelvote(gentity_t *ent);
void cmd_passvote(gentity_t *ent);
void cmd_restart(gentity_t *ent);
void cmd_resetmatch(gentity_t *ent);
void cmd_swap(gentity_t *ent);
void cmd_shuffle(gentity_t *ent);
qboolean cmd_specs999(gentity_t *ent);
void cmd_revealCamper(gentity_t *ent);

#endif // __ADMIN_H
