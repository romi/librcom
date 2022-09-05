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
#include "rcom/MessageLink.h"
#include "rcom/WebSocketServer.h"
#include "rcom/ConsoleLogger.h"
#include "rcom/RcomClient.h"

namespace rcom {
        
        std::unique_ptr<IRPCClient> RcomClient::create(const std::string& topic,
                                                       double timeout_seconds)
        {
                std::unique_ptr<IMessageLink> link
                        = std::make_unique<MessageLink>(topic, timeout_seconds);
                return std::make_unique<RcomClient>(link, timeout_seconds);
        }
        
        RcomClient::RcomClient(std::unique_ptr<IMessageLink>& link,
                               double timeout_seconds)
                : link_(std::move(link)),
                  buffer_(),
                  timeout_(timeout_seconds)
        {
        }

        void RcomClient::execute(const std::string& method, nlohmann::json &params,
                                nlohmann::json &result, RPCError &error)
        {
                //r_debug("RcomClient::execute");

                try {

                        try_execute(method, params, result, error);
                        
                } catch (std::exception& e) {
                        error.code = RPCError::kInternalError;
                        error.message = e.what();                        
                }
        }
        
        void RcomClient::try_execute(const std::string& method, nlohmann::json &params,
                                    nlohmann::json &result, RPCError &error)
        { 
                //r_debug("RcomClient::try_execute");
                make_request(method, params);
                if (send_request(kTextMessage, error)
                    && receive_response(buffer_, error)) {
                        parse_response(result, error);
                }
        }

        void RcomClient::make_request(const std::string& method, nlohmann::json &params)
        {
                //r_debug("RcomClient::make_request");
                nlohmann::json request;
                request["method"] = method;
                request["params"] = params;

                buffer_.clear();
                buffer_.append_string(request.dump().c_str());
        }

        bool RcomClient::send_request(MessageType type, RPCError &error)
        {
                //r_debug("RcomClient::send_request");
                bool success = false;
                if (link_->send(buffer_, type)) {
                        success = true;
                } else {
                        error.code = RPCError::kSendError;
                        error.message = "RcomClient: Sending failed";
                }
                return success;
        }

        bool RcomClient::receive_response(MemBuffer& buffer, RPCError &error)
        {
                //r_debug("RcomClient::receive_response");
                bool success = false;
                if (link_->recv(buffer, timeout_)) {
                        success = true;
                        // const std::vector<uint8_t>& data = buffer.data();
                        // uint8_t first_char = data[0];
                        // if (first_char == '{')
                        //         r_debug("RcomClient: response (text): "
                        //                 "%.*s", buffer.size(), data.data());
                        // else 
                        //         r_debug("RcomClient::receive_response kRecvBinary: "
                        //                 "length %zu", buffer.size());
                } else {
                        // r_debug("RcomClient::receive_response: not text/binary %.*s",
                        //         buffer.size(), buffer.data().data());
                        set_error(error);
                }
                return success;
        }

        void RcomClient::parse_response(nlohmann::json &result, RPCError &error)
        {
                //r_debug("RcomClient::parse_response");
                try {
                        result = nlohmann::json::parse(buffer_.tostring());
                                
                } catch (std::exception& e) {
                        error.code = RPCError::kParseError;
                        error.message = "RcomClient: Parsing response failed";
                }
        }
        
        void RcomClient::set_error(RPCError &error)
        {
                //r_debug("RcomClient::set_error");
                switch (link_->recv_status()) {
                case kRecvError:
                        error.code = RPCError::kReceiveError;
                        error.message = "RcomClient: Receive failed";
                        break;
                case kRecvClosed:
                        error.code = RPCError::kLinkClosed;
                        error.message = "RcomClient: Link closed";
                        break;
                case kRecvTimeOut:
                        error.code = RPCError::kReceiveTimeout;
                        error.message = "RcomClient: Timeout";
                        break;
                case kRecvText:
                case kRecvBinary:
                default:
                        error.code = 0;
                        break;
                }
        }

        void RcomClient::execute(const std::string& method,
                                 nlohmann::json &params,
                                 MemBuffer& result,
                                 RPCError &error)
        {
                //r_debug("RcomClient::execute");

                try {

                        try_execute(method, params, result, error);
                        
                } catch (std::exception& e) {
                        error.code = RPCError::kInternalError;
                        error.message = e.what();                        
                }
        }
        
        void RcomClient::try_execute(const std::string& method,
                                     nlohmann::json &params,
                                     MemBuffer& result,
                                     RPCError &error)
        { 
                make_request(method, params);
                if (send_request(kBinaryMessage, error)) {
                        receive_response(result, error);
                }
        }

        bool RcomClient::is_connected()
        {
                return link_->is_connected();
        }

        RcomClient::~RcomClient() {
                //       r_debug("RcomClient::destruct");
        }
}