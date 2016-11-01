#include "FvEntityDescriptionMap.h"
#include "FvEntityDefConstants.h"
#include <OgreResourceGroupManager.h>
#include <OgreLogManager.h>
#include <OgreArchiveManager.h>
#include <OgreFileSystem.h>
#include <FvMD5.h>
#include <vld.h>

//void main(int argc, char* argv[])
//{
//	for(int i=1; i<argc; ++i)
//	{
//		FILE *pkFile = fopen(argv[i],"rb");
//		Ogre::DataStreamPtr spDataStream(OGRE_NEW Ogre::FileHandleDataStream(pkFile));
//		FvXMLSectionPtr pXML = FvXMLSection::OpenSection(spDataStream);
//
//		FvString newName = "new_";
//		newName += argv[i];
//		pXML->Save(newName);
//	}
//}


//extern FvInt32 DATA_TYPES_TOKEN;
//FvInt32* pDATA_TYPES_TOKEN1 = &DATA_TYPES_TOKEN;

void main()
{
	Ogre::LogManager *pkLog = OGRE_NEW Ogre::LogManager;
	Ogre::ArchiveManager *pkArchive = OGRE_NEW Ogre::ArchiveManager;
	pkArchive->addArchiveFactory(OGRE_NEW Ogre::FileSystemArchiveFactory);
	Ogre::ResourceGroupManager *pkGroupManager = OGRE_NEW Ogre::ResourceGroupManager;
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("./","FileSystem");

	bool bRet(false);
	FILE *pkFile(NULL);
	Ogre::DataStreamPtr spDataStream;
	FvXMLSectionPtr spSection;
	FvMemoryOStream stream;
	FvMD5 md5;
	FvMD5::Digest digest;

	//! 测试Entity
	if(1)
	{

		//! 解析entity配置文件
		pkFile = fopen(FvEntityDef::Constants::EntitiesFile(),"rb");
		spDataStream = Ogre::DataStreamPtr(OGRE_NEW Ogre::FileHandleDataStream(pkFile));
		spSection = FvXMLSection::OpenSection(spDataStream);
		FvEntityDescriptionMap entityDescMap;
		bRet = entityDescMap.Parse(spSection, NULL);
		FV_ASSERT(bRet);

		const FvEntityDescription& entityDes = entityDescMap.EntityDescription(0);
		FvEntityAttributes attrib(entityDes);
		for(FvUInt32 i=0; i<entityDes.PropertyCount(); ++i)
		{
			attrib.SetAttribut(i, entityDes.Property(i)->pInitialValue()->Copy());
		}

		FvObjPtr spObj = attrib.GetAttribut("NEW_ARRAY1");
		FVOBJ_CONVERT_P(spObj, FvObjArray, pArray);
		pArray->Erase(4);
		FvObjPtr spItem = pArray->At(2);
		pArray->Erase(spItem);

		FvObjPtr spSt = attrib.GetAttribut("NEW_STRUCT");
		FvObjPtr spSt1 = new FvObjStruct(spSt->DataType(), false);

/**
		//! 创建entity属性
		const FvEntityDescription& entityDes = entityDescMap.EntityDescription(0);
		FvEntityAttributes attrib(entityDes);
		for(FvUInt32 i=0; i<entityDes.PropertyCount(); ++i)
		{
			attrib.SetAttribut(i, entityDes.Property(i)->pInitialValue()->Copy());
		}
		spSection = FvXMLSection::CreateSection("root");
		attrib.Dump(spSection);
		spSection->Save("entitydump.xml");

		//! entity>>stream
		bRet = entityDes.AddDictionaryToStream(&attrib, stream, FvEntityDescription::FV_CELL_DATA);
		FV_ASSERT(bRet);

		//! stream>>section
		spSection = FvXMLSection::CreateSection("root");
		bRet = entityDes.ReadStreamToSection(stream, FvEntityDescription::FV_CELL_DATA, spSection);
		FV_ASSERT(bRet);
		FV_ASSERT(stream.RemainingLength() == 0);
		spSection->Save("entity_section.xml");

		//! section>>entity
		pkFile = fopen("entity_section.xml","rb");
		spDataStream = Ogre::DataStreamPtr(OGRE_NEW Ogre::FileHandleDataStream(pkFile));
		spSection = FvXMLSection::OpenSection(spDataStream);
		FvEntityAttributes attrib1(entityDes);
		bRet = entityDes.AddSectionToDictionary(spSection, &attrib1, FvEntityDescription::FV_CELL_DATA);
		FV_ASSERT(bRet);
		spSection = FvXMLSection::CreateSection("root");
		attrib1.Dump(spSection);
		spSection->Save("entity1dump.xml");

		//! section>>stream
		pkFile = fopen("entity_section.xml","rb");
		spDataStream = Ogre::DataStreamPtr(OGRE_NEW Ogre::FileHandleDataStream(pkFile));
		spSection = FvXMLSection::OpenSection(spDataStream);
		bRet = entityDes.AddSectionToStream(spSection, stream, FvEntityDescription::FV_CELL_DATA);
		FV_ASSERT(bRet);

		//! stream>>entity
		FvEntityAttributes attrib2(entityDes);
		bRet = entityDes.ReadStreamToDict(stream, FvEntityDescription::FV_CELL_DATA, &attrib2);
		FV_ASSERT(bRet);
		FV_ASSERT(stream.RemainingLength() == 0);
		spSection = FvXMLSection::CreateSection("root");
		attrib2.Dump(spSection);
		spSection->Save("entity2dump.xml");

		//! MD5
		entityDescMap.AddToMD5(md5);
		md5.GetDigest(digest);
		FV_INFO_MSG( "\tMD5 Digest:%s\n", digest.Quote().c_str());
		entityDescMap.AddPersistentPropertiesToMD5(md5);
		md5.GetDigest(digest);
		FV_INFO_MSG( "\tPersistent MD5 Digest:%s\n", digest.Quote().c_str());

		//! 属性同步
		if(1)
		{
			FvEntityAttributes attrib3(entityDes);
			FvEntityAttributes attrib4(entityDes);

			pkFile = fopen("entity_section.xml","rb");
			spDataStream = Ogre::DataStreamPtr(OGRE_NEW Ogre::FileHandleDataStream(pkFile));
			spSection = FvXMLSection::OpenSection(spDataStream);
			bRet = entityDes.AddSectionToDictionary(spSection, &attrib3, FvEntityDescription::FV_CELL_DATA);
			FV_ASSERT(bRet);
			bRet = entityDes.AddSectionToDictionary(spSection, &attrib4, FvEntityDescription::FV_CELL_DATA);
			FV_ASSERT(bRet);

			//! 修改属性
			FvObjPtr pAttrib, pChild, pChild1;
			FvObjVector3* pVector3(NULL);
			FvObjArray* pArray(NULL);
			FvObjIntMap* pIntMap(NULL);
			FvObjStruct* pStruct(NULL);
			FvUInt16 size(0);
			//! 修改第一层属性
			pAttrib = attrib3.GetAttribut("BASE_INT8");
			FVOBJ_SETVAL(pAttrib, FvObjInt8, 7654321);
			pAttrib = attrib3.GetAttribut("RAW_UINT64");
			FVOBJ_SETVAL(pAttrib, FvObjUInt64, 7654321);
			pAttrib = attrib3.GetAttribut("RAW_STRING");
			FVOBJ_SETVAL(pAttrib, FvObjString, "test!!!");
			pAttrib = attrib3.GetAttribut("RAW_VECTOR3");
			pVector3 = FVOBJ_CONVERT(pAttrib, FvObjVector3);
			pVector3->operator =(FvVector3(3, 2, 1));
			pAttrib = attrib3.GetAttribut("NEW_ARRAY");
			pArray = FVOBJ_CONVERT(pAttrib, FvObjArray);
			size = pArray->Size();
			pChild = (*pArray)[1];
			FVOBJ_SETVAL(pChild, FvObjInt32, 66);
			pChild = new FvObjInt32(pArray->GetElementDataType(), 100);
			pArray->PushBack(pChild);
			pChild = new FvObjInt32(pArray->GetElementDataType(), 101);
			pArray->Insert(1, pChild);
			pArray->Erase((*pArray)[2]);
			pArray->Erase(3);
			pAttrib = attrib3.GetAttribut("NEW_ARRAY1");
			pArray = FVOBJ_CONVERT(pAttrib, FvObjArray);
			pArray->Clear();
			pAttrib = attrib3.GetAttribut("NEW_INTMAP");
			pIntMap = FVOBJ_CONVERT(pAttrib, FvObjIntMap);
			size = pIntMap->Size();
			FvInt32 key;
			pIntMap->Get(0, key, pChild);
			pChild = pIntMap->Find(2);
			pIntMap->Insert(7, new FvObjVector3(pIntMap->GetElementDataType(), FvVector3(7,7,7)));
			pIntMap->Erase(0);
			pIntMap->Erase(2);
			pIntMap->Clear();
			pAttrib = attrib3.GetAttribut("NEW_STRUCT");
			pStruct = FVOBJ_CONVERT(pAttrib, FvObjStruct);
			pChild = pStruct->At(0);
			pChild = pStruct->AtName("itemMap");
			//! 修改内层属性
			pAttrib = attrib3.GetAttribut("STRUCT2");
			pStruct = FVOBJ_CONVERT(pAttrib, FvObjStruct);
			pChild = pStruct->AtName("itemArray");
			pArray = FVOBJ_CONVERT(pChild, FvObjArray);
			pChild = (*pArray)[1];
			FVOBJ_SETVAL(pChild, FvObjString, "tt");
			pChild = new FvObjString(pArray->GetElementDataType(), "aa");
			pArray->PushBack(pChild);
			pChild = new FvObjString(pArray->GetElementDataType(), "bb");
			pArray->Insert(1, pChild);
			pArray->Erase((*pArray)[2]);
			pArray->Erase(3);
			pChild = pStruct->AtName("itemStruct");
			pStruct = FVOBJ_CONVERT(pChild, FvObjStruct);
			FvObjStructPtr pStructPtr = new FvObjStruct(*pStruct);
			pChild = pStruct->AtName("itemFloat");
			FVOBJ_SETVAL(pChild, FvObjFloat, 3456);
			pChild = pStruct->AtName("itemArray");
			pArray = FVOBJ_CONVERT(pChild, FvObjArray);
			pArray->Clear();

			//! 同步属性
			FvEntityAttributes::MessageIDs& messageIDs = attrib3.GetMessageIDs();
			FvMemoryOStream& stream = attrib3.GetStream();
			int slen = stream.RemainingLength();
			for(int i=0; i<(int)messageIDs.size(); ++i)
			{
				attrib4.OnPropertyEvent(messageIDs[i], stream);
			}
			FV_ASSERT(stream.RemainingLength() == 0);

			if(1)
			{
				FvMemoryOStream stream;

				//! entity>>stream
				bRet = entityDes.AddDictionaryToStream(&attrib3, stream, FvEntityDescription::FV_CELL_DATA);
				FV_ASSERT(bRet);

				//! stream>>section
				spSection = FvXMLSection::CreateSection("root");
				bRet = entityDes.ReadStreamToSection(stream, FvEntityDescription::FV_CELL_DATA, spSection);
				FV_ASSERT(bRet);
				FV_ASSERT(stream.RemainingLength() == 0);
				spSection->Save("./a/entity_section.xml");
			}
			if(1)
			{
				FvMemoryOStream stream;

				//! entity>>stream
				bRet = entityDes.AddDictionaryToStream(&attrib4, stream, FvEntityDescription::FV_CELL_DATA);
				FV_ASSERT(bRet);

				//! stream>>section
				spSection = FvXMLSection::CreateSection("root");
				bRet = entityDes.ReadStreamToSection(stream, FvEntityDescription::FV_CELL_DATA, spSection);
				FV_ASSERT(bRet);
				FV_ASSERT(stream.RemainingLength() == 0);
				spSection->Save("./b/entity_section.xml");
			}
		}

		spSection = FvXMLSection::CreateSection("root");
		FvDataType::DumpAliases(spSection);
		spSection->Save("aliasdump.xml");

		spSection = FvXMLSection::CreateSection("root");
		FvDataType::DumpDataType(spSection);
		spSection->Save("typesdump.xml");

		spSection = FvXMLSection::CreateSection("root");
		entityDescMap.Dump(spSection);
		spSection->Save("entitisdump.xml");
**/
	}


	//! 测试user data



	FvDataType::ClearStaticsForReload();
	FvMetaDataType::Fini();
	FvDebugMsgHelper::Fini();
	OGRE_DELETE pkGroupManager;
	OGRE_DELETE pkArchive;
	OGRE_DELETE pkLog;
}

