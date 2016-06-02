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

// created on 02-June-2016 by Jackie Zhang

#include "geco-engine-ultils.h"

#ifdef _WIN32
bool multibyte2wchar(const char * src, std::wstring& output)
{
    int len = MultiByteToWideChar(CP_ACP, 0, src, -1, NULL, 0);
    if (len <= 0)
    {
        return false;
    }
    else
    {
        // MultiByteToWideChar will process the \0 at the end of the source,
        //  so len will contain that too, meaning that the output size WILL
        //  include the \0 at the end, which breaks string concatenation,
        //  since they will be put AFTER the \0. So, after processing the string
        //  we remove the \0 of the output.
        output.resize(len);
        int res = MultiByteToWideChar(CP_ACP, 0, src, -1, &output[0], len);
        output.resize(output.length() - 1);
        return res != 0;
    }
}

#endif
