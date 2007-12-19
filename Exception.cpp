
#include "Exception.h"

Exception::Exception(std::string szErrorMessage)
{
	m_Message = szErrorMessage;
}

Exception::~Exception(void)
{
}

const char* Exception::GetErrorMessage()
{
	return m_Message.c_str();
}


