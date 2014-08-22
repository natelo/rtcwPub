/*/
===========================================================================
L0 / rtcwPub :: g_files.c

	Deals with File handling

Created: 24. Mar / 2014
===========================================================================
*/
#include "g_admin.h"

char *TempBannedMessage;

/*
===========
Log Admin related stuff
===========
*/
void logEntry(char *filename, char *info) {
	fileHandle_t	f;
	char *varLine;

	strcat(info, "\r");
	trap_FS_FOpenFile(filename, &f, FS_APPEND);

	varLine = va("%s\n", info);

	trap_FS_Write(varLine, strlen(varLine), f);
	trap_FS_FCloseFile(f);
	return;
}

/*
===========
Banned (file check version)

Took this from old S4NDMoD source as it simply works (time saver).
TODO: Redo & Replace ban & tempban..
===========
*/
void banClient(char arg1[MAX_TOKEN_CHARS], char arg2[MAX_TOKEN_CHARS]) {
	FILE		*bannedfile;

	bannedfile = fopen("banned.txt", "a+");
	fputs(va("%s-%s|\n", arg1, arg2), bannedfile);
	fclose(bannedfile);
}

/*
=========
Banned
=========
*/
qboolean Banned(char * IP, char * password)
{
	char ip1[4];
	char ip2[4];
	char ip3[4];
	char ip4[4];
	char ipfrom[20];
	char ipfrom1[4];
	char ipfrom2[4];
	char ipfrom3[4];
	char ipfrom4[4];
	char ipto[20];
	char ipto1[4];
	char ipto2[4];
	char ipto3[4];
	char ipto4[4];

	char buffer[1024];
	FILE*banfile = fopen("banned.txt", "r");

	Q_strncpyz(ip1, "", sizeof(ip1));
	Q_strncpyz(ip2, "", sizeof(ip2));
	Q_strncpyz(ip3, "", sizeof(ip3));
	Q_strncpyz(ip4, "", sizeof(ip4));

	BreakIP(IP, ip1, ip2, ip3, ip4);

	if (banfile){
		while (fgets(buffer, 1024, banfile) != NULL)
		{
			Q_strncpyz(ipfrom, "", sizeof(ipfrom));
			Q_strncpyz(ipto, "", sizeof(ipto));
			Q_strncpyz(ipfrom1, "", sizeof(ipfrom1));
			Q_strncpyz(ipfrom2, "", sizeof(ipfrom2));
			Q_strncpyz(ipfrom3, "", sizeof(ipfrom3));
			Q_strncpyz(ipfrom4, "", sizeof(ipfrom4));
			Q_strncpyz(ipto1, "", sizeof(ipto1));
			Q_strncpyz(ipto2, "", sizeof(ipto2));
			Q_strncpyz(ipto3, "", sizeof(ipto3));
			Q_strncpyz(ipto4, "", sizeof(ipto4));

			BreakRange(buffer, ipfrom, ipto);

			BreakIP(ipfrom, ipfrom1, ipfrom2, ipfrom3, ipfrom4);

			BreakIP(ipto, ipto1, ipto2, ipto3, ipto4);

			if ((atoi(ip1) >= atoi(ipfrom1) && atoi(ip1) <= atoi(ipto1)) &&
				(atoi(ip2) >= atoi(ipfrom2) && atoi(ip2) <= atoi(ipto2)) &&
				(atoi(ip3) >= atoi(ipfrom3) && atoi(ip3) <= atoi(ipto3)) &&
				(atoi(ip4) >= atoi(ipfrom4) && atoi(ip4) <= atoi(ipto4))) {

				fclose(banfile);
				if (bypassing(password))
					return qfalse;
				else
					return qtrue;
			}
		}
		fclose(banfile);
	}
	return qfalse;
}

/*
===========
Tempbanned

Port from old S4NDMoD (time saver).
===========
*/
void tempbanClient(gentity_t *ent, const int minsbanned) {
	FILE	*tempbannedfile;
	time_t rawtime;
	time(&rawtime);

	tempbannedfile = fopen("tempbanned.txt", "a+");
	fputs(va("%s %i\n",	clientIP(ent, qtrue), (int)((rawtime - 1103760000) + (minsbanned * 60))), tempbannedfile);
	fclose(tempbannedfile);
}
void rewrite_tempbanned(void)
{
	FILE		*bannedfile;
	FILE		*tempfile;
	char		tempbannedips[64];

	bannedfile = fopen("tempbanned.txt", "w+");
	tempfile = fopen("tempbannedtemp.txt", "r+");

	while (fgets(tempbannedips, 64, tempfile) != NULL)
	{
		fputs(tempbannedips, bannedfile);
	}

	fclose(bannedfile);
	fclose(tempfile);
	remove("tempbannedtemp.txt");
}
void write_tempbannedtemp(void)
{
	FILE		*bannedfile;
	FILE		*tempfile;
	char		tempbaninfo[64];
	char		tempbannedip[39];
	int			tempbannedsec;
	time_t		rawtime;
	time(&rawtime);

	bannedfile = fopen("tempbanned.txt", "r+");

	if (bannedfile){
		tempfile = fopen("tempbannedtemp.txt", "w+");

		if (tempfile) {
			while (fgets(tempbaninfo, 64, bannedfile) != NULL)
			{
				sscanf(tempbaninfo, "%s %i", tempbannedip, &tempbannedsec);
				if ((tempbannedsec - (rawtime - 1103760000)) > 0)
					fputs(tempbaninfo, tempfile);
			}

			fclose(tempfile);
		}

		fclose(bannedfile);
		rewrite_tempbanned();
	}
}

void rewrite_banned(void){
	FILE	*bannedfile;
	FILE	*tempfile;
	char	tempbannedips[MAX_STRING_TOKENS];

	bannedfile = fopen("banned.txt", "w+");
	tempfile = fopen("bannedtemp.txt", "r+");

	while (fgets(tempbannedips, 20, tempfile) != NULL)
	{
		fputs(tempbannedips, bannedfile);
	}

	fclose(bannedfile);
	fclose(tempfile);

	remove("bannedtemp.txt");
}

qboolean write_bannedtemp(const char ip[40]){
	FILE	*bannedfile;
	FILE	*tempfile;
	char	bannedips[MAX_STRING_TOKENS];
	char	ipfrom[20];
	char	ipto[20];
	char	_ipfrom[20];
	char	_ipto[20];
	qboolean found = qfalse;

	BreakRange(ip, _ipfrom, _ipto);

	bannedfile = fopen("banned.txt", "r+");
	if (bannedfile){
		tempfile = fopen("bannedtemp.txt", "w+");
		while (fgets(bannedips, 1024, bannedfile) != NULL)
		{
			Q_strncpyz(ipfrom, "", sizeof(ipfrom));
			Q_strncpyz(ipto, "", sizeof(ipto));

			BreakRange(bannedips, ipfrom, ipto);

			if ((!Q_stricmp(ipfrom, ip) && !Q_stricmp(ipto, ip)) || (!Q_stricmp(ipfrom, _ipfrom) && !Q_stricmp(_ipto, ipto)))
				found = qtrue;
			else
				fputs(bannedips, tempfile);
		}
		fclose(bannedfile);
		fclose(tempfile);

		if (found){
			remove("banned.txt");
			rewrite_banned();
		}
		else
			remove("bannedtemp.txt");
	}
	return found;
}

// Clean tempbans
void clean_tempbans(void)
{
	write_tempbannedtemp();
}
// Check if client is tempbanned
qboolean TempBanned(char * Clientip) {

	FILE		*tempbannedfile;
	char		tempbannedip[39];
	int			tempbannedsec;
	int			hours;
	int			tens;
	int			mins;
	int			seconds;
	char		tempbannedinfo[1024];
	char		*sortTime;
	char		*theTime;
	time_t		rawtime;
	time(&rawtime);

	tempbannedfile = fopen("tempbanned.txt", "r");

	if (tempbannedfile){
		while (fgets(tempbannedinfo, 1024, tempbannedfile) != NULL){
			sscanf(tempbannedinfo, "%s %i", tempbannedip, &tempbannedsec);

			if (!Q_stricmp(Clientip, tempbannedip) && (tempbannedsec - (rawtime - 1103760000)) > 0) {
				fclose(tempbannedfile);
				seconds = (tempbannedsec - (rawtime - 1103760000));
				mins = seconds / 60;
				hours = mins / 60;
				seconds -= mins * 60;
				tens = seconds / 10;
				seconds -= tens * 10;
				mins -= hours * 60;

				// Convert to hours, mins etc..
				sortTime = (hours > 0) ? ((mins > 9) ? va("%i:", hours) : va("%i:0", hours)) : "";
				theTime = va("%s%i:%i%i", sortTime, mins, tens, seconds);

				TempBannedMessage = va("^7You Are ^3Temporarily Banned ^7for ^3%s", theTime);
				return qtrue;
			}
		}
		fclose(tempbannedfile);
	}
	return qfalse;
}


