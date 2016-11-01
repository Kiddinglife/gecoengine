//{future header message}
#ifndef __FvEntityDefExportMacro_H__
#define __FvEntityDefExportMacro_H__



//! 定义 FvAllData子类构造函数
#define ALLDATA_CONSTRUCT_START(_CLASS)	\
	_CLASS##AllData::_CLASS##AllData()	\
	:FvAllData()						\
	{

#define ALLDATA_CONSTRUCT_END	}



//! 定义 FvAllData子类序列化
#define ALLDATA_SERIALIZE_START(_CLASS)									\
	bool _CLASS##AllData::SerializeToStream(FvBinaryOStream& kOS) const	\
	{																	\
		kOS

#define ALLDATA_SERIALIZE(_MEMBER)	<< m_##_MEMBER

#define ALLDATA_SERIALIZE_END								\
		<< m_kPos << m_kDir << m_iSpaceID << m_uiSpaceType;	\
		return true;										\
	}



//! 定义 FvAllData子类Cell部分序列化
#define ALLDATA_SERIALIZE_FORCELL_START(_CLASS)										\
	bool _CLASS##AllData::SerializeToStreamForCellData(FvBinaryOStream& kOS) const	\
	{																				\
		kOS

#define ALLDATA_SERIALIZE_FORCELL_END	\
		;return true;					\
	}





#endif//__FvEntityDefExportMacro_H__

