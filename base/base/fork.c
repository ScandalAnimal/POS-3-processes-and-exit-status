#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>

#define _XOPEN_SOURCE
#define _XOPEN_SOURCE_EXTENDED 1 /* XPG 4.2 - needed for WCOREDUMP() */

void printDataAboutProcess(char * name) {
	
	pid_t pid = getpid();
	pid_t ppid = getppid();
	pid_t pgrp = getpgrp();
	uid_t uid = getuid();
	gid_t gid = getgid();
	uid_t euid = geteuid();
	gid_t egid = getegid();
	printf("%s identification: \n", name);
	printf("	pid = %d,	ppid = %d,	pgrp = %d\n", pid, ppid, pgrp);
	printf("	uid = %d,	gid = %d\n", uid, gid);
	printf("	euid = %d,	egid = %d\n", euid, egid);
}

void printDataAfterProcessEnds(char* name, pid_t pid, int status) {

	printf("%s exit (pid = %d):", name, pid);

	if (WIFEXITED(status)) {
		int exit_code = WEXITSTATUS(status);
		printf("	normal termination (exit code = %d)\n", exit_code);		
	}
	else if (WIFSIGNALED(status)) {
		char * message = "";
		#ifdef WCOREDUMP
			if (WCOREDUMP(status)) {
				message = "with core dump ";
			}
		#endif
		int signal_code = WTERMSIG(status);	
		printf("	signal termination %s(signal = %d)\n", message, signal_code);
	}
	else {
		printf("	unknown type of termination\n");		
	}	
}

int main(int argc, char* argv[]) {
	
	argc = argc;
	pid_t parent_pid, child_pid;
	int status;

	printDataAboutProcess("grandparent");

	parent_pid = fork();

	if (parent_pid == 0) {
		printDataAboutProcess("parent");
		child_pid = fork();

		if (child_pid == 0) {
			printDataAboutProcess("child");
			return execv(argv[1], &(argv[1]));
		}
		else if (child_pid > 1) {
			signal(SIGINT, SIG_IGN);
			waitpid(child_pid, &status, 0);
			printDataAfterProcessEnds("child", child_pid, status);
			return EXIT_SUCCESS;
		}
		else {
			return EXIT_FAILURE;
		}

	} 
	else if (parent_pid > 0) {
		signal(SIGINT, SIG_IGN);
		waitpid(parent_pid, &status, 0);
		printDataAfterProcessEnds("parent", parent_pid, status);
		return EXIT_SUCCESS;
	} 
	else {
		return EXIT_FAILURE;
	}
}

