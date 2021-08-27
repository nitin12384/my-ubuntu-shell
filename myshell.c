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
	int n_args;
}command;

typedef struct input_struct{
	command* cmds; // array of commands
	int n_cmds;
	int is_parallel; 	// 0 if no parallel commands
	int is_seq;		// 0 if no seq commands
	char* out_redir;	// NULL if no output redirection
	int is_valid;
} input;

// --------------------------------------------GLOBAL VARIABLES

char* cur_working_directory;
int dm; // debug mode
const int max_words = 20;
const int max_cmds = 10;
const int max_args = 10;
const int max_word_len = 20;

// -------------------------------------------- HELPER FUNCTIONS

// for debugging
void print_command(command* cmd){
	int i; // loop variable
	
	printf("Name : %s, n_args : %d \n", cmd->args[0], cmd->n_args) ;
	printf("Arguements : ") ;
	for(i=0;  i < cmd->n_args; i++){
		printf("\'%s\' ",cmd->args[j]) ;
	}
	printf("\n");
}

void print_input(input* inp){
	int i;  // loop variable
	printf("n_cmds : %d, is_parellel : %d, is_seq : %d, is_valid : %d\n", 
		  	inp->n_cmds, inp->is_parallel, inp->is_seq, inp->is_valid) ;
	
	if(inp->out_redir != NULL){
		printf("Redirection to \'%s\' \n", inp->out_redir) ;
	}
	
	for(i=0; i < inp->n_cmds; i++){
		print_command( &(inp->cmds[i]) );
	}
}

void init_input(input* inp){
	inp->n_cmds = 0;
	inp->cmds = (command*) malloc(max_cmds*sizeof(command)); // array
	inp->is_parallel = 0;
	inp->is_seq = 0 ;
	inp->is_valid = 1;
	inp->our_redir = NULL ;
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
	// This function will parse the input string into multiple commands or a 
	// single command with arguments depending on the delimiter (&&, ##, >, or spaces).
	
	// local variables
	int n_words;
	char* words[max_words] ;
	int i; // loop variable
	int cmd_idx; // index of current command
	int arg_idx; // index of current arguement
	int make_new_command; // whether to make new command or to add current word as arguements to prev command
	
	char* word; // current word 
	
	// divide inp_line into words with strsep acc. to ' '
	
	if(dm) printf("Started parsing ...\n");
	
	n_words = 0;
	while( (word=strsep(&inp_line, max_word_len, ' ')) != NULL ){
		if(n_words >= max_words){
			// error
			// too many words
			// ????
		}
		words[n_words] = word;
		n_words++;
	}
	
	if(dm) printf("n_words : %d\n", n_words);
	
	input* inp = (input*) malloc( sizeof(input)) ;
	init_input(inp);
	
	// n_words should be >0 (empty command has been handled earlier in main() )
	if(n_words == 0){
		inp->is_vallid = 0;
		if(dm) printf("Invalid Input .. No words found\n");
		return inp;
	}
	
	// now check all words for "&&" and "##" and ">"
	
	make_new_command=1 ;
	for(i=0; i<n_words; i++)
	{
		if(make_new_command){
			if(dm) printf("Generating a new command.\n");
			// generate a new command
			cmd_idx = inp->n_cmds;
			inp->n_cmds ++;
			
			// allocating space for array of char* 'args' for current command
			inp->cmds[cmd_idx].args = (char**) malloc(max_args*sizeof(char*)) ;
			
			// add command name to arguement list
			inp->cmds[cmd_idx].args[0] = strdup(words[i]) ;
			inp->cmds[cmd_idx].n_args = 1 ;
			
			// further words will be added to arguement list, not for making new command
			make_new_command = 0;
			
		}
		else{
			// check for command seperators
			if(strcmp(words[i], "&&")==0){
				inp->is_parallel = 1;
				// after this there should be a new command
				make_new_command = 1;
				
			}
			else if(strcmp(words[i], "##")==0){
				inp->is_seq = 1;
				// after this there should be a new command
				make_new_command = 1;
			}
			else if(strcmp(words[i], ">")==0){
				// redirection
				
				// i+1th token should be last token
				if(i != n_words-2){
					if(dm) printf("Invalid Input .. wrong position of \'>\' \n");
					inp->is_valid = 0;
					return inp;
				}
				
				// set out_redir of inp
				inp->out_redir = strdup(words[i+1]) ;
				break;
				
			}
			else{
				// current word is not a command seperator
				// add this as arguements into preveious command
				cmd_idx = inp->n_cmds-1;
				arg_idx = inp->cmds[cmd_idx].n_args ;
				inp->cmds[cmd_idx].n_args++;
				inp->cmds[cmd_idx].args[arg_idx] = strdup(words[i]);
				
			}
			
			
		}
		
	}
	
	// check validity
	if(make_new_command ||  				// a new command was expected, but was not found
	   inp->is_parallel && inp->is_seq 			// both parallel and sequential instructions
	  ){
		
		inp->is_valid = 0;
		return inp;
	}
	
	
	// valid input, finally parsed fully
	return inp ;
	
}

void executeCommand(input* inp)
{
	// checks
	assert(inp != NULL);
	assert(inp->n_cmds==1) ;
	
	// This function will fork a new process to execute a command
	
	// special case for command name = "cd" 
	if(strcmp(inp->cmds[0].args[0], "cd") == 0)
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
		bytes_read = getline(&inp_line, &max_inp_len, stdin) ;
		
		if(bytes_read > max_inp_len){
			// ummm ??????	
		}
		
		// print debug info
		if(dm) printf("Bytes Read : %d, inp_line : \'%s\' \n", bytes_read, inp_line) ;
		
		// Parse input with 'strsep()' for different symbols (&&, ##, >) and for spaces.
		input* inp = parseInput(inp_line);
		
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
		  	strcmp(inp->cmds[0].args[0], "exit") == 0)
		{
			// if user types any other arguements after exit, it will not work
			printf("Exiting shell...");
			break;
		}
		
		if(inp->is_parallel)
			executeParallelCommands();		// This function is invoked when user wants to run multiple commands in parallel (commands separated by &&)
		else if(inp->is_seq)
			executeSequentialCommands();	// This function is invoked when user wants to run multiple commands sequentially (commands separated by ##)
		else if(inp->out_redir != NULL)
			executeCommandRedirection();	// This function is invoked when user wants redirect output of a single command to and output file specificed by user
		else
			executeCommand(inp);		// This function is invoked when user wants to run a single commands
				
	}
	
	return 0;
}
