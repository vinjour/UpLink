#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#define MAX 2048

void addRawsAndCols();
int getUsageData(FILE *fp, char element[100][10][512]);
void timeOut(FILE *fp, char element[100][10][512], int numRows, int timeLimit);
int getCatchPageData(FILE *fp, char element2[100][19][512]);
void addWalletIDandQuota(FILE *fp, char element[100][10][512], char element2[100][19][512], int numRows, int numRows2);