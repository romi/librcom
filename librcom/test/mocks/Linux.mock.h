#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "rcom/ILinux.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"

class MockLinux : public rcom::ILinux {
public:
        MOCK_METHOD2(open, int(const char *pathname, int flags));
        MOCK_METHOD1(close, int(int fd));
        MOCK_METHOD1(exit, void(int status));
        MOCK_METHOD2(signal, sighandler_t(int signum, sighandler_t handler));
        MOCK_METHOD2(stat, int(const char *path, struct stat *buf));
        MOCK_METHOD3(waitpid, pid_t(pid_t pid, int * status, int options));
        MOCK_METHOD3(execve, int(const char *filename, char *const argv[],
                                 char *const envp[]));
        MOCK_METHOD0(fork, pid_t(void));
        MOCK_METHOD2(kill, int (pid_t pid, int sig));
        MOCK_METHOD1(system, int(const char *command));
        MOCK_METHOD1(secure_getenv, char*(const char* name));
        MOCK_METHOD0(getuid, uid_t());
        MOCK_METHOD1(getpwuid, passwd*(uid_t uid));
        MOCK_METHOD3(getrandom, ssize_t(void *, size_t, unsigned int));
        MOCK_METHOD2(fopen, FILE * ( const char *filepath, const char *mode ));
        MOCK_METHOD1(fclose, int (FILE *fp));
        MOCK_METHOD1(opendir, DIR* (const char *path));
        MOCK_METHOD1(closedir, int (DIR *directory));
        MOCK_METHOD1(readdir, dirent *(DIR *directory));
        MOCK_METHOD1(remove, int(const char *filename));
        MOCK_METHOD1(sleep, unsigned int(unsigned int seconds));
        MOCK_METHOD3(ioctl, int(int fd, unsigned long request, void *argp));
        MOCK_METHOD3(poll, int(struct pollfd *fds, nfds_t nfds, int timeout));
        MOCK_METHOD3(read, ssize_t(int fd, void *buf, size_t count));
        MOCK_METHOD3(socket, int(int domain, int type, int protocol));
        MOCK_METHOD3(connect, int(int sockfd, const struct sockaddr *addr,
                                  socklen_t addrlen));
        MOCK_METHOD2(shutdown, int(int sockfd, int how));
        MOCK_METHOD4(recv, ssize_t(int sockfd, void *buf, size_t len, int flags));
        MOCK_METHOD4(send, ssize_t(int sockfd, const void *buf,
                                   size_t len, int flags));
        MOCK_METHOD3(bind, int(int sockfd, const struct sockaddr *addr,
                               socklen_t addrlen));
        MOCK_METHOD2(listen, int(int sockfd, int backlog));
        MOCK_METHOD3(accept, int(int sockfd, struct sockaddr *addr,
                                 socklen_t *addrlen));
        MOCK_METHOD5(setsockopt, int(int sockfd, int level, int optname,
                                     const void *optval, socklen_t optlen));
        MOCK_METHOD3(getsockname, int(int sockfd, struct sockaddr *addr,
                                      socklen_t *addrlen));
        MOCK_METHOD(int, clock_gettime, (clockid_t clock_id, struct timespec *tp), (override));
        MOCK_METHOD(int, clock_nanosleep, (clockid_t clock_id, int flags,
                                           const struct timespec *request,
                                           struct timespec *remain), (override));
};

#pragma GCC diagnostic pop
