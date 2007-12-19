/*
   RainbowCrack - a general propose implementation of Philippe Oechslin's faster time-memory trade-off technique.

   Copyright (C) Zhu Shuanglei <shuanglei@hotmail.com>
*/

#ifndef _HASHALGORITHM_H
#define _HASHALGORITHM_H

void HashLM(unsigned char* pPlain, int nPlainLen, unsigned char* pHash, const unsigned char *pUsername = 0, int nSaltLength = 0);
void HashNTLM(unsigned char* pPlain, int nPlainLen, unsigned char* pHash, const unsigned char *pUsername = 0, int nSaltLength = 0);
void HashMD2(unsigned char* pPlain, int nPlainLen, unsigned char* pHash, const unsigned char *pUsername = 0, int nSaltLength = 0);
void HashMD4(unsigned char* pPlain, int nPlainLen, unsigned char* pHash, const unsigned char *pUsername = 0, int nSaltLength = 0);
void HashMD5(unsigned char* pPlain, int nPlainLen, unsigned char* pHash, const unsigned char *pUsername = 0, int nSaltLength = 0);
void HashSHA1(unsigned char* pPlain, int nPlainLen, unsigned char* pHash, const unsigned char *pUsername = 0, int nSaltLength = 0);
void HashRIPEMD160(unsigned char* pPlain, int nPlainLen, unsigned char* pHash, const unsigned char *pUsername = 0, int nSaltLength = 0);
void HashMSCACHE(unsigned char *pPlain, int nPlainLen, unsigned char* pHash, const unsigned char *pUsername = 0, int nSaltLength = 0);
//****************************************************************************
// MySQL Password Hashing
//****************************************************************************
void HashMySQL323(unsigned char* pPlain, int nPlainLen, unsigned char* pHash, const unsigned char *pUsername = 0, int nSaltLength = 0);
void HashMySQLSHA1(unsigned char* pPlain, int nPlainLen, unsigned char* pHash, const unsigned char *pUsername = 0, int nSaltLength = 0);

//****************************************************************************
// Cisco PIX Password Hashing
//****************************************************************************
void HashPIX(unsigned char* pPlain, int nPlainLen, unsigned char* pHash, const unsigned char *pUsername = 0, int nSaltLength = 0);

//****************************************************************************
// (HALF) LM CHALL hashing
void HashLMCHALL(unsigned char* pPlain, int nPlainLen, unsigned char* pHash, const unsigned char* pSalt = 0, int nSaltLen = 0);
void HashHALFLMCHALL(unsigned char* pPlain, int nPlainLen, unsigned char* pHash, const unsigned char* pSalt = 0, int nSaltLen = 0);

#endif
