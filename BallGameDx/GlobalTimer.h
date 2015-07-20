#pragma once
#include <windows.h>

class GlobalTimer
{
public:
	static GlobalTimer& GetInstance();
	float GetElapsedTime();

private:
	GlobalTimer();

private:
	static GlobalTimer* m_pGlobalTimer;

	LONGLONG m_llQPFTicksPerSec;
    LONGLONG m_llLastElapsedTime;
};

inline float GlobalTimer::GetElapsedTime()
{
    LARGE_INTEGER qwTime;
	QueryPerformanceCounter( &qwTime );

    double fElapsedTime = ( float )( ( double )( qwTime.QuadPart - m_llLastElapsedTime ) / ( double )m_llQPFTicksPerSec );
    m_llLastElapsedTime = qwTime.QuadPart;

    // Clamp the timer to non-negative values to ensure the timer is accurate.
    // fElapsedTime can be outside this range if processor goes into a 
    // power save mode or we somehow get shuffled to another processor.  
    // However, the main thread should call SetThreadAffinityMask to ensure that 
    // we don't get shuffled to another processor.  Other worker threads should NOT call 
    // SetThreadAffinityMask, but use a shared copy of the timer data gathered from 
    // the main thread.
    if( fElapsedTime < 0.0f )
        fElapsedTime = 0.0f;

    return ( float )fElapsedTime;
}
