//{future header message}
#ifndef __FvLoginApp_H__
#define __FvLoginApp_H__

#include <FvMemoryStream.h>
#include <FvSingleton.h>

#include <FvNetInterfaceMinder.h>
#include <FvNetMask.h>
#include <FvNetPublicKeyCipher.h>
#include <FvNetInterfaces.h>

#include <FvXMLSection.h>
#include <FvStreamHelper.h>

#include <FvDocWatcher.h>
#include <FvEMA.h>

#include "FvLoginIntInterface.h"

typedef FvNetChannelOwner DBManager;

class FvLoginApp : public FvSingleton< FvLoginApp >
{
public:
	FvLoginApp( FvNetNub & intNub, FvUInt16 loginPort );

	bool Init( int argc, char * argv[], FvUInt16 loginPort );
	void Run();

	virtual void Login( const FvNetAddress & source,
		FvNetUnpackedMessageHeader & header, FvBinaryIStream & data );

	virtual void Probe( const FvNetAddress & source,
		FvNetUnpackedMessageHeader & header, FvBinaryIStream & data );

	void ControlledShutDown( const FvNetAddress & source,
		FvNetUnpackedMessageHeader & header, FvBinaryIStream & data );

	const FvNetMask &NetMask() const			{ return m_kNetMask; }
	unsigned long ExternalIP() const 		{ return m_uiExternalIP; }

public:
	void SendFailure( const FvNetAddress & addr,
		FvNetReplyID replyID, int status, const char * msg = NULL,
		LogOnParamsPtr pParams = NULL );

	void SendAndCacheSuccess( const FvNetAddress & addr,
			FvNetReplyID replyID, const FvLoginReplyRecord & replyRecord,
			LogOnParamsPtr pParams );

	FvNetNub &GetIntNub()		{ return m_kIntNub; }
	FvNetNub &GetExtNub()		{ return m_kExtNub; }

	DBManager &GetDBManager()					{ return *m_kDBManager.GetChannelOwner(); }
	const DBManager &GetDBManager() const		{ return *m_kDBManager.GetChannelOwner(); }

	bool IsDBReady() const
	{
		return this->GetDBManager().Channel().IsEstablished();
	}

	void ControlledShutDown()
	{
		m_bIsControlledShutDown = true;
		m_kIntNub.BreakProcessing();
	}

	FvUInt64 MaxLoginDelay() const	{ return m_uiMaxLoginDelay; }

	FvUInt8 SystemOverloaded() const
	{ return m_uiSystemOverloaded; }

	void SystemOverloaded( FvUInt8 status )
	{
		m_uiSystemOverloaded = status;
		m_uiSystemOverloadedTime = Timestamp();
	}


private:

	class CachedLogin
	{
	public:
		CachedLogin() : m_uiCreationTime( 0 ) {}

		bool IsTooOld() const;
		bool IsPending() const;

		void pParams( LogOnParamsPtr pParams ) { m_spParams = pParams; }
		LogOnParamsPtr pParams() { return m_spParams; }

		void ReplyRecord( const FvLoginReplyRecord & record );
		const FvLoginReplyRecord &ReplyRecord() const { return m_kReplyRecord; }

		void Reset() { m_uiCreationTime = 0; }

	private:
		FvUInt64 m_uiCreationTime;
		LogOnParamsPtr m_spParams;
		FvLoginReplyRecord m_kReplyRecord;
	};

	bool HandleResentPendingAttempt( const FvNetAddress & addr,
		FvNetReplyID replyID );
	bool HandleResentCachedAttempt( const FvNetAddress & addr,
		LogOnParamsPtr pParams, FvNetReplyID replyID );

	void SendSuccess( const FvNetAddress & addr,
		FvNetReplyID replyID, const FvLoginReplyRecord & replyRecord,
		const FvString & encryptionKey );

	void RateLimitSeconds( unsigned int newPeriod )
	{ m_uiRateLimitDuration = newPeriod * StampsPerSecond(); }
	unsigned int RateLimitSeconds() const
	{ return m_uiRateLimitDuration / StampsPerSecond(); }

	static FvUInt32 UpdateStatsPeriod() { return UPDATE_STATS_PERIOD; }

#ifdef FV_USE_OPENSSL
	FvNetPublicKeyCipher m_kPrivateKey;
#endif
	FvNetNub &m_kIntNub;
	FvNetNub m_kExtNub;

	FvNetMask m_kNetMask;
	unsigned long m_uiExternalIP;

	bool m_bIsControlledShutDown;
	bool m_bIsProduction;

	FvUInt8 m_uiSystemOverloaded;
	FvUInt64 m_uiSystemOverloadedTime;

	bool m_bAllowLogin;
	bool m_bAllowProbe;
	bool m_bLogProbes;

	typedef std::map< FvNetAddress, CachedLogin > CachedLoginMap;
	CachedLoginMap m_kCachedLoginMap;

	FvAnonymousChannelClient m_kDBManager;

	FvUInt64 m_uiMaxLoginDelay;

	bool m_bAllowUnencryptedLogins;

	FvUInt64 m_uiLastRateLimitCheckTime;
	unsigned int m_uiNumAllowedLoginsLeft;
	int m_iLoginRateLimit;
	FvUInt64 m_uiRateLimitDuration;

	static const FvUInt32 UPDATE_STATS_PERIOD;
	FvNetTimerID m_kStatsTimerID;

	class LoginStats: public FvNetTimerExpiryHandler
	{
	public:
		LoginStats();

		virtual int HandleTimeout( FvNetTimerID id, void * arg )
		{
			this->Update();
			return 0;
		}

		void IncRateLimited() 	{ ++m_kAll.Value(); ++m_kRateLimited.Value(); }

		void IncFails() 		{ ++m_kAll.Value(); ++m_kFails.Value(); }

		void IncPending() 		{ ++m_kAll.Value(); ++m_kPending.Value(); }

		void IncSuccesses() 	{ ++m_kAll.Value(); ++m_kSuccesses.Value(); }

		float Fails() const 		{ return m_kFails.Average(); }

		float RateLimited() const 	{ return m_kRateLimited.Average(); }

		float Pending() const 		{ return m_kPending.Average(); }

		float Successes() const 	{ return m_kSuccesses.Average(); }

		float All() const 			{ return m_kAll.Average(); }

		void Update()
		{
			m_kFails.Sample();
			m_kRateLimited.Sample();
			m_kSuccesses.Sample();
			m_kPending.Sample();
			m_kAll.Sample();
		}

	private:
		FvAccumulatingEMA< FvUInt32 > m_kFails;
		FvAccumulatingEMA< FvUInt32 > m_kRateLimited;
		FvAccumulatingEMA< FvUInt32 > m_kPending;
		FvAccumulatingEMA< FvUInt32 > m_kSuccesses;
		FvAccumulatingEMA< FvUInt32 > m_kAll;

		static const float BIAS;
	};

	LoginStats m_kLoginStats;
};

#endif // __FvLoginApp_H__
