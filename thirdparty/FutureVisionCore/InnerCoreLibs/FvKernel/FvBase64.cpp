#include "FvBase64.h"
#include "FvDebug.h"

FV_DECLARE_DEBUG_COMPONENT2( "FvKernel", 0 )

static const char fillchar = '=';
static const FvString::size_type np = FvString::npos;
static const char npc = (char) np;

                               // 0000000000111111111122222222223333333333444444444455555555556666
                               // 0123456789012345678901234567890123456789012345678901234567890123
FvString FvBase64::ms_kBase64Table("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");


                                               // 0  1  2  3  4  5  6  7  8  9
FvString::size_type FvBase64::ms_astDecodeTable[] = {np,np,np,np,np,np,np,np,np,np,  // 0 - 9
                                                 np,np,np,np,np,np,np,np,np,np,  //10 -19
                                                 np,np,np,np,np,np,np,np,np,np,  //20 -29
                                                 np,np,np,np,np,np,np,np,np,np,  //30 -39
                                                 np,np,np,62,np,np,np,63,52,53,  //40 -49
                                                 54,55,56,57,58,59,60,61,np,np,  //50 -59
                                                 np,np,np,np,np, 0, 1, 2, 3, 4,  //60 -69
                                                  5, 6, 7, 8, 9,10,11,12,13,14,  //70 -79
                                                 15,16,17,18,19,20,21,22,23,24,  //80 -89
                                                 25,np,np,np,np,np,np,26,27,28,  //90 -99
                                                 29,30,31,32,33,34,35,36,37,38,  //100 -109
                                                 39,40,41,42,43,44,45,46,47,48,  //110 -119
                                                 49,50,51,np,np,np,np,np,np,np,  //120 -129
                                                 np,np,np,np,np,np,np,np,np,np,  //130 -139
                                                 np,np,np,np,np,np,np,np,np,np,  //140 -149
                                                 np,np,np,np,np,np,np,np,np,np,  //150 -159
                                                 np,np,np,np,np,np,np,np,np,np,  //160 -169
                                                 np,np,np,np,np,np,np,np,np,np,  //170 -179
                                                 np,np,np,np,np,np,np,np,np,np,  //180 -189
                                                 np,np,np,np,np,np,np,np,np,np,  //190 -199
                                                 np,np,np,np,np,np,np,np,np,np,  //200 -209
                                                 np,np,np,np,np,np,np,np,np,np,  //210 -219
                                                 np,np,np,np,np,np,np,np,np,np,  //220 -229
                                                 np,np,np,np,np,np,np,np,np,np,  //230 -239
                                                 np,np,np,np,np,np,np,np,np,np,  //240 -249
                                                 np,np,np,np,np,np               //250 -256
												};


FvString FvBase64::Encode(const char *pcData, size_t stLen)
{
    unsigned int		i;
    char		c;
	FvString	ret;

	ret.reserve( stLen * 4 / 3 + 3 );

    for (i = 0; i < stLen; ++i)
    {
        c = (pcData[i] >> 2) & 0x3f;
        ret.push_back( ms_kBase64Table[c] );
        c = (pcData[i] << 4) & 0x3f;
        if (++i < stLen)
            c |= (pcData[i] >> 4) & 0x0f;

        ret.push_back( ms_kBase64Table[c] );
        if (i < stLen)
        {
            c = (pcData[i] << 2) & 0x3f;
            if (++i < stLen)
                c |= (pcData[i] >> 6) & 0x03;

            ret.push_back( ms_kBase64Table[c] );
        }
        else
        {
            ++i;
            ret.push_back( fillchar );
        }

        if (i < stLen)
        {
            c = pcData[i] & 0x3f;
            ret.push_back( ms_kBase64Table[c] );
        }
        else
        {
            ret.push_back( fillchar );
        }
    }

    return(ret);
}

int FvBase64::Decode( const FvString &kData, char *pcResults, size_t stBufSize )
{
    unsigned int			i;
    char			c;
    char			c1;
	unsigned int			len = kData.length();
	int				d = 0;

	if ( len > stBufSize )
	{
		FV_ERROR_MSG( "Base64::decode - destination buffer not large enough\n" );
		return -1;
	}

    for (i = 0; i < len; ++i)
    {
        c = (char) ms_astDecodeTable[(unsigned char)kData[i]];
        ++i;
        c1 = (char) ms_astDecodeTable[(unsigned char)kData[i]];
        c = (c << 2) | ((c1 >> 4) & 0x3);
        pcResults[d++]=c;
        if (++i < len)
        {
            c = kData[i];
            if (fillchar == c)
                break;

            c = (char) ms_astDecodeTable[(unsigned char)kData[i]];
            c1 = ((c1 << 4) & 0xf0) | ((c >> 2) & 0xf);
            pcResults[d++]=c1;
        }

        if (++i < len)
        {
            c1 = kData[i];
            if (fillchar == c1)
                break;

            c1 = (char) ms_astDecodeTable[(unsigned char)kData[i]];
            c = ((c << 6) & 0xc0) | c1;
            pcResults[d++]=c;
        }
    }

    return d;
}

bool FvBase64::Decode( const FvString &kInData, FvString &kOutData )
{
	if (kInData.size() % 4 != 0)
	{
		return false;
	}

	kOutData.clear();
	kOutData.reserve( (kInData.size() + 3)/4 * 3);

    unsigned int			i;
    char			c;
    char			c1;
	unsigned int			len = kInData.size();

    for (i = 0; i < len; ++i)
    {
        c = (char) ms_astDecodeTable[(unsigned char)kInData[i]];
		if (c == npc) return false;
        ++i;
		if (i == len) return false;
        c1 = (char) ms_astDecodeTable[(unsigned char)kInData[i]];
		if (c1 == npc) return false;
        c = (c << 2) | ((c1 >> 4) & 0x3);
		kOutData.push_back( c );
        if (++i < len)
        {
            c = kInData[i];
            if (fillchar == c)
			{
                break;
			}

            c = (char) ms_astDecodeTable[(unsigned char)c];
			if (c == npc) return false;
            c1 = ((c1 << 4) & 0xf0) | ((c >> 2) & 0xf);
            kOutData.push_back( c1 );
        }

        if (++i < len)
        {
            c1 = kInData[i];
            if (fillchar == c1)
			{
                break;
			}

            c1 = (char) ms_astDecodeTable[(unsigned char)c1];
			if (c1 == npc) return false;
            c = ((c << 6) & 0xc0) | c1;
            kOutData.push_back( c );
        }
    }

    return true;
}
