/*/
===========================================================================
L0 / rtcwPub :: g_stats.h

	Stats prototypes

Created: 25. Mar / 2014
===========================================================================
*/
#ifndef __STATS_H
#define __STATS_H

// Path where stats will be stored
#define STSPTH "./stats/"

/**** Killing sprees (consistent over round) ****/
typedef struct {
	char *msg;
	char *snd;
} killing_sprees_t;

static const killing_sprees_t killingSprees[] = {
	{ "MULTI KILL!", "multikill.wav" },
	{ "MEGA KILL!", "megakill.wav" },
	{ "RAMPAGE!", "rampage.wav" },
	{ "GUNSLINGER!", "gunslinger.wav" },
	{ "ULTRA KILL!", "ultrakill.wav" },
	{ "MANIAC!", "maniac.wav" },
	{ "SLAUGHTER!", "slaughter.wav" },
	{ "MASSACRE!", "massacre.wav" },
	{ "IMPRESSIVE!", "impressive.wav" },
	{ "DOMINATING!", "dominating.wav" },
	{ "BLOODBATH!", "bloodbath.wav" },
	{ "KILLING MACHINE!", "killingmachine.wav" },
	{ "MONSTER KILL!", "monsterkill.wav" },
	{ "LUDICROUS KILL!", "ludicrouskill.wav" },
	{ "UNSTOPPABLE!", "unstoppable.wav" },
	{ "UNREAL!", "unreal.wav" },
	{ "OUTSTANDING!", "outstanding.wav" },
	{ "WICKED SICK!", "wickedsick.wav" },
	{ "HOLY SHIT!", "holyshit.wav" },
	{ "BLAZE OF GLORY!!", "blazeofglory.wav" },
	{ NULL, NULL }
};

/**** Killer Sprees (resets when player dies) ****/
typedef struct {
	char *msg;
	char *snd;
} killer_sprees_t;

static const killer_sprees_t killerSprees[] = {
	{ "MULTI KILL!", "multikill.wav" },
	{ "KILLING SPREE!", "killingspree.wav" },
	{ "RAMPAGE!", "rampage.wav" },
	{ "ULTRA KILL!", "ultraKill.wav" },
	{ "MONSTER KILL!", "monsterkill.wav" },
	{ "LUDICROUS KILL!", "ludicrouskill.wav" },
	{ "DOMINATING!", "dominating.wav" },
	{ "GODLIKE!", "godlike.wav" },
	{ "UNSTOPPABLE!", "unstoppable.wav" },
	{ "WICKED SICK!", "wickedsick.wav" },
	{ "HOLY SHIT!!", "holyshit.wav" },
	{ NULL, NULL }
};

/**** Map Stats ****/
#define MAP_KILLER			1
#define MAP_KILLING_SPREE   2
#define MAP_VICTIM			3
#define MAP_DEATH_SPREE		4
#define MAP_REVIVES			5
#define MAP_HEADSHOTS		6

/**** RMS (Round Match Stats) ****/
#define ROUND_PRINT			1	// Should always be First
#define ROUND_KILLS			2
#define ROUND_DEATHS		3
#define ROUND_HEADSHOTS		4
#define ROUND_TEAMKILLS		5
#define ROUND_TEAMBLEED		6
#define ROUND_POISON		7
#define ROUND_REVIVES		8
#define ROUND_AMMOGIVEN		9
#define ROUND_MEDGIVEN		10
#define ROUND_GIBS			11
#define ROUND_SUICIDES		12
#define ROUND_KNIFETHROW	13
#define ROUND_FASTSTABS		14
#define ROUND_CHICKEN		15
#define ROUND_KILLPEAK		16
#define ROUND_DEATHPEAK		17
#define ROUND_ACC			18
#define ROUND_KR			19
#define ROUND_EFF			20
#define ROUND_LIMIT			21	// Should always be Last!

typedef struct {
	char *reward;
	char *snd;
} round_stats_t;

static const round_stats_t rSM[] = {	
	{ "Match Results", "intro.wav" },
	{ "Most Kills", "blazeofglory.wav" },
	{ "Most Deaths", "yousuck.wav" },
	{ "Most Headshots", "headhunter.wav" },
	{ "Most Team Kills", "teamkiller.wav" },
	{ "Most Team Bleeding", "teambleeder.wav" },
	{ "Most Poisons", "toxic.wav" },
	{ "Most Revives", "excellent.wav" },
	{ "Most Ammo Given", "ammo.wav" },
	{ "Most Med Given", "med.wav" },
	{ "Most Gibs", "ownage.wav" },
	{ "Most Suicides", "suicides.wav" },
	{ "Knife Juggler", "knife_juggler.wav" },
	{ "Stealth Kills", "assasin.wav" },
	{ "Coward", "chicken.wav" },
	{ "Highest Kill Spree", "killingmachine.wav" },
	{ "Highest Death Spree", "slaughter.wav" },
	{ "Highest Accuracy", "accuracy.wav" },
	{ "Highest Kill Ratio", "outstanding.wav" },
	{ "Most Efficient", "impressive.wav" },
	{ NULL, NULL }
};

struct round_stats_structure_s {
	unsigned int stats;				// Label (see defines above)
	unsigned int score;				// Most of stats
	char player[MAX_TOKEN_CHARS];	// Players that made the list
	char out[MAX_TOKEN_CHARS];		// For output
};

typedef struct round_stats_structure_s roundStruct;
extern roundStruct roundStats[];

#endif // __STATS_H
