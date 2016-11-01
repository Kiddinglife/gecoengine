//{future header message}
#ifndef __FvMD5_H__
#define __FvMD5_H__

#ifndef md5_INCLUDED
#  define md5_INCLUDED

typedef unsigned char md5_byte_t;
typedef unsigned int md5_word_t;

typedef struct md5_state_s 
{
    md5_word_t count[2];	
    md5_word_t abcd[4];		
    md5_byte_t buf[64];		
} md5_state_t;

#endif // !md5_INCLUDED

#include "FvKernel.h"
#include "FvBaseTypes.h"

class FvBinaryIStream;
class FvBinaryOStream;

class FV_KERNEL_API FvMD5
{
public:
	struct FV_KERNEL_API Digest
	{
		unsigned char m_acBytes[16];

		Digest() { }
		Digest( FvMD5 & kMD5 ) { *this = kMD5; }
		Digest & operator=( FvMD5 &kMD5 )
			{ kMD5.GetDigest( *this ); return *this; }
		void Clear();

		bool operator==( const Digest &kOther ) const;
		bool operator!=( const Digest &kOther ) const
			{ return !(*this == kOther); }

		bool operator<( const Digest &kOther ) const;

		FvString Quote() const;
		bool Unquote( const FvString &kQuotedDigest );
	};

	FvMD5();

	void Append( const void *pkData, int iNumBytes );
	void GetDigest( Digest &kDigest );

private:
	md5_state_t m_kState;
};

FV_KERNEL_API FvBinaryIStream& operator>>( FvBinaryIStream &kIS, FvMD5::Digest &kDigest );
FV_KERNEL_API FvBinaryOStream& operator<<( FvBinaryOStream &kOS, const FvMD5::Digest &kDigest );

#endif // __FvMD5_H__