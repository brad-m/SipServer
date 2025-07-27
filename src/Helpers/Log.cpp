#include "Log.hpp"
#include <string>

 void Log::Protocol(std::shared_ptr<SipMessage> message)
{
	 std::cout << message->toString() << std::endl << std::endl;
}
 void Log::Message(std::string message)
 {
	 std::cout << message << std::endl;
 }
