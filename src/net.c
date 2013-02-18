#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "net.h"

int create_socket(uint16_t port)
{
				int sock;
				struct sockaddr_in name;

				/* Create the socket */
				sock = socket(PF_INET, SOCK_STREAM, 0);
				if (sock < 0) {
								syslog(LOG_ERR, "Could not create socket");
				}

				/* Give the socket a name */
				name.sin_family = AF_INET;
				name.sin_port = htons(port);
				name.sin_addr.s_addr = INADDR_ANY;
				if (bind(sock, (struct sockaddr *)&name, sizeof(name)) < 0) {
								syslog(LOG_ERR, "Error binding socket name");
				}

				return sock;
}
