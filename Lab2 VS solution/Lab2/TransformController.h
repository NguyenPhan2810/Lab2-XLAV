#pragma once

#include "GeometricTransformer.h"

#include <map>
#include <vector>
#include <string>

enum class TransformTask
{
	zoom,
	resize,
	rotate,
	rotateN,
	flip,
};

enum class PixelInterpolator
{
	NearestNeighbor,
	Bilinear,
};

class TransformController : protected GeometricTransformer
{
public:
	/*
	Function to perform a task required
	Params:
	- source: image willing to modify
	- dest: result of the modification
	- task: one of the task declared in TransformTask
	- args: the arguments for the task. Each task has each type of arguments.
	- interpolator: one of the interpolator in PixelInterpolator
	Returns:
	- 1 if succeed
	- 0 if fail
	*/
	bool execute(const cv::Mat& source, cv::Mat& dest,
		TransformTask task,
		std::vector<std::string> args,
		PixelInterpolator interpolator
	);
};