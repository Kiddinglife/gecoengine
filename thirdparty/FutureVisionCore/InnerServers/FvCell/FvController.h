#ifndef __FvController_H__
#define __FvController_H__

#include <FvBinaryStream.h>
#include "FvControllerMacro.h"
#include "FvControllerFunction.h"

class FvEntity;

class FvController
{
public:
	FvController(FvEntity* pkEntity);
	FvController(FvController* pkParent);
	virtual ~FvController(){}

	static const FvUInt16 INVALID_ID = FvUInt16(-1);

	virtual const FvUInt16 GetType() = 0;

	virtual void Destroy() = 0;

	virtual void Serializer(FvBinaryOStream& kOStream) {}
	virtual void Unserializer(FvBinaryIStream& kIStream) {}

	virtual void Excute(FvUInt16 uiMethodID,FvBinaryIStream& kIStream) {}

	FV_INLINE const FvUInt16 GetID() const { return m_uiID; }
	FV_INLINE void SetID(FvUInt16 uiID) { m_uiID = uiID; }
	FV_INLINE FvEntity* GetEntity() { return m_pkEntity; }
	FV_INLINE void SetEntity(FvEntity *pkEntity) { m_pkEntity = pkEntity; }
	FV_INLINE FvController* GetParent() { return m_pkParent; }
	FV_INLINE void SetParent(FvController* pkParent) { m_pkParent = pkParent; }
	FV_INLINE FvUInt32 GetMemoryOffset() { return m_uiMemoryOffset; }
	FV_INLINE void SetMemoryOffset(FvUInt32 uiOffset) { m_uiMemoryOffset = uiOffset; }

	const bool m_bIsNew;

protected:
	FvController();

	FvUInt16 m_uiID;
	FvEntity* m_pkEntity;
	FvController* m_pkParent;
	FvUInt32 m_uiMemoryOffset;
};

class FvControllerFactory
{
public:

	typedef FvController* (*Creator)();

	template<class T>
	FvControllerFactory(T*,Creator pfCreator) :
	m_uiType(++ms_uiTypeIdentify)
	{
		ms_kCreators[m_uiType] = pfCreator;
	}

	FV_INLINE const FvUInt16 GetType() { return m_uiType; }

	FV_INLINE static FvController* Create(FvUInt32 uiType) 
	{ 
		FV_ASSERT(ms_kCreators.find(uiType) != ms_kCreators.end());
		return (*ms_kCreators[uiType])(); 
	} 
		
private:

	FvUInt16 m_uiType;
	static FvUInt16 ms_uiTypeIdentify;
	static std::map<FvUInt32,Creator> ms_kCreators;
};

template<class T>
FV_INLINE void ControllerSerializer(FvController* pkHost, FvBinaryOStream& kOStream, T& kValue);
template<class T>
FV_INLINE void ControllerUnserializer(FvController* pkHost, FvBinaryIStream& kOStream, T& kValue);

#include "FvController.inl"

#endif // __FvController_H__