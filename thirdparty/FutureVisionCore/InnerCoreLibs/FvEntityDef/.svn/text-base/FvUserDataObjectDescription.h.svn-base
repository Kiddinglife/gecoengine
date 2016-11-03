//{future header message}
#ifndef __FvUserDataObjectDescription_H__
#define __FvUserDataObjectDescription_H__

#include <vector>
#include <float.h>

#include "FvEntityDefDefines.h"
#include "FvDataDescription.h"
#include "FvMethodDescription.h"
#include "FvBaseUserDataObjectDescription.h"

#include <FvXMLSection.h>


class FvMD5;

class FvAddToStreamVisitor;

class FV_ENTITYDEF_API FvUserDataObjectDescription: public FvBaseUserDataObjectDescription
{
public:
	FvUserDataObjectDescription()
	:m_eDomain(NONE)
	{}

	enum UserDataObjectDomain{
		NONE = 0x0,
		BASE = 0x1,
		CELL = 0x2,
		CLIENT =0x4,
	};
	const UserDataObjectDomain Domain() const { return m_eDomain; }

protected:
	virtual bool			ParseInterface( FvXMLSectionPtr pSection, const char * interfaceName );
	virtual	bool			ParseProperties( FvXMLSectionPtr pProperties );

	virtual const FvString	GetDefsDir() const;
	virtual const FvString	GetShortDefsDir() const;
	virtual const FvString	GetClientDir() const;
	virtual const FvString	GetCellDir() const;
	virtual const FvString	GetBaseDir() const;

	UserDataObjectDomain m_eDomain;
};

#endif // UserDataObject_DESCRIPTION_HPP
