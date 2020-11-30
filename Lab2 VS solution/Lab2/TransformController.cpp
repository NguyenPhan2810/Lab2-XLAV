#include "TransformController.h"

void Error(const char* s)
{
    std::cout << "Error occured: " << s << std::endl;
}

bool TransformController::execute(const cv::Mat& source, cv::Mat& dest,
    TransformTask task,
    std::vector<std::string> args,
    PixelInterpolator interpolator)
{
    PixelInterpolate* interp= nullptr;

    // Find the appropriate interpolator
    switch (interpolator)
    {
    case PixelInterpolator::NearestNeighbor: 
        interp = new NearestNeighborInterpolate();
        break;
    case PixelInterpolator::Bilinear:
        interp = new BilinearInterpolate();
        break;
    }

    // Execute the task demanded
    switch (task)
    {
    case TransformTask::zoom:
        break;
    case TransformTask::resize:
        break;
    case TransformTask::rotate:
        break;
    case TransformTask::rotateN:
        break;
    case TransformTask::flip:
        bool axis = args[0] == "Ox";
        if (!Flip(source, dest, axis, interp))
        {
            Error("Flip failed");
            return false;
        }
        break;
    }

    if (interp)
    {
        delete interp;
        interp = nullptr;
    }


    return true;
}
