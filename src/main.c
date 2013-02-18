/* echod - A simple daemon that echos messages
	*
	* main.c
	* 
	* Author: Niklas Karlsson <niklas.karlsson.phd@gmail.com>
	*/

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "daemon.h"
#include "util.h"
#include "net.h"

/*
	* Output the usage help text
	*/
void usage(const char *bin)
{
				int pad_length = strlen(bin);

				printf("Usage: %s [-p port]\n", bin);
				printf("       %*s [-k start|restart|stop]\n", pad_length, " ");
				printf("Options:\n");
				printf("  -p port    : the port to listen on\n");
}

void signal_handler(int signum)
{
				psignal(signum, "caught signal");

				if (signum == SIGTERM) {
								syslog(LOG_NOTICE, "Daemon caugt a SIGTERM, exiting");

								/* Close syslog facility */
								closelog();

								exit(EXIT_SUCCESS);
				}
}

/*
	* Handle options to control the daemon
	*/
void daemon_manager(const char *bin, const char *cmd)
{
				pid_t pid;
				char *str;

				str = str_tolower(cmd);

				if (strcmp(str, "start") == 0) {
								if (load_pid_file() > 0) {
												printf("Daemon is already running\n");
												exit(EXIT_FAILURE);
								}

								pid = create_daemon(bin);
								/* If the returned pid is < 0, there was a problem with the
											fork and we exit with a failure */
								if (pid < 0) {
												free(str);
												exit(EXIT_FAILURE);
								}
								/* If the returned pid > 0 it is the pid of the child and we are
											the parent. Save the pid to the pid file and exit with success */
								if (pid > 0) {
												save_pid_file(pid);
												free(str);
												exit(EXIT_SUCCESS);
								}

								/* If this point is reached, the fork was successfull and we
											are the child process. Continue as daemon from here on. */

				} else if (strcmp(str, "stop") == 0) {
								pid = load_pid_file();
								if (pid <= 0) {
												printf("No pid file, cannot stop\n");
												exit(EXIT_FAILURE);
								}
								/* Send a SIGTERM signal to the daemon */
								kill(pid, SIGTERM);

								remove_pid_file();

								exit(EXIT_SUCCESS);

				} else if (strcmp(str, "restart") == 0 ) {
								/* Leave empty for now */
				}

				free(str);
}

/*
	* Main entry point
	*/
int main(int argc, char *argv[])
{
				char *cmd;
				uint16_t port;
				int c;
				int server_sock, client_sock;
				fd_set active_fd_set, read_fd_set;
				struct sockaddr_in client_addr;
				socklen_t len;
				int i;
				char buffer[MAXMSG];
				int nbytes;

				/* Handle command line options */
				opterr = 0;
				cmd = NULL;
				while ((c = getopt(argc, argv, "hk:p:")) != -1) {
								switch (c) {
				        case 'h':
																usage(argv[0]);
																exit(EXIT_SUCCESS);
				        case 'k':
																cmd = optarg;
																break;
				        case 'p':
																port = atoi(optarg);
																break;
								}
				}

				/* Manage daemon */
				if (cmd != NULL) {
								daemon_manager(argv[0], cmd);
				}

				/* Register a signal handler */
				signal(SIGTERM, &signal_handler);

				/* Create a socket and set it up to accept connections */
				server_sock = create_socket(port);
				if (listen(server_sock, 5) < 0) {
								syslog(LOG_ERR, "'listen' error");
				}

				/* Initialize a set of active sockets */
				FD_ZERO(&active_fd_set);
				FD_SET(server_sock, &active_fd_set);

				while (1) {
								/* Block until input arrives on one or more active sockets */
								read_fd_set = active_fd_set;
								if (select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0) {
												syslog(LOG_ERR, "'select' error");
								}

								/* Service all sockets with pending input */
								for (i = 0; i < FD_SETSIZE; ++i) {
												if (FD_ISSET(i, &read_fd_set)) {
																if (i == server_sock) {
																				/* Connection request on original socket */
																				len = sizeof(client_addr);
																				client_sock = accept(server_sock, 
																																									(struct sockaddr *)&client_addr, 
																																									&len);
																				if (client_sock < 0) {
																								syslog(LOG_ERR, "'accept' error");
																				}
																				
																				syslog(LOG_NOTICE, "Connection from %s on port %hd",
																											inet_ntoa(client_addr.sin_addr),
																											client_addr.sin_port);

																				FD_SET(client_sock, &active_fd_set);
																} else {
																				memset(buffer, 0, MAXMSG);
																				/* Data arriving on already connected socket */
																				nbytes = read(i, buffer, MAXMSG);
																				if (nbytes < 0) {
																								syslog(LOG_ERR, "Error reading from socket");
																				} else if (nbytes == 0) {
																								close(i);
																								FD_CLR(i, &active_fd_set);																								
																				} else {
																								write(i, buffer, strlen(buffer));
																				}
																}
												}
								}

								sleep(1);
				}

				/* Close syslog facility */
				closelog();
}
