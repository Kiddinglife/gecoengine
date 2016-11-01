//{future header message}
#ifndef __FvPropertyOwnerBase_H__
#define __FvPropertyOwnerBase_H__

#include <FvSmartPointer.h>
#include <FvBinaryStream.h>
#include <xstring>

class FvObj;
typedef FvSmartPointer<FvObj> FvObjPtr;
class FvDataType;

//class FvPropertyOwnerBase;
//typedef FvSmartPointer<FvPropertyOwnerBase> FvPropertyOwnerBasePtr;

class FvObj;
class FvOldSimpleDataFvInt8;
class FvOldSimpleDataFvUInt8;
class FvOldSimpleDataFvInt16;
class FvOldSimpleDataFvUInt16;
class FvOldSimpleDataFvInt32;
class FvOldSimpleDataFvUInt32;
class FvOldSimpleDataFvInt64;
class FvOldSimpleDataFvUInt64;
class FvOldSimpleDatafloat;
class FvOldSimpleDatadouble;
class FvObjVector2Old;
class FvObjVector3Old;
class FvObjVector4Old;
class FvObjStringOld;
class FvObjArrayOld;
class FvObjIntMap;
class FvObjStrMap;
class FvObjStructOld;
class FvEntityAttributes;

class FvArrayDataType;
class FvStructDataType;
class FvIntMapDataType;
class FvStringMapDataType;

class FV_ENTITYDEF_API FvPropertyOwnerBase
{
public:
	FvPropertyOwnerBase();
	virtual ~FvPropertyOwnerBase() { FV_ASSERT(!m_pkOwner); }

	void			SetNewObj();
	bool			IsNewObj();
	void			ClearNewObjFlag();
	void			SetDirty();
	bool			IsDirty();
	void			ClearDirtyFlag();
	void			SetDiscard();
	bool			IsDiscard();
	void			ClearDiscardFlag();
	bool			HasOwner() const { return m_pkOwner!=NULL; }
	bool			IsOwner(const FvPropertyOwnerBase* pkOwner)const{return m_pkOwner == pkOwner;}

	enum FvPropertyOperator
	{
		FVPROP_OP_ADD = 0,
		FVPROP_OP_DEL,
		FVPROP_OP_CLR,
		FVPROP_OP_MOD	//! TODO: stream->obj不要new新obj,直接流化
	};

	typedef std::basic_string<FvUInt32> ChangePath;
	typedef std::pair<FvObjPtr,FvObjPtr> ChangeVal;

protected:
	virtual void	PropertyChanged(FvPropertyOperator op, FvUInt32 opIdx,
						const ChangeVal& oldVal, const ChangeVal& newVal, ChangePath& path);
	virtual FvInt32 PropertyDivisions() { return 0; }
	virtual FvPropertyOwnerBase* PropertyVassal(FvUInt32 ref) { return NULL; }
	virtual void	PropertyRenovate(FvPropertyOperator op, FvUInt32 opIdx,
						FvBinaryIStream& data, ChangeVal& oldValue, ChangeVal& newValue) {}

	FvUInt8			AddToStream(FvPropertyOperator op, FvUInt32 opIdx, const ChangeVal& val, const ChangePath& path, FvBinaryOStream& stream, bool bCompressPath=true);
	FvPropertyOwnerBase* GetPathFromStream(FvInt32 messageID, FvBinaryIStream& data, FvPropertyOperator& op, FvUInt32& opIdx, ChangePath& path);

	void			SetRef(FvUInt32 ref) { m_uiOwnerRef=ref; }
	FvUInt32		GetRef() const { return m_uiOwnerRef; }

	virtual	void	Attach(FvPropertyOwnerBase* owner) { FV_ASSERT(!m_pkOwner); m_pkOwner=owner; ClearDiscardFlag(); }
	virtual	void	Detach() { m_pkOwner=NULL; SetDiscard(); }

protected:
	FvPropertyOwnerBase*	m_pkOwner;
	FvUInt32				m_uiOwnerRef;
#ifndef FV_SERVER
	FvUInt8					m_uiFlag;
#endif

	friend class FvObj;
	friend class FvOldSimpleDataFvInt8;
	friend class FvOldSimpleDataFvUInt8;
	friend class FvOldSimpleDataFvInt16;
	friend class FvOldSimpleDataFvUInt16;
	friend class FvOldSimpleDataFvInt32;
	friend class FvOldSimpleDataFvUInt32;
	friend class FvOldSimpleDataFvInt64;
	friend class FvOldSimpleDataFvUInt64;
	friend class FvOldSimpleDatafloat;
	friend class FvOldSimpleDatadouble;
	friend class FvObjVector2Old;
	friend class FvObjVector3Old;
	friend class FvObjVector4Old;
	friend class FvObjStringOld;
	friend class FvObjArrayOld;
	friend class FvObjIntMap;
	friend class FvObjStrMap;
	friend class FvObjStructOld;
	friend class FvEntityAttributes;
	friend class FvArrayDataType;
	friend class FvStructDataType;
	friend class FvIntMapDataType;
	friend class FvStringMapDataType;
	friend class FvNumFixArrayDataType;
};


#endif//__FvPropertyOwnerBase_H__