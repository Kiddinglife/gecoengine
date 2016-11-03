#include "FvConcurrency.h"

FV_DECLARE_DEBUG_COMPONENT2( "FvKernel", 0 )

FvSimpleMutex FvMatrixMutexHolder::m_akMutex[MUTEX_MATRIX_SIZE];
volatile unsigned long FvMatrixMutexHolder::m_uiThreadID[MUTEX_MATRIX_SIZE];

static void NoOp() { }
void FV_KERNEL_API (*BeginThreadBlockingOperation)() = &NoOp;
void FV_KERNEL_API (*CeaseThreadBlockingOperation)() = &NoOp;