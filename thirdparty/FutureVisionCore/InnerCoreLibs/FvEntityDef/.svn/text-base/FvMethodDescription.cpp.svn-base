#include "FvMethodDescription.h"
#include "FvDataDescription.h"

#include <FvMD5.h>
#include <FvNetTypes.h>
#include <FvBinaryStream.h>

#include <float.h>

FV_DECLARE_DEBUG_COMPONENT2( "DataDescription", 0 )


FvMethodDescription::FvMethodDescription()
:FvMemberDescription()
,m_kName()
,m_uiFlags( 0 )
,m_kArgs()
,m_kReturnValues()
,m_iInternalIndex( -1 )
,m_iExposedIndex( -1 )
,m_iExposedSubIndex( -1 )
,m_fPriority( FLT_MAX )
#if FV_ENABLE_WATCHERS
,m_uiTimeSpent( 0 )
,m_uiTimesCalled( 0 )
#endif
{
}

FvMethodDescription::FvMethodDescription( const FvMethodDescription & description )
{
	(*this) = description;
}

FvMethodDescription::~FvMethodDescription()
{
}

FvMethodDescription & FvMethodDescription::operator=( const FvMethodDescription & description )
{
	if (this != &description)
	{
		*static_cast<FvMemberDescription *>(this) = description;
		m_kName				= description.m_kName;
		m_uiFlags			= description.m_uiFlags;
		m_kArgs				= description.m_kArgs;
		m_kReturnValues		= description.m_kReturnValues;
		m_iInternalIndex	= description.m_iInternalIndex;
		m_iExposedIndex		= description.m_iExposedIndex;
		m_iExposedSubIndex	= description.m_iExposedSubIndex;
		m_fPriority			= description.m_fPriority;
#if FV_ENABLE_WATCHERS
		m_uiTimeSpent		= description.m_uiTimeSpent;
		m_uiTimesCalled		= description.m_uiTimesCalled;
#endif
	}

	return *this;
}

bool FvMethodDescription::Parse( FvXMLSectionPtr pSection, eComponent component )
{
	bool result = true;

	m_kName = pSection->SectionName();

	if (m_kName == "")
	{
		FV_WARNING_MSG("FvMethodDescription is missing <Name> tag");
		return false;
	}

	if (pSection->OpenChild( "Exposed" ))
	{
		this->SetExposed();
		FV_IF_NOT_ASSERT_DEV( this->IsExposed() )
		{
			FV_EXIT( "just set exposed, but we're not exposed!" );
		}
	}

	this->Component( component );
/**
	FvXMLSectionIterator iter = pSection->Begin();

	while (iter != pSection->End())
	{
		FvXMLSectionPtr pDS = *iter;
		const FvString & sectionName = pDS->SectionName();
		if (sectionName == "Arg")
		{
			FvDataTypePtr pDataType = FvDataType::BuildDataType( *iter );

			if (!pDataType)
			{
				FV_ERROR_MSG( "FvMethodDescription::Parse: "
						"Arg %s of %s is invalid\n",
					(*iter)->AsString().c_str(), m_kName.c_str() );
				result = false;
			}

			m_kArgs.push_back( pDataType );
		}
		else if (sectionName == "ReturnValues")
		{
			if (!this->ParseReturnValues( pDS ))
			{
				return false;
			}
		}

		iter++;
	}

	m_fPriority = pSection->ReadFloat( "DetailDistance", FLT_MAX );

	if (m_fPriority != FLT_MAX)
	{
		m_fPriority *= m_fPriority;
	}
**/
	return result;
}

bool FvMethodDescription::ParseReturnValues( FvXMLSectionPtr pSection )
{
	FvXMLSectionIterator iter = pSection->Begin();

	std::set<FvString> addedReturnValueNames;
	m_kReturnValues.clear();
	while (iter != pSection->End())
	{
		if((*iter)->GetType() != TiXmlNode::ELEMENT)
		{
			++iter;
			continue;
		}

		FvXMLSectionPtr pDS = *iter;

		if (addedReturnValueNames.find(pDS->SectionName()) !=
				addedReturnValueNames.end())
		{
			FV_ERROR_MSG( "Return value %s of %s is duplicated\n",
				pDS->SectionName().c_str(), m_kName.c_str() );
			m_kReturnValues.clear();
			return false;
		}

		FvDataTypePtr pDataType = FvDataType::BuildDataType( pDS );

		addedReturnValueNames.insert( pDS->SectionName() );
		m_kReturnValues.push_back(
			FvMethodDescription::ReturnValue( pDS->SectionName(), pDataType ) );

		++iter;
	}
	return true;
}

bool FvMethodDescription::AddToStream( bool isFromServer, FvObj * args, FvBinaryOStream & stream ) const
{
	return false;
}


bool FvMethodDescription::CallMethod(FvObj * self,
	FvBinaryIStream & data,
	FvEntityID sourceID,
	FvInt32 replyID,
	const FvNetAddress* pReplyAddr,
	FvNetNub* pNub) const
{
	return false;
}

FvObjPtr FvMethodDescription::GetArgsAsTuple( FvBinaryIStream & data,
	FvEntityID sourceID,
	FvInt32 replyID,
	const FvNetAddress* pReplyAddr,
	FvNetNub* pNub) const
{
	return NULL;
}

FvUInt32 FvMethodDescription::ReturnValueCnt() const
{
	return m_kReturnValues.size();
}


const FvString& FvMethodDescription::ReturnValueName( FvUInt32 index ) const
{
	return m_kReturnValues[index].first;
}


FvDataTypePtr FvMethodDescription::ReturnValueType( FvUInt32 index ) const
{
	return m_kReturnValues[index].second;
}


void FvMethodDescription::AddToMD5( FvMD5 & md5, FvInt32 legacyExposedIndex ) const
{
	md5.Append( m_kName.c_str(), m_kName.size() );
	md5.Append( &m_uiFlags, sizeof( m_uiFlags ) );

	Args::const_iterator argsIter = m_kArgs.begin();

	while (argsIter != m_kArgs.end())
	{
		if (*argsIter)
		{
			(*argsIter)->AddToMD5( md5 );
		}

		argsIter++;
	}

	md5.Append( &legacyExposedIndex, sizeof( legacyExposedIndex ) );
}

void FvMethodDescription::Dump(FvXMLSectionPtr pSection)
{
	if(!pSection) return;

	FvXMLSectionPtr pChild, pChild1, pChild2;
	pChild = pSection->NewSection(m_kName);
	if(pChild)
	{
		if(IsExposed()) pChild->NewSection("Exposed");
		pChild1 = pChild->NewSection("InternalIndex");
		pChild1->SetInt(m_iInternalIndex);
		pChild1 = pChild->NewSection("ExposedIndex");
		pChild1->SetInt(m_iExposedIndex);
		pChild1 = pChild->NewSection("ExposedSubIndex");
		pChild1->SetInt(m_iExposedSubIndex);
		pChild1 = pChild->NewSection("Priority");
		pChild1->SetFloat(m_fPriority);
		for(int i=0; i<(int)m_kArgs.size(); ++i)
		{
			pChild1 = pChild->NewSection("Arg");
			pChild1->SetString(m_kArgs[i]->TypeName());
		}
		if(m_kReturnValues.size())
		{
			pChild1 = pChild->NewSection("ReturnValues");
			for(int i=0; i<(int)m_kReturnValues.size(); ++i)
			{
				pChild2 = pChild1->NewSection(m_kReturnValues[i].first);
				pChild2->SetString(m_kReturnValues[i].second->TypeName());
			}
		}
	}
}

#if FV_ENABLE_WATCHERS
FvWatcherPtr FvMethodDescription::pWatcher()
{
	static FvWatcherPtr watchMe = NULL;

	if (watchMe == NULL)
	{
		FvMethodDescription *pNull = NULL;
		watchMe = new FvDirectoryWatcher();

		watchMe->AddChild( "name", MakeWatcher( pNull->m_kName ) );
		watchMe->AddChild( "priority", MakeWatcher( pNull->m_fPriority ) );
		watchMe->AddChild( "internalIndex", 
						   MakeWatcher( pNull->m_iInternalIndex ) );
		watchMe->AddChild( "exposedIndex", 
						   MakeWatcher( pNull->m_iExposedIndex ) );
		watchMe->AddChild( "timeSpent", MakeWatcher( pNull->m_uiTimeSpent ) );	
		watchMe->AddChild( "timesCalled", MakeWatcher( pNull->m_uiTimesCalled ) );	
		watchMe->AddChild( "stats", FvMemberDescription::pWatcher() );
	}
	return watchMe;
}

#endif
