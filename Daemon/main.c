#include "main.h"

struct session_data {
	int fd;
};

struct lws_protocols protocols[] = {		// first protocol must always be HTTP handler
	{
		"websockets",	// name
		&ws_callback,	// callback
		sizeof(struct session_data),		// per session data size
		0,		// max frame size / rx buffer
	},
	{
		NULL, NULL, 0, 0	// end of list
	}
};

int main(int argc, char **argv) {

	FILE *fp= NULL;
	pid_t process_id = 0;
	pid_t sid = 0;

	char element[100][10][512], element2[100][19][512], element3[100][10][512];
	int timeLimit = 600;		// 10 minutes
	int numRows = 0, numRows2 = 0, numRows3 = 0, numClients = 0, numClients2 = 0;
	int count = 1;

	clock_t end, start = clock();
	double total;

	struct sigaction act;		// register the signal SIGINT handler
	act.sa_handler = INT_HANDLER;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	sigaction( SIGINT, &act, 0);

	struct lws_context *context = NULL;
	struct lws_context_creation_info info;
	struct lws *wsi = NULL;

	memset(&info, 0, sizeof info);		// parameters used to create context
	info.port = CONTEXT_PORT_NO_LISTEN;
	info.iface = NULL;
	info.protocols = protocols;
	info.ssl_cert_filepath = NULL;	// ssl support
	info.ssl_private_key_filepath = NULL;	// ssl support
	info.extensions = NULL;
	info.gid = -1;
	info.uid = -1;
	info.options = 0;

	// lws_daemonize("/var/lock/procd_.lock");
	
	context = lws_create_context(&info);	// create a websocket handler

	struct lws_client_connect_info ccinfo = {0};		// Client connection parameters
	ccinfo.context = context;
	ccinfo.address = "192.168.2.213";
	ccinfo.port = 8000;
	ccinfo.path = "/";
	ccinfo.host = lws_canonical_hostname( context );
	ccinfo.origin = "origin";
	ccinfo.protocol = protocols[0].name;
	wsi = lws_client_connect_via_info(&ccinfo);		// create a client connection

	process_id = fork();		// create child process

	if (process_id < 0) {		// indication of fork() failure
		printf("fork failed!\n");
		exit(1);
	}

	if (process_id > 0) {		// parent process, need to kill it to stop the daemon
		printf("process_id of child process %d \n", process_id);
		exit(0);
	}

	umask(0);		// unmask the file mode

	sid = setsid();		// set new session
	if(sid < 0) {
		exit(1);
	}

	chdir("/root");	// change the current working directory to root.

	close(STDIN_FILENO);		// close stdin stdout and stderr
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	fp = fopen ("Log.txt", "w+");		// open a log file in write mode.

	if (context == NULL) {
		fprintf(fp, "[Main] context is NULL.\n");
	}
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

		system("wrtbwmon update /tmp/usage.db");		// update db file
		
		// Implement and call some function that does core work for this daemon
		fprintf(fp, "Tudo bem !\n\n");
		
		lws_service(context, 100);	// execute the event loop once, and wait for a maximum of 100 ms

		addRowsAndCols();
		numRows = getUsageDataFromTxt(fp, element);
		numRows2 = getCatchPageData(fp, element2);

		if (count > 0) {
			void addRowsAndCols2();
			count--;
		}

		numClients = countNumClients(fp, element, element2, numRows, numRows2);

		end = clock();
		total = (double)(end-start) / CLOCKS_PER_SEC * 100;		// total in sec

		if(total >= 10) {	// every ~10s
			numClients2 = getClientUsage(fp, element, element2, element3, numRows, numRows2, numRows3, numClients, numClients2, wsi);
			addRowsAndCols2();
			addWalletIDandQuota(fp, element, element2, element3, numRows, numRows2, numRows3);
			numRows3 = getUsageDataFromTxt2(fp, element3);
			start = clock();
		}
		timeOut(fp, element, numRows, timeLimit);
	}

	fprintf(fp, "Exiting\n");
	//lws_context_destroy(context);		// destroy the context object
	fclose(fp);
}
