
#include "RainbowTableGenerator.h"
#include "ChainWalkContext.h"
#include <iostream>
#include <sstream>
#include <time.h>
#include "Exception.h"

CRainbowTableGenerator::CRainbowTableGenerator(int nNumProcessors)
{
	m_nCalculationSpeed = 0;
	m_nProcessorCount = 0;
	m_nCurrentCalculatedChains = 0;
	if(nNumProcessors == 0)
	{
#ifdef WIN32
		// Get amount of logical processors in Windows
		SYSTEM_INFO sysInfo;
		GetSystemInfo(&sysInfo);
		m_nProcessorCount = sysInfo.dwNumberOfProcessors;
#else
		// Get amount of logical processors in Linux
		char cpuinfo[1024];
		FILE* fileCPU = fopen("/proc/cpuinfo", "r");
		if(fileCPU == NULL)
		{
			throw new Exception("Unable to autodetect the processor count. Please edit distrrtgen.conf and manually set the amount of processors");
		}
		m_nProcessorCount = 0;
		while(!feof(fileCPU))
		{
			fgets(cpuinfo, sizeof(cpuinfo), fileCPU);
			if(strstr(cpuinfo, "processor"))
				m_nProcessorCount++;
		}
		fclose(fileCPU);
#endif
	}
	else
	{
		m_nProcessorCount = nNumProcessors;
	}

	m_pThreads = new CDataGenerationThread*[m_nProcessorCount];
	for(int i = 0; i < m_nProcessorCount; i++)
	{
		m_pThreads[i] = NULL;
	}

}

CRainbowTableGenerator::~CRainbowTableGenerator(void)
{
	delete [] m_pThreads;
}

int CRainbowTableGenerator::CalculateTable(std::string sFilename, int nRainbowChainCount, std::string sHashRoutineName, std::string sCharsetName, int nPlainLenMin, int nPlainLenMax, int nRainbowTableIndex, int nRainbowChainLen, std::string sSalt)
{
//	std::fstream test("c:\\distrrtgen.log");
//	std::streambuf *old = std::cout.rdbuf(test.rdbuf());
	// Setup CChainWalkContext
	if (!CChainWalkContext::SetHashRoutine(sHashRoutineName))
	{
		std::cout << "hash routine " << sHashRoutineName << " not supported" << std::endl;
		return 1;
	}
	if (!CChainWalkContext::SetPlainCharset(sCharsetName, nPlainLenMin, nPlainLenMax))
		return 2;
	if (!CChainWalkContext::SetRainbowTableIndex(nRainbowTableIndex))
	{
		std::cout << "invalid rainbow table index " << nRainbowTableIndex << std::endl;
		return 3;
	}
//	std::cout.rdbuf(old);
	if(sHashRoutineName == "mscache")// || sHashRoutineName == "lmchall" || sHashRoutineName == "halflmchall")
	{
		int salt_length = 0;
		unsigned char UnicodePlain[MAX_PLAIN_LEN * 2];
		int i;
		for (i = 0; i < sSalt.length(); i++)
		{
			UnicodePlain[i * 2] = ((unsigned char *)sSalt.c_str())[i];
			UnicodePlain[i * 2 + 1] = 0x00;
		}

/*
		unsigned short cur_salt[256];
		while( ((unsigned char *)sSalt.c_str())[salt_length]!=0x00 )
		{
//	#if ARCH_LITTLE_ENDIAN
			cur_salt[salt_length] = ((unsigned char *)sSalt.c_str())[salt_length];
//	#else
//			cur_salt[salt_length] = ((unsigned char *)sSalt.c_str())[salt_length] << 8;
//	#endif
			salt_length ++;
		}
		cur_salt[salt_length] = 0;*/
		CChainWalkContext::SetSalt((unsigned char*)UnicodePlain, i*2);
	}
	else if(sHashRoutineName == "halflmchall")
	{
//		char salt[] = "1122334455667788";
//		__int64 salt = 0x8877665544332211;
		//memcpy(salt, , 8);
//		CChainWalkContext::SetSalt((unsigned char*)&salt, 8);
	}
	CChainWalkContext::Dump();	

	// FileName
//	std::stringstream szFilename;
//	szFilename << partid << ".rt";
	std::ofstream Partfile;
	Partfile.open(sFilename.c_str(), std::ios::out | std::ios::binary | std::ios::app);
	
	// Open file
//	fclose(fopen(szFileName.str().c_str(), "a"));
//	FILE* file = fopen(szFileName.str().c_str(), "r+b");
	
	if (Partfile.is_open() == false)
	{
		std::cout << "failed to create " << sFilename << std::endl;
		return 4;
	}
	
	
	// Check existing chains
	//unsigned int nDataLen = (unsigned int)GetFileLen(Partfile);
	
	long begin,end;
	
	begin = Partfile.tellp();
	Partfile.seekp(0, std::ios::end);
	end = Partfile.tellp();
	unsigned int nDataLen = end - begin;
	
	nDataLen = nDataLen / 16 * 16;
	if ((int)nDataLen == nRainbowChainCount * 16)
	{
		std::cout << "precomputation of this rainbow table already finished" << std::endl;
		Partfile.close();
		return 0;
	}
	if (nDataLen > 0)
	{
		std::cout << "continuing from interrupted precomputation..." << std::endl;
	}
	
	Partfile.seekp(0, std::ios::end);
	
	// Generate rainbow table
//	std::cout << "generating..." << std::endl;
	DataGenerationThreadParameters options;
	options.nRainbowChainLen = nRainbowChainLen;
	for(int i = 0; i < m_nProcessorCount; i++)
	{
		m_pThreads[i] = new CDataGenerationThread();
		m_pThreads[i]->Start(&options);
	}
	int nCalculatedChains = nDataLen / 16;
	m_nCurrentCalculatedChains = nCalculatedChains;
	time_t tStart = time(NULL);
	time_t tEnd;
	int nOldCalculatedchains = GetCurrentCalculatedChains();
	int nTotalChainSpeed = 0;
	
	while(nCalculatedChains < nRainbowChainCount)
	{
		tEnd = time(NULL);
		if(tEnd - tStart > 10)
		{
#ifdef WIN32
			system("cls");
#else
			system("clear");
#endif
			float nPercent = (float)nCalculatedChains / (float)nRainbowChainCount;
			nPercent *= 100;
			std::cout << "Current chain speed: " << ((GetCurrentCalculatedChains() - nOldCalculatedchains) / 10) << std::endl;			
			std::cout.precision(2);
			std::cout << "Percent completed: " << nPercent << "%" << std::endl;
			nOldCalculatedchains = m_nCurrentCalculatedChains;
			tStart = time(NULL);
		}

		for(int i = 0; i < m_nProcessorCount; i++)
		{
			if(m_pThreads[i]->GetIsDataReadyFlag() > 0)
			{

				// Retrieve the calculated data
				const char *data = (const char *)m_pThreads[i]->GetData();
				// Write it to the file
				int nNewChains = DATA_CHUNK_SIZE / 16;
				// Check if too many chains is calculated
				if(nRainbowChainCount < (nCalculatedChains + nNewChains))
				{
					// If this is the fact, reduce the amount of chains we copy
					nNewChains = nRainbowChainCount - nCalculatedChains;
				}
				if(nNewChains == 0)
					break;
				nCalculatedChains += nNewChains;
				
				Partfile.write(data, nNewChains * 16);
				Partfile.flush();

				// And mark the buffer as empty
				m_pThreads[i]->ClearDataReadyFlag();
			}
		}
		Sleep(1);
	}
	// Stop the threads again and destroy them
	for(int i = 0; i < m_nProcessorCount; i++)
	{
		m_pThreads[i]->Stop();
		delete m_pThreads[i];
		m_pThreads[i] = NULL;
	}	


	return 0;
}

