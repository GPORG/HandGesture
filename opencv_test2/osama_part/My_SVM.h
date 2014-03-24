/*
 * My_SVM.h
 *
 *  Created on: Mar 21, 2014
 *      Author: Mohamed
 */

#ifndef MY_SVM_H_
#define MY_SVM_H_
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>
#include <opencv/cv.h>
using namespace std;
using namespace cv;
class My_SVM {
public:
	My_SVM();
	virtual ~My_SVM();
	void train_data();
	float test_data();
	static Mat test_Mat;
};

#endif /* MY_SVM_H_ */
