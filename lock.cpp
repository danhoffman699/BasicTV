#include "util.h"
#include "main.h"
#include "lock.h"

lock_t::lock_t(){
}

lock_t::~lock_t(){
}

void lock_t::lock(){
	const bool prev_state = mutex_lock.try_lock();
	const bool same_thread = std::this_thread::get_id() == id;
	unsigned int thread_id;
	std::stringstream ss;
	ss << std::this_thread::get_id();
	ss >> thread_id;
	const std::string thread_id_str = std::to_string(thread_id);
	if(prev_state){ // was locked, is currently locking
		if(same_thread){
			if(search_for_argv("--spam") != -1){
				std::cout << "[SPAM] same thread locking twice (hash:" << thread_id_str << ")" << std::endl;
			}
		}else{
			if(search_for_argv("--spam") != -1){
				std::cout << "[SPAM] thread id locking thread (hash:" << thread_id_str << ")" << std::endl;
				mutex_lock.lock();
			}
		}
	}else{ // was not locked, is currently
		if(search_for_argv("--spam") != -1){
			std::cout << "thread id locking thread (hash:" << thread_id_str << ")" << std::endl;
			id = std::this_thread::get_id();
		}
	}
	
}

void lock_t::unlock(){
	unsigned int thread_id;
	std::stringstream ss;
	ss << std::this_thread::get_id();
	ss >> thread_id;
	const std::string thread_id_str = std::to_string(thread_id);
	if(search_for_argv("--spam") != -1){
		std::cout << "[SPAM] thread id unlocking thread (hash:" << thread_id_str << ")" << std::endl;
	}
	mutex_lock.try_lock(); // General Protection Error fix
	mutex_lock.unlock();
	id = std::this_thread::get_id();
}
