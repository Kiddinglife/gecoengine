#include "App.h"
#include <vector>
//#include <FvRefList.h>

template<class FT>
class FvIndexVector
{
public:
	//! 类型声明
	typedef FvUInt32 size_type;
	typedef FvInt32 difference_type;


	//! 构造函数
	FvIndexVector()
	:m_uiSize(0),m_uiCapacity(0)
	,m_pkBufer(NULL)
	{
	}

	//! 析构函数
	~FvIndexVector()
	{
		if(m_pkBufer)
			delete [] m_pkBufer;
	}

	FT* GetHead() { return	m_pkBufer; }

	//! 迭代器
	class const_iterator
	{
	public:
		friend class FvIndexVector;

		const_iterator():m_pkIndex(NULL) {}
		const_iterator(FT* pkIndex):m_pkIndex(pkIndex) {}

		const FT& operator*() const
		{
			return *m_pkIndex;
		}

		const FT* operator->() const
		{
			return m_pkIndex;
		}

		const FT& operator[](difference_type iOff) const
		{
			return *(m_pkIndex+iOff);
		}

		const_iterator& operator++()
		{
			++m_pkIndex;
			return *this;
		}

		const_iterator operator++(int)
		{
			const_iterator kTmp = *this;
			++m_pkIndex;
			return kTmp;
		}

		const_iterator& operator--()
		{
			--m_pkIndex;
			return *this;
		}

		const_iterator operator--(int)
		{
			const_iterator kTmp = *this;
			--m_pkIndex;
			return kTmp;
		}

		const_iterator& operator+=(difference_type iOff)
		{
			m_pkIndex += iOff;
			return *this;
		}

		const_iterator operator+(difference_type iOff) const
		{
			const_iterator kTmp = *this;
			return (kTmp += iOff);
		}

		const_iterator& operator-=(difference_type iOff)
		{
			return (*this += -iOff);
		}

		const_iterator operator-(difference_type iOff) const
		{
			const_iterator kTmp = *this;
			return (kTmp += iOff);
		}

		difference_type operator-(const const_iterator& kRight) const
		{
			return difference_type(m_pkIndex - kRight.m_pkIndex);
		}

		bool operator==(const const_iterator& kRight) const
		{
			return m_pkIndex == kRight.m_pkIndex;
		}

		bool operator!=(const const_iterator& kRight) const
		{
			return m_pkIndex != kRight.m_pkIndex;
		}

		bool operator<(const const_iterator& kRight) const
		{
			return m_pkIndex < kRight.m_pkIndex;
		}

		bool operator>(const const_iterator& kRight) const
		{
			return m_pkIndex > kRight.m_pkIndex;
		}

		bool operator<=(const const_iterator& kRight) const
		{
			return m_pkIndex <= kRight.m_pkIndex;
		}

		bool operator>=(const const_iterator& kRight) const
		{
			return m_pkIndex >= kRight.m_pkIndex;
		}

	protected:
		FT*	m_pkIndex;
	};

	class iterator : public const_iterator
	{
	public:
		friend class FvIndexVector;

		iterator():const_iterator() {}
		iterator(FT* pkIndex):const_iterator(pkIndex) {}

		FT& operator*() const
		{
			return *m_pkIndex;
		}

		FT* operator->() const
		{
			return m_pkIndex;
		}

		FT& operator[](difference_type iOff) const
		{
			return *(m_pkIndex+iOff);
		}

		iterator& operator++()
		{
			++m_pkIndex;
			return *this;
		}

		iterator operator++(int)
		{
			iterator kTmp = *this;
			++m_pkIndex;
			return kTmp;
		}

		iterator& operator--()
		{
			--m_pkIndex;
			return *this;
		}

		iterator operator--(int)
		{
			iterator kTmp = *this;
			--m_pkIndex;
			return kTmp;
		}

		iterator& operator+=(difference_type iOff)
		{
			m_pkIndex += iOff;
			return *this;
		}

		iterator operator+(difference_type iOff) const
		{
			iterator kTmp = *this;
			return (kTmp += iOff);
		}

		iterator& operator-=(difference_type iOff)
		{
			return (*this += -iOff);
		}

		iterator operator-(difference_type iOff) const
		{
			iterator kTmp = *this;
			return (kTmp += iOff);
		}

		difference_type operator-(const iterator& kRight) const
		{
			return difference_type(m_pkIndex - kRight.m_pkIndex);
		}

		bool operator==(const iterator& kRight) const
		{
			return m_pkIndex == kRight.m_pkIndex;
		}

		bool operator!=(const iterator& kRight) const
		{
			return m_pkIndex != kRight.m_pkIndex;
		}

		bool operator<(const iterator& kRight) const
		{
			return m_pkIndex < kRight.m_pkIndex;
		}

		bool operator>(const iterator& kRight) const
		{
			return m_pkIndex > kRight.m_pkIndex;
		}

		bool operator<=(const iterator& kRight) const
		{
			return m_pkIndex <= kRight.m_pkIndex;
		}

		bool operator>=(const iterator& kRight) const
		{
			return m_pkIndex >= kRight.m_pkIndex;
		}
	};

	//! 接口
	bool empty() const				{ return !m_uiSize; }
	size_type size() const			{ return m_uiSize; }
	size_type capacity() const		{ return m_uiCapacity; }
	size_type max_size() const		{ return size_type(~0); }

	iterator begin()				{ return iterator(m_pkBufer); }
	const_iterator begin() const	{ return const_iterator(m_pkBufer); }
	iterator end()					{ return iterator(m_pkBufer + m_uiSize); }
	const_iterator end() const		{ return const_iterator(m_pkBufer + m_uiSize); }

	FT& front()						{ FV_ASSERT(m_uiSize && m_pkBufer) return *m_pkBufer; }
	const FT& front() const			{ FV_ASSERT(m_uiSize && m_pkBufer) return *m_pkBufer; }
	FT& back()						{ FV_ASSERT(m_uiSize && m_pkBufer) return *(m_pkBufer+m_uiSize-1); }
	const FT& back() const			{ FV_ASSERT(m_uiSize && m_pkBufer) return *(m_pkBufer+m_uiSize-1); }

	const FT& at(size_type uiPos) const { FV_ASSERT(uiPos<m_uiSize && m_pkBufer) return *(m_pkBufer+uiPos); }
	FT& at(size_type uiPos)			{ FV_ASSERT(uiPos<m_uiSize && m_pkBufer) return *(m_pkBufer+uiPos); }

	const FT& operator[](size_type uiPos) const { return *(m_pkBufer+uiPos); }
	FT& operator[](size_type uiPos)	{ return *(m_pkBufer+uiPos); }

	void resize(size_type uiNewsize)
	{
		m_pkBufer = new FT[uiNewsize];

		m_uiCapacity = uiNewsize;
		m_uiSize = uiNewsize;
	}

protected:

protected:
	size_type	m_uiSize;
	size_type	m_uiCapacity;
	FT*			m_pkBufer;
};

//template<class FT>
//class FvIndexVector
//{
//public:
//	//! 类型声明
//	typedef FvUInt32 size_type;
//	typedef FvInt32 difference_type;
//
//
//	//! 构造函数
//	FvIndexVector()
//	:m_uiSize(0),m_uiCapacity(0)
//	,m_ppkIndex(NULL),m_pkFreeNode(NULL)
//	{
//	}
//
//	FvIndexVector(const FvIndexVector& kVec)
//	{
//		if(kVec.empty())
//			return;
//
//		m_uiSize = size_type(kVec.size());
//		_ReserveIndex(m_uiSize);
//
//		FT** ppkIndex = m_ppkIndex;
//		FvUInt32 uiID(0);
//		const_iterator itrB = kVec.begin();
//		const_iterator itrE = kVec.end();
//		for(; itrB!=itrE; ++itrB,
//			++uiID, ++ppkIndex)
//		{
//			*ppkIndex = _Create(uiID, *itrB);
//		}
//	}
//
//	//! 析构函数
//	~FvIndexVector()
//	{
//		FT** ppkIndex = m_ppkIndex;
//		FT** ppkEnd = m_ppkIndex + m_uiSize;
//		FT* pkVal;
//		for(; ppkIndex<ppkEnd; ++ppkIndex)
//		{
//			pkVal = *ppkIndex;
//			pkVal->~FT();
//			delete [] ((char*)pkVal - 4);
//		}
//
//		char* pkNode;
//		while(m_pkFreeNode)
//		{
//			pkNode = (char*)m_pkFreeNode;
//			m_pkFreeNode = m_pkFreeNode->m_pkNex;
//			delete [] pkNode;
//		}
//
//		free(m_ppkIndex);
//	}
//
//	FT** GetHead() { return	m_ppkIndex; }
//
//	//! 迭代器
//	class const_iterator
//	{
//	public:
//		friend class FvIndexVector;
//
//		const_iterator():m_ppkIndex(NULL) {}
//		const_iterator(FT** ppkIndex):m_ppkIndex(ppkIndex) {}
//
//		const FT& operator*() const
//		{
//			FV_ASSERT(m_ppkIndex);
//			return **m_ppkIndex;
//		}
//
//		const FT* operator->() const
//		{
//			FV_ASSERT(m_ppkIndex);
//			return *m_ppkIndex;
//		}
//
//		const FT& operator[](difference_type iOff) const
//		{
//			FV_ASSERT(m_ppkIndex);
//			return **(m_ppkIndex+iOff);
//		}
//
//		const_iterator& operator++()
//		{
//			FV_ASSERT(m_ppkIndex);
//			++m_ppkIndex;
//			return *this;
//		}
//
//		const_iterator operator++(int)
//		{
//			FV_ASSERT(m_ppkIndex);
//			const_iterator kTmp = *this;
//			++m_ppkIndex;
//			return kTmp;
//		}
//
//		const_iterator& operator--()
//		{
//			FV_ASSERT(m_ppkIndex);
//			--m_ppkIndex;
//			return *this;
//		}
//
//		const_iterator operator--(int)
//		{
//			FV_ASSERT(m_ppkIndex);
//			const_iterator kTmp = *this;
//			--m_ppkIndex;
//			return kTmp;
//		}
//
//		const_iterator& operator+=(difference_type iOff)
//		{
//			FV_ASSERT(m_ppkIndex);
//			m_ppkIndex += iOff;
//			return *this;
//		}
//
//		const_iterator operator+(difference_type iOff) const
//		{
//			const_iterator kTmp = *this;
//			return (kTmp += iOff);
//		}
//
//		const_iterator& operator-=(difference_type iOff)
//		{
//			return (*this += -iOff);
//		}
//
//		const_iterator operator-(difference_type iOff) const
//		{
//			const_iterator kTmp = *this;
//			return (kTmp += iOff);
//		}
//
//		difference_type operator-(const const_iterator& kRight) const
//		{
//			FV_ASSERT(m_ppkIndex && kRight.m_ppkIndex);
//			return difference_type(m_ppkIndex - kRight.m_ppkIndex);
//		}
//
//		bool operator==(const const_iterator& kRight) const
//		{
//			return m_ppkIndex == kRight.m_ppkIndex;
//		}
//
//		bool operator!=(const const_iterator& kRight) const
//		{
//			return m_ppkIndex != kRight.m_ppkIndex;
//		}
//
//		bool operator<(const const_iterator& kRight) const
//		{
//			FV_ASSERT(m_ppkIndex && kRight.m_ppkIndex);
//			return m_ppkIndex < kRight.m_ppkIndex;
//		}
//
//		bool operator>(const const_iterator& kRight) const
//		{
//			FV_ASSERT(m_ppkIndex && kRight.m_ppkIndex);
//			return m_ppkIndex > kRight.m_ppkIndex;
//		}
//
//		bool operator<=(const const_iterator& kRight) const
//		{
//			FV_ASSERT(m_ppkIndex && kRight.m_ppkIndex);
//			return m_ppkIndex <= kRight.m_ppkIndex;
//		}
//
//		bool operator>=(const const_iterator& kRight) const
//		{
//			FV_ASSERT(m_ppkIndex && kRight.m_ppkIndex);
//			return m_ppkIndex >= kRight.m_ppkIndex;
//		}
//
//	protected:
//		FT**	m_ppkIndex;
//	};
//
//	class iterator : public const_iterator
//	{
//	public:
//		friend class FvIndexVector;
//
//		iterator():const_iterator() {}
//		iterator(FT** ppkIndex):const_iterator(ppkIndex) {}
//
//		FT& operator*() const
//		{
//			FV_ASSERT(m_ppkIndex);
//			return **m_ppkIndex;
//		}
//
//		FT* operator->() const
//		{
//			FV_ASSERT(m_ppkIndex);
//			return *m_ppkIndex;
//		}
//
//		FT& operator[](difference_type iOff) const
//		{
//			FV_ASSERT(m_ppkIndex);
//			return **(m_ppkIndex+iOff);
//		}
//
//		iterator& operator++()
//		{
//			FV_ASSERT(m_ppkIndex);
//			++m_ppkIndex;
//			return *this;
//		}
//
//		iterator operator++(int)
//		{
//			FV_ASSERT(m_ppkIndex);
//			iterator kTmp = *this;
//			++m_ppkIndex;
//			return kTmp;
//		}
//
//		iterator& operator--()
//		{
//			FV_ASSERT(m_ppkIndex);
//			--m_ppkIndex;
//			return *this;
//		}
//
//		iterator operator--(int)
//		{
//			FV_ASSERT(m_ppkIndex);
//			iterator kTmp = *this;
//			--m_ppkIndex;
//			return kTmp;
//		}
//
//		iterator& operator+=(difference_type iOff)
//		{
//			FV_ASSERT(m_ppkIndex);
//			m_ppkIndex += iOff;
//			return *this;
//		}
//
//		iterator operator+(difference_type iOff) const
//		{
//			iterator kTmp = *this;
//			return (kTmp += iOff);
//		}
//
//		iterator& operator-=(difference_type iOff)
//		{
//			return (*this += -iOff);
//		}
//
//		iterator operator-(difference_type iOff) const
//		{
//			iterator kTmp = *this;
//			return (kTmp += iOff);
//		}
//
//		difference_type operator-(const iterator& kRight) const
//		{
//			FV_ASSERT(m_ppkIndex && kRight.m_ppkIndex);
//			return difference_type(m_ppkIndex - kRight.m_ppkIndex);
//		}
//
//		bool operator==(const iterator& kRight) const
//		{
//			return m_ppkIndex == kRight.m_ppkIndex;
//		}
//
//		bool operator!=(const iterator& kRight) const
//		{
//			return m_ppkIndex != kRight.m_ppkIndex;
//		}
//
//		bool operator<(const iterator& kRight) const
//		{
//			FV_ASSERT(m_ppkIndex && kRight.m_ppkIndex);
//			return m_ppkIndex < kRight.m_ppkIndex;
//		}
//
//		bool operator>(const iterator& kRight) const
//		{
//			FV_ASSERT(m_ppkIndex && kRight.m_ppkIndex);
//			return m_ppkIndex > kRight.m_ppkIndex;
//		}
//
//		bool operator<=(const iterator& kRight) const
//		{
//			FV_ASSERT(m_ppkIndex && kRight.m_ppkIndex);
//			return m_ppkIndex <= kRight.m_ppkIndex;
//		}
//
//		bool operator>=(const iterator& kRight) const
//		{
//			FV_ASSERT(m_ppkIndex && kRight.m_ppkIndex);
//			return m_ppkIndex >= kRight.m_ppkIndex;
//		}
//	};
//
//	//! 接口
//	bool empty() const				{ return !m_uiSize; }
//	size_type size() const			{ return m_uiSize; }
//	size_type capacity() const		{ return m_uiCapacity; }
//	size_type max_size() const		{ return size_type(~0); }
//
//	iterator begin()				{ return iterator(m_ppkIndex); }
//	const_iterator begin() const	{ return const_iterator(m_ppkIndex); }
//	iterator end()					{ return iterator(m_ppkIndex + m_uiSize); }
//	const_iterator end() const		{ return const_iterator(m_ppkIndex + m_uiSize); }
//
//	FT& front()						{ FV_ASSERT(m_uiSize && m_ppkIndex) return **m_ppkIndex; }
//	const FT& front() const			{ FV_ASSERT(m_uiSize && m_ppkIndex) return **m_ppkIndex; }
//	FT& back()						{ FV_ASSERT(m_uiSize && m_ppkIndex) return **(m_ppkIndex+m_uiSize-1); }
//	const FT& back() const			{ FV_ASSERT(m_uiSize && m_ppkIndex) return **(m_ppkIndex+m_uiSize-1); }
//
//	FvUInt32 PointerToIndex(FT* pkVal)
//	{
//		FV_ASSERT(pkVal);
//		return _GetID(pkVal);
//	}
//
//	void reserve(size_type uiCount)
//	{
//		if(uiCount < m_uiSize)
//			uiCount = m_uiSize;
//		_ReserveIndex(uiCount);
//	}
//
//	const FT& at(size_type uiPos) const { FV_ASSERT(uiPos<m_uiSize && m_ppkIndex) return **(m_ppkIndex+uiPos); }
//	FT& at(size_type uiPos)			{ FV_ASSERT(uiPos<m_uiSize && m_ppkIndex) return **(m_ppkIndex+uiPos); }
//
//	const FT& operator[](size_type uiPos) const { FV_ASSERT(uiPos<m_uiSize && m_ppkIndex) return **(m_ppkIndex+uiPos); }
//	FT& operator[](size_type uiPos)	{ FV_ASSERT(uiPos<m_uiSize && m_ppkIndex) return **(m_ppkIndex+uiPos); }
//
//	void resize(size_type uiNewsize)
//	{
//		if(m_uiSize < uiNewsize)
//		{
//			_ReserveIndex(uiNewsize);
//
//			FT** ppkIndex = m_ppkIndex + m_uiSize;
//			FT** ppkEnd = m_ppkIndex + uiNewsize;
//			FvUInt32 uiID(m_uiSize);
//			for(; ppkIndex<ppkEnd; ++ppkIndex, ++uiID)
//			{
//				*ppkIndex = _Create(uiID);
//			}
//		}
//		else if(uiNewsize < m_uiSize)
//		{
//			FT** ppkIndex = m_ppkIndex + uiNewsize;
//			FT** ppkEnd = m_ppkIndex + m_uiSize;
//			for(; ppkIndex<ppkEnd; ++ppkIndex)
//			{
//				_Destroy(*ppkIndex);
//			}
//
//			_ReserveIndex(uiNewsize);
//		}
//
//		m_uiSize = uiNewsize;
//	}
//
//	void resize(size_type uiNewsize, const FT& kVal)
//	{
//		if(m_uiSize < uiNewsize)
//		{
//			_ReserveIndex(uiNewsize);
//
//			FT** ppkIndex = m_ppkIndex + m_uiSize;
//			FT** ppkEnd = m_ppkIndex + uiNewsize;
//			FvUInt32 uiID(m_uiSize);
//			for(; ppkIndex<ppkEnd; ++ppkIndex, ++uiID)
//			{
//				*ppkIndex = _Create(uiID, kVal);
//			}
//		}
//		else if(uiNewsize < m_uiSize)
//		{
//			FT** ppkIndex = m_ppkIndex + uiNewsize;
//			FT** ppkEnd = m_ppkIndex + m_uiSize;
//			for(; ppkIndex<ppkEnd; ++ppkIndex)
//			{
//				_Destroy(*ppkIndex);
//			}
//
//			_ReserveIndex(uiNewsize);
//		}
//
//		m_uiSize = uiNewsize;
//	}
//
//	void assign(size_type uiCount, const FT& kVal)
//	{
//		resize(uiCount, kVal);
//	}
//
//	void clear()
//	{
//		FT** ppkIndex = m_ppkIndex;
//		FT** ppkEnd = m_ppkIndex + m_uiSize;
//		for(; ppkIndex<ppkEnd; ++ppkIndex)
//		{
//			_Destroy(*ppkIndex);
//		}
//
//		m_uiSize = 0;
//	}
//
//	iterator erase(const_iterator kWhere)
//	{
//		FT** ppkIndex = kWhere.m_ppkIndex;
//		FV_ASSERT(ppkIndex);
//		FvUInt32 uiID = ppkIndex - m_ppkIndex;
//		FV_ASSERT(0 <= uiID && uiID < m_uiSize && uiID == _GetID(*ppkIndex));
//
//		_Destroy(*ppkIndex);
//
//		FT** ppkEnd = m_ppkIndex + m_uiSize -1;
//		for(; ppkIndex<ppkEnd; ++ppkIndex, ++uiID)
//		{
//			*ppkIndex = *(ppkIndex + 1);
//			_SetID(*ppkIndex, uiID);
//		}
//
//		--m_uiSize;
//
//		return kWhere.m_ppkIndex;
//	}
//
//	void pop_back()
//	{
//		FV_ASSERT(m_uiSize);
//		_Destroy(*(m_ppkIndex + m_uiSize -1));
//		--m_uiSize;
//	}
//
//	void push_back()
//	{
//		FV_ASSERT(m_uiSize <= m_uiCapacity);
//		if(m_uiSize == m_uiCapacity)
//		{
//			size_type uiNewCapacity = m_uiCapacity + (m_uiCapacity >> 1);
//			if(uiNewCapacity == m_uiCapacity)
//				uiNewCapacity = m_uiCapacity +1;
//			_ReserveIndex(uiNewCapacity);			
//		}
//
//		*(m_ppkIndex + m_uiSize) = _Create(m_uiSize);
//		++m_uiSize;
//	}
//
//	void push_back(const FT& kVal)
//	{
//		FV_ASSERT(m_uiSize <= m_uiCapacity);
//		if(m_uiSize == m_uiCapacity)
//		{
//			size_type uiNewCapacity = m_uiCapacity + (m_uiCapacity >> 1);
//			if(uiNewCapacity == m_uiCapacity)
//				uiNewCapacity = m_uiCapacity +1;
//			_ReserveIndex(uiNewCapacity);			
//		}
//
//		*(m_ppkIndex + m_uiSize) = _Create(m_uiSize, kVal);
//		++m_uiSize;
//	}
///**
//	FvIndexVector& operator=(const STVector& kVec)
//	{
//		return *this;
//	}
//
//	FvIndexVector& operator=(const FvIndexVector& kVec)
//	{
//		return *this;
//	}
//
//	iterator insert(const_iterator kWhere, const ST& kVal)
//	{
//		return begin();
//	}
//
//	iterator insert(const_iterator kWhere, const FT& kVal)
//	{
//		return begin();
//	}
//
//	void swap(FvIndexVector& kVec)
//	{
//
//	}
//**/
//protected:
//	FT* _Create(FvUInt32 uiID)
//	{
//		char* pkNode = _GetNode();
//		*(FvUInt32*)pkNode = uiID;
//		return ::new(pkNode + 4)FT();
//	}
//
//	FT* _Create(FvUInt32 uiID, const FT& kVal)
//	{
//		char* pkNode = _GetNode();
//		*(FvUInt32*)pkNode = uiID;
//		return ::new(pkNode + 4)FT(kVal);
//	}
//
//	void _Destroy(FT* pkVal)
//	{
//		pkVal->~FT();
//		FreeNode* pkNode = (FreeNode*)((char*)pkVal - 4);
//		pkNode->m_pkNex = m_pkFreeNode;
//		m_pkFreeNode = pkNode;
//	}
//
//	char* _GetNode()
//	{
//		if(m_pkFreeNode)
//		{
//			char* pkNode = (char*)m_pkFreeNode;
//			m_pkFreeNode = m_pkFreeNode->m_pkNex;
//			return pkNode;
//		}
//		else
//		{
//			return new char[sizeof(FT) +4];
//		}
//	}
//
//	void _ReserveIndex(size_type uiSize)
//	{
//		FvUInt32 uiByteCnt = sizeof(FT*) * uiSize;
//		m_ppkIndex = (FT**)realloc(m_ppkIndex, uiByteCnt);
//		m_uiCapacity = uiSize;
//	}
//
//	FvUInt32 _GetID(FT* pkVal)
//	{
//		return *(FvUInt32*)((char*)pkVal - 4);
//	}
//
//	void _SetID(FT* pkVal, FvUInt32 uiID)
//	{
//		*(FvUInt32*)((char*)pkVal - 4) = uiID;
//	}
//
//protected:
//	struct FreeNode
//	{
//		FreeNode*	m_pkNex;
//	};
//
//	size_type	m_uiSize;
//	size_type	m_uiCapacity;
//	FT**		m_ppkIndex;
//	FreeNode*	m_pkFreeNode;
//};


class Demo;

const DWORD _BitMasksX[] = {
	0x1, 0x0, 0x4, 0x0, 0x10, 0x0, 0x40, 0x0,
	0x100, 0x0, 0x400, 0x0, 0x1000, 0x0, 0x4000, 0x0,
	0x10000, 0x0, 0x40000, 0x0, 0x100000, 0x0, 0x400000, 0x0,
	0x1000000, 0x0, 0x4000000, 0x0, 0x10000000, 0x0, 0x40000000, 0x0
};

const DWORD _BitMasksY[] = {
	0x2, 0x0, 0x8, 0x0, 0x20, 0x0, 0x80, 0x0,
	0x200, 0x0, 0x800, 0x0, 0x2000, 0x0, 0x8000, 0x0,
	0x20000, 0x0, 0x80000, 0x0, 0x200000, 0x0, 0x800000, 0x0,
	0x2000000, 0x0, 0x8000000, 0x0, 0x20000000, 0x0, 0x80000000, 0x0
};

const DWORD _BitMasks[] = {
	0x3, 0x0, 0xC, 0x0, 0x30, 0x0, 0xC0, 0x0,
	0x300, 0x0, 0xC00, 0x0, 0x3000, 0x0, 0xC000, 0x0,
	0x30000, 0x0, 0xC0000, 0x0, 0x300000, 0x0, 0xC00000, 0x0,
	0x3000000, 0x0, 0xC000000, 0x0, 0x30000000, 0x0, 0xC0000000, 0x0
};

const DWORD _NegBitMasksX[] = {
	0xFFFFFFFE, 0xFFFFFFFF, 0xFFFFFFFB, 0xFFFFFFFF, 0xFFFFFFEF, 0xFFFFFFFF, 0xFFFFFFBF, 0xFFFFFFFF,
	0xFFFFFEFF, 0xFFFFFFFF, 0xFFFFFBFF, 0xFFFFFFFF, 0xFFFFEFFF, 0xFFFFFFFF, 0xFFFFBFFF, 0xFFFFFFFF,
	0xFFFEFFFF, 0xFFFFFFFF, 0xFFFBFFFF, 0xFFFFFFFF,	0xFFEFFFFF, 0xFFFFFFFF, 0xFFBFFFFF, 0xFFFFFFFF,
	0xFEFFFFFF, 0xFFFFFFFF, 0xFBFFFFFF, 0xFFFFFFFF, 0xEFFFFFFF, 0xFFFFFFFF, 0xBFFFFFFF, 0xFFFFFFFF
};

const DWORD _NegBitMasksY[] = {
	0xFFFFFFFD, 0xFFFFFFFF, 0xFFFFFFF7, 0xFFFFFFFF, 0xFFFFFFDF, 0xFFFFFFFF, 0xFFFFFF7F, 0xFFFFFFFF,
	0xFFFFFDFF, 0xFFFFFFFF, 0xFFFFF7FF, 0xFFFFFFFF, 0xFFFFDFFF, 0xFFFFFFFF, 0xFFFF7FFF, 0xFFFFFFFF,
	0xFFFDFFFF, 0xFFFFFFFF, 0xFFF7FFFF,	0xFFFFFFFF, 0xFFDFFFFF, 0xFFFFFFFF, 0xFF7FFFFF, 0xFFFFFFFF,
	0xFDFFFFFF, 0xFFFFFFFF, 0xF7FFFFFF, 0xFFFFFFFF, 0xDFFFFFFF, 0xFFFFFFFF, 0x7FFFFFFF, 0xFFFFFFFF
};

const DWORD _NegBitMasks[] = {
	0xFFFFFFFC, 0xFFFFFFFF, 0xFFFFFFF3, 0xFFFFFFFF, 0xFFFFFFCF, 0xFFFFFFFF, 0xFFFFFF3F, 0xFFFFFFFF,
	0xFFFFFCFF, 0xFFFFFFFF, 0xFFFFF3FF, 0xFFFFFFFF, 0xFFFFCFFF, 0xFFFFFFFF, 0xFFFF3FFF, 0xFFFFFFFF,
	0xFFFCFFFF, 0xFFFFFFFF, 0xFFF3FFFF,	0xFFFFFFFF, 0xFFCFFFFF, 0xFFFFFFFF, 0xFF3FFFFF, 0xFFFFFFFF,
	0xFCFFFFFF, 0xFFFFFFFF, 0xF3FFFFFF, 0xFFFFFFFF, 0xCFFFFFFF, 0xFFFFFFFF, 0x3FFFFFFF, 0xFFFFFFFF
};

inline void p2Swap(unsigned short& a, unsigned short& b)
{
	unsigned short c = a;
	a = b;
	b = c;
}

class p2SquareSymmetricMatrix
{
public:
	p2SquareSymmetricMatrix();
	p2SquareSymmetricMatrix(WORD size);
	~p2SquareSymmetricMatrix();

	bool			Init(WORD size);

	inline	void	SetBitX(WORD idx1, WORD idx2);
	inline	bool	SetBitXRet(WORD idx1, WORD idx2);
	inline	void	SetBitY(WORD idx1, WORD idx2);
	inline	bool	SetBitYRet(WORD idx1, WORD idx2);
	inline	void	ClearBit(WORD idx1, WORD idx2);
	inline	bool	ClearBitX(WORD idx1, WORD idx2);
	inline	bool	ClearBitY(WORD idx1, WORD idx2);

	inline	void	ClearAll() { memset(m_pBits, 0, m_Size*sizeof(DWORD)); }

protected:
	inline	DWORD	Index(WORD idx1, WORD idx2);

protected:
	DWORD*			m_pBits;
	DWORD			m_Size;

};

inline void	p2SquareSymmetricMatrix::SetBitX(WORD idx1, WORD idx2)
{
	DWORD idx=Index(idx1,idx2);
	m_pBits[idx>>5] |= _BitMasksX[idx&31];
}

inline bool	p2SquareSymmetricMatrix::SetBitXRet(WORD idx1, WORD idx2)
{
	DWORD idx=Index(idx1,idx2);
	m_pBits[idx>>5] |= _BitMasksX[idx&31];
	return (m_pBits[idx>>5] & _BitMasks[idx&31]) != 0;
}

inline void	p2SquareSymmetricMatrix::SetBitY(WORD idx1, WORD idx2)
{
	DWORD idx=Index(idx1,idx2);
	m_pBits[idx>>5] |= _BitMasksY[idx&31];
}

inline bool	p2SquareSymmetricMatrix::SetBitYRet(WORD idx1, WORD idx2)
{
	DWORD idx=Index(idx1,idx2);
	m_pBits[idx>>5] |= _BitMasksY[idx&31];
	return (m_pBits[idx>>5] & _BitMasks[idx&31]) != 0;
}

inline void	p2SquareSymmetricMatrix::ClearBit(WORD idx1, WORD idx2)
{
	DWORD idx=Index(idx1,idx2);
	m_pBits[idx>>5] &= _NegBitMasks[idx&31];
}

inline bool	p2SquareSymmetricMatrix::ClearBitX(WORD idx1, WORD idx2)
{
	DWORD idx=Index(idx1,idx2);
	bool bRet = (m_pBits[idx>>5] & _BitMasks[idx&31]) != 0;
	m_pBits[idx>>5] &= _NegBitMasksX[idx&31];
	return bRet;
}

inline bool	p2SquareSymmetricMatrix::ClearBitY(WORD idx1, WORD idx2)
{
	DWORD idx=Index(idx1,idx2);
	bool bRet = (m_pBits[idx>>5] & _BitMasks[idx&31]) != 0;
	m_pBits[idx>>5] &= _NegBitMasksY[idx&31];
	return bRet;
}

inline DWORD p2SquareSymmetricMatrix::Index(WORD idx1, WORD idx2)
{
	if(idx1>idx2)
		p2Swap(idx1,idx2);
	return (idx1<<1) + ((idx2-1)*idx2);
}

#include <hash_map>
#include <map>

class hash_A
{
public:
	enum
	{
		bucket_size = 1,	//   0   <   bucket_size
		min_buckets = 256	//   min_buckets   =   2   ^^   N,   0   <   N
	};

	hash_A() {}

	unsigned int operator()(const unsigned int& Key ) const
	{
		return Key;
	}

	bool operator()(const unsigned int& _Key1, const unsigned int& _Key2) const
	{
		return _Key1 < _Key2;
	}
};

void InitMyMemAlloc(size_t cnt);
void UnInitMyMemAlloc();
void* MyMalloc(size_t size, size_t cnt);
void MyFree(void* p, size_t cnt);

template
<
typename T
>
class STLAllocator 
{
public :
	/// define our types, as per ISO C++
	typedef T					value_type;
	typedef value_type*			pointer;
	typedef const value_type*	const_pointer;
	typedef value_type&			reference;
	typedef const value_type&	const_reference;
	typedef std::size_t			size_type;
	typedef std::ptrdiff_t		difference_type;


	/// the standard rebind mechanism
	template<typename U>
	struct rebind
	{
		typedef STLAllocator<U> other;
	};

	/// ctor
	inline explicit STLAllocator()
	{ }

	/// dtor
	virtual ~STLAllocator()
	{ }

	/// copy ctor - done component wise
	inline STLAllocator( STLAllocator const& rhs )
	{ }

	/// cast
	template <typename U>
	inline STLAllocator( STLAllocator<U> const& )
	{ }


	/// memory allocation (elements, used by STL)
	inline pointer allocate( size_type count,
		typename std::allocator<void>::const_pointer ptr = 0 )
	{
		//if(count != 1 || sizeof( T ) != sizeof(std::pair<unsigned int, void*>))
		//{
		//	printf(">>>>>>> %d:%d\n", count, sizeof( T ));
		//}
		// convert request to bytes
		//register size_type sz = count*sizeof( T );
		//pointer p  = static_cast<pointer>((void*)new char[sz]);
		return static_cast<pointer>(MyMalloc(sizeof(T), count));
	}

	/// memory deallocation (elements, used by STL)
	inline void deallocate( pointer ptr, size_type count )
	{
		//if(count != 1 || sizeof( T ) != sizeof(std::pair<unsigned int, void*>))
		//{
		//	printf("<<<<<<< %d:%d\n", count, sizeof( T ));
		//}
		// convert request to bytes, but we can't use this?
		//register size_type sz = count*sizeof( T );
		//delete [] (char*)ptr;
		MyFree(ptr, count);
	}

	pointer address(reference x) const
	{
		return &x;
	}

	const_pointer address(const_reference x) const
	{
		return &x;
	}

	size_type max_size() const throw()
	{
		// maximum size this can handle, delegate
		return 0xFFFFFFFF;
	}

	void construct(pointer p, const T& val)
	{
		// call placement new
		new(static_cast<void*>(p)) T(val);
	}

	void destroy(pointer p)
	{
		// do we have to protect against non-classes here?
		// some articles suggest yes, some no
		p->~T();
	}

};

/// determine equality, can memory from another allocator
/// be released by this allocator, (ISO C++)
template<typename T, typename T2>
inline bool operator==(STLAllocator<T> const&,
					   STLAllocator<T2> const&)
{
	// same alloc policy (P), memory can be freed
	return true;
}

/// determine equality, can memory from another allocator
/// be released by this allocator, (ISO C++)
template<typename T, typename OtherAllocator>
inline bool operator==(STLAllocator<T> const&,
					   OtherAllocator const&)
{
	return false;
}
/// determine equality, can memory from another allocator
/// be released by this allocator, (ISO C++)
template<typename T, typename T2>
inline bool operator!=(STLAllocator<T> const&,
					   STLAllocator<T2> const&)
{
	// same alloc policy (P), memory can be freed
	return false;
}

/// determine equality, can memory from another allocator
/// be released by this allocator, (ISO C++)
template<typename T, typename OtherAllocator>
inline bool operator!=(STLAllocator<T> const&,
					   OtherAllocator const&)
{
	return true;
}


struct RelateNode
{
	int objectID;
	void* pVoid;
	RelateNode*	pPre, * pNex;
	//unsigned short observerID, objectID;
};

#include "FvAoIMgr.h"

struct MyObs
{
	int			objID, obsID;
	FvAoIHandle	objHandle, obsHandle;
	float		x,y;
	float		vx,vy;
};

struct MyObj
{
	int			id;
	FvAoIHandle	handle;
	float		x,y;
};

struct MyAoIExt : public FvAoIExt
{
	int		id;

	void Create(void* pkObs, void* pkObj)
	{
		id = ((MyObj*)pkObj)->id;
	}

	void Destroy()
	{

	}
};

struct MyTrapExt : public FvAoIExt
{
	int		id;

	void Create(void* pkObs, void* pkObj)
	{
		id = ((MyObj*)pkObj)->id;
	}

	void Destroy()
	{

	}
};

class MyApp : public App
{
public:
	MyApp();
	virtual~MyApp();

	virtual	bool	Init(int argc, char* argv[]);
	virtual	void	Tick();
	virtual	void	Draw();

	virtual	void	OnKeyDown(int keyVal, bool bRep);
	virtual	void	OnKeyUp(int keyVal) {}
	virtual	void	OnCmd(const char* pcCmd) {}

protected:

protected:
	Demo*			m_pkDemo;
	bool			m_bRunStep;
	std::vector<float>	m_kMins, m_kMaxs;

	p2SquareSymmetricMatrix	m_kMatrix;
	int				m_iMatrixSize;
	int				m_iTestCnt;
	WORD*			m_pTestData;

	int				m_iHashTestCnt;
	//stdext::hash_map<unsigned int, void*, hash_A, STLAllocator<std::pair<unsigned int, void*>>>* m_pkMap;
	std::map<unsigned int, void*, std::less<unsigned int>, STLAllocator<std::pair<unsigned int, void*>>>* m_pkMap;
	unsigned int*	m_pHashKey;
	unsigned int*	m_pHashKey1;
	bool			m_bUse1;
	//stdext::hash_map<unsigned int, void*, hash_A, STLAllocator<std::pair<unsigned int, void*>>>::iterator* m_pMapItr;

	int				m_iNodeCnt;
	unsigned short*	m_pIDs;
	RelateNode		m_kNodeHead;
	RelateNode		m_kNodeTail;
	RelateNode*		FindNode(unsigned short id);
	int				TraversalList();
	int				TraversalArray();
	int				TraversalArray1();
	RelateNode*		m_pkNodes;
	int*			m_pkArray;

	const static FvUInt32	uiCNT = 1000;
	//FvRefList<int>*	kList;
	//FvRefNode<int>*	kNodeList;
	int				TestRefList();
	void			MyTick();

	FvIndexVector<int>	m_kMyIntArray;
	int				TraversalMyArray();
	int				TraversalMyArray1();
	int				TraversalMyArray2();
	int				TraversalMyArray3();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	float							m_GridMinX,m_GridMinY,m_GridMaxX,m_GridMaxY;
	FvAoIMgr<MyAoIExt, MyTrapExt>	m_kGrid;
	int								m_iObsCnt;
	int								m_iPlayerCnt;
	int								m_iMoveCnt;
	MyObs*							m_pkObs;
	float							m_fVision;
	float							m_fVision1;
	bool							m_bAdd;
	float							m_fVel;

	//static const int				m_iObjCnt = 10000;
	//MyObj							m_kObj[m_iObjCnt];
	//MyObs							m_kObs;
	int								m_ShowAoI;
	bool							m_bShowID;
	bool							m_bShow;
	bool							m_bShowLog;
	bool							m_bShowGrid;
	bool							m_bSetMask;

	//FvUInt32*						m_pkValues;
};


App* CreateApp();
void DestroyApp();