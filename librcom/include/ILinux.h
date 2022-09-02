#ifndef RCOM_ILINUX_H
#define RCOM_ILINUX_H

#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/poll.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/random.h>

#include <cstdarg>
#include <csignal>
#include <dirent.h>
#include <cstdio>

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
    };
}
#endif // RCOM_ILINUX_H
