//{future header message}
#ifndef __FvBaseApp_H__
#define __FvBaseApp_H__

#include <FvNetChannel.h>
#include <FvBackupHash.h>

class FvBackupBaseApp;

class FvBaseApp: public FvNetChannelOwner
{
public:
	FvBaseApp( const FvNetAddress & intAddr,
			const FvNetAddress & extAddr,
			int id );

	float Load() const { return m_fLoad; }

	void UpdateLoad( float load, int numBases, int numProxies )
	{
		m_fLoad = load;
		m_iNumBases = numBases;
		m_iNumProxies = numProxies;
	}

	bool HasTimedOut( FvUInt64 currTime, FvUInt64 timeoutPeriod,
		   FvUInt64 timeSinceHeardAny ) const;

	const FvNetAddress & ExternalAddr() const { return m_kExternalAddr; }

	int NumBases() const	{ return m_iNumBases; }
	int NumProxies() const	{ return m_iNumProxies; }

	FvBaseAppID Id() const	{ return m_iId; }
	void Id( int id )		{ m_iId = id; }

	void SetBackup( FvBackupBaseApp * pBackup )	{ m_pkBackup = pBackup; }
	FvBackupBaseApp * GetBackup() const			{ return m_pkBackup; }

	void AddEntity();

	static FvWatcher * MakeWatcher();

	const FvBackupHash & BackupHash() const { return m_kBackupHash; }
	FvBackupHash & BackupHash() { return m_kBackupHash; }

	const FvBackupHash & NewBackupHash() const { return m_kNewBackupHash; }
	FvBackupHash & NewBackupHash() { return m_kNewBackupHash; }

private:
	FvNetAddress		m_kExternalAddr;
	FvBaseAppID			m_iId;

	float				m_fLoad;
	FvInt32				m_iNumBases;
	FvInt32				m_iNumProxies;

	FvBackupBaseApp *	m_pkBackup;

	FvBackupHash		m_kBackupHash;
	FvBackupHash		m_kNewBackupHash;
};


class FvBackupBaseApp : public FvNetChannelOwner
{
public:
	typedef std::set< FvBaseApp * > FvBackedUpSet;

	FvBackupBaseApp( const FvNetAddress & addr, int id );

	bool Backup( FvBaseApp & cache );
	bool StopBackup( FvBaseApp & cache, bool tellBackupBaseApp );

	FvBaseAppID Id() const					{ return m_iId; }
	float Load() const						{ return m_fLoad; }

	void UpdateLoad( float load )
	{
		m_fLoad = load;
	}

private:
	FvBaseAppID			m_iId;
	float				m_fLoad;
public:
	FvBackedUpSet		m_kBackedUp;
};

#endif // __FvBaseApp_H__
