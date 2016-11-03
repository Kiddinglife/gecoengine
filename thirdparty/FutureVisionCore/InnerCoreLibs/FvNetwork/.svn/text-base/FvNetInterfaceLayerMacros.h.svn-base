//{future header message}
#ifndef __FvNetInterfaceLayerMacros_H__
#define __FvNetInterfaceLayerMacros_H__

#include "FvNetInterfaceLayer.h"

#endif // __FvNetInterfaceLayerMacros_H__

#undef LAYER_BEGIN
#undef LAYER_END

#undef LAYER_FUNCTION_0
#undef LAYER_FUNCTION_0V
#undef LAYER_FUNCTION_1
#undef LAYER_FUNCTION_1V
#undef LAYER_FUNCTION_2
#undef LAYER_FUNCTION_2V
#undef LAYER_FUNCTION_3
#undef LAYER_FUNCTION_3V
#undef LAYER_FUNCTION_4
#undef LAYER_FUNCTION_4V


#ifndef DO_SOURCE
	#define LAYER_BEGIN(NAME) \
	class SERVER_TYPE; \
	namespace NAME { \
		extern FvNetInterfaceLayer<SERVER_TYPE> gLayer; \
		int RegisterWithNub(FvNetNub & nub, int id, \
			bool publicise = true); \
		\
		class IF { \
		public: \
			IF( const FvNetAddress & addr, FvNetNub & nub );

	#define LAYER_END() \
		const FvNetAddress & __addr() const { return m_kAddr; } \
		private: \
			FvNetAddress	m_kAddr; \
			FvNetNub		& m_kNub; \
		}; \
	}

	#undef LAYER_FUNCTION_COMMON
	#define LAYER_FUNCTION_COMMON(RET,NAME)	\
		static FvNetInterfaceElement & NAME##__IE; \
		constructor_type<RET>::type NAME

	#define LAYER_FUNCTION_0V LAYER_FUNCTION_0
	#define LAYER_FUNCTION_1V LAYER_FUNCTION_1
	#define LAYER_FUNCTION_2V LAYER_FUNCTION_2
	#define LAYER_FUNCTION_3V LAYER_FUNCTION_3
	#define LAYER_FUNCTION_4V LAYER_FUNCTION_4

	#define LAYER_FUNCTION_0(RET,NAME)	\
		LAYER_FUNCTION_COMMON(RET,NAME) (); \

	#define LAYER_FUNCTION_1(RET,NAME,ARG1)	\
		LAYER_FUNCTION_COMMON(RET,NAME) (const ARG1 arg1);

	#define LAYER_FUNCTION_2(RET,NAME,ARG1,ARG2)	\
		LAYER_FUNCTION_COMMON(RET,NAME) (const ARG1 arg1, const ARG2 arg2);

	#define LAYER_FUNCTION_3(RET,NAME,ARG1,ARG2,ARG3)	\
		LAYER_FUNCTION_COMMON(RET,NAME) \
			(const ARG1 arg1, const ARG2 arg2, const ARG3 arg3);

	#define LAYER_FUNCTION_4(RET,NAME,ARG1,ARG2,ARG3,ARG4)	\
		LAYER_FUNCTION_COMMON(RET,NAME) \
			(const ARG1 arg1, const ARG2 arg2, const ARG3 arg3, \
				const ARG4 arg4);

#else // DO_SOURCE

	#define LAYER_BEGIN(NAME) \
	LAYER_BEGIN_EXTRA_A \
	namespace NAME { \
		FvNetInterfaceLayer<SERVER_TYPE>		gLayer(#NAME); \
		\
		int RegisterWithNub LAYER_BEGIN_EXTRA_B \
		\
		IF::IF( const FvNetAddress & addr, FvNetNub & nub ) : \
			m_kAddr(addr), m_kNub(nub) {}


	#define LAYER_END() }

	#undef LAYER_FUNCTION_COMMON_A
	#define LAYER_FUNCTION_COMMON_A(RET,NAME) \
	FvNetInterfaceElement & IF::NAME##__IE = \
		gLayer.Add(NULLIFY_FOR_CLIENT(&NAME##__Handler)); \
	constructor_type<RET>::type IF::NAME

	#undef LAYER_FUNCTION_COMMON_B
	#define LAYER_FUNCTION_COMMON_B(RET,NAME) \
		FvNetBlockingReplyHandler<constructor_type<RET>::type>	\
			hand(m_kNub); \
		FvNetBundle		b; \
		b.StartRequest(IF::NAME##__IE,&hand);

	#undef LAYER_FUNCTION_COMMON_C
	#define LAYER_FUNCTION_COMMON_C \
		hand.await(); \
		if(hand.err()) throw(FvNetInterfaceLayerError(hand.err())); \
		return hand.get();

	#undef LAYER_FUNCTION_COMMON_BV
	#define LAYER_FUNCTION_COMMON_BV(NAME) \
		FvNetBundle		b; \
		b.StartMessage(IF::NAME##__IE);

	#undef LAYER_FUNCTION_SEND
	#define LAYER_FUNCTION_SEND \
		m_kNub.Send(m_kAddr,b);


	#define LAYER_FUNCTION_0(RET,NAME) \
	LAYER_FUNCTION_EXTRA_0(RET,NAME) \
	LAYER_FUNCTION_COMMON_A(RET,NAME) () \
	{ \
		LAYER_FUNCTION_COMMON_B(RET,NAME) \
		LAYER_FUNCTION_SEND \
		LAYER_FUNCTION_COMMON_C \
	}

	#define LAYER_FUNCTION_0V(RET,NAME) \
	LAYER_FUNCTION_EXTRA_0(RET,NAME) \
	LAYER_FUNCTION_COMMON_A(RET,NAME) () \
	{ \
		LAYER_FUNCTION_COMMON_BV(NAME) \
		LAYER_FUNCTION_SEND \
	}

	#define LAYER_FUNCTION_1(RET,NAME,ARG1) \
	LAYER_FUNCTION_EXTRA_1(RET,NAME,ARG1) \
	LAYER_FUNCTION_COMMON_A(RET,NAME) (const ARG1 arg1) \
	{ \
		LAYER_FUNCTION_COMMON_B(RET,NAME) \
		b << arg1; \
		LAYER_FUNCTION_SEND \
		LAYER_FUNCTION_COMMON_C \
	}

	#define LAYER_FUNCTION_1V(RET,NAME,ARG1) \
	LAYER_FUNCTION_EXTRA_1(RET,NAME,ARG1) \
	LAYER_FUNCTION_COMMON_A(RET,NAME) (const ARG1 arg1) \
	{ \
		LAYER_FUNCTION_COMMON_BV(NAME) \
		b << arg1; \
		LAYER_FUNCTION_SEND \
	}

	#define LAYER_FUNCTION_2(RET,NAME,ARG1,ARG2) \
	LAYER_FUNCTION_EXTRA_2(RET,NAME,ARG1,ARG2) \
	LAYER_FUNCTION_COMMON_A(RET,NAME) (const ARG1 arg1, const ARG2 arg2) \
	{ \
		LAYER_FUNCTION_COMMON_B(RET,NAME) \
		b << arg1; \
		b << arg2; \
		LAYER_FUNCTION_SEND \
		LAYER_FUNCTION_COMMON_C \
	}

	#define LAYER_FUNCTION_2V(RET,NAME,ARG1,ARG2) \
	LAYER_FUNCTION_EXTRA_2(RET,NAME,ARG1,ARG2) \
	LAYER_FUNCTION_COMMON_A(RET,NAME) (const ARG1 arg1, const ARG2 arg2) \
	{ \
		LAYER_FUNCTION_COMMON_BV(NAME) \
		b << arg1; \
		b << arg2; \
		LAYER_FUNCTION_SEND \
	}

	#define LAYER_FUNCTION_3(RET,NAME,ARG1,ARG2,ARG3) \
	LAYER_FUNCTION_EXTRA_3(RET,NAME,ARG1,ARG2,ARG3) \
	LAYER_FUNCTION_COMMON_A(RET,NAME) \
		(const ARG1 arg1, const ARG2 arg2, const ARG3 arg3) \
	{ \
		LAYER_FUNCTION_COMMON_B(RET,NAME) \
		b << arg1; \
		b << arg2; \
		b << arg3; \
		LAYER_FUNCTION_SEND \
		LAYER_FUNCTION_COMMON_C \
	}

	#define LAYER_FUNCTION_3V(RET,NAME,ARG1,ARG2,ARG3) \
	LAYER_FUNCTION_EXTRA_3(RET,NAME,ARG1,ARG2,ARG3) \
	LAYER_FUNCTION_COMMON_A(RET,NAME) \
		(const ARG1 arg1, const ARG2 arg2, const ARG3 arg3) \
	{ \
		LAYER_FUNCTION_COMMON_BV(NAME) \
		b << arg1; \
		b << arg2; \
		b << arg3; \
		LAYER_FUNCTION_SEND \
	}

	#define LAYER_FUNCTION_4(RET,NAME,ARG1,ARG2,ARG3,ARG4) \
	LAYER_FUNCTION_EXTRA_4(RET,NAME,ARG1,ARG2,ARG3,ARG4) \
	LAYER_FUNCTION_COMMON_A(RET,NAME) \
		(const ARG1 arg1, const ARG2 arg2, const ARG3 arg3, const ARG4 arg4) \
	{ \
		LAYER_FUNCTION_COMMON_B(RET,NAME) \
		b << arg1; \
		b << arg2; \
		b << arg3; \
		b << arg4; \
		LAYER_FUNCTION_SEND \
		LAYER_FUNCTION_COMMON_C \
	}

	#define LAYER_FUNCTION_4V(RET,NAME,ARG1,ARG2,ARG3,ARG4) \
	LAYER_FUNCTION_EXTRA_4(RET,NAME,ARG1,ARG2,ARG3,ARG4) \
	LAYER_FUNCTION_COMMON_A(RET,NAME) \
		(const ARG1 arg1, const ARG2 arg2, const ARG3 arg3, const ARG4 arg4) \
	{ \
		LAYER_FUNCTION_COMMON_BV(NAME) \
		b << arg1; \
		b << arg2; \
		b << arg3; \
		b << arg4; \
		LAYER_FUNCTION_SEND \
	}


	#ifndef DO_SERVER
		#undef LAYER_BEGIN_EXTRA_A
		#define LAYER_BEGIN_EXTRA_A class SERVER_TYPE {};

		#undef LAYER_BEGIN_EXTRA_B
		#define LAYER_BEGIN_EXTRA_B \
			(FvNetNub &, int, bool) \
			{ return FV_NET_REASON_CORRUPTED_PACKET; }

		#undef NULLIFY_FOR_CLIENT
		#define NULLIFY_FOR_CLIENT(X) NULL

		#undef LAYER_FUNCTION_EXTRA_0
		#define LAYER_FUNCTION_EXTRA_0(RET,NAME)

		#undef LAYER_FUNCTION_EXTRA_1
		#define LAYER_FUNCTION_EXTRA_1(RET,NAME,A1)

		#undef LAYER_FUNCTION_EXTRA_2
		#define LAYER_FUNCTION_EXTRA_2(RET,NAME,A1,A2)

		#undef LAYER_FUNCTION_EXTRA_3
		#define LAYER_FUNCTION_EXTRA_3(RET,NAME,A1,A2,A3)

		#undef LAYER_FUNCTION_EXTRA_4
		#define LAYER_FUNCTION_EXTRA_4(RET,NAME,A1,A2,A3,A4)

	#else
		#undef LAYER_BEGIN_EXTRA_A
		#define LAYER_BEGIN_EXTRA_A

		#undef LAYER_BEGIN_EXTRA_B
		#define LAYER_BEGIN_EXTRA_B \
			(FvNetNub & nub, int id, bool publicise) \
			{ return gLayer.RegisterWithNub(nub,id,publicise); }

		#undef NULLIFY_FOR_CLIENT
		#define NULLIFY_FOR_CLIENT(X) X

		#undef LAYER_FUNCTION_EXTRA_0
		#define LAYER_FUNCTION_EXTRA_0(RET,NAME) \
		IFHandler0<SERVER_TYPE,RET>	\
			NAME##__Handler(&SERVER_TYPE::NAME);

		#undef LAYER_FUNCTION_EXTRA_1
		#define LAYER_FUNCTION_EXTRA_1(RET,NAME,A1) \
		IFHandler1<SERVER_TYPE,RET,A1> \
			NAME##__Handler(&SERVER_TYPE::NAME);

		#undef LAYER_FUNCTION_EXTRA_2
		#define LAYER_FUNCTION_EXTRA_2(RET,NAME,A1,A2) \
		IFHandler2<SERVER_TYPE,RET,A1,A2> \
			NAME##__Handler(&SERVER_TYPE::NAME);

		#undef LAYER_FUNCTION_EXTRA_3
		#define LAYER_FUNCTION_EXTRA_3(RET,NAME,A1,A2,A3) \
		IFHandler3<SERVER_TYPE,RET,A1,A2,A3> \
			NAME##__Handler(&SERVER_TYPE::NAME);

		#undef LAYER_FUNCTION_EXTRA_4
		#define LAYER_FUNCTION_EXTRA_4(RET,NAME,A1,A2,A3,A4) \
		IFHandler4<SERVER_TYPE,RET,A1,A2,A3,A4> \
			NAME##__Handler(&SERVER_TYPE::NAME);

	#endif

#endif

#undef DO_SOURCE
