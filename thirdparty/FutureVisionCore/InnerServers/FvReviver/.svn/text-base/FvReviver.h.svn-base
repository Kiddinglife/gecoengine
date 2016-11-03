//{future header message}
#ifndef __FvReviver_H__
#define __FvReviver_H__

#include <FvSingleton.h>
#include <FvNetMachineGuard.h>
#include <FvNetNub.h>

class FvComponentReviver;
typedef std::vector< FvComponentReviver * > FvComponentRevivers;

class FvReviver : public FvNetTimerExpiryHandler,
	public FvSingleton< FvReviver >
{
public:
	FvReviver( FvNetNub & nub );
	virtual ~FvReviver();

	bool Init( int argc, char * argv[] );
	bool QueryMachinedSettings();

	void Run();
	void ShutDown();

	void Revive( const char * createComponent );

	bool HasEnabledComponents() const;

	virtual int HandleTimeout( FvNetTimerID id, void * arg );

	int PingPeriod() const		{ return m_iPingPeriod; }
	int MaxPingsToMiss() const	{ return m_iMaxPingsToMiss; }

	void MarkAsDirty()			{ m_bIsDirty = true; }

	FvNetNub &nub()		{ return m_kNub; }

	class TagsHandler : public FvNetMachineGuardMessage::ReplyHandler
	{
	public:
		TagsHandler( FvReviver &reviver ) : m_kReviver( reviver ) {}
		virtual bool OnTagsMessage( FvNetTagsMessage &tm, FvUInt32 addr );

	private:
		FvReviver &m_kReviver;
	};

private:
	enum TimeoutType
	{
		TIMEOUT_REATTACH
	};

	FvNetNub &m_kNub;
	int m_iPingPeriod;
	int m_iMaxPingsToMiss;
	FvComponentRevivers m_kComponents;

	bool m_bShuttingDown;
	bool m_bShutDownOnRevive;
	bool m_bIsDirty;
};

#endif // __FvReviver_H__
