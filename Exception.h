#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__
#include <string>


class Exception
{
protected:
	std::string m_Message;


public:
	Exception(std::string szErrorMessage);
	const char* GetErrorMessage();
public:
	~Exception(void);
};

class DatabaseException : public Exception
{
public:
	DatabaseException(std::string szError) : Exception(szError) { };
};

#endif

