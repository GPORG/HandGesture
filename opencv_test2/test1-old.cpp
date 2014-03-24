/*
 * test1.cpp
 *
 *  Created on: Feb 8, 2014
 *      Author: Mohamed
 */

#include "test1.h"
#include <math.h>
test1::test1(bool method1) {

	cvNamedWindow("final", CV_WINDOW_AUTOSIZE);
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

		if (!method1) {
			//convert to ycrcb instead of gray directly
			gray_im = cvCloneImage(img);
			cvCvtColor(img, gray_im, CV_BGR2YCrCb);
			gray_img = cvCreateImage(cvGetSize(gray_im), 8, 1);
			cvInRangeS(gray_im, cvScalar(0, 131, 80), cvScalar(255, 185, 135),
					gray_img);
			cvSmooth(gray_img, gray_img, CV_MEDIAN, 5, 5);
		} else {
			//converting the original image to grayscale(make it consists of one channel)
			//
			gray_img = cvCreateImage(cvGetSize(img), 8, 1);
			cvCvtColor(img, gray_img, CV_BGR2GRAY);
		}

		if (method1)
			//thresholding the gray scale image to get better results
			cvThreshold(gray_img, gray_img, 128, 255, CV_THRESH_BINARY);//objects will appear white

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

		//get contour points
		//		contour_seq_points = cvApproxPoly(largest_contour, sizeof(CvContour),
		//				space, CV_POLY_APPROX_DP,
		//				cvContourPerimeter(largest_contour) * .02, 0);
		//		for (int i = 0; i < contour_seq_points->total; i++) {
		//			contourPoints.push_back(
		//					(CvPoint*) cvGetSeqElem(contour_seq_points, i));
		//		}

		//draw contour
		cvDrawContours(img, largest_contour, cvScalar(12, 12, 12),
				cvScalar(50, 5, 50), 0, 4, 0);

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
		//	filter_hull_points(true);
		//	int num = 0;
		//				for (int i = 0; i < hullPoint.size(); i++) {
		//					//code for detect finger point
		//					pt = *hullPoint.at(i);
		//					bool finger = is_finger_point(&pt);
		//					if (finger) {
		//						cvCircle(img, pt, 5, cvScalar(0, 0, 255), -1, 0);
		//						//	num++;
		//					}
		//				}
		//cout << "lolo : " << num << endl;
		//get defects points
		defects_space = 0;
		defects = cvConvexityDefects(largest_contour, hull, defects_space);
		hand_boundary = cvMinAreaRect2(largest_contour);
		float l;
		if (hand_boundary.size.height >= hand_boundary.size.width)
			l = hand_boundary.size.height;
		else
			l = hand_boundary.size.width;
		float thersh = l / 4;

		//================================== loop on defects points=============================//
		CvPoint p; //holder a point in each iteration
		CvConvexityDefect* d;
		CvPoint s, e;
		double l1, l2, angle, dot;
		//		p.x=hand_boundary.center.x;
		//		p.y=hand_boundary.center.y;
		//		cvCircle(img,p,8,cvScalar(0,255,123),1,0);
		CvPoint center = cvPoint(0, 0);
		for (int i = 0; i < defects->total; i++) {
			d = (CvConvexityDefect*) cvGetSeqElem(defects, i);

			center.x += d->depth_point->x;
			center.y += d->depth_point->y;

			if (d->depth > 10) {//get inside points only
				p.x = d->depth_point->x;
				p.y = d->depth_point->y;
				s.x = d->start->x;
				s.y = d->start->y;
				e.x = d->end->x;
				e.y = d->end->y;
				l1 = sqrt(pow(s.x - p.x, 2) + pow(s.y - p.y, 2));
				l2 = sqrt(pow(e.x - p.x, 2) + pow(e.y - p.y, 2));
				dot = (s.x - p.x) * (e.x - p.x) + (s.y - p.y) * (e.y - p.y);
				angle = acos(dot / (l1 * l2));
				angle = angle * 180 / (3.14159);
				if (!(l1 < thersh && l2 < thersh && angle > 95)) {
					cvCircle(img, p, 5, cvScalar(0, 255, 0), -1, 0);
					cvCircle(img, s, 5, cvScalar(0, 0, 255), -1, 0);
					cvLine(img, s, p, cvScalar(1, 1, 232), 1, 0);
					cvLine(img, e, p, cvScalar(1, 1, 234), 1, 0);
					finger_points.push_back(&s);
				}
			}
		}
		//get mean poisition of defect point to determine center
		center.x=round(center.x/defects->total);
		center.y=round(center.y/defects->total);
		cvCircle(img,center,10,cvScalar(45,234,234),5,0);


		//		CvBox2D r = cvFitEllipse2(largest_contour);
		//		cvEllipseBox(img, r, cvScalar(0, 0, 0), -1, 0);
		//===========================end loop on defect===============================================//
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

		//			while (contour) {
		//
		//				//get points of the current contour
		//				res = cvApproxPoly(contour, sizeof(CvContour), space,
		//						CV_POLY_APPROX_DP, cvContourPerimeter(contour) * .02, 0);
		//				cout << "number of points : " << res->total << endl;
		//
		//				if (res->total == 12) {
		//					//get the points positions of that contour
		//					CvPoint * pts[res->total];
		//					for (int i = 0; i < res->total; i++)
		//						pts[i] = (CvPoint*) cvGetSeqElem(res, i);
		//					//drawing lines connecting the contour points
		//					int i = 0;
		//					for (; i < (res->total) - 1; i++)
		//						cvLine(img, *pts[i], *pts[i + 1], cvScalar(255, 0, 0), 4);
		//					cvLine(img, *pts[i], *pts[0], cvScalar(255, 0, 0), 4);
		//
		//					//show image after marking it
		//					cvNamedWindow("final", CV_WINDOW_AUTOSIZE);
		//					cvShowImage("final", img);
		//				}
		//
		//		contour = contour->h_next;
		//		}
		char c = cvWaitKey(55);
		cvReleaseMemStorage(&defects_space);
		cvReleaseImage(&gray_img);
		cvClearSeq(largest_contour);
		//		cvClearSeq(contour_seq_points);
		//		contourPoints.clear();
		//		hullPoint.clear();
		//		filtered_hull_points.clear();
		finger_points.clear();
		if (c == 27) {
			loop = false;
			cvReleaseImage(&img);
			if (!method1)
				cvReleaseImage(&gray_im);
			cvReleaseMemStorage(&space);
			cvDestroyAllWindows();
			cvReleaseCapture(&capture);

		}
	}

}

test1::~test1() {
	// TODO Auto-generated destructor stub
}
