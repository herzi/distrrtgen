// rtgen_client.cpp : Defines the entry point for the console application.
//

#ifdef _WIN32
	#pragma warning(disable : 4786)
#endif
#include "config.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h>
#include <stdio.h>
#include "ServerConnector.h"
#include "RainbowTableGenerator.h"
#ifndef WIN32
#include <pwd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h> // For mkdir()
#endif
#define CLIENT_WAIT_TIME_SECONDS 600 // Wait 10 min and try again
enum TALKATIVE
{
	TK_ALL = 0,
	TK_WARNINGS,
	TK_ERRORS
};
int main(int argc, char* argv[])
{	

	std::string sUsername;
	std::string sPassword;
	std::string sProcessors;
	std::string sHomedir;
	int nNumProcessors = 0;
	int nClientID;
    int nTalkative = TK_ALL;
	if(argc > 1)
	{
		if(strcmp(argv[1], "-q") == 0)
		{
			nTalkative = TK_WARNINGS;
		}
		else if(strcmp(argv[1], "-Q") == 0)
		{
			nTalkative = TK_ERRORS;
		}
	}
	// First load the client identification information
	std::ostringstream sClientInfo;
#ifndef WIN32
	struct passwd *userinfo;
	userinfo = getpwuid(getuid());
	sHomedir = userinfo->pw_dir;
	sClientInfo << sHomedir  << "/.distrrtgen/";
#endif
	sClientInfo << ".client";
	std::fstream fClientInfo(sClientInfo.str().c_str(), std::fstream::in);

	if(fClientInfo.is_open() == false) nClientID = 0;
	else 
	{
		fClientInfo >> nClientID;
		fClientInfo.close();
	}
	// Then load the client configuration
	std::ostringstream sConf;
#ifndef WIN32
	sConf << sHomedir << "/.distrrtgen/";
#endif
	sConf << "distrrtgen.conf";
	std::fstream fConfig(sConf.str().c_str(), std::ifstream::in);
	if(fConfig.is_open() == false)
	{
		std::cout << sConf.str() << " could not be opened - " << strerror(errno) << std::endl;
		return 1;
	}
//	std::string sUsername, sPassword;
	fConfig >> sUsername;
	fConfig >> sPassword;
	fConfig >> sProcessors;
	fConfig.close();
	sUsername = sUsername.substr(9);
	sPassword = sPassword.substr(9);
	if(sUsername == "")
	{
		std::cout << "No username configured in " << sConf.str() << std::endl;
		return 1;
	}
	if(sPassword == "")
	{
		std::cout << "No password configured in " << sConf.str() << std::endl;
		return 1;
	}
	if(sProcessors.substr(7).length() > 0) // Check if processor count is manually configured
	{
		nNumProcessors = atoi(sProcessors.substr(7).c_str());
	}
	// If numprocessors is 0, RainbowTableGenerator.cpp will try to detect it itself

	// Check to see if there is something to resume from
	stWorkInfo stWork;
	std::ostringstream sResumeFile;
#ifndef WIN32
	sResumeFile << sHomedir << "/.distrrtgen/";
#endif
	sResumeFile << ".resume";
	FILE *file = fopen(sResumeFile.str().c_str(), "rb");
	if(file != NULL)
	{
		/*
		// Bingo.. There is a resume file.
		fread(&stWork, sizeof(unsigned int), 6, file);
		char buf[8096];
		memset(buf, 0x00, sizeof(buf));
		fread(&buf[0], sizeof(buf), 1, file);
		fclose(file);
		char szCharset[8096], szHR[8096];
		strcpy(&szCharset[0], &buf[0]);
		stWork.sCharset.assign(szCharset);
		const char *pHR = strchr(&buf[0], 0x00);
		pHR++;
		strcpy(&szHR[0], pHR);
		stWork.sHashRoutine.assign(szHR);
		pHR = strchr(pHR, 0x00);
		pHR++;
		strcpy(&szHR[0], pHR);
		stWork.sSalt.assign(szHR);
		if(nTalkative <= TK_ALL)
			std::cout << "Continuing interrupted computations..." << std::endl;
			*/
	}
	try
	{
		if(nTalkative <= TK_ALL)
			std::cout << "Initializing DistrRTgen" << std::endl;
		CRainbowTableGenerator *pGenerator = new CRainbowTableGenerator(nNumProcessors);
		if(nTalkative <= TK_ALL)
			std::cout << "Generating using " << pGenerator->GetProcessorCount() << " processor(s)..." << std::endl;
		ServerConnector *Con = new ServerConnector();				

		while(1)
		{

			try
			{
				if(nClientID == 0) // This client doesn't have an ID. 
				{   // We connect to the server and register ourself
					//ServerConnector *Con = new ServerConnector();
					if(nTalkative <= TK_ALL)
						std::cout << "Connecting to server to perform first time registration...";
					Con->Connect();
					if(nTalkative <= TK_ALL)
					{
						std::cout << "OK" << std::endl;
						std::cout << "Performing logon...";
					}
					nClientID = Con->Login(sUsername, sPassword, nClientID);
					if(nTalkative <= TK_ALL)
						std::cout << "OK" << std::endl;
					std::fstream fClient(sClientInfo.str().c_str(), std::fstream::out);
					if(fClient.is_open() == false)
					{
						std::cout << "Could not open " << sClientInfo.str() << " for writing" << std::endl;
						nClientID = 0;
					}
					else 
					{
						fClient << nClientID;
						fClient.close();
					}
					Con->Disconnect();
				}

				// If there is no work to do, request some!
				if(stWork.sCharset == "")
				{
					if(nTalkative <= TK_ALL)
						std::cout << "Connecting to server...";
					Con->Connect();
					if(nTalkative <= TK_ALL)
					{
						std::cout << "OK" << std::endl;
						std::cout << "Performing logon...";
					}
					Con->Login(sUsername, sPassword, nClientID);
					if(nTalkative <= TK_ALL)
					{
						std::cout << "OK" << std::endl;
						std::cout << "Requesting work...";
					}
					Con->RequestWork(&stWork);
					if(nTalkative <= TK_ALL)
						std::cout << "work recieved!" << std::endl;
					FILE *fileResume = fopen(sResumeFile.str().c_str(), "wb");
					if(fileResume == NULL)
					{
						std::cout << "Unable to open " << sResumeFile.str() << " for writing" << std::endl;
						return 1;
					}
					fwrite(&stWork, sizeof(unsigned int), 6, fileResume); // Write the 6 unsigned ints
					fwrite(stWork.sCharset.c_str(), stWork.sCharset.length(), 1, fileResume);
					fputc(0x00, fileResume);
					fwrite(stWork.sHashRoutine.c_str(), stWork.sHashRoutine.length(), 1, fileResume);
					fclose(fileResume);
					Con->Disconnect();
				}
				std::stringstream szFileName;
#ifdef WIN32
				szFileName << stWork.nPartID << ".rt";
#else
				szFileName << sHomedir << "/.distrrtgen/" << stWork.nPartID << ".rt"; // Store it in the users home directory
#endif
				// do the work
				int nReturn;
				if(nTalkative <= TK_ALL)
					std::cout << "Starting multithreaded rainbowtable generator..." << std::endl;
#ifndef WIN32
				if(nTalkative >= TK_WARNINGS)
					std::freopen("/dev/null", "w", stdout);	
#endif				

				if((nReturn = pGenerator->CalculateTable(szFileName.str(), stWork.nChainCount, stWork.sHashRoutine, stWork.sCharset, stWork.nMinLetters, stWork.nMaxLetters, stWork.nOffset, stWork.nChainLength, stWork.sSalt)) != 0)
				{
					if(nTalkative >= TK_WARNINGS)
						std::freopen("/dev/stdout", "w", stdout);	
					std::cout << "Error id " << nReturn << " recieved while generating table";
					return nReturn;
				}
#ifndef WIN32
				if(nTalkative >= TK_WARNINGS)
					std::freopen("/dev/stdout", "w", stdout);	
#endif
				if(nTalkative <= TK_ALL)
					std::cout << "Calculations of part " << stWork.nPartID << " completed. Sending data..." << std::endl;
				while(1)
				{
					try
					{
						Con->Connect();
						Con->Login(sUsername, sPassword, nClientID);
						int nResult = Con->SendFinishedWork(stWork.nPartID, szFileName.str());
						Con->Disconnect();
						switch(nResult)			
						{
						case TRANSFER_OK:
							if(nTalkative <= TK_ALL)
								std::cout << "Data delivered!" << std::endl;
							break;
						case TRANSFER_NOTREGISTERED:
							if(nTalkative <= TK_ALL)
								std::cout << "Data was not accepted by the server. Dismissing" << std::endl;
							break;
						case TRANSFER_GENERAL_ERROR:
							if(nTalkative <= TK_ALL)
								std::cout << "Could not transfer data to server. Retrying in " << CLIENT_WAIT_TIME_SECONDS / 60 << " minutes" << std::endl;
							Sleep(CLIENT_WAIT_TIME_SECONDS * 1000);
							continue;
						}
						break;
					}
					catch(SocketException *ex)
					{
						std::cout << "Error connecting to server: " << ex->GetErrorMessage() << ". Retrying in " << CLIENT_WAIT_TIME_SECONDS / 60 << " minutes" << std::endl;
						delete ex;
						Sleep(CLIENT_WAIT_TIME_SECONDS * 1000);
					}
					catch(ConnectionException *ex)
					{
						if(ex->GetErrorLevel() >= nTalkative)
							std::cout << ex->GetErrorMessage() << ". Retrying in " << CLIENT_WAIT_TIME_SECONDS / 60 << " minutes" << std::endl;
						delete ex;
						Sleep(CLIENT_WAIT_TIME_SECONDS * 1000);
					}
				}
				remove(szFileName.str().c_str());		
				stWork.sCharset = ""; // Blank out the charset to indicate the work is complete
				unlink(".resume");
			}
			catch(SocketException *ex)
			{
				if(nTalkative <= TK_WARNINGS)
					std::cout << "Could not connect to server: " << ex->GetErrorMessage() << ". Retrying in " << CLIENT_WAIT_TIME_SECONDS / 60 << " minutes" << std::endl;
				delete ex;
				Sleep(CLIENT_WAIT_TIME_SECONDS * 1000);
			}
			catch(ConnectionException *ex)
			{
				if(ex->GetErrorLevel() >= nTalkative)
					std::cout << ex->GetErrorMessage() << ". Retrying in " << CLIENT_WAIT_TIME_SECONDS / 60 << " minutes" << std::endl;
				delete ex;
				Sleep(CLIENT_WAIT_TIME_SECONDS * 1000);
			}
		}	
	} 
	catch (...) 
	{
		if(nTalkative <= TK_ERRORS)
			std::cerr << "Unhandled exception :(" << std::endl;
	}
  return 0;
}

