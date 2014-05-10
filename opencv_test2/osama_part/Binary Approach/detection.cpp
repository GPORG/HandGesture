/*
 * test1.cpp
 *
 *  Created on: Feb 8, 2014
 *      Author: Mohamed
 */

#include "detection.h"
#include <math.h>
detection::detection(bool test_mood) {

	cvNamedWindow("final", CV_WINDOW_AUTOSIZE);
	namedWindow("image", CV_WINDOW_AUTOSIZE);
	capture = cvCreateCameraCapture(0);
	space = 0;
	defects_space = 0;
	//reserve space for all contours
	space = cvCreateMemStorage(0);
	loop = true;
	f = cvFont(1.5, 2);
	int countt = 0;
	s.select_dataset(true);
	currentGestur = "";
	prevGesture = "";
	take_actions = false;
	int actionTime = 1;
	while (loop) {
		//get the image
		img = cvQueryFrame(capture);
		//smooth the input image using gaussian kernal 3,3 to remove noise
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
			if (area > max_area && area <= 200000 && area >= 5000 /*some thresh to hand size*/) {
				max_area = area;
				largest_contour = contour;
			}
			contour = contour->h_next;
		}

		if (largest_contour && largest_contour->total > 0 && max_area > 0) {
			//draw contour
			//			cvDrawContours(img, largest_contour, cvScalar(12, 12, 12),
			//					cvScalar(50, 5, 50), 0, 4, 0);

			hull = cvConvexHull2(largest_contour, 0, CV_CLOCKWISE, 0);
			/*draw the hull
			 *by getting its points and connect them together
			 //			 */
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

			//================================== loop on defects points=============================//
			CvPoint p; //holder a point in each iteration
			CvConvexityDefect* d;
			CvPoint s, e;
			double l1, l2, angle, dot;
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
						//						cvCircle(img, p, 5, cvScalar(0, 255, 0), -1, 0);
						//						cvPutText(img, String.c_str(), s, &f,
						//								Scalar(145, 35, 100));
						numb++;
					}
				}

			}
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
				default:
					gesture_name = "None";
					break;
				}
				cvPutText(img, gesture_name.c_str(), cvPoint(30, 30), &f,
						Scalar(145, 35, 100));
				//take the action
				if (actionTime % 10 == 0)
					apply_action(gesture_name);
				actionTime++;
			}
			cvShowImage("final", img);

			char c = cvWaitKey(100);
			cvReleaseMemStorage(&defects_space);
			cvReleaseImage(&gray_img);
			cvClearSeq(largest_contour);
			if (c == 53) {

				ostringstream oss;
				oss << "false" << "_" << countt << ".jpg";
				string name = oss.str();
				cvSaveImage(name.c_str(), img);
				countt++;
			} else if (c == 27) {
				//				writer.~FileWriter(); // when training only
				loop = false;
				cvReleaseImage(&img);
				cvReleaseImage(&gray_im);
				cvReleaseMemStorage(&space);
				cvDestroyAllWindows();
				cvReleaseCapture(&capture);

			}
		}

	}
}

float detection::label_gesture() {
	Mat img_data(hand);
	int n = 10;
	int unitWidth = img_data.cols / n; // you had image.rows / n;
	int unitHeight = img_data.rows / n;
	Mat dctImage = img_data.clone();
	float test_array[100];
	int index = 0;
	for (int i = 0; i < n; i++) { //i is row index
		// inner loop added so that more than one row of tiles written
		for (int j = 0; j < n; j++) { // j is col index
			Mat subImage = dctImage(
					Rect(j * unitWidth, i * unitHeight, unitWidth, unitHeight));
			//convert it to gray then binary
			int white = countNonZero(subImage);
			if (white > 0)
				test_array[index] = 0.0;
			else
				test_array[index] = 1.0;

			index++;
			subImage.release(); //useless

		}
	}
	img_data.release();//useless
	dctImage.release();//useless
	My_SVM::test_Mat = Mat(1, 100, CV_32FC1, test_array);
	return s.test_data();
}

void detection::apply_action(String gesture_name) {
	if (gesture_name.compare("start") == 0) {
		//check prev gesture
		if (prevGesture.compare("") == 0) {//so it is the first time to use start gesture
			currentGestur = "start";
			prevGesture = "start";
			take_actions = true;
			cout << "start.." << endl;
		} else if (prevGesture.compare("closed") == 0) {
			//here start gesture comes after the separator...then don't apply any actions
			prevGesture = "";
			currentGestur = "";
			take_actions = false;
			cout << "stop..." << endl;
		} else {
			//don't take any actions
		}
	} else if (gesture_name.compare("closed") == 0) {
		//check if we are working or not
		if (take_actions) {
			prevGesture = "closed";
			cout << "applying closed" << endl;
		}
	} else if (gesture_name.compare("open") == 0) {
		if (prevGesture.compare("closed") == 0) {
			cout << "apply open action" << endl;
			PlaySound(TEXT("c.wav"), NULL, SND_ASYNC | SND_LOOP );
			prevGesture = "open";
		}
	} else if (gesture_name.compare("capture") == 0) {
		if (prevGesture.compare("closed") == 0) {
			cout << "apply capture action" << endl;
			prevGesture = "capture";
		}
	} else if (gesture_name.compare("call") == 0) {
		if (prevGesture.compare("closed") == 0) {
			cout << "applying call action" << endl;
			prevGesture = "call";
		}
	} else if (gesture_name.compare("up") == 0) {
		if (prevGesture.compare("closed") == 0) {
			cout << "applying up action" << endl;
			prevGesture = "up";
		}
	} else if (gesture_name.compare("left") == 0) {
		if (prevGesture.compare("closed") == 0) {
			cout << "applying left action" << endl;
			PlaySound(TEXT("d.wav"), NULL, SND_ASYNC | SND_LOOP );
			prevGesture = "left";
		}
	}
}

detection::~detection() {

}

