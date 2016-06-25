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
// Using a C assert here, because MF_ASSERT calls things that can cause another push/pop 
// to happen, causing this to assert again, ad infinitum.
#include <cassert>
#include "../geco-engine-config.h"
#include "debug.h"
#include <thread>
namespace geco
{
    namespace debugging
    {

#if ENABLE_STACK_TRACKER
        struct stack_item_t
        {
            const char*		name;
            const char*		file;
            unsigned int    line;
        };

        struct stack_tracker_t
        {
            stack_tracker_t();
            virtual ~stack_tracker_t();

            static void push(const char* name, const char*file = NULL, unsigned int  line = 0);
            static void pop();
            static unsigned int  stackSize();
            // 0 == top of stack, stackSize-1 == bottom
            static std::string getStackItem(unsigned int  idx);
            static std::string buildReport();
#ifdef _DEBUG
            static  unsigned int getMaxStackPos();
#endif
        };

        struct scoped_stack_tracker_t
        {
            scoped_stack_tracker_t(const char* name, const char* file = NULL,
                unsigned int line = 0)
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