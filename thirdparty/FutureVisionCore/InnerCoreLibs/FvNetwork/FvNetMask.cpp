#include "FvNetMask.h"

#include <stdio.h>

FvNetMask::FvNetMask() :
	m_uiMask(0),
	m_iBits(0)
{
}

bool FvNetMask::Parse( const char* str )
{
	int a, b, c, d;

	if (sscanf( str, "%d.%d.%d.%d/%d", &a, &b, &c, &d, &m_iBits ) != 5)
	{
		m_iBits = 0;
		return false;
	}

	m_uiMask = (d << 24) | (c << 16) | (b << 8) | a;
	return true;
}

bool FvNetMask::ContainsAddress( unsigned long addr ) const
{
	if (m_iBits == 0)
		return true; 
	else
		return (addr & (0xffffffff >> (32 - m_iBits))) == m_uiMask;
}

void FvNetMask::Clear()
{
	m_iBits = 0;
	m_uiMask = 0;
}
