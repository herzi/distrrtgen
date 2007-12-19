
#include "DataGenerationThread.h"
#include "ChainWalkContext.h"
#include <time.h>
#include <iostream>
CDataGenerationThread::CDataGenerationThread(void)
{
	memset(zBuffer, 0x00, sizeof(zBuffer));
	bDataReady = 0;
	bShutdown = 0;
	m_nChainsCalculated = 0;
}

CDataGenerationThread::~CDataGenerationThread(void)
{
}

void CDataGenerationThread::threadProc()
{
#ifdef _WIN32
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_IDLE);
#else
	nice(19);
#endif

	const DataGenerationThreadParameters *Parameters = (const DataGenerationThreadParameters *)Params;
	CChainWalkContext cwc;
	// Make a pointer to the beginning of the data block
	unsigned char *ptrCurrent = zBuffer;
	// Make a pointer to the end of the data block.
	unsigned char *ptrEnd = zBuffer + DATA_CHUNK_SIZE;
#ifdef DEBUG
	std::cout << "Datageneration thread started" << std::endl;
#endif
	while(bTerminateThreadFlag != 1)
	{
		cwc.GenerateRandomIndex();
		uint64 nIndex = cwc.GetIndex();
		// Copy the content over
		memcpy(ptrCurrent, &nIndex, 8);
		// Increase the pointer location
		ptrCurrent += 8;
		int nPos;
		for (nPos = 0; nPos < Parameters->nRainbowChainLen - 1; nPos++)
		{
			cwc.IndexToPlain();
			cwc.PlainToHash();
			cwc.HashToIndex(nPos);
		}

		nIndex = cwc.GetIndex();
		// Copy the content over
		memcpy(ptrCurrent, &nIndex, 8);
		// Increase the pointer location
		ptrCurrent += 8;
		// We counted another chain
		m_nChainsCalculated++;
		// The data buffer is full.. Let's swap buffers
		if(ptrCurrent >= ptrEnd)
		{
#ifdef DEBUG
			std::cout << "Data chunk finished" << std::endl;
#endif
			// The old buffer isn't emptied yet. So we have to wait before writing our data
			while(bDataReady == 1 && bTerminateThreadFlag != 1)
			{
#ifdef DEBUG
				std::cout << "WARNING: Data buffer is not emptied yet!" << std::endl;
#endif
				Sleep(10);
			}
			// Copy the data over
			memcpy(zDataChunk, zBuffer, DATA_CHUNK_SIZE);
			// Mark the buffer as full
			bDataReady = 1;
			// Reset the data pointer
			ptrCurrent = zBuffer;			
		}
	}
}
