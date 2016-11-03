//{future header message}
#ifndef __FvCommonMachineGuard_H__
#define __FvCommonMachineGuard_H__

#include <stdio.h>
#include <string.h>

#include <FvDebug.h>
#include <FvNetPortMap.h>
#include <FvNetEndpoint.h>
#include <FvNetMachineGuard.h>

#include <vector>

#define MAX_BIT_RATE (1<<27)
#define BIT_INCREMENT (MAX_BIT_RATE / 0xFF)
#define MAX_PACKET_RATE 256000
#define PACK_INCREMENT (MAX_PACKET_RATE / 0xFF)

#define FV_MACHINED_VERSION 41

extern const char *g_pcMachinedConfFile;

void InitProcessState( bool bDaemon );
void CleanupProcessState();

void GetProcessorSpeeds( std::vector<float> &kSpeeds );

class FvMachined;

FvUInt32 StartProcess( const char *pcRoot,
	const char *pcResPath,
	const char *pcConfig,
	const char *pcType,
	FvUInt16 uiUID,
	FvUInt16 uiGID,
	const char *pcHome,
	int iArgc,
	const char **pcArgv,
	FvMachined &kMachined );

bool CheckProcess( FvUInt32 uiPID );

void CheckCoreDumps( const char *pcRoot, FvNetUserMessage &kUM );

template <class T>
class FvStat
{
public:
	FvStat() : v1_(), v2_(), v1curr_( true ) {}
	FV_INLINE T& cur() { return v1curr_ ? v1_ : v2_; }
	FV_INLINE T& old() { return v1curr_ ? v2_ : v1_; }
	FV_INLINE T delta() const { return v1curr_ ? v1_ - v2_ : v2_ - v1_; }
	FV_INLINE void update (const T &t) {
		if (v1curr_)
			v2_ = t;
		else
			v1_ = t;
		v1curr_ = !v1curr_;
	}
	FV_INLINE T& next() {
		v1curr_ = !v1curr_;
		return this->cur();
	}
	FV_INLINE bool state() const { return v1curr_; }

private:
	T v1_, v2_;
	bool v1curr_;
};

typedef FvStat< FvUInt64 > FvHighResStat;
FV_IOSTREAM_IMP_BY_MEMCPY(FV_INLINE, FvHighResStat)

struct FvMaxStat
{
	FvHighResStat val, max;
};

struct FvInterfaceInfo
{
	FvString m_kName;
	FvHighResStat m_kBitsTotIn, m_kBitsTotOut, m_kPackTotIn, m_kPackTotOut;
};

struct FvSystemInfo
{
	unsigned int m_uiNumCPUs, m_uiCPUSpeed;
	std::vector< FvMaxStat > m_kCPU;	
	FvMaxStat m_kIOWait;
	FvMaxStat m_kMem;

	FvHighResStat m_kPackTotIn, m_kPackDropIn, m_kPackTotOut, m_kPackDropOut;
	std::vector< struct FvInterfaceInfo > m_kIFInfo;

	FvNetWholeMachineMessage m_kWholeMessage;
	FvNetHighPrecisionMachineMessage m_kHPMessage;
};

struct FvProcessInfo
{
	FvHighResStat m_kCPU, m_kMem;
	unsigned int m_uiAffinity;

	FvNetProcessStatsMessage m_kStatsMessage;

	void Init( const FvNetProcessMessage &kPM );
};

bool UpdateSystemInfoP( FvSystemInfo &kSI );
bool UpdateProcessStats( FvProcessInfo &kPI );

#endif // __FvCommonMachineGuard_H__
