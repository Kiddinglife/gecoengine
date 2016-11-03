//{future header message}
#ifndef __FvBase64_H__
#define __FvBase64_H__

#include "FvKernel.h"
#include "FvBaseTypes.h"

class FV_KERNEL_API FvBase64
{
public:
	static FvString Encode(const char *pcData, size_t stLen);
	static FvString Encode( const FvString &kData )
	{
		return Encode( kData.data(), kData.size() );
	}

	static int Decode(const FvString &kData, char *pcResults, size_t stBufSize);
	static bool Decode( const FvString &kInData, FvString &kOutData );

private:
	static FvString ms_kBase64Table;
	static FvString::size_type ms_astDecodeTable[];
};

#endif // __FvBase64_H__
