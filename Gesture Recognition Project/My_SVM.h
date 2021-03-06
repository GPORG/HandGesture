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
	void select_dataset(bool my_dataset);
	float test_data();

	//variables
	static Mat test_Mat;
	CvSVM SVM;
};

#endif /* MY_SVM_H_ */
