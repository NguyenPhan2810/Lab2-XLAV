#pragma once

#include <Windows.h>
#include <opencv2/opencv.hpp>

/*
 Lớp base dùng để nội suy màu của 1 pixel
*/
class PixelInterpolate
{
public:
	/*
	Hàm tính giá trị màu của ảnh kết quả từ nội suy màu trong ảnh gốc
	Tham số
		- (tx,ty): tọa độ thực của ảnh gốc sau khi thực hiện phép biến đổi affine
		- pSrc: con trỏ ảnh gốc
		- srcWidthStep: widthstep của ảnh gốc
		- nChannels: số kênh màu của ảnh gốc
	Trả về
		- Giá trị màu được nội suy
	*/
	virtual uchar Interpolate(float tx, float ty, int tChannel, uchar* pSrc, int srcWidthStep, int nChannels) = 0;
};

/*
Lớp nội suy màu theo phương pháp song tuyến tính
*/
class BilinearInterpolate : public PixelInterpolate
{
public:
	uchar Interpolate(float tx, float ty, int tChannel, uchar* pSrc, int srcWidthStep, int nChannels);
};

/*
Lớp nội suy màu theo phương pháp láng giềng gần nhất
*/
class NearestNeighborInterpolate : public PixelInterpolate
{
public:
	uchar Interpolate(float tx, float ty, int tChannel, uchar* pSrc, int srcWidthStep, int nChannels);
};

/*
Lớp biểu diễn pháp biến đổi affine
*/
class AffineTransform
{
	cv::Mat _MatrixTransform;//ma trận 3x3 biểu diễn phép biến đổi affine
public:
	void Translate(float dx, float dy);// xây dựng cv::Matrix transform cho phép tịnh tiến theo vector (dx,dy)
	void Rotate(float angle);//xây dựng cv::Matrix transform cho phép xoay 1 góc angle
	void Scale(float sx, float sy);//xây dựng cv::Matrix transform cho phép tỉ lệ theo hệ số 	
	
								   
	// Transform 1 điểm (x,y) theo cv::Matrix transform đã có
	void TransformPoint(float &x, float &y, bool inverseMatrix = false); 

	AffineTransform();
};

/*
Lớp thực hiện phép biến đổi hình học trên ảnh
*/

class GeometricTransformer
{
public:
	/*
	Hàm biến đổi ảnh theo 1 phép biến đổi affine đã có
	Tham số
	 - beforeImage: ảnh gốc trước khi transform
	 - afterImage: ảnh sau khi thực hiện phép biến đổi affine
	 - transformer: phép biến đổi affine
	 - interpolator: biến chỉ định phương pháp nội suy màu
	Trả về:
	 - 0: Nếu ảnh input ko tồn tại hay ko thực hiện được phép biến đổi
	 - 1: Nếu biến đổi thành công
	*/
	int Transform(
		const cv::Mat &beforeImage, 
		cv::Mat &afterImage, 
		AffineTransform* transformer, 
		PixelInterpolate* interpolator);

	/*
	Hàm xoay bảo toàn nội dung ảnh theo góc xoay cho trước
	Tham số
	- srcImage: ảnh input
	- dstImage: ảnh sau khi thực hiện phép xoay
	- angle: góc xoay (đơn vị: độ)
	- interpolator: biến chỉ định phương pháp nội suy màu
	Trả về:
	 - 0: Nếu ảnh input ko tồn tại hay ko thực hiện được phép biến đổi
	 - 1: Nếu biến đổi thành công
	*/
	int RotateKeepImage(
		const cv::Mat &srcImage, cv::Mat &dstImage, float angle, PixelInterpolate* interpolator);

	/*
	Hàm xoay không bảo toàn nội dung ảnh theo góc xoay cho trước
	Tham số
	- srcImage: ảnh input
	- dstImage: ảnh sau khi thực hiện phép xoay
	- angle: góc xoay (đơn vị: độ)
	- interpolator: biến chỉ định phương pháp nội suy màu
	Trả về:
	 - 0: Nếu ảnh input ko tồn tại hay ko thực hiện được phép biến đổi
	 - 1: Nếu biến đổi thành công
	*/
	int RotateUnkeepImage(
		const cv::Mat &srcImage, cv::Mat &dstImage, float angle, PixelInterpolate* interpolator);

	/*
	Hàm phóng to, thu nhỏ ảnh theo tỉ lệ cho trước
	Tham số
	- srcImage: ảnh input
	- dstImage: ảnh sau khi thực hiện phép xoay	
	- sx, sy: tỉ lệ phóng to, thu nhỏ ảnh	
	- interpolator: biến chỉ định phương pháp nội suy màu
	Trả về:
	 - 0: Nếu ảnh input ko tồn tại hay ko thực hiện được phép biến đổi
	 - 1: Nếu biến đổi thành công
	*/
	int Scale(
		const cv::Mat &srcImage, 
		cv::Mat &dstImage, 
		float sx, float sy, 
		PixelInterpolate* interpolator);
		
	
	/*
	Hàm thay đổi kích thước ảnh
	Tham số
	- srcImage: ảnh input
	- dstImage: ảnh sau khi thay đổi kích thước
	- newWidth, newHeight: kích thước mới
	- interpolator: biến chỉ định phương pháp nội suy màu
	Trả về:
	 - 0: Nếu ảnh input ko tồn tại hay ko thực hiện được phép biến đổi
	 - 1: Nếu biến đổi thành công
	*/
	int Resize(
		const cv::Mat &srcImage, 
		cv::Mat &dstImage, 
		int newWidth, int newHeight, 
		PixelInterpolate* interpolator);

	/*
	Hàm lấy đối xứng ảnh
	Tham số
	- srcImage: ảnh input
	- dstImage: ảnh sau khi lấy đối xứng
	- direction = 1 nếu lấy đối xứng theo trục ngang và direction = 0 nếu lấy đối xứng theo trục đứng
	- interpolator: biến chỉ định phương pháp nội suy màu
	Trả về:
	 - 0: Nếu ảnh input ko tồn tại hay ko thực hiện được phép biến đổi
	 - 1: Nếu biến đổi thành công
	*/
	int Flip(
		const cv::Mat &srcImage, 
		cv::Mat &dstImage, 
		bool direction,
		PixelInterpolate* interpolator);
};

