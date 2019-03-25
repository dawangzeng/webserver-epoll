#ifndef _LOCK_H_
#define _LOCK_H_

#include<iostream>
#include<exception>
#include<pthread.h>
using namespace std;

class MutexLock
{
	private:
		pthread_mutex_t mutex;
	public:
		MutexLock(){
			if(pthread_mutex_init(&mutex,NULL))
			{
				cerr<<"init mutex error"<<endl;
				throw exception();
			}
		}
		~MutexLock(){pthread_mutex_destroy(&mutex);}
		bool lock(){
			return pthread_mutex_lock(&mutex) == 0;
		}
		bool unlock(){
			return pthread_mutex_unlock(&mutex) == 0;
		}
};

class Condition
{
	private:
		pthread_mutex_t mutex;
		pthread_cond_t cond;
	public:
		Condition(){
			if(pthread_mutex_init(&mutex,NULL))
			{
				cerr<<"mutex init error"<<endl;
				throw exception();
			}
			if(pthread_cond_init(&cond,NULL))
			{
				cerr<<"cond init error"<<endl;
				pthread_mutex_destroy(&mutex);
				throw exception();
			}
		}
		~Condition(){
			pthread_mutex_destroy(&mutex);
			pthread_cond_destroy(&cond);
		}
		bool wait(){
			pthread_mutex_lock(&mutex);
			int ret = pthread_cond_wait(&cond,&mutex);
			pthread_mutex_unlock(&mutex);
			return ret == 0;
		}
		bool signal(){
			return pthread_cond_signal(&cond) == 0;
		}
		bool broadcast()
		{
			return pthread_cond_broadcast(&cond) ==0;
		}
};


#endif
