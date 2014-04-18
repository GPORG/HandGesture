/*
 * trainner2.cpp
 *
 *  Created on: Feb 8, 2014
 *      Author: Mohamed
 */

#include "trainner2.h"
#include <math.h>
using namespace cv;
trainner2::trainner2() {

	cvNamedWindow("final", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("binary", CV_WINDOW_AUTOSIZE);
	space = 0;
	//reserve space for all contours
	space = cvCreateMemStorage(0);
	f = cvFont(2, 2);
	int row = 0;
	FileReader reader("paths.txt");
	FileWriter writer("out2.txt");
	string line = "";
	moments = (CvMoments*) malloc(sizeof(CvMoments));
	kernal_data = {1,4,6,4,1};
	kernal=Mat(1,5,CV_32FC1,&kernal_data);

	while ((line = reader.readFile()) != "") {
		//get the image
			img = cvLoadImage(line.c_str());
			//smooth the input image using gaussian kernal 3,3 to remove noise
			cvSmooth(img, img, CV_GAUSSIAN, 5, 5);
			//removing noise
			//		cvErode(img, img, 0, 1);
			//		cvDilate(img, img, 0, 1); //Dilate

			//convert to ycrcb instead of gray directly
			gray_im = cvCloneImage(img);
			cvCvtColor(img, gray_im, CV_BGR2YCrCb);
			gray_img = cvCreateImage(cvGetSize(gray_im), 8, 1);
			cvInRangeS(gray_im, cvScalar(0, 131, 80), cvScalar(255, 185, 135),
					gray_img);
			cvSmooth(gray_img, gray_img, CV_MEDIAN, 5, 5);
			//get gray img
			hand_gray = cvCreateImage(cvGetSize(img), 8, 1);
			cvCvtColor(img, hand_gray, CV_BGR2GRAY);
			//get binary image for hand
			binary_hand = cvCloneImage(gray_img);
			//finding all contours in the image
			cvFindContours(gray_img, space, &contour, sizeof(CvContour),
					CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));
			//iterate through each contour
			double max_area = 0;
			double area=0;
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
				//perimter

				hull = cvConvexHull2(largest_contour, 0, CV_CLOCKWISE, 0);

				pt0 = **CV_GET_SEQ_ELEM( CvPoint*, hull, hull->total - 1 );
				for (int i = 0; i < hull->total; i++) {
					pt = **CV_GET_SEQ_ELEM( CvPoint*, hull, i );
					cvLine(img, pt0, pt, cvScalar(255, 0, 0), 4);
					pt0 = pt;

				}
				//getting hand area
				hand_boundary = cvMinAreaRect2(largest_contour);

				float max = hand_boundary.size.width;
				if (max < hand_boundary.size.height)
				max = hand_boundary.size.height;
				//copy the hand in its own image
				CvRect rounded = cvRect(hand_boundary.center.x - (max / 2),
						hand_boundary.center.y - (max / 2), max, max);
				cvSetImageROI(hand_gray, rounded);
				hand_gray = cvCloneImage(hand_gray);//gray image containing the hand
				cvSetImageROI(binary_hand, rounded);
				binary_hand = cvCloneImage(binary_hand);// binary img containg the hand
				cvShowImage("binary", binary_hand);
				//calculate mean
				Scalar means, stds;
				cv::meanStdDev((Mat(hand_gray)), means, stds);
				mean = means.val[0];
				variance = powf(stds.val[0], 2);

				// get moments
				cvMoments(largest_contour, moments, 0);
				xc = moments->m10 / moments->m00;
				yc = moments->m01 / moments->m00;

				//some temp calcs to get orientation angle
				float a = (moments->m20 / moments->m00) - pow(xc, 2);
				float b = 2 * ((moments->m10 / moments->m00) - (xc * yc));
				float c = (moments->m02 / moments->m00) - pow(yc, 2);

				angle_theta = (atan2f(b, (a - c))) / 2;
				angle_theta = (angle_theta * 180) / (3.14159);

				//processing for orientation histogram feature
				Orientation_Histo(Mat(hand_gray),row);
				cvReleaseImage(&hand_gray);

				//save features
				data[row][0] = mean;
				data[row][1] = variance;
				data[row][2] = angle_theta;
				row++;

				cvShowImage("final", img);

				char cr = cvWaitKey(100);
				cvReleaseImage(&gray_img);
				cvClearSeq(largest_contour);
				if (cr == 27) {
					cvReleaseImage(&img);
					cvReleaseMemStorage(&space);
					cvDestroyAllWindows();
				}

			}
		}
		reader.~FileReader();
		train();

		//	//print features
		for (int i = 0; i < dataset_size; i++) {
			ostringstream oss;
			for (int j = 0; j < num_of_features; j++) {
				oss << data[i][j] << ",";
			}
			string name = oss.str();
			writer.writeFile(name);
		}
		writer.~FileWriter();
		//releasing
		cvReleaseImage(&img);
		cvReleaseMemStorage(&space);
		cvDestroyAllWindows();
		free( moments);

	}

void trainner2::train() {
	CvSVMParams params;
	params.svm_type = CvSVM::C_SVC;
	params.kernel_type = CvSVM::LINEAR;
	params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 100, 1e-6);

	assign_labels();

	Mat labels_Mat(dataset_size, 1, CV_32FC1, labels);
	Mat training_Mat(dataset_size, num_of_features, CV_32FC1, data);

	// train the svm
	CvSVM SVM;
	SVM.train(training_Mat, labels_Mat, Mat(), Mat(), params);
	CvFileStorage* fs = cvOpenFileStorage("dataset2.xml", 0, CV_STORAGE_WRITE);
	if (fs) {
		SVM.write(fs, "svm");
	}
	cvReleaseFileStorage(&fs);

}

void trainner2::assign_labels() {
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
	//start
	for (int i = 0; i < 11; i++) {
		labels[index] = 8.0;
		index++;
	}
}

void trainner2::Orientation_Histo(Mat gray_img, int row) {

	Mat grad_x, grad_y,/* magnit,*/ori;
	Mat src;
	Mat hist;
	int scale = 1;
	int delta = 0;
	int ddepth = CV_32F;

	//convert gray_img into CV_32F
	gray_img.convertTo(src, CV_32F);
	//	normalize(frame_gray, src, 0, 1*255, cv::NORM_MINMAX, CV_32F);
	Sobel(src, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
	Sobel(src, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
	//	magnitude(grad_x, grad_y, magnit);//get gradient magnitude
	phase(grad_x, grad_y, ori, true);//get gradient orientation

	//calculate histogram for gradient orientation
	float range[] = { 0, 256 };
	const float* histRange = { range };
	int bins = 36;
	calcHist(&ori, 1, 0, Mat(), hist, 1, &bins, &histRange, true, false);
	//apply filter on orie matrix
	filter2D(hist, hist, -1, kernal, Point(-1, -1), 0, BORDER_DEFAULT);
	// save the histogram count values for each bin
	for (int i = 0; i < 36; i++)
		data[row][i + 3] = hist.at<float> (i);

	//for saving and displaying purpose
	//	normalize(grad_x, grad_x, 0.0, 255.0, cv::NORM_MINMAX, CV_8U);
	//	normalize(grad_y, grad_y, 0.0, 255.0, cv::NORM_MINMAX, CV_8U);
	//	normalize(final, final, 0.0, 255.0, cv::NORM_MINMAX, CV_8U);
	//	normalize(ori, ori, 0.0, 255.0, cv::NORM_MINMAX, CV_8U);
	//	imshow("M", ori);
	//	imshow("Mf", final);

}
trainner2::~trainner2() {
}
