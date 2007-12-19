
#include "SocketException.h"

SocketException::SocketException(int nErrorCode, std::string szErrorMessage) : Exception(szErrorMessage)
{
	this->nErrorCode = nErrorCode;
}

SocketException::~SocketException(void)
{
}
