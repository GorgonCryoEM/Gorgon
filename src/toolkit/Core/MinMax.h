/*
 * Range.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_CORE_RANGE_H_
#define SRC_TOOLKIT_CORE_RANGE_H_

namespace Core {

    class Range {
        public:
            Range();
            virtual ~Range();

            void setMin(float val);
            void setMax(float val);
            float getMin();
            float getMax();

        private:
            float min;
            float max;
    };

} /* namespace Core */

#endif /* SRC_TOOLKIT_CORE_RANGE_H_ */
