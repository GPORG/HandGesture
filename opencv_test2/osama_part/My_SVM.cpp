/*
 * My_SVM.cpp
 *
 *  Created on: Mar 21, 2014
 *      Author: Mohamed
 */

#include "My_SVM.h"

Mat My_SVM::test_Mat;

My_SVM::My_SVM() {
	SVM.load("dataset.xml");
}
void My_SVM::train_data() {
	float labels[27] = { };//old
	Mat labels_Mat(27, 1, CV_32FC1, labels);

	float training[27][64] = { };// old
	Mat training_Mat(27, 64, CV_32FC1, training);

	//set up svm params
	CvSVMParams params;
	params.C = .1;
	params.svm_type = CvSVM::C_SVC;
	params.kernel_type = CvSVM::LINEAR;
	params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 10000000, 1e-6);

	// train the svm
	CvSVM SVM;
	SVM.train(training_Mat, labels_Mat, Mat(), Mat(), params);
	CvFileStorage* fs = cvOpenFileStorage("dataset.xml", 0, CV_STORAGE_WRITE);
	if (fs) {
		SVM.write(fs, "svm");
	}
	cvReleaseFileStorage(&fs);

}
float My_SVM::test_data() {
	float response = SVM.predict(test_Mat);
	return response;

}
My_SVM::~My_SVM() {
}
