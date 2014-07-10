/*
 * test1.cpp
 *
 *  Created on: Feb 8, 2014
 *      Author: Mohamed
 */

#include "test1.h"
#include <math.h>
using namespace cv;
test1::test1(bool method1) {

	cvNamedWindow("final", CV_WINDOW_AUTOSIZE);
	//	namedWindow("image", WINDOW_NORMAL);
	capture = cvCreateCameraCapture(0);
	space = 0;
	defects_space = 0;
	//reserve space for all contours
	space = cvCreateMemStorage(0);
	loop = true;
	f = cvFont(2, 2);
	int count = 0;
	//	bool update = true;
	while (loop) {
		//get the image
		img = cvQueryFrame(capture);
		//smooth the input image using gaussian kernal 3,3 to remove noise
		cvSmooth(img, img, CV_GAUSSIAN, 5, 5);
		//removing noise
		cvErode(img, img, 0, 1);
		cvDilate(img, img, 0, 1); //Dilate

		/*
		 * try to remove background first
		 */
		//		removeBackground(update);
		//		update = false;

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
			//			gray_img=cvCloneImage(img);
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

		if (largest_contour && largest_contour->total > 0) {
			//draw contour
			cvDrawContours(img, largest_contour, cvScalar(12, 12, 12),
					cvScalar(50, 5, 50), 0, 4, 0);

			//=============code for finding hull and drawing it=============================//
			//		//get its convex hull
			hull = cvConvexHull2(largest_contour, 0, CV_CLOCKWISE, 0);
			//
			//		/*draw the hull
			//		 *by getting its points and connect them together
			//		 */
			//
			pt0 = **CV_GET_SEQ_ELEM( CvPoint*, hull, hull->total - 1 );
			for (int i = 0; i < hull->total; i++) {
				pt = **CV_GET_SEQ_ELEM( CvPoint*, hull, i );
				cvLine(img, pt0, pt, cvScalar(255, 0, 0), 4);
				pt0 = pt;

			}
			//============= end code for finding hull and drawing it=============================//
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
			CvPoint center = cvPoint(hand_boundary.center.x,
					hand_boundary.center.y);
			int numb = 0;

			for (int i = 0; i < defects->total; i++) {
				d = (CvConvexityDefect*) cvGetSeqElem(defects, i);

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
					string String =
							static_cast<ostringstream*> (&(ostringstream()
									<< numb))->str();
					if (!(l1 < thersh && l2 < thersh && angle > 95)) {
						cvCircle(img, p, 5, cvScalar(0, 255, 0), -1, 0);
						//cvCircle(img, s, 5, cvScalar(0, 0, 255), -1, 0);
						cvPutText(img, String.c_str(), s, &f,
								Scalar(145, 35, 100));
						//cvLine(img, s, p, cvScalar(1, 1, 232), 1, 0);
						//cvLine(img, e, p, cvScalar(1, 1, 234), 1, 0);
						numb++;
						Recognition::fingers.push_back(s);
					}
				}

			}
			//draw circle at center of palm
			cvCircle(img, center, 10, cvScalar(45, 234, 234), 5, 0);
			//check gesture_name
			r.setData(center);
			string gesture_name = r.detect_gesture();
			cvPutText(img, gesture_name.c_str(), cvPoint(30, 30), &f,
					Scalar(145, 35, 100));
			//show image after marking it
//			if (gesture_name.compare("Not Defined") != 0) {
//				string Stringx = static_cast<ostringstream*> (&(ostringstream()
//						<< count))->str();
//				string xx = "out";
//				xx.append(Stringx);
//				xx .append(".jpg");
//				cvSaveImage(xx.c_str(), img);
//				count++;
//			}
			cvShowImage("final", img);

			char c = cvWaitKey(100);
			cvReleaseMemStorage(&defects_space);
			cvReleaseImage(&gray_img);
			cvClearSeq(largest_contour);
			Recognition::fingers.clear();//
			//			r.~Recognition();
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
}

void test1::removeBackground(bool updateBackground) {

	Mat original(img);
	if (original.empty())
		return;
	Mat mask;
	if (fg_img.empty())
		fg_img.create(original.size(), original.type());

	bg_sub(original, mask, updateBackground ? -1 : 0);
	fg_img = Scalar::all(0);
	original.copyTo(fg_img, mask);
	ko = fg_img;
	img = &ko;
	Mat bgimg;
	bg_sub.getBackgroundImage(bgimg);
	//
	if (!bgimg.empty())
		imshow("image", bgimg);

}
test1::~test1() {
	// TODO Auto-generated destructor stub
}
