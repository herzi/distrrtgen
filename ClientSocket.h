#ifndef _CLIENTSOCKET_H__
#define _CLIENTSOCKET_H__

#include "BaseSocket.h"
#include <sstream>
class CClientSocket :
	public CBaseSocket
{
public:
	CClientSocket(void);
	CClientSocket(int nSocketType, int nProtocol, std::string szHost, int nPort);
public:
};

#endif

