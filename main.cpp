#include "PictureInPicture.hpp"

int main(int argc, char** argv)
{
	
	PictureInPicture pictureInPicture;
	pictureInPicture.start("metallica1mp4", "metallica.mp4");
	while (true);
	//		if (cv::waitKey() >= 30)
	//		break;
	//pictureInPicture.stop();
	return 0;
}