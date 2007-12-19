#ifndef __RAINBOWTABLEGENERATOR_H_
#define __RAINBOWTABLEGENERATOR_H_

#include <string>
#include <fstream>
#include "DataGenerationThread.h"

class CRainbowTableGenerator
{
public:
	CRainbowTableGenerator(int nNumProcessors);
public:
	~CRainbowTableGenerator(void);
	int CalculateTable(std::string sFilename, int nRainbowChainCount, std::string sHashRoutineName, std::string sCharsetName, int nPlainLenMin, int nPlainLenMax, int nRainbowTableIndex, int nRainbowChainLen, std::string sSalt = "");

//	int GetCalculationSpeed() { return m_nCalculationSpeed; }
	int GetCurrentCalculatedChains() 
	{
		if(m_nCurrentCalculatedChains == -1) return 0;
		for(int i = 0; i < m_nProcessorCount; i++)
		{
			if(m_pThreads[i] != NULL)
			{
				m_nCurrentCalculatedChains += m_pThreads[i]->GetChainsCalculated();
			}
		}
		return m_nCurrentCalculatedChains;
	}
	int GetProcessorCount() { return m_nProcessorCount; }

private:
	int m_nCalculationSpeed;
	int m_nCurrentCalculatedChains;
	int m_nProcessorCount;
	CDataGenerationThread **m_pThreads;
//	int GetFileLen(std::ofstream File);
};

#endif
