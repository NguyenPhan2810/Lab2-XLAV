#include "GeometricTransformer.h"

#include <cmath>
#include <math.h>

float DegToRad(float deg)
{
    static const float PI = 3.14159265358979323846f;
    return deg * PI / 180.0f;
}

int index2Dto1D(int row, int col, int channel, int srcWidthStep, int nChannels)
{
    return srcWidthStep * row + nChannels * col + channel;
}

// Ref: https://en.wikipedia.org/wiki/Bilinear_interpolation
uchar BilinearInterpolate::Interpolate(float tx, float ty, int tChannel, uchar* pSrc, int srcWidthStep, int nChannels)
{
    /*
    1, 2, 3, 4 are 4 square points surrounding the point
    1---2
    |---|
    3---4
    */
    int x1 = int(tx);
    int y1 = int(ty);

    uchar v1 = pSrc[index2Dto1D(y1, x1, tChannel, srcWidthStep, nChannels)];
    uchar v2 = pSrc[index2Dto1D(y1, x1 + 1, tChannel, srcWidthStep, nChannels)];
    uchar v3 = pSrc[index2Dto1D(y1 + 1, x1, tChannel, srcWidthStep, nChannels)];
    uchar v4 = pSrc[index2Dto1D(y1 + 1, x1 + 1, tChannel, srcWidthStep, nChannels)];

    // Find value of two points in the X axis
    uchar r1 = (x1 + 1 - tx) * v1 + (tx - x1) * v2;
    uchar r2 = (x1 + 1 - tx) * v3 + (tx - x1) * v4;

    // Find the final value in the Y axis
    uchar p = (y1 + 1 - ty) * r1 + (ty - y1) * r2;


    return p;
}

uchar NearestNeighborInterpolate::Interpolate(float tx, float ty, int tChannel, uchar* pSrc, int srcWidthStep, int nChannels)
{
    int x = roundf(tx);
    int y = roundf(ty);

    int index = index2Dto1D(y, x, tChannel, srcWidthStep, nChannels);

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
            if (tx <= -0.5 || tx >= afterImage.cols - 0.5 || ty <= -0.5 || ty >= afterImage.rows - 0.5)
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
    srcImage.copyTo(dstImage);
    AffineTransform transformer;

    float halfWidth = srcImage.cols / 2.0f - 0.5;
    float halfHeight = srcImage.rows / 2.0f - 0.5;

    // The math done to calculate the ratio of the new diagonal / oldDiagonal
    float Height = srcImage.rows;
    float Width = srcImage.cols;
    float newWidth = Height / tanf(DegToRad(angle + 45));

    float nW2 = newWidth * newWidth;
    float W2 = Width * Width;
    float H2 = Height * Height;
    float ratio = sqrtf((nW2 + H2) / (W2 + H2));

    transformer.Translate(-halfWidth, -halfHeight);
    transformer.Rotate(angle);
    transformer.Scale(ratio, ratio);
    transformer.Translate(halfWidth, halfHeight);

    if (Transform(srcImage, dstImage, &transformer, interpolator))
        return 1;

    return 0;
}

int GeometricTransformer::RotateUnkeepImage(const cv::Mat& srcImage, cv::Mat& dstImage, float angle, PixelInterpolate* interpolator)
{
    srcImage.copyTo(dstImage);
    AffineTransform transformer;

    float halfWidth = srcImage.cols / 2.0f - 0.5;
    float halfHeight = srcImage.rows / 2.0f - 0.5;

    transformer.Translate(-halfWidth, -halfHeight);
    transformer.Rotate(angle);
    transformer.Translate(halfWidth, halfHeight);

    if (Transform(srcImage, dstImage, &transformer, interpolator))
        return 1;

    return 0;
}

int GeometricTransformer::Scale(const cv::Mat& srcImage, cv::Mat& dstImage, float sx, float sy, PixelInterpolate* interpolator)
{
    srcImage.copyTo(dstImage);
    AffineTransform transformer;

    float halfWidth = srcImage.cols / 2.0f - 0.5;
    float halfHeight = srcImage.rows / 2.0f - 0.5;

    transformer.Translate(-halfWidth, -halfHeight);
    transformer.Scale(sx, sy);
    transformer.Translate(halfWidth, halfHeight);

    if (Transform(srcImage, dstImage, &transformer, interpolator))
        return 1;

    return 0;
}

int GeometricTransformer::Resize(const cv::Mat& srcImage, cv::Mat& dstImage, int newWidth, int newHeight, PixelInterpolate* interpolator)
{
    return Scale(srcImage, dstImage, (float)newWidth / srcImage.cols, (float)newHeight / srcImage.rows, interpolator);
}

int GeometricTransformer::Flip(const cv::Mat& srcImage, cv::Mat& dstImage, bool direction, PixelInterpolate* interpolator)
{
    srcImage.copyTo(dstImage);

    AffineTransform transformer;
    float halfWidth = srcImage.cols / 2.0f - 0.5;
    float halfHeight = srcImage.rows / 2.0f - 0.5;

    if (direction)
    {
        transformer.Translate(0, -halfHeight);
        transformer.Scale(1, -1);
        transformer.Translate(0, halfHeight);
    }
    else
    {
        transformer.Translate(-halfWidth, 0);
        transformer.Scale(-1, 1);
        transformer.Translate(halfWidth, 0);
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
    angle = DegToRad(angle);

    float cosA = cosf(angle);
    float sinA = sinf(angle);

    cv::Mat mat(cv::Mat::eye(3, 3, CV_32FC1));
    mat.at<cv::Vec<float, 1>>(0, 0) = cosA;
    mat.at<cv::Vec<float, 1>>(0, 1) = -sinA;
    mat.at<cv::Vec<float, 1>>(1, 0) = sinA;
    mat.at<cv::Vec<float, 1>>(1, 1) = cosA;

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
