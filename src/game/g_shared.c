/*/
===========================================================================
L0 / rtcwPub :: g_shared.c

	Game logic shared functionality

Created: 24. Mar / 2014
===========================================================================
*/
#include "g_local.h"

/*
==================
Ported from et: NQ
DecolorString

Remove color characters
==================
*/
void DecolorString(char *in, char *out)
{
	while (*in) {
		if (*in == 27 || *in == '^') {
			in++;		// skip color code
			if (*in) in++;
			continue;
		}
		*out++ = *in++;
	}
	*out = 0;
}

/*
==================
Time

Returns current time.
==================
*/
extern int trap_RealTime(qtime_t * qtime);
const char *months[12] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
// Returns current time % date
char *getTime( void ) {
	qtime_t		ct;
	trap_RealTime(&ct);

	return va("%02d:%02d:%02d/%02d %s %d",
		ct.tm_hour, ct.tm_min, ct.tm_sec, ct.tm_mday,
		months[ct.tm_mon], 1900 + ct.tm_year);
}

// Returns current date
char *getDate( void ) {
	qtime_t		ct;
	trap_RealTime(&ct);

	return va("%02d/%s/%d",	ct.tm_mday,	months[ct.tm_mon], 1900 + ct.tm_year);
}

/*
===========
S4ndmod - Get IP
===========
*/
void GetIP(const char *strIP1, char *strIP2, char *strPort) {

	int i = 0, j = 0;
	int foundcolon = 0;

	while (strIP1[i] != 0)
	{
		if (!foundcolon)
		{
			if (strIP1[i] == ':') {
				foundcolon = 1;
				strIP2[i] = 0;
			}
			else
				strIP2[i] = strIP1[i];
			i++;
		}
		else
		{
			strPort[j++] = strIP1[i++];
		}
	}
	if (!foundcolon)
		strIP2[i] = 0;
	strPort[j] = 0;
}

/*
=========
ValidIP

Works with regular IPv4 (without port) and IPv4 with subnet
=========
*/
qboolean isValidIP(const char *ip) {
	unsigned int b1, b2, b3, b4, subnet = 8;
	int rc;

	rc = sscanf(ip, "%3u.%3u.%3u.%3u/%2u", &b1, &b2, &b3, &b4, &subnet);
	if (rc < 4 || rc > 5)
		return qfalse;
	if ((b1 | b2 | b3 | b4) > 255 || (subnet > 32 || subnet < 8) || (subnet % 8 != 0))
		return qfalse;
	if (strspn(ip, "0123456789./") < strlen(ip))
		return qfalse;
	return qtrue;
}

/*
===========
Global sound
===========
*/
void APSound(char *sound) {
	gentity_t *ent;
	gentity_t *te;

	ent = g_entities;

	te = G_TempEntity(ent->s.pos.trBase, EV_GLOBAL_SOUND);
	te->s.eventParm = G_SoundIndex(sound);
	te->r.svFlags |= SVF_BROADCAST;
}

/*
===========
Client sound
===========
*/
void CPSound(gentity_t *ent, char *sound) {
	gentity_t *te;

	te = G_TempEntity(ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND);
	te->s.eventParm = G_SoundIndex(sound);
	te->s.teamNum = ent->s.clientNum;
}

/*
===========
Global sound with limited range
===========
*/
void APRSound(gentity_t *ent, char *sound) {
	gentity_t   *te;

	te = G_TempEntity(ent->r.currentOrigin, EV_GENERAL_SOUND);
	te->s.eventParm = G_SoundIndex(sound);
}

/*
===========
Sorts names so they're file friendly.

Quite sure this bit is from S4NDMoD or at least derived from.
===========
*/
extern char *stats_chars[];
char *parseNames(char *name)
{
	int i = 0, j = 0, k = 0;
	char parse[MAX_NETNAME + 1];

	Q_strncpyz(parse, "", sizeof(parse));
	for (i = 0; i < MAX_NETNAME + 1; i++)
	{
		for (k = 0; k < 91; k++)
		{
			if (name[i] == stats_chars[k][0])
			{
				parse[j] = name[i];
				j++;
			}
		}
	}
	return va("%s", parse);
}

/*
==================
L0 - Splits string into tokens
==================
*/
void Q_Tokenize(char *str, char **splitstr, char *delim) {
	char *p;
	int i = 0;

	p = strtok(str, delim);
	while (p != NULL)
	{
		printf("%s", p);

		splitstr[i] = G_Alloc(strlen(p) + 1);

		if (splitstr[i])
			strcpy(splitstr[i], p);
		i++;

		p = strtok(NULL, delim);
	}
}

/*
==================
Parse Strings

From S4NDMoD
==================
*/
void ParseStr(const char *strInput, char *strCmd, char *strArgs)
{
	int i = 0, j = 0;
	int foundspace = 0;

	while (strInput[i] != 0) {
		if (!foundspace) {
			if (strInput[i] == ' ') {
				foundspace = 1;
				strCmd[i] = 0;
			}
			else
				strCmd[i] = strInput[i];
			i++;
		}
		else {
			strArgs[j++] = strInput[i++];
		}
	}
	if (!foundspace)
		strCmd[i] = 0;

	strArgs[j] = 0;
}

/*
==================
TokenList

See if there's a match
==================
*/
qboolean Q_FindToken(char *haystack, char *needle) {

	if (strlen(haystack) && strlen(needle)) {
		char *token;
		
		while (1)
		{
			token = COM_Parse(&haystack);
			if (!token || !token[0])
				break;

			if (!Q_stricmp(needle, token))
				return qtrue;
		}
	}
	return qfalse;
}

/*
===================
L0 - Str replacer

Ported from etPub
===================
*/
char *Q_StrReplace(char *haystack, char *needle, char *newp)
{
	static char final[MAX_STRING_CHARS] = { "" };
	char dest[MAX_STRING_CHARS] = { "" };
	char newStr[MAX_STRING_CHARS] = { "" };
	char *destp;
	int needle_len = 0;
	int new_len = 0;

	if (!*haystack) {
		return final;
	}
	if (!*needle) {
		Q_strncpyz(final, haystack, sizeof(final));
		return final;
	}
	if (*newp) {
		Q_strncpyz(newStr, newp, sizeof(newStr));
	}

	dest[0] = '\0';
	needle_len = strlen(needle);
	new_len = strlen(newStr);
	destp = &dest[0];
	while (*haystack) {
		if (!Q_stricmpn(haystack, needle, needle_len)) {
			Q_strcat(dest, sizeof(dest), newStr);
			haystack += needle_len;
			destp += new_len;
			continue;
		}
		if (MAX_STRING_CHARS > (strlen(dest) + 1)) {
			*destp = *haystack;
			*++destp = '\0';
		}
		haystack++;
	}
	// tjw: don't work with final return value in case haystack 
	//      was pointing at it.
	Q_strncpyz(final, dest, sizeof(final));

	return final;
}
