//{future header message}
#ifndef __FvAttachRelation_H__
#define  __FvAttachRelation_H__

#include <FvKernel.h>
#include <FvDebug.h>





template<class T, T DEFAULT_VALUE = 0>
class FvAttachRelationPlug
{
public:
	template<class T, T DEFAULT_VALUE>friend class FvAttachRelationSocket;
	typedef FvAttachRelationSocket<T, DEFAULT_VALUE> _Socket;

	FvAttachRelationPlug()
		:m_rpSocket(NULL){}

	~FvAttachRelationPlug(void)
	{
		Detach();
	}
	void Detach()
	{
		if(m_rpSocket)
		{
			FV_ASSERT(m_rpSocket->m_rpPlug == this);
			m_rpSocket->m_rpPlug = NULL;
			m_rpSocket->m_kT = DEFAULT_VALUE;
			m_rpSocket = NULL;
		}
	}


	bool IsAttach()const{return (m_rpSocket != NULL);}


private:


	_Socket* m_rpSocket;
};


template<class T, T DEFAULT_VALUE = 0>
class FvAttachRelationSocket
{
public:
	template<class T, T DEFAULT_VALUE>friend class FvAttachRelationPlug;
	typedef FvAttachRelationPlug<T, DEFAULT_VALUE> _Plug;

	FvAttachRelationSocket()
		:m_kT(DEFAULT_VALUE), m_rpPlug(NULL)
	{}

	~FvAttachRelationSocket(void)
	{
		Detach();
	}

	void Detach()
	{
		if(m_rpPlug)
		{
			FV_ASSERT(m_rpPlug->m_rpSocket == this);
			m_rpPlug->m_rpSocket = NULL;
			m_kT = DEFAULT_VALUE;
			m_rpPlug = NULL;
		}
	}

	bool IsAttach()const{return (m_rpPlug != NULL);}


	bool Attach(_Plug& kPlug, const T& kT)
	{
		if(m_rpPlug)
		{
			return false;
		}
		if(kPlug.m_rpSocket)
		{
			kPlug.m_rpSocket->m_kT = DEFAULT_VALUE;
			FV_ASSERT(kPlug.m_rpSocket->m_rpPlug == &kPlug)
			kPlug.m_rpSocket->m_rpPlug = NULL;
		}
		m_kT = kT;
		kPlug.m_rpSocket = this;
		m_rpPlug = &kPlug;
		return true;
	}
private:

	T m_kT;
	_Plug* m_rpPlug;
};


//template<class T, T DEFAULT_VALUE = 0>
//class FvAttachRelationSocketVector
//{
//	void SwapSlot(const FvUInt16 uiSlot_1, const FvUInt16 uiSlot_2);
//	bool Attach(const FvUInt16 uiSlot, FvSlotObject& kObj);
//};



namespace AttachRelation_Demo
{

	static void TEST()
	{
		int iValue = 1;
		typedef FvAttachRelationPlug<int*> Plag;
		typedef FvAttachRelationSocket<int*> Socket;
		Plag kPlag0;
		Plag kPlag1;
		Socket kSocket0;
		Socket kSocket1;
		kSocket0.Attach(kPlag0, &iValue);
		kSocket1.Attach(kPlag0, &iValue);
		kSocket1.Detach();
		kSocket1.Attach(kPlag0, &iValue);
		kPlag0.Detach();
		kSocket1.Attach(kPlag0, &iValue);
		kSocket1.Attach(kPlag1, &iValue);
	}


}

#endif //__FvAttachRelation_H__