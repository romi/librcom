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
#ifndef _LIBRCOM_I_TIMER_H_
#define _LIBRCOM_I_TIMER_H_

namespace rcom {
        
        class ITimer
        {
        public:
                virtual ~ITimer() = default;

                virtual void set_timeout(double seconds); 
                virtual bool has_timed_out(); 
                virtual double get_remaining_time(); 
                virtual void wait(double minumum_time); 
        };
}

#endif // _LIBRCOM_I_CLOCK_H_

