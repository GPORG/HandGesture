/*
 * trainner.h
 *
 *  Created on: Mar 23, 2014
 *      Author: Mohamed
 */

#ifndef TRAINNER_H_
#define TRAINNER_H_

#include "FileReader.h"
#include "filewriter.h"
#include "string.h"
#include <iostream>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv/cv.h"
#include <ctime>
#include "My_SVM.h"
using namespace std;
using namespace cv;

class trainner {
public:
	trainner();
	void extract_feature(int row);
	void train();
	void assign_lables();
	//vars
	IplImage*hand;
	static const int dataset_size = 198; //208;//234;//177; // for train 73 for test 82 // combined 176
	static const int grid_size = 10;
	static const int number_of_features = grid_size * grid_size;
	float data[dataset_size][number_of_features];
	float labels[dataset_size];

};

#endif /* TRAINNER_H_ */
