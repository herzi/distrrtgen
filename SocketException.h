#ifndef _SOCKET_EXCEPTION_H__
#define _SOCKET_EXCEPTION_H__

#include "Exception.h"
#include <string>
class SocketException :
	public Exception
{
public:
	SocketException(int nErrorCode, std::string szErrorMessage);
public:
	~SocketException(void);
	int GetErrorCode() { return nErrorCode; }
private:
	int nErrorCode;
};
#endif
