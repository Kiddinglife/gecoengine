//{future header message}
#ifndef __FvNumFixArrayExportMacro_H__
#define __FvNumFixArrayExportMacro_H__



#define NUM_FIXARRAY_ITEMNAME_START(_CLASS)\
	const char* s_pkNumFixArrayItemName_of_##_CLASS[] = {

#define ITEMNAME(_NAME)	#_NAME,

#define NUM_FIXARRAY_ITEMNAME_END(_PREFIX, _CLASS, _TYPE, _SIZE)	};				\
	const char** _CLASS::GetItemName() const										\
	{																				\
		return s_pkNumFixArrayItemName_of_##_CLASS;									\
	}																				\
	namespace FvDataPrint															\
	{																				\
		template<>																	\
		_PREFIX void Print(const char* pcName, const _CLASS& kVal, FvString& kMsg)	\
		{																			\
			const FvObjNumFixArray<_TYPE, _SIZE>& kParent = kVal;					\
			FvDataPrint::Print(pcName, kParent, kMsg);								\
		}																			\
		template<>																	\
		_PREFIX void Print(const _CLASS& kVal, FvString& kMsg)						\
		{																			\
			const FvObjNumFixArray<_TYPE, _SIZE>& kParent = kVal;					\
			FvDataPrint::Print(kParent, kMsg);										\
		}																			\
	}																				\
	template<>																		\
	_PREFIX FvBinaryIStream& operator>>(FvBinaryIStream& kIS, _CLASS& kArray)		\
	{																				\
		FvObjNumFixArray<_TYPE, _SIZE>& kParent = kArray;							\
		return operator>>(kIS, kParent);											\
	}																				\
	template<>																		\
	_PREFIX FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const _CLASS& kArray)	\
	{																				\
		const FvObjNumFixArray<_TYPE, _SIZE>& kParent = kArray;						\
		return operator<<(kOS, kParent);											\
	}																				\
	namespace FvXMLData																\
	{																				\
		template<>																	\
		_PREFIX void Read(FvXMLSectionPtr spSection, _CLASS& kVal)					\
		{																			\
			FvObjNumFixArray<_TYPE, _SIZE>& kParent = kVal;							\
			FvXMLData::Read(spSection, kParent);									\
		}																			\
		template<>																	\
		_PREFIX void Write(FvXMLSectionPtr spSection, const _CLASS& kVal)			\
		{																			\
			const FvObjNumFixArray<_TYPE, _SIZE>& kParent = kVal;					\
			FvXMLData::Write(spSection, kParent);									\
		}																			\
	}



#endif//__FvNumFixArrayExportMacro_H__
