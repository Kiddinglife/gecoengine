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
#include "ultils.h"
namespace geco
{
	namespace ultils
	{

#ifdef _WIN32
		bool win_geco_acp2w(const char * src, std::wstring& output)
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

		bool geco_utf8tow(const char * src, std::wstring & output)
		{
			int len = MultiByteToWideChar(CP_UTF8, 0, src, -1, NULL, 0);
			if (len <= 0)
			{
				return false;
			}
			else
			{
				output.resize(len);
				int res = MultiByteToWideChar(CP_UTF8, 0, src, -1, &output[0], len);
				output.resize(output.length() - 1);
				return res != 0;
			}
		}

		FILE* win_fopen(const char* filename, const char* mode)
		{
			std::wstring wfilename;
			std::wstring wmode;

			geco_utf8tow(filename, wfilename);
			geco_utf8tow(mode, wmode);

			return _wfopen(wfilename.c_str(), wmode.c_str());
		}

		long win_geco_file_size(FILE* file)
		{
			long currentLocation = ftell(file);
			if (currentLocation < 0)
			{
				currentLocation = 0;
			}
			/*set curr location to file beginning*/
			int res = fseek(file, 0, SEEK_END);
			if (res)
			{
				//ERRORLOG("bw_fileSize: fseek failed\n");
				return -1;
			}
			long length = ftell(file);
			/*set back curr location after getting file length*/
			res = fseek(file, currentLocation, SEEK_SET);
			if (res)
			{
				//ERRORLOG("bw_fileSize: fseek failed\n");
				return -1;
			}
			return length;
		}

		std::wstring win_get_temp_file_path_name()
		{
			wchar_t tempDir[MAX_PATH + 1];
			wchar_t tempFile[MAX_PATH + 1];

			if (GetTempPath(MAX_PATH + 1, tempDir) < MAX_PATH)
			{
				if (GetTempFileName(tempDir, L"BWT", 0, tempFile))
				{
					return tempFile;
				}
			}
			return L"";
		}
		std::wstring win_geco_acp2w(const std::string & s)
		{
			std::wstring ret;
			win_geco_acp2w(s, ret);
			return ret;
		}
		bool win_geco_acp2w(const std::string & s, std::wstring& output)
		{
			return win_geco_acp2w(s.c_str(), output);
		}
#endif
	}
}
