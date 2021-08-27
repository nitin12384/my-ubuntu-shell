/********************************************************************************************
Name - Nitin Kumar
Enrollement No. - BT19CSE071
*********************************************************************************************/

// -------------------------------------------INCLUDES

#include <stdio.h>
#include <string.h>
#include <stdlib.h>			// exit()
#include <unistd.h>			// fork(), getpid(), exec()
#include <sys/wait.h>		// wait()
#include <signal.h>			// signal()
#include <fcntl.h>			// close(), open()

// --------------------------------------------GLOBAL VARIABLES
struct global_st{
   char* cur_working_directory;
}global;

// -------------------------------------------- HELPER FUNCTIONS


// -------------------------------------------- MAJOR FUNCTIONS

parseInput()
{
	// This function will parse the input string into multiple commands or a single command with arguments depending on the delimiter (&&, ##, >, or spaces).
}

void executeCommand()
{
	// This function will fork a new process to execute a command
}

void executeParallelCommands()
{
	// This function will run multiple commands in parallel
}

void executeSequentialCommands()
{	
	// This function will run multiple commands in parallel
}

void executeCommandRedirection()
{
	// This function will run a single command with output redirected to an output file specificed by user
}

// -------------------------------------------- MAIN FUNCTION

int main()
{
	// Initial declarations
	
	while(1)	// This loop will keep your shell running until user exits.
	{
		// Print the prompt in format - currentWorkingDirectory$
		
		// accept input with 'getline()'

		// Parse input with 'strsep()' for different symbols (&&, ##, >) and for spaces.
		parseInput(); 		
		
		if(/*condition*/)	// When user uses exit command.
		{
			printf("Exiting shell...");
			break;
		}
		
		if(/*condition*/)
			executeParallelCommands();		// This function is invoked when user wants to run multiple commands in parallel (commands separated by &&)
		else if(/*condition*/)
			executeSequentialCommands();	// This function is invoked when user wants to run multiple commands sequentially (commands separated by ##)
		else if(/*condition*/)
			executeCommandRedirection();	// This function is invoked when user wants redirect output of a single command to and output file specificed by user
		else
			executeCommand();		// This function is invoked when user wants to run a single commands
				
	}
	
	return 0;
}
