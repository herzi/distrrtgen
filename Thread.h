#ifndef THREAD_H
#define THREAD_H
#ifdef WIN32
#include <windows.h>

#else
#include <pthread.h>
#endif
class CThread
{
protected:
	int bTerminateThreadFlag;
	void *Params;
#ifdef WIN32
	HANDLE threadHandle;
#else
	pthread_t threadHandle;
#endif
public:
	CThread(void);
	void Start(void *Parameters);
	void Stop();
	virtual void threadProc() = 0;
public:
	virtual ~CThread(void);
};

#endif
