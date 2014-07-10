/*
 * yahiaTest.cpp
 *
 *  Created on: Feb 19, 2014
 *      Author: Mohamed
 */

#include "yahiaTest.h"

yahiaTest::yahiaTest() {

	cvNamedWindow("final", CV_WINDOW_AUTOSIZE);
	//	cvNamedWindow("bin", CV_WINDOW_AUTOSIZE);
	capture = cvCreateCameraCapture(0);
	space = 0;
	defects_space = 0;
	//reserve space for all contours
	space = cvCreateMemStorage(0);
	loop = true;

	while (loop) {
		//get the image
		img = cvQueryFrame(capture);
		//smooth the input image using gaussian kernal 3,3 to remove noise
		cvSmooth(img, img, CV_GAUSSIAN, 5, 5);
		//convert to ycrcb instead of gray directly
		gray_im = cvCloneImage(img);
		cvCvtColor(img, gray_im, CV_BGR2YCrCb);
		gray_img = cvCreateImage(cvGetSize(gray_im), 8, 1);
		cvInRangeS(gray_im, cvScalar(0, 131, 80), cvScalar(255, 185, 135),
				gray_img);
		cvSmooth(gray_img, gray_img, CV_MEDIAN, 5, 5);

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

		//draw contour
		if (largest_contour && largest_contour->total > 0) {
			cvDrawContours(img, largest_contour, cvScalar(12, 12, 12),
					cvScalar(50, 5, 50), 0, 4, 0);

			// get rotated rect around the palm
			CvBox2D r = cvFitEllipse2(largest_contour);
			cvEllipseBox(gray_img, r, cvScalar(0, 0, 0), -1, 0);

			//get trh rectangl hold the contour
			CvBox2D rr = cvMinAreaRect2(largest_contour);

			//fill the ellipse with zeros
			CvRect rounded = cvRect(rr.center.x - (rr.size.width / 2),
					rr.center.y - (rr.size.height / 2), rr.size.width,
					rr.size.height);
			cvSetImageROI(gray_img, rounded);

			CvSeq* cc;
			CvMemStorage* ss = cvCreateMemStorage(0);
			cvFindContours(gray_img, ss, &cc, sizeof(CvContour), CV_RETR_LIST,
					CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));

			while (cc) {
				cvDrawContours(img, cc, cvScalar(12, 255, 255),
						cvScalar(232, 233, 244), 0, 4, 0);
				cc = cc->h_next;

			}
			//get its convex hull
			hull = cvConvexHull2(largest_contour, 0, CV_CLOCKWISE, 0);

			/*draw the hull
			 *by getting its points and connect them together
			 */

			pt0 = **CV_GET_SEQ_ELEM( CvPoint*, hull, hull->total - 1 );
			//hullPoint.push_back(
			//	&**CV_GET_SEQ_ELEM( CvPoint*, hull, hull->total - 1 ));
			for (int i = 0; i < hull->total; i++) {
				pt = **CV_GET_SEQ_ELEM( CvPoint*, hull, i );
				//double dist = sqrt(pow(pt0.x - pt. x, 2) + pow(pt0.y - pt.y, 2));
				//if (dist > 10)
				//	cvCircle(img, pt0, 5, cvScalar(255, 255, 0), -1, 0);
				//	hullPoint.push_back(&**CV_GET_SEQ_ELEM( CvPoint*, hull, i ));
				cvLine(img, pt0, pt, cvScalar(255, 0, 0), 4);
				pt0 = pt;

			}
			//get defects points
			defects_space = 0;
			defects = cvConvexityDefects(largest_contour, hull, defects_space);
			hand_boundary = cvMinAreaRect2(largest_contour);
			float l = hand_boundary.size.height;
		}
		//show image after marking it
		cvShowImage("final", img);
		//		cvShowImage("bin",gray_img);
		char c = cvWaitKey(55);
		cvReleaseMemStorage(&defects_space);
		cvReleaseImage(&gray_img);
		cvClearSeq(largest_contour);
		if (c == 27) {
			loop = false;
			cvReleaseImage(&img);
			cvReleaseImage(&gray_im);
			cvReleaseMemStorage(&space);
			cvDestroyAllWindows();
			cvReleaseCapture(&capture);

		}
	}

}

yahiaTest::~yahiaTest() {
	// TODO Auto-generated destructor stub
}
