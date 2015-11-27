// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A class for displaying execution time... maintains a time stack so that the execution time can be broken down.


#ifndef FOUNDATION_TIME_MANAGER_H
#define FOUNDATION_TIME_MANAGER_H

#include <ctime>
#include <string>
#include <vector>
#include <cstdio>

using namespace std;

namespace wustl_mm {
	namespace Foundation {
		struct stopwatch_rec {
			clock_t startTime;
			double cumulatedTime;
		};

		class TimeManager {
		public:			
			TimeManager();
			void PushCurrentTime();
			void PopAndDisplayTime(string textFormat);

			int StartStopWatch();
			void PauseStopWatch(int id);
			void ResumeStopWatch(int id);
			void DisplayStopWatch(int id, string textFormat);

		private:
			vector<clock_t> times;
			vector<stopwatch_rec> stopWatches;
		};

		TimeManager::TimeManager() {
			times.clear();			
		}

		void TimeManager::PushCurrentTime() {
			times.push_back(clock());			
		}

		void TimeManager::PopAndDisplayTime(string textFormat) {
			clock_t start = times[times.size()-1];			
			clock_t finish = clock();			
			double timeTaken = ((double) (finish - start) / (double) CLOCKS_PER_SEC);
			printf(textFormat.c_str(), timeTaken); 
			#ifdef _WIN32
				flushall();
			#endif
			
			times.pop_back();			
		}

		int TimeManager::StartStopWatch() {
			stopwatch_rec rec;
			rec.startTime = clock();
			rec.cumulatedTime = 0;		
			stopWatches.push_back(rec);
			return stopWatches.size()-1;
		}

		void TimeManager::PauseStopWatch(int id) {
			clock_t start = stopWatches[id].startTime;
			clock_t finish = clock();			
			stopWatches[id].cumulatedTime += ((double) (finish - start) / (double) CLOCKS_PER_SEC);			
		}

		void TimeManager::ResumeStopWatch(int id) {
			stopWatches[id].startTime = clock();
		}

		void TimeManager::DisplayStopWatch(int id, string textFormat){			
			printf(textFormat.c_str(), stopWatches[id].cumulatedTime);
			#ifdef _WIN32
				flushall();
			#endif
		}
	}
}


#endif
