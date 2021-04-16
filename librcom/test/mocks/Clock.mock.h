#include "gmock/gmock.h"
#include "IClock.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"

class MockClock : public rcom::IClock
{
public:
        MOCK_METHOD(void, sleep, (double seconds), (override));
        MOCK_METHOD(double, time, (), (override));
};

#pragma GCC diagnostic pop
