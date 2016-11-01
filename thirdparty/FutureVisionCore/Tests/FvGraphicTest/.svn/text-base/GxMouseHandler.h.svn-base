#pragma once

#include "GxScriptDefine.h"

#include <FvRefList.h>
#include <OISKeyboard.h>
#include <FvApplicationDefine.h>

class  GxMouseHandlerInterface: private FvRefNode1<GxMouseHandlerInterface*>
{
public:

	virtual void End();
	virtual bool IsAcceptMove()const = 0;
	virtual bool IsExclusive()const = 0;

protected:

	virtual void OnPressed() = 0;
	virtual void OnReleased() = 0;
	virtual void OnMoved(const OIS::MouseState& kEvent) = 0;
	virtual void Reset() = 0;

	GxMouseHandlerInterface(void);
	~GxMouseHandlerInterface(void);

	friend class GxMouseController;
};

class  GxMouseController
{
public:
	void AttachFront(GxMouseHandlerInterface& kHandler);
	void AttachBack(GxMouseHandlerInterface& kHandler);

	void OnPressed();
	void OnReleased();
	void OnMoved(const OIS::MouseState& kEvent);
	void ClearPress();

	bool IsAcceptMove()const;
	bool IsExclusive()const;

	void Clear();

private:

	GxMouseHandlerInterface* _GetTop()const;

	FvRefList<GxMouseHandlerInterface*, FvRefNode1> m_kWorkList;
};