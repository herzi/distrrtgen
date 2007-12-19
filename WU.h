/***************************************************************************
 *            WU.h
 *
 *  Sun Nov 18 17:19:02 2007
 *  Copyright  2007  alesc
 *  <alexis.dagues@gmail.com>
 * 	This class manage WorkUnit details folowing the XML struct.
 *  <workunit partid="83519" chaincount="500000" type="md5" charset="numeric"
 *  minletters="1" maxletters="12" index="0" chainlength="10000" salt="" expiration="2007-11-05 19:57:57"/>
 ****************************************************************************/

#ifndef _WU_H
#define _WU_H

#include <string>
#include <time.h>

class wu{
        public:
			int partid;
			int chaincount;
			std::string type;
			std::string charset;
			int minletters;
			int maxletters;
			int index;
			int chainlength;
			std::string salt;
			std::string expiration;
			int status;
};

#endif /* _WU_H */
