#include "GeometricTransformer.h"

#include <cmath>
#include <math.h>

uchar BilinearInterpolate::Interpolate(float tx, float ty, int tChannel, uchar* pSrc, int srcWidthStep, int nChannels)
{
    return uchar();
}

uchar NearestNeighborInterpolate::Interpolate(float tx, float ty, int tChannel, uchar* pSrc, int srcWidthStep, int nChannels)
{
    int x = roundf(tx);
    int y = roundf(ty);

    int index = srcWidthStep * y + nChannels * x + tChannel;

    return pSrc[index];
}

int GeometricTransformer::Transform(const cv::Mat& beforeImage, cv::Mat& afterImage, AffineTransform* transformer, PixelInterpolate* interpolator)
{
    uchar* destData = afterImage.data;
    uchar* srcData = beforeImage.data;

    int nChannels = afterImage.channels();

    // Iterate through output
    for (int r = 0; r < afterImage.rows; ++r)
    {
        for (int c = 0; c < afterImage.cols; ++c)
        {
            float tx = c;
            float ty = r;

            // Get the original point from this point of output
            transformer->TransformPoint(tx, ty, true);

            // If the point is not valid then make it full black
            if (tx <= -0.5 || tx >= afterImage.cols || ty <= -0.5 || ty >= afterImage.rows)
                for (int channel = 0; channel < nChannels; ++channel)
                    *(destData + channel) = 0;
            // Or else interpolate from the source data
            else
                for (int channel = 0; channel < nChannels; ++channel)
                    *(destData + channel) = interpolator->Interpolate(tx, ty, channel, srcData, afterImage.step[0], afterImage.step[1]);
        
            // Go to the next pixel
            destData += nChannels;
        }
    }

    return 1;
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
    srcImage.copyTo(dstImage);

    AffineTransform transformer;
    float halfSizeX = srcImage.cols / 2.0f - 0.5;
    float halfSizeY = srcImage.rows / 2.0f - 0.5;

    if (direction)
    {
        transformer.Translate(0, -halfSizeY);
        transformer.Scale(1, -1);
        transformer.Translate(0, halfSizeY);
    }
    else
    {
        transformer.Translate(-halfSizeX, 0);
        transformer.Scale(-1, 1);
        transformer.Translate(halfSizeX, 0);
    }


    if (Transform(srcImage, dstImage, &transformer, interpolator))
        return 1;

    return 0;
}

void AffineTransform::Translate(float dx, float dy)
{
    cv::Mat mat(cv::Mat::eye(3, 3, CV_32FC1));
    mat.at<cv::Vec<float, 1>>(0, 2) = dx;
    mat.at<cv::Vec<float, 1>>(1, 2) = dy;

    _MatrixTransform = mat * _MatrixTransform;
}


void AffineTransform::Rotate(float angle)
{
    static const float PI = 3.14159265358979323846f;
    angle = angle * PI / 180.0f;
    float cosA = cosf(angle);
    float sinA = sinf(angle);

    cv::Mat mat(cv::Mat::eye(3, 3, CV_32FC1));
    mat.at<cv::Vec<float, 1>>(0, 0) = cosA;
    mat.at<cv::Vec<float, 1>>(0, 1) = -sinA;
    mat.at<cv::Vec<float, 1>>(0, 1) = sinA;
    mat.at<cv::Vec<float, 1>>(0, 1) = cosA;

    _MatrixTransform = mat * _MatrixTransform;
}

void AffineTransform::Scale(float sx, float sy)
{
    cv::Mat mat(cv::Mat::eye(3, 3, CV_32FC1));
    mat.at<cv::Vec<float, 1>>(0, 0) = sx;
    mat.at<cv::Vec<float, 1>>(1, 1) = sy;

    _MatrixTransform = mat * _MatrixTransform;
}

void AffineTransform::TransformPoint(float& x, float& y, bool inverseMatrix)
{
    cv::Vec3f P(x, y, 1);

    cv::Mat transformMat = inverseMatrix ? _MatrixTransform.inv() : _MatrixTransform;
    cv::Mat result = transformMat * P;
    x = result.at<cv::Vec<float, 1>>(0, 0)[0];
    y = result.at<cv::Vec<float, 1>>(1, 0)[0];
}

AffineTransform::AffineTransform()
: _MatrixTransform(cv::Mat::eye(3, 3, CV_32FC1))
{
    
}
