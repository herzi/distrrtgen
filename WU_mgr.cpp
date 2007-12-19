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
		//elem->QueryIntAttribute("maxletters", &unit.maxletters);
        unit.salt = elem->Attribute("salt");
		unit.expiration = elem->Attribute("expiration");
		wu_list.push_back(unit);
		std::cout << unit.partid << " : " << unit.type << " " << unit.charset << unit.expiration << std::endl;
        struct tm tm;
		time_t t;

		if (strptime(unit.expiration.c_str(), "%Y-%m-%d %T", &tm) == NULL)
    	/* Handle error */;


		printf("year: %d; month: %d; day: %d;\n",
        	tm.tm_year, tm.tm_mon, tm.tm_mday);
		printf("hour: %d; minute: %d; second: %d\n",
        	tm.tm_hour, tm.tm_min, tm.tm_sec);
		printf("week day: %d; year day: %d\n", tm.tm_wday, tm.tm_yday);
		
		tm.tm_isdst = -1;      /* Not set by strptime(); tells mktime()
                          to determine whether daylight saving time
                          is in effect */
		t = mktime(&tm);
		if (t == -1)
			exit(0);
		
			elem = elem->NextSiblingElement(); // iteration
    	}
	return 0;
}
