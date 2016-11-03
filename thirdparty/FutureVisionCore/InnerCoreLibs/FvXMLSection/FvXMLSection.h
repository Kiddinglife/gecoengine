//{future header message}
#ifndef __FvFvXMLSection_H__
#define __FvFvXMLSection_H__

#include <FvBaseTypes.h>
#include <FvMatrix.h>
#include <FvVector3.h>
#include <FvQuaternion.h>
#include <FvSmartPointer.h>

#include <OgreDataStream.h>

#include <tinyxml.h>

#ifdef FV_AS_STATIC_LIBS
#define FV_XMLSECTION_API
#else
#ifdef FV_XMLSECTION_EXPORT
#define FV_XMLSECTION_API FV_Proper_Export_Flag
#else
#define FV_XMLSECTION_API FV_Proper_Import_Flag
#endif
#endif

class FvXMLSection;

typedef FvSmartPointer<FvXMLSection> FvXMLSectionPtr;

class FvXMLDocument : public FvReferenceCount,public TiXmlDocument
{
public:
	FvXMLDocument(){}
	FvXMLDocument( const FvString& kTag ) : TiXmlDocument( kTag.c_str() ){}
};

typedef FvSmartPointer<FvXMLDocument> FvXMLDocumentPtr;

class FV_XMLSECTION_API FvXMLSectionIterator
{
public:
	FvXMLSectionIterator();
	FvXMLSectionIterator( FvXMLSectionPtr spXMLSection, size_t iIndex );

	bool operator==( const FvXMLSectionIterator &kXMLSection ) const;
	bool operator!=( const FvXMLSectionIterator &kXMLSection ) const;

	FvXMLSectionPtr operator*( void );
	FvXMLSectionIterator operator++(void );
	FvXMLSectionIterator operator++( int );

	FvString Tag( void );

private:

	FvXMLSectionPtr	m_spXMLSection;
	size_t m_iIndex;
};

namespace FvXMLSectionType 
{
	template <class C> struct DefaultValue
	{ static inline C Val() { return C(); } };
	template <> struct DefaultValue<bool>
	{ static inline bool Val() { return false; } };
	template <> struct DefaultValue<int>
	{ static inline int Val() { return 0; } };
	template <> struct DefaultValue<long>
	{ static inline long Val() { return 0; } };
	template <> struct DefaultValue<float>
	{ static inline float Val() { return 0; } };
	template <> struct DefaultValue<double>
	{ static inline double Val() { return 0; } };
	template <> struct DefaultValue<FvVector2>
	{ static inline FvVector2 Val() { return FvVector2::ZERO; } };
	template <> struct DefaultValue<FvVector3>
	{ static inline FvVector3 Val() { return FvVector3::ZERO; } };
	template <> struct DefaultValue<FvVector4>
	{ static inline FvVector4 Val() { return FvVector4::ZERO; } };
}

template <class C> struct FvXMLSectionTypeConverter
{
	static C As( FvXMLSectionPtr spSection,
		const C &kDefaultVal = FvXMLSectionType::DefaultValue<C>::Val() )
	{ return C::XMLSection_As( spSection, kDefaultVal ); }

	static bool Set( FvXMLSectionPtr spSection, const C &kValue)
	{ return C::XMLSection_Set( spSection, kValue ); }
};

class FV_XMLSECTION_API FvXMLSection : public FvReferenceCount
{
public:
	FvXMLSection(const FvString &kName);
	FvXMLSection(TiXmlNode *pkNode,FvXMLDocumentPtr spDoc = NULL);
	virtual ~FvXMLSection();

	class SearchIterator
	{
		friend class FvXMLSection;
	public:
		FvXMLSectionPtr operator*()
		{ return m_pkDataSection->OpenChild( m_iIndex ); }

		SearchIterator& operator++()
		{
			m_iIndex++;
			this->FindNext();
			return *this;
		}
		SearchIterator operator++( int )
		{
			SearchIterator copy( *this );
			++(*this);
			return copy;
		}

		bool operator==( const SearchIterator& other ) const
		{
			return (m_iIndex == other.m_iIndex);
		}

		bool operator!=( const SearchIterator& other ) const
		{
			return !this->operator==( other );
		}

	private:
		SearchIterator( FvXMLSection& ds, const FvString &kSectionName ) :
		   m_pkDataSection( &ds ), m_iIndex( 0 ), m_kSectionName( kSectionName )
		   {
			   this->FindNext();
		   }

		   SearchIterator() :
		   m_pkDataSection( NULL ), m_iIndex( INT_MAX ), m_kSectionName()
		   {}

		   void FindNext()
		   {
			   if (m_pkDataSection)
			   {
				   int numChildren = (int)m_pkDataSection->CountChildren();
				   while (m_iIndex < numChildren)
				   {
					   if (m_pkDataSection->ChildSectionName( m_iIndex ) ==
						   m_kSectionName)
					   {
						   return;
					   }
					   ++m_iIndex;
				   }
			   }
			   m_iIndex = INT_MAX;
		   }

		   FvXMLSection *m_pkDataSection;
		   int m_iIndex;
		   FvString m_kSectionName;
	};

	SearchIterator BeginSearch( const FvString kSectionName )
	{
		return SearchIterator( *this, kSectionName );
	}

	static const SearchIterator &EndOfSearch()
	{
		static SearchIterator s_kSndOfSearchIterator;
		return s_kSndOfSearchIterator;
	}

	void SectionName( const FvString &kName );
	FvString SectionName( void ) const;

	FvXMLSectionPtr NewSection( const FvString &kTag );
	FvXMLSectionPtr OpenSection( const FvString &kTag, bool bCreate = false );
	FvXMLSectionPtr OpenFirstSection( void );
	void OpenSections( const FvString &kTag,
		std::vector<FvXMLSectionPtr> &kDest );

	FvXMLSectionPtr OpenChild( const FvString &kTag );
	FvXMLSectionPtr OpenChild( size_t iIndex );
	FvString ChildSectionName( size_t iIndex );
	size_t CountChildren( void );

	void DelChild( const FvString &kTag );
	void DelChild( FvXMLSectionPtr spSection );
	void DelChildren( void );

	TiXmlNode::NodeType GetType() { if(m_pkXMLNode) return TiXmlNode::NodeType(m_pkXMLNode->Type()); else return TiXmlNode::UNKNOWN; }

	template <class C> C Read( const FvString &kTag,
		const C &kDefaultVal )
	{
		FvXMLSectionPtr spSection = this->OpenSection( kTag, false );

		if (!spSection) return kDefaultVal;
		return spSection->As<C>( kDefaultVal );
	}

	bool ReadBool( const FvString &kTag,
		bool bDefaultVal = false, bool bCreate = false );
	int ReadInt( const FvString &kTag,
		int iDefaultVal = 0, bool bCreate = false );
	long ReadLong( const FvString &kTag,
		long iDefaultVal = 0, bool bCreate = false );
	float ReadFloat( const FvString &kTag,
		const float &fDefaultVal = 0.0f, bool bCreate = false );
	FvString ReadString( const FvString &kTag,
		const FvString &kDefaultVal = "", bool bCreate = false );
	FvVector2 ReadVector2( const FvString &kTag,
		const FvVector2 &kDefaultVal = FvVector2::ZERO, bool bCreate = false );
	FvVector3 ReadVector3( const FvString &kTag,
		const FvVector3 &kDefaultVal = FvVector3::ZERO, bool bCreate = false );
	FvVector4 ReadVector4( const FvString &kTag,
		const FvVector4 &kDefaultVal = FvVector4::ZERO, bool bCreate = false );
	FvQuaternion ReadQuaternion( const FvString &kTag,
		const FvQuaternion &kDefaultVal = FvQuaternion(), bool bCreate = false );
	FvMatrix ReadMatrix34( const FvString &kTag,
		const FvMatrix &kDefaultVal = FvMatrix() );
	void ReadVector2s( const FvString &kTag, 
		std::vector<FvVector2> &kDest );
	void ReadVector3s( const FvString &kTag, 
		std::vector<FvVector3> &kDest );
	void ReadVector4s( const FvString &kTag, 
		std::vector<FvVector4> &kDest );
	void ReadQuaternion4s( const FvString &kTag, 
		std::vector<FvQuaternion> &kDest );
	void ReadStrings( const FvString &kTag,
		std::vector<FvString> &kDest );
	FvString ReadBlob( const FvString &kTag,
		const FvString &kDefaultVal = "", bool bCreate = false );

	template <class C> bool Write( const FvString &kTag, const C & kValue)
	{
		FvXMLSectionPtr spSection = this->OpenSection( kTag, true );

		if (!spSection) return false;
		return spSection->Set( kValue );
	}

	bool WriteBool( const FvString &kTag, 
		bool bValue );
	bool WriteInt( const FvString &kTag, 
		int iValue );
	bool WriteLong( const FvString &kTag, 
		long iValue );
	bool WriteFloat( const FvString &kTag, 
		float fValue );
	bool WriteString( const FvString &kTag, 
		FvString kValue );
	bool WriteVector2( const FvString &kTag, 
		FvVector2 kValue );
	bool WriteVector3( const FvString &kTag, 
		FvVector3 kValue );
	bool WriteVector4( const FvString &kTag, 
		FvVector4 kValue );
	bool WriteQuaternion( const FvString &kTag, 
		FvQuaternion kValue );
	bool WriteBlob( const FvString &kTag, 
		const FvString &kValue );

	template <class C> C As()
	{ 
		return FvXMLSectionTypeConverter<C>::As( this ); 
	}

	template <class C> C As( const C &kDefaultVal )
	{ 
		return FvXMLSectionTypeConverter<C>::As( this, kDefaultVal ); 
	}

	bool AsBool( bool bDefaultVal = false );
	int AsInt( int iDefaultVal = 0 );
	long AsLong( long iDefaultVal = 0 );
	float AsFloat( const float &fDefaultVal = 0.0f );
	double AsDouble( const float &fDefaultVal = 0.0f );
	FvString AsString( const FvString &kDefaultVal = "" );
	FvVector2 AsVector2( const FvVector2 &kDefaultVal = FvVector2::ZERO );
	FvVector3 AsVector3( const FvVector3 &kDefaultVal = FvVector3::ZERO );	
	FvVector4 AsVector4( const FvVector4 &kDefaultVal = FvVector4::ZERO );
	FvQuaternion AsQuaternion( const FvQuaternion &kDefaultVal = FvQuaternion() );	
	FvMatrix AsMatrix34( const FvMatrix &kDefaultVal = FvMatrix() );
	FvString AsBlob( const FvString &kDefaultVal = "" );

	template <class C> bool Set( const C &kValue )
	{ 
		return FvXMLSectionTypeConverter<C>::Set( this, kValue ); 
	}
	bool SetBool( bool bValue );
	bool SetInt( int iValue );
	bool SetLong( long iValue );
	bool SetFloat( float fValue );
	bool SetDouble( float fValue );
	bool SetString( const FvString& kValue );
	bool SetVector2( const FvVector2& kValue );
	bool SetVector3( const FvVector3& kValue );
	bool SetVector4( const FvVector4& kValue );
	bool SetQuaternion( const FvQuaternion& kValue );
	bool SetMatrix34( const FvMatrix &kValue );
	bool SetBlob( const FvString &kValue );

	bool AsAttributeBool( const FvString &kAttribute,
		bool bDefaultVal = false );
	int AsAttributeInt( const FvString &kAttribute,
		int iDefaultVal = 0 );
	float AsAttributeFloat( const FvString &kAttribute,
		const float &fDefaultVal = 0.0f );
	FvString AsAttributeString( const FvString &kAttribute,
		const FvString &kDefaultVal = "" );
	FvVector2 AsAttributeVector2( const FvString &kAttribute,
		const FvVector2 &kDefaultVal = FvVector2::ZERO );
	FvVector3 AsAttributeVector3( const FvString &kAttribute,
		const FvVector3 &kDefaultVal = FvVector3::ZERO );
	FvVector4 AsAttributeVector4( const FvString &kAttribute,
		const FvVector4 &kDefaultVal = FvVector4::ZERO );
	FvQuaternion AsAttributeQuaternion( const FvString &kAttribute,
		const FvQuaternion &kDefaultVal = FvQuaternion() );

	bool GetAttributeBool( const FvString &kTag,
		const FvString &kAttribute,
		bool bDefaultVal = false );
	int GetAttributeInt( const FvString &kTag,
		const FvString &kAttribute,
		int iDefaultVal = 0 );
	float GetAttributeFloat( const FvString &kTag,
		const FvString &kAttribute,
		const float &fDefaultVal = 0.0f );
	FvString GetAttributeString( const FvString &kTag,
		const FvString &kAttribute,
		const FvString &kDefaultVal = "" );
	FvVector2 GetAttributeVector2( const FvString &kTag,
		const FvString &kAttribute,
		const FvVector2 &kDefaultVal = FvVector2::ZERO );
	FvVector3 GetAttributeVector3( const FvString &kTag,
		const FvString &kAttribute,
		const FvVector3 &kDefaultVal = FvVector3::ZERO );
	FvVector4 GetAttributeVector4( const FvString &kTag,
		const FvString &kAttribute,
		const FvVector4 &kDefaultVal = FvVector4::ZERO );
	FvQuaternion GetAttributeQuaternion( const FvString &kTag,
		const FvString &kAttribute,
		const FvQuaternion &kDefaultVal = FvQuaternion() );

	bool SetAttributeBool( const FvString &kAttribute,
		bool bValue );
	bool SetAttributeInt( const FvString &kAttribute,
		int iValue );
	bool SetAttributeFloat( const FvString &kAttribute,
		float fValue );
	bool SetAttributeString( const FvString &kAttribute,
		const FvString& kValue );
	bool SetAttributeVector2( const FvString &kAttribute,
		const FvVector2& kValue );
	bool SetAttributeVector3( const FvString &kAttribute,
		const FvVector3& kValue );
	bool SetAttributeVector4( const FvString &kAttribute,
		const FvVector4& kValue );
	bool SetAttributeQuaternion( const FvString &kAttribute,
		const FvQuaternion& kValue );

	bool SetAttributeBool( const FvString &kTag,
		const FvString &kAttribute,
		bool bValue );
	bool SetAttributeInt( const FvString &kTag,
		const FvString &kAttribute,
		int iValue );
	bool SetAttributeFloat( const FvString &kTag,
		const FvString &kAttribute,
		float fValue );
	bool SetAttributeString( const FvString &kTag,
		const FvString &kAttribute,
		const FvString &kValue );
	bool SetAttributeVector2( const FvString &kTag,
		const FvString &kAttribute,
		const FvVector2 &kValue );
	bool SetAttributeVector3( const FvString &kTag,
		const FvString &kAttribute,
		const FvVector3 &kValue );
	bool SetAttributeVector4( const FvString &kTag,
		const FvString &kAttribute,
		const FvVector4 &kValue );
	bool SetAttributeQuaternion( const FvString &kTag,
		const FvString &kAttribute,
		const FvQuaternion &kValue );

	void RemoveAttribute( const FvString &kAttribute );
	void RemoveAttributes( void );
	
	void Copy( const FvXMLSectionPtr spSection,
		bool bModifyCurrent = true );	
	void CopySections( const FvXMLSectionPtr spSection, FvString kTag );
	void CopySections( const FvXMLSectionPtr spSection );

	FvXMLSectionIterator Begin( void );
	FvXMLSectionIterator End( void );

	static FvXMLSectionPtr CreateSection( const FvString &kName );
	static FvXMLSectionPtr OpenSection( Ogre::DataStreamPtr &kStream );
	bool Save( const FvString &kFileName );

private:

	FvXMLDocumentPtr m_spXMLDocument;

	typedef std::multimap<FvString,FvXMLSectionPtr> XMLSectionNameMap;
	typedef std::vector<FvXMLSectionPtr> XMLSectionIndexMap;

	XMLSectionNameMap m_kSectionNameMap;
	XMLSectionIndexMap m_kSectionIndexMap;
	TiXmlNode *m_pkXMLNode;
};

#define FV_XMLSECTION_TEMPLATES( TYPE, AS_FUNCTION, BE_FUNCTION )			\
	template <> struct FvXMLSectionTypeConverter<TYPE>						\
	{																		\
		static TYPE As( FvXMLSectionPtr spSection, const TYPE &kDefaultVal )\
			{ return spSection->AS_FUNCTION( kDefaultVal ); }				\
		static TYPE As( FvXMLSectionPtr spSection )							\
			{ return spSection->AS_FUNCTION(); }							\
		static bool Set( FvXMLSectionPtr spSection, const TYPE &kValue )	\
			{ return spSection->BE_FUNCTION( kValue ); }					\
	};

#define FV_XMLSECTION_TEMPLATES_PROMOTE( TYPE, DSTYPE,						\
												AS_FUNCTION, BE_FUNCTION )	\
	template <> struct FvXMLSectionTypeConverter<TYPE>						\
	{																		\
		static TYPE As( FvXMLSectionPtr spSection, const TYPE &kDefaultVal )\
		{ return TYPE( spSection->AS_FUNCTION( DSTYPE(kDefaultVal) ) ); }	\
		static TYPE As( FvXMLSectionPtr spSection )							\
		{ return TYPE( spSection->AS_FUNCTION() ); }						\
		static bool Set( FvXMLSectionPtr spSection, const TYPE &kValue )	\
		{ return spSection->BE_FUNCTION( DSTYPE(kValue) ); }				\
	};

#define FV_XMLSECTION_TEMPLATES_NOFORMAT( TYPE, AS_FUNCTION, BE_FUNCTION )	\
	template <> struct FvXMLSectionTypeConverter<TYPE>						\
	{																		\
		static TYPE As( FvXMLSectionPtr spSection, const TYPE &kDefaultVal )\
			{ return spSection->AS_FUNCTION( kDefaultVal ); }				\
		static TYPE As( FvXMLSectionPtr spSection )							\
			{ return spSection->AS_FUNCTION(); }							\
		static bool Set( FvXMLSectionPtr spSection, const TYPE &kValue )	\
			{ return spSection->BE_FUNCTION( kValue ); }					\
	};

#define FV_XMLSECTION_TEMPLATES_USESTRING( TYPE )							\
	template <> struct FvXMLSectionTypeConverter<TYPE>						\
	{																		\
	static TYPE As( FvXMLSectionPtr spSection,								\
	const TYPE &kDefaultVal =  TYPE() )										\
	{																		\
			std::istringstream kIn( spSection->AsString() );				\
			TYPE kVal;														\
			kIn >> kVal;													\
			if (kIn.fail())													\
			kVal = kDefaultVal;												\
																			\
			return kVal;													\
	}																		\
	static bool Set( FvXMLSectionPtr spSection, const TYPE &kValue )		\
	{ 																		\
			std::ostringstream kOut;										\
			kOut << kValue;													\
			return spSection->SetString( kOut.str() );						\
	}																		\
};

FV_XMLSECTION_TEMPLATES( bool, AsBool, SetBool )
FV_XMLSECTION_TEMPLATES( int, AsInt, SetInt )
FV_XMLSECTION_TEMPLATES( long, AsLong, SetLong )
FV_XMLSECTION_TEMPLATES( float, AsFloat, SetFloat )
FV_XMLSECTION_TEMPLATES( double, AsDouble, SetDouble )
FV_XMLSECTION_TEMPLATES_NOFORMAT( FvString, AsString, SetString )
//FV_XMLSECTION_TEMPLATES_NOFORMAT( std::wstring, AsWideString, SetWideString )
FV_XMLSECTION_TEMPLATES( FvVector2, AsVector2, SetVector2 )
FV_XMLSECTION_TEMPLATES( FvVector3, AsVector3, SetVector3 )
FV_XMLSECTION_TEMPLATES( FvVector4, AsVector4, SetVector4 )
FV_XMLSECTION_TEMPLATES( FvMatrix, AsMatrix34, SetMatrix34 )

FV_XMLSECTION_TEMPLATES_PROMOTE( FvInt8, int, AsInt, SetInt )
FV_XMLSECTION_TEMPLATES_PROMOTE( FvInt16, int, AsInt, SetInt )
FV_XMLSECTION_TEMPLATES_PROMOTE( FvUInt8, int, AsInt, SetInt )
FV_XMLSECTION_TEMPLATES_PROMOTE( FvUInt16, int, AsInt, SetInt )
FV_XMLSECTION_TEMPLATES_PROMOTE( FvUInt32, int, AsInt, SetInt )

FV_XMLSECTION_TEMPLATES_USESTRING(FvUInt64)
FV_XMLSECTION_TEMPLATES_USESTRING(FvInt64)


namespace FvXMLData
{
	template<class T>
	void Read(FvXMLSectionPtr spSection, T& kVal);
	template<class T>
	void Write(FvXMLSectionPtr spSection, const T& kVal);

	template<class T>
	void Read(const char* pcName, FvXMLSectionPtr spSection, T& kVal)
	{
		FvXMLSectionPtr spChild = spSection->OpenSection(pcName);
		if(spChild)
			Read(spChild, kVal);
	}
	template<class T>
	void Write(const char* pcName, FvXMLSectionPtr spSection, const T& kVal)
	{
		FvXMLSectionPtr spChild = spSection->OpenSection(pcName);
		if(spChild)
			spChild->DelChildren();
		else
			spChild = spSection->NewSection(pcName);
		if(!spChild)
			return;
		Write(spChild, kVal);
	}

	template<class T, class A>
	void Read(FvXMLSectionPtr spSection, std::vector<T,A>& kVal)
	{
		std::vector<FvXMLSectionPtr> kItems;
		spSection->OpenSections("item", kItems);
		if(kItems.empty())
			return;
		kVal.reserve(kItems.size());
		bool bFirstItem = true;
		std::vector<FvXMLSectionPtr>::iterator itrB = kItems.begin();
		std::vector<FvXMLSectionPtr>::iterator itrE = kItems.end();
		for(; itrB!=itrE; ++itrB)
		{
			if((*itrB)->GetType() != TiXmlNode::ELEMENT)
				continue;
			if(bFirstItem)
			{
				bFirstItem = false;
				kVal.clear();
			}
			T t;
			Read(*itrB, t);
			kVal.push_back(t);
		}
	}
	template<class T, class A>
	void Write(FvXMLSectionPtr spSection, const std::vector<T,A>& kVal)
	{
		if(kVal.empty())
			return;
		std::vector<T,A>::const_iterator itrB = kVal.begin();
		std::vector<T,A>::const_iterator itrE = kVal.end();
		for(; itrB!=itrE; ++itrB)
		{
			FvXMLSectionPtr spItem = spSection->NewSection("item");
			Write(spItem, *itrB);
		}
	}

	template<>
	FV_INLINE void Read<bool>(FvXMLSectionPtr spSection, bool& kVal) { kVal = spSection->As<bool>(kVal); }
	template<>
	FV_INLINE void Write<bool>(FvXMLSectionPtr spSection, const bool& kVal) { spSection->Set<bool>(kVal); }
	template<>
	FV_INLINE void Read<FvInt8>(FvXMLSectionPtr spSection, FvInt8& kVal) { kVal = spSection->As<FvInt8>(kVal); }
	template<>
	FV_INLINE void Write<FvInt8>(FvXMLSectionPtr spSection, const FvInt8& kVal) { spSection->Set<FvInt8>(kVal); }
	template<>
	FV_INLINE void Read<FvInt16>(FvXMLSectionPtr spSection, FvInt16& kVal) { kVal = spSection->As<FvInt16>(kVal); }
	template<>
	FV_INLINE void Write<FvInt16>(FvXMLSectionPtr spSection, const FvInt16& kVal) { spSection->Set<FvInt16>(kVal); }
	template<>
	FV_INLINE void Read<FvInt32>(FvXMLSectionPtr spSection, FvInt32& kVal) { kVal = spSection->As<FvInt32>(kVal); }
	template<>
	FV_INLINE void Write<FvInt32>(FvXMLSectionPtr spSection, const FvInt32& kVal) { spSection->Set<FvInt32>(kVal); }
	template<>
	FV_INLINE void Read<FvInt64>(FvXMLSectionPtr spSection, FvInt64& kVal) { kVal = spSection->As<FvInt64>(kVal); }
	template<>
	FV_INLINE void Write<FvInt64>(FvXMLSectionPtr spSection, const FvInt64& kVal) { spSection->Set<FvInt64>(kVal); }
	template<>
	FV_INLINE void Read<FvUInt8>(FvXMLSectionPtr spSection, FvUInt8& kVal) { kVal = spSection->As<FvUInt8>(kVal); }
	template<>
	FV_INLINE void Write<FvUInt8>(FvXMLSectionPtr spSection, const FvUInt8& kVal) { spSection->Set<FvUInt8>(kVal); }
	template<>
	FV_INLINE void Read<FvUInt16>(FvXMLSectionPtr spSection, FvUInt16& kVal) { kVal = spSection->As<FvUInt16>(kVal); }
	template<>
	FV_INLINE void Write<FvUInt16>(FvXMLSectionPtr spSection, const FvUInt16& kVal) { spSection->Set<FvUInt16>(kVal); }
	template<>
	FV_INLINE void Read<FvUInt32>(FvXMLSectionPtr spSection, FvUInt32& kVal) { kVal = spSection->As<FvUInt32>(kVal); }
	template<>
	FV_INLINE void Write<FvUInt32>(FvXMLSectionPtr spSection, const FvUInt32& kVal) { spSection->Set<FvUInt32>(kVal); }
	template<>
	FV_INLINE void Read<FvUInt64>(FvXMLSectionPtr spSection, FvUInt64& kVal) { kVal = spSection->As<FvUInt64>(kVal); }
	template<>
	FV_INLINE void Write<FvUInt64>(FvXMLSectionPtr spSection, const FvUInt64& kVal) { spSection->Set<FvUInt64>(kVal); }
	template<>
	FV_INLINE void Read<float>(FvXMLSectionPtr spSection, float& kVal) { kVal = spSection->As<float>(kVal); }
	template<>
	FV_INLINE void Write<float>(FvXMLSectionPtr spSection, const float& kVal) { spSection->Set<float>(kVal); }
	template<>
	FV_INLINE void Read<double>(FvXMLSectionPtr spSection, double& kVal) { kVal = spSection->As<double>(kVal); }
	template<>
	FV_INLINE void Write<double>(FvXMLSectionPtr spSection, const double& kVal) { spSection->Set<double>(kVal); }
	template<>
	FV_INLINE void Read<FvVector2>(FvXMLSectionPtr spSection, FvVector2& kVal) { kVal = spSection->As<FvVector2>(kVal); }
	template<>
	FV_INLINE void Write<FvVector2>(FvXMLSectionPtr spSection, const FvVector2& kVal) { spSection->Set<FvVector2>(kVal); }
	template<>
	FV_INLINE void Read<FvVector3>(FvXMLSectionPtr spSection, FvVector3& kVal) { kVal = spSection->As<FvVector3>(kVal); }
	template<>
	FV_INLINE void Write<FvVector3>(FvXMLSectionPtr spSection, const FvVector3& kVal) { spSection->Set<FvVector3>(kVal); }
	template<>
	FV_INLINE void Read<FvVector4>(FvXMLSectionPtr spSection, FvVector4& kVal) { kVal = spSection->As<FvVector4>(kVal); }
	template<>
	FV_INLINE void Write<FvVector4>(FvXMLSectionPtr spSection, const FvVector4& kVal) { spSection->Set<FvVector4>(kVal); }
	template<>
	FV_INLINE void Read<FvMatrix>(FvXMLSectionPtr spSection, FvMatrix& kVal) { kVal = spSection->As<FvMatrix>(kVal); }
	template<>
	FV_INLINE void Write<FvMatrix>(FvXMLSectionPtr spSection, const FvMatrix& kVal) { spSection->Set<FvMatrix>(kVal); }
	template<>
	FV_INLINE void Read<FvString>(FvXMLSectionPtr spSection, FvString& kVal) { kVal = spSection->As<FvString>(kVal); }
	template<>
	FV_INLINE void Write<FvString>(FvXMLSectionPtr spSection, const FvString& kVal) { spSection->Set<FvString>(kVal); }
}


#endif // __FvXMLSection_H__