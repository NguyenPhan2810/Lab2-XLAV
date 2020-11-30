#include "GeometricTransformer.h"

uchar BilinearInterpolate::Interpolate(float tx, float ty, uchar* pSrc, int srcWidthStep, int nChannels)
{
    return uchar();
}

uchar NearestNeighborInterpolate::Interpolate(float tx, float ty, uchar* pSrc, int srcWidthStep, int nChannels)
{
    return uchar();
}

int GeometricTransformer::Transform(const cv::Mat& beforeImage, cv::Mat& afterImage, AffineTransform* transformer, PixelInterpolate* interpolator)
{
    return 0;
}

int GeometricTransformer::RotateKeepImage(const cv::Mat& srcImage, cv::Mat& dstImage, float angle, PixelInterpolate* interpolator)
{
    return 0;
}

int GeometricTransformer::RotateUnkeepImage(const cv::Mat& srcImage, cv::Mat& dstImage, float angle, PixelInterpolate* interpolator)
{
    return 0;
}

int GeometricTransformer::Scale(const cv::Mat& srcImage, cv::Mat& dstImage, float sx, float sy, PixelInterpolate* interpolator)
{
    return 0;
}

int GeometricTransformer::Resize(const cv::Mat& srcImage, cv::Mat& dstImage, int newWidth, int newHeight, PixelInterpolate* interpolator)
{
    return 0;
}

int GeometricTransformer::Flip(const cv::Mat& srcImage, cv::Mat& dstImage, bool direction, PixelInterpolate* interpolator)
{
    return 0;
}

void AffineTransform::Translate(float dx, float dy)
{
}

void AffineTransform::Rotate(float angle)
{
}

void AffineTransform::Scale(float sx, float sy)
{
}

void AffineTransform::TransformPoint(float& x, float& y)
{
}
