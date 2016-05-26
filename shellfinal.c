#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/wait.h>
#include<unistd.h>

char pwd[100];

int execute(char **args);
char *shell_read(void);
char **shell_split(char *line);

int shell_function(char **args);
int shell_launch(char **args);
int shell_cd(char **args);
int shell_exit(char **args)
{
	return 0;
}

int main(int arg, char **argv)
{


	char *line;
	char **arguments;
	int status;

	do{
		// printf("entering loop");
		char *user=getenv("USER");
		char hostname[1024];
		
		gethostname(hostname,1023);
		getcwd(pwd, 100);

		putchar('\n');

		printf("%s@%s%s>", user, hostname, pwd);

		line=shell_read();
		arguments = shell_split(line);
		status = execute(arguments);

		free(line);
		free(arguments);
	}while(status);

}


char *shell_read(void)
{
	char *line = NULL;
	int lines;
	//printf("reading");
	ssize_t buffersize = 0; 
	getline(&line, &buffersize, stdin);
	return line;
}


char **shell_split(char *line)
{
	int buffersize = 64;
	int position = 0;
	char *token;
	char **tokens = malloc(buffersize * sizeof(char*));



	token = strtok(line, " \t\n;");
	while (token != NULL) 
	{

		if (position >= buffersize) {
			buffersize += 64;
			tokens = realloc(tokens, buffersize * sizeof(char*));
		}
		tokens[position] = token;
		position+=1;

		token = strtok(NULL, " \t\n;");
	}
	tokens[position] = NULL;
	return tokens;
}
int shell_launch(char **arguments)
{
	pid_t pid, wpid;
	int status;

	pid = fork();
	if (pid == 0) {
		if (execvp(arguments[0], arguments) == -1) {
			perror("shell");
		}
		//exit(0);
		exit(EXIT_FAILURE);
	} else if (pid < 0) {
		perror("shell");
	} else {
		do {
			wpid = waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}

	return 1;
}
char *builtin_string[] = {"cd", "exit"};

int (*builtin_function[]) (char **) = {&shell_cd, &shell_exit};

int shell_num_builtins()
{
	return sizeof(builtin_string) / sizeof(char *);
}

int shell_cd(char **arguments)
{
	//printf("hey);
	if (arguments[1] == NULL) 
	{
		fprintf(stderr, "shell: expected argument to \"cd\"\n");
	} 
	else
       	{
		/*
		   if(chdir[arguments[1] =0)
		   
		   {
		   	fprintf("unexpected command");
		   }
		   */
		if (chdir(arguments[1]) != 0)
	       	{

			perror("Not valid");
		}
	}
	return 1;
}

int execute(char **arguments)
{
	int i;
	int flag;
//	do{
	if (arguments[0] == NULL)
       	{
		return 1;
	}
	flag++;
	for (i = 0; i < shell_num_builtins(); i++)
       	{
		//printf("lol");
		if (strcmp(arguments[0], builtin_string[i]) == 0)
	       	{
			return (*builtin_function[i])(arguments);
		}
	}
//	}while(flag!=1)
	return shell_launch(arguments);
}
