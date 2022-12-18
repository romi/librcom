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
extern "C" {
#include <linux/i2c.h>
#include <i2c/smbus.h>
//#include <linux/i2c-dev.h>
}
#include "rcom/Linux.h"

// LCOV_EXCL_START
namespace  rcom
{

        Linux::Linux()
        {
        }

        int Linux::open(const char *pathname, int flags)
        {
                return ::open(pathname, flags);
        }

        int Linux::close(int fd)
        {
                return ::close(fd);
        }

        void Linux::exit(int status)
        {
                ::exit(status);
        }

        sighandler_t Linux::signal(int signum, sighandler_t handler)
        {
                return ::signal(signum, handler);
        }

        int Linux::stat(const char *path, struct stat *buf)
        {
                return ::stat(path, buf);
        }

        pid_t Linux::waitpid(pid_t pid, int *status, int options)
        {
                return ::waitpid(pid, status, options);
        }

        int Linux::execve(const char *filename, char *const argv[], char *const envp[])
        {
                return ::execve(filename, argv, envp);
        }

        pid_t Linux::fork(void)
        {
                return ::fork();
        }

        int Linux::kill(pid_t pid, int sig)
        {
                return ::kill(pid, sig);
        }

        int Linux::system(const char* command)
        {
                return ::system(command);
        }

        char* Linux::secure_getenv(const char* name)
        {
                return ::secure_getenv(name);
        }

        uid_t Linux::getuid() {
                    return ::getuid();
        }

        passwd *Linux::getpwuid(uid_t uid) {
                    return ::getpwuid(uid);
        }

        ssize_t Linux::getrandom(void *buf, size_t buflen, unsigned int flags) {
            return ::getrandom(buf, buflen, flags);
        }

        DIR * Linux::opendir(const char * directory)
        {
                return ::opendir( directory );
        }

        int Linux::closedir( DIR * dir )
        {
                return ::closedir( dir );
        }

        FILE * Linux::fopen( const char * file_name, const char * mode )
        {
                return ::fopen( file_name, mode );
        }

        int Linux::fclose(FILE *fp)
        {
                return ::fclose(fp);
        }

        struct dirent* Linux::readdir( DIR * directory )
        {
                return :: readdir( directory );
        }

        int Linux::remove( const char * filename )
        {
                return ::remove( filename );
        }

        unsigned int Linux::sleep(unsigned int seconds)
        {
                return ::sleep(seconds);
        }

        int Linux::ioctl(int fd, unsigned long request, void *argp)
        {
                return ::ioctl(fd, request, argp);
        }
                
        int Linux::poll(struct pollfd *fds, nfds_t nfds, int timeout)
        {
                return ::poll(fds, nfds, timeout);
        }

        ssize_t Linux::read(int fd, void *buf, size_t count)
        {
                return ::read(fd, buf, count);
        }

        int Linux::socket(int domain, int type, int protocol)
        {
                return ::socket(domain, type, protocol);
        }

        int Linux::connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
        {
                return ::connect(sockfd, addr, addrlen);
        }

        int Linux::shutdown(int sockfd, int how)
        {
                return ::shutdown(sockfd, how);
        }
        
        ssize_t Linux::recv(int sockfd, void *buf, size_t len, int flags)
        {
                return ::recv(sockfd, buf, len, flags);
        }
        
        ssize_t Linux::send(int sockfd, const void *buf, size_t len, int flags)
        {
                return ::send(sockfd, buf, len, flags);
        }
        
        int Linux::bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
        {
                return ::bind(sockfd, addr, addrlen);
        }

        int Linux::listen(int sockfd, int backlog)
        {
                return ::listen(sockfd, backlog);
        }

        int Linux::accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
        {
                return ::accept(sockfd, addr, addrlen);
        }

        int Linux::setsockopt(int sockfd, int level, int optname, const
                              void *optval, socklen_t optlen)
        {
                return ::setsockopt(sockfd, level, optname, optval, optlen);
        }

        int Linux::getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
        {
                return ::getsockname(sockfd, addr, addrlen);
        }

        int Linux::clock_gettime(clockid_t clock_id, struct timespec *tp) 
        {
                return ::clock_gettime(clock_id, tp);
        }
        
        int Linux::clock_nanosleep(clockid_t clock_id, int flags,
                                   const struct timespec *request,
                                   struct timespec *remain) 
        {
                return ::clock_nanosleep(clock_id, flags, request, remain);
        }

        int32_t Linux::i2c_smbus_read_block_data(int file, uint8_t command,
                                                 uint8_t length,
                                                 uint8_t *values)
        {
                return ::i2c_smbus_read_i2c_block_data(file, command, length, values);
        }
        
        int32_t Linux::i2c_smbus_write_block_data(int file, uint8_t command,
                                                  uint8_t length,
                                                  const uint8_t *values)
        {
                return ::i2c_smbus_write_i2c_block_data(file, command, length, values);
        }
}
// LCOV_EXCL_END

