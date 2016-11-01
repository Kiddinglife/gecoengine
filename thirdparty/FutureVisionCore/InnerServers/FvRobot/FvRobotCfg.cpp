#include "FvRobotCfg.h"
#include <OgreResourceGroupManager.h>
#include <OgreLogManager.h>
#include <OgreArchiveManager.h>
#include <OgreFileSystem.h>



FV_SINGLETON_STORAGE( FvConfigReader )

FvConfigReader::FvConfigReader()
{

}

FvConfigReader::~FvConfigReader()
{

}

FvConfigReader::Result& FvConfigReader::ReadXML(const char* pcFileName)
{
	m_kResult.clear();

	if(!pcFileName || !pcFileName[0])
	{
		FV_ERROR_MSG("%s, FileName is NULL\n", __FUNCTION__);
		return m_kResult;
	}

	Ogre::DataStreamPtr spDataStream = Ogre::ResourceGroupManager::getSingleton().openResource(pcFileName);
	if(spDataStream.isNull())
	{
		FV_ERROR_MSG("%s, Open File:%s Failed\n", __FUNCTION__, pcFileName);
		return m_kResult;
	}

	FvXMLSectionPtr spXML = FvXMLSection::OpenSection(spDataStream);
	if(spXML == NULL)
	{
		FV_ERROR_MSG("%s, Open File:%s Failed\n", __FUNCTION__, pcFileName);
		return m_kResult;
	}

	for(int i=0; i<(int)m_kItems.size(); ++i)
	{
		Item& kItem = m_kItems[i];
		if(!CanChange(kItem.iIndex))
			continue;

		FvXMLSectionPtr spSec = spXML->OpenSection(kItem.kName);
		if(!spSec)
			continue;

		bool bSuccess(true);

#define _CASE_ITEM(_TYPE, _CLASS, _CHECK)								\
		case _TYPE:														\
			{															\
				_CLASS& bOldValue = *static_cast<_CLASS*>(kItem.pValue);\
				_CLASS bNewValue = spSec->As(bOldValue);				\
				if(!kItem._CHECK(bOldValue, bNewValue))					\
					bSuccess = false;									\
			}

		switch(kItem.eType)
		{
		_CASE_ITEM(IT_BOOL, bool, CheckBool)
			break;
		_CASE_ITEM(IT_INT, int, CheckNum)
			break;
		_CASE_ITEM(IT_FLOAT, float, CheckNum)
			break;
		_CASE_ITEM(IT_STRING, FvString, CheckStr)
			break;
		_CASE_ITEM(IT_VECTOR2, FvVector2, CheckVector)
			break;
		_CASE_ITEM(IT_VECTOR3, FvVector3, CheckVector)
			break;
		_CASE_ITEM(IT_VECTOR4, FvVector4, CheckVector)
			break;
		case IT_USER:
			{
				bSuccess = HandleChange(kItem.iIndex, spSec);
			}
			break;
		}

		if(bSuccess)
			m_kResult.push_back(kItem.iIndex);
	}

	return m_kResult;

#undef _CASE_ITEM
}

FvConfigReader::Result& FvConfigReader::ReadCmd(int iArgc, char* pcArgv[])
{
	m_kResult.clear();

	if(iArgc < 2)
		return m_kResult;

	if(m_kItems.empty())
		return m_kResult;

	std::vector<bool> kExists(m_kItems.size(), false);

	for(int i=1; i<iArgc; ++i)
	{
		int idx = i;
		for(int j=0; j<(int)m_kItems.size(); ++j)
		{
			Item& kItem = m_kItems[j];
			if(_stricmp(pcArgv[idx], kItem.kNameInCmd.c_str()) == 0)
			{
				if(!CanRepeat(kItem.iIndex))
				{
					if(!kExists[j])
						kExists[j] = true;
					else
						break;
				}

				if(!CanChange(kItem.iIndex))
					break;

				bool bSuccess(false);
				switch(kItem.eType)
				{
				case IT_BOOL:
					{
						++idx;
						if(idx < iArgc)
						{
							bool& bOldValue = *static_cast<bool*>(kItem.pValue);
							bool bNewValue = _stricmp(pcArgv[idx], "true")==0 ? true : false;
							if(kItem.CheckBool(bOldValue, bNewValue))
								bSuccess = true;
						}
					}
					break;
				case IT_INT:
					{
						++idx;
						if(idx < iArgc)
						{
							int& iOldValue = *static_cast<int*>(kItem.pValue);
							int iNewValue = atoi(pcArgv[idx]);
							if(kItem.CheckNum(iOldValue, iNewValue))
								bSuccess = true;
						}
					}
					break;
				case IT_FLOAT:
					{
						++idx;
						if(idx < iArgc)
						{
							float& fOldValue = *static_cast<float*>(kItem.pValue);
							float fNewValue = atof(pcArgv[idx]);
							if(kItem.CheckNum(fOldValue, fNewValue))
								bSuccess = true;
						}
					}
					break;
				case IT_STRING:
					{
						++idx;
						if(idx < iArgc)
						{
							FvString& kOldValue = *static_cast<FvString*>(kItem.pValue);
							FvString kNewValue = pcArgv[idx];
							if(kItem.CheckStr(kOldValue, kNewValue))
								bSuccess = true;
						}
					}
					break;
				case IT_VECTOR2:
					{
						if(idx+2 < iArgc)
						{
							FvVector2& kOldValue = *static_cast<FvVector2*>(kItem.pValue);
							FvVector2 kNewValue;
							++idx;
							kNewValue.x = atof(pcArgv[idx]);
							++idx;
							kNewValue.y = atof(pcArgv[idx]);
							if(kItem.CheckVector(kOldValue, kNewValue))
								bSuccess = true;
						}
					}
					break;
				case IT_VECTOR3:
					{
						if(idx+3 < iArgc)
						{
							FvVector3& kOldValue = *static_cast<FvVector3*>(kItem.pValue);
							FvVector3 kNewValue;
							++idx;
							kNewValue.x = atof(pcArgv[idx]);
							++idx;
							kNewValue.y = atof(pcArgv[idx]);
							++idx;
							kNewValue.z = atof(pcArgv[idx]);
							if(kItem.CheckVector(kOldValue, kNewValue))
								bSuccess = true;
						}
					}
					break;
				case IT_VECTOR4:
					{
						if(idx+4 < iArgc)
						{
							FvVector4& kOldValue = *static_cast<FvVector4*>(kItem.pValue);
							FvVector4 kNewValue;
							++idx;
							kNewValue.x = atof(pcArgv[idx]);
							++idx;
							kNewValue.y = atof(pcArgv[idx]);
							++idx;
							kNewValue.z = atof(pcArgv[idx]);
							++idx;
							kNewValue.w = atof(pcArgv[idx]);
							if(kItem.CheckVector(kOldValue, kNewValue))
								bSuccess = true;
						}
					}
					break;
				case IT_USER:
					{
						++idx;
						bSuccess = HandleChange(kItem.iIndex, iArgc-idx, pcArgv+idx);
					}
					break;
				}

				if(bSuccess)
					m_kResult.push_back(kItem.iIndex);

				break;
			}
		}
	}

	return m_kResult;
}

FvConfigReader::Result& FvConfigReader::ReadCmd(const char* pcCmd)
{
	m_kResult.clear();

	if(!pcCmd || !pcCmd[0])
		return m_kResult;

	int iArgc(0);
	std::vector<char*> kArgv;

	++iArgc;
	kArgv.push_back("APP");

	bool bInWord(false);
	FvString kString = pcCmd;
	for(int i=0; i<(int)kString.length(); ++i)
	{
		if(kString[i]==' ' || kString[i]=='\t' || kString[i]=='\n' || kString[i]=='\0')
		{
			if(bInWord)
				bInWord = false;
			kString[i] = '\0';
		}
		else
		{
			if(!bInWord)
			{
				bInWord = true;
				++iArgc;
				kArgv.push_back(&kString[i]);
			}
		}
	}

	//! 检查字符串,去掉首尾的"
	for(int i=1; i<iArgc; ++i)
	{
		if(kArgv[i] && kArgv[i][0]=='\"')
			++kArgv[i];
		int iLen = (int)strlen(kArgv[i]);
		if(iLen > 0 && kArgv[i][iLen-1]=='\"')
			kArgv[i][iLen-1] = 0;
	}

	return ReadCmd(iArgc, &kArgv.front());
}

FvConfigReader::Item* FvConfigReader::AddItem(int iIndex, const char* pcName, bool& bValue)
{
	if(!pcName || !pcName[0])
		return NULL;
	m_kItems.push_back(Item(iIndex, pcName, IT_BOOL, &bValue, NULL));
	m_kItems.back().pkCfg = this;
	return &m_kItems.back();
}

FvConfigReader::Item* FvConfigReader::AddItem(int iIndex, const char* pcName, int& iValue)
{
	if(!pcName || !pcName[0])
		return NULL;
	m_kItems.push_back(Item(iIndex, pcName, IT_INT, &iValue, NULL));
	m_kItems.back().pkCfg = this;
	return &m_kItems.back();
}

FvConfigReader::Item* FvConfigReader::AddItem(int iIndex, const char* pcName, float& fValue)
{
	if(!pcName || !pcName[0])
		return NULL;
	m_kItems.push_back(Item(iIndex, pcName, IT_FLOAT, &fValue, NULL));
	m_kItems.back().pkCfg = this;
	return &m_kItems.back();
}

FvConfigReader::Item* FvConfigReader::AddItem(int iIndex, const char* pcName, FvString& kValue)
{
	if(!pcName || !pcName[0])
		return NULL;
	m_kItems.push_back(Item(iIndex, pcName, IT_STRING, &kValue, NULL));
	m_kItems.back().pkCfg = this;
	return &m_kItems.back();
}

FvConfigReader::Item* FvConfigReader::AddItem(int iIndex, const char* pcName, FvVector2& kValue)
{
	if(!pcName || !pcName[0])
		return NULL;
	m_kItems.push_back(Item(iIndex, pcName, IT_VECTOR2, &kValue, NULL));
	m_kItems.back().pkCfg = this;
	return &m_kItems.back();
}

FvConfigReader::Item* FvConfigReader::AddItem(int iIndex, const char* pcName, FvVector3& kValue)
{
	if(!pcName || !pcName[0])
		return NULL;
	m_kItems.push_back(Item(iIndex, pcName, IT_VECTOR3, &kValue, NULL));
	m_kItems.back().pkCfg = this;
	return &m_kItems.back();
}

FvConfigReader::Item* FvConfigReader::AddItem(int iIndex, const char* pcName, FvVector4& kValue)
{
	if(!pcName || !pcName[0])
		return NULL;
	m_kItems.push_back(Item(iIndex, pcName, IT_VECTOR4, &kValue, NULL));
	m_kItems.back().pkCfg = this;
	return &m_kItems.back();
}

FvConfigReader::Item* FvConfigReader::AddItem(int iIndex, const char* pcName, const char* pcCmdStr)
{
	if(!pcName || !pcName[0])
		return NULL;
	m_kItems.push_back(Item(iIndex, pcName, IT_USER, NULL, pcCmdStr));
	m_kItems.back().pkCfg = this;
	return &m_kItems.back();
}

void FvConfigReader::Item::AddCheckForNum(DataCheck eFlag, float fVal)
{
	if(eFlag == DC_GREATER ||
		eFlag == DC_GREATER_EQUAL)
	{
		iDataCheck |= eFlag;
		fVals[1] = fVal;
	}
	else if(eFlag == DC_LESS ||
		eFlag == DC_LESS_EQUAL)
	{
		iDataCheck |= eFlag;
		fVals[0] = fVal;
	}
}

FvString FvConfigReader::Item::skCmdStr;

const FvString& FvConfigReader::Item::GetCmdStr() const
{
	skCmdStr = kNameInCmd;

	if(eType == IT_USER)
	{
		skCmdStr = skCmdStr + " " + pcCmdArgs;
	}
	else
	{
		skCmdStr = skCmdStr + " ";

		switch(eType)
		{
		case IT_BOOL:
			skCmdStr = skCmdStr + "true|false";
			break;
		case IT_INT:
			skCmdStr = skCmdStr + "%%d";
			break;
		case IT_FLOAT:
			skCmdStr = skCmdStr + "%%f";
			break;
		case IT_STRING:
			skCmdStr = skCmdStr + "%%s";
			break;
		case IT_VECTOR2:
			skCmdStr = skCmdStr + "%%f %%f";
			break;
		case IT_VECTOR3:
			skCmdStr = skCmdStr + "%%f %%f %%f";
			break;
		case IT_VECTOR4:
			skCmdStr = skCmdStr + "%%f %%f %%f %%f";
			break;
		}
	}

	return skCmdStr;
}

const FvString& FvConfigReader::Item::GetValueStr() const
{
	if(eType == IT_USER)
	{
		pkCfg->GetValueStr(iIndex, skCmdStr);
		skCmdStr = kName + " " + skCmdStr;
	}
	else
	{
		skCmdStr = kName + " ";

		char values[128] = {0};
		switch(eType)
		{
		case IT_BOOL:
			{
				bool& kValue = *static_cast<bool*>(pValue);
				sprintf_s(values, sizeof(values), "%s", kValue ? "true" : "false");
			}
			break;
		case IT_INT:
			{
				int& kValue = *static_cast<int*>(pValue);
				sprintf_s(values, sizeof(values), "%d", kValue);
			}
			break;
		case IT_FLOAT:
			{
				float& kValue = *static_cast<float*>(pValue);
				sprintf_s(values, sizeof(values), "%f", kValue);
			}
			break;
		case IT_STRING:
			{
				FvString& kValue = *static_cast<FvString*>(pValue);
				sprintf_s(values, sizeof(values), "%s", kValue.c_str());
			}
			break;
		case IT_VECTOR2:
			{
				FvVector2& kValue = *static_cast<FvVector2*>(pValue);
				sprintf_s(values, sizeof(values), "[%f %f]", kValue.x, kValue.y);
			}
			break;
		case IT_VECTOR3:
			{
				FvVector3& kValue = *static_cast<FvVector3*>(pValue);
				sprintf_s(values, sizeof(values), "[%f %f %f]", kValue.x, kValue.y, kValue.z);
			}
			break;
		case IT_VECTOR4:
			{
				FvVector4& kValue = *static_cast<FvVector4*>(pValue);
				sprintf_s(values, sizeof(values), "[%f %f %f %f]", kValue.x, kValue.y, kValue.z, kValue.w);
			}
			break;
		}

		skCmdStr = skCmdStr + values;
	}

	return skCmdStr;
}


#define _CHECK_REPEAT(_OLD, _NEW)														\
	if(HasFlag(DC_REPEAT) && _OLD==_NEW)												\
	{																					\
		return false;																	\
	}

#define _CHECK_NUM(_VAL)																\
	if(HasFlag(DC_GREATER) && _VAL <= fVals[1])											\
	{																					\
		FV_ERROR_MSG("%s %s %f <= %f\n", __FUNCTION__, kName.c_str(), _VAL, fVals[1]);	\
		return false;																	\
	}																					\
	if(HasFlag(DC_GREATER_EQUAL) && _VAL < fVals[1])									\
	{																					\
		FV_ERROR_MSG("%s %s %f < %f\n", __FUNCTION__, kName.c_str(), _VAL, fVals[1]);	\
		return false;																	\
	}																					\
	if(HasFlag(DC_LESS) && _VAL >= fVals[0])											\
	{																					\
		FV_ERROR_MSG("%s %s %f >= %f\n", __FUNCTION__, kName.c_str(), _VAL, fVals[0]);	\
		return false;																	\
	}																					\
	if(HasFlag(DC_LESS_EQUAL) && _VAL > fVals[0])										\
	{																					\
		FV_ERROR_MSG("%s %s %f > %f\n", __FUNCTION__, kName.c_str(), _VAL, fVals[0]);	\
		return false;																	\
	}

bool FvConfigReader::Item::CheckBool(bool& bOld, const bool& bNew)
{
	_CHECK_REPEAT(bOld, bNew);

	bOld = bNew;
	return true;
}

bool FvConfigReader::Item::CheckNum(int& iOld, const int& iNew)
{
	_CHECK_REPEAT(iOld, iNew);
	_CHECK_NUM(iNew);

	iOld = iNew;
	return true;
}

bool FvConfigReader::Item::CheckNum(float& fOld, const float& fNew)
{
	_CHECK_REPEAT(fOld, fNew);
	_CHECK_NUM(fNew);

	fOld = fNew;
	return true;
}

bool FvConfigReader::Item::CheckStr(FvString& kOld, const FvString& kNew)
{
	_CHECK_REPEAT(kOld, kNew);

	if(HasFlag(DC_NON_NULL) && kNew == "")
	{
		FV_ERROR_MSG("%s %s is NULL\n", __FUNCTION__, kName.c_str());
		return false;
	}

	kOld = kNew;
	return true;
}

bool FvConfigReader::Item::CheckVector(FvVector2& kOld, const FvVector2& kNew)
{
	_CHECK_REPEAT(kOld, kNew);
	_CHECK_NUM(kNew.x);
	_CHECK_NUM(kNew.y);

	if(HasFlag(DC_ORDER) && kNew.x >= kNew.y)
	{
		FV_ERROR_MSG("%s %s [%f,%f] Order Err\n", __FUNCTION__, kName.c_str(), kNew.x, kNew.y);
		return false;
	}

	if(HasFlag(DC_PARTIAL_ORDER) && kNew.x > kNew.y)
	{
		FV_ERROR_MSG("%s %s [%f,%f] Partial Order Err\n", __FUNCTION__, kName.c_str(), kNew.x, kNew.y);
		return false;
	}

	kOld = kNew;
	return true;
}

bool FvConfigReader::Item::CheckVector(FvVector3& kOld, const FvVector3& kNew)
{
	_CHECK_REPEAT(kOld, kNew);
	_CHECK_NUM(kNew.x);
	_CHECK_NUM(kNew.y);
	_CHECK_NUM(kNew.z);

	if(HasFlag(DC_ORDER) && (kNew.x >= kNew.y || kNew.x >= kNew.z || kNew.y >= kNew.z))
	{
		FV_ERROR_MSG("%s %s [%f,%f,%f] Order Err\n", __FUNCTION__, kName.c_str(), kNew.x, kNew.y, kNew.z);
		return false;
	}

	if(HasFlag(DC_PARTIAL_ORDER) && (kNew.x > kNew.y || kNew.x > kNew.z || kNew.y > kNew.z))
	{
		FV_ERROR_MSG("%s %s [%f,%f,%f] Partial Order Err\n", __FUNCTION__, kName.c_str(), kNew.x, kNew.y, kNew.z);
		return false;
	}

	kOld = kNew;
	return true;
}

bool FvConfigReader::Item::CheckVector(FvVector4& kOld, const FvVector4& kNew)
{
	_CHECK_REPEAT(kOld, kNew);
	_CHECK_NUM(kNew.x);
	_CHECK_NUM(kNew.y);
	_CHECK_NUM(kNew.z);
	_CHECK_NUM(kNew.w);

	if(HasFlag(DC_RECT) && (kNew.x >= kNew.y || kNew.z >= kNew.w))
	{
		FV_ERROR_MSG("%s %s [%f,%f,%f,%f] Rect Err\n", __FUNCTION__, kName.c_str(), kNew.x, kNew.y, kNew.z, kNew.w);
		return false;
	}

	if(HasFlag(DC_ORDER) && (kNew.x >= kNew.y || kNew.x >= kNew.z || kNew.x >= kNew.w
								|| kNew.y >= kNew.z || kNew.y >= kNew.w
								|| kNew.z >= kNew.w))
	{
		FV_ERROR_MSG("%s %s [%f,%f,%f,%f] Order Err\n", __FUNCTION__, kName.c_str(), kNew.x, kNew.y, kNew.z, kNew.w);
		return false;
	}

	if(HasFlag(DC_PARTIAL_ORDER) && (kNew.x > kNew.y || kNew.x > kNew.z || kNew.x > kNew.w
								|| kNew.y > kNew.z || kNew.y > kNew.w
								|| kNew.z > kNew.w))
	{
		FV_ERROR_MSG("%s %s [%f,%f,%f,%f] Partial Order Err\n", __FUNCTION__, kName.c_str(), kNew.x, kNew.y, kNew.z, kNew.w);
		return false;
	}

	kOld = kNew;
	return true;
}



FvRobotCfg::FvRobotCfg()
:m_bInInit(false)
,m_bCmdLineHasCtrl(false)
,m_iCreateTick(1)
,m_iCreatePerTick(1)
,m_kTag4UpdateMove()
,m_kTag4AddRobots(),m_kTag4DelRobots()
,m_iAddRobots(0),m_iDelRobots(0)
,m_iSingleID(-1)
{
	InitData();

	FvConfigReader::Item* pkItem(NULL);
#define _ADDITEM_WITHVAL(_IDX)		pkItem = AddItem(_IDX, #_IDX, m_k##_IDX)
#define _ADDITEM(_IDX, _CMDARGS)	pkItem = AddItem(_IDX, #_IDX, _CMDARGS)

	_ADDITEM_WITHVAL(SingleMode);
	pkItem->AddCheck(FvConfigReader::DC_REPEAT);
	_ADDITEM_WITHVAL(WindowSizeWidth);
	pkItem->AddCheckForNum(FvConfigReader::DC_GREATER, 0);
	_ADDITEM_WITHVAL(WindowSizeHeight);
	pkItem->AddCheckForNum(FvConfigReader::DC_GREATER, 0);
	_ADDITEM_WITHVAL(WindowFrameFreq);
	pkItem->AddCheck(FvConfigReader::DC_REPEAT);
	pkItem->AddCheckForNum(FvConfigReader::DC_GREATER, 0);
	pkItem->AddCheckForNum(FvConfigReader::DC_LESS_EQUAL, 30);
	_ADDITEM_WITHVAL(WindowTimerFreq);
	pkItem->AddCheck(FvConfigReader::DC_REPEAT);
	pkItem->AddCheckForNum(FvConfigReader::DC_GREATER, 0);
	pkItem->AddCheckForNum(FvConfigReader::DC_LESS_EQUAL, 100);
	_ADDITEM_WITHVAL(ShouldWriteToConsole);
	pkItem->AddCheck(FvConfigReader::DC_REPEAT);
	_ADDITEM_WITHVAL(ShouldWriteToLog);
	pkItem->AddCheck(FvConfigReader::DC_REPEAT);
	_ADDITEM_WITHVAL(TickFreq);
	pkItem->AddCheck(FvConfigReader::DC_REPEAT);
	pkItem->AddCheckForNum(FvConfigReader::DC_GREATER, 0);
	pkItem->AddCheckForNum(FvConfigReader::DC_LESS_EQUAL, 30);
	_ADDITEM_WITHVAL(TickFragments);
	pkItem->AddCheck(FvConfigReader::DC_REPEAT);
	pkItem->AddCheckForNum(FvConfigReader::DC_GREATER, 0);
	pkItem->AddCheckForNum(FvConfigReader::DC_LESS_EQUAL, 30);
	_ADDITEM_WITHVAL(ServerName);
	pkItem->AddCheck(FvConfigReader::DC_REPEAT);
	pkItem->AddCheck(FvConfigReader::DC_NON_NULL);
	_ADDITEM_WITHVAL(AutoReConnect);
	pkItem->AddCheck(FvConfigReader::DC_REPEAT);
	_ADDITEM_WITHVAL(PacketLossRatio);
	pkItem->AddCheck(FvConfigReader::DC_REPEAT);
	pkItem->AddCheckForNum(FvConfigReader::DC_GREATER_EQUAL, 0);
	pkItem->AddCheckForNum(FvConfigReader::DC_LESS_EQUAL, 1);
	_ADDITEM_WITHVAL(NetworkLatency);
	pkItem->AddCheck(FvConfigReader::DC_REPEAT);
	pkItem->AddCheckForNum(FvConfigReader::DC_GREATER_EQUAL, 0);
	pkItem->AddCheck(FvConfigReader::DC_PARTIAL_ORDER);
	_ADDITEM_WITHVAL(UsernamePrefix);
	pkItem->AddCheck(FvConfigReader::DC_REPEAT);
	_ADDITEM_WITHVAL(Password);
	pkItem->AddCheck(FvConfigReader::DC_REPEAT);
	_ADDITEM_WITHVAL(PublicKey);
	pkItem->AddCheck(FvConfigReader::DC_REPEAT);
	_ADDITEM_WITHVAL(Tag);
	pkItem->AddCheck(FvConfigReader::DC_REPEAT);
	pkItem->AddCheck(FvConfigReader::DC_NON_NULL);
	_ADDITEM_WITHVAL(StartID);
	pkItem->AddCheck(FvConfigReader::DC_REPEAT);
	_ADDITEM_WITHVAL(Count);
	pkItem->AddCheck(FvConfigReader::DC_REPEAT);
	pkItem->AddCheckForNum(FvConfigReader::DC_GREATER_EQUAL, 0);
	pkItem->AddCheck(FvConfigReader::DC_PARTIAL_ORDER);
	_ADDITEM_WITHVAL(CreatePerSec);
	pkItem->AddCheck(FvConfigReader::DC_REPEAT);
	pkItem->AddCheckForNum(FvConfigReader::DC_GREATER, 0);
	_ADDITEM_WITHVAL(BirthRect);
	pkItem->AddCheck(FvConfigReader::DC_REPEAT);
	pkItem->AddCheck(FvConfigReader::DC_RECT);
	_ADDITEM_WITHVAL(MoveType);
	pkItem->AddCheck(FvConfigReader::DC_REPEAT);
	pkItem->AddCheckForNum(FvConfigReader::DC_GREATER_EQUAL, 0);
	pkItem->AddCheckForNum(FvConfigReader::DC_LESS_EQUAL, 2);
	_ADDITEM_WITHVAL(MoveSpeed);
	pkItem->AddCheck(FvConfigReader::DC_REPEAT);
	pkItem->AddCheckForNum(FvConfigReader::DC_GREATER, 0);
	_ADDITEM_WITHVAL(CtrllerType);
	pkItem->AddCheck(FvConfigReader::DC_REPEAT);
	pkItem->AddCheck(FvConfigReader::DC_NON_NULL);
	_ADDITEM_WITHVAL(CtrllerData);
	pkItem->AddCheck(FvConfigReader::DC_REPEAT);
	pkItem->AddCheck(FvConfigReader::DC_NON_NULL);
	_ADDITEM(UpdateMove, "Tag");
	_ADDITEM(AddBots, "Tag Count");
	_ADDITEM(DelBots, "Tag Count");
	_ADDITEM(SingleID, "ID");
	_ADDITEM(PrintMemory, "");

#undef _ADDITEM_WITHVAL
#undef _ADDITEM
}

FvRobotCfg::~FvRobotCfg()
{

}

bool FvRobotCfg::Init(int iArgc, char* pcArgv[])
{
	m_bInInit = true;

	ReadXML("Robot/Robots.xml");
	ReadCmd(iArgc, pcArgv);

	CalcValue();

	m_bInInit = false;
	return true;
}

FvRobotCfg::Result& FvRobotCfg::ParseCmd(const char* pcCmd)
{
	m_bCmdLineHasCtrl = false;
	FvRobotCfg::Result& kResult = ReadCmd(pcCmd);
	CalcValue();
	return kResult;
}

bool FvRobotCfg::CanChange(int iIndex)
{
	if(m_bInInit)
	{
		switch(iIndex)
		{
		case UpdateMove:
		case AddBots:
		case DelBots:
		case SingleID:
		case PrintMemory:
			return false;
		}
	}
	else
	{
		switch(iIndex)
		{
		case ServerName:
		case UsernamePrefix:
		case Password:
		case PublicKey:
		case StartID:
		case Count:
		case MoveType:
			return false;
		}
	}

	return true;
}

bool FvRobotCfg::CanRepeat(int iIndex)
{
	if(m_bInInit)
	{
/**
		switch(iIndex)
		{
		case Controllers:
			return true;
		}
**/
	}
	else
	{
	}

	return false;
}

bool FvRobotCfg::HandleChange(int iIndex, FvXMLSectionPtr spSection)
{
	FV_ASSERT(spSection);
/**
	if(iIndex == Controllers)
	{
		std::vector<FvXMLSectionPtr> kItems;
		FvXMLSectionPtr spSec, spSec1, spSec2;
		spSection->OpenSections("Item", kItems);
		if(!kItems.empty())
		{
			std::vector<FvXMLSectionPtr>::iterator itrB = kItems.begin();
			std::vector<FvXMLSectionPtr>::iterator itrE = kItems.end();
			while(itrB != itrE)
			{
				spSec = (*itrB)->OpenSection("Name");
				spSec1 = (*itrB)->OpenSection("Type");
				spSec2 = (*itrB)->OpenSection("Data");
				if(spSec && spSec1 && spSec2)
				{
					FvString kName = spSec->AsString();
					FvString kType = spSec1->AsString();
					FvString kData = spSec2->AsString();
					if(!ReadController(kName, kType, kData))
						return false;
				}
				else
				{
					if(!spSec)
					{
						FV_ERROR_MSG("Controllers Name is NULL\n");
					}
					if(!spSec1)
					{
						FV_ERROR_MSG("Controllers Type is NULL\n");
					}
					if(!spSec2)
					{
						FV_ERROR_MSG("Controllers Data is NULL\n");
					}
					return false;
				}

				++itrB;
			}

			return true;
		}
	}
**/
	return false;
}

bool FvRobotCfg::HandleChange(int iIndex, int iArgc, char* pcArgv[])
{
	if(m_bInInit)
	{
/**
		if(iIndex == Controllers)
		{
			if(iArgc < 3)
				return false;

			if(!m_bCmdLineHasCtrl)
			{
				m_bCmdLineHasCtrl = true;
				m_kCtrlInfos.clear();
			}

			FvString kName = pcArgv[0];
			FvString kType = pcArgv[1];
			FvString kData = pcArgv[2];
			if(!ReadController(kName, kType, kData))
				return false;

			return true;
		}
**/
	}
	else
	{
		if(iIndex == UpdateMove)
		{
			if(m_kMoveType != 2)//! 只有Auto模式下才可以使用该命令
				return false;

			if(iArgc < 1)
				return false;

			m_kTag4UpdateMove = pcArgv[0];

			return true;
		}
		else if(iIndex == AddBots)
		{
			if(iArgc < 2)
				return false;

			m_kTag4AddRobots = pcArgv[0];
			m_iAddRobots = atoi(pcArgv[1]);

			if(m_iAddRobots <= 0)
				return false;

			return true;
		}
		else if(iIndex == DelBots)
		{
			if(iArgc < 2)
				return false;

			m_kTag4DelRobots = pcArgv[0];
			m_iDelRobots = atoi(pcArgv[1]);

			if(m_iDelRobots < 0)
				return false;

			return true;
		}
		else if(iIndex == SingleID)
		{
			if(iArgc < 1)
				return false;

			int iSingleID = atoi(pcArgv[0]);
			if(m_iSingleID == iSingleID)
				return false;

			if(iSingleID < m_kStartID || m_kStartID+m_kCount.y <= iSingleID)
				return false;

			m_iSingleID = iSingleID;
			return true;
		}
		else if(iIndex == PrintMemory)
		{
			return true;
		}
	}

	return false;
}

void FvRobotCfg::GetValueStr(int iIndex, FvString& kValue)
{
	char tmp[64] = {0};

	if(iIndex == SingleID)
	{
		sprintf_s(tmp, sizeof(tmp), "%d", m_iSingleID);
	}

	kValue = tmp;
}

void FvRobotCfg::InitData()
{
#define _SET_VAR(NAME_, VAL_)	m_k##NAME_ = VAL_

	_SET_VAR(SingleMode, false);
	_SET_VAR(WindowSizeWidth, 64);
	_SET_VAR(WindowSizeHeight, 72);
	_SET_VAR(WindowFrameFreq, 24);
	_SET_VAR(WindowTimerFreq, 100);
	_SET_VAR(ShouldWriteToConsole, true);
	_SET_VAR(ShouldWriteToLog, true);
	_SET_VAR(TickFreq, 30);
	_SET_VAR(TickFragments, 1);
	_SET_VAR(ServerName, "127.0.0.1");
	_SET_VAR(AutoReConnect, true);
	_SET_VAR(PacketLossRatio, 0.0f);
	_SET_VAR(NetworkLatency, FvVector2(0, 0));
	_SET_VAR(UsernamePrefix, "Robot");
	_SET_VAR(Password, "");
	_SET_VAR(PublicKey, "");
	_SET_VAR(Tag, "default");
	_SET_VAR(StartID, 0);
	_SET_VAR(Count, FvVector2(100, 100));
	_SET_VAR(CreatePerSec, 1);
	_SET_VAR(BirthRect, FvVector4(0, 100, 0, 100));
	_SET_VAR(MoveType, 2);
	_SET_VAR(MoveSpeed, 5.0f);
	_SET_VAR(CtrllerType, "Patrol");
	_SET_VAR(CtrllerData, "test.fvp");

#undef _SET_VAR
}
/**
bool FvRobotCfg::ReadController(FvString& kName, FvString& kType, FvString& kData)
{
	if(kName.empty())
	{
		FV_ERROR_MSG("Controllers Name is NULL\n");
		return false;
	}
	if(kType.empty())
	{
		FV_ERROR_MSG("Controllers Type is NULL\n");
		return false;
	}
	if(kData.empty())
	{
		FV_ERROR_MSG("Controllers Data is NULL\n");
		return false;
	}

	Ogre::DataStreamPtr spDataStream = Ogre::ResourceGroupManager::getSingleton().openResource(kData);
	if(spDataStream.isNull())
	{
		FV_ERROR_MSG("Controllers Can't Open Data(%s)\n", kData.c_str());
		return false;
	}

	FvXMLSectionPtr spSection = FvXMLSection::OpenSection(spDataStream);
	if(spSection == NULL)
	{
		FV_ERROR_MSG("Controllers Can't Open Data(%s)\n", kData.c_str());
		return false;
	}

	m_kCtrlInfos.push_back(CtrlInfo(kName, kType, spSection));
	return true;
}
**/
void FvRobotCfg::CalcValue()
{
	float fAll = m_kTickFreq*m_kTickFragments;
	if(m_kCreatePerSec < fAll)
	{
		m_iCreateTick = (int)ceilf(fAll / m_kCreatePerSec);
		m_iCreatePerTick = 1;
	}
	else
	{
		m_iCreateTick = 1;
		m_iCreatePerTick = m_kCreatePerSec / fAll;
	}
}

bool FvRobotCfg::GetSingleMode() const
{
	return m_kSingleMode;
}

int FvRobotCfg::GetWindowSizeWidth() const
{
	return m_kWindowSizeWidth;
}

int FvRobotCfg::GetWindowSizeHeight() const
{
	return m_kWindowSizeHeight;
}

int FvRobotCfg::GetWindowFramePeriod() const
{
	return 1000/m_kWindowFrameFreq;
}

int FvRobotCfg::GetWindowTimerPeriod() const
{
	return 1000/m_kWindowTimerFreq;
}

bool FvRobotCfg::GetShouldWriteToConsole() const
{
	return m_kShouldWriteToConsole;
}

bool FvRobotCfg::GetShouldWriteToLog() const
{
	return m_kShouldWriteToLog;
}

float FvRobotCfg::GetTickFreq() const
{
	return m_kTickFreq;
}

int FvRobotCfg::GetTickFragments() const
{
	return m_kTickFragments;
}

const FvString& FvRobotCfg::GetServerName() const
{
	return m_kServerName;
}

bool FvRobotCfg::GetAutoReConnect() const
{
	return m_kAutoReConnect;
}

float FvRobotCfg::GetPacketLossRatio() const
{
	return m_kPacketLossRatio;
}

float FvRobotCfg::GetMinNetworkLatency() const
{
	return m_kNetworkLatency.x;
}

float FvRobotCfg::GetMaxNetworkLatency() const
{
	return m_kNetworkLatency.y;
}

const FvString& FvRobotCfg::GetUsernamePrefix() const
{
	return m_kUsernamePrefix;
}

const FvString& FvRobotCfg::GetPassword() const
{
	return m_kPassword;
}

const FvString& FvRobotCfg::GetPublicKey() const
{
	return m_kPublicKey;
}

const FvString& FvRobotCfg::GetTag() const
{
	return m_kTag;
}

int FvRobotCfg::GetStartID() const
{
	return m_kStartID;
}

int FvRobotCfg::GetCount() const
{
	return m_kCount.x;
}

int FvRobotCfg::GetMaxCount() const
{
	return m_kCount.y;
}

int FvRobotCfg::GetCreateTick() const
{
	return m_iCreateTick;
}

int FvRobotCfg::GetCreatePerTick() const
{
	return m_iCreatePerTick;
}

FvVector4& FvRobotCfg::GetBirthRect()
{
	return m_kBirthRect;
}

int FvRobotCfg::GetMoveType() const
{
	return m_kMoveType;
}

int FvRobotCfg::GetMoveSpeed() const
{
	return m_kMoveSpeed;
}

const FvString& FvRobotCfg::GetCtrllerType() const
{
	return m_kCtrllerType;
}

const FvString& FvRobotCfg::GetCtrllerData() const
{
	return m_kCtrllerData;
}

const FvString& FvRobotCfg::GetTag4UpdateMove() const
{
	return m_kTag4UpdateMove;
}

const FvString&	FvRobotCfg::GetTag4AddRobots() const
{
	return m_kTag4AddRobots;
}

int FvRobotCfg::GetAddRobots() const
{
	return m_iAddRobots;
}

const FvString&	FvRobotCfg::GetTag4DelRobots() const
{
	return m_kTag4DelRobots;
}

int FvRobotCfg::GetDelRobots() const
{
	return m_iDelRobots;
}

void FvRobotCfg::SetSingleMode(bool bVal)
{
	m_kSingleMode = bVal;
}