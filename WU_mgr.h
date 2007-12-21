/***************************************************************************
 *            WU_mgr.h
 *
 *  Sun Nov 18 17:06:43 2007
 *  Copyright  2007  alesc
 *  <alexis.dagues@gmail.com>
 ****************************************************************************/

#ifndef _WUMGR_H
#define _WUMGR_H

#include <list>
#include "tinyxml.h"
#include "WU.h"
#include "ServerConnector.h"

class wu_mgr{
    public:
        int read(const std::string&);
		int clean(void);
       //int modify(const string&, const string&, const string&);
        int remove(const int);
		bool isNotExpired(const int);
		stWorkInfo getWork(void);
		int putWork(const stWorkInfo&);
		int queueSize(void);
    private:
        std::list<wu> wu_list;
        TiXmlDocument *doc;
        std::string xml_filename;
		int add(const wu&);
};

#endif /* _WUMGR_H */
