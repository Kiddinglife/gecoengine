#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <limits>
#include <math.h>

#include <FvPrintf.h>

#include "FvNetFormatStringHandler.h"
#include "FvNetBSDSNPrintf.h"

#  define VA_COPY(dest, src) va_copy(dest, src)

#if !defined(FV_HAVE_SNPRINTF) || !defined(FV_HAVE_VSNPRINTF)

#define DP_S_DEFAULT 0
#define DP_S_FLAGS   1
#define DP_S_MIN     2
#define DP_S_DOT     3
#define DP_S_MAX     4
#define DP_S_MOD     5
#define DP_S_CONV    6
#define DP_S_DONE    7

#define DP_F_MINUS 	(1 << 0)
#define DP_F_PLUS  	(1 << 1)
#define DP_F_SPACE 	(1 << 2)
#define DP_F_NUM   	(1 << 3)
#define DP_F_ZERO  	(1 << 4)
#define DP_F_UP    	(1 << 5)
#define DP_F_UNSIGNED 	(1 << 6)

#define char_to_int(p) ((p)- '0')
#ifndef MAX
# define MAX(p,q) (((p) >= (q)) ? (p) : (q))
#endif

void HandleFormatString( const char *pcFormat, FvNetFormatStringHandler &kHandler )
{
	char ch;
	int min;
	int max;
	int state;
	int flags;
	int cflags;
	int vflags;

	int pos;
	int strstart, strend;
	const char *format_start = pcFormat;

	state = DP_S_DEFAULT;
	flags = cflags = vflags = min = 0;
	max = -1;
	ch = *pcFormat++;

	pos = 0;
	strstart = strend = 0;

	while (state != DP_S_DONE) {
		if (ch == '\0')
			state = DP_S_DONE;

		switch(state) {

		case DP_S_DEFAULT:
			if (ch == '%') {
				strend = (pcFormat - format_start) - 1;
				if (strstart != strend)
					kHandler.OnString(strstart, strend);
				state = DP_S_FLAGS;
			}
			ch = *pcFormat++;
			break;
		case DP_S_FLAGS:
			switch (ch) {
			case '-':
				flags |= DP_F_MINUS;
				ch = *pcFormat++;
				break;
			case '+':
				flags |= DP_F_PLUS;
				ch = *pcFormat++;
				break;
			case ' ':
				flags |= DP_F_SPACE;
				ch = *pcFormat++;
				break;
			case '#':
				flags |= DP_F_NUM;
				ch = *pcFormat++;
				break;
			case '0':
				flags |= DP_F_ZERO;
				ch = *pcFormat++;
				break;
			default:
				state = DP_S_MIN;
				break;
			}
			break;
		case DP_S_MIN:
			if (isdigit((unsigned char)ch)) {
				min = 10*min + char_to_int (ch);
				ch = *pcFormat++;
			} else if (ch == '*') {
				min = 0;
				vflags |= FV_NET_VARIABLE_MIN_WIDTH;
				ch = *pcFormat++;
				state = DP_S_DOT;
			} else {
				state = DP_S_DOT;
			}
			break;
		case DP_S_DOT:
			if (ch == '.') {
				state = DP_S_MAX;
				ch = *pcFormat++;
			} else {
				state = DP_S_MOD;
			}
			break;
		case DP_S_MAX:
			if (isdigit((unsigned char)ch)) {
				if (max < 0)
					max = 0;
				max = 10*max + char_to_int (ch);
				ch = *pcFormat++;
			} else if (ch == '*') {
				max = -1;
				vflags |= FV_NET_VARIABLE_MAX_WIDTH;
				ch = *pcFormat++;
				state = DP_S_MOD;
			} else {
				state = DP_S_MOD;
			}
			break;
		case DP_S_MOD:
			switch (ch) {
			case 'h':
				cflags = FV_NET_DP_C_SHORT;
				ch = *pcFormat++;
				break;
			case 'l':
				cflags = FV_NET_DP_C_LONG;
				ch = *pcFormat++;
				if (ch == 'l') {
					cflags = FV_NET_DP_C_LLONG;
					ch = *pcFormat++;
				}
				break;
			case 'z': 
				cflags = FV_NET_DP_C_LONG;
				ch = *pcFormat++;
				break;
			case 'L':
				cflags = FV_NET_DP_C_LDOUBLE;
				ch = *pcFormat++;
				break;
			default:
				break;
			}
			state = DP_S_CONV;
			break;
		case DP_S_CONV:
			switch (ch) {
			case 'd':
			case 'i':
				kHandler.OnToken('d', cflags, min, max, flags, 10, vflags);
				break;
			case 'o':
				flags |= DP_F_UNSIGNED;
				kHandler.OnToken('o', cflags, min, max, flags, 8, vflags);
				break;
			case 'u':
				flags |= DP_F_UNSIGNED;
				kHandler.OnToken('u', cflags, min, max, flags, 10, vflags);
				break;
			case 'X':
				flags |= DP_F_UP;
			case 'x':
				flags |= DP_F_UNSIGNED;
				kHandler.OnToken('x', cflags, min, max, flags, 16, vflags);
				break;
			case 'f':
				kHandler.OnToken('f', cflags, min, max, flags, 0, vflags);
				break;
			case 'E':
				flags |= DP_F_UP;
			case 'e':
				kHandler.OnToken('e', cflags, min, max, flags, 0, vflags);
				break;
			case 'G':
				flags |= DP_F_UP;
			case 'g':
				kHandler.OnToken('g', cflags, min, max, flags, 0, vflags);
				break;
			case 'c':
				kHandler.OnToken('c', cflags, min, max, flags, 0, vflags);
				break;
			case 's':
				kHandler.OnToken('s', cflags, min, max, flags, 0, vflags);
				break;
			case 'p':
				kHandler.OnToken('p', cflags, min, max, flags, 16, vflags);
				break;
			case '%':
				kHandler.OnString( pcFormat - format_start - 1,
					pcFormat - format_start );
				break;
			case 'w':
				ch = *pcFormat++;
				break;
			default:
				break;
			}
			ch = *pcFormat++;
			state = DP_S_DEFAULT;
			flags = cflags = vflags = min = 0;
			max = -1;
			strstart = strend = (pcFormat - format_start) - 1;
			break;
		case DP_S_DONE:
			break;
		default:
			break; 
		}

		pos++;
	}

	strend = (pcFormat - format_start) - 1;
	if (strstart != strend)
		kHandler.OnString(strstart, strend);
	return;
}

void BSDFormatString( const char *pcValue, int iFlags, 
	int iMin, int iMax,FvString &kOut )
{
	int padlen, strln;
	int cnt = 0;

#ifdef FV_DEBUG_SNPRINTF
	printf("fmtstr min=%d max=%d s=[%s]\n", iMin, iMax, pcValue);
#endif
	if (pcValue == 0) {
		pcValue = "(null)";
	}

	for (strln = 0; (iMax == -1 || strln < iMax) && pcValue[strln]; ++strln);

	padlen = iMin - strln;
	if (padlen < 0)
		padlen = 0;
	if (iFlags & DP_F_MINUS)
		padlen = -padlen;

	while ((padlen > 0) && (cnt < iMax || iMax == -1)) {
		kOut.push_back( ' ' );
		--padlen;
		++cnt;
	}
	while (*pcValue && (cnt < iMax || iMax == -1)) {
		kOut.push_back( *pcValue++ );
		++cnt;
	}
	while ((padlen < 0) && (cnt < iMax || iMax == -1)) {
		kOut.push_back( ' ' );
		++padlen;
		++cnt;
	}
}

void BSDFormatInt( FV_NET_LLONG iValue, FvUInt8 uiBase, int iMin, int iMax, int iFlags,
				  FvString &kOut )
{
	char signvalue = 0;
	FV_NET_ULLONG uvalue;
	char convert[20];
	int place = 0;
	int spadlen = 0;
	int zpadlen = 0;
	int caps = 0;

	if (iMax < 0)
		iMax = 0;

	uvalue = iValue;

	if(!(iFlags & DP_F_UNSIGNED)) {
		if( iValue < 0 ) {
			signvalue = '-';
			uvalue = -iValue;
		} else {
			if (iFlags & DP_F_PLUS)
				signvalue = '+';
			else if (iFlags & DP_F_SPACE)
				signvalue = ' ';
		}
	}

	if (iFlags & DP_F_UP) caps = 1;

	do {
		convert[place++] =
			(caps? "0123456789ABCDEF":"0123456789abcdef")
			[uvalue % (unsigned)uiBase  ];
		uvalue = (uvalue / (unsigned)uiBase );
	} while(uvalue && (place < 20));
	if (place == 20) place--;
	convert[place] = 0;

	zpadlen = iMax - place;
	spadlen = iMin - MAX (iMax, place) - (signvalue ? 1 : 0);
	if (zpadlen < 0) zpadlen = 0;
	if (spadlen < 0) spadlen = 0;
	if (iFlags & DP_F_ZERO) {
		zpadlen = MAX(zpadlen, spadlen);
		spadlen = 0;
	}
	if (iFlags & DP_F_MINUS)
		spadlen = -spadlen;

#ifdef FV_DEBUG_SNPRINTF
	printf("zpad: %d, spad: %d, min: %d, max: %d, place: %d, base: %d\n",
		zpadlen, spadlen, min, max, place, uiBase);
#endif

	while (spadlen > 0) {
		kOut.push_back( ' ' );
		--spadlen;
	}

	if (signvalue)
		kOut.push_back( signvalue );

	if (zpadlen > 0) {
		while (zpadlen > 0) {
			kOut.push_back( '0' );
			--zpadlen;
		}
	}

	while (place > 0)
		kOut.push_back( convert[--place] );

	while (spadlen < 0) {
		kOut.push_back( ' ' );
		++spadlen;
	}
}

static FV_NET_LDOUBLE abs_val(FV_NET_LDOUBLE value)
{
	FV_NET_LDOUBLE result = value;

	if (value < 0)
		result = -value;

	return result;
}

static FV_NET_LDOUBLE POW10(int exp)
{
	FV_NET_LDOUBLE result = 1;

	while (exp) {
		result *= 10;
		exp--;
	}

	return result;
}

static FV_NET_LLONG ROUND(FV_NET_LDOUBLE value)
{
	FV_NET_LLONG intpart;

	intpart = (FV_NET_LLONG)value;
	value = value - intpart;
	if (value >= 0.5) intpart++;

	return intpart;
}

static double my_modf(double x0, double *iptr)
{
	int i;
	long l;
	double x = x0;
	double f = 1.0;

	for (i=0;i<100;i++) {
		l = (long)x;
		if (l <= (x+1) && l >= (x-1)) break;
		x *= 0.1;
		f *= 10.0;
	}

	if (i == 100) {
		(*iptr) = 0;
		return 0;
	}

	if (i != 0) {
		double i2;
		double ret;

		ret = my_modf(x0-l*f, &i2);
		(*iptr) = l*f + i2;
		return ret;
	}

	(*iptr) = l;
	return x - (*iptr);
}

void BSDFormatFloat( FV_NET_LDOUBLE fValue, int iMin, int iMax, int iFlags,
					FvString &kOut )
{
	if (FvIsNAN( fValue ))
	{
		kOut.append( "nan" );
		return;
	}

	if (FvIsINF( fValue ))
	{
		if (fValue < 0)
			kOut.append( "-inf" );
		else
			kOut.append( "inf" );

		return;
	}

	char signvalue = 0;
	double ufvalue;
	char iconvert[311];
	char fconvert[311];
	int iplace = 0;
	int fplace = 0;
	int padlen = 0;
	int zpadlen = 0;
	int caps = 0;
	int idx;
	double intpart;
	double fracpart;
	double temp;

	if (iMax < 0)
		iMax = 6;

	ufvalue = abs_val (fValue);

	if (fValue < 0) {
		signvalue = '-';
	} else {
		if (iFlags & DP_F_PLUS) {
			signvalue = '+';
		} else {
			if (iFlags & DP_F_SPACE)
				signvalue = ' ';
		}
	}

	if (iMax > 16)
		iMax = 16;

	temp = ufvalue;
	my_modf(temp, &intpart);

	fracpart = (double)ROUND((POW10(iMax)) * (ufvalue - intpart));

	if (fracpart >= POW10(iMax)) {
		intpart++;
		fracpart -= POW10(iMax);
	}

	do {
		temp = intpart*0.1;
		my_modf(temp, &intpart);
		idx = (int) ((temp -intpart +0.05)* 10.0);
		iconvert[iplace++] =
			(caps? "0123456789ABCDEF":"0123456789abcdef")[idx];
	} while (intpart && (iplace < 311));
	if (iplace == 311) iplace--;
	iconvert[iplace] = 0;

	if (fracpart)
	{
		do {
			temp = fracpart*0.1;
			my_modf(temp, &fracpart);
			idx = (int) ((temp -fracpart +0.05)* 10.0);
			fconvert[fplace++] =
				(caps? "0123456789ABCDEF":"0123456789abcdef")[idx];
		} while(fracpart && (fplace < 311));
		if (fplace == 311) fplace--;
	}
	fconvert[fplace] = 0;

	padlen = iMin - iplace - iMax - 1 - ((signvalue) ? 1 : 0);
	zpadlen = iMax - fplace;
	if (zpadlen < 0) zpadlen = 0;
	if (padlen < 0)
		padlen = 0;
	if (iFlags & DP_F_MINUS)
		padlen = -padlen; 

	if ((iFlags & DP_F_ZERO) && (padlen > 0)) {
		if (signvalue) {
			kOut.push_back( signvalue );
			--padlen;
			signvalue = 0;
		}
		while (padlen > 0) {
			kOut.push_back( '0' );
			--padlen;
		}
	}
	while (padlen > 0) {
		kOut.push_back( ' ' );
		--padlen;
	}
	if (signvalue)
		kOut.push_back( signvalue );

	while (iplace > 0)
		kOut.push_back( iconvert[--iplace] );

#ifdef FV_DEBUG_SNPRINTF
	printf("fmtfp: fplace=%d zpadlen=%d\n", fplace, zpadlen);
#endif

	if (iMax > 0) {
		kOut.push_back( '.' );

		while (zpadlen > 0) {
			kOut.push_back( '0' );
			--zpadlen;
		}

		while (fplace > 0)
			kOut.push_back( fconvert[--fplace] );
	}

	while (padlen < 0) {
		kOut.push_back( ' ' );
		++padlen;
	}
}

#endif // !defined(FV_HAVE_SNPRINTF) || !defined(FV_HAVE_VSNPRINTF)
