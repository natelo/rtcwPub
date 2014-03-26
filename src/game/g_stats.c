/*/
===========================================================================
L0 / rtcwPub :: g_stats.c

	All the eye candy is placed here.

Created: 25. Mar / 2014
===========================================================================
*/
#include "g_local.h"

static qboolean firstheadshot;
static qboolean firstblood;

/**** Map Stats & RMS (Round (Warmup) Match Stats) ****/
char *stats_chars[] = {
	"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k",
	"l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v",
	"w", "x", "y", "z", "A", "B", "C", "D", "E", "F", "G",
	"H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R",
	"S", "T", "U", "V", "W", "X", "Y", "Z", "1", "2", "3",
	"4", "5", "6", "7", "8", "9", "0", "!", "@", "#", "$",
	"%", "^", "&", "*", "(", ")", "[", "]", "|", "'", ";",
	":", ",", ".", "?", "/", ">", "<", "-", " ", "+", "=",
	"-", "_", "~"
};

/*
===========
Double+ kills
===========
*/
void stats_DoubleKill(gentity_t *ent, int meansOfDeath) {
	char *random;
	int n = rand() % 3;

	if (!g_doubleKills.integer) {
		return;
	}
	if (g_gamestate.integer == GS_WARMUP_COUNTDOWN) {
		return;
	}
	if (!ent || !ent->client) {
		return;
	}

	// White list approach makes more sense.
	if (meansOfDeath == MOD_LUGER				// handgun
		|| meansOfDeath == MOD_COLT				// handgun
		|| meansOfDeath == MOD_KNIFE_STEALTH	// knife -- can be done :)
		|| meansOfDeath == MOD_THROWKNIFE		// Kill & a knive throw etc
		|| meansOfDeath == MOD_THOMPSON			// Thompson
		|| meansOfDeath == MOD_MP40				// MP40
		|| meansOfDeath == MOD_STEN				// STEN 
		|| meansOfDeath == MOD_POISONDMED		// Poison & kill etc
		) {

		if ((level.time - ent->client->lastKillTime) > 1000) {
			ent->client->doublekill = 0;
			ent->client->lastKillTime = level.time;
			return;
		}
		else {
			ent->client->doublekill++;
			ent->client->lastKillTime = level.time;
		}

		switch (ent->client->doublekill) {
			// 2 kills
			case 1:
				if (n == 0) random = "doublekill.wav";
				else if (n == 1) random = "doublekill2.wav";
				else if (n == 2) random = "doublekill3.wav";
				else random = "doublekill.wav";

				APS(va("rtcwpub/sound/game/sprees/doubleKills/%s", random));
				AP(va("chat \"^3Double Kill! ^7%s\n\"", ent->client->pers.netname));
			break;
			// 3 kills
			case 2:
				APS("rtcwpub/sound/game/sprees/doubleKills/tripplekill.wav");
				AP(va("chat \"^3Tripple Kill! ^7%s\n\"", ent->client->pers.netname));
			break;
			// 4 kills
			case 3:
				APS("rtcwpub/sound/game/sprees/doubleKills/oneandonly.wav");
				AP(va("chat \"^3Pure Ownage! ^7%s\n\"", ent->client->pers.netname));
			break;
		}
	}
}

/*
===========
First headshots

Prints who done first headshots when round starts.
===========
*/
void stats_FirstHeadshot(gentity_t *attacker, gentity_t *targ) {
	qboolean 	onSameTeam = OnSameTeam(targ, attacker);

	if (g_showFirstHeadshot.integer && g_gamestate.integer == GS_PLAYING) {

		if (!firstheadshot &&
			targ &&
			targ->client &&
			attacker &&
			attacker->client &&
			attacker->s.number != ENTITYNUM_NONE &&
			attacker->s.number != ENTITYNUM_WORLD &&
			attacker != targ &&
			g_gamestate.integer == GS_PLAYING &&
			!onSameTeam)
		{
			AP(va("chat \"%s ^7blew out %s^7's brains with the ^3FIRST HEAD SHOT^7!\"", attacker->client->pers.netname, targ->client->pers.netname));
			APS("rtcwpub/sound/game/events/headshot.wav");
			firstheadshot = qtrue;
		}
	}
}

/*
===========
First blood

Prints who draw first blood when round starts.
NOTE: Atm it's only a print..once I'm not lazy I'll set it in a way it can decide winner once timelimit hits on
specific maps (like depot, destuction) - so first blood decides who won.
===========
*/
void stats_FirstBlood(gentity_t *self, gentity_t *attacker) {
	qboolean 	onSameTeam = OnSameTeam(self, attacker);

	if (g_showFirstBlood.integer && g_gamestate.integer == GS_PLAYING) {

		if (!firstblood &&
			self &&
			self->client &&
			attacker &&
			attacker->client &&
			attacker->s.number != ENTITYNUM_NONE &&
			attacker->s.number != ENTITYNUM_WORLD &&
			attacker != self &&
			g_gamestate.integer == GS_PLAYING &&
			!onSameTeam)
		{
			AP(va("chat \"%s ^7drew ^1FIRST BLOOD ^7from ^7%s^1!\"", attacker->client->pers.netname, self->client->pers.netname));
			APS("rtcwpub/sound/game/events/firstblood.wav");
			firstblood = qtrue;
		}
	}
}

/*
===========
Last Blood

Prints in console at the end of the round
===========
*/
void stats_LastBloodMessage(void)
{
	if (g_showLastBlood.integer)
	{
		if (Q_stricmp(level.lastKiller, ""))
		{
			if (Q_stricmp(level.lastVictim, ""))
				AP(va("print \"%s ^7drew ^1Last Blood^7 from %s^7!\n\"", level.lastKiller, level.lastVictim));
			else
				AP(va("print \"%s ^7drew the ^1Last Blood^7!\n\"", level.lastKiller));
		}
	}
}

/*
===========
Killing sprees
===========
*/
void stats_KillingSprees(gentity_t *ent, int score) {
	int killRatio = ent->client->pers.kills;
	int snd_idx;

	if (!g_killingSprees.integer || g_gamestate.integer != GS_PLAYING)
		return;

	// if killer ratio is bellow 100 kills spam every 5th kill
	if (killRatio <= 100 && killRatio >= 5 && (killRatio % 5) == 0) 	{
		snd_idx = (killRatio / 5) - 1;

		AP(va("chat \"^4%s ^4(^7%dK %dhs^4): ^7%s\n\"",
			killingSprees[snd_idx <= 20 ? snd_idx : 19].msg, killRatio, ent->client->pers.headshots, ent->client->pers.netname));

		APS(va("rtcwpub/sound/game/sprees/Sprees/%s", killingSprees[snd_idx < 20 ? snd_idx : 19].snd));
	}
	// Anything above 100 gets spammed each 10th time..
	else if (killRatio > 100 && killRatio >= 10 && (killRatio % 10) == 0) {
		snd_idx = (killRatio / 10) - 1;

		AP(va("chat \"^4HOLY SHIT ^4(^7%dK %dhs^4): ^7%s\n\"", killRatio, ent->client->pers.headshots, ent->client->pers.netname));
		APS("rtcwpub/sound/game/sprees/Sprees/holyshit_alt.wav");
	}

	// could be done some other way but anyway...do the count... :)
	ent->client->ps.persistant[PERS_SCORE] += score;
	if (g_gametype.integer >= GT_TEAM)
		level.teamScores[ent->client->ps.persistant[PERS_TEAM]] += score;

	CalculateRanks();
}

/*
===========
Death spree
===========
*/
void stats_DeathSpree(gentity_t *ent) {
	int deaths = ent->client->pers.spreeDeaths;
	int n = rand() % 2;
	char *snd = "", *spree = "";

	if (!g_deathSprees.integer || deaths <= 0 || g_gamestate.integer != GS_PLAYING)
		return;

	if (deaths == 9) {
		if (n == 0) { spree = va("(^710 Dth^0)"); snd = "dSpree1.wav"; }
		else { spree = va("(^710 Dth^0)"); snd = "dSpree1_alt.wav"; }
	}
	else if (deaths == 14) {
		if (n == 0) { spree = va("(^715 Dth^0)"); snd = "dSpree2.wav"; }
		else { spree = va("(^715 Dth^0)"); snd = "dSpree2_alt.wav"; }
	}
	else if (deaths == 19) {
		if (n == 0) { spree = va("(^720 Dth^0)"); snd = "dSpree3.wav"; }
		else { spree = va("(^720 Dth^0)"); snd = "dSpree3_alt.wav"; }
	}
	else if (deaths == 24) {
		if (n == 0) { spree = va("(^725 Dth^0)"); snd = "dSpree4.wav"; }
		else { spree = va("(^725 Dth^0)"); snd = "dSpree4_alt.wav"; }
	}

	// Has to be offset by 1 as count comes late..
	if (deaths == 9 || deaths == 14 || deaths == 19 || deaths == 24) {
		AP(va("chat \"^0DEATHSPREE! %s: ^7%s\n\"", spree, ent->client->pers.netname));

		APS(va("rtcwpub/sound/game/sprees/deathSpree/%s", snd));
	}
}

/*
===========
Killer spree

Almost identical to Killing sprees, just uses different colors and female sounds.
===========
*/
void stats_KillerSpree(gentity_t *ent, int score) {
	int killRatio = ent->client->pers.lifeKills;
	int snd_idx;

	if (!g_killerSpree.integer || g_gamestate.integer != GS_PLAYING)
		return;

	if (!ent || !ent->client)
		return;

	// if killer ratio is bellow 50 kills spam every 5th kill
	if (killRatio <= 50 && killRatio >= 5 && (killRatio % 5) == 0) 	{
		snd_idx = (killRatio / 5) - 1;

		AP(va("chat \"^2%s ^2(^7%dk^2): ^7%s\n\"",
			killerSprees[snd_idx <= 11 ? snd_idx : 10].msg, killRatio, ent->client->pers.netname));

		APS(va("rtcwpub/sound/game/sprees/killerSprees/%s", killerSprees[snd_idx < 11 ? snd_idx : 10].snd));
	}

	// Anything above 50 gets spammed each 10th time..
	else if (killRatio > 50 && killRatio >= 10 && (killRatio % 10) == 0) {
		snd_idx = (killRatio / 10) - 1;

		AP(va("chat \"^2%s ^2(^7%dk^2): ^7%s\n\"",
			killerSprees[snd_idx <= 11 ? snd_idx : 10].msg, killRatio, ent->client->pers.netname));

		APS(va("rtcwpub/sound/game/sprees/killerSprees/%s", killerSprees[snd_idx < 11 ? snd_idx : 10].snd));
	}

	// could be done some other way but anyway...do the count... :)
	ent->client->ps.persistant[PERS_SCORE] += score;
	if (g_gametype.integer >= GT_TEAM)
		level.teamScores[ent->client->ps.persistant[PERS_TEAM]] += score;

	CalculateRanks();
}

/*
===========
EOM (End of Match) stats
===========
*/
void stats_MatchInfo(void) {
	int i, j, cnt, eff;
	float tot_acc = 0.00f;
	int tot_kills, tot_deaths, tot_gp, tot_hs, tot_sui, tot_tk, tot_dg, tot_dr, tot_td, tot_hits, tot_shots;
	gclient_t *cl;
	char *ref;
	char n1[MAX_NETNAME];
	char n2[MAX_NETNAME];

	AP(va("print \"\nMod: %s \n^7Server: %s  \n^7Time: ^3%s\n\n\"",
		GAMEVERSION, sv_hostname.string, getTime()));

	cnt = 0;
	for (i = TEAM_RED; i <= TEAM_BLUE; i++) {
		if (!TeamCount(-1, i)) {
			continue;
		}

		tot_kills = 0;
		tot_deaths = 0;
		tot_hs = 0;
		tot_sui = 0;
		tot_tk = 0;
		tot_dg = 0;
		tot_dr = 0;
		tot_td = 0;
		tot_gp = 0;
		tot_hits = 0;
		tot_shots = 0;
		tot_acc = 0;

		AP(va("print \"%s ^7Team\n"
			"^7-----------------------------------------------------------------------"
			"\nPlayer          ^3Kll ^7Dth Sui TK ^5Eff Accrcy   ^2HS    DG    DR    TD  ^7Score\n"
			"^7-----------------------------------------------------------------------\n\"", (i == TEAM_RED) ? "^1Axis" : "^4Allied"));

		for (j = 0; j < level.numPlayingClients; j++) {
			cl = level.clients + level.sortedClients[j];

			if (cl->pers.connected != CON_CONNECTED || cl->sess.sessionTeam != i) {
				continue;
			}

			// Bug fix - ^Pentagram always manages to break stats so it needs different approach. ^^
			DecolorString(cl->pers.netname, n1);
			SanitizeString(n1, n2, qfalse);
			Q_CleanStr(n2);
			n2[15] = 0;

			ref = "^7";
			tot_kills += cl->pers.kills;
			tot_deaths += cl->pers.deaths;
			tot_sui += cl->pers.suicides;
			tot_tk += cl->pers.teamKills;
			tot_hs += cl->pers.headshots;
			tot_dg += cl->pers.dmgGiven;
			tot_dr += cl->pers.dmgReceived;
			tot_td += cl->pers.dmgTeam;
			tot_gp += cl->ps.persistant[PERS_SCORE];
			tot_hits += cl->pers.acc_hits;
			tot_shots += cl->pers.acc_shots;

			eff = (cl->pers.deaths + cl->pers.kills == 0) ? 0 : 100 * cl->pers.kills / (cl->pers.deaths + cl->pers.kills);
			if (eff < 0) {
				eff = 0;
			}

			cnt++;
			AP(va("print \"%s%-15s^3%4d^7%4d%4d%3d%s^5%4d %6.2f^2%5d%6d%6d%6d^7%7d\n\"",
				ref,
				n2,
				cl->pers.kills,
				cl->pers.deaths,
				cl->pers.suicides,
				cl->pers.teamKills,
				ref,
				eff,
				((cl->pers.acc_shots == 0) ? 0.00 : ((float)cl->pers.acc_hits / (float)cl->pers.acc_shots) * 100.00f),
				cl->pers.headshots,
				cl->pers.dmgGiven,
				cl->pers.dmgReceived,
				cl->pers.dmgTeam,
				cl->ps.persistant[PERS_SCORE]));
		}

		eff = (tot_kills + tot_deaths == 0) ? 0 : 100 * tot_kills / (tot_kills + tot_deaths);
		if (eff < 0) {
			eff = 0;
		}
		tot_acc = ((tot_shots == 0) ? 0.00 : ((float)tot_hits / (float)tot_shots) * 100.00f);

		AP(va("print \"^7-----------------------------------------------------------------------\n"
			"%-19s^3%4d^7%4d%4d%3d^5%4d %6.2f^2%5d%6d%6d%6d^7%7d\n\n\"",
			"^3Totals^7",
			tot_kills,
			tot_deaths,
			tot_sui,
			tot_tk,
			eff,
			tot_acc,
			tot_hs,
			tot_dg,
			tot_dr,
			tot_td,
			tot_gp));
	}
	AP(va("print \"%s\n\" 0", ((!cnt) ? "^3\nNo scores to report." : "")));
}

/*
===========
Map Stats

This will be bunch of bouncing code around but should be simple enough (hopefully).
===========
*/
// Builds Message String
char *mapStatsMsg(char *map, unsigned int score, char *player, char *date) {
	if (g_mapStats.integer == 1)
		return va("^1HALL OF FAME(^7%s^1) ^7>> Top Killer(^1%d^7): %s ^7/ Achieved: ^1%s^7\n\"",
		map, score, player, ((date == NULL) ? "previous round" : date));
	else if (g_mapStats.integer == 2)
		return va("^1HALL OF FAME(^7%s^1) ^7>> Top Killing Spree(^1%d^7): %s ^7/ Achieved: ^1%s^7\n\"",
		map, score, player, ((date == NULL) ? "previous round" : date));
	else if (g_mapStats.integer == 3)
		return va("^1HALL OF SHAME(^7%s^1) ^7>> Most Deaths(^1%d^7): %s ^7/ Achieved: ^1%s^7\n\"",
		map, score, player, ((date == NULL) ? "previous round" : date));
	else if (g_mapStats.integer == 4)
		return va("^1HALL OF SHAME(^7%s^1) ^7>> Highest Dead Spree(^1%d^7): %s ^7/ Achieved: ^1%s^7\n\"",
		map, score, player, ((date == NULL) ? "previous round" : date));
	else if (g_mapStats.integer == 5)
		return va("^1HALL OF FAME(^7%s^1) ^7>> Most Revives(^1%d^7): %s ^7/ Achieved: ^1%s^7\n\"",
		map, score, player, ((date == NULL) ? "previous round" : date));
	else if (g_mapStats.integer == 6)
		return va("^1HALL OF FAME(^7%s^1) ^7>> Most Headshots(^1%^7): %s ^7/ Achieved: ^1%s^7\n\"",
		map, score, player, ((date == NULL) ? "previous round" : date));
	else
		return NULL;
}

// Write stats (unconditional)
void add_MapStats(char *file) {
	fileHandle_t statsFile;
	char *entry;

	entry = va("%d\\%s\\%s\\null", level.topScore, parseNames(level.topOwner),getTime());

	// Re-create new file (overwrites old one)
	trap_FS_FOpenFile(file, &statsFile, FS_WRITE);
	trap_FS_Write(entry, strlen(entry), statsFile);
	trap_FS_FCloseFile(statsFile);
}

// Set cvar
void set_mapStats(char *map, unsigned int score, char *player, char *date) {
	if (!level.mapStatsPrinted)
		AP(va("chat \"%s\n\"", mapStatsMsg(map, score, player, date)));
}

// Check stats
void handle_MapStats(char *map, char *suffix) {
	fileHandle_t	statsFile;
	char	filename[255], data[512];
	int		file;
	char	*token[512] = { NULL };

	Com_sprintf(filename, 255, "%s%s_%s", STSPTH, map, suffix);
	file = trap_FS_FOpenFile(filename, &statsFile, FS_READ);
	
	// Check|Write
	if (file > 0)
	{
		// read the existing data
		trap_FS_Read(data, file, statsFile);
		trap_FS_FCloseFile(statsFile);

		// score\\name\\date
		Q_Tokenize(data, token, "\\");

		// New record!
		if (token[0] && atoi(token[0]) < level.topScore)
		{
			add_MapStats(filename);
			set_mapStats(map, level.topScore, level.topOwner, NULL);

			if (g_mapStatsNotify.integer)
				AP(va("print \"^1New Map Record! ^7Be it good or bad, %s ^7has the stuff Legends are made of^1!\n\"",
				level.topOwner));
		}
		else
		{
			set_mapStats(map, atoi(token[0]), token[1], token[2]);
		}
	}
	else
	{	
		// Close the pointer..
		trap_FS_FCloseFile(statsFile);

		// Create it if there's an entry for it..
		if (level.topScore && level.topOwner)
		{
			add_MapStats(filename);
			set_mapStats(map, level.topScore, level.topOwner, NULL);

			if (g_mapStatsNotify.integer)
				AP(va("print \"^1New Map Record! ^7Be it good or bad, %s ^7has the stuff Legends are made of^1!\n\"",
				level.topOwner));
		}
	}
}

// Writes stats during game
void write_MapStats(gentity_t *ent, unsigned int score, int type) {
	// FCFS method!
	if (g_mapStats.integer &&
		score > level.topScore &&
		g_gamestate.integer == GS_PLAYING &&
		type == g_mapStats.integer)
	{
		Q_strncpyz(level.topOwner, "", sizeof(level.topOwner));
		strcat(level.topOwner, ent->client->pers.netname);
		level.topScore = score;
	}
}

// Front end for mapStats handling..
void stats_MapStats(void) {
	char mapName[64];
	char *suffix = "killer";
	
	if (!g_mapStats.integer)
		return;

	trap_Cvar_VariableStringBuffer("mapname", mapName, sizeof(mapName));

	// NOTE: Add more if desired/needed and simply patch/add the storing call in appropriate places..
	switch (g_mapStats.integer) {
		// Top Killer (most overall kills)
		case 1:
			suffix = "killer";
		break;
		// Top Killer Spree (most kills in a row)
		case 2:
			suffix = "killingSpree";
		break;
		// Top Victim (most overall deaths)
		case 3:
			suffix = "victim";
		break;
		// Top Death Spree (most deaths in a row)
		case 4:
			suffix = "deathSpree";
		break;
		// Top Medic (most revives)
		case 5:
			suffix = "revives";
		break;
		// Top Headshoter
		case 6:
			suffix = "headshots";
		break;
	}

	handle_MapStats(mapName, suffix);
	level.mapStatsPrinted = qtrue;
}

/*
===========
Round Stats

Stats that are printed each round.
'L0 - NOTE to readers: Don't get scared..my look complex at 1st glance but it's really simple.
===========
*/
roundStruct roundStats[ROUND_LIMIT];

// Process stats during match
void write_RoundStats(char *player, unsigned int score, unsigned int stats) {

	if (!g_roundStats.integer)
		return;

	if (Q_FindToken(g_excludedRoundStats.string, va("%d", stats)))
		return;

	if (score && roundStats[stats].score <= score)
	{
		char *more = "";

		// Adds comma if not a single entry
		if (roundStats[stats].score == score && Q_stricmp(roundStats[stats].player, player) == 0)
			more = va("%s^7, ", roundStats[stats].player);

		player = va("%s%s", more, player);

		Q_strncpyz(roundStats[stats].player, player, sizeof(roundStats[stats].player));
		roundStats[stats].stats = stats;
		roundStats[stats].score = score;
	}
}

// Sort what's left before writting to file (efficency, killer ratio & accuracy)
void sort_RoundStats(void) {
	int i;

	for (i = 0; i < g_maxclients.integer; i++)
	{
		int shots = level.clients[i].pers.acc_shots;
		int hits = level.clients[i].pers.acc_hits;
		int kills = level.clients[i].pers.kills;
		int deaths = level.clients[i].pers.deaths;
		float kr = 0.00f;
		float eff = 0.00f;
		float acc = 0.00f;
		float topKr = 0.00f;
		float topEff = 0.00f;
		float topAcc = 0.00f;

		// Sort stuff
		eff = (deaths + kills == 0) ? 0 : 100 * kills / (deaths + kills);
		if (eff < 0) { eff = 0; }
		kr = (float)kills / (!deaths ? 1 : (float)deaths);
		if (shots > 49) acc = ((float)hits / (float)shots) * 100.0f;

		//
		// Store stuff
		//
		if (eff && eff > topEff)
		{
			if (!Q_FindToken(g_excludedRoundStats.string, va("%d", ROUND_EFF)))
			{
				roundStats[ROUND_EFF].stats = ROUND_EFF;
				Q_strncpyz(roundStats[ROUND_EFF].player, level.clients[i].pers.netname, sizeof(roundStats[ROUND_EFF].player));
				Q_strncpyz(roundStats[ROUND_EFF].out, va("%2.2f", eff), sizeof(roundStats[ROUND_EFF].out));
			}
			topEff = eff;
		}

		if (kr && kr > topKr)
		{
			if (!Q_FindToken(g_excludedRoundStats.string, va("%d", ROUND_KR)))
			{
				roundStats[ROUND_KR].stats = ROUND_KR;
				Q_strncpyz(roundStats[ROUND_KR].player, level.clients[i].pers.netname, sizeof(roundStats[ROUND_KR].player));
				Q_strncpyz(roundStats[ROUND_KR].out, va("%2.2f", kr), sizeof(roundStats[ROUND_KR].out));
			}
			topKr = kr;
		}

		if (acc && acc > topAcc)
		{
			if (!Q_FindToken(g_excludedRoundStats.string, va("%d", ROUND_ACC)))
			{
				roundStats[ROUND_ACC].stats = ROUND_ACC;
				Q_strncpyz(roundStats[ROUND_ACC].player, level.clients[i].pers.netname, sizeof(roundStats[ROUND_ACC].player));
				Q_strncpyz(roundStats[ROUND_ACC].out, va("%2.2f (%d/%d)", acc, hits, shots), sizeof(roundStats[ROUND_ACC].out));
			}
			topAcc = acc;
		}
	}

	return;
}

// Write (to file) stats
void add_RoundStats(void) {
	FILE	*stats;
	int i;

	sort_RoundStats();

	// Unload stats	
	stats = fopen("roundStats.txt", "w+");
	if (roundStats != NULL)
	for (i = 0; i < ROUND_LIMIT; i++)
	{
		char *sorted;

		if (roundStats[i].stats)
		{
			if (roundStats[i].stats == ROUND_ACC || roundStats[i].stats == ROUND_EFF || roundStats[i].stats == ROUND_KR)
				sorted = va("%d %s %s", roundStats[i].stats, roundStats[i].out, parseNames(roundStats[i].player));
			else
				sorted = va("%d %d %s", roundStats[i].stats, roundStats[i].score, parseNames(roundStats[i].player));
		}

		if (roundStats[i].stats/* && (roundStats[i].score > 0 || !Q_stricmp(roundStats[i].out, "0"))*/)
			fputs(va("%s\n", sorted), stats);
	}
	fclose(stats);
}

// Read stats
void read_RoundStats(void) {
	FILE	*statsFile;
	char	data[1024];
	char	*score;
	unsigned int	stats;
	char	players[1024];
	char	arg1[1024];
	char	arg2[1024];
	char	scores[1024];
	char	type[1024];
	char	alt[1024];

	statsFile = fopen("roundStats.txt", "a+");
	while (fgets(data, 1024, statsFile) != NULL)
	{
		ParseStr(data, type, arg1);
		stats = atoi(type);
		ParseStr(arg1, scores, players);
		score = scores;

		// Offset: Have to account for hits/shots
		if (stats == ROUND_ACC)
		{
			ParseStr(arg1, scores, arg2);
			ParseStr(arg2, players, alt);

			score = va("%s %s", scores, players);
			Q_strncpyz(players, alt, sizeof(players));
		}

		roundStats[stats].stats = stats;
		Q_strncpyz(roundStats[stats].out, score, sizeof(roundStats[stats].out));
		Q_strncpyz(roundStats[stats].player, players, sizeof(roundStats[stats].player));
	}
	fclose(statsFile);
}

// Compute warmup time
void sortWarmupTime(int start, int inBetween)
{
	if (roundStats != NULL)
	{
		unsigned int i;
		unsigned int k = 0;
		unsigned int v = 0;
		unsigned int countDown = 7100; // To match countdown() in g_main.c
		unsigned int gracetime = 3000; // Just to make sure..

		for (i = 0; i < ROUND_LIMIT; i++)
		if (roundStats[i].stats != 0)
			k++;

		v = ((countDown + start + gracetime) + (k * inBetween)) / 1000;

		// Sorts warmup to match stats..
		trap_Cvar_Set("g_warmup", va("%d", ((v > 20) ? v : 20)));
	}
}

// Front end
void stats_RoundStats(void) {
	int statsStartup = 4000;
	int statsTime = 3600;

	if (level.statsNum == 0)
	{
		read_RoundStats();
		sortWarmupTime(statsStartup, statsTime);
	}
	else if (level.statsNum == 1)
	{

		AP(va("cp \"^2%s\n\"2", rSM[0].reward));
		APS(va("rtcwpub/sound/scenaric/achievers/%s", rSM[0].snd));
		level.statsStarted = qtrue;
	}
	else if (level.statsNum < ROUND_LIMIT)
	{
		int i;
		for (i = 0; i < ROUND_LIMIT; i++)
		{
			if (i >= level.statsNum)
			{
				if (roundStats[level.statsNum].stats)
				{
					char *score;
					score = va("%s%s", roundStats[level.statsNum].out, rSM[level.statsNum - 1].label);

					AP(va("cp \"^2%s: ^7%s\n^7%s \n\"2",
						rSM[level.statsNum - 1].reward, score, roundStats[level.statsNum].player));
					APS(va("rtcwpub/sound/scenaric/achievers/%s",
						rSM[level.statsNum - 1].snd));
				}
				else
				{
					level.statsNum++;
				}
			}
		}
	}

	level.statsPrint = level.time + (!level.statsStarted ? statsStartup : statsTime);

	// Push it forward
	level.statsNum++;
}


