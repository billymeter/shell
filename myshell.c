#include "myshell.h"

char currentPath[PATH_MAX]; // This will keep track of the current working directory
char helpPath[PATH_MAX] = {'\0'};	// keeps track of the path where the readme file is kept

int main(int argc, char **argv) {
	char defaultPath[PATH_MAX]; // This will remember the default directory. Needed for the 'help' command.
	getcwd(defaultPath, sizeof(defaultPath)); // sets the default path

	setHelpPath(argv[0]); // sets the path where the readme file is located

	if (argc < 2) { // if there are no arguments, start the interactive shell
		runShell();
	} else {	// otherwise the first argument is assumed to be a batch file. Ignore the rest of the arguments
		processBatchScript(argv[1]);
	} 
	return 0;
}

/*
 * runshell()
 * This is the function that starts the interactive shell. It's in an infinite while loop that
 * continually gets user input and processes it appropiately.
 */

void runShell(void) {
	while (1) {
		char cmdBuffer[BUFFER_SIZE] = {'\0'}; 	// a buffer to keep user input
		printPrompt();
		fgets(cmdBuffer, BUFFER_SIZE, stdin);	// get user input
		
		processCmd(cmdBuffer);					// process user input
	}
}

/*
 * processBatchScript(char *filename)
 * Opens filename and reads the file line by line. Each line is processed as if the user typed it
 * into the interactive shell.
 */
int processBatchScript(char *filename) {
	FILE *fp;
	fp = fopen(filename, "r");	// open the batch file

	char buf[BUFFER_SIZE];

	if(fp != NULL) {
		while (fgets(buf, BUFFER_SIZE, fp) != NULL) // read file line by line
			processCmd(buf);	// and process each line
	} else { 	// file couldn't be opened
		printf("Couldn't open file '%s'. Starting shell normally.\n", filename);
		runShell();	// start the interactive shell
	}

	fclose(fp); // close the file

	return 0;
}

/*
 * printPrompt()
 * This prints the prompt for the shell. It tells the user what the current working directory is.
 */
void printPrompt(void) {
	if(!getcwd(currentPath, sizeof(currentPath))) {			// set the current working directory in currentPath
		printf("Error: Unable to get the current path.\n");	// not able to get current working directory
		exit(1);
	}
    
    // print the annoyingly colored prompt
	printf (KYEL "(: " KRED "m" KGRN "y" KBLU "s" KYEL "h" KMAG "e" KCYN "l" \
            KWHT "l " KYEL ":)" KGRN " %s $ " KNRM, currentPath);
}

/*
 * processCmd(char *input)
 * This is the function that scans the user's input for commands. It first searches for any builtin
 * commands that have been typed in, and if none are found, it is assumed the user is trying to execute
 * an external command.
 */

void processCmd(char *input) {
	/*
	 * Scan the input for built in commands
	 */
    
	// change directory
	if (strncmp(input, "cd", 2) == 0) {
		changeDir(&input[3]);				// change the current working directory
	}
	
	// clear screen
	else if(strncmp(input, "clr", 3) == 0) {
		printf(KRST);						// clear the screen and move cursor to 1,1
	}
    
	// dir
	else if (strncmp(input, "dir", 3) == 0) {
		fileList(&input[4]);				// get directory listing
	}
    
	// echo
	else if (strncmp(input, "echo", 4) == 0) {
		printf("%s", &input[5]);			// the comment will come after 'echo', so just use the pointer starting there
	}
    
	// help
	else if (strncmp(input, "help", 4) == 0) {
		printHelp();						// print help screen
	}
    
	// pause
	else if (strncmp(input, "pause", 5) == 0) {
		pauseShell();						// pause the shell
	}
    
	//quit
	else if (strncmp(input, "quit", 4) == 0) {
		exit(0);							// quit the shell
	}
    
	// only enter was pressed
	else if (input[0] == '\n') {
		;	// do nothing
	}
    
	// any other command
	else {
		runCmd(input);						// run external command
	}
    
}

/*
 * runCmd(char *cmd)
 * This is the function that executes external programs. It parses through the users input and tokenizes
 * all of the words separated by DELIMS. If the '&' symbol is present then the process will be executed
 * in the background.
 */
void runCmd(char *cmd) {
	char *args[256] = {'\0'}; 	// maximum of 256 arguments supported
	int argCount = 0;			// keeps track of how many arguments there are
	int background = NO;		// background process?
	char *token = strtok(cmd, DELIMS);
    
	while (token != NULL) { 	// parse through the input string to get all tokens
		if (strncmp(token, "&", 1) == 0)	// search for the background token
			background = YES;
		else {
            args[argCount] = token;			// build an array of strings that contains all the arguments
            argCount++;						// keep count of how many arguments there are
        }
        
		token = strtok(NULL, DELIMS);		// next token please
	}
    
	pid_t pid;
	int ret = 0;
    
	pid = fork();	
	if (pid == 0) { 			// child process
		ret = execvp(args[0], args);	// execute the program requested

		// If execvp fails, then the following lines will execute
		if (argCount == 1) {
			printf("Error executing '%s'\n", args[0]);
		} else {
			printf("Error executing '%s' with arguments ", args[0]);

			int i;
			for (i=1; i<argCount; i++)		// loop through the array to print all the arguments
				printf("'%s' ", args[i]);

			printf("\n");
		}
		
		exit(0);		// exit so we don't have another copy of the shell running
	} else {			// parent process
		if (!background)
			wait(NULL);	// wait on the child process
	}
}

/*
 * changeDir(char *dir)
 * Changes the current working directory to dir. If dir is null, then the current working directory is
 * printed to stdout.
 */
void changeDir(char *d){
	if (d[0] == '\0') {					// if d is null, then the user didn't specify a directory to change to
		printf("%s\n", currentPath);	// so just print the currnet path
		return;
	}
    
	else {
		d[ strlen(d) - 1 ] = '\0'; 			// terminate user input with NULL
	}
    
	int ret = chdir(d);
	
	if (ret) {	// chdir returns 0 on success, so if it is not zero, there is an error
		printf("Error changing to directory '%s'\n", d);
	}
}

/*
 * fileList(char *d)
 * This is for the dir command. It prints a list of files in d to stdout.
 */
void fileList(char *d){
	DIR *dir;
	struct dirent *dirStruct;
    
	if (d[0] == '\0') {						// user did not specify a directory, so use the current one
		dir = opendir(".");
	} else {
		d[ strlen(d) - 1 ] = '\0'; 			// terminate user input with NULL
		dir = opendir(d);					// get listing of the specified directory
	}
    
	if (dir) {
		while ((dirStruct = readdir(dir)) != NULL) {	// loop through all the files in the directory
			printf("%s\n", dirStruct->d_name);
		}
		closedir(dir);
	}
}


/*
 * printHelp()
 * This executes 'more' on the readme file included with the shell. It is assumed that the readme
 * file will be in the same path as the myshell executable.
 */
void printHelp(void){
	pid_t pid;
	int ret = 0;
    
	pid = fork();	
	if (pid == 0) { 			// child process
		ret = execlp("more", "more", helpPath, NULL);	// execute the program requested		
		exit(0);		// exit so we don't have another copy of the shell running
	} else {			// parent process
			wait(NULL);
	}

}

/* pauseShell()
 * Pauses the shell until 'enter' is pressed.
 */
void pauseShell(void){
	printf("Shell paused. Press 'Enter' to continue...\n");
	char c = getchar();
    
	while (c != '\n')	// we want a new line to continue the shell
		c = getchar();
}



/*
 * setHelpPath(char *argv)
 * This function finds the absolute path of myshell and thus the absolute path of the file readme. This is
 * necessary for the 'help' built in command to the shell knows the path to the readme file in order to 
 * display it with 'more'.
 */
void setHelpPath(char *argv) {
	realpath(argv, helpPath);

	char *lastSlash = strrchr(helpPath, '/');
	*lastSlash = '\0';

	snprintf(helpPath, sizeof helpPath, "%s%s", helpPath, "/readme");
}

