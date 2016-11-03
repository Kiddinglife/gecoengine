//{future header message}
#ifndef __FvWatcherTest_H__
#define __FvWatcherTest_H__

#include "FvWatcher.h"
#include <map>

class WatcherClass
{
public:
	WatcherClass()
	{
		FV_WATCH("WatcherClass/name", m_kName, FvWatcher::WT_READ_WRITE, "this is name");
		FV_WATCH("WatcherClass/length", m_uiLength, FvWatcher::WT_READ_WRITE, "this is length" );
		FV_WATCH("WatcherClass/sex", m_bSex, FvWatcher::WT_READ_WRITE, "this is sex" );
	}
	FvString m_kName;
	FvUInt32 m_uiLength;
	bool m_bSex;
};

class FvWatcherTest : public FvWatcherVisitor
{
public:
	bool RunTest();

	bool Visit( FvWatcher::Mode mode,
		const FvString & label,
		const FvString & desc,
		const FvString & valueStr );

	bool Visit( FvWatcher::Mode mode,
		const FvString & label,
		const FvString & valueStr );
};



#endif /* __FvWatcherTest_H__ */