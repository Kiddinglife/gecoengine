//{future header message}
#if defined( DEFINE_INTERFACE_HERE ) || defined( DEFINE_SERVER_HERE )
	#undef __FvLoginInterface_H__
#endif

#ifndef __FvLoginInterface_H__
#define __FvLoginInterface_H__

#include "FvCliSvrCommonDefines.h"

#include <FvMD5.h>
#include <FvSmartPointer.h>
#include <FvDebug.h>
#include <FvMemoryStream.h>
#include <FvNetPublicKeyCipher.h>

#include <cstdlib>

#ifndef FV_LOGIN_INTERFACE_HPP_ONCE_ONLY
#define FV_LOGIN_INTERFACE_HPP_ONCE_ONLY

#include <FvNetInterfaceMinder.h>

#define FV_LOGIN_VERSION FvUInt32( 51 )

const int FV_CLIENT_ONCEOFF_RESEND_PERIOD = 1 * 1000 * 1000; // 1 second

const int FV_CLIENT_ONCEOFF_MAX_RESENDS = 50;

class FvNetPublicKeyCipher;

class FV_CLI_SVR_COMMON_API FvLogOnParams : public FvSafeReferenceCount
{
public:
	typedef FvUInt8 Flags;
	static const Flags HAS_DIGEST = 0x1;
	static const Flags HAS_ALL = 0x1;
	static const Flags PASS_THRU = 0xFF;

	FvLogOnParams() :
		m_uiFlags( HAS_ALL )
	{
		m_uiNonce = std::rand();
		m_kDigest.Clear();
	}

	FvLogOnParams( const FvString & username, const FvString & password,
		const FvString & encryptionKey ) :
		m_uiFlags( HAS_ALL ),
		m_kUsername( username ),
		m_kPassword( password ),
		m_kEncryptionKey( encryptionKey )
	{
		m_uiNonce = std::rand();
		m_kDigest.Clear();
	}

	bool AddToStream( FvBinaryOStream & data, Flags flags = PASS_THRU,
		FvNetPublicKeyCipher * pKey = NULL );

	bool ReadFromStream( FvBinaryIStream & data,
		FvNetPublicKeyCipher * pKey = NULL );

	Flags GetFlags() const { return m_uiFlags; }

	const FvString &Username() const { return m_kUsername; }
	void Username( const FvString & username ) { m_kUsername = username; }

	const FvString &Password() const { return m_kPassword; }
	void Password( const FvString & password ) { m_kPassword = password; }

	const FvString &EncryptionKey() const { return m_kEncryptionKey; }
	void EncryptionKey( const FvString & s ) { m_kEncryptionKey = s; }

	const FvMD5::Digest &Digest() const { return m_kDigest; }
	void Digest( const FvMD5::Digest & digest ){ m_kDigest = digest; }

	bool operator==( const FvLogOnParams & other )
	{
		return m_kUsername == other.m_kUsername &&
			m_kPassword == other.m_kPassword &&
			m_kEncryptionKey == other.m_kEncryptionKey &&
			m_uiNonce == other.m_uiNonce;
	}

private:
	Flags m_uiFlags;
	FvString m_kUsername;
	FvString m_kPassword;
	FvString m_kEncryptionKey;
	FvUInt32 m_uiNonce;
	FvMD5::Digest m_kDigest;
};

typedef FvSmartPointer< FvLogOnParams > LogOnParamsPtr;

FV_INLINE FvBinaryOStream & operator<<( FvBinaryOStream & out, FvLogOnParams & params )
{
	params.AddToStream( out );
	return out;
}


FV_INLINE FvBinaryIStream & operator>>( FvBinaryIStream & in, FvLogOnParams & params )
{
	params.ReadFromStream( in );
	return in;
}

class FvLogOnStatus
{
public:

	enum Status
	{
		NOT_SET,
		LOGGED_ON,
		CONNECTION_FAILED,
		DNS_LOOKUP_FAILED,
		UNKNOWN_ERROR,
		CANCELLED,
		ALREADY_ONLINE_LOCALLY,
		PUBLIC_KEY_LOOKUP_FAILED,
		LAST_CLIENT_SIDE_VALUE = 63,

		LOGIN_MALFORMED_REQUEST,
		LOGIN_BAD_PROTOCOL_VERSION,

		LOGIN_REJECTED_NO_SUCH_USER,
		LOGIN_REJECTED_INVALID_PASSWORD,
		LOGIN_REJECTED_ALREADY_LOGGED_IN,
		LOGIN_REJECTED_BAD_DIGEST,
		LOGIN_REJECTED_DB_GENERAL_FAILURE,
		LOGIN_REJECTED_DB_NOT_READY,
		LOGIN_REJECTED_ILLEGAL_CHARACTERS,
		LOGIN_REJECTED_SERVER_NOT_READY,
		LOGIN_REJECTED_UPDATER_NOT_READY,
		LOGIN_REJECTED_NO_BASEAPPS,
		LOGIN_REJECTED_BASEAPP_OVERLOAD,
		LOGIN_REJECTED_CELLAPP_OVERLOAD,
		LOGIN_REJECTED_BASEAPP_TIMEOUT,
		LOGIN_REJECTED_BASEAPPMGR_TIMEOUT,
		LOGIN_REJECTED_DBMGR_OVERLOAD,
		LOGIN_REJECTED_LOGINS_NOT_ALLOWED,
		LOGIN_REJECTED_RATE_LIMITED,

		LOGIN_CUSTOM_DEFINED_ERROR = 254,
		LAST_SERVER_SIDE_VALUE = 255
	};

	FvLogOnStatus(Status status = NOT_SET) : m_iStatus(status)
	{
	}

	bool Succeeded() const {return m_iStatus == LOGGED_ON;}

	bool Fatal() const
	{
		return
			m_iStatus == CONNECTION_FAILED ||
			m_iStatus == CANCELLED ||
			m_iStatus == UNKNOWN_ERROR;
	}

	bool Okay() const
	{
		return
			m_iStatus == NOT_SET ||
			m_iStatus == LOGGED_ON;
	}

	void operator = (int status) { m_iStatus = status; }

	operator int() const { return m_iStatus; }

	Status value() const { return Status(m_iStatus); }

private:
	int m_iStatus;
};

struct FvLoginReplyRecord
{
	FvNetAddress m_kServerAddr;
	FvUInt32 m_uiSessionKey;
};

FV_INLINE FvBinaryIStream& operator>>(
	FvBinaryIStream &is, FvLoginReplyRecord &lrr )
{
	return is >> lrr.m_kServerAddr >> lrr.m_uiSessionKey;
}

FV_INLINE FvBinaryOStream& operator<<(
	FvBinaryOStream &os, const FvLoginReplyRecord &lrr )
{
	return os << lrr.m_kServerAddr << lrr.m_uiSessionKey;
}

#define PROBE_KEY_HOST_NAME			"hostName"
#define PROBE_KEY_OWNER_NAME		"ownerName"
#define PROBE_KEY_USERS_COUNT		"usersCount"
#define PROBE_KEY_UNIVERSE_NAME		"universeName"
#define PROBE_KEY_SPACE_NAME		"spaceName"
#define PROBE_KEY_BINARY_ID			"binaryID"

#endif // FV_LOGIN_INTERFACE_HPP_ONCE_ONLY

#include <FvNetInterfaceMinderMacros.h>

#pragma pack(push,1)
BEGIN_MERCURY_INTERFACE( LoginInterface )

	// FvUInt32 version
	// bool encrypted
	// FvLogOnParams
	MERCURY_VARIABLE_MESSAGE( Login, 2, &g_kLoginHandler )

	MERCURY_FIXED_MESSAGE( Probe, 0, &g_kProbeHandler )

END_MERCURY_INTERFACE()
#pragma pack(pop)

#endif // __FvLoginInterface_H__
