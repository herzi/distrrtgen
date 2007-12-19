#ifndef DATAGENERATIONTHREAD_H
#define DATAGENERATIONTHREAD_H
#include "Thread.h"
#include "config.h"
typedef struct 
{
	int nRainbowChainLen;
} DataGenerationThreadParameters;

class CDataGenerationThread :
	public CThread
{
public:
	CDataGenerationThread(void);
	void threadProc();
	~CDataGenerationThread(void);
	int GetIsDataReadyFlag() { return bDataReady; }
	void ClearDataReadyFlag() { bDataReady = 0; }
	unsigned const char *GetData() { return zDataChunk; }
//	int GetChainSpeed() { return m_nChainSpeed; }
	int GetChainsCalculated() 
	{ 
		int tmp = m_nChainsCalculated;
		m_nChainsCalculated = 0;
		return tmp; 
	}
private:
	unsigned char zBuffer[DATA_CHUNK_SIZE]; // Used to store temporary data. Can contain 20000 chains (32kb)
	unsigned char zDataChunk[DATA_CHUNK_SIZE]; // Used to store data a generated data chunk. Can contain 20000 chains (32kb)
	int bDataReady; // Used to signal if buffer is filled with data. 0 = empty. 1 = buffer is full
	int bShutdown; // Used to tell the thread to exit.
//	int m_nChainSpeed;
	int m_nChainsCalculated;
};
#endif
