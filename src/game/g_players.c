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
	CP("print \"^7Sn : Team : Name       : ^3IP              ^7: Ping ^7: Status\n\"");
	CP("print \"^3--------------------------------------------------------------------------\n\"");

	for (j = 0; j <= (MAX_CLIENTS - 1); j++) {

		if (g_entities[j].client && !(ent->r.svFlags & SVF_BOT)) {
			char *team, *slot, *ip, *tag, *sortTag, *extra;
			char n1[MAX_NETNAME];
			char n2[MAX_NETNAME];
			cl = g_entities[j].client;

			// Has to be done twice to cope with double carrots..
			DecolorString(cl->pers.netname, n1);
			SanitizeString(n1, n2, qtrue);
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

/*
================
Throw knives

Originally BOTW (tho took it from s4ndmod as it's faster to implement..).
================
*/
// Knife "think" function                                                                                                                                                                                                                                                                                               ////////////////////////////////////////////
void Touch_Knife(gentity_t *ent, gentity_t *other, trace_t *trace) {
	qboolean hurt = qfalse;
	ent->active = qfalse;

	if (!other->client) {
		return;
	}
	if (other->health < 1) {
		return;
	}

	if (VectorLength(ent->s.pos.trDelta) != 0) {
		if ((g_friendlyFire.integer) || (!OnSameTeam(other, ent->parent))) {
			int i;
			int sound;
			int damage = 20;
			damage -= rand() % 10;

			if (damage <= 0) {
				damage = 1;
			}

			// pick a random sound to play
			i = rand() % 3;
			if (i == 0) {
				sound = G_SoundIndex("/sound/weapons/knife/knife_hit1.wav");
			}
			else if (i == 1) {
				sound = G_SoundIndex("/sound/weapons/knife/knife_hit2.wav");
			}
			else {
				sound = G_SoundIndex("/sound/weapons/knife/knife_hit3.wav");
			}

			G_Sound(other, sound);
			G_Damage(other, ent->parent, ent->parent, NULL, trace->endpos, damage, 0, MOD_THROWKNIFE);
			hurt = qtrue;
		}
	}

	if (hurt == qfalse) {
		int makenoise = EV_ITEM_PICKUP;

		if (g_throwKnives.integer > 0) {
			other->client->pers.throwingKnives++;
		}
		if (g_throwKnives.integer != 0) {
			if (other->client->pers.throwingKnives > (g_throwKnives.integer + 5)) {
				other->client->pers.throwingKnives = (g_throwKnives.integer + 5);
			}
		}
		if (ent->noise_index) {
			makenoise = EV_ITEM_PICKUP_QUIET;
			G_AddEvent(other, EV_GENERAL_SOUND, ent->noise_index);
		}
		if (other->client->pers.predictItemPickup) {
			G_AddPredictableEvent(other, makenoise, ent->s.modelindex);
		}
		else {
			G_AddEvent(other, makenoise, ent->s.modelindex);
		}
	}

	ent->freeAfterEvent = qtrue;
	ent->flags |= FL_NODRAW;
	ent->r.svFlags |= SVF_NOCLIENT;
	ent->s.eFlags |= EF_NODRAW;
	ent->r.contents = 0;
	ent->nextthink = 0;
	ent->think = 0;
}
// Actual command
void Cmd_ThrowKnives(gentity_t *ent) {
	vec3_t velocity, angles, offset, org, mins, maxs;
	trace_t tr;
	gentity_t *ent2;
	gitem_t *item = BG_FindItemForWeapon(WP_KNIFE);

	if (g_throwKnives.integer == 0) {
		return;
	}

	if (level.time < (ent->thrownKnifeTime + 800)) {
		return;
	}

	// If out or -1/unlimited
	if ((ent->client->pers.throwingKnives == 0) &&
		(g_throwKnives.integer != -1)) {
		return;
	}

	AngleVectors(ent->client->ps.viewangles, velocity, NULL, NULL);
	VectorScale(velocity, 64, offset);
	offset[2] += ent->client->ps.viewheight / 2;
	VectorScale(velocity, 800, velocity);
	velocity[2] += 50 + random() * 35;
	VectorAdd(ent->client->ps.origin, offset, org);
	VectorSet(mins, -ITEM_RADIUS, -ITEM_RADIUS, 0);
	VectorSet(maxs, ITEM_RADIUS, ITEM_RADIUS, 2 * ITEM_RADIUS);
	trap_Trace(&tr, ent->client->ps.origin, mins, maxs, org, ent->s.number, MASK_SOLID);
	VectorCopy(tr.endpos, org);

	G_Sound(ent, G_SoundIndex("sound/weapons/knife/knife_slash1.wav"));
	ent2 = LaunchItem(item, org, velocity, ent->client->ps.clientNum);
	VectorCopy(ent->client->ps.viewangles, angles);
	angles[1] += 90;
	G_SetAngle(ent2, angles);
	ent2->touch = Touch_Knife;
	ent2->parent = ent;

	if (g_throwKnives.integer > 0) {
		ent->client->pers.throwingKnives--;
	}

	//only show the message if throwing knives are enabled
	if (g_throwKnives.integer > 0) {
		CP(va("chat \"^3Knives left:^7 %d\" %i", ent->client->pers.throwingKnives, qfalse));
	}

	ent->thrownKnifeTime = level.time;
}
