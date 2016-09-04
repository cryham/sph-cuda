#pragma once


class Timer
{
	private:
		double cc,fq, st, dt1,st1;  int iFR;
		LARGE_INTEGER CC;

	public:
		double t, dt, FR, iv, iv1;
		// delta time, FrameRate, interval, intervalFR

		Timer();
		bool update(bool updFR = false);
};

