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

// created on 20-March-2016 by Jackie Zhang

#ifndef __INCLUDE_RANDOM_SEED_CREATOR_H
#define __INCLUDE_RANDOM_SEED_CREATOR_H

#include "geco-export.h"
#include "geco-namesapces.h"

GECO_ULTILS_BEGIN_NSPACE

/// Initialise seed for Random Generator
/// \note not threadSafe, use an instance of RandomSeedCreator if necessary per thread
/// \param[in] seed The seed value for the random number generator.
extern void GECO_EXPORT seedMT(unsigned int seed);

/// \internal
/// \note not threadSafe, use an instance of RandomSeedCreator if necessary per thread
extern unsigned int GECO_EXPORT reloadMT(void);

/// Gets a random unsigned int
/// \note not threadSafe, use an instance of RandomSeedCreator if necessary per thread
/// \return an integer random value.
extern unsigned int GECO_EXPORT randomMT(void);

/// Gets a random float
/// \note not threadSafe, use an instance of RandomSeedCreator if necessary per thread
/// \return 0 to 1.0f, inclusive
extern float GECO_EXPORT frandomMT(void);

/// Randomizes a buffer
/// \note not threadSafe, use an instance of RandomSeedCreator if necessary per thread
extern void GECO_EXPORT fillBufferMT(void *buffer, unsigned int bytes);

GECO_EXPORT class RandomSeedCreator
{
    protected:
    unsigned int state[624 + 1];
    unsigned int *next;
    int left;

    public:
    RandomSeedCreator();
    virtual ~RandomSeedCreator();
    void SeedMT(unsigned int seed);
    unsigned int ReloadMT(void);
    unsigned int RandomMT(void);
    float FrandomMT(void);
    void FillBufferMT(void *buffer, unsigned int bytes);
};

GECO_ULTILS_END_NSPACE
#endif

