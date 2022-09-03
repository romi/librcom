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

namespace rcom {
        
        enum HttpStatusCode {
                kHttpStatusContinue = 100,
                kHttpStatusSwitchingProtocols = 101,
                kHttpStatusOK = 200,
                kHttpStatusCreated = 201,
                kHttpStatusAccepted = 202,
                kHttpStatusNonAuthoritativeInformation = 203,
                kHttpStatusNoContent = 204,
                kHttpStatusResetContent = 205,
                kHttpStatusPartialContent = 206,
                kHttpStatusMultipleChoices = 300,
                kHttpStatusMovedPermanently = 301,
                kHttpStatusFound = 302,
                kHttpStatusSeeOther = 303,
                kHttpStatusNotModified = 304,
                kHttpStatusTemporaryRedirect = 307,
                kHttpStatusPermanentRedirect = 308,
                kHttpStatusBadRequest = 400,
                kHttpStatusUnauthorized = 401,
                kHttpStatusForbidden = 403,
                kHttpStatusNotFound = 404,
                kHttpStatusMethodNotAllowed = 405,
                kHttpStatusNotAcceptable = 406,
                kHttpStatusProxyAuthenticationRequired = 407,
                kHttpStatusRequestTimeout = 408,
                kHttpStatusConflict = 409,
                kHttpStatusGone = 410,
                kHttpStatusLengthRequired = 411,
                kHttpStatusPreconditionFailed = 412,
                kHttpStatusEntityTooLarge = 413,
                kHttpStatusURITooLong = 414,
                kHttpStatusUnsupportedMediaType = 415,
                kHttpStatusRangeNotSatisfiable = 416,
                kHttpStatusExpectationFailed = 417,
                kHttpStatusImATeapot = 418,
                kHttpStatusUnprocessableEntity = 422,
                kHttpStatusTooEarly = 425,
                kHttpStatusUpgradeRequired = 426,
                kHttpStatusPreconditionRequired = 428,
                kHttpStatusTooManyRequests = 429,
                kHttpStatusRequestHeaderFieldsTooLarge = 431,
                kHttpStatusUnavailableForLegalReasons = 451,
                kHttpStatusInternalServerError = 500,
                kHttpStatusNotImplemented = 501,
                kHttpStatusBadGateway = 502,
                kHttpStatusServiceUnavailable = 503,
                kHttpStatusGatewayTimeout = 504,
                kHttpStatusHTTPVersionNotSupported = 505,
                kHttpStatusNetworkAuthenticationRequired = 511,
        };
}
