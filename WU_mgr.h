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

class wu_mgr{
    public:
        int read(const std::string&);
       /* int modify(const string&, const string&, const string&);
        int add(const wu&);
        int remove(const string&);*/
    private:
        std::list<wu> wu_list;
        TiXmlDocument *doc;
        std::string xml_filename;
};

#endif /* _WUMGR_H */
