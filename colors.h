/* This file contains the constants for colors for a terminal that supports ANSI escape codes */

#define KNRM  "\x1B[0m"		// Normal
#define KRED  "\x1B[31m"	// Red
#define KGRN  "\x1B[32m"	// Green
#define KYEL  "\x1B[33m"	// Yellow
#define KBLU  "\x1B[34m"	// Blue
#define KMAG  "\x1B[35m"	// Magenta
#define KCYN  "\x1B[36m"	// Cyan
#define KWHT  "\x1B[37m"	// White

#define KRST  "\x1B[2J\x1B[1;1H" //  Clears the screen and resets the cursor