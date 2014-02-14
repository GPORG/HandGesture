/*
 * Sift_Impl.cpp
 *
 *  Created on: Feb 12, 2014
 *      Author: Mohamed
 */

#include "Sift_Impl.h"

Sift_Impl::Sift_Impl() {

	//load the training image
	Mat train = imread("pic4.jpg");
	Mat train_gray;
	//convert training image to gray
	cvtColor(train, train_gray, CV_BGR2GRAY);

	vector<KeyPoint> key_points; //sift key points from trained image
	Mat sift_desc; // descriptors extracted from trained image

	// create the sift detector
	SiftFeatureDetector detector;
	detector.detect(train_gray, key_points);

	//create the descriptor extractor
	SiftDescriptorExtractor extractor;
	extractor.compute(train_gray, key_points, sift_desc);

	/*
	 * The typical flow is to put the train descriptors in the matcher object with the add() method, and
	 initialize the nearest neighbor search data structures with the train() method. Then, you can find closest matches
	 or nearest neighbors for query test descriptors using the match() or knnMatch() method
	 */

	//FLANN based descriptor matcher object
	FlannBasedMatcher matcher;
	vector<Mat> trained_descriptors(1, sift_desc);
	matcher.add(trained_descriptors);
	matcher.train();

	//getting test images
	CvCapture* capture;
	capture = cvCaptureFromCAM(0);
	IplImage* test_img;
	IplImage* test_img_gray;

	while (char(waitKey(33)) != 27) {
		test_img = cvQueryFrame(capture);
		cvCvtColor(test_img, test_img_gray, CV_BGR2GRAY);

		//get the key points and descriptors from the test img
		vector<KeyPoint> test_keypoints;
		Mat test_sift_descriptor;
		detector.detect(test_img_gray, test_keypoints);
		extractor.compute(test_img_gray, test_keypoints, test_sift_descriptor);

		//match trained and test descriptors, getting 2 nearest neighbors
		vector<vector<DMatch> > matches;
		matcher.knnMatch(test_sift_descriptor, sift_desc, matches, 2);

		//============================================
		//===========copied=============
		//filter for matches according to lowe's algorithm
		vector<DMatch> final_matches;
		for (int i = 0; i < matches.size(); i++) {
			if (matches[i][0].distance < .6 * matches[i][1].distance)
				final_matches.push_back(matches[i][0]);
		}
		cout << matches.size() << endl;

		//============================================

	}
}

Sift_Impl::~Sift_Impl() {
	// TODO Auto-generated destructor stub
}
