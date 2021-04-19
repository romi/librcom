/*
  romi-rover

  Copyright (C) 2019 Sony Computer Science Laboratories
  Author(s) Peter Hanappe

  romi-rover is collection of applications for the Romi Rover.

  romi-rover is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see
  <http://www.gnu.org/licenses/>.

 */
#include <stdexcept>
#include "RPCClient.h"

namespace rcom {
        
        RPCClient::RPCClient(const char *name, const char *topic, double timeout_seconds)
                : link_(topic, timeout_seconds), buffer_()
        {
                (void) name;
        }

        static int32_t to_membuffer(void* userdata, const char* s, size_t len)
        {
                rpp::MemBuffer *serialised = reinterpret_cast<rpp::MemBuffer*>(userdata);
                serialised->append((uint8_t *) s, len);
                return 0;
        }

        void RPCClient::execute(const std::string& method, JsonCpp &params,
                                JsonCpp &result, RPCError &error)
        {
                r_debug("RPCClient::execute");

                try {

                        try_execute(method, params, result, error);
                        
                } catch (std::exception& e) {
                        error.code = RPCError::kInternalError;
                        error.message = e.what();                        
                }
        }
        
        void RPCClient::try_execute(const std::string& method, JsonCpp &params,
                                    JsonCpp &result, RPCError &error)
        { 
                make_request(method, params);
                if (send_request(error)) {
                        receive_response(result, error);
                }
        }

        void RPCClient::make_request(const std::string& method, JsonCpp &params)
        {
                JsonCpp request = JsonCpp::construct("{\"method\": \"%s\"}",
                                                     method.c_str());
                
                // FIXME: use C++ API?
                json_object_set(request.ptr(), "params", params.ptr());
                
                // FIXME! improve JsonCpp and/or MemBuffer
                // Yet another copy
                buffer_.clear();
                json_serialise(request.ptr(), k_json_compact, to_membuffer,
                               reinterpret_cast<void*>(&buffer_));
        }

        bool RPCClient::send_request(RPCError &error)
        {
                bool success = false;
                if (link_.send(buffer_)) {
                        success = true;
                } else {
                        error.code = RPCError::kSendError;
                        error.message = "RPCClient: Sending failed";
                }
                return success;
        }

        void RPCClient::receive_response(JsonCpp &result, RPCError &error)
        {
                if (link_.recv(buffer_, 0.1)) {
                        parse_response(result, error);
                        
                } else {
                        set_error(error);
                }
        }

        void RPCClient::parse_response(JsonCpp &result, RPCError &error)
        {
                try {
                        result = JsonCpp::parse(buffer_);
                                
                } catch (std::exception& e) {
                        error.code = RPCError::kParseError;
                        error.message = "RPCClient: Parsing response failed";
                }
        }
        
        void RPCClient::set_error(RPCError &error)
        {
                switch (link_.recv_status()) {
                case kRecvError:
                        error.code = RPCError::kReceiveError;
                        error.message = "RPCClient: Receive failed";
                        break;
                case kRecvClosed:
                        error.code = RPCError::kLinkClosed;
                        error.message = "RPCClient: Link closed";
                        break;
                case kRecvTimeOut:
                        error.code = RPCError::kReceiveTimeout;
                        error.message = "RPCClient: Timeout";
                        break;
                case kRecvText:
                case kRecvBinary:
                default:
                        error.code = 0;
                        break;
                }
        }
}
