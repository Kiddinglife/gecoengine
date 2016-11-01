#include "FvMySQLPrepared.h"
#include "FvMySQLWrapper.h"

#include <FvDebug.h>

FV_DECLARE_DEBUG_COMPONENT(0);

namespace MySqlPrep
{
	Statement::Statement( MySql& con, const FvString& stmt ) : 
		m_pkSTMT( mysql_stmt_init( con.Get() ) )
	{
		if ( !m_pkSTMT )
			throw MySqlError( con.Get() );

		if (mysql_stmt_prepare( m_pkSTMT, stmt.c_str(), stmt.length() ))
			throw MySqlError( m_pkSTMT );

		if (m_pkSTMT)
			m_pkMeta = mysql_stmt_result_metadata( m_pkSTMT );
		else
			m_pkMeta = 0;
	}

	Statement::~Statement()
	{
		if (m_pkMeta) mysql_free_result( m_pkMeta );
		if (m_pkSTMT) mysql_stmt_close( m_pkSTMT );
	}

	void Statement::BindParams( const Bindings& bindings )
	{
		if (m_pkSTMT)
		{
			FV_ASSERT( bindings.size() == this->ParamCount() );
			m_kParamBindings = bindings;
			if (mysql_stmt_bind_param( m_pkSTMT, m_kParamBindings.get() ))
				throw MySqlError( m_pkSTMT );
		}
	}

	void Statement::BindResult( const Bindings& bindings )
	{
		if (m_pkSTMT)
		{
			FV_ASSERT( bindings.size() == this->ResultCount() );
			m_kResultBindings = bindings;
			if (mysql_stmt_bind_result( m_pkSTMT, m_kResultBindings.get() ))
				throw MySqlError( m_pkSTMT );
		}
	}
}