/*
 * detection2.cpp
 *
 *  Created on: Feb 8, 2014
 *      Author: Mohamed
 */

#include "detection2.h"
#include <math.h>
detection2::detection2(bool test_mood) {

	cvNamedWindow("final", CV_WINDOW_AUTOSIZE);
	capture = cvCreateCameraCapture(0);
	space = 0;
	//reserve space for all contours
	space = cvCreateMemStorage(0);
	loop = true;
	f = cvFont(1.5, 2);
	int countt = 0;
	s.select_dataset(false);
	moments = (CvMoments*) malloc(sizeof(CvMoments));
	kernal_data = {1,4,6,4,1};
	kernal=Mat(1,5,CV_32FC1,&kernal_data);
	while (loop) {
		img = cvQueryFrame(capture);
		//smooth the input image using gaussian kernal 3,3 to remove guassian noise
			cvSmooth(img, img, CV_GAUSSIAN, 5, 5);
			//removing noise
			cvErode(img, img, 0, 1);
			cvDilate(img, img, 0, 1); //Dilate

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
			area=0;
			//finding largest contour
			while (contour) {
				area = cvContourArea(contour);
				if (area > max_area) {
					max_area = area;
					largest_contour = contour;

				}
				contour = contour->h_next;
			}
			area=max_area;

			if (largest_contour && largest_contour->total > 0) {

				permiter=cvContourPerimeter(largest_contour);
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
				CvRect rounded = cvRect(hand_boundary.center.x - (max / 2) - 25,
						hand_boundary.center.y - (max / 2) - 25, max + 50, max + 50);
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

				if (test_mood) {
					float g = label_gesture();
					string gesture_name = "";
					switch ((int) g) {
						case 1:
						gesture_name = "up";
						break;
						case 2:
						gesture_name = "open";
						break;
						case 3:
						gesture_name = "capture";
						break;
						case 4:
						gesture_name = "call";
						break;
						case 5:
						gesture_name = "left";
						break;
						case 6:
						gesture_name = "right";
						break;
						case 7:
						gesture_name = "closed";
						break;
						case 8:
						gesture_name = "start";
						break;
						case 9:
						gesture_name = "up";
						break;
						case 10:
						gesture_name = "open";
						break;
						case 11:
						gesture_name = "capture";
						break;
						case 12:
						gesture_name = "call";
						break;
						default:
						gesture_name = "None";
						break;
					}
					cvPutText(img, gesture_name.c_str(), cvPoint(30, 30), &f,
							Scalar(145, 35, 100));
				}

				cvShowImage("final", img);

				cvReleaseImage(&hand_gray);
				char cr = cvWaitKey(100);
				cvReleaseImage(&gray_img);
				cvClearSeq(largest_contour);
				if (cr == 27) {
					cvReleaseImage(&img);
					cvReleaseMemStorage(&space);
					cvReleaseCapture(&capture);
					cvDestroyAllWindows();
					free(moments);
				} else if (c == 53) {
					if (!test_mood) {
						ostringstream oss;
						oss << "up" << "_" << countt << ".jpg";
						string name = oss.str();
						cvSaveImage(name.c_str(), img);
						countt++;
					}
				}

			}
		}
	}

float detection2::label_gesture() {
	features[0] = area;
	features[1] = permiter;
	features[2] = mean;
	features[3] = variance;
	features[4] = angle_theta;

	Orientation_Histo();

	My_SVM::test_Mat = Mat(1, num_of_features, CV_32FC1, features);
	return s.test_data();
}
void detection2::Orientation_Histo() {

	Mat input(hand_gray);
	Mat grad_x, grad_y, ori;
	Mat src;
	Mat hist;
	//convert gray_img into CV_32F
	input.convertTo(src, CV_32F);
	//	normalize(frame_gray, src, 0, 1*255, cv::NORM_MINMAX, CV_32F);
	Sobel(src, grad_x, CV_32F, 1, 0, 3, 1, 0, BORDER_DEFAULT);
	Sobel(src, grad_y, CV_32F, 0, 1, 3, 1, 0, BORDER_DEFAULT);
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
		features[i + 5] = hist.at<float> (i);

	//====================== get radial signature.====================================//
	int count[100];
	for (int i = 0; i < 100; i++)
		count[i] = 0;

	Mat nonZero_indeces;
	Mat binary = Mat(binary_hand);
	findNonZero(binary, nonZero_indeces);
	Point index;
	float slope = 0;
	int cx = binary.rows / 2;
	int cy = binary.cols / 2;
	for (int i = 0; i < 100; i++) {
		slope = tanf((i + 1) * 3.6 * PI / 180.0);
		for (unsigned int j = 0; j < nonZero_indeces.total(); j++) {
			index = nonZero_indeces.at<Point> (j);
			float val = cy + (slope * (index.x - cx));
			if (abs(index.y - roundf(val)) < 1) {
				count[i]++;
			}
		}
	}
	//====================== end of radial signature.====================================//
	for (int i = 0; i < 100; i++)
		features[i + 41] = count[i];

}

detection2::~detection2() {

}

