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
	FileReader reader("pathsF.txt");
	FileWriter writer("out.txt");
	string line = "";
	IplImage* img;
	IplImage*gray_im;
	IplImage*gray_img;
	CvSeq* contour; //pointer to a contour.
	CvMemStorage* space = cvCreateMemStorage(0);
	CvBox2D hand_boundary;
	CvSeq* largest_contour;
	int i = 0;
	int all_time[dataset_size];
	while ((line = reader.readFile()) != "") {
		std::clock_t start;

		start = std::clock();

		//load the img
		img = cvLoadImage(line.c_str());
		cvSmooth(img, img, CV_GAUSSIAN, 5, 5);
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
//			cvShowImage("image", hand);
			cvWaitKey(0);
			cvReleaseImage(&gray_img);
			cvClearSeq(largest_contour);
			//			string bin = extract_feature();
			//write to file
			//			writer.writeFile(bin);
			extract_feature(i);

		} else {
			for (int j = 0; j < number_of_features; j++)
				data[i][j] = 0.0;
		}
		int timm = (std::clock() - start) / (double) (CLOCKS_PER_SEC / 1000);
		all_time[i] = timm;
		i++;

	}
	int sum = 0;

	for (int i = 0; i < dataset_size; i++) {
		sum += all_time[i];
	}
	sum = sum / dataset_size;
	cout << sum << endl;
	reader.~FileReader();

	// now train the classifier
	train();

	//print features
	for (int i = 0; i < dataset_size; i++) {
		ostringstream oss;
		if (i < 11)
			oss << "up";
		else if (i < 30)
			oss << "open";
		else if (i < 60)
			oss << "capture";
		else if (i < 83)
			oss << "call";
		else if (i < 101)
			oss << "left";
		else if (i < 125)
			oss << "right";
		else if (i < 136)
			oss << "closed";
		else if (i < 149)
			oss << "start";
		else if (i < 159)
			oss << "Lup";
		else if (i < 173)
			oss << "Lopen";
		else if (i < 190)
			oss << "Lcapture";
		else if (i < 197)
			oss << "Lcall";
		oss << ",";
		for (int j = 0; j < number_of_features; j++) {
			if (data[i][j] == 0.0)
				oss << "0";
			else
				oss << "1";
			oss << ",";
		}
		string name = oss.str();
		writer.writeFile(name);
	}
	writer.~FileWriter();
}
void trainner::extract_feature(int row) {
//	stringstream oss;
//	oss<<row<<"_";
//	if (row >= 11 && row <30){
//		oss<<"openBinary.jpg";
//		cvSaveImage(oss.str().c_str(), hand);
//	}
	Mat img_data(hand);
	int unitWidth = img_data.cols / grid_size; // you had image.rows / n;
	int unitHeight = img_data.rows / grid_size;
	Mat dctImage = img_data.clone();
	String binary = "";
	int index = 0;
	for (int i = 0; i < grid_size; i++) { //i is row index
		// inner loop added so that more than one row of tiles written
		for (int j = 0; j < grid_size; j++) { // j is col index
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
	Mat training_Mat(dataset_size, number_of_features, CV_32FC1, data);

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
	////	// up
	//	for (int i = 0; i < 5; i++) {
	//		labels[index] = 1.0;
	//		index++;
	//	}
	//	//open
	//	for (int i = 0; i < 11; i++) {
	//		labels[index] = 2.0;
	//		index++;
	//	}
	//	// capture
	//	for (int i = 0; i < 19; i++) {
	//		labels[index] = 3.0;
	//		index++;
	//	}
	//	//call
	//	for (int i = 0; i < 12; i++) {
	//		labels[index] = 4.0;
	//		index++;
	//	}
	//	//left
	//	for (int i = 0; i < 11; i++) {
	//		labels[index] = 5.0;
	//		index++;
	//	}
	//	//right
	//	for (int i = 0; i < 11; i++) {
	//		labels[index] = 6.0;
	//		index++;
	//	}
	//	//closed
	//	for (int i = 0; i < 6; i++) {
	//		labels[index] = 7.0;
	//		index++;
	//	}
	//	//start
	//	for (int i = 0; i < 7; i++) {
	//		labels[index] = 8.0;
	//		index++;
	//	}
	// separate test
	//	//	// up
	//		for (int i = 0; i < 8; i++) {
	//			labels[index] = 1.0;
	//			index++;
	//		}
	//		//open
	//		for (int i = 0; i < 8; i++) {
	//			labels[index] = 2.0;
	//			index++;
	//		}
	//		// capture
	//		for (int i = 0; i < 10; i++) {
	//			labels[index] = 3.0;
	//			index++;
	//		}
	//		//call
	//		for (int i = 0; i < 12; i++) {
	//			labels[index] = 4.0;
	//			index++;
	//		}
	//		//left
	//		for (int i = 0; i < 9; i++) {
	//			labels[index] = 5.0;
	//			index++;
	//		}
	//		//right
	//		for (int i = 0; i < 6; i++) {
	//			labels[index] = 6.0;
	//			index++;
	//		}
	//		//closed
	//		for (int i = 0; i < 9; i++) {
	//			labels[index] = 7.0;
	//			index++;
	//		}
	//		//start
	//		for (int i = 0; i < 11; i++) {
	//			labels[index] = 8.0;
	//			index++;
	//		}
	//
	// after first combine
	// up
	for (int i = 0; i < /*14*/11; i++) {
		labels[index] = 1.0;
		index++;
	}
	//open
	for (int i = 0; i < /*22*/19; i++) {
		labels[index] = 2.0;
		index++;
	}
	// capture
	for (int i = 0; i < /*38*/30; i++) {
		labels[index] = 3.0;
		index++;
	}
	//call
	for (int i = 0; i < /*25*/23; i++) {
		labels[index] = 4.0;
		index++;
	}
	//left
	for (int i = 0; i < 18/*22*/; i++) {
		labels[index] = 5.0;
		index++;
	}
	//right
	for (int i = 0; i < /*30*/24; i++) {
		labels[index] = 6.0;
		index++;
	}
	//closed
	for (int i = 0; i < 11/*15*/; i++) {
		labels[index] = 7.0;
		index++;
	}
	//start
	for (int i = 0; i < 13/*18*/; i++) {
		labels[index] = 8.0;
		index++;
	}
	//Lup
	for (int i = 0; i < 10; i++) {
		labels[index] = 9;
		index++;
	}
	//Lopen
	for (int i = 0; i < 14/*15*/; i++) {
		labels[index] = 10;
		index++;
	}
	//Lcapture
	for (int i = 0; i < 17; i++) {
		labels[index] = 11;
		index++;
	}
	//Lcall
	for (int i = 0; i < 7; i++) {
		labels[index] = 12;
		index++;
	}
	//false
	for (int i = 0; i < 1; i++) {
		labels[index] = 13;
		index++;
	}
}
