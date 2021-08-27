/********************************************************************************************
Name - Nitin Kumar
Enrollement No. - BT19CSE071
*********************************************************************************************/

// -------------------------------------------INCLUDES

#include <stdio.h>			// printf(), getline()
#include <string.h>			// strcmp()
#include <stdlib.h>			// exit(), malloc()
#include <unistd.h>			// fork(), getpid(), exec()
#include <sys/wait.h>		// wait()
#include <signal.h>			// signal()
#include <fcntl.h>			// close(), open()
#include <assert.h>			// assert()
// -------------------------------------------- STRUCTURE DEFINITIONS
typedef struct command_struct{
	char** args;
	char* cmd_name;
	int n_args;
}command;

typedef struct input_struct{
	command* cmds; // array of commands
	int n_cmds;
	int n_parallel; // 0 if no parallel commands
	int n_seq;		// 0 if no seq commands
	char* out_redir // NULL if no output redirection
	int is_valid;
} input;

// --------------------------------------------GLOBAL VARIABLES
char* cur_working_directory;
int dm; // debug mode
// -------------------------------------------- HELPER FUNCTIONS

// for debugging
void print_command(command* cmd){
	printf("Name : %s, n_args : %d \n", cmd->cmd_name, cmd->n_args) ;
	printf("Arguements : ") ;
	for(j=0;  j < cmd->n_args; j++){
		printf("\'%s\' ",cmd->args[j]) ;
	}
	printf("\n");
}

void print_input(input* inp){
	printf("n_cmds : %d, n_parellel : %d, n_seq : %d, is_valid : %d\n", 
		  	inp->n_cmds, inp->n_parallel, inp->n_seq, inp->is_valid) ;
	
	if(inp->out_redir != NULL){
		printf("Redirection to \'%s\' \n", inp->out_redir) ;
	}
	
	for(i=0; i < inp->ncmds; i++){
		print_command( &(inp->cmds[i]) );
	}
}

// -------------------------------------------- MAJOR FUNCTIONS

void execute_cd(command* cmd){
	assert(cmd != NULL);
	// there should be two arguements "cd" and <directory>
	assert(cmd->n_args == 2);
	
	// check if directory exist
	
	// if not print wrong directory message ?? or shell incorrect command 
	
	
}

input* parseInput(char* inp_line)
{
	// This function will parse the input string into multiple commands or a single command with arguments depending on the delimiter (&&, ##, >, or spaces).
}

void executeCommand(input* inp)
{
	// checks
	assert(inp != NULL);
	assert(inp->n_cmds==1) ;
	
	// This function will fork a new process to execute a command
	// special case for cd 
	if(strcmp(inp->cmds[0].cmd_name, "cd") == 0)
	{
		execute_cd( &(inp->cmds[0]) );
		return;
	}
	
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
	// ---------------------- Initial declarations
	
	// Initialize globals
	cur_working_directory = "~" ;
	dm = 1;  // debug mode
	
	// Locals
	const int max_inp_len = 100 ;
	
	//Looping
	int i,j;
	
	
	char* inp_line = (char*) malloc( max_inp_len * sizeof(char)) ;
	int bytes_read;
	
	
	
	// This loop will keep your shell running until user exits.
	while(1)	
	{
		// Print the prompt in format - currentWorkingDirectory$
		// extra space after $
		printf("%s$ ", cur_working_directory) ;
		
		// accept input with 'getline()'
		bytes_read = getline(&inp_line, max_inp_len, stdin) ;
		
		if(dm) printf("Bytes Read : %d, inp_line : \'%s\' \n", bytes_read, inp_line) ;
		
		// Parse input with 'strsep()' for different symbols (&&, ##, >) and for spaces.
		input* inp = parseInput();
		
		if(dm){
			// print debug info
			printf("Input Parsed As : \n");
			print_input(inp) ;
		}
		
		// Check if not valid
		if(inp->is_valid == 0){
			// invalid command
			printf("Shell: Incorrect command\n");
			continue;
		}
		
		// Check for empty command
		if(inp->n_cmds == 0){
			printf("\n");
			continue;
		}
		
		// Check for exit command
		if(	inp->n_cmds == 1 && 
		   	inp->cmds[0].n_args == 1 &&
		  	strcmp(inp->cmds[0].cmd_name, "exit") == 0)
		{
			// if user types any ohter arguements after exit, it will not work
			printf("Exiting shell...");
			break;
		}
		
		if(inp->n_parallel > 0)
			executeParallelCommands();		// This function is invoked when user wants to run multiple commands in parallel (commands separated by &&)
		else if(inp->n_seq > 0)
			executeSequentialCommands();	// This function is invoked when user wants to run multiple commands sequentially (commands separated by ##)
		else if(inp->out_redir != NULL)
			executeCommandRedirection();	// This function is invoked when user wants redirect output of a single command to and output file specificed by user
		else
			executeCommand(inp);		// This function is invoked when user wants to run a single commands
				
	}
	
	return 0;
}
