//{future header message}
#ifndef __FvIdClient_H__
#define __FvIdClient_H__

#include <queue>
#include <FvNetNub.h>
#include "FvServerCommon.h"



class FV_SERVERCOMMON_API FvIDClient : private FvNetReplyMessageHandler
{
	typedef FvInt32 ID;
public:
	FvIDClient();
	virtual ~FvIDClient() {}

	bool	Init(FvNetChannel * pChannel,
			const FvNetInterfaceElement& getMoreMethod,
			const FvNetInterfaceElement& putBackMethod,
			size_t criticallyLowSize,
			size_t lowSize,
			size_t desiredSize,
			size_t highSize,
			bool bBlockAtInit = true);

	void	PutUsedID( ID );

	ID		GetID();

	void	ReturnIDs();

	bool	IsInited() { return m_bInited; }

protected:
	typedef std::queue<ID> IDQueue;

	static void PlaceIDsOntoStream( size_t n, IDQueue&, FvBinaryOStream& );
	static void RetrieveIDsFromStream( IDQueue&, FvBinaryIStream& );

	IDQueue m_kReadyIDs;

	FvNetChannel * m_pkChannel;

	void PerformUpdates( bool isEmergency );

	size_t m_uiHighSize;
	size_t m_uiDesiredSize;
	size_t m_uiLowSize;
	size_t m_uiCriticallyLowSize;

private:
	struct LockedID
	{
		LockedID( ID id );

		FvUInt64 m_uiUnlockTime;
		ID m_iId;
	};

	std::queue<LockedID> m_kLockedIDs;

protected:
	bool m_bPendingRequest;

private:
	bool m_bInEmergency;
	bool m_bInited;

	void GetMoreIDs();
	bool GetMoreIDsBlocking();
	void GetMoreIDs( FvNetReplyMessageHandler * pHandler );

	void PutBackIDs();

	const FvNetInterfaceElement * m_pkGetMoreMethod;
	const FvNetInterfaceElement * m_pkPutBackMethod;

	void HandleMessage( const FvNetAddress& source,
			FvNetUnpackedMessageHeader& header, FvBinaryIStream& data,
			void * arg );
	void HandleException( const FvNetNubException& exception, void * arg );
};



#endif // __FvIdClient_H__
