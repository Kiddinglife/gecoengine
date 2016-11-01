#ifndef __FvAStar_H__
#define __FvAStar_H__

#include <set>
#include <queue>
#include <vector>
#include <list>

#include <FvKernel.h>

template<typename Type, FvUInt32 maxSize>
class FvMinHeap
{
public:
	FvMinHeap():_size(0){}
	void Push(Type* key );
	void FastPush(Type* key );
	Type* Pop();
	void MakeHeap();
	void Clear(){_size = 0;}
	bool Empty()const{return (_size == 0);}
	FvUInt32 Size()const{return _size;}

private:
	FvUInt32 _GetLeft(const FvUInt32 idx){return idx<<1; };//k*2
	FvUInt32 _GetRight(const FvUInt32 idx){return (idx<<1)+1; };//k*2+1
	FvUInt32 _GetParent(const FvUInt32 idx){return (idx>>1); };//k/2

	static void _Swap(Type*& one, Type*& other){Type* tmp= one; one= other; other = tmp;}
	void _Siftdown(const FvUInt32 pos);

	FvUInt32 _size;
	Type* _data[maxSize+1];
};


template<typename Type, FvUInt32 maxSize>
void FvMinHeap<Type, maxSize>::_Siftdown(const FvUInt32 pos)
{
	FvUInt32 posMod = pos;
	FvUInt32 left = _GetLeft(posMod);
	while( left <= _size )
	{
		FvUInt32 t = left;//int t= pos<<1; 
		const FvUInt32 right = _GetRight(posMod);
		if( right <= _size && _data[right]->Key() < _data[t]->Key() ) 
		{
			t= right;
		}
		if( _data[t]->Key() < _data[posMod]->Key() )
		{
			_Swap(_data[t], _data[posMod]);
			posMod = t; 
			left = _GetLeft(posMod);
		}
		else 
		{
			break;
		}
	}
}

template<typename Type, FvUInt32 maxSize>
void FvMinHeap<Type, maxSize>::Push(Type* key )
{
	if(_size < maxSize - 1)
	{
		++_size;
		_data[_size]= key;
		FvUInt32 pos= _size;
		while( pos > 1 )
		{
			const FvUInt32 parent = _GetParent(pos);
			if( _data[parent]->Key() > _data[pos]->Key())
			{
				_Swap(_data[pos], _data[parent]);
				pos = parent;
			}
			else 
			{
				break;
			}
		}
	}	
}


template<typename Type, FvUInt32 maxSize>
void FvMinHeap<Type, maxSize>::FastPush(Type* key )
{
	FV_ASSERT(_size < maxSize - 1);
	++_size;
	_data[_size]= key;
	FvUInt32 pos= _size;
	while( pos > 1 )
	{
		const FvUInt32 parent = _GetParent(pos);
		if( _data[parent]->Key() > _data[pos]->Key())
		{
			_Swap(_data[pos], _data[parent]);
			pos = parent;
		}
		else 
		{
			break;
		}
	}	
}

template<typename Type, FvUInt32 maxSize>
Type* FvMinHeap<Type, maxSize>::Pop()
{
	if(_size > 0)
	{
		_Swap(_data[1], _data[_size]);
		--_size;
		_Siftdown(1); 
		return _data[_size+1];
	}
	else
	{
		return NULL;
	}
}


class Buffer
{
	static const size_t MAX_SIZE = 1024 * 256;
	static const size_t ALIGNMENT = 32;
	char buffer_[MAX_SIZE];
	std::list<char*> buffers_;

	char* buffer()
	{
		if( buffers_.empty() )
			return buffer_;
		return *buffers_.begin();
	}
	size_t offset_;
public:
	Buffer() : offset_( 0 )
	{}
	~Buffer()
	{
		for( std::list<char*>::iterator iter = buffers_.begin();
			iter != buffers_.end(); ++iter )
		{
			delete[] *iter;
		}
	}
	void *get( size_t size )
	{
		size_t offset = offset_;
		offset_ += ( size + ALIGNMENT - 1 ) / ALIGNMENT * ALIGNMENT;
		if( offset_ > MAX_SIZE )
		{
			if( size + ALIGNMENT > MAX_SIZE )
			{
				char* result = new char[size];
				buffers_.insert( buffers_.begin(), result );

				char* buffer = new char[MAX_SIZE];
				buffers_.insert( buffers_.begin(), buffer );
				offset_ = 0;

				return result;
			}
			char* buffer = new char[MAX_SIZE];
			buffers_.insert( buffers_.begin(), buffer );
			offset_ = 0;
			return get( size );
		}
		return buffer() + offset;
	}
};

template <class T> class buffer_allocator
{
public:
	Buffer& buffer_;
	typedef T					value_type;
	typedef value_type*			pointer;
	typedef const value_type*	const_pointer;
	typedef value_type&			reference;
	typedef const value_type&	const_reference;
	typedef std::size_t			size_type;
	typedef std::ptrdiff_t		difference_type;

	template <class U> struct rebind
	{
		typedef buffer_allocator<U> other;
	};

	buffer_allocator( Buffer& buffer )
		: buffer_( buffer )
	{}

	buffer_allocator( const buffer_allocator& that ) : buffer_( that.buffer_ ) {}

	template <class U> buffer_allocator( const buffer_allocator<U>& that ) : buffer_( that.buffer_ ) {}

	pointer address( reference x ) const	{	return &x;	}

	const_pointer address( const_reference x ) const	{	return x;	}

	pointer allocate( size_type n, const_pointer = 0 )
	{
		return static_cast<pointer>( buffer_.get( n * sizeof(T) ) );
	}

	void deallocate( pointer, size_type ) {}

	size_type max_size() const
	{
		return size_type( -1 ) / sizeof( T );
	}

	void construct( pointer p, const_reference x )	{	new(p) value_type( x );	}

	void destroy( pointer p )	{	p->~value_type();	}
};

template<class State, class GoalState = State> class AStar
{
public:
	typedef State TState;
	typedef GoalState TGoalState;

	AStar();
	~AStar();

	bool		search(State& start, GoalState& goal, float maxDistance );
	State*		first();
	State*		next();
	void		reset();

	bool infiniteLoopProblem;

private:
	Buffer buffer_;

	struct IntState
	{
		State		ext;		
		float		g;			
		float		h;			
		float 		f;			
		IntState*	pParent;	
		IntState*	pChild;		
		IntState*	hashNext;
		IntState*	freeNext;
		IntState()
			: pParent( NULL ), pChild( NULL )
		{}

		float Key()const{return f;}
		static IntState* alloc()
		{
			IntState* head = freeHead();
			if( head != NULL )
			{
				freeHead() = head->freeNext;
				new (head) IntState;
				return head;
			}
			return new IntState;
		}
		static void free( IntState* state )
		{
			state->~IntState();
			state->freeNext = freeHead();
			freeHead() = state;
		}
		static IntState*& freeHead()
		{
			static IntState* freeHead = NULL;
			return freeHead;
		}
	};

	class IntStateHash
	{
		static const int BIN_SIZE = 257;
		IntState* bin_[BIN_SIZE];
	public:
		IntStateHash()
		{
			for( int i = 0; i < BIN_SIZE; ++i )
				bin_[ i ] = 0;
		}
		unsigned int hash( IntState* state )
		{
			return (unsigned int)( state->ext.Hash() ) % BIN_SIZE;
		}
		IntState* find( IntState* state )
		{
			IntState* head = bin_[ hash( state ) ];
			while( head != NULL )
			{
				if( head->ext.Compare( state->ext ) == 0 )
					return head;
				head = head->hashNext;
			}
			return NULL;
		}
		void erase( IntState* state )
		{
			IntState* head = bin_[ hash( state ) ];
			if(head == NULL)
			{
				return;
			}
			FV_ASSERT( head );

			if( head->ext.Compare( state->ext ) == 0 )
				bin_[ hash( state ) ] = head->hashNext;
			else
			{
				IntState* next = head->hashNext;
				while( next != NULL )
				{
					if( next->ext.Compare( state->ext ) == 0 )
					{
						head->hashNext = next->hashNext;
						next->hashNext = NULL;
						break;
					}
					head = next;
					next = head->hashNext;
				}
			}
		}
		void insert( IntState* state )
		{
			if( find( state ) )
				return;
			state->hashNext = bin_[ hash( state ) ];
			bin_[ hash( state ) ] = state;
		}
		void reset()
		{
			for( int i = 0; i < BIN_SIZE; ++i )
			{
				IntState* item = bin_[ i ];
				while( item )
				{
					IntState* next = item->hashNext;
					IntState::free( item );
					item = next;
				}
				bin_[ i ] = NULL;
			}
		}
		int size() const
		{
			int size = 0;
			for( int i = 0; i < BIN_SIZE; ++i )
			{
				IntState* item = bin_[ i ];
				while( item )
				{
					++size;
					item = item->hashNext;
				}
			}
			return size;
		}
	};

	struct cmp_f
	{
		bool operator()(const IntState* p1, const IntState* p2) const
		{
			return p1->f > p2->f;
		};
	};

	struct cmp_ext
	{
		bool operator()(const IntState* p1, const IntState* p2) const
		{
			return p1->ext.Compare(p2->ext) < 0;
		};
	};

	typedef IntStateHash AStarSet;
	typedef FvMinHeap<IntState, 1024> AStarQueue;

	IntState* 	iter_;
	IntState* 	start_;
	AStarSet 	set_;
	AStarQueue 	open_;
};

#include "FvAStar.inl"

#endif // __FvAStar_H__
