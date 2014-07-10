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
	loop = true;
	f = cvFont(1.5, 2);
	int countt = 0;
	s.select_dataset(true);
	currentGestur = "";
	prevGesture = "";
	take_actions = false;
	int actionTime = 1;
	target_gestures_count = 0;
	take_dynamic_action = false;
	dynamic_gesture_direction = "";
	while (loop) {
		//get the image
		img = cvQueryFrame(capture);
		//smooth the input image using gaussian kernal 3,3 to remove noise
		cvSmooth(img, img, CV_GAUSSIAN, 5, 5);
		//convert to ycrcb instead of gray directly
		gray_im = cvCloneImage(img);
		cvCvtColor(img, gray_im, CV_BGR2YCrCb);
		gray_img = cvCreateImage(cvGetSize(gray_im), 8, 1);
		//		 Constants for finding range of skin color in YCrCb
		//		min_YCrCb = numpy.array([0,133,77],numpy.uint8)
		//		max_YCrCb = numpy.array([255,173,127],numpy.uint8)
		//		cvInRangeS(gray_im, cvScalar(0, 131, 80), cvScalar(255, 185, 135),
		//				gray_img);
		cvInRangeS(gray_im, cvScalar(0, 133, 77), cvScalar(255, 173, 127),
				gray_img);

		cvSmooth(gray_img, gray_img, CV_MEDIAN, 5, 5);
		hand = cvCloneImage(gray_img);
		//reserve space for all contours
		space = cvCreateMemStorage(0);
		//finding all contours in the image
		cvFindContours(gray_img, space, &contour, sizeof(CvContour),
				CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));
		//iterate through each contour
		double max_area = 0;
		double area = 0;
		//finding largest contour
		while (contour) {
			area = cvContourArea(contour);
			if (area > max_area /*&& area <= 200000 && area >= 5000 /*some thresh to hand size*/) {
				max_area = area;
				largest_contour = contour;
			}
			contour = contour->h_next;
		}
		cout << max_area << endl;
		if (largest_contour != NULL && largest_contour->total > 0 && max_area
				> 0) {
			hull = cvConvexHull2(largest_contour, 0, CV_CLOCKWISE, 0);
			/*draw the hull
			 *by getting its points and connect them together
			 //			 */
			if (test_mood) {
				pt0 = **CV_GET_SEQ_ELEM( CvPoint*, hull, hull->total - 1 );
				for (int i = 0; i < hull->total; i++) {
					pt = **CV_GET_SEQ_ELEM( CvPoint*, hull, i );
					cvLine(img, pt0, pt, cvScalar(255, 0, 0), 4);
					pt0 = pt;
				}
			}
			//============= end code for finding hull and drawing it=============================//
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
				/* part for dynamic code*/

				check_dynamic_frames2(gesture_name);

				/* end part of dynamic code */
				cvPutText(img, gesture_name.c_str(), cvPoint(30, 30), &f,
						Scalar(145, 35, 100));
				//take the action
				if (actionTime % take_action_interval == 0)
					apply_action(gesture_name, take_dynamic_action);
				actionTime++;
			}
			cvShowImage("final", img);

			char c = cvWaitKey(wait_time);
			cvReleaseImage(&gray_img);
			cvReleaseImage(&gray_im);
			cvClearSeq(hull);
			cvClearSeq(largest_contour);
			if (contour != NULL)
				cvClearSeq(contour);
			cvReleaseImage(&hand);
			if (space != NULL)
				cvReleaseMemStorage(&space);
			if (c == 53) {

				ostringstream oss;
				oss << "U" << "_" << countt << ".jpg";
				string name = oss.str();
				cvSaveImage(name.c_str(), img);
				countt++;
			} else if (c == 27) {
				loop = false;
				cvReleaseImage(&img);
				cvReleaseImage(&gray_im);
				if (space != NULL)
					cvReleaseMemStorage(&space);
				cvDestroyAllWindows();
				cvReleaseCapture(&capture);

			}
		} else {
			cvShowImage("final", img);
			cvWaitKey(wait_time);
			cvReleaseImage(&gray_img);
			//cvReleaseImage(&gray_im);
			if (contour != NULL)
				cvClearSeq(contour);
			cvReleaseImage(&hand);
			if (space != NULL)
				cvReleaseMemStorage(&space);
		}
	}
}

float detection::label_gesture() {
	Mat img_data(hand);
	int unitWidth = img_data.cols / grid_size; // you had image.rows / n;
	int unitHeight = img_data.rows / grid_size;
	Mat dctImage = img_data.clone();
	float test_array[number_of_features];
	int index = 0;
	for (int i = 0; i < grid_size; i++) { //i is row index
		// inner loop added so that more than one row of tiles written
		for (int j = 0; j < grid_size; j++) { // j is col index
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
	My_SVM::test_Mat = Mat(1, number_of_features, CV_32FC1, test_array);
	return s.test_data();
}

void detection::check_dynamic_frames2(string gesture_name) {
	if (dynamic_gesture_direction.compare("") == 0) {
		if (gesture_name.compare("up") == 0) { // next gesture up
			if (stored_dirs.size() == 0) {//first frame
				first_frame = Mat(cvCloneImage(img));
				old_hand_boundary
						= Rect(
								hand_boundary.center.x
										- (hand_boundary.size.width / 2),
								hand_boundary.center.y
										- (hand_boundary.size.height / 2),
								hand_boundary.size.width,
								hand_boundary.size.height);
				none_dir_count = 0;
				take_dynamic_action = false;
				stored_dirs.push_back("initial");
			} else { // not the first frame
				if (last_frame.cols != 0) {
					first_frame = last_frame.clone();
					old_hand_boundary.height = new_hand_boundary.height;
					old_hand_boundary.x = new_hand_boundary.x;
					old_hand_boundary.y = new_hand_boundary.y;
					old_hand_boundary.width = new_hand_boundary.width;
				}
				last_frame = Mat(cvCloneImage(img));
				new_hand_boundary
						= Rect(
								hand_boundary.center.x
										- (hand_boundary.size.width / 2),
								hand_boundary.center.y
										- (hand_boundary.size.height / 2),
								hand_boundary.size.width,
								hand_boundary.size.height);

				string dir = o_f.get_final_direction(first_frame, last_frame,
						old_hand_boundary, new_hand_boundary);
				if (dir.compare("none") == 0)
					none_dir_count++;
				else
					stored_dirs.push_back(dir);
				if (none_dir_count == max_non_dir) {
					// know the final direction
					// for simplicity now..assume the vecotr contains only one direction.
					if (stored_dirs.size() > 1) {
						dynamic_gesture_direction = stored_dirs.at(1);
						cout << "Going to " << dynamic_gesture_direction
								<< endl;
						dynamic_gesture_direction = "";
						prevGesture = "up";
					}
					none_dir_count = 0;
					stored_dirs.clear();
				}
			}
		} else { // next gesture isn't up
			none_dir_count = 0;
			stored_dirs.clear();
		}
	}
}

void detection::check_dynamic_frames(string gesture_name) {
	if (gesture_name.compare("up") == 0) { // next gesture is up
		if (target_gestures_count == 0) { // catch the frame as the first frame
			first_frame = Mat(cvCloneImage(img));
			old_hand_boundary = Rect(
					hand_boundary.center.x - (hand_boundary.size.width / 2),
					hand_boundary.center.y - (hand_boundary.size.height / 2),
					hand_boundary.size.width, hand_boundary.size.height);
		}
		target_gestures_count++;
		if (target_gestures_count == 20) { // catch the frame as the last frame
			last_frame = Mat(cvCloneImage(img));
			new_hand_boundary = Rect(
					hand_boundary.center.x - (hand_boundary.size.width / 2),
					hand_boundary.center.y - (hand_boundary.size.height / 2),
					hand_boundary.size.width, hand_boundary.size.height);
			target_gestures_count = 0;
			take_dynamic_action = true;
			dynamic_gesture_direction = o_f.get_final_direction(first_frame,
					last_frame, old_hand_boundary, new_hand_boundary);
			//			cout<<"we got "<<dynamic_gesture_direction<<endl;
			cout << "Going to " << dynamic_gesture_direction << endl;
			dynamic_gesture_direction = "";
			//						take_dynamic_action = false;
			prevGesture = "up";
		}

	} else {// next gesure isn't up
		target_gestures_count = 0;
		take_dynamic_action = false;
		dynamic_gesture_direction = "";
	}
}

void detection::apply_action(String gesture_name, bool take_dynamic_action) {
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
			PlaySound(TEXT("c.wav"), NULL, SND_ASYNC | SND_LOOP);
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
			PlaySound(TEXT("d.wav"), NULL, SND_ASYNC | SND_LOOP);
			prevGesture = "left";
		}
	}
}

detection::~detection() {

}

