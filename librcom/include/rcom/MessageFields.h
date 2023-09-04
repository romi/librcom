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
#ifndef _LIBRCOM_MESSAGEFIELDS_H_
#define _LIBRCOM_MESSAGEFIELDS_H_

namespace rcom {

        static const constexpr char *kID = "id";
        static const constexpr char *kMethod = "method";
        static const constexpr char *kParams = "params";
        static const constexpr char *kResult = "result";
        static const constexpr char *kError = "error";
        static const constexpr char *kErrorCode = "code";
        static const constexpr char *kErrorMessage = "message";
        static const constexpr char *kNoID = "unknown";
        static const constexpr char *kUnknownMethod = "unknown";
}

#endif // _LIBRCOM_MESSAGEFIELDS_H_

