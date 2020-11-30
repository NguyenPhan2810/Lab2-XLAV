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
    {
        float sx = std::atof(args[0].c_str());
        float sy = args.size() > 1 ? std::atof(args[1].c_str()) : sx;
        if (!Scale(source, dest, sx, sy, interp))
        {
            Error("Scale failed");
            return false;
        }
        break;
    }
    case TransformTask::resize:
    {
        int newWidth = std::atoi(args[0].c_str());
        int newHeight = args.size() > 1 ? std::atoi(args[1].c_str()) : newWidth;
        if (!Resize(source, dest, newWidth, newHeight, interp))
        {
            Error("Resize failed");
            return false;
        }
        break;
    }
    case TransformTask::rotate:
    {
        float angle = std::atof(args[0].c_str());
        if (!RotateKeepImage(source, dest, angle, interp))
        {
            Error("RotateKeepImage failed");
            return false;
        }
        break;
    }
    case TransformTask::rotateN:
    {
        float angle = std::atof(args[0].c_str());
        if (!RotateUnkeepImage(source, dest, angle, interp))
        {
            Error("RotateUnkeepImage failed");
            return false;
        }
        break;
    }
    case TransformTask::flip:
    {
        bool axis = args[0] == "Ox";
        if (!Flip(source, dest, axis, interp))
        {
            Error("Flip failed");
            return false;
        }
        break;
    }
    }

    if (interp)
    {
        delete interp;
        interp = nullptr;
    }


    return true;
}
