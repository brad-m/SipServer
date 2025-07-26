#include "Log.hpp"

 void Log::Protocol(std::shared_ptr<SipMessage> message)
{
	std::cout  << message->toString() << std::endl;
}
 void Log::Message(std::string message)
 {
	 std::cout << message << std::endl;
 }
