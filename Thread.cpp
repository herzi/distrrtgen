
#include "Thread.h"
#include <iostream>
#include "config.h"
CThread::CThread(void)
{
	bTerminateThreadFlag = false;

}

CThread::~CThread(void)
{
}

void CThread::Stop() {
	// set our terminate flag and let our threadProc exit naturally
	bTerminateThreadFlag = true;

	// Why don't we just wait here until our function finishes before we continue processing.
	// Note: our end() function will stall the calling thread until this thread finishes executing.

	// remember our waitForSingleObject function?  Let's use it here to wait for our thread to finish.
#ifdef WIN32
	WaitForSingleObject(threadHandle, INFINITE);
#else
	pthread_join(threadHandle, NULL);
#endif
}

void StartThreadFunction(CThread* pThread)
{
	pThread->threadProc();	
}
void CThread::Start(void *Parameters)
{
	this->Params = Parameters;
#ifdef WIN32
	threadHandle = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE) StartThreadFunction, this, NULL, NULL);
#else
	int nRet = pthread_create(&threadHandle, NULL, (void*(*)(void*))StartThreadFunction, (void*) this);
	if(nRet != 0)
	{
		std::cout << "ERROR: pthread_create() returned " << nRet;
	}
#endif	
}
