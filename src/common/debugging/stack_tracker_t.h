/*
* Geco Gaming Company
* All Rights Reserved.
* Copyright (c)  2016 GECOEngine.
*
* GECOEngine is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* GECOEngine is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.

* You should have received a copy of the GNU Lesser General Public License
* along with KBEngine.  If not, see <http://www.gnu.org/licenses/>.
*
*/

// created on 25-June-2016 by Jackie Zhang

#ifndef _SRC_STACK_TRACER
#define _SRC_STACK_TRACER

#include <string>
// Using a C assert here, because MF_ASSERT calls things that can cause 
// another push/pop to happen, causing this to assert again, ad infinitum.
#include <cassert>
#include <thread>
#include "../geco-engine-config.h"
#include "../plateform.h"

namespace geco
{
    namespace debugging
    {

#if ENABLE_STACK_TRACKER
        enum : uint
        {
            THREAD_STACK_DEPTH = 1024			// The maximum stack size for a thread
        };

        struct stack_item_t
        {
            const char*		name;
            const char*		file;
            uint    line;
        };

        struct stack_tracker_t
        {
            static thread_local stack_item_t stack_[THREAD_STACK_DEPTH];
            static thread_local uint stack_pos_;
            static thread_local uint		max_stack_pos_;

            static void push(const char* name, const char*file = NULL, uint  line = 0)
            {
                assert(stack_tracker_t::stack_pos_ < THREAD_STACK_DEPTH);
                uint stackpos = stack_tracker_t::stack_pos_;
                stack_[stackpos] = { name, file, line };
                ++stack_tracker_t::stack_pos_;
                if (stack_tracker_t::stack_pos_ > stack_tracker_t::max_stack_pos_)
                {
                    stack_tracker_t::max_stack_pos_ = stack_tracker_t::stack_pos_;
                }
            }
            static void pop()
            {
                assert(stack_pos_ > 0);
                --stack_pos_;
            }
            static uint  stackSize() { return stack_pos_; }
            // 0 == top of stack, stackSize-1 == bottom
            static std::string getStackItem(uint  idx);
            static std::string report();
        };

        struct scoped_stack_tracker_t
        {
            scoped_stack_tracker_t(const char* name, const char* file = NULL,
                uint line = 0)
            {
                stack_tracker_t::push(name, file, line);
            }

            ~scoped_stack_tracker_t()
            {
                stack_tracker_t::pop();
            }
        };
#endif
    }
}
#endif