#include "TransformController.h"

#include <map>
#include <vector>
#include <string>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

#define windowsBefore "Image before"
#define windowsAfter "Image after"

std::map<std::string, TransformTask> taskMap
{
	std::pair<std::string, TransformTask>("--zoom", TransformTask::zoom),
	std::pair<std::string, TransformTask>("--resize", TransformTask::resize),
	std::pair<std::string, TransformTask>("--rotate", TransformTask::rotate),
	std::pair<std::string, TransformTask>("--rotateN", TransformTask::rotateN),
	std::pair<std::string, TransformTask>("--flip", TransformTask::flip)
};

std::map<std::string, PixelInterpolator> interpolateMap
{
	std::pair<std::string, PixelInterpolator>("--bl", PixelInterpolator::Bilinear),
	std::pair<std::string, PixelInterpolator>("--nn", PixelInterpolator::NearestNeighbor)
};

int main(int argc, char** argv)
{
	std::vector<std::string> args;
	std::string task;
	std::string interpolate;
	std::string inputPath;

	// Retrieve commands
	if (argc > 1)
	{
		task = std::string(argv[1]);
		interpolate = std::string(argv[2]);
		inputPath = std::string(argv[3]);

		for (int i = 4; i < argc; ++i)
			args.push_back(std::string(argv[i]));
	}
	else
	{
		task = "--flip";
		interpolate = "--nn";
		inputPath = "lena.png";
		args.push_back("Oy");
	}

	// Confirm commands
	std::cout << task << " " << interpolate << " " << inputPath;
	for (auto arg : args)
		std::cout << " " << arg;
	std::cout << std::endl;

	// Perform commands
	TransformController controller;

	cv::Mat before = cv::imread(inputPath, cv::ImreadModes::IMREAD_ANYCOLOR);
	cv::Mat after;
	if (controller.execute(before, after, taskMap.at(task), args, interpolateMap.at(interpolate)))
	{
		cv::namedWindow(windowsBefore);
		cv::namedWindow(windowsAfter);

		cv::imshow(windowsBefore, before);
		cv::imshow(windowsAfter, after);

		cv::waitKey(0);
	}

	return 0;

}