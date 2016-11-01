//{future header message}
#ifndef __FvBaseUserDataObjectDescription_H__
#define __FvBaseUserDataObjectDescription_H__

#include "FvEntityDefDefines.h"
#include "FvDataDescription.h"
#include "FvMethodDescription.h"

#include <FvNetTypes.h>
#include <FvXMLSection.h>

#include <string>
#include <vector>
#include <float.h>

class FvMD5;

class FvAddToStreamVisitor;

class FV_ENTITYDEF_API FvBaseUserDataObjectDescription
{
public:
	FvBaseUserDataObjectDescription();
	virtual ~FvBaseUserDataObjectDescription();

	virtual bool			Parse( const FvString & name, FvXMLSectionPtr pSection = NULL, bool isFinal = true );

	//! TODO: Dic需要统一一下,UserData和Entity
	void					AddToDictionary( FvXMLSectionPtr pSection, FvObj* pObject ) const;

	const FvString&			Name() const;

	FvUInt32				PropertyCount() const;
	FvDataDescription*		Property( FvUInt32 n ) const;
	virtual FvDataDescription*FindProperty( const FvString& name ) const;	//! TODO: 为什么是虚函数?
	bool					FindPropertyIndex( const FvString& name, FvUInt32& idx ) const;

protected:
	virtual bool			ParseInterface( FvXMLSectionPtr pSection, const char * interfaceName );
	virtual bool			ParseImplements( FvXMLSectionPtr pInterfaces );
	virtual	bool			ParseProperties( FvXMLSectionPtr pProperties ) = 0;

	virtual const FvString	GetDefsDir() const = 0;
	virtual const FvString	GetShortDefsDir() const = 0;
	virtual const FvString	GetClientDir() const = 0;
	virtual const FvString	GetCellDir() const = 0;
	virtual const FvString	GetBaseDir() const = 0;

	FvString			m_kName;

	typedef std::vector< FvDataDescription > Properties;
	Properties 			m_kProperties;

	typedef std::map< FvString, FvUInt32 > PropertyMap;
	PropertyMap			m_kPropertyMap;

#ifdef FV_EDITOR_ENABLED
	FvString			m_kEditorModel;
#endif

};

#include "FvBaseUserDataObjectDescription.inl"

FvXMLSectionPtr	OpenXMLInDef(const FvString& kDir, const FvString& kFile);
FvXMLSectionPtr OpenAliasesFile();
FvXMLSectionPtr OpenFatherAliasesFile();


#endif // __FvBaseUserDataObjectDescription_H__
