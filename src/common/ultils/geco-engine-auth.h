#ifndef _INCLUDE_GECO_ENGINE_AUTH
#define _INCLUDE_GECO_ENGINE_AUTH

#include <string>
#include <cstring>
//#include "openssl/md5.h"
#include "../ds/geco-bit-stream.h"

namespace geco
{
    namespace ultils
    {
        extern std::string encode_base64(const char* data, size_t len);
        extern std::string encode_base64(const std::string & data);
        extern int decode_base64(const std::string& data, char* results, size_t bufSize);
        extern bool decode_base64(const std::string & inData, std::string & outData);

        /**
         *  This class implements the standard MD5 cryptographic hash function.
         */
        //class MD5
        //{
        //    public:
        //        /**
        //         *  This struct manages the message digest associated with an MD5.
        //         */
        //        struct Digest
        //        {
        //                unsigned char bytes[16];

        //                Digest()
        //                {
        //                }
        //                Digest(MD5 & md5)
        //                {
        //                    *this = md5;
        //                }
        //                Digest & operator=(MD5 & md5)
        //                {
        //                    md5.getDigest(*this);
        //                    return *this;
        //                }

        //                /**
        //                 *  This method clears this digest to all 0s
        //                 */
        //                void clear()
        //                {
        //                    memset(this, 0, sizeof(*this));
        //                }

        //                /**
        //                 *  This method returns whether or not two Digests are equal.
        //                 *
        //                 *  @return True if equal, otherwise false.
        //                 */
        //                bool operator==(const Digest & other) const
        //                {
        //                    return memcmp(this->bytes, other.bytes, sizeof(Digest)) == 0;
        //                }

        //                bool operator!=(const Digest & other) const
        //                {
        //                    return !(*this == other);
        //                }

        //                /**
        //                 *  This method returns the sort ordering of two digests.
        //                 *
        //                 *  @return True if this less than other, otherwise false.
        //                 */
        //                bool operator<(const Digest & other) const
        //                {
        //                    return memcmp(this->bytes, other.bytes, sizeof(Digest)) < 0;
        //                }

        //                /**
        //                 *  Quote the given digest to something that can live in XML.
        //                 */
        //                std::string quote() const
        //                {
        //                    const char hexTbl[17] = "0123456789ABCDEF";

        //                    char buf[32];
        //                    for (uint i = 0; i < 16; i++)
        //                    {
        //                        buf[(i << 1) | 0] = hexTbl[bytes[i] >> 4];
        //                        buf[(i << 1) | 1] = hexTbl[bytes[i] & 0x0F];
        //                    }
        //                    return std::string(buf, 32);
        //                }

        //                /**
        //                 *  Reverse the operation of the quote method.
        //                 *  Returns true if successful.
        //                 */
        //                // helper function to unquote a nibble
        //                inline unsigned char unquoteNibble(char c)
        //                {
        //                    return c > '9' ? c - ('A' - 10) : c - '0';
        //                }
        //                bool unquote(const std::string & quotedDigest)
        //                {
        //                    if (quotedDigest.length() == 32)
        //                    {
        //                        const char * buf = quotedDigest.c_str();
        //                        for (uint i = 0; i < 16; i++)
        //                        {
        //                            bytes[i] = (unquoteNibble(buf[(i << 1) | 0]) << 4)
        //                                    | unquoteNibble(buf[(i << 1) | 1]);
        //                        }
        //                        return true;
        //                    }
        //                    else
        //                    {
        //                        this->clear();
        //                        return false;
        //                    }
        //                }

        //                bool isEmpty() const
        //                {
        //                    for (size_t i = 0; i < sizeof(bytes); ++i)
        //                    {
        //                        if (bytes[i] != '\0')
        //                        {
        //                            return false;
        //                        }
        //                    }

        //                    return true;
        //                }
        //        };

        //        MD5()
        //        {
        //            MD5_Init(&state_);
        //        }

        //        /**
        //         *  This method appends data to the MD5.
        //         */
        //        void append(const void * data, int numBytes)
        //        {
        //            MD5_Update(&state_, (const unsigned char*) data, numBytes);
        //        }

        //        /**
        //         *  This method gets the digest associated with this MD5.
        //         */
        //        void getDigest(MD5::Digest & digest)
        //        {
        //            MD5_Final(digest.bytes, &state_);
        //        }

        //    private:
        //        MD5_CTX state_;
        //};

        //extern geco_bit_stream_t& operator>>(geco_bit_stream_t &is, MD5::Digest &d);
        //extern geco_bit_stream_t& operator<<(geco_bit_stream_t &os, const MD5::Digest &d);
    }
}
#endif
