#pragma once
#include<mutex>
#include<queue>
#include "common.h"
template<class Val>
class ThreadSafeBuffer
{
private:
    mutable std::mutex rwmutex;
	std::vector<Val> buffer;
	size_t counter;
	size_t size;
	
	
public:
	ThreadSafeBuffer()
	{
		size = 0;
		buffer.resize(BUFFER_SIZE);
		counter = 0;
	//	std::cout << "TSB::TSB" << std::endl;
	}

	bool get(Val& frame)
	{

		std::unique_lock<std::mutex> lock(rwmutex);
	//	std::cout << "TSB::get (in) " << "size: " << size << " counter: " << counter << std::endl;
		if (size == 0)
			return false;
		frame = buffer[counter];
		counter = (counter + 1) % BUFFER_SIZE;
		size--;
	//	std::cout << "TSB::get (out) " << "size: " << size << " counter: " << counter << std::endl;
		return true;

	}

	bool push(const Val& frame)
	{
		std::unique_lock<std::mutex> lock(rwmutex);

		//std::cout << "TSB::push(in) " << "size: " << size << " counter: " << counter << std::endl;
		if (size >= BUFFER_SIZE)
			return false;
		buffer[(counter + size) % BUFFER_SIZE] = frame;

		size++;

		//std::cout << "TSB::push (out) " << "size: " << size << " counter: " << counter << std::endl;
		return true;
	}

	void clear()	
	{
		size = 0;
		counter = 0;
	}
};
