#include "header.h"
#include "timer.h"


//  ___ ___ ___ ___  Timer class  ___ ___ ___ ___

Timer::Timer()
{
	iv = 0.;  iv1 = 0.4;
	iFR = 0;  FR = 0.;
	
	LARGE_INTEGER FQ;
	if (QueryPerformanceFrequency( &FQ ))
		fq = double( FQ.QuadPart );
	else	printf("QueryPerformanceFrequency Error !\n");

	if (!QueryPerformanceCounter( &CC ))
	{	printf("QueryPerformanceCounter Error !\n");	}
	cc = double( CC.QuadPart );
	t = cc / fq;
	st = t;  st1 = t;

	printf("CPU Freq = %7.4f GHz\n\n", fq*0.000000001);
}

bool Timer::update(bool updFR)
{
	if (!QueryPerformanceCounter( &CC ))	return true;
	cc = double( CC.QuadPart );
	t = cc / fq;
	
	dt = t - st;  // delta time
	if (dt < iv)  // interval
		return false;
	
	st = t;  // old time
	
	//  framerate update
	if (!updFR)
		return true;
	iFR++;  // frames count

	dt1 = t - st1;
	if (dt1 >= iv1)
	{
		FR = iFR / dt1;
		//if (FR < 1.)  FR = 0.;
		//if (FR > 1000.)  FR = 999.;
		iFR = 0;

		st1 = t;
	}
	return true;
}
