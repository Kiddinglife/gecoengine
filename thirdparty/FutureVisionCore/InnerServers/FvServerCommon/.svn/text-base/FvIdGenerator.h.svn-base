#ifndef __FvIdGenerator_H__
#define __FvIdGenerator_H__

#include <FvBinaryStream.h>

#include <vector>
#include <map>
#include "FvServerCommon.h"


struct FvIDBlock
{
	FvUInt32 	start;
	FvUInt32	end;
};

FV_IOSTREAM_IMP_BY_MEMCPY(FV_INLINE, FvIDBlock)

typedef std::vector<FvIDBlock> FvIDBlockVector;


class FV_SERVERCOMMON_API FvIDGenerator
{
public:
	virtual ~FvIDGenerator() {};
	virtual void GetBlocks(FvIDBlockVector& blockVector,
			FvUInt32 count, FvUInt32 tolerance) = 0;

	virtual void PutBlocks(const FvIDBlockVector& blockVector) = 0;
};

#endif//__FvIdGenerator_H__
