#pragma once
#include "BufferedReader.hpp"
#include "common.h"


class PictureInPicture
{
private:
	BufferedReader<cv::Mat> fBuf, sBuf;
	IplImage* smallImg;
	std::thread writer;
	uint16_t fFps, sFps;
	double scale;
	bool imgAllocated;
	bool hasToClose;


	static double min(double a, double b)
	{
		return a < b ? a : b;
	}
	static uint16_t gcd(uint16_t a, uint16_t b)
	{
		while (a*b)
			a < b ? b %= a : a %= b;
		return a + b;
	}

	void run()
	{
		cv::Mat fFrame;
		cv::Mat sFrame;
		IplImage fImg, sImg;
		bool fImgLoaded = false,
			sImgLoaded = false;
		
		
		uint16_t ticks = 1;
		uint16_t fps = fFps*sFps / gcd(fFps, sFps);

		uint16_t durSleep = 1e6/ double(fps) + 0.5;
			
		cvNamedWindow("PictureInPicture", CV_WINDOW_NORMAL);
		while(!hasToClose)
		{
			if (ticks % fFps)
				sImgLoaded = fBuf.buffer.get(fFrame) && fFrame.data != nullptr;

			if (ticks % sFps)
				fImgLoaded = sBuf.buffer.get(sFrame) && sFrame.data != nullptr;

			std::this_thread::sleep_for(std::chrono::microseconds(durSleep));

			if (sImgLoaded && fImgLoaded)
			{

				sImg = IplImage(sFrame);
				fImgLoaded = false;

				fImg = IplImage(fFrame);
				sImgLoaded = false;
			}

			if (sImgLoaded && !fImgLoaded)
			{

				fImg = IplImage(fFrame);
				sImgLoaded = false;
			}

			if (!sImgLoaded && fImgLoaded)
			{
				sImg = IplImage(sFrame);
				fImgLoaded = false;

			}

			if(pip(&fImg, &sImg))
				cvShowImage("PictureInPicture", &sImg);

			ticks = (ticks + 1) % fps;
		}
	}

	bool pip(IplImage* fImg, IplImage* sImg)
	{

		if (sImg->imageData == nullptr || fImg->imageData == nullptr)
			return false;

		if (!imgAllocated)
		{
			scale = min(fImg->height / (4 * sImg->height), fImg->width / (4 * sImg->width));

			smallImg = cvCreateImage(cvSize(sImg->width*scale,sImg->height*scale),sImg->depth,sImg->nChannels);
			imgAllocated = true;
		}
		cvResize(fImg, smallImg);
		cvSetImageROI(sImg, cvRect(0, 0, smallImg->width, smallImg->height));
		cvZero(sImg);
		cvCopy(smallImg, sImg);
		cvSetImageROI(sImg, cvRect(0, 0, sImg->width, sImg->height));
		return true;
	}
	
	PictureInPicture & operator= (const PictureInPicture& op2) {}
	PictureInPicture(const PictureInPicture& op) {}

public:
	PictureInPicture(): sBuf(),fBuf()
	{
		imgAllocated = false;
		scale = 0;
		smallImg = nullptr;

	}
	bool start(const char* fFile, const char* sFile)
	{

		if (!fBuf.open(fFile) || !sBuf.open(sFile))
			return false;

		auto runner = std::bind(&PictureInPicture::run, this);
		writer = std::thread(runner);
		printf("PIP started\n");
		return true;	
	}
	void stop()
	{
		hasToClose = true;
	}


};