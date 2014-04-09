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
void banClient(char arg[MAX_TOKEN_CHARS]) {
	FILE		*bannedfile;

	bannedfile = fopen("banned.txt", "a+");
	fputs(va("%s\n", arg), bannedfile);
	fclose(bannedfile);
}
// Check if client is banned
qboolean Banned(char * Clientip, char * password)
{
	FILE * bannedfile;
	char		ip1[10];
	char		ip2[10];
	char		ip3[10];
	char		ip4[10];
	char		banip1[10];
	char		banip2[10];
	char		banip3[10];
	char		banip4[10];
	char		bannedips[1024];
	Q_strncpyz(ip1, "", sizeof(ip1));
	Q_strncpyz(ip2, "", sizeof(ip2));
	Q_strncpyz(ip3, "", sizeof(ip3));
	Q_strncpyz(ip4, "", sizeof(ip4));

	BreakIP(va("%s\n", Clientip), ip1, ip2, ip3, ip4);
	bannedfile = fopen("banned.txt", "r");

	if (bannedfile){
		while (fgets(bannedips, 1024, bannedfile) != NULL){
			Q_strncpyz(banip1, "", sizeof(banip1));
			Q_strncpyz(banip2, "", sizeof(banip2));
			Q_strncpyz(banip3, "", sizeof(banip3));
			Q_strncpyz(banip4, "", sizeof(banip4));

			BreakIP(bannedips, banip1, banip2, banip3, banip4);

			if (!Q_stricmp(banip1, ip1) || !Q_stricmp(banip1, "*")){
				if (!Q_stricmp(banip2, ip2) || !Q_stricmp(banip2, "*")){
					if (!Q_stricmp(banip3, ip3) || !Q_stricmp(banip3, "*")){
						if (!Q_stricmp(banip4, ip4) || !Q_stricmp(banip4, "*\n")){

							fclose(bannedfile);
							if (bypassing(password))
								return qfalse;
							else
								return qtrue;
						}
					}
				}
			}
		}
		fclose(bannedfile);
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


