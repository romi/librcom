#include "IResponseParser.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"

class MockResponseParser : public rcom::IResponseParser
{
public:
        MOCK_METHOD(bool, parse, (rcom::ISocket& socket), (override));
        MOCK_METHOD(rcom::IResponse&, response, (), (override));
};

#pragma GCC diagnostic pop

