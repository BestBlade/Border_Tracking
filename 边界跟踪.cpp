#include <iostream>
#include<opencv2/core/core.hpp>  
#include<opencv2/highgui/highgui.hpp>  
#include"opencv2/imgproc/imgproc.hpp"
#include <stdio.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <stack>

using namespace std;
using namespace cv;
//================二值化图像==================//
int myOtsu(Mat img) {
	if (img.channels() != 1) {
		cerr << "please input the gray picture" << endl;
	}

	float pixel_porb[256] = { 0 };
	for (int x = 0; x < img.rows; x++) {
		for (int y = 0; y < img.cols; y++) {
			pixel_porb[img.at<uchar>(x, y)]++;
		}
	}
	for (int i = 0; i < 256; i++) {
		pixel_porb[i] /= img.rows * img.cols;
	}

	float gmax = 0;
	int threshould = 0;
	for (int i = 0; i < 256; i++) {
		float w0 = 0;
		float w1 = 0;
		float u0 = 0;
		float u1 = 0;
		for (int j = 0; j < 256; j++) {
			if (i <= j) {
				w0 += pixel_porb[j];
				u0 += j * pixel_porb[j];
			}
			else {
				w1 += pixel_porb[j];
				u1 += j * pixel_porb[j];
			}
		}
		//平均灰度
		float u = u0 + u1;
		u0 /= w0;
		u1 /= w1;
		float g = w0 * pow((u - u0), 2) + w1 * pow((u - u1), 2);
		if (g > gmax) {
			gmax = g;
			threshould = i;
		}
	}
	return threshould;
}

Mat mythreshould(Mat img, int threshould) {
	if (img.channels() != 1) {
		cerr << "please input the gray picture" << endl;
	}

	Mat bw(img.rows, img.cols, img.type());
	for (int x = 0; x < img.rows; x++) {
		for (int y = 0; y < img.cols; y++) {
			if (img.at<uchar>(x, y) <= threshould) {
				bw.at<uchar>(x, y) = 0;
			}
			else {
				bw.at<uchar>(x, y) = 255;
			}
		}
	}
	return bw;
}
//============================================//

//寻找到起始点像素坐标
Point firstWhite(Mat bw) {
	for (int x = 0; x < bw.rows; x++) {
		for (int y = 0; y < bw.cols; y++) {
			if (bw.at<uchar>(x, y) == 255) {
				return Point(x, y);
			}
		}
	}
}

Mat borderTracking(Mat bw) {
	Mat dst(bw.size(), CV_8U, Scalar::all(0));

	Mat temp = bw.clone();
	//创建边界存贮栈
	stack<Point> border;
	//从右开始，顺时针
	int dir[8][2] = { {0,1},{1,1} ,{1,0},{1,-1},{0,-1}, {-1,-1},{-1,0},{-1,1} };
	//寻找开始点
	Point start = firstWhite(bw);
	border.push(start);
	//初始方向
	int next_dir = 0;
	cout << "(" << start.x << "," << start.y << ")" << "    " << "Start dir:" << next_dir << endl;
	dst.at<uchar>(start.x, start.y) = 255;
	//先计算next坐标
	Point next_point(start.x + dir[next_dir][0], start.y + dir[next_dir][1]);
	//再计算next角度
	for (int i = 0; i < 8; i++) {
		//临时存储遍历角度
		int now_dir = (i + next_dir) % 8;
		//临时存储遍历坐标
		Point temp(next_point.x + dir[now_dir][0], next_point.y + dir[now_dir][1]);

		if (bw.at<uchar>(temp.x, temp.y) == 255) {
			//计算垂直方向
			next_point.x += dir[now_dir][0];
			next_point.y += dir[now_dir][1];
			next_dir = (now_dir + 6) % 8;
			break;
		}
	}

	while (next_point != start) {
		border.push(next_point);
		cout << "(" << next_point.x << "," << next_point.y << ")" << "    " << "Start dir:" << next_dir << endl;
		dst.at<uchar>(next_point.x, next_point.y) = 255;

		for (int i = 0; i < 8; i++) {
			int now_dir = (i + next_dir) % 8;

			Point temp(next_point.x + dir[now_dir][0], next_point.y + dir[now_dir][1]);

			if (bw.at<uchar>(temp.x, temp.y) == 255) {
				//计算垂直方向
				next_point.x += dir[now_dir][0];
				next_point.y += dir[now_dir][1];
				next_dir = (now_dir + 6) % 8;
				break;
			}
		}
	}

	return dst;
}

int main() {
	Mat img = imread("C://Users//Chrysanthemum//Desktop//4.png", 0);

	int otsuthreshould = myOtsu(img);
	Mat bw = mythreshould(img, otsuthreshould);

	Mat dst = borderTracking(bw);

	imshow("origin pic", img);
	imshow("bw pic", bw);
	imshow("border", dst);
	waitKey();
}