/*
 * trainner.cpp
 *
 *  Created on: Mar 23, 2014
 *      Author: Mohamed
 */

#include "trainner.h"

trainner::trainner() {

	/*
	 * reading images, convert them to binary,extract features
	 */
	FileReader reader("paths.txt");
	FileWriter writer("out.txt");
	string line = "";
	IplImage* img;
	IplImage*gray_im;
	IplImage*gray_img;
	CvSeq* contour; //pointer to a contour.
	CvMemStorage* space = cvCreateMemStorage(0);
	;
	CvBox2D hand_boundary;
	CvSeq* largest_contour;

	int i = 0;

	while ((line = reader.readFile()) != "") {
		//load the img
		img = cvLoadImage(line.c_str());
		gray_im = cvCloneImage(img);
		cvCvtColor(img, gray_im, CV_BGR2YCrCb);
		gray_img = cvCreateImage(cvGetSize(gray_im), 8, 1);
		cvInRangeS(gray_im, cvScalar(0, 131, 80), cvScalar(255, 185, 135),
				gray_img);
		cvSmooth(gray_img, gray_img, CV_MEDIAN, 5, 5);

		hand = cvCloneImage(gray_img);
		//finding all contours in the image
		cvFindContours(gray_img, space, &contour, sizeof(CvContour),
				CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));
		//iterate through each contour
		double max_area = 0;
		double area = 0;
		//finding largest contour
		while (contour) {
			area = cvContourArea(contour);
			if (area > max_area) {
				max_area = area;
				largest_contour = contour;
			}
			contour = contour->h_next;
		}

		if (largest_contour && largest_contour->total > 0) {

			hand_boundary = cvMinAreaRect2(largest_contour);

			float max = hand_boundary.size.width;
			if (max < hand_boundary.size.height)
				max = hand_boundary.size.height;
			//copy the hand in its own image
			CvRect rounded =
					cvRect(hand_boundary.center.x - (max / 2) - 25,
							hand_boundary.center.y - (max / 2) - 25, max + 50,
							max + 50);
			cvSetImageROI(hand, rounded);
			hand = cvCloneImage(hand);
			cvShowImage("image", hand);
			cvWaitKey(1000);
			cvReleaseImage(&gray_img);
			cvClearSeq(largest_contour);
			//			string bin = extract_feature();
			//write to file
			//			writer.writeFile(bin);
			extract_feature(i);
			i++;

		}

	}
	//	writer.~FileWriter();
	reader.~FileReader();

	// now train the classifier
	train();

	//print features
	for (int i = 0; i < dataset_size; i++) {
		string xx = "";
		for (int j = 0; j < 100; j++) {
			if (data[i][j] == 0.0)
				xx.append("0");
			else
				xx.append("1");
		}
		writer.writeFile(xx);
	}
	writer.~FileWriter();
}
void trainner::extract_feature(int row) {
	Mat img_data(hand);
	int n = 10;
	int unitWidth = img_data.cols / n; // you had image.rows / n;
	int unitHeight = img_data.rows / n;
	Mat dctImage = img_data.clone();
	String binary = "";
	int index = 0;
	for (int i = 0; i < n; i++) { //i is row index
		// inner loop added so that more than one row of tiles written
		for (int j = 0; j < n; j++) { // j is col index
			Mat subImage = dctImage(
					Rect(j * unitWidth, i * unitHeight, unitWidth, unitHeight));
			//convert it to gray then binary
			int white = countNonZero(subImage);
			//			cout<<"white : "<<white<<endl;
			if (white > 0)
				data[row][index] = 0.0;
			//				binary.append("0,");
			else
				data[row][index] = 1.0;
			//				binary.append("1,");

			index++;
		}
		//		binary.append(" ");
	}
	//	return binary;
	//	return features;
}
void trainner::train() {
	//set up svm params
	CvSVMParams params;
	params.svm_type = CvSVM::C_SVC;
	params.kernel_type = CvSVM::LINEAR;
	params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 100, 1e-6);

	assign_lables();

	Mat labels_Mat(dataset_size, 1, CV_32FC1, labels);
	Mat training_Mat(dataset_size, 100, CV_32FC1, data);

	// train the svm
	CvSVM SVM;
	SVM.train(training_Mat, labels_Mat, Mat(), Mat(), params);
	CvFileStorage* fs = cvOpenFileStorage("dataset.xml", 0, CV_STORAGE_WRITE);
	if (fs) {
		SVM.write(fs, "svm");
	}
	cvReleaseFileStorage(&fs);
}

void trainner::assign_lables() {
	int index = 0;
	// up
	for (int i = 0; i < 8; i++) {
		labels[index] = 1.0;
		index++;
	}
	//open
	for (int i = 0; i < 8; i++) {
		labels[index] = 2.0;
		index++;
	}
	// capture
	for (int i = 0; i < 10; i++) {
		labels[index] = 3.0;
		index++;
	}
	//call
	for (int i = 0; i < 12; i++) {
		labels[index] = 4.0;
		index++;
	}
	//left
	for (int i = 0; i < 9; i++) {
		labels[index] = 5.0;
		index++;
	}
	//right
	for (int i = 0; i < 6; i++) {
		labels[index] = 6.0;
		index++;
	}
	//closed
	for (int i = 0; i < 9; i++) {
		labels[index] = 7.0;
		index++;
	}
}
