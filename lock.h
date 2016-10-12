#ifndef LOCK_H
#define LOCK_H
#include "mutex"
#include "thread"
struct lock_t{
private:
	std::mutex mutex_lock;
	std::thread::id id;
public:
	lock_t();
	~lock_t();
	void lock();
	void unlock();
};
#endif
