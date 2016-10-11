/*
 * TimeManager.h
 *
 *      Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_FOUNDATION_TIMEMANAGER_H_
#define SRC_TOOLKIT_FOUNDATION_TIMEMANAGER_H_

#include <ctime>
#include <string>
#include <vector>
#include <cstdio>

using namespace std;

namespace Foundation {
    struct stopwatch_rec {
        clock_t startTime;
        double cumulatedTime;
    };

    /*
     *
     */
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

} /* namespace Foundation */

#endif /* SRC_TOOLKIT_FOUNDATION_TIMEMANAGER_H_ */
