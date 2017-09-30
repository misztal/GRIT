#ifndef UTIL_USE_PTHREADS_H
#define UTIL_USE_PTHREADS_H
 
#ifdef WIN32

#include <WinBase.h>

typedef HANDLE pthread_t;

void pthread_create(
	                 pthread_t* t 
	               , const void* ptr
				   , void *(__cdecl *start) (void *)
				   , void* arg)
{
	*t = CreateThread( 
                       NULL                   // default security attributes
                     , 0                      // use default stack size  
                     , ( LPTHREAD_START_ROUTINE )( start )                 // thread function name
                     , arg                    // argument to thread function 
                     , 0                      // use default creation flags 
                     , NULL
	   			     );   // returns the thread identifier 

}

void pthread_join( 
                   pthread_t& t
  			     , const void** ptr
				 )
{
    WaitForSingleObject(t, INFINITE);
	CloseHandle(t);
}

#else
#include <pthread.h>
#endif

 // UTIL_USE_PTHREADS_H
#endif
