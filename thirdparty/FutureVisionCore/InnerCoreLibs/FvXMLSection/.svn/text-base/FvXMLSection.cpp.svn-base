#include "FvXMLSection.h"
#include <FvDebug.h>
#include <FvBase64.h>

FvXMLSectionIterator::FvXMLSectionIterator() :
m_spXMLSection(reinterpret_cast<FvXMLSection *>( NULL)),
m_iIndex(0)
{
}

FvXMLSectionIterator::FvXMLSectionIterator( FvXMLSectionPtr spDataSection,
										 size_t iIndex ) :
m_spXMLSection(spDataSection),
m_iIndex(iIndex)
{
}

bool FvXMLSectionIterator::operator==( const FvXMLSectionIterator &kIt ) const
{
	return m_iIndex == kIt.m_iIndex && m_spXMLSection == kIt.m_spXMLSection;
}

bool FvXMLSectionIterator::operator!=( const FvXMLSectionIterator &kIt ) const
{
	return m_iIndex != kIt.m_iIndex || m_spXMLSection != kIt.m_spXMLSection;
}

FvXMLSectionPtr FvXMLSectionIterator::operator*( void )
{
	FvXMLSectionPtr spChild = m_spXMLSection->OpenChild( m_iIndex );
	return spChild;
}

FvXMLSectionIterator FvXMLSectionIterator::operator++( void )
{
	++m_iIndex;
	return *this;
}

FvXMLSectionIterator FvXMLSectionIterator::operator++( int )
{
	++m_iIndex;
	return FvXMLSectionIterator( m_spXMLSection, m_iIndex - 1 );
}

FvString FvXMLSectionIterator::Tag(void)
{
	return m_spXMLSection->ChildSectionName( m_iIndex );
}

FvXMLSection::FvXMLSection( const FvString &kName )
{
	m_spXMLDocument = new FvXMLDocument(kName);
	m_pkXMLNode = m_spXMLDocument.GetObject();
}

FvXMLSection::FvXMLSection( TiXmlNode *pkNode,FvXMLDocumentPtr spDoc ) :
m_pkXMLNode(pkNode),
m_spXMLDocument(spDoc)
{
	TiXmlNode *pkIter = 0;
	while(pkIter = pkNode->IterateChildren(pkIter))
	{	
		if(pkIter->Type() == TiXmlNode::TEXT) continue;
		FvString kSecName = pkIter->Value();
		FvXMLSectionPtr spSection = new FvXMLSection(pkIter,m_spXMLDocument);
		m_kSectionIndexMap.push_back(spSection);
		m_kSectionNameMap.insert(XMLSectionNameMap::value_type(kSecName,spSection));
	}
}

FvXMLSection::~FvXMLSection()
{
	m_kSectionNameMap.clear();
	m_kSectionIndexMap.clear();
	m_pkXMLNode = NULL;
	m_spXMLDocument = NULL;
}

void FvXMLSection::SectionName( const FvString &kName )
{
	FV_ASSERT(m_pkXMLNode);
	return m_pkXMLNode->SetValue(kName.c_str());
}

FvString FvXMLSection::SectionName( void ) const
{
	FV_ASSERT(m_pkXMLNode);
	return m_pkXMLNode->Value();
}

FvXMLSectionPtr FvXMLSection::NewSection(const FvString &kTag)
{
	TiXmlElement *pkElement = new TiXmlElement(kTag.c_str());
	if(m_pkXMLNode->LinkEndChild(pkElement) == NULL)
		return NULL;
	FvXMLSectionPtr spSection = new FvXMLSection(pkElement,m_spXMLDocument);
	m_kSectionIndexMap.push_back(spSection);
	m_kSectionNameMap.insert(XMLSectionNameMap::value_type(kTag,spSection));
	return spSection;
}

FvXMLSectionPtr FvXMLSection::OpenSection( const FvString &kTag, bool bCreate/*=false*/ )
{
	if (kTag.empty()) return this;
	FvXMLSectionPtr spChild;
	FvString::size_type stPos = kTag.find_first_of("/");

	if( stPos != FvString::npos )
	{
		spChild = this->OpenChild( kTag.substr( 0, stPos ) );

		if(!spChild)
		{
			if(bCreate)
				spChild = this->NewSection( kTag.substr( 0, stPos ) );

			if(!spChild)
				return NULL;
		}

		FvString::size_type stPastSlash = kTag.find_first_not_of( "/", stPos );
		if ( stPastSlash == std::string::npos )
		{
			return spChild;
		}
		else
		{
			return spChild->OpenSection( kTag.substr( stPastSlash ),
				bCreate);
		}
	}

	spChild = this->OpenChild(kTag);

	if(!spChild && bCreate)
		spChild = this->NewSection(kTag);

	return spChild;
}

FvXMLSectionPtr FvXMLSection::OpenFirstSection( void )
{
	if(m_kSectionIndexMap.size() <= 0)
		return FvXMLSectionPtr(NULL);;
	
	return m_kSectionIndexMap[0];
}

void FvXMLSection::OpenSections( const FvString &kTag,
				  std::vector<FvXMLSectionPtr> &kDest)
{
	if (kTag.empty()) return;
	FvXMLSectionPtr spChild;
	FvString::size_type stPos = kTag.find_first_of("/");

	if( stPos != FvString::npos )
	{
		spChild = this->OpenChild( kTag.substr( 0, stPos ) );

		if(!spChild)
			return;

		FvString::size_type stPastSlash = kTag.find_first_not_of( "/", stPos );
		if ( stPastSlash == std::string::npos )
		{
			this->OpenSections( kTag.substr( 0, stPastSlash ),kDest );
		}
		else
		{
			spChild->OpenSections( kTag.substr( stPastSlash ),
				kDest);
		}
	}
	else
	{
		XMLSectionNameMap::iterator kFind = m_kSectionNameMap.lower_bound(kTag);
		while (kFind != m_kSectionNameMap.upper_bound(kTag))
		{
			kDest.push_back(kFind->second);
			kFind++;
		}
	}
}

FvXMLSectionPtr FvXMLSection::OpenChild( const FvString &kTag )
{
	XMLSectionNameMap::iterator kIt = m_kSectionNameMap.find(kTag);
	if(kIt != m_kSectionNameMap.end())
		return kIt->second;
	
	return FvXMLSectionPtr(NULL);
}

FvXMLSectionPtr FvXMLSection::OpenChild( size_t iIndex )
{
	if(m_kSectionIndexMap.size() <= size_t(iIndex))
		return FvXMLSectionPtr(NULL);

	return m_kSectionIndexMap[iIndex];
}

FvString FvXMLSection::ChildSectionName( size_t iIndex )
{
	if(m_kSectionIndexMap.size() <= size_t(iIndex))
		return "<none>";

	return m_kSectionIndexMap[iIndex]->SectionName();
}

size_t FvXMLSection::CountChildren()
{
	return m_kSectionIndexMap.size();
}

void FvXMLSection::DelChild( const FvString &kTag )
{
	XMLSectionNameMap::iterator kIter = m_kSectionNameMap.begin();

	while (kIter != m_kSectionNameMap.end())
	{
		if (kIter->first == kTag)
		{
			XMLSectionIndexMap::iterator kIndexIt = m_kSectionIndexMap.begin();
			for(; kIndexIt != m_kSectionIndexMap.end(); ++kIndexIt)
			{
				if(*kIndexIt == kIter->second)
				{
					m_pkXMLNode->RemoveChild((*kIndexIt)->m_pkXMLNode);
					m_kSectionIndexMap.erase(kIndexIt);
					m_kSectionNameMap.erase(kIter);
					return;
				}
			}
		}

		kIter++;
	}
}

void FvXMLSection::DelChild( FvXMLSectionPtr spSection )
{
	XMLSectionNameMap::iterator kIter = m_kSectionNameMap.begin();

	while (kIter != m_kSectionNameMap.end())
	{
		if (kIter->second == spSection)
		{
			XMLSectionIndexMap::iterator kIndexIt = m_kSectionIndexMap.begin();
			for(; kIndexIt != m_kSectionIndexMap.end(); ++kIndexIt)
			{
				if(*kIndexIt == kIter->second)
				{
					m_pkXMLNode->RemoveChild((*kIndexIt)->m_pkXMLNode);
					m_kSectionIndexMap.erase(kIndexIt);
					m_kSectionNameMap.erase(kIter);
					return;
				}
			}
		}

		kIter++;
	}
}

void FvXMLSection::DelChildren()
{	
	XMLSectionIndexMap::iterator kIndexIt = m_kSectionIndexMap.begin();
	for(; kIndexIt != m_kSectionIndexMap.end(); ++kIndexIt)
		m_pkXMLNode->RemoveChild((*kIndexIt)->m_pkXMLNode);

	m_kSectionNameMap.clear();
	m_kSectionIndexMap.clear();
}

bool FvXMLSection::ReadBool( const FvString &kTag,
			  bool bDefaultVal, bool bCreate )
{
	FvXMLSectionPtr spSection = this->OpenSection(kTag);

	if(spSection)
		return spSection->AsBool(bDefaultVal);
	else
	{
		if(bCreate)
			this->WriteBool(kTag,bDefaultVal);

		return bDefaultVal;
	}
}

int FvXMLSection::ReadInt( const FvString &kTag,
			int iDefaultVal, bool bCreate )
{
	FvXMLSectionPtr spSection = this->OpenSection(kTag);

	if(spSection)
		return spSection->AsInt(iDefaultVal);
	else
	{
		if(bCreate)
			this->WriteInt(kTag,iDefaultVal);

		return iDefaultVal;
	}
}

long FvXMLSection::ReadLong( const FvString &kTag,
						  long iDefaultVal, bool bCreate )
{
	FvXMLSectionPtr spSection = this->OpenSection(kTag);

	if(spSection)
		return spSection->AsLong(iDefaultVal);
	else
	{
		if(bCreate)
			this->WriteLong(kTag,iDefaultVal);

		return iDefaultVal;
	}
}

float FvXMLSection::ReadFloat( const FvString &kTag,
			  const float &fDefaultVal, bool bCreate )
{
	FvXMLSectionPtr spSection = this->OpenSection(kTag);

	if(spSection)
		return spSection->AsFloat(fDefaultVal);
	else
	{
		if(bCreate)
			this->WriteFloat(kTag,fDefaultVal);

		return fDefaultVal;
	}
}

FvString FvXMLSection::ReadString( const FvString &kTag,
			   const FvString &kDefaultVal, bool bCreate )
{
	FvXMLSectionPtr spSection = this->OpenSection(kTag);

	if(spSection)
		return spSection->AsString(kDefaultVal);
	else
	{
		if(bCreate)
			this->WriteString(kTag,kDefaultVal);

		return kDefaultVal;
	}
}

FvVector2 FvXMLSection::ReadVector2( const FvString &kTag,
				const FvVector2 &kDefaultVal, bool bCreate )
{
	FvXMLSectionPtr spSection = this->OpenSection(kTag);

	if(spSection)
		return spSection->AsVector2(kDefaultVal);
	else
	{
		if(bCreate)
			this->WriteVector2(kTag,kDefaultVal);

		return kDefaultVal;
	}
}

FvVector3 FvXMLSection::ReadVector3( const FvString &kTag,
				const FvVector3 &kDefaultVal, bool bCreate )
{
	FvXMLSectionPtr spSection = this->OpenSection(kTag);

	if(spSection)
		return spSection->AsVector3(kDefaultVal);
	else
	{
		if(bCreate)
			this->WriteVector3(kTag,kDefaultVal);

		return kDefaultVal;
	}
}

FvVector4 FvXMLSection::ReadVector4( const FvString &kTag,
				const FvVector4 &kDefaultVal, bool bCreate )
{
	FvXMLSectionPtr spSection = this->OpenSection(kTag);

	if(spSection)
		return spSection->AsVector4(kDefaultVal);
	else
	{
		if(bCreate)
			this->WriteVector4(kTag,kDefaultVal);

		return kDefaultVal;
	}
}

FvQuaternion FvXMLSection::ReadQuaternion( const FvString &kTag,
							const FvQuaternion &kDefaultVal, bool bCreate )
{
	FvXMLSectionPtr spSection = this->OpenSection(kTag);

	if(spSection)
		return spSection->AsQuaternion(kDefaultVal);
	else
	{
		if(bCreate)
			this->WriteQuaternion(kTag,kDefaultVal);

		return kDefaultVal;
	}
}

FvMatrix FvXMLSection::ReadMatrix34( const FvString &kTag,
				 const FvMatrix &kDefaultVal )
{
	FvXMLSectionPtr spSection = this->OpenSection(kTag);

	if(spSection)
		return spSection->AsMatrix34(kDefaultVal);
	else
		return kDefaultVal;
}

void FvXMLSection::ReadVector2s( const FvString &kTag, 
				  std::vector<FvVector2> &kDest )
{
	FvXMLSectionIterator kIt = this->Begin();
	for(; kIt != this->End(); kIt++)
	{
		FvXMLSectionPtr pDS = *kIt;
		if(pDS->SectionName() == kTag)
			kDest.push_back(pDS->AsVector2());
	}
}

void FvXMLSection::ReadVector3s( const FvString &kTag, 
				  std::vector<FvVector3> &kDest )
{
	FvXMLSectionIterator kIt = this->Begin();
	for(; kIt != this->End(); kIt++)
	{
		FvXMLSectionPtr pDS = *kIt;
		if(pDS->SectionName() == kTag)
			kDest.push_back(pDS->AsVector3());
	}
}

void FvXMLSection::ReadVector4s( const FvString &kTag, 
				  std::vector<FvVector4> &kDest )
{
	FvXMLSectionIterator kIt = this->Begin();
	for(; kIt != this->End(); kIt++)
	{
		FvXMLSectionPtr pDS = *kIt;
		if(pDS->SectionName() == kTag)
			kDest.push_back(pDS->AsVector4());
	}
}

void FvXMLSection::ReadQuaternion4s( const FvString &kTag, 
					  std::vector<FvQuaternion> &kDest )
{
	FvXMLSectionIterator kIt = this->Begin();
	for(; kIt != this->End(); kIt++)
	{
		FvXMLSectionPtr pDS = *kIt;
		if(pDS->SectionName() == kTag)
			kDest.push_back(pDS->AsQuaternion());
	}
}

void FvXMLSection::ReadStrings( const FvString &kTag,
				 std::vector<FvString> &kDest )
{
	FvXMLSectionIterator kIt = this->Begin();
	for(; kIt != this->End(); kIt++)
	{
		FvXMLSectionPtr pDS = *kIt;
		if(pDS->SectionName() == kTag)
			kDest.push_back(pDS->AsString());
	}
}

FvString FvXMLSection::ReadBlob( const FvString &kTag,
				  const FvString &kDefaultVal, bool bCreate )
{
	FvXMLSectionPtr spSection = this->OpenSection(kTag);

	if (spSection)
		return spSection->AsBlob( kDefaultVal );
	else
	{
		if(bCreate)
			this->WriteBlob(kTag,kDefaultVal);

		return kDefaultVal;
	}
}

bool FvXMLSection::WriteBool(const FvString &kTag, 
			   bool bValue)
{
	FvXMLSectionPtr spSection = this->OpenSection(kTag, true);

	if(spSection)
		return spSection->SetBool(bValue);
	else
		return false;
}

bool FvXMLSection::WriteInt(const FvString &kTag, 
			  int iValue)
{
	FvXMLSectionPtr spSection = this->OpenSection(kTag, true);

	if(spSection)
		return spSection->SetInt(iValue);
	else
		return false;
}

bool FvXMLSection::WriteLong(const FvString &kTag, 
			  long iValue)
{
	FvXMLSectionPtr spSection = this->OpenSection(kTag, true);

	if(spSection)
		return spSection->SetLong(iValue);
	else
		return false;
}

bool FvXMLSection::WriteFloat(const FvString &kTag, 
				float fValue)
{
	FvXMLSectionPtr spSection = this->OpenSection(kTag, true);

	if(spSection)
		return spSection->SetFloat(fValue);
	else
		return false;
}

bool FvXMLSection::WriteString(const FvString &kTag, 
				 FvString kValue)
{
	FvXMLSectionPtr spSection = this->OpenSection(kTag, true);

	if(spSection)
		return spSection->SetString(kValue);
	else
		return false;
}

bool FvXMLSection::WriteVector2(const FvString &kTag, 
				  FvVector2 kValue)
{
	FvXMLSectionPtr spSection = this->OpenSection(kTag, true);

	if(spSection)
		return spSection->SetVector2(kValue);
	else
		return false;
}

bool FvXMLSection::WriteVector3(const FvString &kTag, 
				  FvVector3 kValue)
{
	FvXMLSectionPtr spSection = this->OpenSection(kTag, true);

	if(spSection)
		return spSection->SetVector3(kValue);
	else
		return false;
}

bool FvXMLSection::WriteVector4(const FvString &kTag, 
				  FvVector4 kValue)
{
	FvXMLSectionPtr spSection = this->OpenSection(kTag, true);

	if(spSection)
		return spSection->SetVector4(kValue);
	else
		return false;
}

bool FvXMLSection::WriteQuaternion( const FvString &kTag, 
					 FvQuaternion kValue )
{
	FvXMLSectionPtr spSection = this->OpenSection(kTag, true);

	if(spSection)
		return spSection->SetQuaternion(kValue);
	else
		return false;
}

bool FvXMLSection::WriteBlob( const FvString &kTag, 
			   const FvString &kValue )
{
	FvXMLSectionPtr spSection = this->OpenSection(kTag, true);

	if (spSection)
		return spSection->SetBlob( kValue );
	else
		return false;
}

bool FvXMLSection::AsBool( bool bDefaultVal )
{
	TiXmlNode *pkValue = 0;
	if(!m_pkXMLNode || !(pkValue = m_pkXMLNode->FirstChild()) ||
		(pkValue->Type() != TiXmlNode::TEXT))
		return bDefaultVal;

	FvString kStrValue = pkValue->Value();

	if ( !_stricmp( kStrValue.c_str(),"true" ) )
	{
		return true;
	}
	if ( !_stricmp( kStrValue.c_str(),"false" ) )
	{
		return false;
	}

	return bDefaultVal;
}

int FvXMLSection::AsInt( int iDefaultVal )
{
	TiXmlNode *pkValue = 0;
	if(!m_pkXMLNode || !(pkValue = m_pkXMLNode->FirstChild()) ||
		(pkValue->Type() != TiXmlNode::TEXT))
		return iDefaultVal;

	FvString kStrValue = pkValue->Value();

	if ( kStrValue != "" )
	{
		return int(atoi( kStrValue.c_str()));
	}
	else
	{
		return iDefaultVal;
	}
}

long FvXMLSection::AsLong( long iDefaultVal )
{
	TiXmlNode *pkValue = 0;
	if(!m_pkXMLNode || !(pkValue = m_pkXMLNode->FirstChild()) ||
		(pkValue->Type() != TiXmlNode::TEXT))
		return iDefaultVal;

	FvString kStrValue = pkValue->Value();

	if ( kStrValue != "" )
	{
		return int(atol( kStrValue.c_str()));
	}
	else
	{
		return iDefaultVal;
	}
}

float FvXMLSection::AsFloat(const float &fDefaultVal )
{
	return this->AsDouble();
}

double FvXMLSection::AsDouble( const float &fDefaultVal )
{
	TiXmlNode *pkValue = 0;
	if(!m_pkXMLNode || !(pkValue = m_pkXMLNode->FirstChild()) ||
		(pkValue->Type() != TiXmlNode::TEXT))
		return fDefaultVal;

	FvString kStrValue = pkValue->Value();

	if ( kStrValue != "" )
	{
		return float(atof( kStrValue.c_str()));
	}
	else
	{
		return fDefaultVal;
	}
}

FvString FvXMLSection::AsString(const FvString &kDefaultVal )
{
	TiXmlNode *pkValue = 0;
	if(!m_pkXMLNode || !(pkValue = m_pkXMLNode->FirstChild()) ||
		(pkValue->Type() != TiXmlNode::TEXT))
		return kDefaultVal;

	return pkValue->Value();
}

FvVector2 FvXMLSection::AsVector2( const FvVector2 &kDefaultVal )
{
	TiXmlNode *pkValue;
	if(!m_pkXMLNode || !(pkValue = m_pkXMLNode->FirstChild()) ||
		(pkValue->Type() != TiXmlNode::TEXT))
		return kDefaultVal;

	FvString kStrValue = pkValue->Value();
	FvVector2 kVec;

	if (sscanf( kStrValue.c_str(), "%f%f", &kVec.x, &kVec.y) != 2)
	{
		return kDefaultVal;
	}

	return kVec;
}

FvVector3 FvXMLSection::AsVector3( const FvVector3 &kDefaultVal )
{
	TiXmlNode *pkValue;
	if(!m_pkXMLNode || !(pkValue = m_pkXMLNode->FirstChild()) ||
		(pkValue->Type() != TiXmlNode::TEXT))
		return kDefaultVal;

	FvString kStrValue = pkValue->Value();
	FvVector3 kVec;

	if (sscanf( kStrValue.c_str(), "%f%f%f", &kVec.x, &kVec.y, &kVec.z ) != 3)
	{
		return kDefaultVal;
	}

	return kVec;
}

FvVector4 FvXMLSection::AsVector4( const FvVector4 &kDefaultVal )
{
	TiXmlNode *pkValue;
	if(!m_pkXMLNode || !(pkValue = m_pkXMLNode->FirstChild()) ||
		(pkValue->Type() != TiXmlNode::TEXT))
		return kDefaultVal;

	FvString kStrValue = pkValue->Value();
	FvVector4 kVec;

	if (sscanf( kStrValue.c_str(), "%f%f%f%f", &kVec.x, &kVec.y, &kVec.z, &kVec.w ) != 4)
	{
		return kDefaultVal;
	}

	return kVec;
}

FvQuaternion FvXMLSection::AsQuaternion( const FvQuaternion &kDefaultVal )
{
	TiXmlNode *pkValue;
	if(!m_pkXMLNode || !(pkValue = m_pkXMLNode->FirstChild()) ||
		(pkValue->Type() != TiXmlNode::TEXT))
		return kDefaultVal;

	FvString kStrValue = pkValue->Value();
	FvQuaternion kQuat;

	if (sscanf( kStrValue.c_str(), "%f%f%f%f", &kQuat.x, &kQuat.y, &kQuat.z, &kQuat.w ) != 4)
	{
		return kDefaultVal;
	}

	return kQuat;
}

FvMatrix FvXMLSection::AsMatrix34( const FvMatrix &kDefaultVal )
{
	if(!m_pkXMLNode)
	{
		return kDefaultVal;
	}

	FvMatrix kRet;
	kRet( 3, 3 ) = 1.f;

	kRet[0] = this->ReadVector3( "row0", kDefaultVal[0] );
	kRet[1] = this->ReadVector3( "row1", kDefaultVal[1] );
	kRet[2] = this->ReadVector3( "row2", kDefaultVal[2] );
	kRet[3] = this->ReadVector3( "row3", kDefaultVal[3] );

	return kRet;
}

FvString FvXMLSection::AsBlob( const FvString &kDefaultVal )
{
	FvString kEncoded = this->AsString();
	int iEncLen = kEncoded.length();
	char *pcDecoded = new char[ iEncLen ];
	int iDecLen = FvBase64::Decode( kEncoded, pcDecoded, iEncLen );
	FvString kRetString( pcDecoded, iDecLen );
	delete [] pcDecoded;
	return kRetString;
}

bool FvXMLSection::SetBool(bool bValue)
{
	char pcFormattedString[512];
	sprintf_s( pcFormattedString, sizeof(pcFormattedString), "%s", bValue ? "true" : "false" );
	return this->SetString( pcFormattedString );
}

bool FvXMLSection::SetInt(int iValue)
{
	char pcFormattedString[512];
	sprintf_s( pcFormattedString, sizeof(pcFormattedString), "%d", iValue );
	return this->SetString( pcFormattedString );
}

bool FvXMLSection::SetLong(long iValue)
{
	char pcFormattedString[512];
	sprintf_s( pcFormattedString, sizeof(pcFormattedString), "%d", iValue );
	return this->SetString( pcFormattedString );
}

bool FvXMLSection::SetFloat(float fValue)
{
	char pcFormattedString[512];
	sprintf_s( pcFormattedString, sizeof(pcFormattedString), "%f", fValue );
	return this->SetString( pcFormattedString );
}

bool FvXMLSection::SetDouble(float fValue)
{
	char pcFormattedString[512];
	sprintf_s( pcFormattedString, sizeof(pcFormattedString), "%f", fValue );
	return this->SetString( pcFormattedString );
}

bool FvXMLSection::SetString(const FvString& kValue)
{
	TiXmlNode *pkText = m_pkXMLNode->FirstChild();
	if(pkText != NULL && 
		pkText->Type() == TiXmlNode::TEXT)
	{
		static_cast<TiXmlText*>(pkText)->SetValue(kValue.c_str());
		return true;
	}
	else if(pkText == NULL)
	{
		pkText = new TiXmlText(kValue.c_str());
		return m_pkXMLNode->LinkEndChild(pkText);
	}

	return false;
}

bool FvXMLSection::SetVector2(const FvVector2& kValue)
{
	char pcFormattedString[512];

	sprintf_s( pcFormattedString, sizeof(pcFormattedString), "%f %f", kValue.x, kValue.y );
	return this->SetString( pcFormattedString );
}

bool FvXMLSection::SetVector3(const FvVector3& kValue)
{
	char pcFormattedString[512];

	sprintf_s( pcFormattedString, sizeof(pcFormattedString), "%f %f %f", kValue.x, kValue.y, kValue.z );
	return this->SetString( pcFormattedString );
}

bool FvXMLSection::SetVector4(const FvVector4& kValue)
{
	char pcFormattedString[512];

	sprintf_s( pcFormattedString, sizeof(pcFormattedString), "%f %f %f %f", kValue.x, kValue.y, kValue.z, kValue.w );
	return this->SetString( pcFormattedString );
}

bool FvXMLSection::SetQuaternion( const FvQuaternion& kValue )
{
	char pcFormattedString[512];

	sprintf_s( pcFormattedString, sizeof(pcFormattedString), "%f %f %f %f", kValue.x, kValue.y, kValue.z, kValue.w );
	return this->SetString( pcFormattedString );
}

bool FvXMLSection::SetMatrix34(const FvMatrix &kValue)
{
	bool bGood = true;

	bGood &= this->WriteVector3( "row0", kValue[0] );
	bGood &= this->WriteVector3( "row1", kValue[1] );
	bGood &= this->WriteVector3( "row2", kValue[2] );
	bGood &= this->WriteVector3( "row3", kValue[3] );

	return bGood;
}

bool FvXMLSection::SetBlob( const FvString &kValue )
{
	FvString kEncoded = FvBase64::Encode( kValue.data(), kValue.length() );
	return this->SetString( kEncoded );
}

bool FvXMLSection::AsAttributeBool( const FvString &kAttribute,
					   bool bDefaultVal )
{
	if(!m_pkXMLNode || (m_pkXMLNode->Type() != TiXmlNode::ELEMENT))
		return bDefaultVal;

	const char *pcStrValue = static_cast<TiXmlElement*>(m_pkXMLNode)->Attribute(kAttribute.c_str());

	if(pcStrValue == NULL)
	{
		return bDefaultVal;
	}

	if ( !_stricmp( pcStrValue,"true" ) )
	{
		return true;
	}
	if ( !_stricmp( pcStrValue,"false" ) )
	{
		return false;
	}

	return bDefaultVal;
}

int FvXMLSection::AsAttributeInt( const FvString &kAttribute,
					 int iDefaultVal )
{
	if(!m_pkXMLNode || (m_pkXMLNode->Type() != TiXmlNode::ELEMENT))
		return iDefaultVal;

	const char *pcStrValue = static_cast<TiXmlElement*>(m_pkXMLNode)->Attribute(kAttribute.c_str());

	if ( pcStrValue != NULL )
	{
		return int(atoi(pcStrValue));
	}
	else
	{
		return iDefaultVal;
	}
}

float FvXMLSection::AsAttributeFloat( const FvString &kAttribute,
						 const float &fDefaultVal )
{
	if(!m_pkXMLNode || (m_pkXMLNode->Type() != TiXmlNode::ELEMENT))
		return fDefaultVal;

	const char *pcStrValue = static_cast<TiXmlElement*>(m_pkXMLNode)->Attribute(kAttribute.c_str());

	if ( pcStrValue != NULL )
	{
		return float(atof(pcStrValue));
	}
	else
	{
		return fDefaultVal;
	}
}

FvString FvXMLSection::AsAttributeString( const FvString &kAttribute,
							 const FvString &kDefaultVal )
{
	if(!m_pkXMLNode || (m_pkXMLNode->Type() != TiXmlNode::ELEMENT))
		return kDefaultVal;

	const char *pcValue = static_cast<TiXmlElement*>(m_pkXMLNode)->Attribute(kAttribute.c_str());
	return (pcValue != NULL) ? pcValue : "";
}

FvVector2 FvXMLSection::AsAttributeVector2( const FvString &kAttribute,
							   const FvVector2 &kDefaultVal )
{
	if(!m_pkXMLNode || (m_pkXMLNode->Type() != TiXmlNode::ELEMENT))
		return kDefaultVal;

	const char *pcStrValue = static_cast<TiXmlElement*>(m_pkXMLNode)->Attribute(kAttribute.c_str());
	FvVector2 kVec;

	if(pcStrValue == NULL)
	{
		return kDefaultVal;
	}

	if (sscanf( pcStrValue, "%f%f", &kVec.x, &kVec.y) != 2)
	{
		return kDefaultVal;
	}

	return kVec;
}

FvVector3 FvXMLSection::AsAttributeVector3( const FvString &kAttribute,
							   const FvVector3 &kDefaultVal )
{
	if(!m_pkXMLNode || (m_pkXMLNode->Type() != TiXmlNode::ELEMENT))
		return kDefaultVal;

	const char *pcStrValue = static_cast<TiXmlElement*>(m_pkXMLNode)->Attribute(kAttribute.c_str());
	FvVector3 kVec;

	if(pcStrValue == NULL)
	{
		return kDefaultVal;
	}

	if (sscanf( pcStrValue, "%f%f%f", &kVec.x, &kVec.y, &kVec.z ) != 3)
	{
		return kDefaultVal;
	}

	return kVec;
}

FvVector4 FvXMLSection::AsAttributeVector4( const FvString &kAttribute,
							   const FvVector4 &kDefaultVal )
{
	if(!m_pkXMLNode || (m_pkXMLNode->Type() != TiXmlNode::ELEMENT))
		return kDefaultVal;

	const char *pcStrValue = static_cast<TiXmlElement*>(m_pkXMLNode)->Attribute(kAttribute.c_str());
	FvVector4 kVec;

	if(pcStrValue == NULL)
	{
		return kDefaultVal;
	}

	if (sscanf( pcStrValue, "%f%f%f%f", &kVec.x, &kVec.y, &kVec.z, &kVec.w ) != 4)
	{
		return kDefaultVal;
	}

	return kVec;
}

FvQuaternion FvXMLSection::AsAttributeQuaternion( const FvString &kAttribute,
								const FvQuaternion &kDefaultVal )
{
	if(!m_pkXMLNode || (m_pkXMLNode->Type() != TiXmlNode::ELEMENT))
		return kDefaultVal;

	const char *pcStrValue = static_cast<TiXmlElement*>(m_pkXMLNode)->Attribute(kAttribute.c_str());
	FvQuaternion kQuat;

	if(pcStrValue == NULL)
	{
		return kDefaultVal;
	}

	if (sscanf( pcStrValue, "%f%f%f%f", &kQuat.x, &kQuat.y, &kQuat.z, &kQuat.w ) != 4)
	{
		return kDefaultVal;
	}

	return kQuat;
}

bool FvXMLSection::GetAttributeBool( const FvString &kTag,
					  const FvString &kAttribute,
					  bool bDefaultVal )
{
	FvXMLSectionPtr spSection = this->OpenSection(kTag);

	if(spSection)
		return spSection->AsAttributeBool(kAttribute,bDefaultVal);
	else
		return bDefaultVal;
}

int FvXMLSection::GetAttributeInt( const FvString &kTag,
					const FvString &kAttribute,
					int iDefaultVal )
{
	FvXMLSectionPtr spSection = this->OpenSection(kTag);

	if(spSection)
		return spSection->AsAttributeInt(kAttribute,iDefaultVal);
	else
		return iDefaultVal;
}

float FvXMLSection::GetAttributeFloat( const FvString &kTag,
						const FvString &kAttribute,
						const float &fDefaultVal )
{
	FvXMLSectionPtr spSection = this->OpenSection(kTag);

	if(spSection)
		return spSection->AsAttributeFloat(kAttribute,fDefaultVal);
	else
		return fDefaultVal;
}

FvString FvXMLSection::GetAttributeString( const FvString &kTag,
							const FvString &kAttribute,
							const FvString &kDefaultVal )
{
	FvXMLSectionPtr spSection = this->OpenSection(kTag);

	if(spSection)
		return spSection->AsAttributeString(kAttribute,kDefaultVal);
	else
		return kDefaultVal;
}

FvVector2 FvXMLSection::GetAttributeVector2( const FvString &kTag,
							  const FvString &kAttribute,
							  const FvVector2 &kDefaultVal )
{
	FvXMLSectionPtr spSection = this->OpenSection(kTag);

	if(spSection)
		return spSection->AsAttributeVector2(kAttribute,kDefaultVal);
	else
		return kDefaultVal;
}

FvVector3 FvXMLSection::GetAttributeVector3( const FvString &kTag,
							  const FvString &kAttribute,
							  const FvVector3 &kDefaultVal )
{
	FvXMLSectionPtr spSection = this->OpenSection(kTag);

	if(spSection)
		return spSection->AsAttributeVector3(kAttribute,kDefaultVal);
	else
		return kDefaultVal;
}

FvVector4 FvXMLSection::GetAttributeVector4( const FvString &kTag,
							  const FvString &kAttribute,
							  const FvVector4 &kDefaultVal )
{
	FvXMLSectionPtr spSection = this->OpenSection(kTag);

	if(spSection)
		return spSection->AsAttributeVector4(kAttribute,kDefaultVal);
	else
		return kDefaultVal;
}

FvQuaternion FvXMLSection::GetAttributeQuaternion( const FvString &kTag,
									const FvString &kAttribute,
									const FvQuaternion &kDefaultVal )
{
	FvXMLSectionPtr spSection = this->OpenSection(kTag);

	if(spSection)
		return spSection->AsAttributeQuaternion(kAttribute,kDefaultVal);
	else
		return kDefaultVal;
}

bool FvXMLSection::SetAttributeBool( const FvString &kAttribute,
					  bool bValue )
{	
	if(!m_pkXMLNode || (m_pkXMLNode->Type() != TiXmlNode::ELEMENT))
		return false;

	char pcFormattedString[512];
	sprintf_s( pcFormattedString, sizeof(pcFormattedString), "%s", bValue ? "true" : "false" );
	static_cast<TiXmlElement*>(m_pkXMLNode)->SetAttribute( kAttribute.c_str(),pcFormattedString );
	return true;
}

bool FvXMLSection::SetAttributeInt( const FvString &kAttribute,
					 int iValue )
{
	if(!m_pkXMLNode || (m_pkXMLNode->Type() != TiXmlNode::ELEMENT))
		return false;

	char pcFormattedString[512];
	sprintf_s( pcFormattedString, sizeof(pcFormattedString), "%d", iValue );
	static_cast<TiXmlElement*>(m_pkXMLNode)->SetAttribute( kAttribute.c_str(),pcFormattedString );
	return true;
}

bool FvXMLSection::SetAttributeFloat( const FvString &kAttribute,
					   float fValue )
{
	if(!m_pkXMLNode || (m_pkXMLNode->Type() != TiXmlNode::ELEMENT))
		return false;

	char pcFormattedString[512];
	sprintf_s( pcFormattedString, sizeof(pcFormattedString), "%f", fValue );
	static_cast<TiXmlElement*>(m_pkXMLNode)->SetAttribute( kAttribute.c_str(),pcFormattedString );
	return true;
}

bool FvXMLSection::SetAttributeString( const FvString &kAttribute,
						const FvString& kValue )
{
	if(!m_pkXMLNode || (m_pkXMLNode->Type() != TiXmlNode::ELEMENT))
		return false;

	static_cast<TiXmlElement*>(m_pkXMLNode)->SetAttribute( kAttribute.c_str(),kValue.c_str() );
	return true;
}

bool FvXMLSection::SetAttributeVector2( const FvString &kAttribute,
						 const FvVector2& kValue )
{
	if(!m_pkXMLNode || (m_pkXMLNode->Type() != TiXmlNode::ELEMENT))
		return false;

	char pcFormattedString[512];
	sprintf_s( pcFormattedString, sizeof(pcFormattedString), "%f %f", kValue.x, kValue.y );
	static_cast<TiXmlElement*>(m_pkXMLNode)->SetAttribute( kAttribute.c_str(),pcFormattedString );
	return true;
}

bool FvXMLSection::SetAttributeVector3( const FvString &kAttribute,
						 const FvVector3& kValue )
{
	if(!m_pkXMLNode || (m_pkXMLNode->Type() != TiXmlNode::ELEMENT))
		return false;

	char pcFormattedString[512];
	sprintf_s( pcFormattedString, sizeof(pcFormattedString), "%f %f %f", kValue.x, kValue.y, kValue.z );
	static_cast<TiXmlElement*>(m_pkXMLNode)->SetAttribute( kAttribute.c_str(),pcFormattedString );
	return true;
}

bool FvXMLSection::SetAttributeVector4( const FvString &kAttribute,
						 const FvVector4& kValue )
{
	if(!m_pkXMLNode || (m_pkXMLNode->Type() != TiXmlNode::ELEMENT))
		return false;

	char pcFormattedString[512];
	sprintf_s( pcFormattedString, sizeof(pcFormattedString), "%f %f %f %f", kValue.x, kValue.y, kValue.z, kValue.w );
	static_cast<TiXmlElement*>(m_pkXMLNode)->SetAttribute( kAttribute.c_str(),pcFormattedString );
	return true;
}

bool FvXMLSection::SetAttributeQuaternion( const FvString &kAttribute,
							const FvQuaternion& kValue )
{
	if(!m_pkXMLNode || (m_pkXMLNode->Type() != TiXmlNode::ELEMENT))
		return false;

	char pcFormattedString[512];
	sprintf_s( pcFormattedString, sizeof(pcFormattedString), "%f %f %f %f", kValue.x, kValue.y, kValue.z, kValue.w );
	static_cast<TiXmlElement*>(m_pkXMLNode)->SetAttribute( kAttribute.c_str(),pcFormattedString );
	return true;
}

bool FvXMLSection::SetAttributeBool( const FvString &kTag,
					  const FvString &kAttribute,
					  bool bValue )
{
	FvXMLSectionPtr spSection = this->OpenSection(kTag, true);

	if(spSection)
		return spSection->SetAttributeBool( kAttribute,bValue );
	else
		return false;
}

bool FvXMLSection::SetAttributeInt( const FvString &kTag,
					const FvString &kAttribute,
					int iValue )
{
	FvXMLSectionPtr spSection = this->OpenSection(kTag, true);

	if(spSection)
		return spSection->SetAttributeInt( kAttribute,iValue );
	else
		return false;
}

bool FvXMLSection::SetAttributeFloat( const FvString &kTag,
						const FvString &kAttribute,
						float fValue )
{
	FvXMLSectionPtr spSection = this->OpenSection(kTag, true);

	if(spSection)
		return spSection->SetAttributeFloat( kAttribute,fValue );
	else
		return false;
}

bool FvXMLSection::SetAttributeString( const FvString &kTag,
							const FvString &kAttribute,
							const FvString &kValue )
{
	FvXMLSectionPtr spSection = this->OpenSection(kTag, true);

	if(spSection)
		return spSection->SetAttributeString( kAttribute,kValue );
	else
		return false;
}

bool FvXMLSection::SetAttributeVector2( const FvString &kTag,
							  const FvString &kAttribute,
							  const FvVector2 &kValue )
{
	FvXMLSectionPtr spSection = this->OpenSection(kTag, true);

	if(spSection)
		return spSection->SetAttributeVector2( kAttribute,kValue );
	else
		return false;
}

bool FvXMLSection::SetAttributeVector3( const FvString &kTag,
							  const FvString &kAttribute,
							  const FvVector3 &kValue )
{
	FvXMLSectionPtr spSection = this->OpenSection(kTag, true);

	if(spSection)
		return spSection->SetAttributeVector3( kAttribute,kValue );
	else
		return false;
}

bool FvXMLSection::SetAttributeVector4( const FvString &kTag,
							  const FvString &kAttribute,
							  const FvVector4 &kValue )
{
	FvXMLSectionPtr spSection = this->OpenSection(kTag, true);

	if(spSection)
		return spSection->SetAttributeVector4( kAttribute,kValue );
	else
		return false;
}

bool FvXMLSection::SetAttributeQuaternion( const FvString &kTag,
							const FvString &kAttribute,
							const FvQuaternion &kValue )
{
	FvXMLSectionPtr spSection = this->OpenSection(kTag, true);

	if(spSection)
		return spSection->SetAttributeQuaternion( kAttribute,kValue );
	else
		return false;
}

void FvXMLSection::RemoveAttribute( const FvString &kAttribute )
{
	if(!m_pkXMLNode || (m_pkXMLNode->Type() != TiXmlNode::ELEMENT))
		return;

	static_cast<TiXmlElement*>(m_pkXMLNode)->RemoveAttribute(kAttribute.c_str());
}

void FvXMLSection::RemoveAttributes( void )
{
	if(!m_pkXMLNode || (m_pkXMLNode->Type() != TiXmlNode::ELEMENT))
		return;

	TiXmlElement *pkElement = static_cast<TiXmlElement*>(m_pkXMLNode);
	const TiXmlAttribute *pkAttribute = pkElement->FirstAttribute();
	while( pkAttribute )
	{
		const TiXmlAttribute *pkRemove = pkAttribute;
		pkAttribute = pkAttribute->Next();
		pkElement->RemoveAttribute( pkRemove->Name() );	
	}
}

void FvXMLSection::Copy( const FvXMLSectionPtr spSection,
		  bool bModifyCurrent )
{
	if (bModifyCurrent)
	{
		this->DelChildren();
		this->SectionName( spSection->SectionName() );
		this->SetString( spSection->AsString() );
		if(spSection->m_pkXMLNode->Type() == TiXmlNode::ELEMENT &&
			this->m_pkXMLNode->Type() == TiXmlNode::ELEMENT )
		{
			TiXmlElement *pkSrc = static_cast<TiXmlElement*>(spSection->m_pkXMLNode);
			TiXmlElement *pkDest = static_cast<TiXmlElement*>(this->m_pkXMLNode);
			const TiXmlAttribute *pkAttribute = 0;
			for( pkAttribute = pkSrc->FirstAttribute();
				pkAttribute; pkAttribute = pkAttribute->Next() )
			{
				pkDest->SetAttribute( pkAttribute->Name(), pkAttribute->Value() );
			}
		}
	}

	FvXMLSectionIterator kIter = spSection->Begin();

	while (kIter != spSection->End())
	{
		FvXMLSectionPtr spNewSection = this->NewSection( (*kIter)->SectionName() );

		spNewSection->Copy( *kIter );

		kIter++;
	}
}

void FvXMLSection::CopySections( const FvXMLSectionPtr spSection, FvString kTag )
{
	FvXMLSectionIterator kIter = spSection->Begin();

	while (kIter != spSection->End())
	{
		if ((*kIter)->SectionName() == kTag)
		{
			FvXMLSectionPtr spNewSection = this->NewSection( (*kIter)->SectionName() );

			spNewSection->Copy( *kIter );
		}

		kIter++;
	}
}

void FvXMLSection::CopySections( const FvXMLSectionPtr spSection )
{
	this->SetString( spSection->AsString() );

	FvXMLSectionIterator kIter = spSection->Begin();

	while (kIter != spSection->End())
	{
		FvXMLSectionPtr spNewSection = this->OpenSection( (*kIter)->SectionName(), true );

		spNewSection->CopySections( *kIter );

		kIter++;
	}
}

FvXMLSectionIterator FvXMLSection::Begin()
{
	return FvXMLSectionIterator(this, 0);
}

FvXMLSectionIterator FvXMLSection::End()
{
	return FvXMLSectionIterator(this, this->CountChildren());
}

FvXMLSectionPtr FvXMLSection::CreateSection( const FvString &kName )
{
	FvXMLDocumentPtr spDocument = new FvXMLDocument();
	TiXmlElement *pkElement = new TiXmlElement(kName.c_str());
	if (spDocument->LinkEndChild(pkElement) == NULL)
		return NULL;

	return new FvXMLSection((TiXmlNode*)spDocument->RootElement(),spDocument);
}

FvXMLSectionPtr FvXMLSection::OpenSection(Ogre::DataStreamPtr &kStream)
{
	FV_ASSERT( !kStream.isNull() );
	size_t stLength = kStream->size();
	FvString kData;
	kData.reserve(stLength);

	char *pcBuf = new char[ stLength+1 ];
	pcBuf[0] = 0;

	if ( kStream->read( pcBuf, stLength ) <= 0 ) {
		delete [] pcBuf;
		return NULL;
	}

	const char *pcLastPos = pcBuf;
	const char *pcPos = pcBuf;

	pcBuf[stLength] = 0;
	while( *pcPos ) 
	{
		FV_ASSERT( pcPos < (pcBuf+stLength) );
		if ( *pcPos == 0xa ) 
		{
			kData.append( pcLastPos, (pcPos-pcLastPos+1) );	
			++pcPos;									
			pcLastPos = pcPos;							
			FV_ASSERT( pcPos <= (pcBuf+stLength) );
		}
		else if ( *pcPos == 0xd ) 
		{
			if ( (pcPos-pcLastPos) > 0 ) 
			{
				kData.append( pcLastPos, pcPos-pcLastPos );	
			}
			kData += (char)0xa;						

			if ( *(pcPos+1) == 0xa ) 
			{
				pcPos += 2;
				pcLastPos = pcPos;
				FV_ASSERT( pcPos <= (pcBuf+stLength) );
			}
			else 
			{
				++pcPos;
				pcLastPos = pcPos;
				FV_ASSERT( pcPos <= (pcBuf+stLength) );
			}
		}
		else 
		{
			++pcPos;
		}
	}
	if ( pcPos-pcLastPos ) 
	{
		kData.append( pcLastPos, pcPos-pcLastPos );
	}		
	delete [] pcBuf;
	pcBuf = 0;

	FvXMLDocumentPtr spDocument = new FvXMLDocument;
	spDocument->Parse(kData.c_str(), 0);

	if(spDocument->Error())
		return NULL;
	
	return new FvXMLSection(spDocument->RootElement(),spDocument);
}

bool FvXMLSection::Save(const FvString &kFileName)
{
	if(m_spXMLDocument)
		return m_spXMLDocument->SaveFile(kFileName.c_str());

	return false;
}