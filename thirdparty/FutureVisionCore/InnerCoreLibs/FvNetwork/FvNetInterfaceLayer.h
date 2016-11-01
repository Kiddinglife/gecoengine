//{future header message}
#ifndef __FvNetInterfaceLayer_H__
#define __FvNetInterfaceLayer_H__

#include "FvNetInterfaceMinder.h"

class FvNetInterfaceLayerError
{
public:
	FvNetInterfaceLayerError(int why) : reason(why) {}
	int	reason;
};

template <class SERV> class FvNetIFHandler : public FvNetInputMessageHandler
{
public:
	FvNetIFHandler() : m_pkInst(NULL), m_pkNub(NULL)
	{
	}

	void SetInstance(SERV & inst)
	{
		m_pkInst = &inst;
	}

	void SetNub(FvNetNub & nub)
	{
		m_pkNub = &nub;
	}
protected:
	SERV *m_pkInst;
	FvNetNub *m_pkNub;
};


template <class C> struct constructor_type { typedef C type; };
template <class C> struct constructor_type<C &> { typedef C type; };

#define ARGBODY(N)					\
typename constructor_type<ARG##N>::type	arg##N;	data >> arg##N;

#define REPLYBODY					\
	FvNetBundle		b;			\
	b.StartReply(header.m_iReplyID);	\
	b << r;							\
	m_pkNub->Send(source,b);


template <class SERV, class RET>
	class IFHandler0 :
		public FvNetIFHandler<SERV>
{
public:
	typedef		RET (SERV::*SubFnType)();
	IFHandler0(SubFnType subFn)	{ subFn_ = subFn; }

	virtual void HandleMessage(const FvNetAddress & source,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & /*data*/)
	{
		RET r = (m_pkInst->*m_pfSubFn)();
		REPLYBODY
	}

private:
	SubFnType m_pfSubFn;
};

template <class SERV>
	class IFHandler0<SERV,void> :
		public FvNetIFHandler<SERV>
{
public:
	typedef		void (SERV::*SubFnType)();
	IFHandler0(SubFnType subFn)	{ subFn_ = subFn; }

	virtual void HandleMessage(const FvNetAddress & /*source*/,
		FvNetUnpackedMessageHeader & /*header*/,
		FvBinaryIStream & /*data*/)
	{
		(m_pkInst->*m_pfSubFn)();
	}

private:
	SubFnType m_pfSubFn;
};

template <class SERV, class RET, class ARG1>
	class IFHandler1 :
		public FvNetIFHandler<SERV>
{
public:
	typedef		RET (SERV::*SubFnType)(ARG1);
	IFHandler1(SubFnType subFn)	{ subFn_ = subFn; }

	virtual void HandleMessage(const FvNetAddress & source,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data)
	{
		ARGBODY(1)
		RET r = (m_pkInst->*m_pfSubFn)(arg1);
		REPLYBODY
	}

private:
	SubFnType m_pfSubFn;
};

template <class SERV, class ARG1>
	class IFHandler1<SERV,void,ARG1> :
		public FvNetIFHandler<SERV>
{
public:
	typedef		void (SERV::*SubFnType)(ARG1);
	IFHandler1(SubFnType subFn)	{ subFn_ = subFn; }

	virtual void HandleMessage(const FvNetAddress & /*source*/,
		FvNetUnpackedMessageHeader & /*header*/,
		FvBinaryIStream & data)
	{
		ARGBODY(1)
		(m_pkInst->*m_pfSubFn)(arg1);
	}

private:
	SubFnType m_pfSubFn;
};

template <class SERV, class RET, class ARG1, class ARG2>
	class IFHandler2 :
		public FvNetIFHandler<SERV>
{
public:
	typedef		RET (SERV::*SubFnType)(ARG1,ARG2);
	IFHandler2(SubFnType subFn)	{ subFn_ = subFn; }

	virtual void HandleMessage(const FvNetAddress & source,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data)
	{
		ARGBODY(1)
		ARGBODY(2)
		RET r = (m_pkInst->*m_pfSubFn)(arg1,arg2);
		REPLYBODY
	}

private:
	SubFnType m_pfSubFn;
};

template <class SERV, class ARG1, class ARG2>
	class IFHandler2<SERV,void,ARG1,ARG2> :
		public FvNetIFHandler<SERV>
{
public:
	typedef		void (SERV::*SubFnType)(ARG1,ARG2);
	IFHandler2(SubFnType subFn)	{ subFn_ = subFn; }

	virtual void HandleMessage(const FvNetAddress & /*source*/,
		FvNetUnpackedMessageHeader & /*header*/,
		FvBinaryIStream & data)
	{
		ARGBODY(1)
		ARGBODY(2)
		(m_pkInst->*m_pfSubFn)(arg1,arg2);
	}

private:
	SubFnType m_pfSubFn;
};

template <class SERV, class RET, class ARG1, class ARG2, class ARG3>
	class IFHandler3 :
		public FvNetIFHandler<SERV>
{
public:
	typedef		RET (SERV::*SubFnType)(ARG1,ARG2,ARG3);
	IFHandler3(SubFnType subFn)	{ subFn_ = subFn; }

	virtual void HandleMessage(const FvNetAddress & source,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data)
	{
		ARGBODY(1)
		ARGBODY(2)
		ARGBODY(3)
		RET r = (m_pkInst->*m_pfSubFn)(arg1,arg2,arg3);
		REPLYBODY
	}

private:
	SubFnType m_pfSubFn;
};

template <class SERV, class ARG1, class ARG2, class ARG3>
	class IFHandler3<SERV,void,ARG1,ARG2,ARG3> :
		public FvNetIFHandler<SERV>
{
public:
	typedef		void (SERV::*SubFnType)(ARG1,ARG2,ARG3);
	IFHandler3(SubFnType subFn)	{ subFn_ = subFn; }

	virtual void HandleMessage(const FvNetAddress & /*source*/,
		FvNetUnpackedMessageHeader & /*header*/,
		FvBinaryIStream & data)
	{
		ARGBODY(1)
		ARGBODY(2)
		ARGBODY(3)
		(m_pkInst->*m_pfSubFn)(arg1,arg2,arg3);
	}

private:
	SubFnType m_pfSubFn;
};

template <class SERV, class RET, class ARG1, class ARG2, class ARG3, class ARG4>
	class IFHandler4 :
		public FvNetIFHandler<SERV>
{
public:
	typedef		RET (SERV::*SubFnType)(ARG1,ARG2,ARG3,ARG4);
	IFHandler4(SubFnType subFn)	{ m_pfSubFn = subFn; }

	virtual void HandleMessage(const FvNetAddress & source,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data)
	{
		ARGBODY(1)
		ARGBODY(2)
		ARGBODY(3)
		ARGBODY(4)
		RET r = (m_pkInst->*m_pfSubFn)(arg1,arg2,arg3,arg4);
		REPLYBODY
	}

private:
	SubFnType m_pfSubFn;
};

template <class SERV, class ARG1, class ARG2, class ARG3, class ARG4>
	class IFHandler4<SERV,void,ARG1,ARG2,ARG3,ARG4> :
		public FvNetIFHandler<SERV>
{
public:
	typedef		void (SERV::*SubFnType)(ARG1,ARG2,ARG3,ARG4);
	IFHandler4(SubFnType subFn)	{ m_pfSubFn = subFn; }

	virtual void HandleMessage(const FvNetAddress & /*source*/,
		FvNetUnpackedMessageHeader & /*header*/,
		FvBinaryIStream & data)
	{
		ARGBODY(1)
		ARGBODY(2)
		ARGBODY(3)
		ARGBODY(4)
		(m_pkInst->*m_pfSubFn)(arg1,arg2,arg3,arg4);
	}

private:
	SubFnType m_pfSubFn;
};



template <class SERV> class FvNetInterfaceLayer
{
public:
	FvNetInterfaceLayer( const char * name ) :
		m_bSumServer(false),
		m_cNext(0),
		m_pcName(name)
	{}

	~FvNetInterfaceLayer()
	{
		for(unsigned int i=0;i<m_kELTs.size();i++)
			delete m_kELTs[i];
		m_kELTs.clear();
	}

	void SetInstance( SERV & inst )
	{
		for(unsigned int i=0;i<m_kELTs.size();i++)
			m_kELTs[i]->dispatcher->SetInstance(inst);
	}

	int RegisterWithNub(FvNetNub & nub, int id, bool publicise = true)
	{
		for(unsigned int i=0;i<m_kELTs.size();i++)
			m_kELTs[i]->dispatcher->SetNub(nub);

		iterator	b = begin(), e = end();
		return nub.ServeInterface(b,e,NULL,m_pcName,id,publicise);
	}

	FvNetInterfaceElement & add(FvNetIFHandler<SERV> * newDispo = NULL)
	{
		LayerElt	*newLE = new LayerElt;

		newLE->ie.m_uiIdentifier = m_cNext++;
		newLE->ie.m_uiLengthStyle = FV_NET_VARIABLE_LENGTH_MESSAGE;
		newLE->ie.m_iLengthParam = 2;
		newLE->dispatcher = newDispo;

		if (newDispo!=NULL) m_bSumServer = true;

		m_kELTs.push_back(newLE);

		return newLE->ie;
	}


	class iterator : public FvNetInterfaceIterator
	{
	public:
		iterator(FvNetInterfaceLayer *from, unsigned char index) :
			from_(from),
			index_(index)
		{
		}

		virtual FvNetInputMessageHandler * handler()
			{ return (*from_).m_kELTs[index_]->dispatcher; }

		virtual const FvNetInterfaceElement & operator*()
			{ return (*from_).m_kELTs[index_]->ie; }
		virtual void operator++(int)
			{ index_++; }
		virtual bool operator==(const FvNetInterfaceIterator &i) const
			{ return ((iterator*)&i)->index_ == index_; }
		virtual bool operator!=(const FvNetInterfaceIterator &i) const
			{ return ((iterator*)&i)->index_ != index_; }
	private:
		FvNetInterfaceLayer	* from_;
		unsigned char index_;
	};
	friend class iterator;

	iterator begin()
		{ return iterator(this,0); }
	iterator end()
		{ return iterator(this,m_cNext); }
private:
	struct LayerElt
	{
		FvNetInterfaceElement ie;
		FvNetIFHandler<SERV> *dispatcher;
	};

	std::vector<LayerElt*> m_kELTs;

	bool m_bSumServer;
	unsigned char m_cNext;
	const char *m_pcName;
};

#endif // __FvNetInterfaceLayer_H__
