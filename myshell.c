/********************************************************************************************
Name - Nitin Kumar
Enrollement No. - BT19CSE071
*********************************************************************************************/

// -------------------------------------------INCLUDES

#include <stdio.h>			// printf(), getline()
#include <string.h>			// strcmp(), strsep()
#include <stdlib.h>			// exit(), malloc()
#include <unistd.h>			// fork(), getpid(), execvp(), dup(), dup2(), getcwd()
#include <sys/wait.h>			// wait()
#include <signal.h>			// 
#include <fcntl.h>			// close(), open()
#include <assert.h>			// assert()
#include <dirent.h>			// opendir(), chdir(), closedir()
#include <errno.h>			// errno

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
const int max_path_len = 100;

int cur_process_pid ;
int cur_process_killed;

// -------------------------------------------- HELPER FUNCTIONS

// for debugging
void print_command(command* cmd){
	int i; // loop variable
	
	printf("Name : %s, n_args : %d \n", cmd->args[0], cmd->n_args) ;
	printf("Arguements : ") ;
	for(i=0;  i < cmd->n_args; i++){
		printf("\'%s\' ",cmd->args[i]) ;
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
	inp->out_redir = NULL ;
}

// -------------------------------------------- SIGNAL HANDLING FUNCTIONS

void my_handler(int s)
{
      	if(dm) printf("Caught signal %d\n",s);
		
}

void register_handler(){
	struct sigaction sig_handler;
	sig_handler.sa_handler = my_handler ;
	sigemptyset(&sigIntHandler.sa_mask);
   	sigIntHandler.sa_flags = 0;

   	sigaction(SIGINT, &sigIntHandler, NULL);

}


// -------------------------------------------- MAJOR FUNCTIONS


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
	while( (word=strsep(&inp_line, " ")) != NULL ){
		if(n_words >= max_words){
			// error
			// too many words
			// ????
		}
		// if word is "", its not a word
		if(dm) printf("strsep() gave word : \'%s\' \n", word);
		if(word[0] != '\0' ){
			words[n_words] = word;
			n_words++;	
		}
		else{
			if(dm) printf("word was blank string, not accepted\n");
		}
	}
	
	if(dm) printf("n_words : %d\n", n_words);
	
	input* inp = (input*) malloc( sizeof(input)) ;
	init_input(inp);
	
	// n_words can be == 0 
	if(n_words == 0){
		// inp->n_cmds is already 0
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
	
	// for each command
	// make sure each array of arguements is terminated by NULL
	
	for(i=0; i < inp->n_cmds; i++){
		arg_idx = inp->cmds[i].n_args ;
		inp->cmds[i].args[ arg_idx ] = NULL ;
	}
	
	
	// valid input, finally parsed fully
	return inp ;
	
}


void execute_cd(command* cmd){
	assert(cmd != NULL);
	// there should be two arguements "cd" and <directory>
	assert(cmd->n_args == 2);
	char* new_path = cmd->args[1] ;
	
	if(dm) printf("cd called with path : \'%s\'\n", new_path) ;
	
	// check if directory exist
	// if not print wrong directory message ?? or shell incorrect command
	// else change the directory
	
	DIR* dir = opendir(new_path);
	if (dir) {
	    	/* Directory exists. */
		int ret_val = chdir(new_path) ;
		if(dm) printf("Directory Exist, ret_val of chdir : %d \n", ret_val) ;
		
		// free dir variable
		closedir(dir);
	} 
	else if (ENOENT == errno) {
	    /* Directory does not exist. */
		if(dm) printf("Directory does not exist");
		
		printf("cd error : \'%s\' - no such directory.\n", new_path);
	} 
	else {
	    	/* opendir() failed for some other reason. */
		if(dm) printf("opendir() failed but directory probably exist.\n");
		
	}
	
}

void executeCommand(input* inp, int out_redir)
{
	// local- variables
	int ret_val; // for fork()
	int ret_val2; // for execvp()
	int ret_val3; // for dup2()
	int stdout_save; // to save stdout if call to execvp fails
	
	// checks
	assert(inp != NULL);
	//assert(inp->n_cmds==1) ;
	
	// This function will fork a new process to execute a command
	if(dm) printf("executeCommand() called .\n");
	if(dm) print_command( &(inp->cmds[0]) );
	
	// execute first command in the cmds array 
	
	// special case for command name = "cd" 
	if(strcmp(inp->cmds[0].args[0], "cd") == 0)
	{
		execute_cd( &(inp->cmds[0]) );
		return;
	}
	
	// fork process
	ret_val = fork() ;
	
	if(ret_val == 0){
		// child
		if(dm) printf("In child Process Now...(PId : %d)\n", getpid());
		
		
		// if out_redir, then open other file
		if(out_redir){
			assert(inp->out_redir != NULL) ;
			
			stdout_save = dup(STDOUT_FILENO) ;
			if(stdout_save == -1){
				if(dm) printf("Error in duping stdout\n");
			}
			close(STDOUT_FILENO);
			open(inp->out_redir, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
		}
		
		//set current process PID
		cur_process_pid = getpid();
		cur_process_killed = 0;
		
		ret_val2 = execvp( inp->cmds[0].args[0], inp->cmds[0].args ) ;
		// if it returns means there was an error
		// incorrect command probably
		
		// re open stdout
		ret_val3 = dup2(stdout_save, STDOUT_FILENO);
		if(ret_val3 == -1){
			if(dm) printf("Error in reopening stdout.\n");
		}
		
		if(dm) printf("execvp returned in Child, ret_val2 : %d\n", ret_val2);
		
		printf("Shell: Incorrect command\n");
		
		// end the child process
		exit(0) ;
	}
	else if(ret_val > 0){
		// parent
		if(dm) printf("(Child PID : %d, Parent PID : %d)start of parent process (before wait)...\n", ret_val, getpid());
		wait(NULL);
		if(dm) printf("In end of parent process...\n");
	}
	else{
		if(dm) printf("Fork failed in executeCommand\n");
	}
	
} 

void executeParallelCommands(input *inp)
{
	// This function will run multiple commands in parallel
	
	if(dm) printf("execute Parallel function called for %d commands.\n", inp->n_cmds) ;
	
	// using fork create inp->n_cmds processes
	int cmd_idx=0; // command index for each child process
	
	int* pid_for_cmd = (int*) malloc(inp->n_cmds * sizeof(int)) ;
	// pid_for_cmd[i] is the pid of child process which will execute ith command.
	
	int i; // loop variable
	int ret_val ;
	int ret_val2;
	int ret_val3;
	
	// main fork
	ret_val = fork();
	
	
	if(ret_val==0){
		if(dm) printf("In child Process of main fork (PID : %d)\n", getpid()) ;
		pid_for_cmd[0] = getpid() ;
		cmd_idx = 1;
		
		while(cmd_idx < inp->n_cmds){
			ret_val2 = fork() ;
			if(ret_val2 == 0){
				if(dm) printf("Child process for command %d. (PID : %d)\n", cmd_idx, getpid());
				pid_for_cmd[cmd_idx] = getpid();
				
				cmd_idx++;
			}
			else if(ret_val2 > 0){
				// no more fork will be called in parent process
				// child process will call more forks itself
				
				if(dm) printf("Process for command %d. (PID : %d). No more fork now from this process.\n", cmd_idx-1, getpid()) ;
				break;
			}
			else{
				if(dm) printf("fork() failed\n");
				break;
			}
		}
		
		// All new process are created
		for(i=0; i<inp->n_cmds; i++){
			
			// process with pid == pid_for_cmd[i] will run ith command
			if(getpid() == pid_for_cmd[i]){
				// run ith command with execvp
				if(dm) printf("%dth command will run now (PID : %d)\n", i, getpid()) ;
				
				// exec. 
				ret_val3 = execvp( inp->cmds[i].args[0], inp->cmds[i].args ) ;
				
				// in case if it returned
				if(dm) printf("execvp returned in Child, ret_val3 : %d\n", ret_val3);
				// if it returns means there was an error
				// incorrect command probably
				printf("Shell: Incorrect command\n");
				
				// end the child process
				exit(0) ;
				
			}
		}
		
	}
	else if(ret_val>0){
		if(dm) printf("(Before wait)In parent Process of main fork (PID : %d, ret_val of fork() : %d)\n", getpid(), ret_val) ;
		wait(NULL) ;
		if(dm) printf("At the end of parent process. Now returning to main.\n") ;
	}
	else{
		if(dm) printf("fork() failed\n");
	}
	
}

void executeSequentialCommands(input* inp)
{	
	// This function will run multiple commands in sequence
	// we can use multiple calls to executeCommand() function
	int i;
	
	for(i=0; i<inp->n_cmds; i++){
		// execute first command in inp->cmds array
		// and then return
		executeCommand(inp,0);
		
		// move the cmds pointer, so that it points to next element now
		inp->cmds++; 
	}
}

void executeCommandRedirection(input* inp)
{
	// This function will run a single command with output redirected to an output file specificed by user
	executeCommand(inp, 1);
	
}

// -------------------------------------------- MAIN FUNCTION

int main()
{
	// ---------------------- Initial declarations
	register_handler() ;
	
	// Initialize globals
	cur_working_directory = (char* ) malloc(max_path_len*sizeof(char));
	dm = 0;  // debug mode
	
	// Locals
	const int max_inp_len = 100 ;
	size_t inp_len = max_inp_len; // to be used in getline() function 
	//Looping
	int i,j;
	
	
	char* inp_line = (char*) malloc( max_inp_len * sizeof(char)) ;
	int bytes_read;
	
	
	
	// This loop will keep your shell running until user exits.
	while(1)	
	{
		// get cur_working_directory
		getcwd(cur_working_directory, max_path_len) ;
		
		// Print the prompt in format - currentWorkingDirectory$
		// extra space after $
		printf("%s$", cur_working_directory) ;
		
		// accept input with 'getline()'
		bytes_read = getline(&inp_line, &inp_len, stdin) ;
		
		
		
		if(bytes_read > max_inp_len){
			// ummm ??????	
		}
		
		// remove '\n' character in the end of string      !! its a debugging hack
		inp_line[bytes_read-1] = '\0' ; 
		
		// print debug info
		if(dm) printf("Bytes Read : %d, inp_len : %ld, inp_line : \'%s\' \n", bytes_read, inp_len, inp_line) ;
		
		// special command for debugging
		if(strcmp(inp_line, "flipdm") == 0){
			dm = !dm ;
			printf("Now dm is %d\n", dm);
			continue;
		}
		
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
			continue;
		}
		
		// Check for exit command
		if(	inp->n_cmds == 1 && 
		   	inp->cmds[0].n_args == 1 &&
		  	strcmp(inp->cmds[0].args[0], "exit") == 0)
		{
			// if user types any other arguements after exit, it will not work
			printf("Exiting shell...\n");
			break;
		}
		
		if(inp->is_parallel)
			executeParallelCommands(inp);		// This function is invoked when user wants to run multiple commands in parallel (commands separated by &&)
		else if(inp->is_seq)
			executeSequentialCommands(inp);	// This function is invoked when user wants to run multiple commands sequentially (commands separated by ##)
		else if(inp->out_redir != NULL)
			executeCommandRedirection(inp);	// This function is invoked when user wants redirect output of a single command to and output file specificed by user
		else
			executeCommand(inp, 0);		// This function is invoked when user wants to run a single commands
				
	}
	
	return 0;
}



/*

Suggestions
(Remove 1 level of quotes)
"hello" parsed as hello
"'hello'" parsed as 'hello'

Add garbage handling

*/
