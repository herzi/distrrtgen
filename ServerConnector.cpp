#include "ServerConnector.h"
#include <sstream>
#include <sys/stat.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>
#include "config.h"
#include "tinyxml.h"
#define SERVER_PORT 80
#define SERVER_NAME "http://distributed.freerainbowtables.com/communicate.php"
#define UPLOAD_URL "http://distributed.freerainbowtables.com/upload.php"
#define _FILE_OFFSET_BITS 64
#ifndef VERSION
	#define VERSION "3.0 LX"
#endif

enum TALKATIVE
{
	TK_ALL = 0,
	TK_WARNINGS,
	TK_ERRORS
};

int nTalkative = TK_ALL;

struct MemoryStruct {
    char *memory;
    size_t size;
};

// There might be a realloc() out there that doesn't like reallocing
// NULL pointers, so we take care of it here */
static void *myrealloc(void *ptr, size_t size)
{
    if(ptr)
      return realloc(ptr, size);
    else
      return malloc(size);
}

// Static function to retrieve server XML answers on HTTP protocol - used with CURL
static size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)data;
  
    mem->memory = (char *)myrealloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory) {
      memcpy(&(mem->memory[mem->size]), ptr, realsize);
      mem->size += realsize;
      mem->memory[mem->size] = 0;
    }
    return realsize;
}

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
	// deprecated since we use HTTP protocol
	// TODO : Add a simple http connect to verify server is up...
	return true;
}

void ServerConnector::Disconnect()
{
	//commented by alesc <alexis.dagues@gmail.com> no need with HTTP
	//std::ostringstream sDisc; // Send a disconnection packet
	//sDisc << (unsigned char)0xfc << (unsigned char)0x07 << (unsigned char)0x00 << (unsigned char)0x00;
	//*s << sDisc.str();
	delete s;
	s = NULL;
}
int ServerConnector::Login(std::string sUsername, std::string sPassword, std::string sHostname, int nClientID, double nFrequency)
{
	try
	{
		// build xml stream to logon the server
		CURL *curl;
  		CURLcode res;
  		struct curl_slist *headers = NULL;
  		headers = curl_slist_append(headers, "Content-Type: text/xml");
  		TiXmlDocument doc;
  		TiXmlElement* client;
  		client = new TiXmlElement( "distrrtgenclient" );
  		//if (nClientID==0) std::cout<<"This machine has no ClientID" << std::endl;
		client->SetAttribute("clientid", nClientID);
  		client->SetAttribute("name", sHostname.c_str());
  		client->SetAttribute("version", VERSION);
  
  		TiXmlElement* cpus;
  		TiXmlElement* cpu;
  		cpus = new TiXmlElement( "cpus" );
  		cpu  = new TiXmlElement( "cpu" );		
  		cpu->SetAttribute("currentspeed", (int)nFrequency);
  		cpu->SetAttribute("maxspeed", (int)nFrequency);
  		cpus->LinkEndChild(cpu);
  		TiXmlElement* credentials;
  		credentials = new TiXmlElement( "credentials");
  		TiXmlElement* username;
  		username = new TiXmlElement("username");
  		username->LinkEndChild(new TiXmlText(sUsername.c_str()));
  		TiXmlElement* password;
  		password = new TiXmlElement("password");
  		password->LinkEndChild(new TiXmlText(sPassword.c_str()));
  		credentials->LinkEndChild(username);
  		credentials->LinkEndChild(password);
  		client->LinkEndChild(cpus);
  		client->LinkEndChild(credentials);
  
  		doc.LinkEndChild(client);
  		//doc.SaveFile("test1.xml");
  		//doc.Print();
  		TiXmlPrinter login;
  		doc.Accept(&login);
  		//FILE* xmlresponse;
		//xmlresponse = fopen("result.xml", "wb");
 		struct MemoryStruct xmlresponse;
  		xmlresponse.memory=NULL; /* we expect realloc(NULL, size) to work */
  		xmlresponse.size = 0;    /* no data at this point */
		
  		curl = curl_easy_init();
  		if(curl) {
    		curl_easy_setopt(curl, CURLOPT_URL, SERVER_NAME);
    		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&xmlresponse);
    		curl_easy_setopt(curl, CURLOPT_POST, 1);
    		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, login.CStr());
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    		res = curl_easy_perform(curl);
    		curl_easy_cleanup(curl);
  		}
		
  		//std::cout << xmlresponse.memory << std::endl;
		TiXmlDocument xLoginAnswer;
		if(xLoginAnswer.Parse(xmlresponse.memory))
		{
			//std::cout << "Parsing OK" << std::endl;
		}
		else
		{
			throw new ConnectionException(EL_ERROR, "Invalid XML response received from server");
		}
		//Check if the server gave a clientId and we return it.
		//Else we continue login process.
		//All the parsing code has to be reviewed
		//we never check element name before attribute querying...
		
		TiXmlElement * pElem = xLoginAnswer.FirstChildElement("distrrtgenserver");
	 	TiXmlAttribute * pAttrib;
		
		if( pElem->FirstChildElement("client") )
		{
			pElem = pElem->FirstChildElement("client");
			std::cout << "This client was unregistered." << std::endl;
			return atoi(pElem->Attribute("id"));
			throw new ConnectionException(EL_ERROR, "This client was unregistered.");
		}
		else
		{			
			if ( pElem->FirstChildElement("loginresult") )
			{
		    	pElem = xLoginAnswer.FirstChildElement("distrrtgenserver")->FirstChildElement("loginresult");
				bLoggedIn = atoi(pElem->Attribute("status"));
				if (bLoggedIn!=1)
				{
					throw new ConnectionException(EL_ERROR, pElem->GetText());
				}
			}
			else
			{
				throw new ConnectionException(EL_ERROR, "Error parsing XML response");
			}
		}
		
		if(xmlresponse.memory)
          free(xmlresponse.memory);
		
		if (bLoggedIn==1) return true;
			else return false;
		
	}
	catch(SocketException *ex)
	{		
		std::cout << "Exception catched" << std::endl;
		throw new ConnectionException(EL_NOTICE, ex->GetErrorMessage());
		return false;
	}
	return false;
}

int ServerConnector::RequestWork(stWorkInfo *stWork, std::string sUsername, std::string sPassword, std::string sHostname, int nClientID, double nFrequency)
{
	try
	{
		// build xml stream to request workunit from the server
		CURL *curl;
  		CURLcode res;
  		struct curl_slist *headers = NULL;
  		headers = curl_slist_append(headers, "Content-Type: text/xml");
  		TiXmlDocument doc;
  		TiXmlElement* client;
  		client = new TiXmlElement( "distrrtgenclient" );
  		if (nClientID==0) std::cout<<"This machine has no ClientID" << std::endl;
		client->SetAttribute("clientid", nClientID);
  		client->SetAttribute("name", sHostname.c_str());
  		client->SetAttribute("version", VERSION);
  
  		TiXmlElement* cpus;
  		TiXmlElement* cpu;
  		cpus = new TiXmlElement( "cpus" );
  		cpu  = new TiXmlElement( "cpu" );		
  		cpu->SetAttribute("currentspeed", (int)nFrequency);
  		cpu->SetAttribute("maxspeed", (int)nFrequency);
  		cpus->LinkEndChild(cpu);
  		TiXmlElement* credentials;
  		credentials = new TiXmlElement( "credentials");
  		TiXmlElement* username;
  		username = new TiXmlElement("username");
  		username->LinkEndChild(new TiXmlText(sUsername.c_str()));
  		TiXmlElement* password;
  		password = new TiXmlElement("password");
  		password->LinkEndChild(new TiXmlText(sPassword.c_str()));
  		credentials->LinkEndChild(username);
  		credentials->LinkEndChild(password);
		//We add requestwork element in XML query
		TiXmlElement* action;
		action = new TiXmlElement("action");
		action->LinkEndChild(new TiXmlText("requestwork"));
  		client->LinkEndChild(cpus);
  		client->LinkEndChild(credentials);
  		client->LinkEndChild(action); //last child is requestwork
  		doc.LinkEndChild(client);
  		
		//doc.Print();
  		
		TiXmlPrinter login;
  		doc.Accept(&login);
  		
		struct MemoryStruct xmlresponse;
  		xmlresponse.memory=NULL; /* we expect realloc(NULL, size) to work */
  		xmlresponse.size = 0;    /* no data at this point */
		
  		curl = curl_easy_init();
  		if(curl) {
    		curl_easy_setopt(curl, CURLOPT_URL, SERVER_NAME);
    		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&xmlresponse);
    		curl_easy_setopt(curl, CURLOPT_POST, 1);
    		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, login.CStr());
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    		res = curl_easy_perform(curl);
    		curl_easy_cleanup(curl);
  		}
  		TiXmlDocument xLoginAnswer;
		if(xLoginAnswer.Parse(xmlresponse.memory))
		{
			//std::cout << xmlresponse.memory << std::endl;
		}
		else
		{
			throw new ConnectionException(EL_ERROR, "Invalid XML response received from server");
		}
		//Check if the server gave a clientId and we return it.
		//Else we continue login process.
		//All the parsing code has to be reviewed
		//we never check element name before attribute querying...
		
		TiXmlElement * pElem = xLoginAnswer.FirstChildElement()->FirstChildElement("loginresult");
	 	TiXmlAttribute * pAttrib = pElem->FirstAttribute();
		if (!strcmp(pAttrib->Name(),"status"))
		{
		   	bLoggedIn = atoi(pAttrib->Value());
			if (bLoggedIn != 1)
			{
				throw new ConnectionException(EL_ERROR, pElem->GetText());
			}
		}
		else
		{
			throw new ConnectionException(EL_ERROR, "Error parsing XML response");
		}
		if (bLoggedIn) std::cout << " " << std::endl;
		//We are positionning to workunit element
		pElem = pElem->NextSiblingElement("workunit");
		if (!pElem) std::cout << "Err. retrieving workunit element" << std::endl;
		else std::cout << " " << std::endl;
		
		//std::cout << " Continue Parsing XML file" << std::endl;
		if(pElem->Attribute("partid"))
		{
			unsigned int tmp;
			std::istringstream str_stream(pElem->Attribute("partid"));
			str_stream >> tmp;
			stWork->nPartID = tmp;
			std::cout << "Received PartID n° : " << stWork->nPartID << std::endl; 
		}
		else std::cout << "not partid" << std::endl;
		
		if (stWork->nPartID == 0)
		{
			std::cout << pElem->GetText() << std::endl;
			throw new ConnectionException(EL_ERROR, pElem->GetText());
		}
		else
		{
			std::cout << "Loading WorkUnit Details" << std::endl;
			unsigned int itmp;
			
			if (pElem->Attribute("chaincount"))
			{			
				std::istringstream str_stream(pElem->Attribute("chaincount"));
				str_stream >> itmp;
				stWork->nChainCount = itmp;
			}
			
			if (pElem->Attribute("minletters"))
			{	
				std::istringstream str_stream(pElem->Attribute("minletters"));
				str_stream >> itmp;
				stWork->nMinLetters = itmp;
			}
			
			if (pElem->Attribute("maxletters"))
			{
				std::istringstream str_stream(pElem->Attribute("maxletters"));
				str_stream >> itmp;
				stWork->nMaxLetters = itmp;
			}
			if (pElem->Attribute("index"))
			{
				std::istringstream str_stream(pElem->Attribute("index"));
				str_stream >> itmp;
				stWork->nOffset = itmp;
			}
			if (pElem->Attribute("chainlength"))
			{
				std::istringstream str_stream(pElem->Attribute("chainlength"));
				str_stream >> itmp;
				stWork->nChainLength = itmp;
			}
			
			stWork->sHashRoutine = pElem->Attribute("type");
			stWork->sCharset = pElem->Attribute("charset");
			stWork->sSalt = pElem->Attribute("salt");
			
		}		
					
		if(xmlresponse.memory)
          free(xmlresponse.memory);
	
		return true;
		
	}
	catch(SocketException* ex)
	{
		throw new ConnectionException(EL_NOTICE, ex->GetErrorMessage());
	}
	return false;

}

int ServerConnector::SendFinishedWork(int nPartID, std::string Filename, std::string sUsername, std::string sPassword)
{
	try
	{
		struct curl_httppost *post=NULL;
 		struct curl_httppost *last=NULL;
		struct curl_slist *headers=NULL;
		struct MemoryStruct xmlresponse;
  		CURL *curl;
		CURLcode res;
		std::stringstream szPartname;
		std::stringstream szUrlpost;
		std::string sPartname;
		std::string sUrlpost;
		
		//Limitrate seems to not work with POST FORM DATA
		curl_off_t limitrate = 0;
		
		xmlresponse.memory=NULL; 
  		xmlresponse.size = 0;    
		
		// build filename
		szPartname << nPartID << ".part";
		sPartname = szPartname.str();
		
		//build url upload.php?username=blablabla
		szUrlpost << UPLOAD_URL << "?username=" << sUsername << "&password=" << sPassword;
		sUrlpost = szUrlpost.str();
		
		curl = curl_easy_init();
		if (curl)
		{
			curl_formadd(&post, &last,
              CURLFORM_COPYNAME, "name",
              CURLFORM_COPYCONTENTS, "file", CURLFORM_END);
			curl_formadd(&post, &last,
	 	      CURLFORM_COPYNAME, "filename",
              CURLFORM_COPYCONTENTS, sPartname.c_str(), CURLFORM_END); 			 			
			curl_formadd(&post, &last,
              CURLFORM_COPYNAME, "file",
              CURLFORM_FILE, Filename.c_str(), CURLFORM_END); //Path of the part file
			
			// Disable HTTP1.1 Expect:
			headers = curl_slist_append(headers, "Expect:");

 			// Header content-disposition
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers); 
			
			if(nTalkative <= TK_ALL)
				std::cout << "Uploading part : " << sPartname.c_str() << std::endl;
			// Set the form info
 			curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, (long)limitrate);
			curl_easy_setopt(curl, CURLOPT_POST, 1);
			curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);
			curl_easy_setopt(curl, CURLOPT_URL, sUrlpost.c_str());
 			curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&xmlresponse);
    		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
/* CURLOPT_MAX_SEND_SPEED_LARGE and CURLOPT_MAX_RECV_SPEED_LARGE were
 * introduced in 7.15.5:
 * http://curl.netmirror.org/changes.html */
#if LIBCURL_VERSION_NUM >= 0x070f05
			curl_easy_setopt(curl, CURLOPT_MAX_SEND_SPEED_LARGE, limitrate);
			curl_easy_setopt(curl, CURLOPT_MAX_RECV_SPEED_LARGE, limitrate);
#endif
			curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, 0);
			curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, 0);
			
			if(nTalkative <= TK_ALL)
				curl_easy_setopt(curl, CURLOPT_NOPROGRESS, false);
			//curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress);
			//curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, &Bar);
			
			
			res = curl_easy_perform(curl); /* post away! */
			if (res == 0)
			{
				//std::cout << "Uploading of Part ID n° " << nPartID << " succeded" << std::endl;
				// Retrieve Server XML answer
				TiXmlDocument xUploadAnswer;
				if(xUploadAnswer.Parse(xmlresponse.memory))
				{
					TiXmlElement * pElem;
					pElem = xUploadAnswer.FirstChildElement();
					//printf("%s\n",xmlresponse); //Debug purpose
					if(pElem->FirstChildElement("ok"))
					{
						curl_formfree(post);
						curl_easy_cleanup(curl);
						if(xmlresponse.memory)
          					free(xmlresponse.memory);
						return TRANSFER_OK;
					}
	 				else if(pElem->FirstChildElement("error"))
					{
						curl_formfree(post);
						curl_easy_cleanup(curl);
						if(xmlresponse.memory)
          					free(xmlresponse.memory);
						std::cout << pElem->FirstChildElement("error")->GetText() << std::endl;
						return TRANSFER_NOTREGISTERED;
						//throw new ConnectionException(EL_ERROR, pElem->FirstChildElement("error")->GetText());
					}
					else
					{
						throw new ConnectionException(EL_ERROR, "Unreadable error from server side");
					}	
				}
				else
				{
					throw new ConnectionException(EL_ERROR, "Invalid XML response received from server");
					curl_formfree(post);
					curl_easy_cleanup(curl);
					if(xmlresponse.memory)
          				free(xmlresponse.memory);
					return TRANSFER_GENERAL_ERROR;
				}
				curl_formfree(post);
				curl_easy_cleanup(curl);
				if(xmlresponse.memory)
          			free(xmlresponse.memory);
				return TRANSFER_GENERAL_ERROR;
			}
			else
			{
				curl_formfree(post);
				curl_easy_cleanup(curl);
				if(xmlresponse.memory)
          			free(xmlresponse.memory);
				std::cout << res << std::endl;
				throw new ConnectionException(EL_ERROR, "Error while uploading part content");
			}
		}
		else
		{	
			curl_formfree(post);
			curl_easy_cleanup(curl);
				if(xmlresponse.memory)
          			free(xmlresponse.memory);			
			throw new ConnectionException(EL_ERROR, "Error while creating CURL object");
		}		
 		// free the post data again
 		curl_formfree(post);
		curl_easy_cleanup(curl);
		if(xmlresponse.memory)
          free(xmlresponse.memory);
		return TRANSFER_GENERAL_ERROR;
	}
	catch(SocketException* ex)
	{
		throw new ConnectionException(EL_NOTICE, ex->GetErrorMessage());
	}
	return TRANSFER_GENERAL_ERROR;
}
