/***************************************************************************
 *            MD5new.h
 *
 *  Simple C++ header file for Optimized md5 hash routine
 *  Mon Nov  5 19:36:44 2007
 *  
 *  <alexis.dagues@gmail.com>
 ****************************************************************************/

#ifndef _MD5NEW_H
#define _MD5NEW_H
#ifndef __WIN32__
	#define uint32 unsigned long int
#endif

void MD5_NEW( unsigned char * pData, int len, unsigned char * pDigest);


#endif /* _MD5NEW_H */
