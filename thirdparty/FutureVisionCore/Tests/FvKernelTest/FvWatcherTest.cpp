//{future source message}
#include "FvWatcherTest.h"
#include <iostream>

bool g_trace = false;
bool g_blendNodes = true;

bool FvWatcherTest::RunTest()
{
	FV_WATCH("tracker/trace", g_trace, FvWatcher::WT_READ_WRITE, "Enables debugging output statements for trackers.");
	FV_WATCH("tracker/blendNodes", g_blendNodes, FvWatcher::WT_READ_WRITE, "Enables blending for the trackers, "
		"when enabled, the trackers will blend smoothly between different directions." );

	FvWatcher::RootWatcher().VisitChildren(NULL,"tracker",*this);
	WatcherClass *pkClass1 = new WatcherClass;
	pkClass1->m_kName = "class1";
	pkClass1->m_bSex = false;
	pkClass1->m_uiLength = 12;
	WatcherClass *pkClass2  = new WatcherClass;
	pkClass2->m_kName = "class2";
	pkClass2->m_bSex = true;
	pkClass2->m_uiLength = 17;
	FvWatcher::RootWatcher().VisitChildren(NULL,"WatcherClass/",*this);

	delete pkClass1;
	delete pkClass2;

	return true;
}

bool FvWatcherTest::Visit( FvWatcher::Mode mode,
				   const FvString & label,
				   const FvString & desc,
				   const FvString & valueStr )
{

	return true;
}

bool FvWatcherTest::Visit( FvWatcher::Mode mode,
		   const FvString & label,
		   const FvString & valueStr )
{
	return true;
}