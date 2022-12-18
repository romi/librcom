/*
  rcom

  Copyright (C) 2019 Sony Computer Science Laboratories
  Author(s) Peter Hanappe

  rcom is light-weight libary for inter-node communication.

  rcom is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation, either version 3 of the
  License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see
  <http://www.gnu.org/licenses/>.

 */
#ifndef RCOM_ILINUX_H
#define RCOM_ILINUX_H

#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/random.h>

#include <cstdarg>
#include <csignal>
#include <dirent.h>
#include <cstdio>
#include <stdint.h>

#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#include <pwd.h>
#include <time.h>

namespace rcom {

    class ILinux {
    public:
            virtual ~ILinux() = default;
            
            virtual int open(const char *pathname, int flags) = 0;
            virtual int stat(const char *path, struct stat *buf) = 0;
            virtual int ioctl(int fd, unsigned long request, void *argp) = 0;
            virtual ssize_t read(int fd, void *buf, size_t count) = 0;
            virtual int close(int fd) = 0;
        
            virtual FILE *fopen(const char *filename, const char *mode) = 0;
            virtual int fclose(FILE *fp) = 0;
            virtual int remove(const char *s) = 0;

            virtual DIR *opendir(const char *directory) = 0;
            virtual int closedir(DIR *) = 0;
            virtual struct dirent *readdir(DIR *directory) = 0;
        
            virtual sighandler_t signal(int signum, sighandler_t handler) = 0;

            virtual void exit(int status) = 0;
            virtual pid_t fork() = 0;
            virtual int system(const char *command) = 0;
            virtual int execve(const char *filename, char *const argv[],
                               char *const envp[]) = 0;
            virtual pid_t waitpid(pid_t pid, int *status, int options) = 0;
            virtual int kill(pid_t pid, int sig) = 0;
        
            virtual char* secure_getenv(const char* name) = 0;
            virtual uid_t getuid () = 0;
            virtual passwd *getpwuid (uid_t uid) = 0;
        
            virtual ssize_t getrandom(void *buf, size_t buflen, unsigned int flags) = 0;

            virtual unsigned int sleep(unsigned int seconds) = 0;
            virtual int clock_gettime(clockid_t clock_id, struct timespec *tp) = 0;
            virtual int clock_nanosleep(clockid_t clock_id, int flags,
                                        const struct timespec *request,
                                        struct timespec *remain) = 0;
        
            virtual int poll(struct pollfd *fds, nfds_t nfds, int timeout) = 0;
        
            virtual int socket(int domain, int type, int protocol) = 0;
            virtual int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) = 0;
            virtual int shutdown(int sockfd, int how) = 0;
            virtual ssize_t recv(int sockfd, void *buf, size_t len, int flags) = 0;
            virtual ssize_t send(int sockfd, const void *buf, size_t len, int flags) = 0;
            virtual int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) = 0;
            virtual int listen(int sockfd, int backlog) = 0;
            virtual int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) = 0;
            virtual int setsockopt(int sockfd, int level, int optname,
                                   const void *optval, socklen_t optlen) = 0;
            virtual int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen) = 0;

            virtual int32_t i2c_smbus_read_block_data(int file, uint8_t command,
                                                      uint8_t length, uint8_t *values) = 0;
            virtual int32_t i2c_smbus_write_block_data(int file, uint8_t command,
                                                       uint8_t length,
                                                       const uint8_t *values) = 0;
    };
}
#endif // RCOM_ILINUX_H
