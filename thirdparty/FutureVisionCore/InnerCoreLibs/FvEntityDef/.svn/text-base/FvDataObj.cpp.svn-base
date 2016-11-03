#include "FvDataObj.h"
#include "FvEntityDefUtility.h"
#include "FvEntityData.h"
#include "FvEntityExport.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FvDataObj::FvDataObj()
:FvDataOwner()
{

}

FvDataObj::~FvDataObj()
{

}

#ifdef FV_SERVER
FvDataOwner* FvDataObj::GetRootDataForLeaf(OpCode uiOpCode)
{
	if(m_pkOwner)
		return m_pkOwner->GetRootData(uiOpCode, this);
	else
		return NULL;
}
#endif


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FvObjBool FvObjBool::ms_kOldData;

#ifdef FV_SERVER
FvObjBool& FvObjBool::operator=(const FvObjBool& val)
{
	if(this == &val || m_kVal == val.m_kVal)
		return *this;
	m_kVal=val.m_kVal;
	FvDataOwner* pkRoot(NULL);
	if(pkRoot = GetRootDataForLeaf(OPCD_MOD))
	{
		ms_kDataStream << m_kVal;
		pkRoot->NotifyDataChanged();
	}
	return *this;
}
#endif
void FvObjBool::DataRenovate(FvDataCallBack* pkCB, OpCode uiOpCode, FvBinaryIStream& kIS)
{
	FV_ASSERT(uiOpCode == OPCD_MOD);
	if(pkCB)
	{
		ms_kOldData.m_kVal = m_kVal;
		kIS >> m_kVal;
		pkCB->Handle(OPCD_MOD, &ms_kOldData);
	}
	else
	{
		kIS >> m_kVal;
	}
	FV_ASSERT(!kIS.Error());
}

bool FvObjBool::SerializeToStream(FvBinaryOStream& kOS) const
{
	kOS << m_kVal;
	return true;
}

bool FvObjBool::DeserializeFromStream(FvBinaryIStream& kIS)
{
	kIS >> m_kVal;
	FV_ASSERT(!kIS.Error());
	return true;
}

bool FvObjBool::SerializeToSection(FvXMLSectionPtr spSection) const
{
	spSection->SetBool(m_kVal);
	return true;
}

bool FvObjBool::DeserializeFromSection(FvXMLSectionPtr spSection)
{
	m_kVal = spSection->AsBool(m_kVal);
	return true;
}
#ifdef FV_SERVER
void FvObjBool::EqualTo(const FvObjBool& kRight)
{
	m_kVal = kRight.m_kVal;
}
#endif
template<>
FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const FvObjBool& kVal)
{
	return kOS << kVal.m_kVal;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FvObjString FvObjString::ms_kOldData;

#ifdef FV_SERVER
FvObjString& FvObjString::operator=(const FvObjString& val)
{
	if(this == &val || m_kVal == val.m_kVal)
		return *this;
	m_kVal=val.m_kVal;
	NofityDataChg();
	return *this;
}
FvObjString& FvObjString::operator+=(const FvObjString& val)
{
	if(val.m_kVal == "")
		return *this;
	m_kVal+=val.m_kVal;
	NofityDataChg();
	return *this;
}
FvObjString& FvObjString::append(const FvObjString& val)
{
	if(val.m_kVal == "")
		return *this;
	m_kVal+=val.m_kVal;
	NofityDataChg();
	return *this;
}
FvObjString& FvObjString::assign(const FvObjString& val)
{
	if(this == &val || m_kVal == val.m_kVal)
		return *this;
	m_kVal=val.m_kVal;
	NofityDataChg();
	return *this;
}
void FvObjString::resize(FvUInt32 uiSize)
{
	if(size() == uiSize)
		return;
	m_kVal.resize(uiSize);
	NofityDataChg();
	return;
}
void FvObjString::resize(FvUInt32 uiSize, char c)
{
	if(size() == uiSize)
		return;
	m_kVal.resize(uiSize, c);
	NofityDataChg();
	return;
}
void FvObjString::clear()
{
	if(empty())
		return;
	m_kVal.clear();
	FvDataOwner* pkRoot(NULL);
	if(pkRoot = GetRootDataForLeaf(OPCD_CLR))
	{
		pkRoot->NotifyDataChanged();
	}
}
void clear();
#endif
void FvObjString::DataRenovate(FvDataCallBack* pkCB, OpCode uiOpCode, FvBinaryIStream& kIS)
{
	FV_ASSERT(uiOpCode == OPCD_MOD || uiOpCode == OPCD_CLR);
	if(pkCB)
	{
		ms_kOldData.m_kVal = m_kVal;
		if(uiOpCode == OPCD_MOD)
			kIS >> m_kVal;
		else
			m_kVal.clear();
		pkCB->Handle(OPCD_MOD, &ms_kOldData);
	}
	else
	{
		if(uiOpCode == OPCD_MOD)
			kIS >> m_kVal;
		else
			m_kVal.clear();
	}
	FV_ASSERT(!kIS.Error());
}

bool FvObjString::SerializeToStream(FvBinaryOStream& kOS) const
{
	kOS << m_kVal;
	return true;
}

bool FvObjString::DeserializeFromStream(FvBinaryIStream& kIS)
{
	kIS >> m_kVal;
	FV_ASSERT(!kIS.Error());
	return true;
}

bool FvObjString::SerializeToSection(FvXMLSectionPtr spSection) const
{
	spSection->SetString(m_kVal);
	return true;
}

bool FvObjString::DeserializeFromSection(FvXMLSectionPtr spSection)
{
	m_kVal = spSection->AsString(m_kVal);
	return true;
}
#ifdef FV_SERVER
void FvObjString::EqualTo(const FvObjString& kRight)
{
	m_kVal = kRight.m_kVal;
}
void FvObjString::NofityDataChg()
{
	FvDataOwner* pkRoot(NULL);
	if(pkRoot = GetRootDataForLeaf(OPCD_MOD))
	{
		ms_kDataStream << m_kVal;
		pkRoot->NotifyDataChanged();
	}
}
#endif
template<>
FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const FvObjString& kVal)
{
	return kOS << kVal.m_kVal;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FvObjVector2 FvObjVector2::ms_kOldData;

#ifdef FV_SERVER
FvObjVector2& FvObjVector2::operator=(const FvObjVector2& val)
{
	if(this == &val || m_kVal == val.m_kVal)
		return *this;
	m_kVal=val.m_kVal;
	NofityDataChg();
	return *this;
}
FvObjVector2& FvObjVector2::operator+=(const FvObjVector2& kVector)
{
	if(kVector.m_kVal == FvVector2::ZERO)
		return *this;
	m_kVal+=kVector.m_kVal;
	NofityDataChg();
	return *this;
}
FvObjVector2& FvObjVector2::operator-=(const FvObjVector2& kVector)
{
	if(kVector.m_kVal == FvVector2::ZERO)
		return *this;
	m_kVal-=kVector.m_kVal;
	NofityDataChg();
	return *this;
}
FvObjVector2& FvObjVector2::operator*=(float fScale)
{
	if(fScale == 1.0f)
		return *this;
	m_kVal*=fScale;
	NofityDataChg();
	return *this;
}
FvObjVector2& FvObjVector2::operator/=(float fScale)
{
	if(fScale == 1.0f)
		return *this;
	m_kVal/=fScale;
	NofityDataChg();
	return *this;
}
void FvObjVector2::SetZero()
{
	if(m_kVal == FvVector2::ZERO)
		return;
	m_kVal.SetZero();
	NofityDataChg();
}
void FvObjVector2::Set(float fX, float fY)
{
	if(m_kVal.x==fX && m_kVal.y==fY)
		return;
	m_kVal.Set(fX, fY);
	NofityDataChg();
}
void FvObjVector2::Scale(const FvObjVector2& kVector, float fScale)
{
	FvVector2 kTmp = m_kVal;
	m_kVal.Scale(kVector.m_kVal, fScale);
	if(kTmp == m_kVal)
		return;
	NofityDataChg();
}
void FvObjVector2::Normalise()
{
	if(m_kVal.LengthSquared() == 1.0)
		return;
	m_kVal.Normalise();
	NofityDataChg();
}
void FvObjVector2::ParallelMultiply(const FvObjVector2& kVector)
{
	if(kVector.m_kVal == FvVector2(1, 1))
		return;
	m_kVal.ParallelMultiply(kVector.m_kVal);
	NofityDataChg();
}
void FvObjVector2::ParallelMultiply(const FvObjVector2& kVector0, const FvObjVector2& kVector1)
{
	FvVector2 kTmp = m_kVal;
	m_kVal.ParallelMultiply(kVector0.m_kVal, kVector1.m_kVal);
	if(kTmp == m_kVal)
		return;
	NofityDataChg();
}
void FvObjVector2::Lerp(const FvObjVector2& kVector0, const FvObjVector2& kVector1, float fAlpha)
{
	FvVector2 kTmp = m_kVal;
	m_kVal.Lerp(kVector0.m_kVal, kVector1.m_kVal, fAlpha);
	if(kTmp == m_kVal)
		return;
	NofityDataChg();
}
void FvObjVector2::Clamp(const FvObjVector2& kLower, const FvObjVector2& kUpper)
{
	FvVector2 kTmp = m_kVal;
	m_kVal.Clamp(kLower.m_kVal, kUpper.m_kVal);
	if(kTmp == m_kVal)
		return;
	NofityDataChg();
}
void FvObjVector2::ProjectOnto(const FvObjVector2& kVector0, const FvObjVector2& kVector1)
{
	FvVector2 kTmp = m_kVal;
	m_kVal.ProjectOnto(kVector0.m_kVal, kVector1.m_kVal);
	if(kTmp == m_kVal)
		return;
	NofityDataChg();
}
#endif
void FvObjVector2::DataRenovate(FvDataCallBack* pkCB, OpCode uiOpCode, FvBinaryIStream& kIS)
{
	FV_ASSERT(uiOpCode == OPCD_MOD);
	if(pkCB)
	{
		ms_kOldData.m_kVal = m_kVal;
		kIS >> m_kVal;
		pkCB->Handle(OPCD_MOD, &ms_kOldData);
	}
	else
	{
		kIS >> m_kVal;
	}
	FV_ASSERT(!kIS.Error());
}

bool FvObjVector2::SerializeToStream(FvBinaryOStream& kOS) const
{
	kOS << m_kVal;
	return true;
}

bool FvObjVector2::DeserializeFromStream(FvBinaryIStream& kIS)
{
	kIS >> m_kVal;
	FV_ASSERT(!kIS.Error());
	return true;
}

bool FvObjVector2::SerializeToSection(FvXMLSectionPtr spSection) const
{
	spSection->SetVector2(m_kVal);
	return true;
}

bool FvObjVector2::DeserializeFromSection(FvXMLSectionPtr spSection)
{
	m_kVal = spSection->AsVector2(m_kVal);
	return true;
}
#ifdef FV_SERVER
void FvObjVector2::EqualTo(const FvObjVector2& kRight)
{
	m_kVal = kRight.m_kVal;
}
void FvObjVector2::NofityDataChg()
{
	FvDataOwner* pkRoot(NULL);
	if(pkRoot = GetRootDataForLeaf(OPCD_MOD))
	{
		ms_kDataStream << m_kVal;
		pkRoot->NotifyDataChanged();
	}
}
#endif
template<>
FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const FvObjVector2& kVal)
{
	return kOS << kVal.m_kVal;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FvObjVector3 FvObjVector3::ms_kOldData;

#ifdef FV_SERVER
FvObjVector3& FvObjVector3::operator=(const FvObjVector3& val)
{
	if(this == &val || m_kVal == val.m_kVal)
		return *this;
	m_kVal=val.m_kVal;
	NofityDataChg();
	return *this;
}
FvObjVector3& FvObjVector3::operator+=(const FvObjVector3& kVector)
{
	if(kVector.m_kVal == FvVector3::ZERO)
		return *this;
	m_kVal+=kVector.m_kVal;
	NofityDataChg();
	return *this;
}
FvObjVector3& FvObjVector3::operator-=(const FvObjVector3& kVector)
{
	if(kVector.m_kVal == FvVector3::ZERO)
		return *this;
	m_kVal-=kVector.m_kVal;
	NofityDataChg();
	return *this;
}
FvObjVector3& FvObjVector3::operator*=(float fScale)
{
	if(fScale == 1.0f)
		return *this;
	m_kVal*=fScale;
	NofityDataChg();
	return *this;
}
FvObjVector3& FvObjVector3::operator/=(float fScale)
{
	if(fScale == 1.0f)
		return *this;
	m_kVal/=fScale;
	NofityDataChg();
	return *this;
}
void FvObjVector3::SetZero()
{
	if(m_kVal == FvVector3::ZERO)
		return;
	m_kVal.SetZero();
	NofityDataChg();
}
void FvObjVector3::Set(float fX, float fY, float fZ)
{
	FvVector3 kTmp = m_kVal;
	m_kVal.Set(fX, fY, fZ);
	if(m_kVal == kTmp)
		return;
	NofityDataChg();
}
void FvObjVector3::Scale(const FvObjVector3& kVector, float fScale)
{
	FvVector3 kTmp = m_kVal;
	m_kVal.Scale(kVector.m_kVal, fScale);
	if(m_kVal == kTmp)
		return;
	NofityDataChg();
}
void FvObjVector3::Normalise()
{
	if(m_kVal.LengthSquared() == 1.0f)
		return;
	m_kVal.Normalise();
	NofityDataChg();
}
void FvObjVector3::ParallelMultiply(const FvObjVector3& kVector)
{
	if(kVector.m_kVal == FvVector3(1, 1, 1))
		return;
	m_kVal.ParallelMultiply(kVector.m_kVal);
	NofityDataChg();
}
void FvObjVector3::ParallelMultiply(const FvObjVector3& kVector0, const FvObjVector3& kVector1)
{
	FvVector3 kTmp = m_kVal;
	m_kVal.ParallelMultiply(kVector0.m_kVal, kVector1.m_kVal);
	if(m_kVal == kTmp)
		return;
	NofityDataChg();
}
void FvObjVector3::Lerp(const FvObjVector3& kVector0, const FvObjVector3& kVector1, float fAlpha)
{
	FvVector3 kTmp = m_kVal;
	m_kVal.Lerp(kVector0.m_kVal, kVector1.m_kVal, fAlpha);
	if(m_kVal == kTmp)
		return;
	NofityDataChg();
}
void FvObjVector3::Clamp(const FvObjVector3& kLower, const FvObjVector3& kUpper)
{
	FvVector3 kTmp = m_kVal;
	m_kVal.Clamp(kLower.m_kVal, kUpper.m_kVal);
	if(m_kVal == kTmp)
		return;
	NofityDataChg();
}
void FvObjVector3::SetPitchYaw(float fPitchInRadians, float fYawInRadians)
{
	FvVector3 kTmp = m_kVal;
	m_kVal.SetPitchYaw(fPitchInRadians, fYawInRadians);
	if(m_kVal == kTmp)
		return;
	NofityDataChg();
}
void FvObjVector3::CrossProduct(const FvObjVector3& kVector0, const FvObjVector3& kVector1)
{
	FvVector3 kTmp = m_kVal;
	m_kVal.CrossProduct(kVector0.m_kVal, kVector1.m_kVal);
	if(m_kVal == kTmp)
		return;
	NofityDataChg();
}
void FvObjVector3::ProjectOnto(const FvObjVector3& kVector0, const FvObjVector3& kVector1)
{
	FvVector3 kTmp = m_kVal;
	m_kVal.ProjectOnto(kVector0.m_kVal, kVector1.m_kVal);
	if(m_kVal == kTmp)
		return;
	NofityDataChg();
}
#endif
void FvObjVector3::DataRenovate(FvDataCallBack* pkCB, OpCode uiOpCode, FvBinaryIStream& kIS)
{
	FV_ASSERT(uiOpCode == OPCD_MOD);
	if(pkCB)
	{
		ms_kOldData.m_kVal = m_kVal;
		kIS >> m_kVal;
		pkCB->Handle(OPCD_MOD, &ms_kOldData);
	}
	else
	{
		kIS >> m_kVal;
	}
	FV_ASSERT(!kIS.Error());
}

bool FvObjVector3::SerializeToStream(FvBinaryOStream& kOS) const
{
	kOS << m_kVal;
	return true;
}

bool FvObjVector3::DeserializeFromStream(FvBinaryIStream& kIS)
{
	kIS >> m_kVal;
	FV_ASSERT(!kIS.Error());
	return true;
}

bool FvObjVector3::SerializeToSection(FvXMLSectionPtr spSection) const
{
	spSection->SetVector3(m_kVal);
	return true;
}

bool FvObjVector3::DeserializeFromSection(FvXMLSectionPtr spSection)
{
	m_kVal = spSection->AsVector3(m_kVal);
	return true;
}
#ifdef FV_SERVER
void FvObjVector3::EqualTo(const FvObjVector3& kRight)
{
	m_kVal = kRight.m_kVal;
}
void FvObjVector3::NofityDataChg()
{
	FvDataOwner* pkRoot(NULL);
	if(pkRoot = GetRootDataForLeaf(OPCD_MOD))
	{
		ms_kDataStream << m_kVal;
		pkRoot->NotifyDataChanged();
	}
}
#endif
template<>
FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const FvObjVector3& kVal)
{
	return kOS << kVal.m_kVal;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FvObjVector4 FvObjVector4::ms_kOldData;

#ifdef FV_SERVER
FvObjVector4& FvObjVector4::operator=(const FvObjVector4& val)
{
	if(this == &val || m_kVal == val.m_kVal)
		return *this;
	m_kVal=val.m_kVal;
	NofityDataChg();
	return *this;
}
FvObjVector4& FvObjVector4::operator+=(const FvObjVector4& kVector)
{
	if(kVector.m_kVal == FvVector4::ZERO)
		return *this;
	m_kVal+=kVector.m_kVal;
	NofityDataChg();
	return *this;
}
FvObjVector4& FvObjVector4::operator-=(const FvObjVector4& kVector)
{
	if(kVector.m_kVal == FvVector4::ZERO)
		return *this;
	m_kVal-=kVector.m_kVal;
	NofityDataChg();
	return *this;
}
FvObjVector4& FvObjVector4::operator*=(float fScale)
{
	if(fScale == 1.0f)
		return *this;
	m_kVal*=fScale;
	NofityDataChg();
	return *this;
}
FvObjVector4& FvObjVector4::operator/=(float fScale)
{
	if(fScale == 1.0f)
		return *this;
	m_kVal/=fScale;
	NofityDataChg();
	return *this;
}
void FvObjVector4::SetZero()
{
	if(m_kVal == FvVector4::ZERO)
		return;
	m_kVal.SetZero();
	NofityDataChg();
}
void FvObjVector4::Set(float fX, float fY, float fZ, float fW)
{
	if(m_kVal == FvVector4(fX, fY, fZ, fW))
		return;
	m_kVal.Set(fX, fY, fZ, fW);
	NofityDataChg();
}
void FvObjVector4::Scale(const FvObjVector4& kVector, float fScale)
{
	FvVector4 kTmp = m_kVal;
	m_kVal.Scale(kVector.m_kVal, fScale);
	if(m_kVal == kTmp)
		return;
	NofityDataChg();
}
void FvObjVector4::Scale(float fScale)
{
	if(fScale == 1.0f)
		return;
	m_kVal.Scale(fScale);
	NofityDataChg();
}
void FvObjVector4::Normalise()
{
	if(m_kVal.LengthSquared() == 1.0f)
		return;
	m_kVal.Normalise();
	NofityDataChg();
}
void FvObjVector4::ParallelMultiply(const FvObjVector4& kVector)
{
	if(kVector.m_kVal == FvVector4(1, 1, 1, 1))
		return;
	m_kVal.ParallelMultiply(kVector.m_kVal);
	NofityDataChg();
}
void FvObjVector4::ParallelMultiply(const FvObjVector4& kVector0, const FvObjVector4& kVector1)
{
	FvVector4 kTmp = m_kVal;
	m_kVal.ParallelMultiply(kVector0.m_kVal, kVector1.m_kVal);
	if(m_kVal == kTmp)
		return;
	NofityDataChg();
}
void FvObjVector4::Lerp(const FvObjVector4& kVector0, const FvObjVector4& kVector1, float fAlpha)
{
	FvVector4 kTmp = m_kVal;
	m_kVal.Lerp(kVector0.m_kVal, kVector1.m_kVal, fAlpha);
	if(m_kVal == kTmp)
		return;
	NofityDataChg();
}
void FvObjVector4::Clamp(const FvObjVector4& kLower, const FvObjVector4& kUpper)
{
	FvVector4 kTmp = m_kVal;
	m_kVal.Clamp(kLower.m_kVal, kUpper.m_kVal);
	if(m_kVal == kTmp)
		return;
	NofityDataChg();
}
#endif
void FvObjVector4::DataRenovate(FvDataCallBack* pkCB, OpCode uiOpCode, FvBinaryIStream& kIS)
{
	FV_ASSERT(uiOpCode == OPCD_MOD);
	if(pkCB)
	{
		ms_kOldData.m_kVal = m_kVal;
		kIS >> m_kVal;
		pkCB->Handle(OPCD_MOD, &ms_kOldData);
	}
	else
	{
		kIS >> m_kVal;
	}
	FV_ASSERT(!kIS.Error());
}

bool FvObjVector4::SerializeToStream(FvBinaryOStream& kOS) const
{
	kOS << m_kVal;
	return true;
}

bool FvObjVector4::DeserializeFromStream(FvBinaryIStream& kIS)
{
	kIS >> m_kVal;
	FV_ASSERT(!kIS.Error());
	return true;
}

bool FvObjVector4::SerializeToSection(FvXMLSectionPtr spSection) const
{
	spSection->SetVector4(m_kVal);
	return true;
}

bool FvObjVector4::DeserializeFromSection(FvXMLSectionPtr spSection)
{
	m_kVal = spSection->AsVector4(m_kVal);
	return true;
}
#ifdef FV_SERVER
void FvObjVector4::EqualTo(const FvObjVector4& kRight)
{
	m_kVal = kRight.m_kVal;
}
void FvObjVector4::NofityDataChg()
{
	FvDataOwner* pkRoot(NULL);
	if(pkRoot = GetRootDataForLeaf(OPCD_MOD))
	{
		ms_kDataStream << m_kVal;
		pkRoot->NotifyDataChanged();
	}
}
#endif
template<>
FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const FvObjVector4& kVal)
{
	return kOS << kVal.m_kVal;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FvObjBlob::FvObjBlob(int iSize)
:FvDataObj()
{
	FV_ASSERT(iSize > 0);
	m_pcBegin = new char[iSize];
	m_pcCurr = m_pcBegin;
	m_pcEnd = m_pcBegin + iSize;
	m_pcRead = m_pcBegin;
}

FvObjBlob::FvObjBlob(const FvString& kSrc)
:FvDataObj()
{
	int iSize = (int)kSrc.size();
	if(iSize == 0)
		iSize = DEFSIZE;
	m_pcBegin = new char[iSize];
	m_pcCurr = m_pcBegin;
	m_pcEnd = m_pcBegin + iSize;
	m_pcRead = m_pcBegin;

	AddBlob(kSrc.c_str(), kSrc.size());
}

FvObjBlob::FvObjBlob(const FvObjBlob& src)
:FvDataObj()
{
	int iSize = (int)(src.m_pcEnd - src.m_pcBegin);
	FV_ASSERT(iSize > 0);
	m_pcBegin = new char[iSize];
	memcpy(m_pcBegin, src.m_pcBegin, iSize);
	m_pcCurr = src.m_pcCurr - src.m_pcBegin + m_pcBegin;
	m_pcRead = src.m_pcRead - src.m_pcBegin + m_pcBegin;
	m_pcEnd = m_pcBegin + iSize;
}

FvObjBlob::~FvObjBlob()
{
	delete [] m_pcBegin;
}

void* FvObjBlob::Reserve(int iBytes)
{
	char* pOldCurr = m_pcCurr;
	m_pcCurr += iBytes;

	if(m_pcCurr > m_pcEnd)
	{
		int multiplier = (int)((m_pcCurr - m_pcBegin)/(m_pcEnd - m_pcBegin) + 1);
		int newSize = multiplier * (int)(m_pcEnd - m_pcBegin);
		char* pNewData = new char[newSize];
		memcpy(pNewData, m_pcBegin, pOldCurr - m_pcBegin);
		m_pcCurr = m_pcCurr - m_pcBegin + pNewData;
		pOldCurr = pOldCurr - m_pcBegin + pNewData;
		m_pcRead = m_pcRead - m_pcBegin + pNewData;
		delete [] m_pcBegin;
		m_pcBegin = pNewData;
		m_pcEnd = m_pcBegin + newSize;
	}

	return pOldCurr;
}

int FvObjBlob::Size() const
{
	return (int)(m_pcCurr - m_pcBegin);
}

const void*	FvObjBlob::Retrieve(int iBytes)
{
	char* pOldRead = m_pcRead;

	FV_IF_NOT_ASSERT_DEV(m_pcRead + iBytes <= m_pcCurr)
	{
		m_bError = true;

		return iBytes <= int(sizeof(ms_acErrBuf)) ? ms_acErrBuf : NULL;
	}

	m_pcRead += iBytes;

	return pOldRead;
}

int FvObjBlob::RemainingLength() const
{
	return (int)(m_pcCurr - m_pcRead);
}

char FvObjBlob::Peek()
{
	FV_IF_NOT_ASSERT_DEV(m_pcRead < m_pcCurr)
	{
		m_bError = true;
		return -1;
	}

	return *(char*)m_pcRead;
}

void* FvObjBlob::Data() const
{
	return m_pcBegin;
}

void FvObjBlob::Reset()
{
	m_pcRead = m_pcCurr = m_pcBegin;
}

void FvObjBlob::Rewind()
{
	m_pcRead = m_pcBegin;
}

FvObjBlob& FvObjBlob::operator=(const FvString& val)
{
	Reset();
	AddBlob(val.c_str(), val.size());
	return *this;
}

FvObjBlob& FvObjBlob::operator=(const FvObjBlob& val)
{
	int size(val.m_pcEnd-val.m_pcBegin);

	if(m_pcEnd-m_pcBegin < size)
	{
		delete [] m_pcBegin;

		m_pcBegin = new char[size];
		m_pcEnd = m_pcBegin + size;
	}

	m_pcCurr = m_pcBegin + (val.m_pcCurr-val.m_pcBegin);
	m_pcRead = m_pcBegin + (val.m_pcRead-val.m_pcBegin);

	memcpy(m_pcBegin, val.m_pcBegin, size);

	return *this;
}

#ifdef FV_SERVER
void FvObjBlob::ModifyNotify()
{
	FvDataOwner* pkRoot(NULL);
	if(pkRoot = GetRootDataForLeaf(OPCD_MOD))
	{
		ms_kDataStream << *this;
		pkRoot->NotifyDataChanged();
	}
}
#endif

void FvObjBlob::DataRenovate(FvDataCallBack* pkCB, OpCode uiOpCode, FvBinaryIStream& kIS)
{
	FV_ASSERT(uiOpCode == OPCD_MOD);
	kIS >> *this;
	if(pkCB)
		pkCB->Handle(OPCD_MOD, NULL);
	FV_ASSERT(!kIS.Error());
}

bool FvObjBlob::SerializeToStream(FvBinaryOStream& kOS) const
{
	kOS << *this;
	return true;
}

bool FvObjBlob::DeserializeFromStream(FvBinaryIStream& kIS)
{
	kIS >> *this;
	FV_ASSERT(!kIS.Error());
	return true;
}

bool FvObjBlob::SerializeToSection(FvXMLSectionPtr spSection) const
{
	spSection->SetBlob(FvString((char*)Data(), Size()));
	return true;
}

bool FvObjBlob::DeserializeFromSection(FvXMLSectionPtr spSection)
{
	FvString kVal = spSection->AsBlob();
	Reset();
	AddBlob(kVal.c_str(), kVal.size());
	return true;
}
#ifdef FV_SERVER
void FvObjBlob::EqualTo(const FvObjBlob& kRight)
{
	*this = kRight;
}
#endif
template<>
FvBinaryIStream& operator>>(FvBinaryIStream& kIS, FvObjBlob& kBlob)
{
	int	iLen = kIS.ReadPackedInt();
	FV_ASSERT(!kIS.Error());
	kBlob.Reset();
	kBlob.AddBlob(kIS.Retrieve(iLen), iLen);
	return kIS;
}
template<>
FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const FvObjBlob& kBlob)
{
	kOS.AppendString(static_cast<const char*>(kBlob.Data()), kBlob.Size());
	return kOS;
}
namespace FvDataPrint
{
	template<>
	void Print(const char* pcName, const FvObjBlob& kVal, FvString& kMsg)
	{
		FV_ASSERT(pcName);
		FvDataPrint::Print(kMsg, "%s[Blob]", pcName);
	}
	template<>
	void Print(const FvObjBlob& kVal, FvString& kMsg)
	{
		kMsg.append("BLOB");
	}
}
namespace FvXMLData
{
	template<>
	void Read(FvXMLSectionPtr spSection, FvObjBlob& kVal)
	{
#ifdef FV_SERVER
		FV_ASSERT(!kVal.HasOwner() && "The Blob has DataOwner!");//! 有Owner不能反流化
#endif
		kVal.DeserializeFromSection(spSection);
	}
	template<>
	void Write(FvXMLSectionPtr spSection, const FvObjBlob& kVal)
	{
		kVal.SerializeToSection(spSection);
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FvObjMailBox FvObjMailBox::ms_kOldData;

FvObjMailBox::FvObjMailBox()
:FvDataObj()
{
	m_kMBRef.Init();
}

FvObjMailBox::FvObjMailBox(const FvObjMailBox& src)
:FvDataObj()
{
	m_kMBRef.m_iID = src.m_kMBRef.m_iID;
	m_kMBRef.m_kAddr = src.m_kMBRef.m_kAddr;
}

bool FvObjMailBox::IsValid() const
{
	return !m_kMBRef.m_kAddr.IsNone() &&
		m_kMBRef.GetType() != FV_INVALID_ENTITY_TYPE_ID &&
		m_kMBRef.m_iID != FV_NULL_ENTITY;
}

#ifdef FV_SERVER
void FvObjMailBox::Clear()
{
	m_kMBRef.Init();
	MailBoxChangNotify();
}
#endif

bool FvObjMailBox::operator==(const FvObjMailBox& kMailBox) const
{
	return m_kMBRef == kMailBox.m_kMBRef;
}

bool FvObjMailBox::operator!=(const FvObjMailBox& kMailBox) const
{
	return m_kMBRef != kMailBox.m_kMBRef;
}

/**
#ifdef FV_SERVER
FvObjMailBox& FvObjMailBox::operator=(const FvObjMailBox& val)
{
	if(this == &val || this->operator ==(val))
		return *this;
	m_kMBRef = val.m_kMBRef;
	MailBoxChangNotify();
	return *this;
}

void FvObjMailBox::Set(FvEntityID iEntityID, FvEntityTypeID uiEntityType, const FvNetAddress& kAddr, FvEntityMailBoxRef::Component eComponent)
{
	FvEntityMailBoxRef t;
	t.Init(iEntityID, kAddr, eComponent, uiEntityType);
	if(m_kMBRef == t)
		return;
	m_kMBRef = t;
	MailBoxChangNotify();
}

void FvObjMailBox::Set(const FvEntityMailBoxRef& kMBRef)
{
	if(m_kMBRef == kMBRef)
		return;
	m_kMBRef = kMBRef;
	MailBoxChangNotify();
}

void FvObjMailBox::SetComponent(FvEntityMailBoxRef::Component eComponent)
{
	if(m_kMBRef.GetComponent() == eComponent)
		return;
	m_kMBRef.SetComponent(eComponent);
	MailBoxChangNotify();
}

void FvObjMailBox::SetType(FvEntityTypeID uiEntityType)
{
	if(m_kMBRef.GetType() == uiEntityType)
		return;
	m_kMBRef.SetType(uiEntityType);
	MailBoxChangNotify();
}

void FvObjMailBox::SetID(FvEntityID iEntityID)
{
	if(m_kMBRef.m_iID == iEntityID)
		return;
	m_kMBRef.m_iID = iEntityID;
	MailBoxChangNotify();
}

void FvObjMailBox::SetAddr(const FvNetAddress& kAddr)
{
	if(m_kMBRef.m_kAddr==kAddr &&
		m_kMBRef.m_kAddr.m_uiSalt==kAddr.m_uiSalt)
		return;
	m_kMBRef.m_kAddr = kAddr;
	MailBoxChangNotify();
}
#endif
**/

#ifdef FV_SERVER
void FvObjMailBox::MailBoxChangNotify()
{
	FvDataOwner* pkRoot(NULL);
	if(pkRoot = GetRootDataForLeaf(OPCD_MOD))
	{
		ms_kDataStream << m_kMBRef;
		pkRoot->NotifyDataChanged();
	}
}
#endif
FvEntityMailBoxRef::Component FvObjMailBox::GetComponent() const
{
	return m_kMBRef.GetComponent();
}

FvEntityTypeID FvObjMailBox::GetEntityType() const
{
	return m_kMBRef.GetType();
}

FvEntityID FvObjMailBox::GetEntityID() const
{
	return m_kMBRef.m_iID;
}

const FvNetAddress& FvObjMailBox::GetAddr() const
{
	return m_kMBRef.m_kAddr;
}

char FvObjMailBox::ms_acBuf[128];
const char* FvObjMailBox::c_str() const
{
	sprintf_s(ms_acBuf, sizeof(ms_acBuf), "%s, Id:%d, type:%d, Component:%d", m_kMBRef.m_kAddr.c_str(), m_kMBRef.m_iID, m_kMBRef.GetType(), m_kMBRef.GetComponent());
	return ms_acBuf;
}

void FvObjMailBox::DataRenovate(FvDataCallBack* pkCB, OpCode uiOpCode, FvBinaryIStream& kIS)
{
	FV_ASSERT(uiOpCode == OPCD_MOD);
	if(pkCB)
	{
		ms_kOldData.m_kMBRef = m_kMBRef;
		kIS >> m_kMBRef;
		pkCB->Handle(OPCD_MOD, &ms_kOldData);
	}
	else
	{
		kIS >> m_kMBRef;
	}
	FV_ASSERT(!kIS.Error());
}

bool FvObjMailBox::SerializeToStream(FvBinaryOStream& kOS) const
{
	kOS << m_kMBRef;
	return true;
}

bool FvObjMailBox::DeserializeFromStream(FvBinaryIStream& kIS)
{
	kIS >> m_kMBRef;
	FV_ASSERT(!kIS.Error());
	return true;
}

bool FvObjMailBox::SerializeToSection(FvXMLSectionPtr spSection) const
{
	char buf[64] = {0};
	//! ip,id,type,component
	sprintf_s(buf, sizeof(buf), "%s,%d,%d,%d", m_kMBRef.m_kAddr.c_str(), m_kMBRef.m_iID, m_kMBRef.GetType(), m_kMBRef.GetComponent());
	spSection->SetString(buf);
	return true;
}

bool FvObjMailBox::DeserializeFromSection(FvXMLSectionPtr spSection)
{
	FvString val = spSection->AsString();

	char ip[32] = {0};
	int a1, a2, a3;

	//! ip,id,type,component
	if(sscanf(val.c_str(), "%s,%d,%d,%d",
		ip, &a1, &a2, &a3) != 4)
		return false;

	FvNetAddress addr(ip);
	m_kMBRef.Init(a1, addr, (FvEntityMailBoxRef::Component)a3, a2);
	return true;
}
#ifdef FV_SERVER
void FvObjMailBox::EqualTo(const FvObjMailBox& kRight)
{
	m_kMBRef.m_iID = kRight.m_kMBRef.m_iID;
	m_kMBRef.m_kAddr = kRight.m_kMBRef.m_kAddr;
}
#endif
template<>
FvBinaryIStream& operator>>(FvBinaryIStream& kIS, FvObjMailBox& kMailBox)
{
#ifdef FV_SERVER
	FV_ASSERT(!kMailBox.HasOwner() && "The MailBox has DataOwner!");//! 有Owner不能反流化
#endif
	return kIS >> kMailBox.m_kMBRef;
}
template<>
FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const FvObjMailBox& kMailBox)
{
	return kOS << kMailBox.m_kMBRef;
}
namespace FvDataPrint
{
	template<>
	void Print(const char* pcName, const FvObjMailBox& kVal, FvString& kMsg)
	{
		FV_ASSERT(pcName);
		FvDataPrint::Print(kMsg, "%s[%s]", pcName, kVal.c_str());
	}
	template<>
	void Print(const FvObjMailBox& kVal, FvString& kMsg)
	{
		FvDataPrint::Print(kMsg, "%s", kVal.c_str());
	}
}
namespace FvXMLData
{
	template<>
	void Read(FvXMLSectionPtr spSection, FvObjMailBox& kVal)
	{
#ifdef FV_SERVER
		FV_ASSERT(!kVal.HasOwner() && "The MailBox has DataOwner!");//! 有Owner不能反流化
#endif
		kVal.DeserializeFromSection(spSection);
	}
	template<>
	void Write(FvXMLSectionPtr spSection, const FvObjMailBox& kVal)
	{
		kVal.SerializeToSection(spSection);
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FvObjUDO::FvUDOProxy::Map FvObjUDO::FvUDOProxy::ms_kUDOProxyMap;

FvObjUDO::FvUDOProxy::FvUDOProxy(const FvString& kGuid_)
:kGuid(kGuid_),pkAttrib(NULL)
{
	bool bRet = ms_kUDOProxyMap.insert(std::make_pair(kGuid, this)).second;
	FV_ASSERT(bRet);
}

FvObjUDO::FvUDOProxy::FvUDOProxy(const FvUniqueID& kGuid_)
:kGuid(kGuid_),pkAttrib(NULL)
{
	bool bRet = ms_kUDOProxyMap.insert(std::make_pair(kGuid, this)).second;
	FV_ASSERT(bRet);
}

FvObjUDO::FvUDOProxy::~FvUDOProxy()
{
	Map::iterator itr = ms_kUDOProxyMap.find(kGuid);
	FV_ASSERT(itr != ms_kUDOProxyMap.end());
	ms_kUDOProxyMap.erase(itr);
}

FvObjUDO::FvUDOProxy* FvObjUDO::FvUDOProxy::GetUDOProxy(const FvString& kGuid_)
{
	return GetUDOProxy(FvUniqueID(kGuid_));
}

FvObjUDO::FvUDOProxy* FvObjUDO::FvUDOProxy::GetUDOProxy(const FvUniqueID& kGuid_)
{
	Map::iterator itr = ms_kUDOProxyMap.find(kGuid_);
	if(itr != ms_kUDOProxyMap.end())
		return itr->second;
	return NULL;
}

FvObjUDO::FvUDOProxy* FvObjUDO::FvUDOProxy::CreateUDOProxy(const FvString& kGuid)
{
	return new FvUDOProxy(kGuid);
}

FvObjUDO::FvUDOProxy*	FvObjUDO::FvUDOProxy::CreateUDOProxy(const FvUniqueID& kGuid)
{
	return new FvUDOProxy(kGuid);
}


FvObjUDO FvObjUDO::ms_kOldData;
AllUDOExports* FvObjUDO::ms_pkAllUDOExports = NULL;

FvObjUDO::FvObjUDO()
:FvDataObj()
{

}

FvObjUDO::FvObjUDO(const FvObjUDO& src)
:FvDataObj(),m_spUDOProxy(src.m_spUDOProxy)
{

}

FvObjUDO::FvObjUDO(const FvString& kGuid)
:FvDataObj(),m_spUDOProxy(FvUDOProxy::GetUDOProxy(kGuid))
{

}

bool FvObjUDO::IsReady() const
{
	if(!m_spUDOProxy || !m_spUDOProxy->pkAttrib)
		return false;
	return true;
}

FvUInt16 FvObjUDO::GetUDOType() const
{
	FV_ASSERT(m_spUDOProxy && m_spUDOProxy->pkAttrib);
	return m_spUDOProxy->pkAttrib->GetUDOType();
}

FvUDOAttrib* FvObjUDO::GetUDOAttrib() const
{
	FV_ASSERT(m_spUDOProxy && m_spUDOProxy->pkAttrib);
	return m_spUDOProxy->pkAttrib;
}

bool FvObjUDO::operator==(const FvObjUDO& kUDO) const
{
	return m_spUDOProxy == kUDO.m_spUDOProxy;
}

bool FvObjUDO::operator!=(const FvObjUDO& kUDO) const
{
	return m_spUDOProxy != kUDO.m_spUDOProxy;
}
#ifdef FV_SERVER
FvObjUDO& FvObjUDO::operator=(const FvObjUDO& val)
{
	if(m_spUDOProxy == val.m_spUDOProxy)
		return *this;
	m_spUDOProxy = val.m_spUDOProxy;
	FvDataOwner* pkRoot(NULL);
	if(pkRoot = GetRootDataForLeaf(OPCD_MOD))
	{
		SerializeToStream(ms_kDataStream);
		pkRoot->NotifyDataChanged();
	}
	return *this;
}
FvObjUDO& FvObjUDO::operator=(const FvString& kGuid)
{
	if(m_spUDOProxy && m_spUDOProxy->kGuid == FvUniqueID(kGuid))
		return *this;
	m_spUDOProxy = FvUDOProxy::GetUDOProxy(kGuid);
	FvDataOwner* pkRoot(NULL);
	if(pkRoot = GetRootDataForLeaf(OPCD_MOD))
	{
		SerializeToStream(ms_kDataStream);
		pkRoot->NotifyDataChanged();
	}
	return *this;
}
void FvObjUDO::Clear()
{
	if(!m_spUDOProxy)
		return;
	m_spUDOProxy = NULL;
	FvDataOwner* pkRoot(NULL);
	if(pkRoot = GetRootDataForLeaf(OPCD_MOD))
	{
		SerializeToStream(ms_kDataStream);
		pkRoot->NotifyDataChanged();
	}
}
#endif
void FvObjUDO::DataRenovate(FvDataCallBack* pkCB, OpCode uiOpCode, FvBinaryIStream& kIS)
{
	FV_ASSERT(uiOpCode == OPCD_MOD);
	if(pkCB)
	{
		ms_kOldData.m_spUDOProxy = m_spUDOProxy;
		DeserializeFromStream(kIS);
		pkCB->Handle(OPCD_MOD, &ms_kOldData);
	}
	else
	{
		DeserializeFromStream(kIS);
	}
}

bool FvObjUDO::SerializeToStream(FvBinaryOStream& kOS) const
{
	if(m_spUDOProxy)kOS << m_spUDOProxy->kGuid;
	else			kOS << FvUniqueID::Zero();
	return true;
}

bool FvObjUDO::DeserializeFromStream(FvBinaryIStream& kIS)
{
	FvUniqueID kGuid;
	kIS >> kGuid;
	FV_ASSERT(!kIS.Error());

	if(kGuid == FvUniqueID::Zero())
	{
		m_spUDOProxy = NULL;
	}
	else
	{
		m_spUDOProxy = FvUDOProxy::GetUDOProxy(kGuid);
		if(!m_spUDOProxy)
			m_spUDOProxy = FvUDOProxy::CreateUDOProxy(kGuid);
	}
	return true;
}

bool FvObjUDO::SerializeToSection(FvXMLSectionPtr spSection) const
{
	if(m_spUDOProxy)spSection->WriteString("guid", m_spUDOProxy->kGuid);
	else			spSection->WriteString("guid", FvUniqueID::Zero());
	return true;
}

bool FvObjUDO::DeserializeFromSection(FvXMLSectionPtr spSection)
{
	FvString kGuid = spSection->ReadString("guid", "");
	if(kGuid == "")
	{
		m_spUDOProxy = NULL;
	}
	else
	{
		m_spUDOProxy = FvUDOProxy::GetUDOProxy(kGuid);
		if(!m_spUDOProxy)
			m_spUDOProxy = FvUDOProxy::CreateUDOProxy(kGuid);
	}
	return true;
}
#ifdef FV_SERVER
void FvObjUDO::EqualTo(const FvObjUDO& kRight)
{
	m_spUDOProxy = kRight.m_spUDOProxy;
}
#endif
void FvObjUDO::InitUDO(FvUInt16 uiType, FvUDOInitData& kInitData)
{
	FV_ASSERT(!m_spUDOProxy && ms_pkAllUDOExports && uiType<ms_pkAllUDOExports->uiCnt);
	m_spUDOProxy = FvUDOProxy::GetUDOProxy(kInitData.kGuid);
	if(!m_spUDOProxy)
		m_spUDOProxy = FvUDOProxy::CreateUDOProxy(kInitData.kGuid);
	m_spUDOProxy->pkAttrib = ms_pkAllUDOExports->pkUDOExport[uiType].pFunCreate();
	m_spUDOProxy->pkAttrib->m_kPosition = kInitData.kPosition;
	m_spUDOProxy->pkAttrib->m_kDirection = kInitData.kDirection;
	m_spUDOProxy->pkAttrib->DeserializeFromSection(kInitData.spPropertiesDS);
	//! 清空InitData
	kInitData.Clear();
}

void FvObjUDO::DestroyUDO()
{
	FV_ASSERT(m_spUDOProxy && m_spUDOProxy->pkAttrib);
	delete m_spUDOProxy->pkAttrib;//! 释放内存,解除UDO对其他UDO的引用
	m_spUDOProxy->pkAttrib = NULL;
	m_spUDOProxy = NULL;
}

FvUInt16 FvObjUDO::NameToType(const FvString& kGuid)
{
	FV_ASSERT(ms_pkAllUDOExports);
	return ms_pkAllUDOExports->NameToType(kGuid.c_str());
}

template<>
FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const FvObjUDO& kUDO)
{
	kUDO.SerializeToStream(kOS);
	return kOS;
}
template<>
FvBinaryIStream& operator>>(FvBinaryIStream& kIS, FvObjUDO& kUDO)
{
#ifdef FV_SERVER
	FV_ASSERT(!kUDO.HasOwner() && "The UDO has DataOwner!");//! 有Owner不能反流化
#endif
	kUDO.DeserializeFromStream(kIS);
	return kIS;
}
namespace FvDataPrint
{
	template<>
	void Print(const char* pcName, const FvObjUDO& kVal, FvString& kMsg)
	{
		FV_ASSERT(pcName);
		if(kVal.m_spUDOProxy)
			FvDataPrint::Print(kMsg, "%s[%s]", pcName, kVal.m_spUDOProxy->kGuid.ToString().c_str());
		else
			FvDataPrint::Print(kMsg, "%s[0]", pcName);
	}
	template<>
	void Print(const FvObjUDO& kVal, FvString& kMsg)
	{
		if(kVal.m_spUDOProxy)
			FvDataPrint::Print(kMsg, "UDO:%s", kVal.m_spUDOProxy->kGuid.ToString().c_str());
		else
			kMsg.append("UDO:0");
	}
}
namespace FvXMLData
{
	template<>
	void Read(FvXMLSectionPtr spSection, FvObjUDO& kVal)
	{
#ifdef FV_SERVER
		FV_ASSERT(!kVal.HasOwner() && "The UDO has DataOwner!");//! 有Owner不能反流化
#endif
		kVal.DeserializeFromSection(spSection);
	}
	template<>
	void Write(FvXMLSectionPtr spSection, const FvObjUDO& kVal)
	{
		kVal.SerializeToSection(spSection);
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FvObjStruct::FvObjStruct()
:FvDataObj()
{

}

FvUInt32 FvObjStruct::Size() const
{
	FvUInt32 uiCnt(0);
	NameAddr* pkInfo = NULL;
	GetStructAddr(uiCnt, pkInfo);
	return uiCnt;
}

FvDataObj* FvObjStruct::Get(FvUInt32 uiIdx) const
{
	FvUInt32 uiCnt(0);
	NameAddr* pkInfo = NULL;
	GetStructAddr(uiCnt, pkInfo);
	if(uiIdx < uiCnt)
		return (FvDataObj*)((char*)this + pkInfo[uiIdx].uiAddr);
	return NULL;
}

FvUInt32 FvObjStruct::Size(NameAddr*& pkInfo) const
{
	FvUInt32 uiCnt(0);
	GetStructAddr(uiCnt, pkInfo);
	return uiCnt;
}

FvDataObj* FvObjStruct::Get(NameAddr* pkInfo, FvUInt32 uiIdx) const
{
	FV_ASSERT(pkInfo);
	return (FvDataObj*)((char*)this + pkInfo[uiIdx].uiAddr);
}
#ifdef FV_SERVER
FvDataOwner* FvObjStruct::GetRootData(OpCode uiOpCode, FvDataOwner* pkVassal, FvUInt16 uiDataID)
{
	if(!m_pkOwner)
		return NULL;

	FvUInt32 uiCnt(0);
	FvUInt32* pBits = NULL;
	FvUInt16* pIdx = NULL;
	PtIdxToID* pIDs = NULL;
	bool bRet = GetPtToID(uiCnt, pBits, pIdx, pIDs);
	FV_ASSERT(bRet);

	FvUInt16 uiIdx, uiID;
	FvPointerToIndexAndDataID(this, pkVassal, uiIdx, uiID,
		uiCnt, pBits, pIdx, pIDs);

	return m_pkOwner->GetRootData(uiOpCode, this, uiDataID + uiID);
}
#endif
FvDataOwner* FvObjStruct::DataVassal(FvUInt32 uiDataID)
{
	FvUInt32 uiCnt(0);
	FvUInt32* pAddr = NULL;
	bool bRet = GetIDToAddr(uiCnt, pAddr);
	FV_ASSERT(bRet && uiDataID && uiDataID <= uiCnt);
	return (FvDataOwner*)((char*)this + pAddr[uiDataID-1]);
}
#ifdef FV_SERVER
void FvObjStruct::Attach(FvDataOwner* pkOwner, bool bSetOnce)
{
	FvDataOwner::Attach(pkOwner);
	pkOwner = bSetOnce ? this : pkOwner;

	FvUInt32 uiCnt(0);
	NameAddr* pkInfo = NULL;
	GetStructAddr(uiCnt, pkInfo);
	for(FvUInt32 i(0); i<uiCnt; ++i)
	{
		((FvDataObj*)((char*)this + pkInfo[i].uiAddr))->Attach(pkOwner);
	}
}

void FvObjStruct::Detach()
{
	FvDataOwner::Detach();

	FvUInt32 uiCnt(0);
	NameAddr* pkInfo = NULL;
	GetStructAddr(uiCnt, pkInfo);
	for(FvUInt32 i(0); i<uiCnt; ++i)
	{
		((FvDataObj*)((char*)this + pkInfo[i].uiAddr))->Detach();
	}
}
#endif
bool FvObjStruct::SerializeToStream(FvBinaryOStream& kOS) const
{
	FvUInt32 uiCnt(0);
	NameAddr* pkInfo = NULL;
	GetStructAddr(uiCnt, pkInfo);
	for(FvUInt32 i(0); i<uiCnt; ++i)
	{
		((FvDataObj*)((char*)this + pkInfo[i].uiAddr))->SerializeToStream(kOS);
	}
	return true;
}

bool FvObjStruct::DeserializeFromStream(FvBinaryIStream& kIS)
{
	FvUInt32 uiCnt(0);
	NameAddr* pkInfo = NULL;
	GetStructAddr(uiCnt, pkInfo);
	for(FvUInt32 i(0); i<uiCnt; ++i)
	{
		((FvDataObj*)((char*)this + pkInfo[i].uiAddr))->DeserializeFromStream(kIS);
	}
	return true;
}

bool FvObjStruct::SerializeToSection(FvXMLSectionPtr spSection) const
{
	FvUInt32 uiCnt(0);
	NameAddr* pkInfo = NULL;
	GetStructAddr(uiCnt, pkInfo);
	for(FvUInt32 i(0); i<uiCnt; ++i)
	{
		FvXMLSectionPtr spChild = spSection->NewSection(pkInfo[i].pName);
		((FvDataObj*)((char*)this + pkInfo[i].uiAddr))->SerializeToSection(spChild);
	}
	return true;
}

bool FvObjStruct::DeserializeFromSection(FvXMLSectionPtr spSection)
{
	FvUInt32 uiCnt(0);
	NameAddr* pkInfo = NULL;
	GetStructAddr(uiCnt, pkInfo);
	for(FvUInt32 i(0); i<uiCnt; ++i)
	{
		FvXMLSectionPtr spChild = spSection->OpenSection(pkInfo[i].pName);
		if(spChild)
			((FvDataObj*)((char*)this + pkInfo[i].uiAddr))->DeserializeFromSection(spChild);
	}
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

