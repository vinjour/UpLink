#include "main.h"


// create the daemon and return the logfile
FILE *daemonize() {

	FILE *fp= NULL;

	pid_t process_id = 0;
	pid_t sid = 0;

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

	chdir("/root");	// change the current working directory to root

	close(STDIN_FILENO);		// close stdin stdout and stderr
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	fp = fopen (LOGFILE, "a+");		// open a log file in write mode

	return fp;
}










