#include "ServerConnector.h"
#include <sstream>
#include <sys/stat.h>
#include <fstream>
#include <iostream>
#include <vector>
#include "config.h"
#define SERVER_PORT 23984
#define SERVER_NAME "distributed.freerainbowtables.com"
ServerConnector::ServerConnector(void)
{
	bLoggedIn = false;
	s = NULL;
}

ServerConnector::~ServerConnector(void)
{
	if(s != NULL)
	{
		Disconnect();
		delete s;
	}
}

int ServerConnector::Connect()
{
	try
	{
		std::vector<unsigned char> vVersion;
		vVersion.push_back(0xfc);
		vVersion.push_back(0x01);
		vVersion.push_back(0x01);
		vVersion.push_back(PROTOCOL_VERSION);

		s = new CClientSocket(1, IPPROTO_TCP, SERVER_NAME, SERVER_PORT);					
		*s << vVersion;
		std::vector<unsigned char> vVerResponse;
		*s >> vVerResponse;
		if(vVerResponse.size() < 4 || vVerResponse[0] != (unsigned char)0xfc || vVerResponse[1] != (unsigned char)0x01 || vVerResponse[2] != (unsigned char)0x02) 
		{
			throw new ConnectionException(EL_NOTICE, "Invalid response recieved from server");
		}
		if(vVerResponse[3] == (unsigned char)0x00 || vVerResponse[3] == (unsigned char)0x02)
		{  // If 0x02, dont show the message, but just show a standard message
			throw new ConnectionException(EL_ERROR, "DistrRTgen client is out of date. Please go to www.freerainbowtables.com and download the newest version");
		}
		return true;
	}
	catch(SocketException *ex)
	{		
		throw new ConnectionException(EL_NOTICE, ex->GetErrorMessage());
	}
	return false;
}
void ServerConnector::Disconnect()
{
	std::ostringstream sDisc; // Send a disconnection packet
	sDisc << (unsigned char)0xfc << (unsigned char)0x07 << (unsigned char)0x00 << (unsigned char)0x00;
	*s << sDisc.str();
	delete s;
	s = NULL;
}
int ServerConnector::Login(std::string sUsername, std::string sPassword, int nClientID)
{
	try
	{
		nClientID = htonl(nClientID); // Convert it to network format
		std::ostringstream sLogin;
		sLogin << (unsigned char)0xfc << (unsigned char)0x02 << (unsigned char)0x01 << (unsigned char)0x00 << sUsername << (unsigned char)0x00 << sPassword  << (unsigned char)0x00;
		sLogin.write((const char*)&nClientID, 4);
		*s << sLogin.str();
		//std::string r;
		std::vector<unsigned char> vLogin;
		*s >> vLogin;
		if(vLogin.size() < 4 || vLogin[0] != (unsigned char)0xfc || vLogin[1] != (unsigned char)0x02 || vLogin[2] != (unsigned char)0x02) 
		{
			throw new ConnectionException(EL_NOTICE, "Invalid response recieved from server");
		}
		if(vLogin[3] == (unsigned char)0x01)
		{
			if(nClientID == 0)
			{ // We need to register a new client..
				std::ostringstream sNewClient;
				int nNumProcessors = 1;
				int nSpeed1 = htonl(2200);
				sNewClient << (unsigned char)0xfc << (unsigned char)0x03 << (unsigned char)0x01 << (unsigned char)0x00;
				*s << sNewClient.str();
				std::string sRecieve;
				*s >> sRecieve;
				const char *ptr = sRecieve.c_str();
				if(ptr[0] != (char)0xfc || ptr[1] != (char)0x03 || ptr[2] != (char)0x02 || ptr[3] != (char)0x01)
				{
					throw new ConnectionException(EL_NOTICE, "Invalid response recieved from server");
				}
				memcpy(&nClientID, &ptr[4], 4);
				nClientID = ntohl(nClientID);
			}
			bLoggedIn = true;
			return nClientID;
		}
		else if(vLogin[3] == (unsigned char)0x02)
		{
			throw new ConnectionException(EL_ERROR, "ClientID is not registered with this user account. Please delete the file ~user/.distrrtgen/.client and try again");
		}
		throw new ConnectionException(EL_ERROR, "Invalid username/password combination");
	}
	catch(SocketException *ex)
	{		
		throw new ConnectionException(EL_NOTICE, ex->GetErrorMessage());
	}
	return false;
}

int ServerConnector::RequestWork(stWorkInfo *stWork)
{
	try
	{
		std::ostringstream szGive;
		szGive << (unsigned char)0xfc << (unsigned char)0x04 << (unsigned char)0x01 << (unsigned char)0x00;
		*s << szGive.str();
		std::string sRecieve;
		*s >> sRecieve;
		char *pRecieved = new char[sRecieve.length()];
		memcpy(pRecieved, sRecieve.c_str(), sRecieve.length());
		if(pRecieved[0] != (char)0xfc || pRecieved[1] != (char)0x04 || pRecieved[2] != (char)0x02)
		{
			delete pRecieved;
			throw new ConnectionException(EL_NOTICE, "Invalid response recieved from server");
		}
		if(pRecieved[3] == (char)0x02)
		{
			throw new ConnectionException(EL_NOTICE, "No work left");
		}
		else if(pRecieved[3] == (char)0x01)
		{
			unsigned int nPartID;
			memcpy(&nPartID, &pRecieved[4], sizeof(int)); // Retrieve the part id from the response
			stWork->nPartID = ntohl(nPartID);

			char *pString = &pRecieved[8]; // Set the pointer to the hash routine location
			unsigned int nHRLength = strlen(pString);
			char *szHashRoutine = new char[nHRLength + 1];
			strcpy(szHashRoutine, pString);	
			stWork->sHashRoutine.assign(szHashRoutine);
			delete szHashRoutine;
			pString += nHRLength + 1; // Advance the pointer to the charset
			if(stWork->sHashRoutine == "mscache" || stWork->sHashRoutine == "halflmchall")
			{
				unsigned int nSaltLength = strlen(pString);
				char *sSalt = new char[nSaltLength + 1];
				strcpy(sSalt, pString);	
				stWork->sSalt.assign(sSalt);
				delete sSalt;
				pString += nSaltLength + 1;				
			}
			unsigned int nCharsetLength = strlen(pString);
			char *szCharset = new char[nCharsetLength + 1];
			strcpy(szCharset, pString);
			stWork->sCharset.assign(szCharset);
			delete szCharset;

			pString += nCharsetLength + 1; // Advance the pointer to the min letters
			stWork->nMinLetters = pString[0];

			pString++;
			stWork->nMaxLetters = pString[0];

			pString++;
			stWork->nOffset = pString[0];

			pString++;
			memcpy(&stWork->nChainLength, pString, 4);
			stWork->nChainLength = ntohl(stWork->nChainLength);

			pString += 4; // Advance the pointer 4 bytes 
			memcpy(&stWork->nChainCount, pString, 4);
			stWork->nChainCount = ntohl(stWork->nChainCount);

			delete pRecieved;
			return true;
		}
		else
		{
			delete pRecieved;
			throw new ConnectionException(EL_NOTICE, "Invalid response recieved from server");
		}
	}
	catch(SocketException* ex)
	{
		throw new ConnectionException(EL_NOTICE, ex->GetErrorMessage());
	}
	return false;

}

int ServerConnector::SendFinishedWork(int nPartID, std::string Filename)
{
	try
	{
		// Load file into memory
		struct stat results;
		stat(Filename.c_str(), &results);
		char *data = new char[results.st_size];
		std::ifstream datafile;
		datafile.open(Filename.c_str(), std::ios::in | std::ios::binary);
		datafile.read(data, results.st_size);
		datafile.close();

		std::ostringstream szFinished;
		nPartID = htonl(nPartID); // Convert to Network format
		szFinished << (unsigned char)0xfc << (unsigned char)0x05 << (unsigned char)0x01 << (unsigned char)0x01;
		szFinished.write((const char*)&nPartID, sizeof(int));
		*s << szFinished.str();

		std::vector<unsigned char> vResponse;
		// Recieve a response whether or not we can start the transfer
		*s >> vResponse;
		if(vResponse.size() < 4 || vResponse[0] != (unsigned char)0xfc || vResponse[1] != (unsigned char)0x05 || vResponse[2] != (unsigned char)0x02)
		{
			throw new ConnectionException(EL_NOTICE, "Invalid response recieved from server");
		}
		if(vResponse[3] == (unsigned char)0x02)
		{
			return TRANSFER_NOTREGISTERED;
		}
		else if(vResponse[3] == (unsigned char)0x01)
		{ // Its okay to start the transfer			
			std::ostringstream sStart, sEnd;
			sStart << (unsigned char)0xfc << (unsigned char)0x05 << (unsigned char)0x01 << (unsigned char)0x02; 
			// Signal the filetransfer starts now
			*s << sStart.str();
//			std::cout << "Sending " << results.st_size << " bytes of data" << std::endl;

			s->SendBytes(data, results.st_size);

			sEnd << (unsigned char)0xfc << (unsigned char)0x05 << (unsigned char)0x01 << (unsigned char)0x03;
			// Signal the filetransfer has ended
			*s << sEnd.str();
			std::vector<unsigned char> vRecieved;
			*s >> vRecieved;
			if(vRecieved[3] == 0x04)
			{
				return TRANSFER_OK;
			}
			return TRANSFER_GENERAL_ERROR;
		}
//		std::cout << r << std::endl;
		// r should be == "RECIEVED"
		return TRANSFER_OK;
	}
	catch(SocketException* ex)
	{
		throw new ConnectionException(EL_NOTICE, ex->GetErrorMessage());
	}
	return TRANSFER_GENERAL_ERROR;
}


