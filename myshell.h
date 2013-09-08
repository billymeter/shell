#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#include "colors.h"

#define BUFFER_SIZE 1024 	// buffer size for user input
#define YES 1 				// sometimes it makes more sense to use YES or NO 
#define NO 0				// instead of 1 or 0

#define DELIMS " \t\n" 		// delimiters for the strtok() function


void runShell(void);
int processBatchScript(char *filename);

void printPrompt(void);

void processCmd(char *input);
void runCmd(char *cmd);

void changeDir(char *d);
void fileList(char *d);
void printHelp(void);
void pauseShell(void);
void setHelpPath(char *argv);