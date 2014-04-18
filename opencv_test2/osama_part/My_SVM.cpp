/*
 * My_SVM.cpp
 *
 *  Created on: Mar 21, 2014
 *      Author: Mohamed
 */

#include "My_SVM.h"

Mat My_SVM::test_Mat;

My_SVM::My_SVM() {

}
float My_SVM::test_data() {
	float response = SVM.predict(test_Mat);
	return response;

}
void My_SVM::select_dataset(bool my_dataset) {
	if (my_dataset)
		SVM.load("dataset.xml");
	else
		SVM.load("dataset2.xml");
}
My_SVM::~My_SVM() {
}
