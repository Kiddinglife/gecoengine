#pragma once

#include "FvCallback.h"

template<class TNotifier>
class FvAsynNotifier : public TNotifier
{
	FV_NOT_COPY_COMPARE(FvAsynNotifier);
public:
	FvAsynNotifier(){}
	template<class TListener>
	FvAsynNotifier(TListener& kListener):TNotifier(kListener){}
	~FvAsynNotifier(){}

	void AsynNotify(){m_FrameEnd_CB.Exec(*this);}
	void DetachAsyn(){m_FrameEnd_CB.Detach();TNotifier::Detach();}

private:
	//
	_CALLBACK_FRIEND_CLASS_;
	void _AsyNofity(){TNotifier::Notify();}

	FvFrameEndCallback0<FvAsynNotifier, &_AsyNofity> m_FrameEnd_CB;

};