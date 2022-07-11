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

#define MAXX 2048

// functions form functions.c
void addRowsAndCols();
void addRowsAndCols2();
int getUsageDataFromTxt2(FILE *fp, char element3[100][10][512]);
int getUsageDataFromTxt(FILE *fp, char element[100][10][512]);
void timeOut(FILE *fp, char element[100][10][512], int numRows, int timeLimit);
int getCatchPageData(FILE *fp, char element2[100][19][512]);
int getClientUsage(FILE *fp, char element[100][10][512], char element2[100][19][512], char element3[100][10][512], int numRows, int numRows2, int numRows3, int numClients, int numClients2, struct lws *wsi_in);
void addWalletIDandQuota(FILE *fp, char element[100][10][512], char element2[100][19][512], char element3[100][10][512], int numRows, int numRows2, int numRows3);
int countNumClients(FILE *fp, char element[100][10][512], char element2[100][19][512], int numRows, int numRows2);

// functions from client.c
void INT_HANDLER(int signo);
int websocket_write_back(struct lws *wsi_in, char *str, int str_size_in);
int ws_callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len);
void *pthread_routine(void *tool_in);

