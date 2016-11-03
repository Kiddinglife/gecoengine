#include "FvMySQLNamedLock.h"
#include "FvMySQLWrapper.h"
#include "FvMySQLNotPrepared.h"

namespace MySQL
{

bool ObtainNamedLock( MySql& connection, const FvString& lockName )
{
	std::stringstream ss;
	ss << "SELECT GET_LOCK( '" << lockName << "', 0 )";

	MySqlUnPrep::Statement getLockStmt( connection, ss.str() );
	int result;
	MySqlUnPrep::Bindings bindings;
	bindings << result;
	getLockStmt.BindResult( bindings );

	connection.Execute( getLockStmt );

	return (getLockStmt.Fetch() && result);
}

void ReleaseNamedLock( MySql& connection, const FvString& lockName )
{
	std::stringstream ss;
	ss << "SELECT RELEASE_LOCK( '" << lockName << "')";

	connection.Execute( ss.str() );
}

NamedLock::NamedLock( MySql& connection, const FvString& lockName,
		bool shouldLock ) :
	m_kConnection( connection ), m_kLockName( lockName ),
	m_bIsLocked( false )
{
	if (shouldLock && !this->Lock())
	{
		throw Exception( m_kLockName );
	}
}

NamedLock::~NamedLock()
{
	this->Unlock();
}

bool NamedLock::Lock()
{
	if (!m_bIsLocked && ObtainNamedLock( m_kConnection, m_kLockName ))
	{
		m_bIsLocked = true;
		return true;
	}

	return false;
}

bool NamedLock::Unlock()
{
	if (m_bIsLocked)
	{
		if (!m_kConnection.HasFatalError())
		{
			ReleaseNamedLock( m_kConnection, m_kLockName );
		}
		m_bIsLocked = false;
		return true;
	}

	return false;
}

NamedLock::Exception::Exception( const FvString& lockName )
{
	std::stringstream ss;
	ss << "Failed to obtain lock named '" << lockName << "'";
	m_kErrMsg = ss.str();
}

}
