//
//#include "watcher_forwarding_baseapp.hpp"
//#include "cstdmf/watcher_path_request.hpp"
//#include "server/watcher_forwarding_collector.hpp"
//
//#include "baseappmgr.hpp"
//#include "baseapp.hpp"
//#include "baseapp/baseapp_int_interface.hpp"
//
//
//// Overridden from ForwardingCollector
//ForwardingCollector *FvBaseAppForwardingWatcher::newCollector(
//	WatcherPathRequestV2 & pathRequest,
//	const std::string & destWatcher,
//	const std::string & targetInfo )
//{
//	ForwardingCollector *collector =
//		new ForwardingCollector( pathRequest, destWatcher );
//	if (collector == NULL)
//	{
//		FV_ERROR_MSG( "newCollector: Failed to create new watcher "
//					"ForwardingCollector.\n" );
//		return NULL;
//	}
//
//	AddressList *targetList = this->getComponentAddressList( targetInfo );
//
//	if (!collector->init( BaseAppIntInterface::callWatcher,
//		FvBaseAppMgr::Instance().Nub(), targetList ))
//	{
//		FV_ERROR_MSG( "BAForwardingWatcher::newCollector: Failed to initialise "
//					"ForwardingCollector.\n" );
//		delete collector;
//		collector = NULL;
//
//		if (targetList)
//			delete targetList;
//	}
//
//	return collector;
//}
//
//
///**
// * Generate a list of BaseApp(s) (Component ID, Address pair) as determined by
// * the target information for the forwarding watchers.
// * 
// * @param targetInfo A string describing which component(s) to add to the list.
// *
// * @returns A list of AddressPair's to forward a watcher request to.
// */
//AddressList * FvBaseAppForwardingWatcher::getComponentAddressList(
//	const std::string & targetInfo )
//{
//	AddressList * list = new AddressList;
//	if (list == NULL)
//	{
//		FV_ERROR_MSG( "BAForwardingWatcher::getComponentList: Failed to "
//					"generate component list.\n" );
//		return list;
//	}
//
//	FvBaseAppMgr::FvBaseApps & baseApps = FvBaseAppMgr::Instance().baseApps();
//
//	if (targetInfo == TARGET_LEAST_LOADED)
//	{
//		FvBaseApp *baseApp = FvBaseAppMgr::Instance().findBestBaseApp();
//		if (baseApp)
//		{
//			list->push_back(
//					AddressPair( baseApp->Addr(), baseApp->id() ) );
//		}
//
//	}
//	else if (targetInfo == TARGET_ALL)
//	{
//		FvBaseAppMgr::FvBaseApps::iterator iter = baseApps.begin();
//		while (iter != baseApps.end())
//		{
//			list->push_back(
//				AddressPair( (*iter).second->Addr(), (*iter).second->Id() ) );
//			iter++;
//		}
//
//	}
//	else
//	{
//		FV_ERROR_MSG( "BaseAppMgr ForwardingWatcher::getComponentAddressList: "
//					"Invalid forwarding destination '%s'.\n",
//					targetInfo.c_str() );
//		if (list)
//			delete list;
//		list = NULL;
//	}
////	else
////	{
////		// TODO: possible optimsiation here 
////		ComponentIDList targetList = this->getComponentIDList( targetInfo );
////		BaseAppMgr::BaseApps::iterator baseAppIter = baseApps.begin();
////		while (baseAppIter != baseApps.end())
////		{
////			BaseApp *baseApp = (*baseAppIter).second;
////			ComponentIDList::iterator targetIter = targetList.begin();
////			while (targetIter != targetList.end())
////			{
////				// The baseapp matches a baseapp in our list
////				if (*targetIter == baseApp->id())
////				{
////					list->push_back(
////						AddressPair( baseApp->Addr(), baseApp->id() ) );
////				}
////
////				targetIter++;
////			}
////
////			baseAppIter++;
////		}
////	}
//
//	return list;
//}
