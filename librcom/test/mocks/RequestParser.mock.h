#include "gmock/gmock.h"
#include "IRequestParser.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"

class MockRequestParser : public rcom::IRequestParser
{
public:
        MOCK_METHOD(bool, parse, (rcom::ISocket& socket), (override));
        MOCK_METHOD(rcom::IRequest&, request, (), (override));
};

#pragma GCC diagnostic pop

