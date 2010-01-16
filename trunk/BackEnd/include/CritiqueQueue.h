#pragma once

#include <pthread.h>
#include <queue>


namespace ONLINE_MATLAB{

  using namespace std;
  template <typename QueueType>
  class CritiqueQueue{
  public:
  CritiqueQueue()
      {
        pthread_mutex_init(&mQueueLock, NULL);
        pthread_cond_init(&mQueueSig, NULL);
      }

    ~CritiqueQueue() {}

    void push(QueueType item)
    {
      pthread_mutex_lock(&mQueueLock);
      mQueue.push(item);
      signal();
      pthread_mutex_unlock(&mQueueLock);
    }

    void pop()
    {
      pthread_mutex_lock(&mQueueLock);
      mQueue.pop();
      pthread_mutex_unlock(&mQueueLock);
    }

    QueueType front()
    {
      pthread_mutex_lock(&mQueueLock);
      QueueType item = mQueue.front();
      pthread_mutex_unlock(&mQueueLock);
      return item;
    }

    bool empty()
    {
      pthread_mutex_lock(&mQueueLock);      
      bool empty = mQueue.empty();
      pthread_mutex_unlock(&mQueueLock);
      return empty;
    }

    void wait()
    {
      pthread_cond_wait(&mQueueSig, &mQueueLock);
      unlock();
    }

    void signal()
    {
      pthread_cond_signal(&mQueueSig);
    }

    void lock()
    {
      pthread_mutex_lock(&mQueueLock); 
    }

    void unlock()
    {
      pthread_mutex_unlock(&mQueueLock);      
    }

  private:
    pthread_mutex_t mQueueLock;
    pthread_cond_t mQueueSig;    

    queue<QueueType> mQueue;
    

  };

}// namespace
