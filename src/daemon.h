#ifndef DAEMON_H
#define DAEMON_H

pid_t create_daemon(const char *bin);
int save_pid_file(const pid_t pid);
pid_t load_pid_file();
void remove_pid_file();

#endif
