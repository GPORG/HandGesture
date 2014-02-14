/*
 * test1.cpp
 *
 *  Created on: Feb 8, 2014
 *      Author: Mohamed
 */

#include "test1.h"
#include <math.h>
test1::test1(bool method1) {

	//initialize all windows
	//	cvNamedWindow("original", CV_WINDOW_AUTOSIZE);
	//	cvNamedWindow("gray image", CV_WINDOW_AUTOSIZE);
	//	cvNamedWindow("After thresholding", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("final", CV_WINDOW_AUTOSIZE);

	CvCapture* capture = cvCreateCameraCapture(0);
	IplImage* img;
	IplImage* gray_img;
	IplImage * gray_im; // used with ycrcb

	CvSeq* contour; //pointer to a contour.
	CvSeq* res; //hold seq of points of a contour.
	CvMemStorage* space = 0;
	CvSeq* largest_contour;
	CvSeq* hull;

	CvPoint pt0, pt;

	CvMemStorage* defects_space = 0;
	CvSeq* defects;

	//reserve space for all contours
	space = cvCreateMemStorage(0);

	bool loop = true;
	while (loop) {
		//get the image
		img = cvQueryFrame(capture);
		//img = cvLoadImage("pic5t.jpg");
		//showing the input image
		//		cvShowImage("original", img);

		//smooth the input image using gaussian kernal 3,3 to remove noise
		cvSmooth(img, img, CV_GAUSSIAN, 5, 5);

		if (!method1) {
			//convert to ycrcb instead of gray directly
			gray_im = cvCloneImage(img);
			cvCvtColor(img, gray_im, CV_BGR2YCrCb);
			gray_img = cvCreateImage(cvGetSize(gray_im), 8, 1);
			cvInRangeS(gray_im, cvScalar(0, 131, 80), cvScalar(255, 185, 135),
					gray_img);
		} else {
			//converting the original image to grayscale(make it consists of one channel)
			//
			gray_img = cvCreateImage(cvGetSize(img), 8, 1);
			cvCvtColor(img, gray_img, CV_BGR2GRAY);
		}
		//showing the gray scale image
		//		cvShowImage("gray image", gray_img);

		if (method1)
			//thresholding the gray scale image to get better results
			cvThreshold(gray_img, gray_img, 128, 255, CV_THRESH_BINARY);//objects will appear white
		//showing the thresholded image
		//		cvShowImage("After thresholding", gray_img);

		//contour part

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
		//here we have largest contour
		//draw its convex hull
		hull = cvConvexHull2(largest_contour, 0, CV_CLOCKWISE, 0);

		/*draw the hull
		 *by getting its points and connect them together
		 */

		pt0 = **CV_GET_SEQ_ELEM( CvPoint*, hull, hull->total - 1 );

		for (int i = 0; i < hull->total; i++) {
			pt = **CV_GET_SEQ_ELEM( CvPoint*, hull, i );
			cvLine(img, pt0, pt, cvScalar(255, 0, 0), 4);
			pt0 = pt;
		}

		//get defects points
		defects_space = 0;
		defects = cvConvexityDefects(largest_contour, hull, defects_space);

		// loop on defects points
		CvPoint p; //holder a point in each iteration
		for (int i = 0; i < defects->total; i++) {
			CvConvexityDefect* d =
					(CvConvexityDefect*) cvGetSeqElem(defects, i);

			if (d->depth > 10) {//get inside points only
				p.x = d->depth_point->x;
				p.y = d->depth_point->y;
				cvCircle(img, p, 5, cvScalar(0, 255, 0), -1, 0);
			}
		}

		//code for detect number of fingers
		//loop on defects points
//		CvBox2D box;
//		int num_of_fingers = 0;
//		if (largest_contour) {
//			box = cvMinAreaRect2(largest_contour);
//			for (int i = 0; i < defects->total; i++) {
//				CvConvexityDefect * d = (CvConvexityDefect*) cvGetSeqElem(
//						defects, i);
//
//				CvPoint start = cvPoint(d->start->x, d->start->y);
//				CvPoint depth = cvPoint(d->depth_point->x, d->depth_point->y);
//				CvPoint end = cvPoint(d->end->x, d->end->y);
//
//				if ((start.y < box.center.y || depth.y < box.center.y)
//						&& (start.y < depth.y) && (sqrt(
//						pow(start.x - depth.x, 2) + pow(start.y - depth.y, 2))
//						> box.size.height / 6.5)) {
//					num_of_fingers++;
//					cvLine(img, start, depth, cvScalar(45, 231, 232), 4, 0);
//					cvLine(img, depth, end, cvScalar(45, 231, 232), 4, 0);
//				}
//				cvCircle(img, start, 5, cvScalar(0, 0, 125), -1, 0);
//				cvCircle(img, depth, 5, cvScalar(0, 234, 12), -1, 0);
//
//			}
//		}
		//show image after marking it
		cvShowImage("final", img);
		//cout << "num of finger " << num_of_fingers << endl;

			while (contour) {

				//get points of the current contour
				res = cvApproxPoly(contour, sizeof(CvContour), space,
						CV_POLY_APPROX_DP, cvContourPerimeter(contour) * .02, 0);
				cout << "number of points : " << res->total << endl;

				if (res->total == 12) {
					//get the points positions of that contour
					CvPoint * pts[res->total];
					for (int i = 0; i < res->total; i++)
						pts[i] = (CvPoint*) cvGetSeqElem(res, i);
					//drawing lines connecting the contour points
					int i = 0;
					for (; i < (res->total) - 1; i++)
						cvLine(img, *pts[i], *pts[i + 1], cvScalar(255, 0, 0), 4);
					cvLine(img, *pts[i], *pts[0], cvScalar(255, 0, 0), 4);

					//show image after marking it
					cvNamedWindow("final", CV_WINDOW_AUTOSIZE);
					cvShowImage("final", img);
				}

		contour = contour->h_next;
		}
		char c = cvWaitKey(55);
		cvReleaseMemStorage(&defects_space);
		cvReleaseImage(&gray_img);
		//		cvClearSeq(largest_contour);
		//		cvClearSeq(hull);
		//		cvClearSeq(defects);
		if (c == 27) {
			loop = false;
			cvReleaseImage(&img);
			if (!method1)
				cvReleaseImage(&gray_im);
			//freeing memory
			//cvclearseq..
			cvReleaseMemStorage(&space);
			cvDestroyAllWindows();
			cvReleaseCapture(&capture);

		}
	}

}

test1::~test1() {
	// TODO Auto-generated destructor stub
}
