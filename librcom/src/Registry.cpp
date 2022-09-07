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
#include "rcom/Log.h"
#include "rcom/Registry.h"

namespace rcom {

        Registry::Registry() : entries_()
        {
        } 

        ssize_t Registry::find(const std::string& topic)
        {
                ssize_t retval = -1;
                for (size_t i = 0; i < entries_.size(); i++) {
                        if (topic.compare(entries_[i].topic) == 0) {
                                retval = (ssize_t) i;
                                break;
                        }
                }
                return retval;
        }

        void Registry::erase(ssize_t index)
        {
                entries_.erase(entries_.begin() + index);
        }

        void Registry::erase(const std::string& topic)
        {
                ssize_t index = find(topic);
                if (index >= 0)
                        erase(index);
        }

        void Registry::insert(const std::string& topic, IAddress& address)
        {
                entries_.emplace_back(topic, address);
        }
        
        //
        
        void Registry::set(const std::string& topic, IAddress& address)
        {
                if (address.is_set()) {
                        erase(topic);
                        insert(topic, address);
                } else {
                        throw std::runtime_error("Registry::add: address not set");
                }
        }
        
        bool Registry::get(const std::string& topic, IAddress& address, double timeout)
        {
                (void) timeout;
                bool found = false;
                ssize_t index = find(topic);
                if (index >= 0) {
                        address.set(entries_[(size_t)index].address);
                        found = true;
                }
                return found;
        }
                
        void Registry::remove(const std::string& topic)
        {
                ssize_t index = find(topic);
                if (index >= 0)
                        erase(index);
        }
}

