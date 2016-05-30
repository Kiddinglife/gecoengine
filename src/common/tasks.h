/*
This source file is part of GecoEngine
For the latest info, see http://www.kbengine.org/

Copyright (c) 2008-2016 GecoEngine.

GecoEngine is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

GecoEngine is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with GecoEngine.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef KBE_TASKS_H
#define KBE_TASKS_H

#include "common/task.h"
#include "common/common.h"

namespace GecoEngine
{

    /**
     *	ÈÎÎñÈÝÆ÷
     */
    class Tasks
    {
        public:
        Tasks();
        ~Tasks();

        void add(Task * pTask);
        bool cancel(Task * pTask);
        void process();
        private:

        typedef std::vector<GecoEngine::Task *> Container;
        Container container_;
    };

}

#endif // KBE_TASKS_H
