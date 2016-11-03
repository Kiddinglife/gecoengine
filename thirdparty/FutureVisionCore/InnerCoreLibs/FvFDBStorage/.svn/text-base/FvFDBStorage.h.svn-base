#ifndef NIFDBSTORAGE_H
#define NIFDBSTORAGE_H

#include <FvBaseTypes.h>
#include <OgreResource.h>

#include "FvFDBStorageDefine.h"

class FV_FDBSTORAGE_API FvFDBStorage : public Ogre::Resource
{
public:

	struct FDBHeader
	{
		FvUInt32 m_uiHeader;
		FvUInt32 m_uiRecordCount;
		FvUInt32 m_uiRecordSize;
		FvUInt32 m_uiStringSize;
		FvUInt32 m_uiFieldCount;
	};

	enum FieldFlag
	{
		FIELD_FLAG_BOOL		= 0x01,
		FIELD_FLAG_BYTE		= 0x02,
		FIELD_FLAG_INT		= 0x04,
		FIELD_FLAG_FLOAT	= 0x08,
		FIELD_FLAG_STRING	= 0x10,
		FIELD_FLAG_WSTRING	= 0x20,
		FIELD_FLAG_INDEX	= 0x40,
	};

	static const FvUInt32 MAX_FIELD_COUNT = 2048;
	static const FvUInt32 MAX_DATA_SIZE = MAX_FIELD_COUNT * 1024 * 10;

	FvFDBStorage(Ogre::ResourceManager *pkCreate, const FvString &kName, Ogre::ResourceHandle uiHandle,
		const FvString &kGroupName, bool bIsManual = false, Ogre::ManualResourceLoader *pkLoader = 0);

	~FvFDBStorage();

	void loadImpl(void);

	void unloadImpl(void);

	size_t calculateSize(void) const;

	template<class STRUCT>
	STRUCT const *LookupEntry(const char* kIndex ,FvUInt32 uiField = 0)
	{
		if(sizeof(STRUCT) != m_kHeader.m_uiRecordSize)
			return NULL;

		StringIndex &kIndexMap = m_kStringIndex[uiField];
		StringIndex::iterator kIt = kIndexMap.find(kIndex);
		if(kIt == kIndexMap.end())
			return NULL;

		return (STRUCT*)kIt->second;
	}

	template<class STRUCT>
	STRUCT const *LookupEntry(const wchar_t* kIndex ,FvUInt32 uiField = 0)
	{
		if(sizeof(STRUCT) != m_kHeader.m_uiRecordSize)
			return NULL;

		WStringIndex &kIndexMap = m_kWStringIndex[uiField];
		WStringIndex::iterator kIt = kIndexMap.find(kIndex);
		if(kIt == kIndexMap.end())
			return NULL;

		return (STRUCT*)kIt->second;
	}

	template<class STRUCT>
	STRUCT const *LookupEntry(int kIndex ,FvUInt32 uiField = 0)
	{
		int size = sizeof(STRUCT);
		if(size != m_kHeader.m_uiRecordSize)
			return NULL;

		NumericIndex &kIndexMap = m_kNumericIndex[uiField];
		NumericIndex::iterator kIt = kIndexMap.find(kIndex);
		if(kIt == kIndexMap.end())
			return NULL;

		return (STRUCT*)kIt->second;
	}


	const stdext::hash_map<int,void*>& GetNumericIdx(FvUInt32 uiField = 0);

	const FDBHeader& GetHeader()const{return m_kHeader;}

private:


	FDBHeader m_kHeader;

	unsigned char *m_pcFieldDescription;

	unsigned char *m_pcData;

	unsigned char *m_pcStringTable;

	/// ×Ö·ûË÷Òý
	typedef stdext::hash_map<FvString,void*> StringIndex;
	/// ¿í×Ö·ûË÷Òý
	typedef stdext::hash_map<std::wstring,void*> WStringIndex;
	/// Êý×ÖË÷Òý
	typedef stdext::hash_map<int,void*> NumericIndex;

	/// Ë÷Òý±í
	stdext::hash_map<FvUInt32,WStringIndex> m_kWStringIndex;
	stdext::hash_map<FvUInt32,StringIndex> m_kStringIndex;
	stdext::hash_map<FvUInt32,NumericIndex> m_kNumericIndex;
};

class FV_FDBSTORAGE_API FvFDBStoragePtr : public Ogre::SharedPtr<FvFDBStorage> 
{
public:
	FvFDBStoragePtr() : Ogre::SharedPtr<FvFDBStorage>() {}
	explicit FvFDBStoragePtr(FvFDBStorage* rep) : Ogre::SharedPtr<FvFDBStorage>(rep) {}
	FvFDBStoragePtr(const FvFDBStoragePtr& r) : Ogre::SharedPtr<FvFDBStorage>(r) {} 
	FvFDBStoragePtr(const Ogre::ResourcePtr& r);
	FvFDBStoragePtr& operator=(const Ogre::ResourcePtr& r);
protected:
	void destroy(void);
};

#endif