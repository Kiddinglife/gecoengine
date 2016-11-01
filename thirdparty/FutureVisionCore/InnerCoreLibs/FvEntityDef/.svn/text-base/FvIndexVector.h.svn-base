//{future header message}
#ifndef __FvIndexVector_H__
#define __FvIndexVector_H__

#include <vector>
#include <FvPowerDefines.h>


//! 索引数组
template<class FT, class ST>
class FvIndexVector
{
public:
	//! 类型声明
	typedef FvUInt32 size_type;
	typedef FvInt32 difference_type;
	typedef std::vector<ST> STVector;


	//! 构造函数
	FvIndexVector()
	:m_uiSize(0),m_uiCapacity(0)
	,m_ppkIndex(NULL),m_pkFreeNode(NULL)
	{
	}

	FvIndexVector(const STVector& kVec)
	:m_uiSize(0),m_uiCapacity(0)
	,m_ppkIndex(NULL),m_pkFreeNode(NULL)
	{
		if(kVec.empty())
			return;

		m_uiSize = size_type(kVec.size());
		_ReserveIndex(m_uiSize);

		FT** ppkIndex = m_ppkIndex;
		FvUInt32 uiID(0);
		FvUInt32 uiSize = FvUInt32(kVec.size());
		for(; uiID<uiSize; ++uiID, ++ppkIndex)
		{
			*ppkIndex = _Create(uiID, kVec[uiID]);
		}
	}

	FvIndexVector(const FvIndexVector& kVec)
	:m_uiSize(0),m_uiCapacity(0)
	,m_ppkIndex(NULL),m_pkFreeNode(NULL)
	{
		if(kVec.empty())
			return;

		m_uiSize = size_type(kVec.size());
		_ReserveIndex(m_uiSize);

		FT** ppkIndex = m_ppkIndex;
		FvUInt32 uiID(0);
		const_iterator itrB = kVec.begin();
		const_iterator itrE = kVec.end();
		for(; itrB!=itrE; ++itrB,
			++uiID, ++ppkIndex)
		{
			*ppkIndex = _Create(uiID, *itrB);
		}
	}

	//! 析构函数
	~FvIndexVector()
	{
		FT** ppkIndex = m_ppkIndex;
		FT** ppkEnd = m_ppkIndex + m_uiSize;
		FT* pkVal;
		for(; ppkIndex<ppkEnd; ++ppkIndex)
		{
			pkVal = *ppkIndex;
			pkVal->~FT();
			delete [] ((char*)pkVal - 4);
		}

		char* pkNode;
		while(m_pkFreeNode)
		{
			pkNode = (char*)m_pkFreeNode;
			m_pkFreeNode = m_pkFreeNode->m_pkNex;
			delete [] pkNode;
		}

		free(m_ppkIndex);
	}

	//! 类型转换
	operator STVector() const
	{
		STVector kVec;
		kVec.reserve(m_uiSize);

		FT** ppkIndex = m_ppkIndex;
		FT** ppkEnd = m_ppkIndex + m_uiSize;
		for(; ppkIndex<ppkEnd; ++ppkIndex)
		{
			kVec.push_back(**ppkIndex);
		}
		return kVec;
	}

	//! 迭代器
	class const_iterator
	{
	public:
		friend class FvIndexVector;

		const_iterator():m_ppkIndex(NULL) {}
		const_iterator(FT** ppkIndex):m_ppkIndex(ppkIndex) {}

		const FT& operator*() const
		{
			FV_ASSERT(m_ppkIndex);
			return **m_ppkIndex;
		}

		const FT* operator->() const
		{
			FV_ASSERT(m_ppkIndex);
			return *m_ppkIndex;
		}

		const FT& operator[](difference_type iOff) const
		{
			FV_ASSERT(m_ppkIndex);
			return **(m_ppkIndex+iOff);
		}

		const_iterator& operator++()
		{
			FV_ASSERT(m_ppkIndex);
			++m_ppkIndex;
			return *this;
		}

		const_iterator operator++(int)
		{
			FV_ASSERT(m_ppkIndex);
			const_iterator kTmp = *this;
			++m_ppkIndex;
			return kTmp;
		}

		const_iterator& operator--()
		{
			FV_ASSERT(m_ppkIndex);
			--m_ppkIndex;
			return *this;
		}

		const_iterator operator--(int)
		{
			FV_ASSERT(m_ppkIndex);
			const_iterator kTmp = *this;
			--m_ppkIndex;
			return kTmp;
		}

		const_iterator& operator+=(difference_type iOff)
		{
			if(iOff == 0) return *this;
			FV_ASSERT(m_ppkIndex);
			m_ppkIndex += iOff;
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
			FV_ASSERT(m_ppkIndex && kRight.m_ppkIndex);
			return difference_type(m_ppkIndex - kRight.m_ppkIndex);
		}

		bool operator==(const const_iterator& kRight) const
		{
			return m_ppkIndex == kRight.m_ppkIndex;
		}

		bool operator!=(const const_iterator& kRight) const
		{
			return m_ppkIndex != kRight.m_ppkIndex;
		}

		bool operator<(const const_iterator& kRight) const
		{
			FV_ASSERT(m_ppkIndex && kRight.m_ppkIndex);
			return m_ppkIndex < kRight.m_ppkIndex;
		}

		bool operator>(const const_iterator& kRight) const
		{
			FV_ASSERT(m_ppkIndex && kRight.m_ppkIndex);
			return m_ppkIndex > kRight.m_ppkIndex;
		}

		bool operator<=(const const_iterator& kRight) const
		{
			FV_ASSERT(m_ppkIndex && kRight.m_ppkIndex);
			return m_ppkIndex <= kRight.m_ppkIndex;
		}

		bool operator>=(const const_iterator& kRight) const
		{
			FV_ASSERT(m_ppkIndex && kRight.m_ppkIndex);
			return m_ppkIndex >= kRight.m_ppkIndex;
		}

	protected:
		FT**	m_ppkIndex;
	};

	class iterator : public const_iterator
	{
	public:
		friend class FvIndexVector;

		iterator():const_iterator() {}
		iterator(FT** ppkIndex):const_iterator(ppkIndex) {}

		FT& operator*() const
		{
			FV_ASSERT(m_ppkIndex);
			return **m_ppkIndex;
		}

		FT* operator->() const
		{
			FV_ASSERT(m_ppkIndex);
			return *m_ppkIndex;
		}

		FT& operator[](difference_type iOff) const
		{
			FV_ASSERT(m_ppkIndex);
			return **(m_ppkIndex+iOff);
		}

		iterator& operator++()
		{
			FV_ASSERT(m_ppkIndex);
			++m_ppkIndex;
			return *this;
		}

		iterator operator++(int)
		{
			FV_ASSERT(m_ppkIndex);
			iterator kTmp = *this;
			++m_ppkIndex;
			return kTmp;
		}

		iterator& operator--()
		{
			FV_ASSERT(m_ppkIndex);
			--m_ppkIndex;
			return *this;
		}

		iterator operator--(int)
		{
			FV_ASSERT(m_ppkIndex);
			iterator kTmp = *this;
			--m_ppkIndex;
			return kTmp;
		}

		iterator& operator+=(difference_type iOff)
		{
			if(iOff == 0) return *this;
			FV_ASSERT(m_ppkIndex);
			m_ppkIndex += iOff;
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
			FV_ASSERT(m_ppkIndex && kRight.m_ppkIndex);
			return difference_type(m_ppkIndex - kRight.m_ppkIndex);
		}

		bool operator==(const iterator& kRight) const
		{
			return m_ppkIndex == kRight.m_ppkIndex;
		}

		bool operator!=(const iterator& kRight) const
		{
			return m_ppkIndex != kRight.m_ppkIndex;
		}

		bool operator<(const iterator& kRight) const
		{
			FV_ASSERT(m_ppkIndex && kRight.m_ppkIndex);
			return m_ppkIndex < kRight.m_ppkIndex;
		}

		bool operator>(const iterator& kRight) const
		{
			FV_ASSERT(m_ppkIndex && kRight.m_ppkIndex);
			return m_ppkIndex > kRight.m_ppkIndex;
		}

		bool operator<=(const iterator& kRight) const
		{
			FV_ASSERT(m_ppkIndex && kRight.m_ppkIndex);
			return m_ppkIndex <= kRight.m_ppkIndex;
		}

		bool operator>=(const iterator& kRight) const
		{
			FV_ASSERT(m_ppkIndex && kRight.m_ppkIndex);
			return m_ppkIndex >= kRight.m_ppkIndex;
		}
	};

	//! 接口
	bool empty() const				{ return !m_uiSize; }
	size_type size() const			{ return m_uiSize; }
	size_type capacity() const		{ return m_uiCapacity; }
	size_type max_size() const		{ return size_type(~0); }

	iterator begin()				{ return iterator(m_ppkIndex); }
	const_iterator begin() const	{ return const_iterator(m_ppkIndex); }
	iterator end()					{ return iterator(m_ppkIndex + m_uiSize); }
	const_iterator end() const		{ return const_iterator(m_ppkIndex + m_uiSize); }

	FT& front()						{ FV_ASSERT(m_uiSize && m_ppkIndex) return **m_ppkIndex; }
	const FT& front() const			{ FV_ASSERT(m_uiSize && m_ppkIndex) return **m_ppkIndex; }
	FT& back()						{ FV_ASSERT(m_uiSize && m_ppkIndex) return **(m_ppkIndex+m_uiSize-1); }
	const FT& back() const			{ FV_ASSERT(m_uiSize && m_ppkIndex) return **(m_ppkIndex+m_uiSize-1); }

	FvUInt32 PointerToIndex(FT* pkVal)
	{
		FV_ASSERT(pkVal);
		return _GetID(pkVal);
	}

	void reserve(size_type uiCount)
	{
		if(uiCount < m_uiSize)
			uiCount = m_uiSize;
		_ReserveIndex(uiCount);
	}

	const FT& at(size_type uiPos) const { FV_ASSERT(uiPos<m_uiSize && m_ppkIndex) return **(m_ppkIndex+uiPos); }
	FT& at(size_type uiPos)			{ FV_ASSERT(uiPos<m_uiSize && m_ppkIndex) return **(m_ppkIndex+uiPos); }

	const FT& operator[](size_type uiPos) const { FV_ASSERT(uiPos<m_uiSize && m_ppkIndex) return **(m_ppkIndex+uiPos); }
	FT& operator[](size_type uiPos)	{ FV_ASSERT(uiPos<m_uiSize && m_ppkIndex) return **(m_ppkIndex+uiPos); }

	void resize(size_type uiNewsize)
	{
		if(m_uiSize < uiNewsize)
		{
			_ReserveIndex(uiNewsize);

			FT** ppkIndex = m_ppkIndex + m_uiSize;
			FT** ppkEnd = m_ppkIndex + uiNewsize;
			FvUInt32 uiID(m_uiSize);
			for(; ppkIndex<ppkEnd; ++ppkIndex, ++uiID)
			{
				*ppkIndex = _Create(uiID);
			}
		}
		else if(uiNewsize < m_uiSize)
		{
			FT** ppkIndex = m_ppkIndex + uiNewsize;
			FT** ppkEnd = m_ppkIndex + m_uiSize;
			for(; ppkIndex<ppkEnd; ++ppkIndex)
			{
				_Destroy(*ppkIndex);
			}

			_ReserveIndex(uiNewsize);
		}

		m_uiSize = uiNewsize;
	}

	void resize(size_type uiNewsize, const ST& kVal)
	{
		if(m_uiSize < uiNewsize)
		{
			_ReserveIndex(uiNewsize);

			FT** ppkIndex = m_ppkIndex + m_uiSize;
			FT** ppkEnd = m_ppkIndex + uiNewsize;
			FvUInt32 uiID(m_uiSize);
			for(; ppkIndex<ppkEnd; ++ppkIndex, ++uiID)
			{
				*ppkIndex = _Create(uiID, kVal);
			}
		}
		else if(uiNewsize < m_uiSize)
		{
			FT** ppkIndex = m_ppkIndex + uiNewsize;
			FT** ppkEnd = m_ppkIndex + m_uiSize;
			for(; ppkIndex<ppkEnd; ++ppkIndex)
			{
				_Destroy(*ppkIndex);
			}

			_ReserveIndex(uiNewsize);
		}

		m_uiSize = uiNewsize;
	}

	void resize(size_type uiNewsize, const FT& kVal)
	{
		if(m_uiSize < uiNewsize)
		{
			_ReserveIndex(uiNewsize);

			FT** ppkIndex = m_ppkIndex + m_uiSize;
			FT** ppkEnd = m_ppkIndex + uiNewsize;
			FvUInt32 uiID(m_uiSize);
			for(; ppkIndex<ppkEnd; ++ppkIndex, ++uiID)
			{
				*ppkIndex = _Create(uiID, kVal);
			}
		}
		else if(uiNewsize < m_uiSize)
		{
			FT** ppkIndex = m_ppkIndex + uiNewsize;
			FT** ppkEnd = m_ppkIndex + m_uiSize;
			for(; ppkIndex<ppkEnd; ++ppkIndex)
			{
				_Destroy(*ppkIndex);
			}

			_ReserveIndex(uiNewsize);
		}

		m_uiSize = uiNewsize;
	}

	void assign(size_type uiCount, const ST& kVal)
	{
		resize(uiCount, kVal);
	}

	void assign(size_type uiCount, const FT& kVal)
	{
		resize(uiCount, kVal);
	}

	void clear()
	{
		FT** ppkIndex = m_ppkIndex;
		FT** ppkEnd = m_ppkIndex + m_uiSize;
		for(; ppkIndex<ppkEnd; ++ppkIndex)
		{
			_Destroy(*ppkIndex);
		}

		m_uiSize = 0;
	}

	iterator erase(const_iterator kWhere)
	{
		FT** ppkIndex = kWhere.m_ppkIndex;
		FV_ASSERT(ppkIndex && m_ppkIndex<=ppkIndex);
		FvUInt32 uiID = ppkIndex - m_ppkIndex;
		FV_ASSERT(0 <= uiID && uiID < m_uiSize && uiID == _GetID(*ppkIndex));

		_Destroy(*ppkIndex);

		FT** ppkEnd = m_ppkIndex + m_uiSize -1;
		for(; ppkIndex<ppkEnd; ++ppkIndex, ++uiID)
		{
			*ppkIndex = *(ppkIndex + 1);
			_SetID(*ppkIndex, uiID);
		}

		--m_uiSize;

		return kWhere.m_ppkIndex;
	}

	void erase(size_type uiIndex)
	{
		FV_ASSERT(0<=uiIndex && uiIndex<m_uiSize);
		FT** ppkIndex = m_ppkIndex + uiIndex;

		_Destroy(*ppkIndex);

		FvUInt32 uiID(uiIndex);
		FT** ppkEnd = m_ppkIndex + m_uiSize -1;
		for(; ppkIndex<ppkEnd; ++ppkIndex, ++uiID)
		{
			*ppkIndex = *(ppkIndex + 1);
			_SetID(*ppkIndex, uiID);
		}

		--m_uiSize;
	}

	size_type erase_value(FT* pkVal)
	{
		size_type uiIdx = PointerToIndex(pkVal);
		FV_ASSERT(uiIdx < m_uiSize);
		FT** ppkIndex = m_ppkIndex + uiIdx;

		_Destroy(*ppkIndex);

		FvUInt32 uiID = uiIdx;
		FT** ppkEnd = m_ppkIndex + m_uiSize -1;
		for(; ppkIndex<ppkEnd; ++ppkIndex, ++uiID)
		{
			*ppkIndex = *(ppkIndex + 1);
			_SetID(*ppkIndex, uiID);
		}

		--m_uiSize;
		return uiIdx;
	}

	FT* erase_no_delete(size_type uiIndex)
	{
		FV_ASSERT(0<=uiIndex && uiIndex<m_uiSize);
		FT** ppkIndex = m_ppkIndex + uiIndex;
		FT* pkNode = *ppkIndex;

		FvUInt32 uiID(uiIndex);
		FT** ppkEnd = m_ppkIndex + m_uiSize -1;
		for(; ppkIndex<ppkEnd; ++ppkIndex, ++uiID)
		{
			*ppkIndex = *(ppkIndex + 1);
			_SetID(*ppkIndex, uiID);
		}

		--m_uiSize;
		return pkNode;
	}

	void pop_back()
	{
		FV_ASSERT(m_uiSize);
		_Destroy(*(m_ppkIndex + m_uiSize -1));
		--m_uiSize;
	}

	FT* pop_back_no_delete()
	{
		FV_ASSERT(m_uiSize);
		FT* pkNode = *(m_ppkIndex + m_uiSize -1);
		--m_uiSize;
		return pkNode;
	}

	void push_back()
	{
		FV_ASSERT(m_uiSize <= m_uiCapacity);
		if(m_uiSize == m_uiCapacity)
		{
			size_type uiNewCapacity = m_uiCapacity + (m_uiCapacity >> 1);
			if(uiNewCapacity == m_uiCapacity)
				uiNewCapacity = m_uiCapacity +1;
			_ReserveIndex(uiNewCapacity);			
		}

		*(m_ppkIndex + m_uiSize) = _Create(m_uiSize);
		++m_uiSize;
	}

	void push_back(const ST& kVal)
	{
		FV_ASSERT(m_uiSize <= m_uiCapacity);
		if(m_uiSize == m_uiCapacity)
		{
			size_type uiNewCapacity = m_uiCapacity + (m_uiCapacity >> 1);
			if(uiNewCapacity == m_uiCapacity)
				uiNewCapacity = m_uiCapacity +1;
			_ReserveIndex(uiNewCapacity);			
		}

		*(m_ppkIndex + m_uiSize) = _Create(m_uiSize, kVal);
		++m_uiSize;
	}

	void push_back(const FT& kVal)
	{
		FV_ASSERT(m_uiSize <= m_uiCapacity);
		if(m_uiSize == m_uiCapacity)
		{
			size_type uiNewCapacity = m_uiCapacity + (m_uiCapacity >> 1);
			if(uiNewCapacity == m_uiCapacity)
				uiNewCapacity = m_uiCapacity +1;
			_ReserveIndex(uiNewCapacity);			
		}

		*(m_ppkIndex + m_uiSize) = _Create(m_uiSize, kVal);
		++m_uiSize;
	}

	void DestroyNode(FT* pkVal)
	{
		FV_ASSERT(pkVal);
		_Destroy(pkVal);
	}
/**
	FvIndexVector& operator=(const STVector& kVec)
	{
		return *this;
	}

	FvIndexVector& operator=(const FvIndexVector& kVec)
	{
		return *this;
	}

	iterator insert(const_iterator kWhere, const ST& kVal)
	{
		return begin();
	}

	iterator insert(const_iterator kWhere, const FT& kVal)
	{
		return begin();
	}

	void swap(FvIndexVector& kVec)
	{

	}
**/
protected:
	FT* _Create(FvUInt32 uiID)
	{
		char* pkNode = _GetNode();
		*(FvUInt32*)pkNode = uiID;
		return ::new(pkNode + 4)FT();
	}

	FT* _Create(FvUInt32 uiID, const ST& kVal)
	{
		char* pkNode = _GetNode();
		*(FvUInt32*)pkNode = uiID;
		return ::new(pkNode + 4)FT(kVal);
	}

	FT* _Create(FvUInt32 uiID, const FT& kVal)
	{
		char* pkNode = _GetNode();
		*(FvUInt32*)pkNode = uiID;
		return ::new(pkNode + 4)FT(kVal);
	}

	void _Destroy(FT* pkVal)
	{
		pkVal->~FT();
		FreeNode* pkNode = (FreeNode*)((char*)pkVal - 4);
		pkNode->m_pkNex = m_pkFreeNode;
		m_pkFreeNode = pkNode;
	}

	char* _GetNode()
	{
		if(m_pkFreeNode)
		{
			char* pkNode = (char*)m_pkFreeNode;
			m_pkFreeNode = m_pkFreeNode->m_pkNex;
			return pkNode;
		}
		else
		{
			return new char[sizeof(FT) +4];
		}
	}

	void _ReserveIndex(size_type uiSize)
	{
		FvUInt32 uiByteCnt = sizeof(FT*) * uiSize;
		m_ppkIndex = (FT**)realloc(m_ppkIndex, uiByteCnt);
		m_uiCapacity = uiSize;
	}

	FvUInt32 _GetID(FT* pkVal)
	{
		return *(FvUInt32*)((char*)pkVal - 4);
	}

	void _SetID(FT* pkVal, FvUInt32 uiID)
	{
		*(FvUInt32*)((char*)pkVal - 4) = uiID;
	}

protected:
	struct FreeNode
	{
		FreeNode*	m_pkNex;
	};

	size_type	m_uiSize;
	size_type	m_uiCapacity;
	FT**		m_ppkIndex;
	FreeNode*	m_pkFreeNode;
};


//! FT和ST相同时的特化
template<class FT>
class FvIndexVector<FT, FT>
{
public:
	//! 类型声明
	typedef FvUInt32 size_type;
	typedef FvInt32 difference_type;
	typedef std::vector<FT> FTVector;


	//! 构造函数
	FvIndexVector()
	:m_uiSize(0),m_uiCapacity(0)
	,m_ppkIndex(NULL),m_pkFreeNode(NULL)
	{
	}

	FvIndexVector(const FTVector& kVec)
	:m_uiSize(0),m_uiCapacity(0)
	,m_ppkIndex(NULL),m_pkFreeNode(NULL)
	{
		if(kVec.empty())
			return;

		m_uiSize = size_type(kVec.size());
		_ReserveIndex(m_uiSize);

		FT** ppkIndex = m_ppkIndex;
		FvUInt32 uiID(0);
		FTVector::const_iterator itrB = kVec.begin();
		FTVector::const_iterator itrE = kVec.end();
		for(; itrB!=itrE; ++itrB,
			++uiID, ++ppkIndex)
		{
			*ppkIndex = _Create(uiID, *itrB);
		}
	}

	FvIndexVector(const FvIndexVector& kVec)
	:m_uiSize(0),m_uiCapacity(0)
	,m_ppkIndex(NULL),m_pkFreeNode(NULL)
	{
		if(kVec.empty())
			return;

		m_uiSize = size_type(kVec.size());
		_ReserveIndex(m_uiSize);

		FT** ppkIndex = m_ppkIndex;
		FvUInt32 uiID(0);
		const_iterator itrB = kVec.begin();
		const_iterator itrE = kVec.end();
		for(; itrB!=itrE; ++itrB,
			++uiID, ++ppkIndex)
		{
			*ppkIndex = _Create(uiID, *itrB);
		}
	}

	//! 析构函数
	~FvIndexVector()
	{
		FT** ppkIndex = m_ppkIndex;
		FT** ppkEnd = m_ppkIndex + m_uiSize;
		FT* pkVal;
		for(; ppkIndex<ppkEnd; ++ppkIndex)
		{
			pkVal = *ppkIndex;
			pkVal->~FT();
			delete [] ((char*)pkVal - 4);
		}

		char* pkNode;
		while(m_pkFreeNode)
		{
			pkNode = (char*)m_pkFreeNode;
			m_pkFreeNode = m_pkFreeNode->m_pkNex;
			delete [] pkNode;
		}

		free(m_ppkIndex);
	}

	//! 类型转换
	operator FTVector() const
	{
		FTVector kVec;
		kVec.reserve(m_uiSize);

		FT** ppkIndex = m_ppkIndex;
		FT** ppkEnd = m_ppkIndex + m_uiSize;
		for(; ppkIndex<ppkEnd; ++ppkIndex)
		{
			kVec.push_back(**ppkIndex);
		}
		return kVec;
	}

	//! 迭代器
	class const_iterator
	{
	public:
		friend class FvIndexVector;

		const_iterator():m_ppkIndex(NULL) {}
		const_iterator(FT** ppkIndex):m_ppkIndex(ppkIndex) {}

		const FT& operator*() const
		{
			FV_ASSERT(m_ppkIndex);
			return **m_ppkIndex;
		}

		const FT* operator->() const
		{
			FV_ASSERT(m_ppkIndex);
			return *m_ppkIndex;
		}

		const FT& operator[](difference_type iOff) const
		{
			FV_ASSERT(m_ppkIndex);
			return **(m_ppkIndex+iOff);
		}

		const_iterator& operator++()
		{
			FV_ASSERT(m_ppkIndex);
			++m_ppkIndex;
			return *this;
		}

		const_iterator operator++(int)
		{
			FV_ASSERT(m_ppkIndex);
			const_iterator kTmp = *this;
			++m_ppkIndex;
			return kTmp;
		}

		const_iterator& operator--()
		{
			FV_ASSERT(m_ppkIndex);
			--m_ppkIndex;
			return *this;
		}

		const_iterator operator--(int)
		{
			FV_ASSERT(m_ppkIndex);
			const_iterator kTmp = *this;
			--m_ppkIndex;
			return kTmp;
		}

		const_iterator& operator+=(difference_type iOff)
		{
			if(iOff == 0) return *this;
			FV_ASSERT(m_ppkIndex);
			m_ppkIndex += iOff;
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
			FV_ASSERT(m_ppkIndex && kRight.m_ppkIndex);
			return difference_type(m_ppkIndex - kRight.m_ppkIndex);
		}

		bool operator==(const const_iterator& kRight) const
		{
			return m_ppkIndex == kRight.m_ppkIndex;
		}

		bool operator!=(const const_iterator& kRight) const
		{
			return m_ppkIndex != kRight.m_ppkIndex;
		}

		bool operator<(const const_iterator& kRight) const
		{
			FV_ASSERT(m_ppkIndex && kRight.m_ppkIndex);
			return m_ppkIndex < kRight.m_ppkIndex;
		}

		bool operator>(const const_iterator& kRight) const
		{
			FV_ASSERT(m_ppkIndex && kRight.m_ppkIndex);
			return m_ppkIndex > kRight.m_ppkIndex;
		}

		bool operator<=(const const_iterator& kRight) const
		{
			FV_ASSERT(m_ppkIndex && kRight.m_ppkIndex);
			return m_ppkIndex <= kRight.m_ppkIndex;
		}

		bool operator>=(const const_iterator& kRight) const
		{
			FV_ASSERT(m_ppkIndex && kRight.m_ppkIndex);
			return m_ppkIndex >= kRight.m_ppkIndex;
		}

	protected:
		FT**	m_ppkIndex;
	};

	class iterator : public const_iterator
	{
	public:
		friend class FvIndexVector;

		iterator():const_iterator() {}
		iterator(FT** ppkIndex):const_iterator(ppkIndex) {}

		FT& operator*() const
		{
			FV_ASSERT(m_ppkIndex);
			return **m_ppkIndex;
		}

		FT* operator->() const
		{
			FV_ASSERT(m_ppkIndex);
			return *m_ppkIndex;
		}

		FT& operator[](difference_type iOff) const
		{
			FV_ASSERT(m_ppkIndex);
			return **(m_ppkIndex+iOff);
		}

		iterator& operator++()
		{
			FV_ASSERT(m_ppkIndex);
			++m_ppkIndex;
			return *this;
		}

		iterator operator++(int)
		{
			FV_ASSERT(m_ppkIndex);
			iterator kTmp = *this;
			++m_ppkIndex;
			return kTmp;
		}

		iterator& operator--()
		{
			FV_ASSERT(m_ppkIndex);
			--m_ppkIndex;
			return *this;
		}

		iterator operator--(int)
		{
			FV_ASSERT(m_ppkIndex);
			iterator kTmp = *this;
			--m_ppkIndex;
			return kTmp;
		}

		iterator& operator+=(difference_type iOff)
		{
			if(iOff == 0) return *this;
			FV_ASSERT(m_ppkIndex);
			m_ppkIndex += iOff;
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
			FV_ASSERT(m_ppkIndex && kRight.m_ppkIndex);
			return difference_type(m_ppkIndex - kRight.m_ppkIndex);
		}

		bool operator==(const iterator& kRight) const
		{
			return m_ppkIndex == kRight.m_ppkIndex;
		}

		bool operator!=(const iterator& kRight) const
		{
			return m_ppkIndex != kRight.m_ppkIndex;
		}

		bool operator<(const iterator& kRight) const
		{
			FV_ASSERT(m_ppkIndex && kRight.m_ppkIndex);
			return m_ppkIndex < kRight.m_ppkIndex;
		}

		bool operator>(const iterator& kRight) const
		{
			FV_ASSERT(m_ppkIndex && kRight.m_ppkIndex);
			return m_ppkIndex > kRight.m_ppkIndex;
		}

		bool operator<=(const iterator& kRight) const
		{
			FV_ASSERT(m_ppkIndex && kRight.m_ppkIndex);
			return m_ppkIndex <= kRight.m_ppkIndex;
		}

		bool operator>=(const iterator& kRight) const
		{
			FV_ASSERT(m_ppkIndex && kRight.m_ppkIndex);
			return m_ppkIndex >= kRight.m_ppkIndex;
		}
	};

	//! 接口
	bool empty() const				{ return !m_uiSize; }
	size_type size() const			{ return m_uiSize; }
	size_type capacity() const		{ return m_uiCapacity; }
	size_type max_size() const		{ return size_type(~0); }

	iterator begin()				{ return iterator(m_ppkIndex); }
	const_iterator begin() const	{ return const_iterator(m_ppkIndex); }
	iterator end()					{ return iterator(m_ppkIndex + m_uiSize); }
	const_iterator end() const		{ return const_iterator(m_ppkIndex + m_uiSize); }

	FT& front()						{ FV_ASSERT(m_uiSize && m_ppkIndex) return **m_ppkIndex; }
	const FT& front() const			{ FV_ASSERT(m_uiSize && m_ppkIndex) return **m_ppkIndex; }
	FT& back()						{ FV_ASSERT(m_uiSize && m_ppkIndex) return **(m_ppkIndex+m_uiSize-1); }
	const FT& back() const			{ FV_ASSERT(m_uiSize && m_ppkIndex) return **(m_ppkIndex+m_uiSize-1); }

	FvUInt32 PointerToIndex(FT* pkVal)
	{
		FV_ASSERT(pkVal);
		return _GetID(pkVal);
	}

	void reserve(size_type uiCount)
	{
		if(uiCount < m_uiSize)
			uiCount = m_uiSize;
		_ReserveIndex(uiCount);
	}

	const FT& at(size_type uiPos) const { FV_ASSERT(uiPos<m_uiSize && m_ppkIndex) return **(m_ppkIndex+uiPos); }
	FT& at(size_type uiPos)			{ FV_ASSERT(uiPos<m_uiSize && m_ppkIndex) return **(m_ppkIndex+uiPos); }

	const FT& operator[](size_type uiPos) const { FV_ASSERT(uiPos<m_uiSize && m_ppkIndex) return **(m_ppkIndex+uiPos); }
	FT& operator[](size_type uiPos)	{ FV_ASSERT(uiPos<m_uiSize && m_ppkIndex) return **(m_ppkIndex+uiPos); }

	void resize(size_type uiNewsize)
	{
		if(m_uiSize < uiNewsize)
		{
			_ReserveIndex(uiNewsize);

			FT** ppkIndex = m_ppkIndex + m_uiSize;
			FT** ppkEnd = m_ppkIndex + uiNewsize;
			FvUInt32 uiID(m_uiSize);
			for(; ppkIndex<ppkEnd; ++ppkIndex, ++uiID)
			{
				*ppkIndex = _Create(uiID);
			}
		}
		else if(uiNewsize < m_uiSize)
		{
			FT** ppkIndex = m_ppkIndex + uiNewsize;
			FT** ppkEnd = m_ppkIndex + m_uiSize;
			for(; ppkIndex<ppkEnd; ++ppkIndex)
			{
				_Destroy(*ppkIndex);
			}

			_ReserveIndex(uiNewsize);
		}

		m_uiSize = uiNewsize;
	}

	void resize(size_type uiNewsize, const FT& kVal)
	{
		if(m_uiSize < uiNewsize)
		{
			_ReserveIndex(uiNewsize);

			FT** ppkIndex = m_ppkIndex + m_uiSize;
			FT** ppkEnd = m_ppkIndex + uiNewsize;
			FvUInt32 uiID(m_uiSize);
			for(; ppkIndex<ppkEnd; ++ppkIndex, ++uiID)
			{
				*ppkIndex = _Create(uiID, kVal);
			}
		}
		else if(uiNewsize < m_uiSize)
		{
			FT** ppkIndex = m_ppkIndex + uiNewsize;
			FT** ppkEnd = m_ppkIndex + m_uiSize;
			for(; ppkIndex<ppkEnd; ++ppkIndex)
			{
				_Destroy(*ppkIndex);
			}

			_ReserveIndex(uiNewsize);
		}

		m_uiSize = uiNewsize;
	}

	void assign(size_type uiCount, const FT& kVal)
	{
		resize(uiCount, kVal);
	}

	void clear()
	{
		FT** ppkIndex = m_ppkIndex;
		FT** ppkEnd = m_ppkIndex + m_uiSize;
		for(; ppkIndex<ppkEnd; ++ppkIndex)
		{
			_Destroy(*ppkIndex);
		}

		m_uiSize = 0;
	}

	iterator erase(const_iterator kWhere)
	{
		FT** ppkIndex = kWhere.m_ppkIndex;
		FV_ASSERT(ppkIndex && m_ppkIndex<=ppkIndex);
		FvUInt32 uiID = ppkIndex - m_ppkIndex;
		FV_ASSERT(0 <= uiID && uiID < m_uiSize && uiID == _GetID(*ppkIndex));

		_Destroy(*ppkIndex);

		FT** ppkEnd = m_ppkIndex + m_uiSize -1;
		for(; ppkIndex<ppkEnd; ++ppkIndex, ++uiID)
		{
			*ppkIndex = *(ppkIndex + 1);
			_SetID(*ppkIndex, uiID);
		}

		--m_uiSize;

		return kWhere.m_ppkIndex;
	}

	void erase(size_type uiIndex)
	{
		FV_ASSERT(0<=uiIndex && uiIndex<m_uiSize);
		FT** ppkIndex = m_ppkIndex + uiIndex;

		_Destroy(*ppkIndex);

		FvUInt32 uiID(uiIndex);
		FT** ppkEnd = m_ppkIndex + m_uiSize -1;
		for(; ppkIndex<ppkEnd; ++ppkIndex, ++uiID)
		{
			*ppkIndex = *(ppkIndex + 1);
			_SetID(*ppkIndex, uiID);
		}

		--m_uiSize;
	}

	size_type erase_value(FT* pkVal)
	{
		size_type uiIdx = PointerToIndex(pkVal);
		FV_ASSERT(uiIdx < m_uiSize);
		FT** ppkIndex = m_ppkIndex + uiIdx;

		_Destroy(*ppkIndex);

		FvUInt32 uiID = uiIdx;
		FT** ppkEnd = m_ppkIndex + m_uiSize -1;
		for(; ppkIndex<ppkEnd; ++ppkIndex, ++uiID)
		{
			*ppkIndex = *(ppkIndex + 1);
			_SetID(*ppkIndex, uiID);
		}

		--m_uiSize;
		return uiIdx;
	}

	FT* erase_no_delete(size_type uiIndex)
	{
		FV_ASSERT(0<=uiIndex && uiIndex<m_uiSize);
		FT** ppkIndex = m_ppkIndex + uiIndex;
		FT* pkNode = *ppkIndex;

		FvUInt32 uiID(uiIndex);
		FT** ppkEnd = m_ppkIndex + m_uiSize -1;
		for(; ppkIndex<ppkEnd; ++ppkIndex, ++uiID)
		{
			*ppkIndex = *(ppkIndex + 1);
			_SetID(*ppkIndex, uiID);
		}

		--m_uiSize;
		return pkNode;
	}

	void pop_back()
	{
		FV_ASSERT(m_uiSize);
		_Destroy(*(m_ppkIndex + m_uiSize -1));
		--m_uiSize;
	}

	FT* pop_back_no_delete()
	{
		FV_ASSERT(m_uiSize);
		FT* pkNode = *(m_ppkIndex + m_uiSize -1);
		--m_uiSize;
		return pkNode;
	}

	void push_back()
	{
		FV_ASSERT(m_uiSize <= m_uiCapacity);
		if(m_uiSize == m_uiCapacity)
		{
			size_type uiNewCapacity = m_uiCapacity + (m_uiCapacity >> 1);
			if(uiNewCapacity == m_uiCapacity)
				uiNewCapacity = m_uiCapacity +1;
			_ReserveIndex(uiNewCapacity);			
		}

		*(m_ppkIndex + m_uiSize) = _Create(m_uiSize);
		++m_uiSize;
	}

	void push_back(const FT& kVal)
	{
		FV_ASSERT(m_uiSize <= m_uiCapacity);
		if(m_uiSize == m_uiCapacity)
		{
			size_type uiNewCapacity = m_uiCapacity + (m_uiCapacity >> 1);
			if(uiNewCapacity == m_uiCapacity)
				uiNewCapacity = m_uiCapacity +1;
			_ReserveIndex(uiNewCapacity);			
		}

		*(m_ppkIndex + m_uiSize) = _Create(m_uiSize, kVal);
		++m_uiSize;
	}

	void DestroyNode(FT* pkVal)
	{
		FV_ASSERT(pkVal);
		_Destroy(pkVal);
	}
/**
	FvIndexVector& operator=(const STVector& kVec)
	{
		return *this;
	}

	FvIndexVector& operator=(const FvIndexVector& kVec)
	{
		return *this;
	}

	iterator insert(const_iterator kWhere, const ST& kVal)
	{
		return begin();
	}

	iterator insert(const_iterator kWhere, const FT& kVal)
	{
		return begin();
	}

	void swap(FvIndexVector& kVec)
	{

	}
**/
protected:
	FT* _Create(FvUInt32 uiID)
	{
		char* pkNode = _GetNode();
		*(FvUInt32*)pkNode = uiID;
		return ::new(pkNode + 4)FT();
	}

	FT* _Create(FvUInt32 uiID, const FT& kVal)
	{
		char* pkNode = _GetNode();
		*(FvUInt32*)pkNode = uiID;
		return ::new(pkNode + 4)FT(kVal);
	}

	void _Destroy(FT* pkVal)
	{
		pkVal->~FT();
		FreeNode* pkNode = (FreeNode*)((char*)pkVal - 4);
		pkNode->m_pkNex = m_pkFreeNode;
		m_pkFreeNode = pkNode;
	}

	char* _GetNode()
	{
		if(m_pkFreeNode)
		{
			char* pkNode = (char*)m_pkFreeNode;
			m_pkFreeNode = m_pkFreeNode->m_pkNex;
			return pkNode;
		}
		else
		{
			return new char[sizeof(FT) +4];
		}
	}

	void _ReserveIndex(size_type uiSize)
	{
		FvUInt32 uiByteCnt = sizeof(FT*) * uiSize;
		m_ppkIndex = (FT**)realloc(m_ppkIndex, uiByteCnt);
		m_uiCapacity = uiSize;
	}

	FvUInt32 _GetID(FT* pkVal)
	{
		return *(FvUInt32*)((char*)pkVal - 4);
	}

	void _SetID(FT* pkVal, FvUInt32 uiID)
	{
		*(FvUInt32*)((char*)pkVal - 4) = uiID;
	}

protected:
	struct FreeNode
	{
		FreeNode*	m_pkNex;
	};

	size_type	m_uiSize;
	size_type	m_uiCapacity;
	FT**		m_ppkIndex;
	FreeNode*	m_pkFreeNode;
};


#endif//__FvIndexVector_H__
