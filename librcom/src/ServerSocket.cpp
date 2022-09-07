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
#include <stdexcept>
#include "rcom/ServerSocket.h"
#include "rcom/Address.h"

namespace rcom {
        
        ServerSocket::ServerSocket(const std::shared_ptr<ILinux>& linux,
                                   const std::shared_ptr<ILog>& log,
                                   IAddress& address)
                : socket_(linux, log)
        {
                if (!socket_.listen(address))
                        throw std::runtime_error("ServerSocket: open failed");
        }
                
        int ServerSocket::accept(double timeout)
        {
                return socket_.accept(timeout);
        }
                
        void ServerSocket::get_address(IAddress& address)
        {
                return socket_.get_address(address);
        }

        void ServerSocket::close()
        {
                socket_.close();
        }
}
