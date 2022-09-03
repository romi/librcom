#ifndef RCOM_LINUX_H
#define RCOM_LINUX_H

#include "rcom/ILinux.h"

namespace  rcom {

        class Linux : public ILinux {
        public:
                Linux();
                virtual ~Linux() = default;

                int open(const char *pathname, int flags) override;
                int close(int fd) override;
                void exit(int status) override;
                sighandler_t signal(int signum, sighandler_t handler) override;
                int stat(const char *path, struct stat *buf) override;
                pid_t waitpid(pid_t pid, int *status, int options) override;
                int execve(const char *filename, char *const argv[],
                           char *const envp[]) override;
                pid_t fork(void) override;
                int kill(pid_t pid, int sig) override;
                int system(const char *command) override;
                char* secure_getenv(const char* name) override;
                uid_t getuid () override;
                passwd *getpwuid (uid_t uid) override;
                ssize_t getrandom(void *buf, size_t buflen, unsigned int flags) override;
                FILE *fopen(const char *filename, const char *mode) override;
                int fclose(FILE *fp) override;
                DIR *opendir(const char *directory) override;
                int closedir(DIR *dir) override;
                struct dirent *readdir(DIR *directory) override;
                int remove(const char *filename) override;
                unsigned int sleep(unsigned int seconds) override;
                int ioctl(int fd, unsigned long request, void *argp) override;
                int poll(struct pollfd *fds, nfds_t nfds, int timeout) override;
                ssize_t read(int fd, void *buf, size_t count) override;
                int socket(int domain, int type, int protocol) override;
                int connect(int sockfd, const struct sockaddr *addr,
                            socklen_t addrlen) override;
                int shutdown(int sockfd, int how) override;
                ssize_t recv(int sockfd, void *buf, size_t len, int flags) override;
                ssize_t send(int sockfd, const void *buf, size_t len, int flags) override;
                int bind(int sockfd, const struct sockaddr *addr,
                         socklen_t addrlen) override;
                int listen(int sockfd, int backlog) override;
                int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) override;
                int setsockopt(int sockfd, int level, int optname,
                               const void *optval, socklen_t optlen) override;
                int getsockname(int sockfd, struct sockaddr *addr,
                                socklen_t *addrlen) override;
                int clock_gettime(clockid_t clock_id, struct timespec *tp) override;
                int clock_nanosleep(clockid_t clock_id, int flags,
                                    const struct timespec *request,
                                    struct timespec *remain) override;
        };
}

#endif // RCOM_LINUX_H
