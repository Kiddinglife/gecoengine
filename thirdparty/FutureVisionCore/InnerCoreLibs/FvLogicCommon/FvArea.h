//{future header message}
#ifndef __Fv2DArea_H__
#define __Fv2DArea_H__


#include "FvLogicCommonDefine.h"
#include "FvMemoryNode.h"
#include <FvVector3.h>

class FV_LOGIC_COMMON_API FvArea
{
	FV_MEM_TRACKER(FvArea);
protected:
public:
	FvArea(void);
	virtual ~FvArea(void);

	virtual bool InArea(const float x, const float y, const float z)const = 0;
	virtual bool InArea(const FvVector3& pos)const = 0;
	
	virtual bool InArea(const float x, const float y, const float z, const float fRange)const = 0;
	virtual bool InArea(const FvVector3& pos, const float fRange)const = 0;

	float Range()const;
	const FvVector3& Center()const;

	void SetCenter(const float x, const float y, const float z);
	void SetCenter(const FvVector3& pos);

protected:

	void _SetCenter(const float x, const float y, const float z);

	FvVector3 m_kCenter;
	float m_fRange;
};

//+------------------------------------------------------------------------------------------------------------
//! 园(球)形区域
class FV_LOGIC_COMMON_API FvRoundArea: public FvArea
{
public:
	FvRoundArea();
	void Init(const float fRadius);
	//
	virtual bool InArea(const FvVector3& pos)const{return _InArea(pos.x, pos.y, pos.z);}
	virtual bool InArea(const float x, const float y, const float z)const{return _InArea(x, y, z);}
	virtual bool InArea(const FvVector3& pos, const float fRange)const{return _InArea(pos.x, pos.y, pos.z, fRange);}
	virtual bool InArea(const float x, const float y, const float z, const float fRange)const{return _InArea(x, y, z, fRange);}
	//
	bool _InArea(const float x, const float y, const float z)const;
	bool _InArea(const float x, const float y, const float z, const float fRange)const;

private:
	float m_fRadius2;//Radius*Radius

};

//+------------------------------------------------------------------------------------------------------------
//! 扇形区域
class FV_LOGIC_COMMON_API FvSectorArea: public FvArea
{
public:
	FvSectorArea();
	void Init(const float fRadius, const float fLeftAngle, const float fRightAngle);
	void SetDir(const float fDir);

	virtual bool InArea(const FvVector3& pos)const{return _InArea(pos.x, pos.y, pos.z);}
	virtual bool InArea(const float x, const float y, const float z)const{return _InArea(x, y, z);}
	virtual bool InArea(const FvVector3& pos, const float fRange)const{return _InArea(pos.x, pos.y, pos.z, fRange);}
	virtual bool InArea(const float x, const float y, const float z, const float fRange)const{return _InArea(x, y, z, fRange);}
	//
	bool _InArea(const float x, const float y, const float z)const;
	bool _InArea(const float x, const float y, const float z, const float fRange)const;

private:

	float m_fLeftAngle;
	float m_fRightAngle;

	float m_fDirLeftAngle;
	float m_fDirRightAngle;

	float m_fRadius2;
};


//+------------------------------------------------------------------------------------------------------------
//! 旋转矩形


class FV_LOGIC_COMMON_API FvRotRect
{
public:
	FvRotRect();
	bool In(const FvVector3& center, const FvVector3& pos)const;
	bool In(const FvVector3& center, const FvVector3& pos, const float fRange)const;
	void SetDirection(const FvVector3& dir);
	float m_fLen;
	float m_fHalfWidth;
private:
	FvVector3 m_kDir;
};


class FV_LOGIC_COMMON_API FvRotRectArea: public FvArea
{
public:
	FvRotRectArea();
	void Rotate(const float fAngle);
	void SetDir(const FvVector3& kDir);
	void Init(const float fFront, const float fHalfWidth);
	//
	virtual bool InArea(const FvVector3& pos)const{return _InArea(pos.x, pos.y, pos.z);}
	virtual bool InArea(const float x, const float y, const float z)const{return _InArea(x, y, z);}
	virtual bool InArea(const FvVector3& pos, const float fRange)const{return _InArea(pos.x, pos.y, pos.z, fRange);}
	virtual bool InArea(const float x, const float y, const float z, const float fRange)const{return _InArea(x, y, z, fRange);}
	//
	bool _InArea(const float x, const float y, const float z)const;
	bool _InArea(const float x, const float y, const float z, const float fRange)const;

private:

	FvRotRect m_kRotRect;
};


//+------------------------------------------------------------------------------------------------------------
class FV_LOGIC_COMMON_API FvAreaData
{
public:
	enum 
	{
		ROUND,
		SECTOR,
		ROT_RECT,
	};

	FvUInt32 uiType;
	FvVector3 m_kCenter;

	float m_fValue0;
	float m_fValue1;
	float m_fValue2;
};

//+------------------------------------------------------------------------------------------------------------
class FV_LOGIC_COMMON_API FvAreaTool
{
public:
	const FvArea& GetArea(const FvAreaData& kData); 

private:

	FvRoundArea m_kRoundArea;
	FvSectorArea m_kSectorArea;
	FvRotRectArea m_kRotRectArea;
};


#endif //__Fv2DArea_H__