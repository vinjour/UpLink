#include "main.h"


int main(int argc, char **argv) {

	char tableUsageDB[MAXROWS][10][MAXSTR], tableUsageDB2[MAXROWS][10][MAXSTR], tableNDS[MAXROWS][18][MAXSTR];
	int numRowsUsageDB = 0, numRowsUsageDB2 = 0, numRowsNDS = 0, numClientsUsageDB = 0, numClientsUsageDB2 = 0;
	char *macRouter;
	int count = 1;

	clock_t end, start = clock();
	double total;

	FILE *fp = daemonize();

	struct lws_context *context = createContext(fp);
	struct lws_client_connect_info ccinfo = createInfoForWSI(context);
	struct lws *wsi = lws_client_connect_via_info(&ccinfo);		// create a client connection

	fprintf(fp, "[Main] context created.\n");

	if (wsi == NULL) {
		fprintf(fp, "[Main] wsi create error.\n");
	}
	fprintf(fp, "[Main] wsi create success.\n\n");

	while (1) {
		sleep(1);		// let the process sleep for some time
		fflush(fp);
		
		while (!wsi) {
			wsi = lws_client_connect_via_info(&ccinfo);		// create a client connection
		}

		system("wrtbwmon update /tmp/usage.db");		// update usage.db file
		
		fprintf(fp, "\nTudo bem !\n");
		
		lws_service(context, TIMELOOPWS);	// execute the event loop once, and wait for a maximum of TIMELOOPWS ms

		// every second
		copyUsageDBtoUsagetxt();
		numRowsUsageDB = getDatasFromUsageTxt(fp, tableUsageDB);
		numRowsNDS = getDatasFromNDSlog(fp, tableNDS);
		//quotaExceeded(fp, tableUsageDB, tableNDS, numRowsUsageDB, numRowsNDS);
		//timeOut(fp, tableUsageDB, tableNDS, numRowsUsageDB, numRowsNDS);
		isAlreadyClient(fp, tableNDS, numRowsNDS);
		numClientsUsageDB = countNumClients(fp, tableUsageDB, tableNDS, numRowsUsageDB, numRowsNDS);

		end = clock();
		total = (double)(end-start) / CLOCKS_PER_SEC * 100;		// total in seconds

		// just once after (2*TIMEONCE) seconds
		if( (total >= TIMEONCE) && (count > 0) ) {
			copyUsageDBtoUsage2txt();
			macRouter = getMacAddressRouter(fp);
			routerConnectToServer(fp, wsi, macRouter);	
			count--;
		}

		// every (2*TIMEEVERY) seconds
		if(total >= TIMEEVERY) {
			numClientsUsageDB2 = sendDatasToServer(fp, tableUsageDB, tableUsageDB2, tableNDS,
															numRowsUsageDB, numRowsUsageDB2, numRowsNDS,
															numClientsUsageDB, numClientsUsageDB2, wsi);
			copyUsageDBtoUsage2txt();
			numRowsUsageDB2 = getDatasFromUsage2Txt(fp, tableUsageDB2);
			start = clock();	// restart the clock
		}
	}

	fprintf(fp, "Exiting\n");
	lws_context_destroy(context);		// destroy the context object
	fclose(fp);			// close the logfile
}
