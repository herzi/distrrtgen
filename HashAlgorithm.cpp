/*
   RainbowCrack - a general propose implementation of Philippe Oechslin's faster time-memory trade-off technique.

   Copyright (C) Zhu Shuanglei <shuanglei@hotmail.com>
*/

#include "HashAlgorithm.h"

#include "Public.h"

#include "MD5new.h" //added by alesc <alexis.dagues@gmail.com>

#include <openssl/des.h>
#include <openssl/md2.h>
#include <openssl/md4.h>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <openssl/ripemd.h>
#ifdef _WIN32
	#pragma comment(lib, "libeay32.lib")
#endif
#define MSCACHE_HASH_SIZE 16

void setup_des_key(unsigned char key_56[], des_key_schedule &ks)
{
	des_cblock key;

	key[0] = key_56[0];
	key[1] = (key_56[0] << 7) | (key_56[1] >> 1);
	key[2] = (key_56[1] << 6) | (key_56[2] >> 2);
	key[3] = (key_56[2] << 5) | (key_56[3] >> 3);
	key[4] = (key_56[3] << 4) | (key_56[4] >> 4);
	key[5] = (key_56[4] << 3) | (key_56[5] >> 5);
	key[6] = (key_56[5] << 2) | (key_56[6] >> 6);
	key[7] = (key_56[6] << 1);

	//des_set_odd_parity(&key);
	des_set_key(&key, ks);
}

void HashLM(unsigned char* pPlain, int nPlainLen, unsigned char* pHash, const unsigned char *pUsername, int nSaltLength)
{
	/*
	unsigned char data[7] = {0};
	memcpy(data, pPlain, nPlainLen > 7 ? 7 : nPlainLen);
	*/

	int i;
	for (i = nPlainLen; i < 7; i++)
		pPlain[i] = 0;

	static unsigned char magic[] = {0x4B, 0x47, 0x53, 0x21, 0x40, 0x23, 0x24, 0x25};
	des_key_schedule ks;
	//setup_des_key(data, ks);
	setup_des_key(pPlain, ks);
	des_ecb_encrypt((des_cblock*)magic, (des_cblock*)pHash, ks, DES_ENCRYPT);
}

void HashLMCHALL(unsigned char* pPlain, int nPlainLen, unsigned char* pHash, const unsigned char* pSalt, int nSaltLen)
{
unsigned char pass[14];
unsigned char pre_lmresp[21];
static unsigned char magic[] = {0x4B, 0x47, 0x53, 0x21, 0x40, 0x23, 0x24, 0x25};
des_key_schedule ks;

memset (pass,0,sizeof(pass));
memset (pre_lmresp,0,sizeof(pre_lmresp));

memcpy (pass,pPlain, nPlainLen);

setup_des_key(pass, ks);
des_ecb_encrypt((des_cblock*)magic, (des_cblock*)pre_lmresp, ks, DES_ENCRYPT);

setup_des_key(&pass[7], ks);
des_ecb_encrypt((des_cblock*)magic, (des_cblock*)&pre_lmresp[8], ks, DES_ENCRYPT);

setup_des_key(pre_lmresp, ks);
des_ecb_encrypt((des_cblock*)pSalt, (des_cblock*)pHash, ks, DES_ENCRYPT);

setup_des_key(&pre_lmresp[7], ks);
des_ecb_encrypt((des_cblock*)pSalt, (des_cblock*)&pHash[8], ks, DES_ENCRYPT);

setup_des_key(&pre_lmresp[14], ks);
des_ecb_encrypt((des_cblock*)pSalt, (des_cblock*)&pHash[16], ks, DES_ENCRYPT);

} 

void HashHALFLMCHALL(unsigned char* pPlain, int nPlainLen, unsigned char* pHash, const unsigned char* pSalt, int nSaltLen)
{	
	unsigned char pre_lmresp[8];
	static unsigned char magic[] = {0x4B, 0x47, 0x53, 0x21, 0x40, 0x23, 0x24, 0x25};
	static unsigned char salt[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};

	des_key_schedule ks;
	unsigned char plain[8] = {0};	
	memcpy(plain, pPlain, nPlainLen);
	setup_des_key(plain, ks);
	des_ecb_encrypt((des_cblock*)magic, (des_cblock*)pre_lmresp, ks, DES_ENCRYPT);

	setup_des_key(pre_lmresp, ks);
	des_ecb_encrypt((des_cblock*)salt, (des_cblock*)pHash, ks, DES_ENCRYPT);
} 

void HashNTLM(unsigned char* pPlain, int nPlainLen, unsigned char* pHash, const unsigned char *pSalt, int nSaltLength)
{
	unsigned char UnicodePlain[MAX_PLAIN_LEN * 2];
	int i;
	for (i = 0; i < nPlainLen; i++)
	{
		UnicodePlain[i * 2] = pPlain[i];
		UnicodePlain[i * 2 + 1] = 0x00;
	}
	//mod:alesc
	#ifndef _FAST_HASH_
		MD4(UnicodePlain, nPlainLen * 2, pHash);
	#else
		MD4_CTX ctx;   
    	MD4_Init(&ctx);
    	MD4_Update(&ctx, UnicodePlain, nPlainLen * 2);
    	MD4_Final((unsigned char *) pHash, &ctx);
	#endif
}

void HashMD2(unsigned char* pPlain, int nPlainLen, unsigned char* pHash, const unsigned char *pSalt, int nSaltLength)
{
	//mod:alesc
	#ifndef _FAST_HASH_
		MD2(pPlain, nPlainLen, pHash);
	#else
		MD2_CTX ctx;   
    	MD2_Init(&ctx);
    	MD2_Update(&ctx, pPlain, nPlainLen);
    	MD2_Final((unsigned char *) pHash, &ctx);
	#endif
}

void HashMD4(unsigned char* pPlain, int nPlainLen, unsigned char* pHash, const unsigned char *pSalt, int nSaltLength)
{
	//mod:alesc
	#ifndef _FAST_HASH_
		MD4(pPlain, nPlainLen, pHash);
	#else
		MD4_CTX ctx;   
    	MD4_Init(&ctx);
    	MD4_Update(&ctx, pPlain, nPlainLen);
    	MD4_Final((unsigned char *) pHash, &ctx);
	#endif
}

void HashMD5(unsigned char* pPlain, int nPlainLen, unsigned char* pHash, const unsigned char *pSalt, int nSaltLength)
{
   //mod:alesc
   	#ifndef _FAST_HASH_
	    MD5(pPlain, nPlainLen, pHash);
   		//http://www.freerainbowtables.com/phpBB3/viewtopic.php?f=4&p=916&sid=53804aa79a7bc4bb06cff38481889cf7#p910
   	#elif _FAST_MD5_
   		MD5_NEW(pPlain, nPlainLen, pHash); /* seems to be not thread safe ? */	
	#else
		MD5_CTX ctx;   
   		MD5_Init(&ctx);
   		MD5_Update(&ctx, pPlain, nPlainLen);
   		MD5_Final((unsigned char *) pHash, &ctx);
	#endif
}

void HashSHA1(unsigned char* pPlain, int nPlainLen, unsigned char* pHash, const unsigned char *pSalt, int nSaltLength)
{
	//mod:alesc
	#ifndef _FAST_HASH_
		SHA1(pPlain, nPlainLen, pHash);
	#else
		SHA_CTX ctx;
		SHA1_Init(&ctx);
		SHA1_Update(&ctx, (unsigned char *) pPlain, nPlainLen);
		SHA1_Final(pHash, &ctx);	
	#endif
}

void HashRIPEMD160(unsigned char* pPlain, int nPlainLen, unsigned char* pHash, const unsigned char *pSalt, int nSaltLength)
{
	//mod:alesc
	#ifndef _FAST_HASH_
		RIPEMD160(pPlain, nPlainLen, pHash);
	#else
		RIPEMD160_CTX ctx;
		RIPEMD160_Init(&ctx);
		RIPEMD160_Update(&ctx, (unsigned char *) pPlain, nPlainLen);
		RIPEMD160_Final(pHash, &ctx);
	#endif
}

void HashMSCACHE(unsigned char *pPlain, int nPlainLen, unsigned char* pHash, const unsigned char *pSalt, int nSaltLength)
{
	unsigned char *buf = (unsigned char*)calloc(MSCACHE_HASH_SIZE + nSaltLength, sizeof(unsigned char));
	HashNTLM(pPlain, nPlainLen, buf, NULL);
	//MD4(pPlain, nPlainLen, buf);
	memcpy(buf + MSCACHE_HASH_SIZE, pSalt, nSaltLength);
	//mod:alesc
	#ifndef _FAST_HASH_
		MD4(buf, MSCACHE_HASH_SIZE + nSaltLength, pHash);
	#else
		MD4_CTX ctx;   
    	MD4_Init(&ctx);
    	MD4_Update(&ctx, buf, MSCACHE_HASH_SIZE + nSaltLength);
    	MD4_Final((unsigned char *) pHash, &ctx);
	#endif	
	free(buf);
}

//*********************************************************************************
// Code for MySQL password hashing
//*********************************************************************************

inline void mysql_hash_password_323(unsigned long *result, const char *password) 
{
  register unsigned long nr=1345345333L, add=7, nr2=0x12345671L;
  unsigned long tmp;
  for (; *password ; password++) 
  {
    if (*password == ' ' || *password == '\t') continue;
	tmp= (unsigned long) (unsigned char) *password;
	nr^= (((nr & 63)+add)*tmp)+ (nr << 8);
	nr2+=(nr2 << 8) ^ nr;
	add+=tmp;
  }
  result[0]=nr & (((unsigned long) 1L << 31) -1L); /* Don't use sign bit (str2int) */;
  result[1]=nr2 & (((unsigned long) 1L << 31) -1L);
  return;
}

void HashMySQL323(unsigned char* pPlain, int nPlainLen, unsigned char* pHash, const unsigned char *pSalt, int nSaltLength)
{
	unsigned long hash_pass[2];	
	unsigned char* f = (unsigned char*) hash_pass;

	unsigned char* pass = (unsigned char*) calloc (nPlainLen+4,sizeof(unsigned char));
	memcpy(pass,pPlain,nPlainLen);

	mysql_hash_password_323(hash_pass, (char*) pass);
	pHash[0]=*(f+3); pHash[1]=*(f+2); pHash[2]=*(f+1); pHash[3]=*(f+0);
	pHash[4]=*(f+7); pHash[5]=*(f+6); pHash[6]=*(f+5); pHash[7]=*(f+4);

	free (pass);
}

void HashMySQLSHA1(unsigned char* pPlain, int nPlainLen, unsigned char* pHash, const unsigned char *pSalt, int nSaltLength)
{
	unsigned char hash_stage1[SHA_DIGEST_LENGTH];
		SHA_CTX ctx;
		SHA1_Init(&ctx);
		SHA1_Update(&ctx, (unsigned char *) pPlain, nPlainLen);
		SHA1_Final(hash_stage1, &ctx);
		SHA1_Init(&ctx);
		SHA1_Update(&ctx, hash_stage1, SHA_DIGEST_LENGTH);
		SHA1_Final(pHash, &ctx);
}

//*********************************************************************************
// Code for PIX password hashing
//*********************************************************************************
static char itoa64[] =          /* 0 ... 63 => ascii - 64 */
        "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

void _crypt_to64(char *s, unsigned long v, int n)
{
        while (--n >= 0) {
                *s++ = itoa64[v&0x3f];
                v >>= 6;
        }
}

void HashPIX(unsigned char* pPlain, int nPlainLen, unsigned char* pHash, const unsigned char *pSalt, int nSaltLength)
{
	char temp[MD5_DIGEST_LENGTH+1];
	unsigned char final[MD5_DIGEST_LENGTH];
	char* pass = (char*) calloc (nPlainLen+MD5_DIGEST_LENGTH,sizeof(char));

	memcpy (pass,pPlain,nPlainLen);

	MD5_CTX ctx;
	MD5_Init(&ctx);
	MD5_Update(&ctx, (unsigned char *) pass, MD5_DIGEST_LENGTH);
	MD5_Final(final, &ctx);

	char* p = (char*) temp;
	_crypt_to64(p,*(unsigned long*) (final+0),4); p += 4;
	_crypt_to64(p,*(unsigned long*) (final+4),4); p += 4;
	_crypt_to64(p,*(unsigned long*) (final+8),4); p += 4;
	_crypt_to64(p,*(unsigned long*) (final+12),4); p += 4;
	*p=0;

	memcpy(pHash,temp,MD5_DIGEST_LENGTH);

	free (pass);
}
