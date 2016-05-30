/*
 * Copyright (c) 2016
 * Geco Gaming Company
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for GECO purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation. Geco Gaming makes no
 * representations about the suitability of this software for GECO
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 */

#include "geco-bit-stream.h"
#include "geco-globals.h"

using namespace geco::ultils;

GECO_NET_BEGIN_NSPACE

static const bit_size_t GECO_STREAM_STACK_ALLOC_BITS = BYTES_TO_BITS(GECO_STREAM_STACK_ALLOC_BYTES);

GECO_STATIC_FACTORY_DEFIS(geco_bit_stream_t, geco_bit_stream_t);

geco_bit_stream_t::geco_bit_stream_t() :
allocated_bits_size_(GECO_STREAM_STACK_ALLOC_BITS),
writable_bit_pos_(0),
readable_bit_pos_(0),
uchar_data_(statck_buffer_),
can_free_(false),
is_read_only_(false)
{
    memset(uchar_data_, 0, GECO_STREAM_STACK_ALLOC_BYTES);
}

geco_bit_stream_t::geco_bit_stream_t(const bit_size_t initialBytesAllocate) :
writable_bit_pos_(0),
readable_bit_pos_(0),
is_read_only_(false)
{
    if (initialBytesAllocate <= GECO_STREAM_STACK_ALLOC_BYTES)
    {
        uchar_data_ = statck_buffer_;
        allocated_bits_size_ = GECO_STREAM_STACK_ALLOC_BITS;
        can_free_ = false;
        assert(uchar_data_);
        memset(uchar_data_, 0, GECO_STREAM_STACK_ALLOC_BYTES);
    }
    else
    {
        uchar_data_ = (uchar*)gMallocEx(initialBytesAllocate, TRACKE_MALLOC);
        allocated_bits_size_ = BYTES_TO_BITS(initialBytesAllocate);
        can_free_ = true;
        assert(uchar_data_);
        memset(uchar_data_, 0, initialBytesAllocate);
    }
}
geco_bit_stream_t::geco_bit_stream_t(uchar* src, const byte_size_t len, bool copy/*=false*/) :
allocated_bits_size_(BYTES_TO_BITS(len)),
writable_bit_pos_(BYTES_TO_BITS(len)),
readable_bit_pos_(0),
can_free_(false),
is_read_only_(!copy)
{
    if (copy)
    {
        if (len > 0)
        {
            if (len <= GECO_STREAM_STACK_ALLOC_BYTES)
            {
                uchar_data_ = statck_buffer_;
                allocated_bits_size_ = BYTES_TO_BITS(GECO_STREAM_STACK_ALLOC_BYTES);
                memset(uchar_data_, 0, GECO_STREAM_STACK_ALLOC_BYTES);
            }
            else
            {
                uchar_data_ = (uchar*)gMallocEx(len, TRACKE_MALLOC);
                can_free_ = true;
                memset(uchar_data_, 0, len);
            }
            memcpy(uchar_data_, src, len);
        }
        else
        {
            uchar_data_ = 0;
        }
    }
    else
    {
        uchar_data_ = src;
    }
}
geco_bit_stream_t::~geco_bit_stream_t()
{
    if (can_free_ && allocated_bits_size_ > GECO_STREAM_STACK_ALLOC_BYTES)
    {
        gFreeEx(uchar_data_, TRACKE_MALLOC);
    }
}

void geco_bit_stream_t::ReadMini(uchar* dest, const bit_size_t bits2Read, bool isUnsigned)
{
    uint currByte;
    uchar byteMatch;
    uchar halfByteMatch;

    if (isUnsigned)
    {
        byteMatch = 0;
        halfByteMatch = 0;
    }
    else
    {
        byteMatch = 0xFF;
        halfByteMatch = 0xF0;
    }

    if (!IsBigEndian())
    {
        currByte = (bits2Read >> 3) - 1;
        while (currByte > 0)
        {
            // If we read a 1 then the data is byteMatch.
            ReadMini(isUnsigned);
            if (isUnsigned)// Check that bit
            {
                //JINFO << "matched";
                dest[currByte] = byteMatch;
                currByte--;
            }
            else /// the first byte is not matched 
            {
                // Read the rest of the bytes
                ReadBits(dest, (currByte + 1) << 3);
                return;
            }
        }
        assert(currByte == 0);
    }
    else
    {
        currByte = 0;
        while (currByte < ((bits2Read >> 3) - 1))
        {
            // If we read a 1 then the data is byteMatch.
            ReadMini(isUnsigned);
            if (isUnsigned)// Check that bit
            {
                //JINFO << "matched";
                dest[currByte] = byteMatch;
                currByte++;
            }
            else /// the first byte is not matched 
            {
                // Read the rest of the bytes
                ReadBits(dest, bits2Read - (currByte << 3));
                return;
            }
        }
    }

    // If this assert is hit the stream wasn't long enough to read from
    assert(get_payloads() >= 1);

    /// the upper(left aligned) half of the last byte(now currByte == 0) is a 0000
    /// (positive) or 1111 (nagative) write a bit 1 and the remaining 4 bits. 
    ReadMini(isUnsigned);
    if (isUnsigned)
    {
        ReadBits(dest + currByte, 4);
        // read the remaining 4 bits
        dest[currByte] |= halfByteMatch;
    }
    else
    {
        ReadBits(dest + currByte, 8);
    }
}

void geco_bit_stream_t::AppendBitsCouldRealloc(const bit_size_t bits2Append)
{
    bit_size_t newBitsAllocCount = bits2Append + writable_bit_pos_; /// official
    //bit_size_t newBitsAllocCount = bits2Append + mWritingPosBits + 1;

    // If this assert hits then we need to specify mReadOnly as false 
    // It needs to reallocate to hold all the data and can't do it unless we allocated to begin with
    // Often hits if you call Write or Serialize on a read-only bitstream
    assert(is_read_only_ == false);

    //if (newBitsAllocCount > 0 && ((mBitsAllocSize - 1) >> 3) < // official
    //	((newBitsAllocCount - 1) >> 3)) 

    /// see if one or more new bytes need to be allocated
    if (allocated_bits_size_ < newBitsAllocCount)
    {
        // Less memory efficient but saves on news and deletes
        /// Cap to 1 meg buffer to save on huge allocations
        // [11/16/2015 JACKIE]  
        /// fix bug: newBitsAllocCount should plus 1MB if < 1MB, otherwise it should doule itself
        if (newBitsAllocCount > 1048576)/// 1024B*1024 = 1048576B = 1024KB = 1MB
            newBitsAllocCount += 1048576;
        else
            newBitsAllocCount <<= 1;
        // Use realloc and free so we are more efficient than delete and new for resizing
        bit_size_t bytes2Alloc = BITS_TO_BYTES(newBitsAllocCount);
        if (uchar_data_ == statck_buffer_)
        {
            if (bytes2Alloc > GECO_STREAM_STACK_ALLOC_BYTES)
            {
                uchar_data_ = (uchar *)gMallocEx(bytes2Alloc, TRACKE_MALLOC);
                if (writable_bit_pos_ > 0)
                    memcpy(uchar_data_, statck_buffer_, BITS_TO_BYTES(allocated_bits_size_));
                can_free_ = true;
            }
        }
        else
        {
            /// if allocate new memory, old data is copied and old memory is frred
            uchar_data_ = (uchar*)gReallocEx(uchar_data_, bytes2Alloc, TRACKE_MALLOC);
            can_free_ = true;
        }

        assert(uchar_data_ != 0);
    }

    if (newBitsAllocCount > allocated_bits_size_)
        allocated_bits_size_ = newBitsAllocCount;
}

void geco_bit_stream_t::ReadBits(uchar *dest, bit_size_t bits2Read, bool alignRight /*= true*/)
{
    /// Assume bits to write are 10101010+00001111, 
    /// bits2Write = 4, rightAligned = true, and so 
    /// @mWritingPosBits = 5   @startWritePosBits = 5&7 = 5
    /// 
    /// |<-------data[0]------->|     |<---------data[1]------->|        
    ///+++++++++++++++++++++++++++++++++++
    /// | 0 |  1 | 2 | 3 |  4 | 5 |  6 | 7 | 8 |  9 |10 |11 |12 |13 |14 | 15 |  src bits index
    ///+++++++++++++++++++++++++++++++++++
    /// | 0 |  0 | 0 | 1 |  0 | 0 |  0 | 0 | 0 |  0 |  0 |  0 |  0  |  0 |  0 |   0 |  src  bits in memory
    ///+++++++++++++++++++++++++++++++++++
    /// 
    /// start write first 3 bits 101 after shifting to right by , 00000 101 
    /// write result                                                                      00010 101

    assert(bits2Read > 0);
    assert(get_payloads() >= bits2Read);
    //if (bits2Read <= 0 || bits2Read > get_payloads()) return;

    /// get offset that overlaps one byte boudary, &7 is same to %8, but faster
    const bit_size_t startReadPosBits = readable_bit_pos_ & 7;

    /// byte position where start to read
    byte_size_t readPosByte = readable_bit_pos_ >> 3;

    if (startReadPosBits == 0 && (bits2Read & 7) == 0)
    {
        memcpy(dest, uchar_data_ + (readable_bit_pos_ >> 3), bits2Read >> 3);
        readable_bit_pos_ += bits2Read;
        return;
    }

    /// if @mReadPosBits is aligned  do memcpy for efficiency
    if (startReadPosBits == 0)
    {
        memcpy(dest, &uchar_data_[readPosByte], BITS_TO_BYTES(bits2Read));
        readable_bit_pos_ += bits2Read;

        /// if @bitsSize is not multiple times of 8, 
        /// process the last read byte to shit the bits
        bit_size_t offset = bits2Read & 7;
        if (offset > 0)
        {
            if (alignRight)
                dest[BITS_TO_BYTES(bits2Read) - 1] >>= (8 - offset);
            else
                dest[BITS_TO_BYTES(bits2Read) - 1] |= 0;
        }
        return;
        // return true;
    }

    bit_size_t writePosByte = 0;
    memset(dest, 0, BITS_TO_BYTES(bits2Read)); /// Must set all 0 

    /// Read one complete byte each time 
    while (bits2Read > 0)
    {
        readPosByte = readable_bit_pos_ >> 3;

        /// firstly read left-fragment bits in this byte 
        dest[writePosByte] |= (uchar_data_[readPosByte] << (startReadPosBits));

        /// secondly read right-fragment bits  ( if any ) in this byte
        if (startReadPosBits > 0 && bits2Read > (8 - startReadPosBits))
        {
            dest[writePosByte] |= uchar_data_[readPosByte + 1] >> (8 - startReadPosBits);
        }

        if (bits2Read >= 8)
        {
            bits2Read -= 8;
            readable_bit_pos_ += 8;
            writePosByte++;
        }
        else
        {
            // Reading a partial byte for the last byte, shift right so the data is aligned on the right
            /*if (alignRight) dest[writePosByte] >>= (8 - bits2Read);*/

            //  [11/16/2015 JACKIE] Add: zero unused bits  
            if (alignRight)
                dest[writePosByte] >>= (8 - bits2Read);/// right align result byte: 0000 1111
            else
                dest[writePosByte] |= 0;/// left align result byte: 1111 0000

            ///  [11/15/2015 JACKIE] fix bug of not incrementing mReadingPosBits
            readable_bit_pos_ += bits2Read;
            bits2Read = 0;
        }
    }
    //return true;
}

void geco_bit_stream_t::read_ranged_float(float &outFloat, float floatMin, float floatMax)
{
    assert(floatMax > floatMin);

    ushort percentile;
    ReadMini(percentile);

    outFloat = floatMin + ((float)percentile / 65535.0f) * (floatMax - floatMin);
    if (outFloat<floatMin) outFloat = floatMin;
    else if (outFloat>floatMax) outFloat = floatMax;
}

void geco_bit_stream_t::ReadAlignedBytes(uchar *dest, const byte_size_t bytes2Read)
{
    assert(bytes2Read > 0);
    assert(get_payloads() >= BYTES_TO_BITS(bytes2Read));
    /// if (bytes2Read <= 0) return;

    /// Byte align
    align_readable_bit_pos();

    /// read the data
    memcpy(dest, uchar_data_ + (readable_bit_pos_ >> 3), bytes2Read);
    readable_bit_pos_ += bytes2Read << 3;
}

void geco_bit_stream_t::ReadAlignedBytes(char *dest, byte_size_t &bytes2Read, const byte_size_t maxBytes2Read)
{
    bytes2Read = ReadBit();
    ///ReadMini(bytes2Read);
    if (bytes2Read > maxBytes2Read) bytes2Read = maxBytes2Read;
    if (bytes2Read == 0) return;
    ReadAlignedBytes((uchar*)dest, bytes2Read);
}

void geco_bit_stream_t::ReadAlignedBytesAlloc(char **dest, byte_size_t &bytes2Read, const byte_size_t maxBytes2Read)
{
    gFreeEx(*dest, TRACKE_MALLOC);
    *dest = 0;
    bytes2Read = ReadBit();
    ///ReadMini(bytes2Read);
    if (bytes2Read > maxBytes2Read) bytes2Read = maxBytes2Read;
    if (bytes2Read == 0) return;
    *dest = (char*)gMallocEx(bytes2Read, TRACKE_MALLOC);
    ReadAlignedBytes((uchar*)*dest, bytes2Read);
}

void geco_bit_stream_t::WriteBits(const uchar* src, bit_size_t bits2Write, bool rightAligned /*= true*/)
{
    /// Assume bits to write are 10101010+00001111, 
    /// bits2Write = 4, rightAligned = true, and so 
    /// @mWritingPosBits = 5   @startWritePosBits = 5&7 = 5
    /// 
    /// |<-------data[0]------->|     |<---------data[1]------->|        
    ///+++++++++++++++++++++++++++++++++++
    /// | 0 |  1 | 2 | 3 |  4 | 5 |  6 | 7 | 8 |  9 |10 |11 |12 |13 |14 | 15 |  src bits index
    ///+++++++++++++++++++++++++++++++++++
    /// | 0 |  0 | 0 | 1 |  0 | 0 |  0 | 0 | 0 |  0 |  0 |  0 |  0  |  0 |  0 |   0 |  src  bits in memory
    ///+++++++++++++++++++++++++++++++++++
    /// 
    /// start write first 3 bits 101 after shifting to right by , 00000 101 
    /// write result                                                                      00010 101

    assert(is_read_only_ == false);
    assert(bits2Write > 0);

    //if( mReadOnly ) return false;
    //if( bits2Write == 0 ) return false;

    AppendBitsCouldRealloc(bits2Write);

    /// get offset that overlaps one byte boudary, &7 is same to %8, but faster
    /// @startWritePosBits could be zero
    const bit_size_t startWritePosBits = writable_bit_pos_ & 7;

    // If currently aligned and numberOfBits is a multiple of 8, just memcpy for speed
    if (startWritePosBits == 0 && (bits2Write & 7) == 0)
    {
        memcpy(uchar_data_ + (writable_bit_pos_ >> 3), src, bits2Write >> 3);
        writable_bit_pos_ += bits2Write;
        return;
    }

    uchar dataByte;
    //const uchar* inputPtr = src;

    while (bits2Write > 0)
    {
        dataByte = *(src++);

        /// if @dataByte is the last byte to write, we have to convert this byte into 
        /// stream internal data by shifting the bits in this last byte to left-aligned
        if (bits2Write < 8 && rightAligned) dataByte <<= 8 - bits2Write;

        /// The folowing if-else block will write one byte each time
        if (startWritePosBits == 0)
        {
            /// startWritePosBits == 0  means there are no overlapped bits to be further 
            /// processed and so we can directly write @dataByte into stream
            uchar_data_[writable_bit_pos_ >> 3] = dataByte;
        }
        else
        {
            /// startWritePosBits != 0 means there are  overlapped bits to be further 
            /// processed and so we cannot directly write @dataBytedirectly into stream
            /// we have process overlapped bits before writting
            /// firstly write the as the same number of bits from @dataByte intot
            /// @data[mWritePosBits >> 3] to that in the right-half of 
            /// @data[mWritePosBits >> 3]
            uchar_data_[writable_bit_pos_ >> 3] |= dataByte >> startWritePosBits;

            /// then to see if we have remaining bits in @dataByte to write 
            /// 1. startWritePosBits > 0 means @data[mWritePosBits >> 3] is a partial byte
            /// 2. bits2Write > ( 8 - startWritePosBits ) means the rest space in 
            /// @data[mWritePosBits >> 3] cannot hold all remaining bits in @dataByte
            /// we have to write these reamining bits to the next byte 
            assert(startWritePosBits > 0);
            if (bits2Write > (8 - startWritePosBits))
            {
                /// write remaining bits into the  byte next to @data[mWritePosBits >> 3]
                uchar_data_[(writable_bit_pos_ >> 3) + 1] = (dataByte << (8 - startWritePosBits));
            }
        }

        /// we wrote one complete byte in above codes just now
        if (bits2Write >= 8)
        {
            writable_bit_pos_ += 8;
            bits2Write -= 8;
        }
        else ///  it is the last (could be partial) byte we wrote in the above codes,
        {
            writable_bit_pos_ += bits2Write;
            bits2Write = 0;
        }
    }
}
void geco_bit_stream_t::Write(geco_bit_stream_t *jackieBits, bit_size_t bits2Write)
{
    assert(is_read_only_ == false);
    assert(bits2Write > 0);
    assert(bits2Write <= jackieBits->get_payloads());

    AppendBitsCouldRealloc(bits2Write);
    bit_size_t numberOfBitsMod8 = (jackieBits->readable_bit_pos_ & 7);
    bit_size_t newBits2Read = 8 - numberOfBitsMod8;

    /// write some bits to make @mReadingPosBits aligned to next byte boudary
    if (newBits2Read > 0)
    {
        while (newBits2Read-- > 0)
        {
            numberOfBitsMod8 = writable_bit_pos_ & 7;
            if (numberOfBitsMod8 == 0)
            {
                /// see if this src bit  is 1 or 0, 0x80 (16)= 128(10)= 10000000 (2)
                if ((jackieBits->uchar_data_[jackieBits->readable_bit_pos_ >> 3] &
                    (0x80 >> (jackieBits->readable_bit_pos_ & 7))))
                    // Write 1
                    uchar_data_[writable_bit_pos_ >> 3] = 0x80;
                else
                    uchar_data_[writable_bit_pos_ >> 3] = 0;
            }
            else
            {
                /// see if this src bit  is 1 or 0, 0x80 (16)= 128(10)= 10000000 (2)
                if ((jackieBits->uchar_data_[jackieBits->readable_bit_pos_ >> 3] &
                    (0x80 >> (jackieBits->readable_bit_pos_ & 7))))
                {
                    /// set dest bit to 1 if the src bit is 1,do-nothing if the src bit is 0
                    uchar_data_[writable_bit_pos_ >> 3] |= 0x80 >> (numberOfBitsMod8);
                }
                else
                {
                    uchar_data_[writable_bit_pos_ >> 3] |= 0;
                }
            }

            jackieBits->readable_bit_pos_++;
            writable_bit_pos_++;
        }
        bits2Write -= newBits2Read;
    }
    // call WriteBits() for efficient  because it writes one byte from src at one time much faster
    assert((jackieBits->readable_bit_pos_ & 7) == 0);
    WriteBits(&jackieBits->uchar_data_[jackieBits->readable_bit_pos_ >> 3], bits2Write, false);
    jackieBits->readable_bit_pos_ += bits2Write;
}

void geco_bit_stream_t::write_ranged_float(float src, float floatMin, float floatMax)
{
    assert(floatMax > floatMin);
    assert(src < floatMax + .001f);
    assert(src >= floatMin - .001f);

    float percentile = 65535.0f * ((src - floatMin) / (floatMax - floatMin));
    if (percentile < 0.0f) percentile = 0.0;
    if (percentile > 65535.0f) percentile = 65535.0f;
    //Write((uint16_t)percentile);
    WriteMini((ushort)percentile);
}

void geco_bit_stream_t::WriteMini(const uchar* src, const bit_size_t bits2Write, const bool isUnsigned)
{
    byte_size_t currByte;
    uchar byteMatch = isUnsigned ? 0 : 0xFF; /// 0xFF=255=11111111

    if (!IsBigEndian())
    {
        /// get the highest byte with highest index  PCs
        currByte = (bits2Write >> 3) - 1;

        ///  high byte to low byte, 
        /// if high byte is a byteMatch then write a 1 bit.
        /// Otherwise write a 0 bit and then write the remaining bytes
        while (currByte > 0)
        {
            ///  If high byte is byteMatch (0 or 0xff)
            /// then it would have the same value shifted
            if (src[currByte] == byteMatch)
            {
                Write(true);
                currByte--;
            }
            else /// the first byte is not matched
            {
                Write(false);
                // Write the remainder of the data after writing bit false
                WriteBits(src, (currByte + 1) << 3, true);
                return;
            }
        }
        /// make sure we are now on the lowest byte (index 0)
        assert(currByte == 0);
    }
    else
    {
        /// get the highest byte with highest index  PCs
        currByte = 0;

        ///  high byte to low byte, 
        /// if high byte is a byteMatch then write a 1 bit.
        /// Otherwise write a 0 bit and then write the remaining bytes
        while (currByte < ((bits2Write >> 3) - 1))
        {
            ///  If high byte is byteMatch (0 or 0xff)
            /// then it would have the same value shifted
            if (src[currByte] == byteMatch)
            {
                geco_debug("write match%d\n", byteMatch);
                Write(true);
                currByte++;
            }
            else /// the first byte is not matched
            {
                geco_debug("write not match%d\n", byteMatch);
                Write(false);
                // Write the remainder of the data after writing bit false
                WriteBits(src + currByte, bits2Write - (currByte << 3), true);
                return;
            }
        }
        /// make sure we are now on the lowest byte (index highest)
        assert(currByte == ((bits2Write >> 3) - 1));
    }

    /// last byte
    if ((src[currByte] & 0xF0) == 0x00 || (src[currByte] & 0xF0) == 0xF0)
    { /// the upper(left aligned) half of the last byte(now currByte == 0) is a 0000 (positive) or 1111 (nagative)
        /// write a bit 1 and the remaining 4 bits.
        Write(true);
        WriteBits(src + currByte, 4, true);
    }
    else
    {        /// write a 0 and the remaining 8 bites.
        Write(false);
        WriteBits(src + currByte, 8, true);
    }
}

void geco_bit_stream_t::write_aligned_bytes(const uchar *src, const byte_size_t numberOfBytesWrite)
{
    align_writable_bit_pos();
    Write((char*)src, numberOfBytesWrite);
}

void geco_bit_stream_t::write_aligned_bytes(const uchar *src, const byte_size_t bytes2Write, const byte_size_t maxBytes2Write)
{
    if (src == 0 || bytes2Write == 0)
    {
        //  [11/15/2015 JACKIE] this is ooficial impl whixch will waste 4 bits
        // actually we only nned to send one bit
        ///WriteMini(bytes2Write);
        WriteBitZero();
        return;
    }
    WriteMini(bytes2Write);
    write_aligned_bytes(src, bytes2Write < maxBytes2Write ?
    bytes2Write : maxBytes2Write);
}

void geco_bit_stream_t::pad_zeros_up_to(uint bytes)
{
    int numWrite = bytes - get_written_bytes();
    if (numWrite > 0)
    {
        align_writable_bit_pos();
        AppendBitsCouldRealloc(BYTES_TO_BITS(numWrite));
        memset(uchar_data_ + (writable_bit_pos_ >> 3), 0, numWrite);
        writable_bit_pos_ += BYTES_TO_BITS(numWrite);
    }
}

void geco_bit_stream_t::Bitify(char* out, bit_size_t mWritePosBits, uchar* mBuffer)
{
    printf("%s[%dbits %dbytes]:\n", "BitsDumpResult", mWritePosBits, BITS_TO_BYTES(mWritePosBits));
    if (mWritePosBits <= 0)
    {
        strcpy(out, "no bits to print\n");
        return;
    }
    int strIndex = 0;
    int inner;
    int stopPos;
    int outter;
    int len = BITS_TO_BYTES(mWritePosBits);

    for (outter = 0; outter < len; outter++)
    {
        if (outter == len - 1)
            stopPos = 8 - (((mWritePosBits - 1) & 7) + 1);
        else
            stopPos = 0;

        for (inner = 7; inner >= stopPos; inner--)
        {
            if ((mBuffer[outter] >> inner) & 1)
                out[strIndex++] = '1';
            else
                out[strIndex++] = '0';
        }
        out[strIndex++] = '\n';
    }

    out[strIndex++] = '\n';
    out[strIndex++] = 0;
}
void geco_bit_stream_t::Bitify(void)
{
    char out[4096 * 8];
    Bitify(out, writable_bit_pos_, uchar_data_);
    printf("%s\n", out);
}
void geco_bit_stream_t::Hexlify(char* out, bit_size_t mWritePosBits, uchar* mBuffer)
{
    //geco_debug("%s[%d bytes]:\n", "HexDumpResult", BITS_TO_BYTES(mWritePosBits));
    if (mWritePosBits <= 0)
    {
        strcpy(out, "no bytes to print\n");
        return;
    }
    for (bit_size_t Index = 0; Index < BITS_TO_BYTES(mWritePosBits); Index++)
    {
        sprintf(out + Index * 3, "%02x ", mBuffer[Index]);
    }

}
void geco_bit_stream_t::Hexlify(void)
{
    char out[4096];
    Hexlify(out, writable_bit_pos_, uchar_data_);
    printf("%s\n", out);
}

GECO_NET_END_NSPACE
