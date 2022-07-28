#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <math.h>
#include <libwebsockets.h>
#include <jansson.h>


#define IPADDRESS "192.168.2.213"		// ip address of the websocket server
#define PORT 8002						// communication port for websocket

#define LOGFILE "Log.txt"	// name of the logfile

#define MAXBUF 2048		// max size of a buffer
#define MAXROWS 100		// max number of rows
#define MAXSTR 512			// max size of a string

#define TIMELOOPWS 100	// execution time for the events in websocket client  (in milliseconds)
#define TIMEONCE 3			// time to execute single actions   (in seconds)
#define TIMEEVERY 5			// time to execute repetitive actions  (in seconds)
#define TIMELIMIT 14440		// time limit before an inactive user is removed (in seconds)

#define DBMAC 0			// mac address of usage.db
#define DBIP 1			// ip address of usage.db
#define DBIFACE 2		// interface used (wlan, eth0, ...) of usage.db
#define DBDWNLD 3		// total download of usage.db
#define DBUPLD 4		// total upload of usage.db
#define DBTOTAL 5		// total (download+upload) of usage.db
#define DBFS 6			// date and time of first seen user of usage.db
#define DBLS 7			// date and time of last seen user of usage.db
#define DBIDW 8			// id wallet of usage.db
#define DBQOTA 9		// quota of usage.db

#define NDSQOTA 2		// mac address of ndslog.log
#define NDSNAME 3		// username of ndslog.log
#define NDSEMAIL 4		// mail address of ndslog.log
#define NDSSTACO 5		// connection status of ndslog.log
#define NDSMAC 12		// mac address of ndslog.log
#define NDSIP 13			// ip address of ndslog.log

char msgRcv[MAXSTR];	// global char to receive messages from server

// functions from functions.c
void copyUsageDBtoUsagetxt();
void copyUsageDBtoUsage2txt();
int getDatasFromUsageTxt(FILE *, char tableUsageDB[MAXROWS][10][MAXSTR]);
int getDatasFromUsage2Txt(FILE *, char tableUsageDB2[MAXROWS][10][MAXSTR]);
int getDatasFromNDSlog(FILE *, char tableNDS[MAXROWS][18][MAXSTR]);
void getClearElementsFromTableNDS(FILE *, char tableNDS[MAXROWS][18][MAXSTR], int);
void sendTimeOut(FILE *, char tableUsageDB[MAXROWS][10][MAXSTR], char tableNDS[MAXROWS][18][MAXSTR], int, int, struct lws*);
int countNumClients(FILE *, char tableUsageDB[MAXROWS][10][MAXSTR], char tableNDS[MAXROWS][18][MAXSTR], int, int);
char *getMacAddressRouter(FILE *);
void sendRouterConnectToServer(FILE *, struct lws*, char *macAddRouter);
void isAlreadyClient(FILE *, char tableNDS[MAXROWS][18][MAXSTR], int);
int sendDatasToServer(FILE *, char tableUsageDB[MAXROWS][10][MAXSTR], char tableUsageDB2[MAXROWS][10][MAXSTR],
							 char tableNDS[MAXROWS][18][MAXSTR], int, int, int, int, int, struct lws*);
void sendQuotaExceeded(FILE *, char tableUsageDB[MAXROWS][10][MAXSTR], char tableNDS[MAXROWS][18][MAXSTR], int, int, struct lws*);


// functions from wsClient.c
struct lws_context *createContext(FILE *);
struct lws_client_connect_info createInfoForWSI(struct lws_context *context);
int writeToServer(struct lws*, char*, int);
int parseMsgAndValidateActions(FILE *);
int webSocketCallback(struct lws*, enum lws_callback_reasons, void*, void*, size_t);


// functions from daemonize.c
FILE *daemonize();
