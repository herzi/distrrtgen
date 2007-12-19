#include "ClientSocket.h"


CClientSocket::CClientSocket(int nSocketType, int nProtocol, std::string szHost, int nPort) : CBaseSocket(nSocketType, nProtocol)
{
	std::string error;

	hostent *he;
	if ((he = gethostbyname(szHost.c_str())) == 0) 
	{
		std::ostringstream szError;
		szError << "Error while trying to resolve hostname '" << szHost << "' : " << GetSocketError();
		throw new SocketException(GetSocketErrorCode(), szError.str());
	}

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(nPort);
	addr.sin_addr = *((in_addr *)he->h_addr);
	memset(&(addr.sin_zero), 0, 8); 

	if (connect(rSocket, (sockaddr *) &addr, sizeof(sockaddr)) == SOCKET_ERROR) 
	{
		std::ostringstream szError;
		szError << "Error while trying to connect to '" << szHost << "' : " << GetSocketError();
		throw new SocketException(GetSocketErrorCode(), szError.str());
	}

}
