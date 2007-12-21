/***************************************************************************
 *            WU_mgr.cpp
 *
 *  Sun Nov 18 16:57:18 2007
 *  Copyright  2007  alesc
 * <alexis.dagues@gmail.com>
 ****************************************************************************/

#include <string>
#include <list>
#include <sstream>
#include <iostream>
#include "WU_mgr.h"
#include "WU.h"
#include "ServerConnector.h" //for stWorkInfo struct

using namespace std;

/*
    method name : read(const string& filename)
	This method read the XML WU file and load work units in a list.
	<workunit partid="83519" chaincount="500000" type="md5" charset="numeric" minletters="1" maxletters="12" index="0" chainlength="10000" salt="" expiration="2007-11-05 19:57:57"/>
*/
int wu_mgr::read(const string& filename){
    wu unit;
    TiXmlElement *elem;
    xml_filename = filename;

    doc = new TiXmlDocument(filename.c_str());
    if(!doc->LoadFile()){
        std::cerr << "error while reading XML file" << std::endl;
        std::cerr << "error #" << doc->ErrorId() << " : " << doc->ErrorDesc() << std::endl;
        return 1;
    }

    TiXmlHandle hdl(doc);
    elem = hdl.FirstChildElement().FirstChildElement().Element();

    if(!elem){
        std::cerr << "Node doesn't exist." << std::endl;
        return 2;
    }

    while (elem){
		//partid="83519" chaincount="500000" type="md5" charset="numeric" minletters="1" maxletters="12" index="0" chainlength="10000" salt="" expiration="2007-11-05 19:57:57"/>
        elem->QueryIntAttribute("partid", &unit.partid);
		elem->QueryIntAttribute("chaincount", &unit.chaincount);
		unit.type = elem->Attribute("type");
        unit.charset = elem->Attribute("charset");
		elem->QueryIntAttribute("minletters", &unit.minletters);
		elem->QueryIntAttribute("maxletters", &unit.maxletters);
		elem->QueryIntAttribute("index", &unit.index);
		elem->QueryIntAttribute("chainlength", &unit.chainlength);
        unit.salt = elem->Attribute("salt");
		unit.expiration = elem->Attribute("expiration");
		
		wu_list.push_back(unit);
		//std::cout << unit.partid << " : " << unit.type << " " << unit.charset << unit.expiration << std::endl;
        
		elem = elem->NextSiblingElement(); // iteration
    }
	return 0;
}

/*
 * Return the size of wu_list (number of WorkUnits)
 */
int wu_mgr::queueSize(void)
{
   	return(wu_list.size());
}

/*
 * This method erase all WU with expiration date passed
 * from memory : wu_list
 */
int wu_mgr::clean(void)
{
	struct tm tm;
	//struct tm tm_now;
	time_t t_wu;
	time_t t_now;
	wu unit;
	/* fetch local time, TODO : GMT management ?*/
	t_now = time(&t_now);
	
    list<wu>::iterator i=wu_list.begin();
	/* clean wu_list */
    while( i != wu_list.end() )
    {        
		if (strptime(i->expiration.c_str(), "%Y-%m-%d %T", &tm) == NULL)
		{
			this->remove(i->partid);
   			//wu_list.erase(i); /* Handle error */	
		}
    	else
		{
			t_wu = mktime(&tm);
			if (t_wu == -1)
			{
				this->remove(i->partid);
				//wu_list.erase(i);
			}
			else if ( t_wu < t_now ) /* Expired */
			{	
				std::cout << "WU is expired" << std::endl;
				this->remove(i->partid);
				//wu_list.erase(i);
			}
			else
			{
				//std::cout << "WU is ready" << std::endl;
				//i++;
			}
		}
	i++;		
	}
	this->read(xml_filename.c_str());
	
	return(0);
}

/*
 * Add a WU to the file WU_list.xml and the wu_list
 */

int wu_mgr::add(const wu& unit){
    TiXmlElement *f = doc->FirstChildElement();

    TiXmlElement new_wu ("workunit");
    new_wu.SetAttribute("partid", unit.partid);
    new_wu.SetAttribute("chaincount", unit.chaincount);
    new_wu.SetAttribute("type", unit.type.c_str());
	new_wu.SetAttribute("charset", unit.charset.c_str());
	new_wu.SetAttribute("minletters", unit.minletters);
	new_wu.SetAttribute("maxletters", unit.maxletters);
	new_wu.SetAttribute("index", unit.index);
	new_wu.SetAttribute("chainlength", unit.chainlength);
	new_wu.SetAttribute("salt", unit.salt.c_str());
	new_wu.SetAttribute("expiration", unit.expiration.c_str());
    f->InsertEndChild(new_wu);
    doc->SaveFile(xml_filename.c_str());

    // mise a jour de la liste
    wu_list.push_back(unit);

    return 0;
}

int wu_mgr::remove(const int partid){
    bool found = false;
	int xmlPartid = 0;
    TiXmlHandle hdl(doc);
    TiXmlElement *elem = hdl.FirstChildElement().FirstChildElement().Element();

    while(elem && !found){
        elem->QueryIntAttribute("partid", &xmlPartid);
		if( xmlPartid == partid){
            found = true;
            break;
        }
        elem = elem->NextSiblingElement();
    }

    if (!found){
        std::cout << "The part "<< partid << " doesn't exist." << std::endl;
        return 1;
    }
    else {
        TiXmlElement *f = doc->FirstChildElement();
        f->RemoveChild(elem);
        doc->SaveFile(xml_filename.c_str());

        return 0;
    }
}

/*
 *  Return true if PartID (arg) is NOT expired.
 */
bool wu_mgr::isNotExpired(const int partid)
{
	struct tm tm;
	//struct tm tm_now;
	time_t t_wu;
	time_t t_now;
	wu unit;
	/* fetch local time, TODO : GMT management ?*/
	t_now = time(&t_now);
	
    list<wu>::iterator i=wu_list.begin();
	/* clean wu_list */
    while( i != wu_list.end() )
    {        
		if (i->partid == partid)
		{
			if (strptime(i->expiration.c_str(), "%Y-%m-%d %T", &tm) == NULL)
			{
				return false;
			}
			else break;
		}
		i++;
	}
		
	t_wu = mktime(&tm);
	if (t_wu == -1)
	{
		return false;
	}
	else if ( t_wu < t_now ) /* Expired */
	{	
		return false;
	}
	else
	{
		return true;
	}
		
	return false; //whenever..	
}
/*
 *  Take the stWorkInfo from RequestWork method and create new WU object in WU_list
 */
int wu_mgr::putWork( const stWorkInfo& stwork)
{
	wu unit;
	unit.partid = stwork.nPartID;
	unit.chaincount = stwork.nChainCount;
	unit.type = stwork.sHashRoutine;
	unit.charset = stwork.sCharset;
	unit.minletters = stwork.nMinLetters;
	unit.maxletters = stwork.nMaxLetters;
	unit.index = stwork.nOffset;
	unit.chainlength = stwork.nChainLength;
	unit.salt = stwork.sSalt;
	unit.expiration = stwork.sExpiration;
	this->add(unit);
	return 0;
}

/*
 *  Get the first WU object in WU_list
 */
stWorkInfo wu_mgr::getWork(void)
{
	stWorkInfo stwork;
	this->clean();
	wu unit = wu_list.front();
	
	stwork.nPartID = unit.partid;
	stwork.nChainCount = unit.chaincount;
	stwork.sHashRoutine = unit.type;
	stwork.sCharset = unit.charset;
	stwork.nMinLetters = unit.minletters;
	stwork.nMaxLetters = unit.maxletters;
	stwork.nOffset = unit.index;
	stwork.nChainLength = unit.chainlength;
	stwork.sSalt = unit.salt;
	stwork.sExpiration = unit.expiration;
	
	return stwork;
}
