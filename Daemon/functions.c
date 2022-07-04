#include "functions.h"

void addRawsAndCols() {

	char *file="/tmp/usage.db";
	FILE *in = fopen( file, "r" );

	FILE *out = fopen("/tmp/usage.txt", "w+"); // Open a new txt file for writing

	char *line = NULL;
	size_t len = 0UL;
	ssize_t result;
	char buffer[ 1024 ];
	
	for ( ; ; ) {
	
		 result = getline( &line, &len, in );
		if ( result == -1 ) {
			break;
		}

		char *p = strchr( line, '\n' );	// we are looking for the character \n
		if ( p != NULL ) {
			*p = '\0';		// we replace by \0
		}

		snprintf( buffer, sizeof( buffer), "%s%s\n", line, ",idwallet,quota" );	// copying each line of usage.db and adding ",idwallet,quota" at the end of each line
		fputs( buffer, out );		// copying the buffer in the new usage.txt file
	}

	fclose(in);
	fclose(out);
}


int getUsageData(FILE *fp, char element[100][10][512]) {
	
	char user[100][MAX];
	char line[MAX];	// line read from the file
	char *p, *pt;		// pointer to return fgets, pointer to return strchr
	int k, i=0, j=0;			// k row reading index, i number of rows, j number of cols

	char *file="/tmp/usage.txt";
	FILE *pFile = fopen( file, "r" );

	if (pFile == NULL) {
        	fprintf(fp, "Error opening \"%s\": %s.\n", file, strerror(errno));
        	exit(EXIT_FAILURE);
	}
	
	p = fgets(line, MAX, pFile);
	while (p != NULL) {
		pt = strchr(line, '\n');		// we are looking for the character \n
		if (pt != NULL) {
			*pt = '\0';			// we replace by \0
 		}

		strcpy(user[i], line);		// copy the row in the table 
		p = fgets(line, MAX, pFile);		// read the next line
		i++;		// we go to the next index for the table
	}

	fclose(pFile);

	for (k=1; k<i; k++) {		// read the rows stored in the tab variable
		char *token = strtok(user[k], ",");	// separate each element in a row delimited by a comma
		
		while (token != NULL) {
			for (j=0; j<10; j++) {
				strcpy(element[k][j], token);		// Put each element in a row in different columns
				token = strtok(NULL, ",");
			}
		}
	}
	return i;	// return number of rows
}


void timeOut(FILE *fp, char element[100][10][512], int numRows, int timeLimit) {

	struct tm mytm;
	time_t t, now=time(NULL);
	int i;

	for (i=1; i<numRows; i++) {
		strptime(element[i][7],"%d-%m-%Y_%H:%M:%S", &mytm);	// convert string datetime into timestamp
		t = mktime(&mytm);
		int diff = now-t;		// calculate the difference between actual time and last seen time
		
		if (diff >= timeLimit) {
			fprintf(fp, "Exceeded time limit of %d seconds !\n", diff);
			fprintf(fp, "User %s will be kicked out !\n", element[i][0]);
		}
	}
	fprintf(fp, "\n");
}


int getCatchPageData(FILE *fp, char element2[100][19][512]) {

	char user2[100][MAX];
	char line[MAX];	// line read from the file
	char *p, *pt;		// pointer to return fgets, pointer to return strchr
	int h, i=0, j=0, l, m, len;			// h row reading index, i number of rows, j number of cols

	char *file2="/tmp/ndslog/ndslog.log";
	FILE *logfile = fopen( file2, "r" );

	if (logfile == NULL) {
        	fprintf(fp, "Error opening \"%s\": %s.\n", file2, strerror(errno));
        	return 0;
	}
	else {
		p = fgets(line, MAX, logfile);
		while (p != NULL) {
			pt = strchr(line, '\n');		// we are looking for the character \n
			if (pt != NULL) {
				*pt = '\0';			// we replace by \0
	 		}
	
			strcpy(user2[i], line);		// copy the row in the table 
			p = fgets(line, MAX, logfile);		// read the next line
			i++;		// we go to the next index for the table
		}
	
		fclose(logfile);
	
		for (h=1; h<i; h++) {		// read the rows stored in the user variable
			len = strlen(user2[h]);
			for (l=0; l<len; l++) {
				if (user2[h][l] == ' ') {
					for (m=l; m<len; m++) {
						user2[h][m] = user2[h][m+1];
					}
					len--;
					l--;
				}
			}
	
			if (len > 250) {
				char *token = strtok(user2[h], ",");		// separate each element in a row delimited by a comma
			
				while (token != NULL) {
					for (j=0; j<19; j++) {
						strcpy(element2[h][j], token);		// put each element in a row in different columns
						token = strtok(NULL, ",");
					}
				}
			}
		}
		return i;	// return number of rows
	}
}


void addWalletIDandQuota(FILE *fp, char element[100][10][512], char element2[100][19][512], int numRows, int numRows2) {

	int i, j, h;
	
	for (h=1; h<numRows2; h++) {
		memmove(&element2[h][13][0], &element2[h][13][4], strlen(element2[h][13]));		// get only mac adress without "mac="
		memmove(&element2[h][2][0], &element2[h][2][6], strlen(element2[h][2]));			// get only quota without "quota="
		memmove(&element2[h][3][0], &element2[h][3][9], strlen(element2[h][3]));			// get only walletID without "walletID="
	}

	for (i=1; i<numRows; i++) {		// for every line in usage.txt
		for (j=1; j<numRows2; j++) {		// for every line in ndslog.log
			if (strcmp(element[i][0], element2[j][13]) == 0) {	// if same mac adress
				fprintf(fp, "User %s wants a quota of %s bytes\n", element2[j][3], element2[j][2]);	// print IDwallet and quota

				if ( (strcmp(element[i][3], element2[j][2]) >= 0) || (strcmp(element[i][4], element2[j][2]) >= 0) ) {
					fprintf(fp, "User %s has exceeded quota of %s bytes !\n", element2[j][3], element2[j][2]);	// Quota limit exceed
					fprintf(fp, "Download : %s - Upload : %s\n\n",  element[i][3], element[i][4]);
				}
			}
			
		}
	}
	fprintf(fp, "\n");
}
