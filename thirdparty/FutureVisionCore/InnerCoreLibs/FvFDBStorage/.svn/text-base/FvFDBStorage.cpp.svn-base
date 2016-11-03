#include "FvFDBStorage.h"
#include <FvDebug.h>
#include <OgreResourceGroupManager.h>

FvFDBStorage::FvFDBStorage(Ogre::ResourceManager *pkCreate, const FvString &kName, Ogre::ResourceHandle uiHandle,
				 const FvString &kGroupName, bool bIsManual, Ogre::ManualResourceLoader *pkLoader):
Ogre::Resource(pkCreate,kName,uiHandle,kGroupName,bIsManual,pkLoader),
m_pcFieldDescription(NULL),
m_pcData(NULL),
m_pcStringTable(NULL)
{
	memset(&m_kHeader,0,sizeof(FDBHeader));
}

FvFDBStorage::~FvFDBStorage()
{
	unload();
}

void FvFDBStorage::loadImpl(void)
{
	Ogre::DataStreamPtr spRWDataStream = Ogre::ResourceGroupManager::getSingleton().
		openResource(mName,mGroup);

	if(!spRWDataStream.isNull())
	{
		spRWDataStream->read(&m_kHeader,sizeof(FDBHeader));
		if(m_kHeader.m_uiHeader != 0x00424446)
		{
			return;                                       //'FDB\0'
		}

		if(m_kHeader.m_uiFieldCount > MAX_FIELD_COUNT) 
		{
			FV_ASSERT(false&&"Field count is too long");
			return;
		}

		m_pcFieldDescription = new unsigned char[m_kHeader.m_uiFieldCount];
		spRWDataStream->read(m_pcFieldDescription,m_kHeader.m_uiFieldCount);

		size_t stDataSize = m_kHeader.m_uiRecordCount * m_kHeader.m_uiRecordSize + m_kHeader.m_uiStringSize;
		if(stDataSize > MAX_DATA_SIZE)
		{
			FV_ASSERT(false&&"Data size is too long");
			return;
		}

		FvUInt32 uiFieldNameSize;
		spRWDataStream->read(&uiFieldNameSize,4);
		for(FvUInt32 i = 0; i < uiFieldNameSize; i++)
		{
			FvUInt32 uiStrLength;
			spRWDataStream->read(&uiStrLength,4);
			spRWDataStream->skip(uiStrLength);
		}

		m_pcData = new unsigned char[stDataSize];
		m_pcStringTable = m_pcData + m_kHeader.m_uiRecordCount * m_kHeader.m_uiRecordSize;
		spRWDataStream->read(m_pcData,stDataSize);

		/// 连结字符，建立索引
		for(FvUInt32 uiRecord = 0; uiRecord < m_kHeader.m_uiRecordCount; uiRecord++)
		{
			size_t stFieldPos = 0;
			unsigned char *pcRecordPtr = (m_pcData + m_kHeader.m_uiRecordSize*uiRecord);
			for(FvUInt32 uiFiled = 0; uiFiled < m_kHeader.m_uiFieldCount; uiFiled++)
			{
				if((m_pcFieldDescription[uiFiled]&FIELD_FLAG_INDEX) &&
					(m_pcFieldDescription[uiFiled]&FIELD_FLAG_INT))
				{
					NumericIndex &kIndexMap = m_kNumericIndex[uiFiled];
					kIndexMap.insert(NumericIndex::value_type(
						*(FvUInt32*)(pcRecordPtr + stFieldPos),
						pcRecordPtr));
				}
				if((m_pcFieldDescription[uiFiled]&FIELD_FLAG_INDEX) &&
					(m_pcFieldDescription[uiFiled]&FIELD_FLAG_STRING))
				{
					StringIndex &kIndexMap = m_kStringIndex[uiFiled];
					kIndexMap.insert(StringIndex::value_type(
						FvString((char*)m_pcStringTable + *(int*)(pcRecordPtr + stFieldPos)),
						pcRecordPtr));
				}
				if((m_pcFieldDescription[uiFiled]&FIELD_FLAG_INDEX) &&
					(m_pcFieldDescription[uiFiled]&FIELD_FLAG_WSTRING))
				{
					WStringIndex &kIndexMap = m_kWStringIndex[uiFiled];
					kIndexMap.insert(WStringIndex::value_type(
						std::wstring((wchar_t*)((char*)m_pcStringTable + *(int*)(pcRecordPtr + stFieldPos))),
						pcRecordPtr));
				}
				if((m_pcFieldDescription[uiFiled]&FIELD_FLAG_BOOL) ||
					(m_pcFieldDescription[uiFiled]&FIELD_FLAG_BYTE))
				{
					stFieldPos += 1;
					continue;
				}
				if(m_pcFieldDescription[uiFiled]&FIELD_FLAG_INT)
				{
					stFieldPos += 4;
					continue;
				}
				if((m_pcFieldDescription[uiFiled]&FIELD_FLAG_STRING) ||
					(m_pcFieldDescription[uiFiled]&FIELD_FLAG_WSTRING))
				{
					unsigned char *pcStringPtr = pcRecordPtr + stFieldPos;
					if(FvUInt32(*(int*)pcStringPtr) >= m_kHeader.m_uiStringSize)
					{
						*(void**)(pcStringPtr) = NULL;
						continue;
					}
					*(void**)(pcStringPtr) = m_pcStringTable + FvUInt32(*(int*)pcStringPtr);
					stFieldPos += sizeof(void*);
					continue;
				}
				if(m_pcFieldDescription[uiFiled]&FIELD_FLAG_FLOAT)
				{
					stFieldPos += 4;
					continue;
				}
			}
		}
	}
}

void FvFDBStorage::unloadImpl(void)
{
	delete[] m_pcFieldDescription;
	delete[] m_pcData;
	m_kStringIndex.clear();
	m_kNumericIndex.clear();
}
const FvFDBStorage::NumericIndex& FvFDBStorage::GetNumericIdx(FvUInt32 uiField /* = 0 */)
{
	const NumericIndex &kIndexMap = m_kNumericIndex[uiField];
	return kIndexMap;
}

size_t FvFDBStorage::calculateSize(void) const
{
	return size_t(20) + m_kHeader.m_uiFieldCount + m_kHeader.m_uiRecordCount * m_kHeader.m_uiRecordSize + m_kHeader.m_uiStringSize;
}

FvFDBStoragePtr::FvFDBStoragePtr(const Ogre::ResourcePtr& r) : Ogre::SharedPtr<FvFDBStorage>()
{
	OGRE_MUTEX_CONDITIONAL(r.OGRE_AUTO_MUTEX_NAME)
	{
		OGRE_LOCK_MUTEX(*r.OGRE_AUTO_MUTEX_NAME)
			OGRE_COPY_AUTO_SHARED_MUTEX(r.OGRE_AUTO_MUTEX_NAME)
			pRep = static_cast<FvFDBStorage*>(r.getPointer());
		pUseCount = r.useCountPointer();
		if (pUseCount)
		{
			++(*pUseCount);
		}
	}
}

FvFDBStoragePtr& FvFDBStoragePtr::operator=(const Ogre::ResourcePtr& r)
{
	if (pRep == static_cast<FvFDBStorage*>(r.getPointer()))
		return *this;
	release();
	OGRE_MUTEX_CONDITIONAL(r.OGRE_AUTO_MUTEX_NAME)
	{
		OGRE_LOCK_MUTEX(*r.OGRE_AUTO_MUTEX_NAME)
			OGRE_COPY_AUTO_SHARED_MUTEX(r.OGRE_AUTO_MUTEX_NAME)
			pRep = static_cast<FvFDBStorage*>(r.getPointer());
		pUseCount = r.useCountPointer();
		if (pUseCount)
		{
			++(*pUseCount);
		}
	}
	else
	{
		assert(r.isNull() && "RHS must be null if it has no mutex!");
		setNull();
	}
	return *this;
}

void FvFDBStoragePtr::destroy(void)
{
	Ogre::SharedPtr<FvFDBStorage>::destroy();
}