#ifndef LockFreeQueue_H_
#define LockFreeQueue_H_

#include "geco-namesapces.h"
GECO_NET_BEGIN_NSPACE

template <typename elementType, unsigned int  nSize = 128>
class JackieSPSCQueue
{
    public:
    /// this is used to simulate removeAtIndex()
    /// when just simple set valid to false to skip this element
    /// in this way, we do not need add locks for remove functions
    struct Element { elementType ele;  bool valid; };

    JackieSPSCQueue() :
        m_pBuffer(0),
        m_nSize(nSize),
        m_ElementSize(sizeof(elementType)),
        m_nIn(0),
        m_nOut(0)
    {
        //round up to the next power of 2
        if (!IsPower2(nSize*m_ElementSize)) { m_nSize = RoundUpPower2(nSize*m_ElementSize); }
        m_pBuffer = (char*)malloc(m_nSize);
        m_nIn = m_nOut = 0;
    }
    virtual ~JackieSPSCQueue()
    {
        if (0 != m_pBuffer) { free(m_pBuffer); m_pBuffer = 0; }
    }

    void Clear(void) { m_nIn = m_nOut = 0; }
    unsigned int Size() const { return  (m_nIn - m_nOut) / m_ElementSize; }
    bool IsEmpty() { Size() == 0; }
    /// These two functions will do whil-loop internally 
    /// until the needed element is returned
    /// will walways return true;
    bool PushTail(const elementType& buffer)
    {
        unsigned int  m_len = 0;
        while (m_len < m_ElementSize)
        {
            m_len = m_ElementSize - m_len;
            unsigned int l = m_nSize - m_nIn + m_nOut;
            m_len = m_len < l ? m_len : l;

            /// Ensure that we sample the m_nOut index -before- we start putting bytes into the UnlockQueue.
#ifdef _WIN32
            MemoryBarrier();
#else
            __sync_synchronize();
#endif

            /// first put the data starting from fifo->in to buffer end 
            l = m_nSize - (m_nIn  & (m_nSize - 1));
            l = m_len < l ? m_len : l;

            memcpy(m_pBuffer + (m_nIn & (m_nSize - 1)), &buffer, l);
            /// then put the rest (if any) at the beginning of the buffer 
            memcpy(m_pBuffer, &buffer + l, m_len - l);

            /// Ensure that we add the bytes to the kfifo -before- we update the fifo->in index.
#ifdef _WIN32
            MemoryBarrier();
#else
            __sync_synchronize();
#endif
            m_nIn += m_len;
        }
        return true;
    }
    bool PopHead(elementType& buffer)
    {
        unsigned int  m_len = 0;
        while (m_len < m_ElementSize)
        {
            m_len = m_ElementSize - m_len;
            unsigned int l = m_nIn - m_nOut;
            m_len = m_len < l ? m_len : l;

            /// Ensure that we sample the fifo->in index -before- westart removing bytes from the kfifo.
#ifdef _WIN32
            MemoryBarrier();
#else
            __sync_synchronize();
#endif
            /// first get the data from fifo->out until the end of the buffer 
            l = m_nSize - (m_nOut & (m_nSize - 1));
            l = m_len < l ? m_len : l;

            memcpy(&buffer, m_pBuffer + (m_nOut & (m_nSize - 1)), l);
            /// then get the rest (if any) from the beginning of the buffer 
            memcpy(&buffer + l, m_pBuffer, m_len - l);

            /// Ensure that we remove the bytes from the kfifo -before- we update the fifo->out index.
#ifdef _WIN32
            MemoryBarrier();
#else
            __sync_synchronize();
#endif
            m_nOut += m_len;
        }
        return true;
    }


    /// @Notice 
    /// This function can only be used by cosumer thread for thread safe issue
    /// Caller need check @position is bwtween 0 and Size() before call it
    elementType& operator[] (unsigned int position) const
    {
        return *((elementType*)(m_pBuffer + (m_nOut & (m_nSize - 1)) + position*m_ElementSize));
    }

    private:
    bool IsPower2(unsigned long n) { return (n != 0 && ((n & (n - 1)) == 0)); };
    unsigned long RoundUpPower2(unsigned long val)
    {
        if ((val & (val - 1)) == 0) return val;
        unsigned long maxulong = (unsigned long)((unsigned long)~0);
        unsigned long andv = ~(maxulong&(maxulong >> 1));
        while ((andv & val) == 0) andv >>= 1;
        return andv << 1;
    }

    private:
    unsigned long long padding1[16];
    unsigned int   m_nIn;        /* data is added at offset (in % size) */
    unsigned long long padding2[16]; // 128 byte
    unsigned int   m_nOut;        /* data is extracted from off. (out % size) */
    unsigned long long padding3[16];

    char *m_pBuffer;    /* the buffer holding the data */
    unsigned int   m_nSize;        /* the size of the allocated buffer */
    unsigned int   m_ElementSize;
};

GECO_NET_END_NSPACE
#endif
