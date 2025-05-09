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
// extern "C" {
// #include <linux/i2c.h>
// #include <i2c/smbus.h>
// //#include <linux/i2c-dev.h>
// }

#include <time.h>
#include <math.h>
#include <stdint.h>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <pwd.h>
#include <string.h>
#include <csignal>
#include <stdexcept>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/wait.h>
#include <sys/random.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include "rcom/Linux.h"
#include "rcom/Log.h"

// LCOV_EXCL_START
namespace  rcom
{

        Linux::Linux(ILog& log) : log_(log), ip_()
        {
        }

        WaitStatus Linux::wait(int sockfd, int timeout)
        {
                WaitStatus retval = kWaitError;
                int timeout_ms = (int) (timeout * 1000.0);
                
                struct pollfd fds[1];
                fds[0].fd = sockfd;
                fds[0].events = POLLIN;
                
                int pollrc = poll(fds, 1, timeout_ms);
                if (pollrc < 0) {
                        log_err(log_, "do_wait: poll error %d", errno);
                        
                } else if (pollrc > 0) {
                        if (fds[0].revents & POLLIN) {
                                retval = kWaitOK;
                        }
                } else {
                        retval = kWaitTimeout;
                }
                return retval;
        }

        int Linux::connect(int sockfd, const IAddress& address)
        {
                socklen_t addrlen = sizeof(struct sockaddr_in);
                struct sockaddr_in addr;
                address_to_sockaddr(address, addr);
                return ::connect(sockfd, (struct sockaddr *) &addr, addrlen);
        }

        int Linux::getaddress(int sockfd, IAddress& address)
        {
                struct sockaddr_in local_addr;
                uint32_t socklen = sizeof(local_addr);
                memset((char *) &local_addr, 0, socklen);
                
                getsockname(sockfd, (struct sockaddr*) &local_addr, &socklen);
                
                sockaddr_to_address(address, local_addr);

                return 0;
        }

        void Linux::address_to_sockaddr(const IAddress& a, struct sockaddr_in& addr)
        {
                addr.sin_family = AF_INET;
                addr.sin_port = htons(a.port());
                if (inet_aton(a.ip().c_str(), &addr.sin_addr) == 0) {
                        throw std::runtime_error("Linux::address_to_sockaddr");
                }
        }

        void Linux::sockaddr_to_address(IAddress& a, const struct sockaddr_in& addr)
        {
                a.set(inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
        }

        int Linux::socket_close(int fd)
        {
                ssize_t n;
                char buf[512];
                if (fd != -1) {
                        shutdown(fd);
                        while (true) {
                                n = recv(fd, buf, 512, 0);
                                if (n <= 0)
                                        break;
                        }
                        close(fd);
                }
                return 0;
        }

        int Linux::tcp_client_socket(const IAddress& address)
        {
                int sockfd = socket();
                if (sockfd != -1) {
                        int ret = connect(sockfd, address);
                        if (ret != 0) {
                                log_err(log_, "Linux::tcp_client_socket: connect failed: "
                                        "%s", strerror(errno));
                                socket_close(sockfd);
                                sockfd = -1;
                        }
                } else {
                        log_err(log_, "Linux::tcp_client_socket: socket failed: "
                                "%s", strerror(errno));
                }
                return sockfd;
        }

        int Linux::tcp_server_socket(const IAddress& address)
        {
                int sockfd = socket();
                if (sockfd != -1) {
                        int ret = bind(sockfd, address);
                        if (ret == 0) {
                                ret = listen(sockfd, 10);
                                if (ret != 0) {
                                        log_err(log_, "Linux::tcp_server_socket: "
                                                "listen failed: %s", strerror(errno));
                                        socket_close(sockfd);
                                        sockfd = -1;
                                }
                        } else {
                                log_err(log_, "Linux::tcp_server_socket: bind failed: %s",
                                        strerror(errno));
                                socket_close(sockfd);
                                sockfd = -1;
                        }
                } else {
                        log_err(log_, "Linux::tcp_server_socket: socket failed: %s",
                                strerror(errno));
                }
                return sockfd;
        }
        
        int Linux::bind(int sockfd, const IAddress& address)
        {
                socklen_t addrlen = sizeof(struct sockaddr_in);
                struct sockaddr_in addr;
                address_to_sockaddr(address, addr);
                return bind(sockfd, (struct sockaddr *) &addr, addrlen);
        }

        double Linux::time()
        {
                struct timespec spec = {0, 0};
                double result;
                
                clock_gettime(CLOCK_REALTIME, &spec);
                result = (double) spec.tv_sec + (double) spec.tv_nsec / 1.0e9;
                
                return result;
        }
        
        void Linux::sleep(double seconds)
        {
                struct timespec spec = {0, 0};
                struct timespec remain = {0, 0};
                
                spec.tv_sec = (time_t) floor(seconds);
                double nsec = seconds - (double) spec.tv_sec;
                spec.tv_nsec = (time_t) floor(nsec * 1.0e9);
                int r = clock_nanosleep(CLOCK_REALTIME, 0, &spec, &remain);
                
                if (r != 0) {
                        if (r == EINTR) {
                                throw std::runtime_error("Linux::nanosleep: Interrupted");
                        } else {
                                throw std::runtime_error("Linux::nanosleep: Failed");
                        }
                }
        }

        // POSIX

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

        ssize_t Linux::getrandom(void *buf, size_t buflen, unsigned int flags)
        {
            return ::getrandom(buf, buflen, flags);
        }

        int Linux::poll(struct pollfd *fds, nfds_t nfds, int timeout)
        {
                return ::poll(fds, nfds, timeout);
        }

        int Linux::socket()
        {
                return ::socket(AF_INET, SOCK_STREAM, 0);
        }

        int Linux::listen(int sockfd, int backlog)
        {
                return ::listen(sockfd, backlog);
        }
        
        int Linux::bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
        {
                return ::bind(sockfd, addr, addrlen);
        }

        int Linux::accept(int sockfd)
        {
                return ::accept(sockfd, nullptr, nullptr);
        }

        int Linux::setsockopt(int sockfd, int level, int optname, 
                              const void *optval, int optlen)
        {
                return ::setsockopt(sockfd, level, optname, optval, (socklen_t) optlen);
        }
        
        int Linux::shutdown(int sockfd)
        {
                return ::shutdown(sockfd, SHUT_RDWR);
        }

        int Linux::close(int fd)
        {
                return ::close(fd);
        }
        
        ssize_t Linux::recv(int sockfd, void *buf, size_t len, int flags)
        {
                return ::recv(sockfd, buf, len, flags);
        }
        
        ssize_t Linux::send(int sockfd, const void *buf, size_t len)
        {
                return ::send(sockfd, buf, len, MSG_NOSIGNAL);
        }
        
        int Linux::getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
        {
                return ::getsockname(sockfd, addr, addrlen);
        }
        
        void Linux::set_local_ip(const std::string& ip)
        {
                ip_ = ip;
        }

        std::string Linux::local_ip()
        {
                std::string ip = "127.0.0.1"; // By default
                struct ifaddrs *ifaddr;

                if (!ip_.empty()) {
                        ip = ip_;
                        
                } else  if (getifaddrs(&ifaddr) == 0) {
                        scan_interfaces(ip, ifaddr);
                        freeifaddrs(ifaddr);
                        
                } else {
                        throw std::runtime_error("getifaddrs() failed");
                }

                return ip;
        }
        
        void Linux::scan_interfaces(std::string& ip, struct ifaddrs *ifaddr)
        {
                for (struct ifaddrs *ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
                        if ((ifa->ifa_addr != NULL) && match_interface(ifa)) {
                                get_interface_ip(ip, ifa);
                        }
                }
        }

        bool Linux::match_interface(struct ifaddrs *ifa)
        {
                return ((strncmp(ifa->ifa_name, "w", 1) == 0)
                        && (ifa->ifa_addr->sa_family == AF_INET));
        }
        
        void Linux::get_interface_ip(std::string& ip, struct ifaddrs *ifa)
        {
                char host[NI_MAXHOST];
                int s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in),
                                    host, NI_MAXHOST,
                                    NULL, 0,
                                    NI_NUMERICHOST);
                        
                if (s == 0) {
                        ip = host;
                } else {
                        throw std::runtime_error("getnameinfo() failed");
                }
        }
        
        // int Linux::open(const char *pathname, int flags)
        // {
        //         return ::open(pathname, flags);
        // }

        // void Linux::exit(int status)
        // {
        //         ::exit(status);
        // }

        // sighandler_t Linux::signal(int signum, sighandler_t handler)
        // {
        //         return ::signal(signum, handler);
        // }

        // int Linux::stat(const char *path, struct stat *buf)
        // {
        //         return ::stat(path, buf);
        // }

        // char* Linux::secure_getenv(const char* name)
        // {
        //         return ::secure_getenv(name);
        // }

        // uid_t Linux::getuid() {
        //             return ::getuid();
        // }

        // passwd *Linux::getpwuid(uid_t uid) {
        //             return ::getpwuid(uid);
        // }

        // FILE * Linux::fopen( const char * file_name, const char * mode )
        // {
        //         return ::fopen( file_name, mode );
        // }

        // int Linux::fclose(FILE *fp)
        // {
        //         return ::fclose(fp);
        // }

        // int Linux::remove( const char * filename )
        // {
        //         return ::remove( filename );
        // }

        // unsigned int Linux::sleep(unsigned int seconds)
        // {
        //         return ::sleep(seconds);
        // }

        // ssize_t Linux::read(int fd, void *buf, size_t count)
        // {
        //         return ::read(fd, buf, count);
        // }

        // int Linux::socket(int domain, int type, int protocol)
        // {
        //         return ::socket(domain, type, protocol);
        // }

        // int32_t Linux::i2c_smbus_read_block_data(int file, uint8_t command,
        //                                          uint8_t length,
        //                                          uint8_t *values)
        // {
        //         return ::i2c_smbus_read_i2c_block_data(file, command, length, values);
        // }
        
        // int32_t Linux::i2c_smbus_write_block_data(int file, uint8_t command,
        //                                           uint8_t length,
        //                                           const uint8_t *values)
        // {
        //         return ::i2c_smbus_write_i2c_block_data(file, command, length, values);
        // }


        // pid_t Linux::waitpid(pid_t pid, int *status, int options)
        // {
        //         return ::waitpid(pid, status, options);
        // }

        // int Linux::execve(const char *filename, char *const argv[], char *const envp[])
        // {
        //         return ::execve(filename, argv, envp);
        // }

        // pid_t Linux::fork(void)
        // {
        //         return ::fork();
        // }

        // int Linux::kill(pid_t pid, int sig)
        // {
        //         return ::kill(pid, sig);
        // }

        // int Linux::system(const char* command)
        // {
        //         return ::system(command);
        // }

        // DIR * Linux::opendir(const char * directory)
        // {
        //         return ::opendir( directory );
        // }

        // int Linux::closedir( DIR * dir )
        // {
        //         return ::closedir( dir );
        // }

        // struct dirent* Linux::readdir( DIR * directory )
        // {
        //         return :: readdir( directory );
        // }

        // int Linux::ioctl(int fd, unsigned long request, void *argp)
        // {
        //         return ::ioctl(fd, request, argp);
        // }
}
// LCOV_EXCL_END

