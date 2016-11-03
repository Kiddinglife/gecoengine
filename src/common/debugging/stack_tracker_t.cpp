#include "stack_tracker_t.h"
namespace geco
{
    namespace debugging
    {

#if ENABLE_STACK_TRACKER
        thread_local stack_item_t  stack_tracker_t::stack_[THREAD_STACK_DEPTH] = { 0 };
        thread_local uint  stack_tracker_t::stack_pos_ = 0;
        thread_local uint  stack_tracker_t::max_stack_pos_ = 0;

        /// 0 == top of stack, stackSize-1 == bottom 
        std::string stack_tracker_t::getStackItem(uint idx)
        {
            assert(idx < stack_tracker_t::stack_pos_);
            return stack_[stack_tracker_t::stack_pos_ - 1 - idx].name;
        }
        std::string stack_tracker_t::report()
        {
            std::string res;
            for (uint idx = 0; idx < stack_tracker_t::stack_pos_; ++idx)
            {
                res += getStackItem(idx);
                if (idx < stack_tracker_t::stack_pos_ - 1)
                {
                    res += "<-";
                }
            }
            return res;
        }
#endif

    }
}
