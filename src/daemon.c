#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>

#define PID_FILE "/tmp/echod.pid"

/*
	* Create the pid file
	*/
int save_pid_file(const pid_t pid)
{
				FILE *pfile;

				pfile = fopen(PID_FILE, "w");
				if (pfile == NULL) {
								return -1;
				}

				fprintf(pfile, "%d\n", (int)pid);
				fclose(pfile);

				return 0;
}

/*
	* Load the pid file
	*/
pid_t load_pid_file()
{
				FILE *pfile;
				pid_t pid;

				pfile = fopen(PID_FILE, "r");
				if (pfile == NULL) {
								return (pid_t)(-1);
				}

				fscanf(pfile, "%d", &pid);
				fclose(pfile);
				
				return pid;
}

void remove_pid_file()
{
				remove(PID_FILE);
}

/*
	* Create a daemon process using fork
	*/
pid_t create_daemon(const char *bin)
{
				pid_t pid, sid;

				pid = fork();

				if (pid != 0) {
								return pid;
				}

				/* If this point is reached, we are the child process and must
							act appropriatly */
				umask(0);

				/* Open a connection to the syslog server */
				openlog(bin, LOG_NOWAIT|LOG_PID, LOG_USER);

				/* Log successfull start */
				syslog(LOG_NOTICE, "Daemon successfully started");
				
				sid = setsid();
				if (sid < 0) {
								syslog(LOG_ERR, "Could not create process group");
								exit(EXIT_FAILURE);
				}

				/* Change the current working directory */
				if ((chdir("/")) < 0) {
								syslog(LOG_ERR, "Could not change working directory\n");
								exit(EXIT_FAILURE);
				}

				close(STDIN_FILENO);
				close(STDOUT_FILENO);
				close(STDERR_FILENO);

				return pid;
}

