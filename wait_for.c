#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <unistd.h>
#include <sys/wait.h>

#include <sys/prctl.h>

int main(int argc, char *argv[])
{
	if (argc<2)
	{
		printf("This program runs a specified program and waits for all of its decendants\n"
		       "Usage: %s program [program args]\n", argv[0]);
		return EXIT_SUCCESS;
	}

	//Adopt our orphaned decendants ourselves instead of making init do it
	prctl(PR_SET_CHILD_SUBREAPER);

	pid_t child=fork();
	if (child==-1) //Error
	{
		perror("fork");
		return EXIT_FAILURE;
	}
	else if (child==0) //We're the child
	{
		execvp(argv[1], argv+1);
		perror("execvp");
		return EXIT_FAILURE;
	}
	//We're the parent
	//I can't tell if wait is allowed to alter status on a failed call, so
	//err on the side of caution and cache the last valid value.
	int last_status=0;
	while (1)
	{
		int status;
		child=wait(&status);
		if (child==-1)
		{
			if (errno==EINTR)
				continue;
			//We have no more children, so exit
			if (errno==ECHILD)
				break;
			//Unknown error
			perror("wait");
			return EXIT_FAILURE;
		}
		last_status=status;
	}
	if (last_status==0 || WIFEXITED(last_status))
		return WEXITSTATUS(last_status);
	// Fail if the last child was terminated with a signal
	return EXIT_FAILURE;
}
