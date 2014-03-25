/*/
===========================================================================
L0 / rtcwPub :: g_players.h

	New Player's commands..

Created: 25. Mar / 2014
===========================================================================
*/
#include "g_local.h"

/*
===========
Getstatus

Prints IP's and some match info..
===========
*/
void cmd_getstatus(gentity_t *ent) {
	gclient_t *cl;
	int	j;
	// uptime
	int secs = level.time / 1000;
	int mins = (secs / 60) % 60;
	int hours = (secs / 3600) % 24;
	int days = (secs / (3600 * 24));

	CP(va("print \"\n^3Mod: ^7%s \n^3Server: ^7%s\n\"", GAMEVERSION, sv_hostname.string));
	CP("print \"^3--------------------------------------------------------------------------\n\"");
	CP("print \"^7Sl : Team : Name       : ^3IP              ^7: Ping ^7: Status\n\"");
	CP("print \"^3--------------------------------------------------------------------------\n\"");

	for (j = 0; j <= (MAX_CLIENTS - 1); j++) {

		if (g_entities[j].client && !(ent->r.svFlags & SVF_BOT)) {
			char *team, *slot, *ip, *tag, *sortTag, *extra;
			char n1[MAX_NETNAME];
			char n2[MAX_NETNAME];
			cl = g_entities[j].client;

			// Has to be done twice to cope with double carrots..
			DecolorString(cl->pers.netname, n1);
			SanitizeString(n1, n2);
			Q_CleanStr(n2);
			n2[10] = 0;

			// player is connecting
			if (cl->pers.connected == CON_CONNECTING) {
				CP(va("print \"%2d  : >><< : %-10s : ^3>>Connecting<<  ^7:      : \n\"", j, n2));
			}

			// player is connected
			if (cl->pers.connected == CON_CONNECTED) {	
				tag = "";
				extra = "";				
				slot = va("%2d", j);

				team = (cl->sess.sessionTeam == TEAM_SPECTATOR) ? "^3SPEC^7" :
					(cl->sess.sessionTeam == TEAM_RED ? "^1Axis^7" : "^4Alld^7");

				ip = (cl->sess.admin == ADM_NONE) ?
					va("%i.%i.*.*", cl->sess.ip[0], cl->sess.ip[1]) :
					va("%i.%i.%i.%i", cl->sess.ip[0], cl->sess.ip[1], cl->sess.ip[2], cl->sess.ip[3]);

				switch (cl->sess.admin) {
					case ADM_NONE:
						sortTag = "";
						break;
					case ADM_1:
						sortTag = (cl->sess.incognito) ? va("%s ^7*", a1_tag.string) : va("%s", a1_tag.string);
						break;
					case ADM_2:
						sortTag = (cl->sess.incognito) ? va("%s ^7*", a2_tag.string) : va("%s", a2_tag.string);
						break;
					case ADM_3:
						sortTag = (cl->sess.incognito) ? va("%s ^7*", a3_tag.string) : va("%s", a3_tag.string);
						break;
					case ADM_4:
						sortTag = (cl->sess.incognito) ? va("%s ^7*", a4_tag.string) : va("%s", a4_tag.string);
						break;
					case ADM_5:
						sortTag = (cl->sess.incognito) ? va("%s ^7*", a5_tag.string) : va("%s", a5_tag.string);
						break;
				}

				// Grab any intel (bypass later on etc..
				if (cl->sess.admin == ADM_NONE) {
					if (cl->sess.ignored)
						extra = "^3Ignored";
				}

				if (ent->client->sess.admin == ADM_NONE) {
					tag = (cl->sess.admin != ADM_NONE && !cl->sess.incognito) ? sortTag : extra;
				} 
				else if (ent->client->sess.admin != ADM_NONE) {
					tag = (cl->sess.admin == ADM_NONE && cl->sess.ignored) ? "^3Ignored" : sortTag;
				}

				// Print it now
				CP(va("print \"%-2s : %s : %-10s : ^3%-15s ^7: %-4d ^7: %-12s \n\"",
					slot,
					team,
					n2,
					ip,
					cl->ps.ping,
					tag
					));
			}
		}
	}
	CP("print \"^3--------------------------------------------------------------------------\n\"");
	CP(va("print \"Time  : ^3%s \n^7Uptime: ^3%d ^7day%s ^3%d ^7hours ^3%d ^7minutes\n\"", getTime(), days, (days != 1 ? "s" : ""), hours, mins));
	CP("print \"\n\"");

	return;
}
