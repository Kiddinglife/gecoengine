
#include <vld.h>
#include <FvAura.h>
#include <FvDestroyManager.h>

#include <FvCellUnit.h>


#include "FvTestSpell.h"


//class TestA
//{
//public:
//	void Func_1(){};
//	void Func_2(const TestA*){}
//};

//
//template<class T>
//FvUInt32 GhostInvoker<T>::GetFuncIdx<TestA, &TestA::Func_1>()
//{
//	return 1;
//}
//
//template<class T>
//template<>
//FvUInt32 GhostInvoker<T>::GetFuncIdx<TestA, &TestA::Func_2>()
//{
//	return 2;
//}

class Param
{

};

class TestA
{
public:
	void Func_1(){};
	void Func_2(const Param){}
};

class TestAFunc
{
public:
	enum _Idx
	{
		FUNC_1,
		FUNC_2,
		FUNC_TOTAL,
	};
};

_FUNC_SET_(TestA)
_SET_FUNC_IDX_1_(TestA, Func_1, TestAFunc::FUNC_1)
_SET_FUNC_IDX_2_(TestA, Func_2, Param, TestAFunc::FUNC_2)

void main()
{
	//! 初始化函数集合
	GhostInvoker<TestA>::SetFuncSize(TestAFunc::FUNC_TOTAL);
	_ADD_GHOST_FUNC_1_(TestA, Func_1);
	_ADD_GHOST_FUNC_2_(TestA, Func_2, Param);

	TestA AObj;
	//! 远程调用
	GhostInvoker<TestA>::Exec(AObj, TestAFunc::FUNC_1);
	GhostInvoker<TestA>::Exec(AObj, TestAFunc::FUNC_2);
	GhostInvoker<TestA>::Exec(AObj, TestAFunc::FUNC_TOTAL);

	GhostInvoker<TestA>::Exec(AObj, _FUNC_IDX_1_(TestA, Func_1));
	GhostInvoker<TestA>::Exec(AObj, _FUNC_IDX_2_(TestA, Func_2, Param));

	//! 本地调用
	GhostInvoker<TestA>::Invoke<&TestA::Func_1>(AObj);
	GhostInvoker<TestA>::Invoke<Param, &TestA::Func_2>(AObj, Param());


	//CellUnit_Demo::TestAttack();
	//Aura_Demo::Test();
	//FvDestroyManager::UpdateDeleteList();
}
