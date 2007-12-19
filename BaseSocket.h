#ifndef _BASESOCKET_H__
#define _BASESOCKET_H__

// If using Windows, then use Winsock
#ifdef WIN32
	#pragma comment(lib, "ws2_32")
	#include <WinSock2.h>
#endif

#ifndef WIN32 // Linux
	#include <sys/socket.h>
	#include <sys/types.h>
	#include <sys/ioctl.h>
	#include <sys/un.h>
	#include <arpa/inet.h>      /* inet_ntoa() to format IP address */
	#include <netinet/in.h>     /* in_addr structure */
	#include <netdb.h>
	#include <errno.h>
	#ifndef INVALID_SOCKET
		#define INVALID_SOCKET -1
	#endif
	#ifndef SOCKET_ERROR	
		#define SOCKET_ERROR -1
	#endif
	#define SOCKET int
#endif
#include "SocketException.h"
#include <vector>
class CBaseSocket
{
public:
	CBaseSocket(int nSocketType, int nProtocol);
	CBaseSocket(SOCKET rNewSocket);
	~CBaseSocket(void);
public:
	std::string GetPeerName();
	// Used to send test with
	void operator << (std::string Line);
	// Used to send binary data
	void operator << (std::vector<unsigned char> Data);
	// Used to recieve data
	void operator >> (std::string &Line);
	// Used to recieve data
	void operator >> (std::vector<unsigned char> &Data);
	std::string ReceiveBytes(void *argPtr, void (*callback)(void *arg, size_t TotalByteCount), int amountBytes); 
	void SendBytes(const char *s, int length);
	// Used for FD_SET to return the socket
	operator SOCKET() const {
		return rSocket;
	}
protected:
	SOCKET rSocket;
private:
	static int nAmountSockets;
protected:
	inline std::string GetSocketError()
	{
		std::string szErrorDesc;
#ifdef WIN32
		char szErrormsg[512];
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, WSAGetLastError(), 0, szErrormsg, 511, NULL);
		szErrorDesc = szErrormsg;
#else
		szErrorDesc = strerror(errno);
#endif
		return szErrorDesc;
	}
	inline int GetSocketErrorCode()
	{
#ifdef WIN32
		return WSAGetLastError();
#else
		return errno;
#endif
	}
};

#endif
