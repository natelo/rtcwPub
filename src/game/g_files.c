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
Add IP to banned file
===========
*/
void banClient(char arg[MAX_TOKEN_CHARS]) {
	FILE		*bannedfile;

	bannedfile = fopen("banned.txt", "a+");
	fputs(va("%s\n", arg), bannedfile);
	fclose(bannedfile);
}

/*
===========
Banned (file check version)
===========
*/
qboolean Banned(char *ip, char *password) {	
	qboolean banned = qfalse;	
	FILE* banfile;

	banfile = fopen("banned.txt", "r");
	if (banfile) {
		char line[1024];
		unsigned int clientIP[4]; 
		sscanf(ip, "%3u.%3u.%3u.%3u", &clientIP[0], &clientIP[1], &clientIP[2], &clientIP[3]);

		while (fgets(line, 1024, banfile) != NULL) {
			unsigned int match[5];
			unsigned int subrange;

			sscanf(line, "%3u.%3u.%3u.%3u/%2u", &match[0], &match[1], &match[2], &match[3], &match[4]);
			subrange = match[4];

			// Some (really basic) sanity checks
			if (strlen(line) < 7 || !(match[0] > 0 || match[0] < 256))
				continue;

			// Check it now..only bothers with it, if first bit matches..
			if (clientIP[0] == match[0]) {

				// Full Range (32)
				if (clientIP[1] == match[1] && clientIP[2] == match[2] && clientIP[3] == match[3]) {
					banned = qtrue;
				}
				else {
					if (subrange &&
						(
							subrange == 24 ||
							subrange == 16 ||
							subrange == 8
						)
					) {
						// Traverse through it now
						if (subrange == 24 && clientIP[2] == match[2] && clientIP[1] == match[1] && clientIP[0] == match[0])
							banned = qtrue;
						else if (subrange == 16 && clientIP[1] == match[1] && clientIP[0] == match[0])
							banned = qtrue;
						else if (subrange == 8) // First bit already matched upon entry..
							banned = qtrue;

						// Else = no match..
					}
					// There's no subrange..so bail out 
				}
			}

			if (banned) {
				// Overwrite if needed..
				if (bypassing(password))
					banned = qfalse;

				break;
			}
		}
		fclose(banfile);
	}
	return banned;
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


