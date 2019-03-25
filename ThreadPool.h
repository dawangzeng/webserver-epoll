#ifndef _THREADPOOL_H
#define _THREADPOOL_H
#include "lock.h"
#include "task.h"
#include<queue>

template<typename T>
class ThreadPool
{
	private:
		MutexLock queue_mutex_locker;//队列锁
		Condition  queue_cond_locker;
		pthread_t *threads;
		queue<T*> tasks;//任务队列
		int thread_num;//线程池中线程个数
		bool is_stop;//是否销毁线程
	public:
		ThreadPool(int size = 50);
		~ThreadPool();
		T * get_task();
		bool append(T *task);//添加任务
		static void * worker(void *);//线程启动函数
		void run();//线程真正执行逻辑		
};
template<typename T>
ThreadPool<T>::ThreadPool(int size):threads(NULL),thread_num(size),is_stop(false)
{
	if(thread_num<0)
	{
		cout<<"thread_num can't less 0!"<<endl;
		throw exception();
	}
	threads = new pthread_t[thread_num];
	if(!threads)
	{
		cout<<"new memory failed!"<<endl;
		throw exception();
	}
	//创建制定线程，绑定线程函数
	for(int i =0; i < thread_num; i++)
	{
		int ret = pthread_create(&threads[i],NULL,worker,this);
		if(ret < 0)
		{
			cout<<"thread create failed!"<<endl;
			delete threads;
			throw exception();
		}
		if(pthread_detach(threads[i]))
		{
			cout<<"thread detach failed!"<<endl;
			delete threads;
			throw exception();
		}
	}
}
template<typename T>
ThreadPool<T>::~ThreadPool()
{
	is_stop = true;
	queue_cond_locker.broadcast();//广播通知所有线程结束
	delete threads;
}
template<typename T>
void * ThreadPool<T>::worker(void *param)
{
	ThreadPool * pool = (ThreadPool *)param;
	pool->run();
	return pool;
}

template<typename T>
bool ThreadPool<T>::append(T* task)
{
	queue_mutex_locker.lock();
	tasks.push(task);
	queue_mutex_locker.unlock();

	//唤醒线程
	queue_cond_locker.signal();//如果所有线程都在运行该函数调用无效
	
	return true;
}

template<typename T>
T* ThreadPool<T>::get_task()
{
	Task * task = NULL;
	queue_mutex_locker.lock();
	if(!tasks.empty())
	{
		task = tasks.front();
		tasks.pop();
	}
	queue_mutex_locker.unlock();
	return task;
}
template<typename T>
void ThreadPool<T>::run()
{
	while(!is_stop)
	{
		Task* task = get_task();
		if(!task)
			queue_cond_locker.wait();
		else
		{
			cout<<"task done!"<<endl;
			task->Done();
			delete task;
		}
	}
}
#endif
