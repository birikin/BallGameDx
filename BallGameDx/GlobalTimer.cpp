#include "GlobalTimer.h"

GlobalTimer* GlobalTimer::m_pGlobalTimer = NULL;

GlobalTimer& GlobalTimer::GetInstance()
{
	if(m_pGlobalTimer==NULL)
	{
		m_pGlobalTimer=new GlobalTimer();
	}
	return *m_pGlobalTimer;
}

GlobalTimer::GlobalTimer()
{
	LARGE_INTEGER qwTicksPerSec = { 0 };
    QueryPerformanceFrequency( &qwTicksPerSec );
    m_llQPFTicksPerSec = qwTicksPerSec.QuadPart;

	LARGE_INTEGER qwTime = { 0 };
    QueryPerformanceCounter( &qwTime );
	m_llLastElapsedTime = qwTime.QuadPart;
}


