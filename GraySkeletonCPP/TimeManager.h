#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

#include <time.h>
#include <string.h>
#include <vector>

using namespace std;

namespace wustl_mm {
	namespace GraySkeletonCPP {
		class TimeManager {
		public:			
			TimeManager();
			void PushCurrentTime();
			void PopAndDisplayTime(string textFormat);
			vector<clock_t> times;
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
			times.pop_back();			
		}
	}
}


#endif