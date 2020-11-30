#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
int main()
{

	// read the input image from file
	Mat image = imread("lena.png", ImreadModes::IMREAD_ANYCOLOR);
	// create and set the window name
	namedWindow(" Show Image ");
	// show the image on window
	imshow(" Show Image ", image);
	// close the window
	waitKey(0);
	return 0;

}