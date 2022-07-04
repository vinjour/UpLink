#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include "functions.h"

int main(int argc, char **argv) {

	FILE *fp= NULL;
	pid_t process_id = 0;
	pid_t sid = 0;
	
	char element[100][10][512];
	char element2[100][19][512];
	int timeLimit = 600;		// 10 minutes
	int numRows;
	int numRows2;

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

	while (1) {
		sleep(2);		// let the process sleep for some time
		fflush(fp);

		system("wrtbwmon update /tmp/usage.db");		// update db file
		
		// Implement and call some function that does core work for this daemon
		fprintf(fp, "Tudo bem !\n\n");
		
		addRawsAndCols();
		getCatchPageData(fp, element2);
		numRows2 = getCatchPageData(fp, element2);
		getUsageData(fp, element);
		numRows = getUsageData(fp, element);
		addWalletIDandQuota(fp, element, element2, numRows, numRows2);
		timeOut(fp, element, numRows, timeLimit);
	}
	
	fclose(fp);
}
