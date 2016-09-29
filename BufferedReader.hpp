#pragma once
#include"ThreadSafeBuffer.hpp"
#include "common.h"
template<class ValueType>
class BufferedReader
{
private:
	std::thread reader;
	bool hasToClose = false;
	void run()
	{
		cv::Mat frame;
		if (!capturer.isOpened())
			return;
		for (; !hasToClose && capturer.isOpened();)
		{
			bool frameRead = capturer.read(frame);
			if (frameRead)
			{
				while (!buffer.push(frame) && !hasToClose);
			}
			printf("BR get frame\n");
			
		}
		hasToClose = false;
	}

public:
	ThreadSafeBuffer<ValueType> buffer;
	cv::VideoCapture capturer;

	BufferedReader() {};
	bool open(const char* filename)
	{
		if (!capturer.open(filename))
			return false;
		auto runner = std::bind(&BufferedReader<ValueType>::run, this);
		reader = std::thread(runner);
		if (!reader.joinable())
			return false;
		return true;
	}
	
	void close()
	{
		capturer.release();
		hasToClose = true;
		buffer.clear();
	}	

	~BufferedReader()
	{
			
	}
};

