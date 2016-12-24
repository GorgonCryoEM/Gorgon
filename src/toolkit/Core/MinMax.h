/*
 * MinMax.h
 *
 * Author: shadow_walker <shadowwalkersb@gmail.com>
 *
 */

#ifndef SRC_TOOLKIT_CORE_MINMAX_H_
#define SRC_TOOLKIT_CORE_MINMAX_H_

namespace Core {

    class MinMax {
        public:
            MinMax();
            virtual ~MinMax();

            void setMin(float val);
            void setMax(float val);
            float getMin() const;
            float getMax() const;

        private:
            float min;
            float max;
    };

} /* namespace Core */

#endif /* SRC_TOOLKIT_CORE_MINMAX_H_ */
