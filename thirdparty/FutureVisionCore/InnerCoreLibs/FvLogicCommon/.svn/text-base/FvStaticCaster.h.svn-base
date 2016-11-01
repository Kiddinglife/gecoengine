#pragma once

#include <FvKernel.h>

class FvStaticCaster
{
	FV_NOT_COPY_COMPARE(FvStaticCaster);
public:
	FvStaticCaster(){}
	template <class TFather, class TChild, TChild TFather::*OFFSET>
	static TFather& GetFather(TChild& kChild)
	{
		return _GetFather<TFather, TChild, OFFSET>(kChild);
	}
	template <class TFather, class TChild, TChild TFather::*OFFSET>
	static const TFather& GetFather(const TChild& kChild)
	{
		return _GetFather<TFather, TChild, OFFSET>(kChild);
	}
private:

	template <class TFather, class TChild, TChild TFather::*OFFSET>
	static TFather& _GetFather(const TChild& kChild)
	{
		const TFather* const pStandardFather = 0;
		const TChild* const pStandardChild = &((*pStandardFather).*OFFSET);
		int pOffset = int(pStandardFather) - int(pStandardChild);
		char* p = (char*)&kChild;
		p += pOffset;
		return *(TFather*)p;
	}
};


namespace StaticCaster_Demo
{

	class Child
	{
	public:
		int _i;
	};
	class Father
	{
	public:
		int _i;
		bool _b1;
		Child _kChild;
		bool _b2;
	};

	static void Test()
	{
		Father kFather;
		kFather._b1 = true;
		kFather._b2 = false;
		kFather._i = 10;
		kFather._kChild._i = 2;
		Father& kSameFather = FvStaticCaster::GetFather<Father, Child, &Father::_kChild>(kFather._kChild);
	}

}