#include "udThread.h"

#if UDPLATFORM_WINDOWS
//
// SetThreadName function taken from https://docs.microsoft.com/en-us/visualstudio/debugger/how-to-set-a-thread-name-in-native-code
// Usage: SetThreadName ((DWORD)-1, "MainThread");
//
#include <windows.h>
const DWORD MS_VC_EXCEPTION = 0x406D1388;
#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
  DWORD dwType; // Must be 0x1000.
  LPCSTR szName; // Pointer to name (in user addr space).
  DWORD dwThreadID; // Thread ID (-1=caller thread).
  DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)
static void SetThreadName(DWORD dwThreadID, const char* threadName)
{
  THREADNAME_INFO info;
  info.dwType = 0x1000;
  info.szName = threadName;
  info.dwThreadID = dwThreadID;
  info.dwFlags = 0;
#pragma warning(push)
#pragma warning(disable: 6320 6322)
  __try {
    RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
  }
  __except (EXCEPTION_EXECUTE_HANDLER) {
  }
#pragma warning(pop)
}
#else
#include <sched.h>
#include <pthread.h>
#include <errno.h>
#include <semaphore.h>

void udThread_MsToTimespec(struct timespec *pTimespec, int waitMs)
{
  if (pTimespec == nullptr)
    return;

  clock_gettime(CLOCK_REALTIME, pTimespec);
  pTimespec->tv_sec += waitMs / 1000;
  pTimespec->tv_nsec += long(waitMs % 1000) * 1000000L;

  pTimespec->tv_sec += (pTimespec->tv_nsec / 1000000000L);
  pTimespec->tv_nsec %= 1000000000L;
}
#endif

#define DEBUG_CACHE 0
#define MAX_CACHED_THREADS 16
#define CACHE_WAIT_SECONDS 30
static volatile udThread *s_pCachedThreads[MAX_CACHED_THREADS ? MAX_CACHED_THREADS : 1];

struct udThread
{
  // First element of structure is GUARANTEED to be the operating system thread handle
# if UDPLATFORM_WINDOWS
  HANDLE handle;
# else
  pthread_t t;
# endif
  udThreadStart *pThreadStarter;
  void *pThreadData;
  udSemaphore *pCacheSemaphore; // Semaphore is non-null only while on the cached thread list
  volatile int32_t refCount;
};

// ----------------------------------------------------------------------------
static uint32_t udThread_Bootstrap(udThread *pThread)
{
  uint32_t threadReturnValue;
  bool reclaimed = false; // Set to true when the thread is reclaimed by the cache system
  UDASSERT(pThread->pThreadStarter, "No starter function");
  do
  {
#if DEBUG_CACHE
    if (reclaimed)
      udDebugPrintf("Successfully reclaimed thread %p\n", pThread);
#endif
    threadReturnValue = pThread->pThreadStarter ? pThread->pThreadStarter(pThread->pThreadData) : 0;

    udInterlockedExchangePointer(&pThread->pThreadStarter, nullptr);
    udInterlockedExchangePointer(&pThread->pThreadData, nullptr);
    reclaimed = false;

    if (pThread->refCount == 1)
    {
      // Instead of letting this thread go to waste, see if we can cache it to be recycled
      for (int slotIndex = 0; slotIndex < MAX_CACHED_THREADS; ++slotIndex)
      {
        if (udInterlockedCompareExchangePointer(&s_pCachedThreads[slotIndex], pThread, nullptr) == nullptr)
        {
#if DEBUG_CACHE
          udDebugPrintf("Making thread %p available for cache (slot %d)\n", pThread, slotIndex);
#endif
          // Successfully added to the cache, now wait to see if anyone wants to dance
          int timeoutWakeup = udWaitSemaphore(pThread->pCacheSemaphore, CACHE_WAIT_SECONDS * 1000);
          if (udInterlockedCompareExchangePointer(&s_pCachedThreads[slotIndex], nullptr, pThread) == pThread)
          {
#if DEBUG_CACHE
            udDebugPrintf("Allowing thread %p to die\n", pThread);
#endif
          }
          else
          {
#if DEBUG_CACHE
            udDebugPrintf("Reclaiming thread %p\n", pThread);
#endif
            // If it was woken via timeout, AND it was reclaimed, there's an outstanding semaphore ref
            if (timeoutWakeup)
              udWaitSemaphore(pThread->pCacheSemaphore);
            reclaimed = true;
          }
          break;
        }
      }
    }
  } while (reclaimed && pThread->pThreadStarter);

  // Call to destroy here will decrement reference count, and only destroy if
  // the original creator of the thread didn't take a reference themselves
  if (pThread)
    udThread_Destroy(&pThread);

  return threadReturnValue;
}


// ****************************************************************************
udResult udThread_Create(udThread **ppThread, udThreadStart *pThreadStarter, void *pThreadData, udThreadCreateFlags /*flags*/, const char *pThreadName)
{
  udResult result;
  udThread *pThread = nullptr;
  int slotIndex;
  udUnused(pThreadName);

  UD_ERROR_NULL(pThreadStarter, udR_InvalidParameter_);
  for (slotIndex = 0; pThread == nullptr && slotIndex < MAX_CACHED_THREADS; ++slotIndex)
  {
    pThread = const_cast<udThread*>(s_pCachedThreads[slotIndex]);
    if (udInterlockedCompareExchangePointer(&s_pCachedThreads[slotIndex], nullptr, pThread) != pThread)
      pThread = nullptr;
  }
  if (pThread)
  {
    udInterlockedExchangePointer(&pThread->pThreadStarter, pThreadStarter);
    udInterlockedExchangePointer(&pThread->pThreadData, pThreadData);
    udIncrementSemaphore(pThread->pCacheSemaphore);
  }
  else
  {
    pThread = udAllocType(udThread, 1, udAF_Zero);
    UD_ERROR_NULL(pThread, udR_MemoryAllocationFailure);
    pThread->pCacheSemaphore = udCreateSemaphore();
#if DEBUG_CACHE
    udDebugPrintf("Creating udThread %p\n", pThread);
#endif
    pThread->pThreadStarter = pThreadStarter;
    pThread->pThreadData = pThreadData;
    udInterlockedExchange(&pThread->refCount, 1);
# if UDPLATFORM_WINDOWS
    pThread->handle = CreateThread(NULL, 4096, (LPTHREAD_START_ROUTINE)udThread_Bootstrap, pThread, 0, NULL);
#else
    typedef void *(*PTHREAD_START_ROUTINE)(void *);
    pthread_create(&pThread->t, NULL, (PTHREAD_START_ROUTINE)udThread_Bootstrap, pThread);
#endif
  }
#if UDPLATFORM_WINDOWS
  if (pThreadName)
    SetThreadName(GetThreadId(pThread->handle), pThreadName);
#elif (UDPLATFORM_LINUX || UDPLATFORM_ANDROID)
  if (pThreadName)
    pthread_setname_np(pThread->t, pThreadName);
#endif

  if (ppThread)
  {
    // Since we're returning a handle, increment the ref count because the caller is now expected to destroy it
    *ppThread = pThread;
    udInterlockedPreIncrement(&pThread->refCount);
  }
  result = udR_Success;

epilogue:
  return result;
}

// ****************************************************************************
// Author: Dave Pevreal, November 2014
void udThread_SetPriority(udThread *pThread, udThreadPriority priority)
{
  if (pThread)
  {
#if UDPLATFORM_WINDOWS
    switch (priority)
    {
    case udTP_Lowest:   SetThreadPriority(pThread->handle, THREAD_PRIORITY_LOWEST); break;
    case udTP_Low:      SetThreadPriority(pThread->handle, THREAD_PRIORITY_BELOW_NORMAL); break;
    case udTP_Normal:   SetThreadPriority(pThread->handle, THREAD_PRIORITY_NORMAL); break;
    case udTP_High:     SetThreadPriority(pThread->handle, THREAD_PRIORITY_ABOVE_NORMAL); break;
    case udTP_Highest:  SetThreadPriority(pThread->handle, THREAD_PRIORITY_HIGHEST); break;
    }
#elif UDPLATFORM_LINUX
    int policy = sched_getscheduler(0);
    int lowest = sched_get_priority_min(policy);
    int highest = sched_get_priority_max(policy);
    int pthreadPrio = (priority * (highest - lowest) / udTP_Highest) + lowest;
    pthread_setschedprio(pThread->t, pthreadPrio);
#else
    udUnused(priority);
#endif
  }
}

// ****************************************************************************
void udThread_Destroy(udThread **ppThread)
{
  if (ppThread)
  {
    udThread *pThread = *ppThread;
    *ppThread = nullptr;
    if (pThread && udInterlockedPreDecrement(&pThread->refCount) == 0)
    {
      udDestroySemaphore(&pThread->pCacheSemaphore);
#if UDPLATFORM_WINDOWS
      CloseHandle(pThread->handle);
#endif
      udFree(pThread);
    }
  }
}

// ****************************************************************************
// Author: Dave Pevreal, July 2018
void udThread_DestroyCached()
{
  while (1)
  {
    bool anyThreadsLeft = false;
    for (int slotIndex = 0; slotIndex < MAX_CACHED_THREADS; ++slotIndex)
    {
      volatile udThread *pThread = udInterlockedExchangePointer(&s_pCachedThreads[slotIndex], nullptr);
      if (pThread)
      {
        udIncrementSemaphore(pThread->pCacheSemaphore);
        anyThreadsLeft = true;
      }
    }
    if (anyThreadsLeft)
      udYield();
    else
      break;
  }
}

// ****************************************************************************
udResult udThread_Join(udThread *pThread, int waitMs)
{
  if (!pThread)
    return udR_InvalidParameter_;

#if UDPLATFORM_WINDOWS
  UDCOMPILEASSERT(INFINITE == UDTHREAD_WAIT_INFINITE, "Infinite constants don't match");

  DWORD result = WaitForSingleObject(pThread->handle, (DWORD)waitMs);
  if (result)
  {
    if (result == WAIT_TIMEOUT)
      return udR_Timeout;

    return udR_Failure_;
  }
#elif UDPLATFORM_LINUX
  if (waitMs == UDTHREAD_WAIT_INFINITE)
  {
    int result = pthread_join(pThread->t, nullptr);
    if (result)
    {
      if (result == EINVAL)
        return udR_InvalidParameter_;

      return udR_Failure_;
    }
  }
  else
  {
    timespec ts;
    udThread_MsToTimespec(&ts, waitMs);

    int result = pthread_timedjoin_np(pThread->t, nullptr, &ts);
    if (result)
    {
      if (result == ETIMEDOUT)
        return udR_Timeout;

      if (result == EINVAL)
        return udR_InvalidParameter_;

      return udR_Failure_;
    }
  }
#else
  udUnused(waitMs);
  int result = pthread_join(pThread->t, nullptr);
  if (result)
  {
    if (result == EINVAL)
      return udR_InvalidParameter_;

    return udR_Failure_;
  }
#endif

  return udR_Success;
}

#if UDPLATFORM_OSX || UDPLATFORM_IOS
# define UD_USE_PLATFORM_SEMAPHORE 0
# define UD_UNSUPPORTED_PLATFORM_SEMAPHORE 1
#else
# define UD_USE_PLATFORM_SEMAPHORE 0
# define UD_UNSUPPORTED_PLATFORM_SEMAPHORE 0
#endif
#define UD_GENERIC_SEMAPHORE_DUPLICATE_CODE 1

struct udSemaphore
{
#if UD_USE_PLATFORM_SEMAPHORE
# if UDPLATFORM_WINDOWS
  HANDLE handle;
# elif UD_UNSUPPORTED_PLATFORM_SEMAPHORE
#  error "Unsupported platform."
# else
  sem_t handle;
# endif
#else
# if UD_GENERIC_SEMAPHORE_DUPLICATE_CODE
#  if UDPLATFORM_WINDOWS
  CRITICAL_SECTION criticalSection;
  CONDITION_VARIABLE condition;
#  else
  pthread_mutex_t mutex;
  pthread_cond_t condition;
#  endif
# else
  udMutex *pMutex;
  udConditionVariable *pCondition;
# endif

  volatile int count;
  volatile int refCount;
#endif
};

// ****************************************************************************
// Author: Samuel Surtees, August 2017
udSemaphore *udCreateSemaphore()
{
  udResult result;
  udSemaphore *pSemaphore = udAllocType(udSemaphore, 1, udAF_None);
  UD_ERROR_NULL(pSemaphore, udR_MemoryAllocationFailure);
#if UD_USE_PLATFORM_SEMAPHORE
# if UDPLATFORM_WINDOWS
  pSemaphore->handle = CreateSemaphore(NULL, 0, 0x7fffffff, NULL);
  UD_ERROR_NULL(pSemaphore, udR_Failure_);
# elif UD_UNSUPPORTED_PLATFORM_SEMAPHORE
#  error "Unsupported platform."
# else
  UD_ERROR_IF(sem_init(&pSemaphore->handle, 0, 0) == -1, udR_Failure_);
# endif
#else
# if UD_GENERIC_SEMAPHORE_DUPLICATE_CODE
#  if UDPLATFORM_WINDOWS
  InitializeCriticalSection(&pSemaphore->criticalSection);
  InitializeConditionVariable(&pSemaphore->condition);
#  else
  pthread_mutex_init(&(pSemaphore->mutex), NULL);
  pthread_cond_init(&(pSemaphore->condition), NULL);
#  endif
# else
  pSemaphore->pMutex = udCreateMutex();
  pSemaphore->pCondition = udCreateConditionVariable();
# endif

  pSemaphore->count = 0;
  pSemaphore->refCount = 1;
#endif

  result = udR_Success;

epilogue:
  return pSemaphore;
}

#if !UD_USE_PLATFORM_SEMAPHORE
// ----------------------------------------------------------------------------
// Author: Samuel Surtees, August 2017
void udDestroySemaphore_Internal(udSemaphore *pSemaphore)
{
  if (pSemaphore == nullptr)
    return;

#if UD_GENERIC_SEMAPHORE_DUPLICATE_CODE
# if UDPLATFORM_WINDOWS
  LeaveCriticalSection(&pSemaphore->criticalSection);
  DeleteCriticalSection(&pSemaphore->criticalSection);
  // CONDITION_VARIABLE doesn't have a delete/destroy function
# else
  pthread_mutex_unlock(&pSemaphore->mutex);
  pthread_mutex_destroy(&pSemaphore->mutex);
  pthread_cond_destroy(&pSemaphore->condition);
# endif
#else
  udReleaseMutex(pSemaphore->pMutex);
  udDestroyMutex(&pSemaphore->pMutex);
  udDestroyConditionVariable(&pSemaphore->pCondition);
#endif

  udFree(pSemaphore);
}

#if UD_GENERIC_SEMAPHORE_DUPLICATE_CODE
// ----------------------------------------------------------------------------
// Author: Samuel Surtees, August 2017
void udLockSemaphore_Internal(udSemaphore *pSemaphore)
{
#if UDPLATFORM_WINDOWS
  EnterCriticalSection(&pSemaphore->criticalSection);
#else
  pthread_mutex_lock(&pSemaphore->mutex);
#endif
}

// ----------------------------------------------------------------------------
// Author: Samuel Surtees, August 2017
void udUnlockSemaphore_Internal(udSemaphore *pSemaphore)
{
#if UDPLATFORM_WINDOWS
  LeaveCriticalSection(&pSemaphore->criticalSection);
#else
  pthread_mutex_unlock(&pSemaphore->mutex);
#endif
}

// ----------------------------------------------------------------------------
// Author: Samuel Surtees, August 2017
void udWakeSemaphore_Internal(udSemaphore *pSemaphore)
{
#if UDPLATFORM_WINDOWS
  WakeConditionVariable(&pSemaphore->condition);
#else
  pthread_cond_signal(&pSemaphore->condition);
#endif
}

// ----------------------------------------------------------------------------
// Author: Samuel Surtees, August 2017
bool udSleepSemaphore_Internal(udSemaphore *pSemaphore, int waitMs)
{
#if UDPLATFORM_WINDOWS
  BOOL retVal = SleepConditionVariableCS(&pSemaphore->condition, &pSemaphore->criticalSection, (waitMs == UDTHREAD_WAIT_INFINITE ? INFINITE : waitMs));
  return (retVal == TRUE);
#else
  int retVal = 0;
  if (waitMs == UDTHREAD_WAIT_INFINITE)
  {
    retVal = pthread_cond_wait(&(pSemaphore->condition), &(pSemaphore->mutex));
  }
  else
  {
    struct timespec ts;
    udThread_MsToTimespec(&ts, waitMs);
    retVal = pthread_cond_timedwait(&(pSemaphore->condition), &(pSemaphore->mutex), &ts);
  }

  return (retVal == 0);
#endif
}
#endif // UD_GENERIC_SEMAPHORE_DUPLICATE_CODE
#endif // !UD_USE_PLATFORM_SEMAPHORE

// ****************************************************************************
// Author: Samuel Surtees, August 2017
void udDestroySemaphore(udSemaphore **ppSemaphore)
{
  if (ppSemaphore == nullptr || *ppSemaphore == nullptr)
    return;

  udSemaphore *pSemaphore = (*(udSemaphore*volatile*)ppSemaphore);
  if (udInterlockedCompareExchangePointer(ppSemaphore, nullptr, pSemaphore) != pSemaphore)
    return;

#if UD_USE_PLATFORM_SEMAPHORE
# if UDPLATFORM_WINDOWS
  CloseHandle(pSemaphore->handle);
# elif UD_UNSUPPORTED_PLATFORM_SEMAPHORE
#  error "Unsupported platform."
# else
  sem_destroy(&pSemaphore->handle);
# endif
  udFree(pSemaphore);
#else
# if UD_GENERIC_SEMAPHORE_DUPLICATE_CODE
  udLockSemaphore_Internal(pSemaphore);
# else
  udLockMutex(pSemaphore->pMutex);
# endif
  if (udInterlockedPreDecrement(&pSemaphore->refCount) == 0)
  {
    udDestroySemaphore_Internal(pSemaphore);
  }
  else
  {
    int refCount = pSemaphore->refCount;
    for (int i = 0; i < refCount; ++i)
    {
      ++(pSemaphore->count);
# if UD_GENERIC_SEMAPHORE_DUPLICATE_CODE
      udWakeSemaphore_Internal(pSemaphore);
# else
      udSignalConditionVariable(pSemaphore->pCondition);
# endif
    }
# if UD_GENERIC_SEMAPHORE_DUPLICATE_CODE
    udUnlockSemaphore_Internal(pSemaphore);
# else
    udReleaseMutex(pSemaphore->pMutex);
# endif
  }
#endif
}

// ****************************************************************************
// Author: Samuel Surtees, August 2017
void udIncrementSemaphore(udSemaphore *pSemaphore, int count)
{
#if UD_USE_PLATFORM_SEMAPHORE
  if (pSemaphore == nullptr)
    return;

# if UDPLATFORM_WINDOWS
  ReleaseSemaphore(pSemaphore->handle, count, nullptr);
# elif UD_UNSUPPORTED_PLATFORM_SEMAPHORE
#  error "Unsupported platform."
# else
  while (count-- > 0)
    sem_post(&pSemaphore->handle);
# endif
#else
  // Exit the function if the refCount is 0 - It's being destroyed!
  if (pSemaphore == nullptr || pSemaphore->refCount == 0)
    return;

  udInterlockedPreIncrement(&pSemaphore->refCount);
  while (count-- > 0)
  {
# if UD_GENERIC_SEMAPHORE_DUPLICATE_CODE
    udLockSemaphore_Internal(pSemaphore);
    ++(pSemaphore->count);
    udWakeSemaphore_Internal(pSemaphore);
    udUnlockSemaphore_Internal(pSemaphore);
# else
    udLockMutex(pSemaphore->pMutex);
    ++(pSemaphore->count);
    udSignalConditionVariable(pSemaphore->pCondition);
    udReleaseMutex(pSemaphore->pMutex);
# endif
  }
  udInterlockedPreDecrement(&pSemaphore->refCount);
#endif
}

// ****************************************************************************
// Author: Samuel Surtees, August 2017
int udWaitSemaphore(udSemaphore *pSemaphore, int waitMs)
{
#if UD_USE_PLATFORM_SEMAPHORE
  if (pSemaphore == nullptr)
    return -1;

# if UDPLATFORM_WINDOWS
  return WaitForSingleObject(pSemaphore->handle, waitMs);
# elif UD_UNSUPPORTED_PLATFORM_SEMAPHORE
#  error "Unsupported platform."
# else
  if (waitMs == UDTHREAD_WAIT_INFINITE)
  {
    return sem_wait(&pSemaphore->handle);
  }
  else
  {
    struct  timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
      return -1;

    ts.tv_sec += waitMs / 1000;
    ts.tv_nsec += long(waitMs % 1000) * 1000000L;

    ts.tv_sec += (ts.tv_nsec / 1000000000L);
    ts.tv_nsec %= 1000000000L;

    return sem_timedwait(&pSemaphore->handle, &ts);
  }
# endif
#else
  // Exit the function if the refCount is 0 - It's being destroyed!
  if (pSemaphore == nullptr || pSemaphore->refCount == 0)
    return -1;

  udInterlockedPreIncrement(&pSemaphore->refCount);
# if UD_GENERIC_SEMAPHORE_DUPLICATE_CODE
  udLockSemaphore_Internal(pSemaphore);
# else
  udLockMutex(pSemaphore->pMutex);
# endif
  bool retVal;
  if (pSemaphore->count > 0)
  {
    retVal = true;
    pSemaphore->count--;
  }
  else
  {
    if (waitMs == UDTHREAD_WAIT_INFINITE)
    {
      retVal = true;
      while (pSemaphore->count == 0)
      {
# if UD_GENERIC_SEMAPHORE_DUPLICATE_CODE
        retVal = udSleepSemaphore_Internal(pSemaphore, waitMs);
# else
        retVal = (udWaitConditionVariable(pSemaphore->pCondition, pSemaphore->pMutex, waitMs) == 0);
# endif

        // If something went wrong, exit the loop
        if (!retVal)
          break;
      }

      if (retVal)
        pSemaphore->count--;
    }
    else
    {
# if UD_GENERIC_SEMAPHORE_DUPLICATE_CODE
      retVal = udSleepSemaphore_Internal(pSemaphore, waitMs);
# else
      retVal = (udWaitConditionVariable(pSemaphore->pCondition, pSemaphore->pMutex, waitMs) == 0);
# endif

      if (retVal)
      {
        // Check for spurious wake-up
        if (pSemaphore->count > 0)
          pSemaphore->count--;
        else
          retVal = false;
      }
    }
  }

  if (udInterlockedPreDecrement(&pSemaphore->refCount) == 0)
  {
    udDestroySemaphore_Internal(pSemaphore);
    return -1;
  }
  else
  {
# if UD_GENERIC_SEMAPHORE_DUPLICATE_CODE
    udUnlockSemaphore_Internal(pSemaphore);
# else
    udReleaseMutex(pSemaphore->pMutex);
# endif

    // 0 is success, not 0 is failure
    return !retVal;
  }
#endif
}

// ****************************************************************************
// Author: Samuel Surtees, September 2017
udConditionVariable *udCreateConditionVariable()
{
#if UDPLATFORM_WINDOWS
  CONDITION_VARIABLE *pCondition = udAllocType(CONDITION_VARIABLE, 1, udAF_None);
  InitializeConditionVariable(pCondition);
#else
  pthread_cond_t *pCondition = udAllocType(pthread_cond_t, 1, udAF_None);
  pthread_cond_init(pCondition, NULL);
#endif

  return (udConditionVariable*)pCondition;
}

// ****************************************************************************
// Author: Samuel Surtees, September 2017
void udDestroyConditionVariable(udConditionVariable **ppConditionVariable)
{
  udConditionVariable *pCondition = *ppConditionVariable;
  *ppConditionVariable = nullptr;

  // Windows doesn't have a clean-up function
#if !UDPLATFORM_WINDOWS
  pthread_cond_destroy((pthread_cond_t *)pCondition);
#endif

  udFree(pCondition);
}

// ****************************************************************************
// Author: Samuel Surtees, September 2017
void udSignalConditionVariable(udConditionVariable *pConditionVariable, int count)
{
  while (count-- > 0)
  {
#if UDPLATFORM_WINDOWS
    CONDITION_VARIABLE *pCondition = (CONDITION_VARIABLE*)pConditionVariable;
    WakeConditionVariable(pCondition);
#else
    pthread_cond_t *pCondition = (pthread_cond_t*)pConditionVariable;
    pthread_cond_signal(pCondition);
#endif
  }
}

// ****************************************************************************
// Author: Samuel Surtees, September 2017
int udWaitConditionVariable(udConditionVariable *pConditionVariable, udMutex *pMutex, int waitMs)
{
#if UDPLATFORM_WINDOWS
  CONDITION_VARIABLE *pCondition = (CONDITION_VARIABLE*)pConditionVariable;
  CRITICAL_SECTION *pCriticalSection = (CRITICAL_SECTION*)pMutex;
  BOOL retVal = SleepConditionVariableCS(pCondition, pCriticalSection, (waitMs == UDTHREAD_WAIT_INFINITE ? INFINITE : waitMs));
  return (retVal == TRUE ? 0 : 1); // This isn't (!retVal) for clarity.
#else
  pthread_cond_t *pCondition = (pthread_cond_t*)pConditionVariable;
  pthread_mutex_t *pMutexInternal = (pthread_mutex_t*)pMutex;
  int retVal = 0;
  if (waitMs == UDTHREAD_WAIT_INFINITE)
  {
    retVal = pthread_cond_wait(pCondition, pMutexInternal);
  }
  else
  {
    struct timespec ts;
    udThread_MsToTimespec(&ts, waitMs);
    retVal = pthread_cond_timedwait(pCondition, pMutexInternal, &ts);
  }

  return retVal;
#endif
}

// ****************************************************************************
// Author: Samuel Surtees, March 2019
udRWLock *udCreateRWLock()
{
#if UDPLATFORM_WINDOWS
  SRWLOCK *pRWLock = udAllocType(SRWLOCK, 1, udAF_None);
  if (pRWLock)
    InitializeSRWLock(pRWLock);
  return (udRWLock*)pRWLock;
#else
  pthread_rwlock_t *pRWLock = (pthread_rwlock_t *)udAlloc(sizeof(pthread_rwlock_t));
  if (pRWLock)
    pthread_rwlock_init(pRWLock, NULL);
  return (udRWLock*)pRWLock;
#endif
}

// ****************************************************************************
// Author: Samuel Surtees, March 2019
void udDestroyRWLock(udRWLock **ppRWLock)
{
  if (ppRWLock && *ppRWLock)
  {
#if UDPLATFORM_WINDOWS
    // Windows doesn't have a clean-up function
    udFree(*ppRWLock);
#else
    pthread_rwlock_t *pRWLock = (pthread_rwlock_t *)(*ppRWLock);
    if (udInterlockedCompareExchangePointer(ppRWLock, nullptr, pRWLock) == (udRWLock*)pRWLock)
    {
      pthread_rwlock_destroy(pRWLock);
      udFree(pRWLock);
    }
#endif
  }
}

// ****************************************************************************
// Author: Samuel Surtees, March 2019
int udReadLockRWLock(udRWLock *pRWLock)
{
  if (pRWLock)
  {
#if UDPLATFORM_WINDOWS
    SRWLOCK *pLock = (SRWLOCK*)pRWLock;
    AcquireSRWLockShared(pLock);
    return 0; // Can't fail
#else
    pthread_rwlock_t *pLock = (pthread_rwlock_t*)pRWLock;
    return pthread_rwlock_rdlock(pLock);
#endif
  }
  return 0;
}

// ****************************************************************************
// Author: Samuel Surtees, March 2019
int udWriteLockRWLock(udRWLock *pRWLock)
{
  if (pRWLock)
  {
#if UDPLATFORM_WINDOWS
    SRWLOCK *pLock = (SRWLOCK*)pRWLock;
    AcquireSRWLockExclusive(pLock);
    return 0; // Can't fail
#else
    pthread_rwlock_t *pLock = (pthread_rwlock_t*)pRWLock;
    return pthread_rwlock_wrlock(pLock);
#endif
  }
  return 0;
}

// ****************************************************************************
// Author: Samuel Surtees, March 2019
void udReadUnlockRWLock(udRWLock *pRWLock)
{
  if (pRWLock)
  {
#if UDPLATFORM_WINDOWS
    SRWLOCK *pLock = (SRWLOCK*)pRWLock;
    ReleaseSRWLockShared(pLock);
#else
    pthread_rwlock_t *pLock = (pthread_rwlock_t*)pRWLock;
    pthread_rwlock_unlock(pLock);
#endif
  }
}

// ****************************************************************************
// Author: Samuel Surtees, March 2019
void udWriteUnlockRWLock(udRWLock *pRWLock)
{
  if (pRWLock)
  {
#if UDPLATFORM_WINDOWS
    SRWLOCK *pLock = (SRWLOCK*)pRWLock;
    ReleaseSRWLockExclusive(pLock);
#else
    pthread_rwlock_t *pLock = (pthread_rwlock_t*)pRWLock;
    pthread_rwlock_unlock(pLock);
#endif
  }
}

// ****************************************************************************
udMutex *udCreateMutex()
{
#if UDPLATFORM_WINDOWS
  CRITICAL_SECTION *pCriticalSection = udAllocType(CRITICAL_SECTION, 1, udAF_None);
  if (pCriticalSection)
    InitializeCriticalSection(pCriticalSection);
  return (udMutex *)pCriticalSection;
#else
  pthread_mutex_t *mutex = (pthread_mutex_t *)udAlloc(sizeof(pthread_mutex_t));
  if (mutex)
  {
    // Initialise the mutex to be recursive, this allows the same thread to lock multiple times
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(mutex, &attr);
    pthread_mutexattr_destroy(&attr);
  }
  return (udMutex*)mutex;
#endif
}

// ****************************************************************************
void udDestroyMutex(udMutex **ppMutex)
{
  if (ppMutex && *ppMutex)
  {
#if UDPLATFORM_WINDOWS
    CRITICAL_SECTION *pCriticalSection = (CRITICAL_SECTION*)(*ppMutex);
    *ppMutex = NULL;
    DeleteCriticalSection(pCriticalSection);
    udFree(pCriticalSection);
#else
    pthread_mutex_t *mutex = (pthread_mutex_t *)(*ppMutex);
    pthread_mutex_destroy(mutex);
    udFree(mutex);
    *ppMutex = nullptr;
#endif
  }
}

// ****************************************************************************
udMutex *udLockMutex(udMutex *pMutex)
{
  if (pMutex)
  {
#if UDPLATFORM_WINDOWS
    EnterCriticalSection((CRITICAL_SECTION*)pMutex);
#else
    pthread_mutex_lock((pthread_mutex_t *)pMutex);
#endif
  }
  return pMutex;
}

// ****************************************************************************
void udReleaseMutex(udMutex *pMutex)
{
  if (pMutex)
  {
#if UDPLATFORM_WINDOWS
    LeaveCriticalSection((CRITICAL_SECTION*)pMutex);
#else
    pthread_mutex_unlock((pthread_mutex_t *)pMutex);
#endif
  }
}


