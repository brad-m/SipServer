#include "SipMessage.hpp"
class Log {
public:
	static void Protocol(std::shared_ptr<SipMessage> message);
	static void Log::Message(std::string message);
};