//{future header message}
#ifndef __FvUserDataObjectType_H__
#define __FvUserDataObjectType_H__

#include "FvEntityDefDefines.h"
#include "FvUserDataObjectDescription.h"
#include <FvSmartPointer.h>
#include <map>


class FvUserDataObject;
class FvUserDataObjectType;
typedef FvSmartPointer<FvUserDataObjectType> FvUserDataObjectTypePtr;
typedef std::map<const FvString, FvUserDataObjectTypePtr> FvUserDataObjectTypes;
struct FvUserDataObjectInitData;


class FV_ENTITYDEF_API FvUserDataObjectType : public FvReferenceCount
{
public:
	FvUserDataObjectType(const FvUserDataObjectDescription& kDescription);
	~FvUserDataObjectType();

	FvUserDataObject* NewUserDataObject( const FvUserDataObjectInitData& kInitData ) const;

	bool HasProperty( const char* pcAttr ) const;

	static bool Init();
	static bool Load( FvUserDataObjectTypes& kTypes );	
	static void Migrate( FvUserDataObjectTypes& kTypes );
	static void ClearStatics();
	static FvUserDataObjectTypePtr	GetType( const char* pcClassName );
	static FvUserDataObjectTypes&	GetTypes() { return ms_kCurTypes; }

	const FvUserDataObjectDescription&	Description()  const { return m_kDescription; };


private:
	FvUserDataObjectDescription		m_kDescription;

	static FvUserDataObjectTypes	ms_kCurTypes;
};


#endif//__FvUserDataObjectType_H__

