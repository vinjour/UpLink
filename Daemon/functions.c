#include "main.h"


// Copy usage.db to usage.txt adding 2 columns : walletID and quota
void copyUsageDBtoUsagetxt() {

	char *file="/tmp/usage.db";
	FILE *in = fopen( file, "r" );

	FILE *out = fopen("/tmp/usage.txt", "w+"); // Open a new txt file for writing

	char *line = NULL;
	size_t len = 0UL;
	ssize_t result;
	char buffer[MAXBUF];
	
	while (1) {
	
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


// Copy usage.db to usage2.txt adding 2 columns : walletID and quota
void copyUsageDBtoUsage2txt() {

	char *file="/tmp/usage.db";
	FILE *in = fopen( file, "r" );

	FILE *out = fopen("/tmp/usage2.txt", "w+"); // Open a new txt file for writing

	char *line = NULL;
	size_t len = 0UL;
	ssize_t result;
	char buffer[MAXBUF];
	
	while (1) {
	
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


// Get elements from usage.txt, put it in a 3D array and return number of rows
int getDatasFromUsageTxt(FILE *fp, char tableUsageDB[MAXROWS][10][MAXSTR]) {
	
	char tab[MAXROWS][MAXBUF];
	char line[MAXBUF];	// line read from the file
	char *p, *pt;		// pointer to return fgets, pointer to return strchr
	int k, i=0, j=0;			// k row reading index, i number of rows, j number of cols

	char *file="/tmp/usage.txt";
	FILE *pFile = fopen( file, "r" );

	if (pFile == NULL) {
        	fprintf(fp, "Error opening \"%s\": %s.\n", file, strerror(errno));
        	exit(EXIT_FAILURE);
	}
	
	p = fgets(line, MAXBUF, pFile);
	while (p != NULL) {
		pt = strchr(line, '\n');		// we are looking for the character \n
		if (pt != NULL) {
			*pt = '\0';			// we replace by \0
 		}

		strcpy(tab[i], line);		// copy the row in the table 
		p = fgets(line, MAXBUF, pFile);		// read the next line
		i++;		// we go to the next index for the table
	}

	fclose(pFile);

	for (k=1; k<i; k++) {		// read the rows stored in tab
		char *element = strtok(tab[k], ",");	// separate each element in a row delimited by a comma
		
		while (element != NULL) {
			for (j=0; j<10; j++) {
				strcpy(tableUsageDB[k][j], element);		// Put each element in a row in different columns
				element = strtok(NULL, ",");
			}
		}
	}
	return i;	// return number of rows
}


// get elements from usage2.txt, put it in a 3D array and return number of rows
int getDatasFromUsage2Txt(FILE *fp, char tableUsageDB2[MAXROWS][10][MAXSTR]) {
	
	char tab[MAXROWS][MAXBUF];
	char line[MAXBUF];	// line read from the file
	char *p, *pt;		// pointer to return fgets, pointer to return strchr
	int k, i=0, j=0;			// k row reading index, i number of rows, j number of cols

	char *file="/tmp/usage2.txt";
	FILE *pFile = fopen( file, "r" );

	if (pFile == NULL) {
        	fprintf(fp, "Error opening \"%s\": %s.\n", file, strerror(errno));
        	exit(EXIT_FAILURE);
	}
	
	p = fgets(line, MAXBUF, pFile);
	while (p != NULL) {
		pt = strchr(line, '\n');		// we are looking for the character \n
		if (pt != NULL) {
			*pt = '\0';			// we replace by \0
 		}

		strcpy(tab[i], line);		// copy the row in the table 
		p = fgets(line, MAXBUF, pFile);		// read the next line
		i++;		// we go to the next index for the table
	}

	fclose(pFile);

	for (k=1; k<i; k++) {		// read the rows stored in tab
		char *element = strtok(tab[k], ",");	// separate each element in a row delimited by a comma
		
		while (element != NULL) {
			for (j=0; j<10; j++) {
				strcpy(tableUsageDB2[k][j], element);		// Put each element in a row in different columns
				element = strtok(NULL, ",");
			}
		}
	}
	return i;	// return number of rows
}


// get elements from ndslog.log, put it in a 3D array and return number of rows
int getDatasFromNDSlog(FILE *fp, char tableNDS[MAXROWS][18][MAXSTR]) {

	char tab[100][MAXBUF];
	char line[MAXBUF];	// line read from the file
	char *p, *pt;		// pointer to return fgets, pointer to return strchr
	int h, i=0, j=0, l, m, len;			// h row reading index, i number of rows, j number of cols

	char *file2="/tmp/ndslog/ndslog.log";
	FILE *logfile = fopen( file2, "r" );

	if (logfile == NULL) {
        	fprintf(fp, "Error opening \"%s\": %s.\n", file2, strerror(errno));
        	return 0;
	}

	p = fgets(line, MAXBUF, logfile);
	while (p != NULL) {
		pt = strchr(line, '\n');		// we are looking for the character \n
		if (pt != NULL) {
			*pt = '\0';			// we replace by \0
		}
	
		strcpy(tab[i], line);		// copy the row in the table 
		p = fgets(line, MAXBUF, logfile);		// read the next line
		i++;		// we go to the next index for the table
	}
	
	fclose(logfile);
	
	for (h=1; h<i; h++) {		// read the rows stored in tab
		len = strlen(tab[h]);
		for (l=0; l<len; l++) {
			if (tab[h][l] == ' ') {
				for (m=l; m<len; m++) {
					tab[h][m] = tab[h][m+1];
				}
				len--;
				l--;
			}
		}
	
		if (len > 250) {
			char *element = strtok(tab[h], ",");		// separate each element in a row delimited by a comma
			
			while (element != NULL) {
				for (j=0; j<18; j++) {
					strcpy(tableNDS[h][j], element);		// put each element in a row in different columns
					element = strtok(NULL, ",");
				}
			}
		}
	}
	return i;	// return number of rows
}


// clear the elements in TableNDS to use them later
void getClearElementsFromTableNDS(FILE *fp, char tableNDS[MAXROWS][18][MAXSTR], int numRowsNDS) {

	int i;

	for (i=1; i<numRowsNDS; i++) {
		memmove(&tableNDS[i][NDSMAC][0], &tableNDS[i][NDSMAC][4], strlen(tableNDS[i][NDSMAC]));			// get only mac adress without "mac="
		memmove(&tableNDS[i][NDSQOTA][0], &tableNDS[i][NDSQOTA][6], strlen(tableNDS[i][NDSQOTA]));		// get only quota without "quota="
		memmove(&tableNDS[i][NDSIP][0], &tableNDS[i][NDSIP][3], strlen(tableNDS[i][NDSIP]));					// get only ip adress without "ip="
		memmove(&tableNDS[i][NDSNAME][0], &tableNDS[i][NDSNAME][5], strlen(tableNDS[i][NDSNAME]));		// get only username without "user="
	}
}


// check if client is not connected since timeLimit and deletes it
void timeOut(FILE *fp, char tableUsageDB[MAXROWS][10][MAXSTR], char tableNDS[MAXROWS][18][MAXSTR], int numRowsUsageDB, int numRowsNDS, struct lws *wsi_in) {

	struct tm mytm;
	time_t t, now=time(NULL);
	int i = 0, j = 0, h = 0;

	for (i=1; i<numRowsUsageDB; i++) {		// for every line in usage.txt
		for (j=numRowsNDS; j>0; j--) {		// for every line in ndslog.log
			if ( (strcmp(tableUsageDB[i][DBMAC], tableNDS[j][NDSMAC]) == 0) && (strstr(tableNDS[j][NDSSTACO], "status=Client") != NULL) && (strcmp(tableUsageDB[i][DBIP], tableNDS[j][NDSIP]) == 0) ) {	// if same mac and ip address and client connected
				
				strptime(tableUsageDB[i][DBLS],"%d-%m-%Y_%H:%M:%S", &mytm);	// convert string datetime into timestamp
				t = mktime(&mytm);
				int diff = now-t;		// calculate the difference between actual time and last seen time
		
				if (diff >= TIMELIMIT) {

					char* sDelete = NULL;
					json_t *delete = json_object();

					json_object_set_new(delete, "action", json_string("delete"));
					json_object_set_new(delete, "name", json_string(tableNDS[j][NDSNAME]));
					json_object_set_new(delete, "mac", json_string(tableNDS[j][NDSMAC]));
				
					sDelete = json_dumps(delete, 0);
					lws_callback_on_writable(wsi_in);
					writeToServer(wsi_in, sDelete, -1);
					
					json_decref(delete);
					free(sDelete);

					for(h=0; h<19; h++) {
						tableNDS[j][h][0] = '\0';
					}
				}
			}
		}
	}
}


// return number of clients (registered to usage.txt and ndslog.log)
int countNumClients(FILE *fp, char tableUsageDB[MAXROWS][10][MAXSTR], char tableNDS[MAXROWS][18][MAXSTR], int numRowsUsageDB, int numRowsNDS) {

	int i = 0, j = 0, k = 0;

	for (i=1; i<numRowsUsageDB; i++) {		// for every line in usage.txt
		for (j=numRowsNDS; j>0; j--) {		// for every line in ndslog.log
			if ( (strcmp(tableUsageDB[i][DBMAC], tableNDS[j][NDSMAC]) == 0) && (strstr(tableNDS[j][NDSSTACO], "status=Client") != NULL) && (strcmp(tableUsageDB[i][DBIP], tableNDS[j][NDSIP]) == 0) ) {	// if same mac and ip address and client connected
				k++;
			}
		}
	}
	return k;	// return number of clients
}


// get and return the mac address of the router (eth0)
char *getMacAddressRouter(FILE *fp) {

	char tab[1][MAXSTR];
	char line[MAXSTR];	// line read from the file
	char *p, *pt;		// pointer to return fgets, pointer to return strchr
	int i=0;			// i number of rows

	char  *macAddressRouter;
	macAddressRouter = malloc(sizeof(char)*20);

	char *file="/sys/class/net/eth0/address";
	FILE *pFile = fopen( file, "r" );

	if (pFile == NULL) {
        	fprintf(fp, "Error opening \"%s\": %s.\n", file, strerror(errno));
        	exit(EXIT_FAILURE);
	}
	
	p = fgets(line, MAXBUF, pFile);
	while (p != NULL) {
		pt = strchr(line, '\n');		// we are looking for the character \n
		if (pt != NULL) {
			*pt = '\0';			// we replace by \0
 		}

		strcpy(tab[i], line);		// copy the row in the table 
		p = fgets(line, MAXBUF, pFile);		// read the next line
		i++;		// we go to the next index for the table
	}
	fclose(pFile);

	strcpy(macAddressRouter, tab[0]);
	return macAddressRouter;
}


// check if router is connected to server and sends validation to server
void routerConnectToServer(FILE *fp, struct lws *wsi_in, char *macRouter) {

	char *sRouter = NULL;
	json_t *router = json_object();

	json_object_set_new(router, "action", json_string("router"));
	json_object_set_new(router, "name", json_string("UpLink router"));
	json_object_set_new(router, "mac", json_string(macRouter));
	
	sRouter = json_dumps(router, 0);
	lws_callback_on_writable(wsi_in);
	writeToServer(wsi_in, sRouter, -1);
	
	json_decref(router);
	free(sRouter);
}


// check if a client already registered in ndslog.log, add quota of both and delete the 2nd connection
void isAlreadyClient(FILE *fp, char tableNDS[MAXROWS][18][MAXSTR], int numRowsNDS) {
	int i = 0, j = 0, match = 0;
 
	for (j=numRowsNDS; j>0; j--) {		// for every line in ndslog.log
		for (i=numRowsNDS; i>0; i--) {		// for every line in ndslog.log
	
			int newQuota = 0, h = 0;
			char buffer[10];

			if ( (strcmp(tableNDS[j][NDSNAME], tableNDS[i][NDSNAME]) == 0) && (strcmp(tableNDS[j][NDSMAC], tableNDS[i][NDSMAC]) == 0) ) {
				match++;
			}

			if (match == 2) {
				newQuota = atoi(tableNDS[i][NDSQOTA]) + atoi(tableNDS[j][NDSQOTA]);
				sprintf(buffer, "%d", newQuota);
				strcpy(tableNDS[i][NDSQOTA], buffer);
				
				for(h=0; h<19; h++) {
					tableNDS[j][h][0] = '\0';
				}
			}
		}
		match = 0;
	}
}

// return number of clients (registered to usage.txt and ndslog.log), send to server each new client's connection and each data transfer
int sendDatasToServer(FILE *fp, char tableUsageDB[MAXROWS][10][MAXSTR], char tableUsageDB2[MAXROWS][10][MAXSTR], 
							char tableNDS[MAXROWS][18][MAXSTR], int numRowsUsageDB, int numRowsUsageDB2, int numRowsNDS,
							 int numClientsUsageDB, int numClientsUsageDB2, struct lws *wsi_in) {

	int i = 0, j = 0, k = 0, diffClients = 0;
	unsigned long int diffDownload, diffUpload, sum;
	
	diffClients = numClientsUsageDB - numClientsUsageDB2;	// difference between actual clients and those from 15s ago

	for (j=numRowsNDS; j>0; j--) {		// for every line in ndslog.log
		for (i=1; i<numRowsUsageDB; i++) {		// for every line in usage.txt
			if ( (strcmp(tableUsageDB[i][DBMAC], tableNDS[j][NDSMAC]) == 0) && (strcmp(tableUsageDB[i][DBIP], tableNDS[j][NDSIP]) == 0) && (strstr(tableNDS[j][NDSSTACO], "status=Client") != NULL) ) {	// if same mac and ip address and client connected
				k++;
				if (diffClients > 0) {
					char* sConnect = NULL;
					json_t *connect = json_object();

					json_object_set_new(connect, "action", json_string("connect"));
					json_object_set_new(connect, "name", json_string(tableNDS[j][NDSNAME]));
					json_object_set_new(connect, "mac", json_string(tableNDS[j][NDSMAC]));
				
					sConnect = json_dumps(connect, 0);
					lws_callback_on_writable(wsi_in);
					writeToServer(wsi_in, sConnect, -1);
					
					json_decref(connect);
					free(sConnect);
					diffClients--;
				}
				
				if ( (numRowsUsageDB == numRowsUsageDB2) && (strcmp(tableUsageDB[i][DBMAC], tableUsageDB2[i][DBMAC]) == 0) ) {
					
					diffDownload = ((unsigned long)atoi(tableUsageDB[i][DBDWNLD])) - ((unsigned long)atoi(tableUsageDB2[i][DBDWNLD]));		// difference between actual total download and from 15s ago
					diffUpload = ((unsigned long)atoi(tableUsageDB[i][DBUPLD])) - ((unsigned long)atoi(tableUsageDB2[i][DBUPLD]));		// difference between actual total upload and from 15s ago
					sum = diffUpload + diffDownload;		// sum of download and upload bits used in 15 seconds

					if ( (diffDownload == (atoi(tableUsageDB[i][DBDWNLD]))) || (diffUpload == (atoi(tableUsageDB[i][DBUPLD]))) ) {		// specific for special bug event
						diffDownload = 0;
						diffUpload = 0;
					}

					if ( (diffDownload != 0) || (diffUpload != 0) ) {
						char* sTransfer = NULL;
						json_t *transfer = json_object();

						json_object_set_new(transfer, "action", json_string("transfer"));
						json_object_set_new(transfer, "nameOrigin", json_string(tableNDS[j][NDSNAME]));
						json_object_set_new(transfer, "nameDestination", json_string("UpLinkRouter"));
						json_object_set_new(transfer, "dataInBytes", json_integer(sum));

						sTransfer = json_dumps(transfer, 0);
						lws_callback_on_writable(wsi_in);
						writeToServer(wsi_in, sTransfer, -1);

						json_decref(transfer);
						free(sTransfer);
					}
				}
			}	
		}
	}
	return k;		// return number of clients
}


// check if client has exceeded his quota and put him in guest zone
void quotaExceeded(FILE *fp, char tableUsageDB[MAXROWS][10][MAXSTR], char tableNDS[MAXROWS][18][MAXSTR], int numRowsUsageDB, int numRowsNDS, struct lws *wsi_in) {

	int i = 0, j = 0, h = 0;
	unsigned long int quota = 0;

	for (i=1; i<numRowsUsageDB; i++) {		// for every line in usage.txt
		for (j=numRowsNDS; j>0; j--) {		// for every line in ndslog.log
			if ( (strcmp(tableUsageDB[i][DBMAC], tableNDS[j][NDSMAC]) == 0) && (strstr(tableNDS[j][NDSSTACO], "status=Client") != NULL) && (strcmp(tableUsageDB[i][DBIP], tableNDS[j][NDSIP]) == 0) ) {	// if same mac and ip address and client connected
				
				quota = atoi(tableNDS[j][NDSQOTA]) * 1000000; 	// get user's quota (MB) and convert it in bytes

				if ( (atoi(tableUsageDB[i][3]) >= quota) || (atoi(tableUsageDB[i][4]) >= quota) ) {
					
					char* sDelete = NULL;
					json_t *delete = json_object();

					json_object_set_new(delete, "action", json_string("delete"));
					json_object_set_new(delete, "name", json_string(tableNDS[j][NDSNAME]));
					json_object_set_new(delete, "mac", json_string(tableNDS[j][NDSMAC]));
				
					sDelete = json_dumps(delete, 0);
					lws_callback_on_writable(wsi_in);
					writeToServer(wsi_in, sDelete, -1);
					
					json_decref(delete);
					free(sDelete);

					for(h=0; h<19; h++) {
						tableNDS[j][h][0] = '\0';
					}
				}
			}
		}
	}
}
