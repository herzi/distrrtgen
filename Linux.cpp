/* This file is part of distrrtgen
 *
 * AUTHORS
 *     Sven Herzberg
 *
 * Copyright (C) 2008  Sven Herzberg
 *
 * This work is provided "as is"; redistribution and modification
 * in whole or in part, in any medium, physical or electronic is
 * permitted without restriction.
 *
 * This work is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * In no event shall the authors or contributors be liable for any
 * direct, indirect, incidental, special, exemplary, or consequential
 * damages (including, but not limited to, procurement of substitute
 * goods or services; loss of use, data, or profits; or business
 * interruption) however caused and on any theory of liability, whether
 * in contract, strict liability, or tort (including negligence or
 * otherwise) arising in any way out of the use of this software, even
 * if advised of the possibility of such damage.
 */

#include "Platform.h"

#ifdef __linux

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define CPU_INFO_FILENAME "/proc/cpuinfo"

int
Platform::getProcessorCount (void)
{
	return sysconf(_SC_NPROCESSORS_ONLN);
}

double
Platform::getProcessorFrequency (void)
{
	FILE* F;
	const char* cpuprefix = "cpu MHz";
	char cpuline[300+1];
	char* pos;
	int ok = 0;
	double nFrequency = 0.0;

	// open cpuinfo system file
	F = fopen(CPU_INFO_FILENAME,"r");
	if (!F) return 0;
	
	//read lines
	while (!feof(F))
  	{
    fgets (cpuline, sizeof(cpuline), F);
	//test if it's a processor id line
	/* deprecated
	if (!strncmp(cpuline, cpunumber, strlen(cpunumber)))
	{
		
	}
    */
    // test if it's the frequency line
    if (!strncmp(cpuline, cpuprefix, strlen(cpuprefix)))
    	{
      		// Yes, grep the frequency
      		pos = strrchr (cpuline, ':') +2;
      		if (!pos) break;
      		if (pos[strlen(pos)-1] == '\n') pos[strlen(pos)-1] = '\0';
      		strcpy (cpuline, pos);
      		strcat (cpuline,"e6");
      		nFrequency = atof (cpuline)/1000000;
      		ok = 1;
      		//break;  //bug : http://www.freerainbowtables.com/phpBB3/viewtopic.php?f=4&p=916&sid=53804aa79a7bc4bb06cff38481889cf7#p909
    	}
  	}

	return nFrequency;
}

#endif

