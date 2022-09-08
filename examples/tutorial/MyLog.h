#include <rcom/ILog.h>

class MyLog : public rcom::ILog
{
public:
        MyLog() {}
        ~MyLog() override = default;
                
        void error(const std::string& message) override {
                std::cout << "MyErr: " << message << std::endl;
        }
                
        void warn(const std::string& message) override {
                std::cout << "MyWarn: " << message << std::endl;
        }
                        
        void info(const std::string& message) override {
                std::cout << "MyInfo: " << message << std::endl;
        }
                
        void debug(const std::string& message) override {
                std::cout << "MyDebug: " << message << std::endl;
        }
};
