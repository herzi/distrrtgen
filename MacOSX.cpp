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

#ifdef __APPLE_CC__

#include <CoreServices/CoreServices.h>
#include <sys/types.h>
#include <sys/sysctl.h>

int
Platform::getProcessorCount (void)
{
	return MPProcessors ();
}

double
Platform::getProcessorFrequency (void)
{
	uint64_t nFrequency64 = 0;
	size_t   nFrequency64_len = sizeof (nFrequency64);
	if (0 != sysctlbyname ("hw.cpufrequency", &nFrequency64,&nFrequency64_len,NULL, 0))
	{
		perror ("error reading frequency");
		return 0.0;
	}
	return nFrequency64 / 1000000;
}

#endif

