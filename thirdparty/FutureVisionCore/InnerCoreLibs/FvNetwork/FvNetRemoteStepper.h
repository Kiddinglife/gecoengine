//{future header message}
#ifndef __FvNetRemoteStepper_H__
#define __FvNetRemoteStepper_H__

#include "FvNetwork.h"

#include <FvBaseTypes.h>

class FvNetEndpoint;

class FV_NETWORK_API FvNetRemoteStepper
{
public:
	FvNetRemoteStepper();
	~FvNetRemoteStepper();

	static void Start();
	static void Step( const FvString & desc, bool wait = true );

private:
	void StepInt( const FvString & desc, bool wait );
	bool Tryaccept();

	FvNetEndpoint *m_pkLEP;
	FvNetEndpoint *m_pkCEP;
};

#endif // __FvNetRemoteStepper_H__
