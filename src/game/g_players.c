/*/
===========================================================================
L0 / rtcwPub :: g_players.h

	New Player's commands..

Created: 25. Mar / 2014
===========================================================================
*/
#include "g_admin.h"

/*
===========
Getstatus

Prints IP's and some match info..
===========
*/
void Cmd_getStatus(gentity_t *ent) {
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
void Cmd_throwKnives(gentity_t *ent) {
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

/*
================
Private messages
================
*/
void Cmd_pMsg(gentity_t *ent)
{
	char cmd[MAX_TOKEN_CHARS];
	char name[MAX_STRING_CHARS];
	char nameList[MAX_STRING_CHARS];
	char *msg;
	int matchList[MAX_CLIENTS];
	int count, i;

	if (!g_allowPMs.integer) {
		CP("print \"Private messages are Disabled^1!\n\"");
		return;
	}

	if (trap_Argc() < 3) {
		trap_Argv(0, cmd, sizeof(cmd));
		CP(va("print \"^3Usage:^7  %s <match> <message>\n\"", cmd));
		return;
	}

	if (ent->client->sess.ignored) {		
		CP("cp \"You are ignored^1!\n\"2");
		return;
	}

	trap_Argv(1, name, sizeof(name));
	if (strlen(name) < 2) {
		CP("print \"You must match at least ^32 ^7characters of the name^3!\n\"");
		return;
	}

	msg = ConcatArgs(2);
	if (strlen(msg) >= 700) {
		logEntry(SYSLOG, va("Nuking (Cmd_Pmsg :: strlen >= 700): %s (IP: %s)", ent->client->pers.netname, clientIP(ent, qtrue)));
		trap_DropClient(ent - g_entities, "^7Player Kicked: ^3Nuking");
		return;
	}

	count = ClientNumberFromNameMatch(name, matchList);
	if (count == 0) {
		CP("print \"No matching clients found^3!\n\"");
		return;
	}

	Q_strncpyz(nameList, "", sizeof(nameList));
	for (i = 0; i < count; i++)	{
		strcat(nameList, g_entities[matchList[i]].client->pers.netname);
		if (i != (count - 1))
			strcat(nameList, "^7, ");

		// Notify receiver(s)
		CPx(matchList[i], va("chat \"^3Message from ^7%s^7!\n\"", ent->client->pers.netname));
		CPx(matchList[i], va("cp \"^3New Private Message^7!\n\"2", ent->client->pers.netname));

		// Print full message in console..
		CPx(matchList[i], va("print \"^3Private Message from ^7%s^7: \n^3Message: ^7%.99s\n\"", ent->client->pers.netname, msg));

		// Send sound to them as well (keep an eye on this!)
		CPS(g_entities - matchList[i], "rtcwpub/sound/client/pm.wav");
	}

	// Let player know who received his message
	CP(va("print \"^3Message was sent to: ^7%s \n^3Message: ^7%.99s\n\"", nameList, msg));

	// It's unavoidable that others will add this at one point so I'll just save them some time.
	// But for the record: I'm totally not happy about this as imho it defeats the purpose.
	if (g_logPMs.integer)
		logEntry(PMLOG, va(
		"Sender: %s (IP: %s)\n"
			": Receiver(s): %s\n"
			": Message: %s\n"
			"---------------------------", 
		ent->client->pers.netname,
		clientIP(ent, qtrue),
		nameList,
		msg));
}

/*
================
Shows time
================
*/
void Cmd_Time(gentity_t *ent) {
	CP(va("chat \"%s^7 ^7current time is: ^3%s\n\"", getTime()));
	CPS(ent, "sound/multiplayer/dynamite_01.wav");
}

/*
===================
Drag players

Came from BOTW/S4NDMoD
===================
*/
void Cmd_Drag(gentity_t *ent) {
	gentity_t *target;
	vec3_t start, dir, end;
	trace_t tr;
	target = NULL;

	if (!g_dragBodies.integer)
		return;

	if (level.time < (ent->lastDragTime + 20))
		return;

	if (ent->client->ps.stats[STAT_HEALTH] <= 0)
		return;

	AngleVectors(ent->client->ps.viewangles, dir, NULL, NULL);

	VectorCopy(ent->s.pos.trBase, start);
	start[2] += ent->client->ps.viewheight;
	VectorMA(start, 100, dir, end);


	trap_Trace(&tr, start, NULL, NULL, end, ent->s.number, CONTENTS_CORPSE);

	if (tr.entityNum >= MAX_CLIENTS)
		return;

	target = &(g_entities[tr.entityNum]);

	if ((!target->inuse) || (!target->client))
		return;

	VectorCopy(target->r.currentOrigin, start);
	VectorCopy(ent->r.currentOrigin, end);
	VectorSubtract(end, start, dir);
	VectorNormalize(dir);
	VectorScale(dir, 100, target->client->ps.velocity);
	VectorCopy(dir, target->movedir);

	ent->lastDragTime = level.time;
}

/*
=================
L0 - Shove players away

Ported from BOTW source.
=================
*/
void Cmd_Push(gentity_t* ent)
{
	gentity_t *target;
	trace_t tr;
	vec3_t start, end, forward;
	float shoveAmount;

	if (!g_shove.integer)
		return;

	if (ent->client->ps.stats[STAT_HEALTH] <= 0)
		return;

	if (level.time < (ent->lastPushTime + 600))
		return;

	AngleVectors(ent->client->ps.viewangles, forward, NULL, NULL);

	VectorCopy(ent->s.pos.trBase, start);
	start[2] += ent->client->ps.viewheight;
	VectorMA(start, 128, forward, end);

	trap_Trace(&tr, start, NULL, NULL, end, ent->s.number, CONTENTS_BODY);

	if (tr.entityNum >= MAX_CLIENTS)
		return;

	target = &(g_entities[tr.entityNum]);

	if ((!target->inuse) || (!target->client))
		return;

	if (target->client->ps.stats[STAT_HEALTH] <= 0)
		return;

	// L0
	// Push is meant to get rid of blockers...not to push enemy players..
	// Most that use drop reload script, push players when they (drop)reload
	// which is F annoying in a crowded server..
	// If enemy is blocking you way, KILL HIM that's the F point.
	if (ent->client->sess.sessionTeam != target->client->sess.sessionTeam)
		return;

	shoveAmount = 512 * .8;
	VectorMA(target->client->ps.velocity, shoveAmount, forward, target->client->ps.velocity);

	APRS(target, "sound/multiplayer/vo_revive.wav");

	ent->lastPushTime = level.time;
}

/*
=================
Drop objective

Port from NQ
=================
*/
void Cmd_DropObj(gentity_t *self)
{
	gitem_t *item = NULL;

	// drop flag regardless
	if (self->client->ps.powerups[PW_REDFLAG]) {
		item = BG_FindItem("Red Flag");
		if (!item)
			item = BG_FindItem("Objective");

		self->client->ps.powerups[PW_REDFLAG] = 0;
	}
	if (self->client->ps.powerups[PW_BLUEFLAG]) {
		item = BG_FindItem("Blue Flag");
		if (!item)
			item = BG_FindItem("Objective");

		self->client->ps.powerups[PW_BLUEFLAG] = 0;
	}

	if (item) {
		vec3_t launchvel = { 0, 0, 0 };
		vec3_t forward;
		vec3_t origin;
		vec3_t angles;
		gentity_t *flag;

		VectorCopy(self->client->ps.origin, origin);
		// tjw: if the player hasn't died, then assume he's
		//      throwing objective per g_dropObj
		if (self->health > 0) {
			VectorCopy(self->client->ps.viewangles, angles);
			if (angles[PITCH] > 0)
				angles[PITCH] = 0;
			AngleVectors(angles, forward, NULL, NULL);
			VectorMA(self->client->ps.velocity,
				96, forward, launchvel);
			VectorMA(origin, 36, forward, origin);
			origin[2] += self->client->ps.viewheight;
		}

		flag = LaunchItem(item, origin, launchvel, self->s.number);

		flag->s.modelindex2 = self->s.otherEntityNum2;// JPW NERVE FIXME set player->otherentitynum2 with old modelindex2 from flag and restore here
		flag->message = self->message;	// DHM - Nerve :: also restore item name
		// Clear out player's temp copies
		self->s.otherEntityNum2 = 0;
		self->message = NULL;
		self->droppedObj = qtrue;
	}
	else
	{
		Cmd_throwKnives(self);
	}
}


