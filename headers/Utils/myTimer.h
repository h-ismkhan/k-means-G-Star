#ifndef _MY_TIMER_H
#define _MY_TIMER_H

#include <chrono>

class MyChronometer
{
	double recent_duration = 0, total_duration = 0;
	chrono::high_resolution_clock::time_point t1, t2;
public:
	void start()
	{
		t1 = chrono::high_resolution_clock::now();
	}
	void end()
	{
		t2 = chrono::high_resolution_clock::now();

		chrono::duration<double> time_span = chrono::duration_cast<chrono::duration<double>>(t2 - t1);
		recent_duration = time_span.count();
		total_duration = total_duration + recent_duration;
	}
	double duration(){ return total_duration; }
	double duration_recent(){ return recent_duration; }
};

#endif
