//{future header message}
#ifndef __FvMySQLNamedLock_H__
#define __FvMySQLNamedLock_H__

#include <FvBaseTypes.h>

#include <sstream>

class MySql;

namespace MySQL
{

bool ObtainNamedLock( MySql& connection, const FvString& lockName );
void ReleaseNamedLock( MySql& connection, const FvString& lockName );

class NamedLock
{
public:
	class Exception: public std::exception
	{
	public:
		Exception( const FvString& lockName );
		~Exception() throw() {}
		const char *what() const throw()	{ return m_kErrMsg.c_str(); }
	private:
		FvString m_kErrMsg;
	};

	NamedLock( MySql& connection, const FvString& lockName,
			bool shouldLock = true );
	~NamedLock();

	bool Lock();
	bool Unlock();

	const FvString &LockName() const 	{ return m_kLockName; }
	bool IsLocked() const 					{ return m_bIsLocked; }

private:
	MySql &m_kConnection;
	FvString m_kLockName;
	bool m_bIsLocked;
};

}

#endif 	// __FvMySQLNamedLock_H__
