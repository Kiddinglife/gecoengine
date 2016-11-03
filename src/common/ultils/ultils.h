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

// http://www.cnblogs.com/nysanier/archive/2011/04/19/2020778.html
// created on 02-June-2016 by Jackie Zhang
#ifndef _INCLUDE_GECO_ENGINE_ULTILS
#define _INCLUDE_GECO_ENGINE_ULTILS

#include <string>
#include <cstring>
#include <stdio.h>
#include <stdint.h>
#ifdef _MSC_VER
#include <intrin.h>     /* _BitScanForward, _BitScanReverse */
#endif
#if defined(_WIN32)
#include <windows.h>
#define WIN32_LEAN_AND_MEAN
#endif
//#define TIMEOUT_DISABLE_BUILTIN_BITOPS
namespace geco
{
namespace ultils
{

#if !defined countof
#define countof(a) (sizeof (a) / sizeof *(a))
#endif

#if !defined endof
#define endof(a) (&(a)[countof(a)])
#endif

#if !defined MIN
#define MIN(a, b) (((a) < (b))? (a) : (b))
#endif

#if !defined MAX
#define MAX(a, b) (((a) > (b))? (a) : (b))
#endif

/// Given a number of bits,
/// return how many bytes are needed to hold all bits.
/// For example, 17 bits will need 3 bytes to hold.
#define BITS_TO_BYTES(x) (((x)+7)>>3)
#define BYTES_TO_BITS(x) ((x)<<3)

inline void Bitify(char* out, int mWritePosBits, unsigned char* mBuffer,
		bool hide_zero_low_bytes = false)
{
	printf(
			"[%dbits %dbytes]\ntop (low byte)-> bottom (high byte),\nright(low bit)->left(high bit):\n",
			mWritePosBits, BITS_TO_BYTES(mWritePosBits));
	if (mWritePosBits <= 0)
	{
		strcpy(out, "no bits to print\n");
		return;
	}
	int strIndex = 0;
	int inner;
	int stopPos;
	int outter;
	int len = BITS_TO_BYTES(mWritePosBits);
	bool first_1 = true;

	for (outter = 0; outter < len; outter++)
	{
		if (outter == len - 1)
			stopPos = 8 - (((mWritePosBits - 1) & 7) + 1);
		else
			stopPos = 0;

		for (inner = 7; inner >= stopPos; inner--)
		{
			if ((mBuffer[outter] >> inner) & 1)
			{
				if (hide_zero_low_bytes && first_1 && strIndex >= 8)
				{
					strIndex = (strIndex & 7) + 2;
					first_1 = false;
				}
				out[strIndex++] = '1';
			}
			else
			{
				out[strIndex++] = '0';
			}
		}
		out[strIndex++] = '\n';
	}

	out[strIndex++] = '\n';
	out[strIndex++] = 0;
}

//-------------------------------------------------------
//	Section: String and File Ultil Functions
//-------------------------------------------------------

/**
 * @brief Converts the given utf-8 string to the wide representation.
 * Returns true if it succeeded, otherwise false if there was a decoding error.
 */
bool geco_utf8tow(const char * s, std::wstring& output);
inline bool geco_utf8tow(const std::string & s, std::wstring& output)
{
	return geco_utf8tow(s.c_str(), output);
}
inline std::wstring geco_utf8tow(const std::string & s)
{
	std::wstring ret;
	geco_utf8tow(s, ret);
	return ret;
}

#if defined(_WIN32)
/*
 * @brief Converts the given narrow string to the wide representation.
 * using theactive code page on this system. Returns true if it succeeded, otherwise
 *	false if there was a decoding error.
 */
extern bool win_geco_acp2w(const char * src, std::wstring& output);
inline bool win_geco_acp2w(const std::string & s, std::wstring& output)
{
	return win_geco_acp2w(s.c_str(), output);
}
inline std::wstring win_geco_acp2w(const std::string & s)
{
	std::wstring ret;
	win_geco_acp2w(s, ret);
	return ret;
}
#define geco_fopen win_fopen
inline FILE* win_fopen(const char* filename, const char* mode)
{
	std::wstring wfilename;
	std::wstring wmode;

	geco_utf8tow(filename, wfilename);
	geco_utf8tow(mode, wmode);

	return _wfopen(wfilename.c_str(), wmode.c_str());
}

inline long win_geco_file_size(FILE* file)
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

inline std::wstring win_get_temp_file_path_name()
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
#else
#include<stdio.h>
#define geco_fopen fopen //bw_fopen might be used directly
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//	                                      Section:  timer bit operations
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
 *  @return sizeofbits -1  if given 0, eg uint64_t a = 0, ctz64(a) returns 63
 *  other number will be fine
 * */
#if defined(__GNUC__) && !defined(TIMEOUT_DISABLE_BUILTIN_BITOPS)
/* First define ctz and clz functions; these are compiler-dependent if
 * you want them to be fast. On GCC and clang and some others,
 * we can use __builtin functions. They are not defined for n==0,
 * but timeout.s never calls them with n==0 */
inline int ctz32(unsigned int val)
{
	return __builtin_ctz(val);
}
inline int clz32(unsigned int val)
{
	return __builtin_clz(val);
}
inline int ctz64(uint64_t val)
{
	return __builtin_ctzll(val);
}
inline int clz64(uint64_t val)
{
	return __builtin_clzll(val);
}
#elif !defined(_MSC_VER) && !defined(TIMEOUT_DISABLE_BUILTIN_BITOPS)
/* On MSVC, we have these handy functions. We can ignore their return
 * values, since we will never supply val == 0. */
__inline int ctz32(unsigned long val)
{
	DWORD zeros = 0;
	_BitScanForward(&zeros, val);
	return (int)zeros;
}
__inline int clz32(unsigned long val)
{
	DWORD zeros = 0;
	_BitScanReverse(&zeros, val);
	return (int)zeros;
}
#ifdef _WIN64
/* According to the documentation, these only exist on Win64. */
__inline int ctz64(uint64_t val)
{
	DWORD zeros = 0;
	_BitScanForward64(&zeros, val);
	return (int)zeros;
}
__inline int clz64(uint64_t val)
{
	DWORD zeros = 0;
	_BitScanReverse64(&zeros, val);
	return (int)zeros;
}
#else
__inline int ctz64(uint64_t val)
{
	uint32_t lo = (uint32_t)val;
	uint32_t hi = (uint32_t)(val >> 32);
	return lo ? ctz32(lo) : 32 + ctz32(hi);
}
__inline int clz64(uint64_t val)
{
	uint32_t lo = (uint32_t)val;
	uint32_t hi = (uint32_t)(val >> 32);
	return hi ? clz32(hi) : 32 + clz32(lo);
}
#endif
#else
/*we have to impl these functions by ourselves*/
/* uint64_t will take 8 times assignment to be reversed */
inline void reverse(unsigned char *src, const unsigned int length)
{
	unsigned char temp;
	for (unsigned int i = 0; i < (length >> 1); i++)
	{
		temp = src[i];
		src[i] = src[length - i - 1];
		src[length - i - 1] = temp;
	}
}
inline static int get_leading_zeros_size(char x)
{
	return get_leading_zeros_size((unsigned char)x);
}
inline static int get_leading_zeros_size(unsigned char x)
{
	// x = 0000 0010, n = 8,
	// y = x >>4 = 0000 0000
	// y = x >>2 = 0000 0000
	// y = x >>1 = 0000 0001 != 0 -> return 8-2 = 6

	// x = 0100 0000, n = 8,
	// y = x >>4 = 0000 0100 != 0 -> n = 4, x = 0000 0100
	// y = x >>2 = 0000 0001 != 0 -> n = 2, x = 0000 0001
	// y = x >>1 = 0000 0000 != 0 -> return 2-1 = 1

	unsigned char y;
	int n;

	n = 8;
	y = x >> 4;
	if (y != 0)
	{
		n = n - 4;
		x = y;
	}
	y = x >> 2;
	if (y != 0)
	{
		n = n - 2;
		x = y;
	}
	y = x >> 1;
	if (y != 0)
	return (int)(n - 2);
	return (int)(n - 1);
}
inline static int get_trailing_zeros_size(unsigned char x)
{
	// x = 0000 1000,
	// y = x << 4 = 1000 0000 != 0 -> n = 4, x = 1000 0000
	// y = x <<2 = 0000 0000
	// y = x <<1 = 0000 0000 == 0 -> return 4-1 = 3
	// x = 0000 0010,
	// y = x << 4 = 0010 0000 != 0 -> n = 4, x = 0010 0000
	// y = x <<2 = 1000 0000 != 0 -> n = 2
	// y = x <<1 = 0000 0000 == 0 -> return 2-1 = 1
	// x = 0010 0000,
	// y = x << 4 = 0000 0000
	// y = x <<2 =  1000 0000 != 0, n = 6, x = 1000 0000
	// y = x <<1 =  0000 0000 != 0 -> n-2 = 8-2 = 6
	unsigned char y;
	int n;

	n = 8;
	y = x << 4;
	if (y != 0)
	{
		n = n - 4;
		x = y;
	}
	y = x << 2;
	if (y != 0)
	{
		n = n - 2;
		x = y;
	}
	y = x << 1;
	if (y != 0)
	return (int)(n - 2);
	return (int)(n - 1);
}
inline static int get_leading_zeros_size(unsigned short x)
{
	unsigned short y;
	int n;

	n = 16;
	y = x >> 8;
	if (y != 0)
	{
		n = n - 8;
		x = y;
	}
	y = x >> 4;
	if (y != 0)
	{
		n = n - 4;
		x = y;
	}
	y = x >> 2;
	if (y != 0)
	{
		n = n - 2;
		x = y;
	}
	y = x >> 1;
	if (y != 0)
	return (int)(n - 2);
	return (int)(n - 1);
}
inline static int get_trailing_zeros_size(unsigned short x)
{
	unsigned short y;
	int n;

	n = 16;
	y = x << 8;
	if (y != 0)
	{
		n = n - 8;
		x = y;
	}
	y = x << 4;
	if (y != 0)
	{
		n = n - 4;
		x = y;
	}
	y = x << 2;
	if (y != 0)
	{
		n = n - 2;
		x = y;
	}
	y = x << 1;
	if (y != 0)
	return (int)(n - 2);
	return (int)(n - 1);
}
inline static int get_leading_zeros_size(short x)
{
	return get_leading_zeros_size((unsigned short)x);
}
inline static int get_leading_zeros_size(unsigned int x)
{
	unsigned int y;
	int n;

	n = 32;
	y = x >> 16;
	if (y != 0)
	{
		n = n - 16;
		x = y;
	}
	y = x >> 8;
	if (y != 0)
	{
		n = n - 8;
		x = y;
	}
	y = x >> 4;
	if (y != 0)
	{
		n = n - 4;
		x = y;
	}
	y = x >> 2;
	if (y != 0)
	{
		n = n - 2;
		x = y;
	}
	y = x >> 1;
	if (y != 0)
	return (int)(n - 2);
	return (int)(n - 1);
}
inline static int get_trailing_zeros_size(unsigned int x)
{
	unsigned int y;
	int n;

	n = 32;
	y = x << 16;
	if (y != 0)
	{
		n = n - 16;
		x = y;
	}
	y = x << 8;
	if (y != 0)
	{
		n = n - 8;
		x = y;
	}
	y = x << 4;
	if (y != 0)
	{
		n = n - 4;
		x = y;
	}
	y = x << 2;
	if (y != 0)
	{
		n = n - 2;
		x = y;
	}
	y = x << 1;
	if (y != 0)
	return (int)(n - 2);
	return (int)(n - 1);
}
inline static int get_leading_zeros_size(int x)
{
	return get_leading_zeros_size((unsigned int)x);
}
inline static int get_leading_zeros_size(uint64_t x)
{
	uint64_t y;
	int n;

	n = 64;
	y = x >> 32;
	if (y != 0)
	{
		n = n - 32;
		x = y;
	}
	y = x >> 16;
	if (y != 0)
	{
		n = n - 16;
		x = y;
	}
	y = x >> 8;
	if (y != 0)
	{
		n = n - 8;
		x = y;
	}
	y = x >> 4;
	if (y != 0)
	{
		n = n - 4;
		x = y;
	}
	y = x >> 2;
	if (y != 0)
	{
		n = n - 2;
		x = y;
	}
	y = x >> 1;
	if (y != 0)
	return (int)(n - 2);
	return (int)(n - 1);
}
inline static int get_trailing_zeros_size(uint64_t x)
{
	// x = 8 = 7 zero chars 1000
	// x = 3 zero char + 1000 ...
	// x = 1 zero char + 1000 ...
	//x = 0000 1000 ...
	// x = 1000 ...
	// x = 00 ...
	uint64_t y;
	int n = 64;

	y = x << 32;
	if (y != 0)
	{
		n -= 32;
		x = y;
	}
	y = x << 16;
	if (y != 0)
	{
		n -= 16;
		x = y;
	}
	y = x << 8;
	if (y != 0)
	{
		n -= 8;
		x = y;
	}
	y = x << 4;
	if (y != 0)
	{
		n -= 4;
		x = y;
	}
	y = x << 2;
	if (y != 0)
	{
		n -= 2;
		x = y;
	}
	y = x << 1;
	if (y != 0)
	return (int)(n - 2);
	return (int)(n - 1);
}
inline static int get_leading_zeros_size(int64_t x)
{
	return get_leading_zeros_size((uint64_t)x);
}
inline int clz32(unsigned int val)
{
	return get_leading_zeros_size(val);
}
inline int ctz32(unsigned int val)
{
	return get_trailing_zeros_size(val);
}
inline int clz64(uint64_t val)
{
	return get_leading_zeros_size(val);
}
inline int ctz64(uint64_t val)
{
	return get_trailing_zeros_size(val);
}
#endif
}
}
#endif
