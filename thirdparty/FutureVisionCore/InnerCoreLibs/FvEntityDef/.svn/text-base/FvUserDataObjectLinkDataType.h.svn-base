//{future header message}
#ifndef __FvUserDataObjectLinkDataType_H__
#define __FvUserDataObjectLinkDataType_H__

#include "FvEntityDefDefines.h"
#include "FvDataTypes.h"
#include "FvDataDescription.h"



class FV_ENTITYDEF_API FvUserDataObjectLinkDataType : public FvDataType
{
	friend class FvUserDataObjectLinkMetaDataType;
public:
	FvUserDataObjectLinkDataType(FvMetaDataType* pkMeta);

	static FvUserDataObjectLinkDataType& Instance() { return *ms_pkInstance; }
	static FvUserDataObjectLinkDataType* pInstance() { return ms_pkInstance; }

protected:
	virtual void SetDefaultValue( FvXMLSectionPtr pSection ) {}
	virtual FvObjPtr pDefaultValue() const { return ms_spDefaultValue; }

	virtual void AddToStream( FvObjPtr pValue, FvBinaryOStream & stream, bool isPersistentOnly ) const;
	virtual FvObjPtr CreateFromStream( FvBinaryIStream & stream, bool isPersistentOnly ) const;
	virtual void AddToSection( FvObjPtr pValue, FvXMLSectionPtr pSection ) const;
	virtual FvObjPtr CreateFromSection( FvXMLSectionPtr pSection ) const;

	virtual void AddToMD5(FvMD5& md5) const;

private:
	static	void CreateInstance(FvMetaDataType* pMetaDataType);
	static	void DestroyInstance();
	static FvUserDataObjectLinkDataType* ms_pkInstance;
	static FvObjPtr ms_spDefaultValue;
};

#endif // __FvUserDataObjectLinkDataType_H__