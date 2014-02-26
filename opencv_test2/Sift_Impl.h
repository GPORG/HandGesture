/*
 * Sift_Impl.h
 *
 *  Created on: Feb 12, 2014
 *      Author: Mohamed
 */

#ifndef SIFT_IMPL_H_
#define SIFT_IMPL_H_

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/features2d/features2d.hpp>
#include "opencv2/features2d/features2d.hpp"
#include <opencv2/nonfree/features2d.hpp>
#include "opencv2/nonfree/features2d.hpp"

#include <iostream>
using namespace cv;
using namespace std;


class Sift_Impl {
public:
	Sift_Impl();
	virtual ~Sift_Impl();
};

#endif /* SIFT_IMPL_H_ */
