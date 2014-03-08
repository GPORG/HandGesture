/*
 * Recognition.cpp
 *
 *  Created on: Mar 6, 2014
 *      Author: Mohamed
 */

#include "Recognition.h"

vector<CvPoint> Recognition::fingers;

Recognition::Recognition() {

}
void Recognition::setData(CvPoint centero) {

	center.x = centero.x;
	center.y = centero.y;
}
string Recognition::detect_gesture() {

	string gesture_name = "Not Defined";

	switch (fingers.size()) {
	case 0:
		gesture_name = no_fingers();
		break;
	case 1:
		gesture_name = one_finger_gestures();
		break;
	case 2:
		gesture_name = two_fingers_gestures();
		break;
	case 3:
		gesture_name = three_fingers_gestures();
		break;
	case 4:
		gesture_name = four_fingers_gestures();
		break;
	case 5:
		gesture_name = five_fingers_gestures();
		break;
	default:
		//later // error as usual
		break;
	}
	return gesture_name;
}

string Recognition::no_fingers() {
	return "Not Defined";;
}
string Recognition::one_finger_gestures() {
	string name = "Not Defined";
	float angle = 0;
	CvPoint finger = fingers.at(0);
	if (finger.y <= center.y) {//finger is up
		if (finger.x >= center.x) {//finger is right
			//get angle made by line from finger to center with x-axis
			angle = get_angle(center, finger, true);
			if (angle >= 0 && angle <= 30)
				//point to right
				name = "right";
			else if (angle >= 70 && angle <= 90)
				//point to up
				name = "up";
		} else { //finger is left and up
			angle = get_angle(center, finger, true);
			if (angle >= 90 && angle <= 120)
				name = "up";
			else if (angle >= 150 && angle <= 180)
				name = "left";
		}

	} else {
		//finger is down..not yet
	}

	return name;
}

string Recognition::two_fingers_gestures() {
	string name = "Not Defined";
	bool leftHand = false;
	bool rightHand = false;
	CvPoint first = fingers.at(0);
	CvPoint second = fingers.at(1);
	if ((second.y < center.y && second.x < center.x) && (first.y > center.y
			&& first.x > center.x))
		leftHand = true;
	else if ((second.y < center.y && second.x > center.x) && (first.y
			> center.y && first.x < center.x))
		rightHand = true;
	if (leftHand || rightHand)
		name = "call";
	return name;
}

string Recognition::three_fingers_gestures() {
	string name = "Not Defined";
	return name;
}

string Recognition::four_fingers_gestures() {
	string name = "Not Defined";
	return name;
}

string Recognition::five_fingers_gestures() {
	string name = "Not Defined";
	return name;
}

float Recognition::get_angle(CvPoint o, CvPoint p, bool x_axes) {
	float angle = 0;
	//make dot product
	CvPoint helper;
	if (x_axes) {
		helper .x = o.x + 1;
		helper.y = o.y;
	} else {
		helper .x = o.x;
		helper.y = o.y - 1;
	}
	// get the dot product
	float dot = (p.x - o.x) * (helper.x - o.x) + (p.y - o.y) * (helper.y - o.y);
	//get magnitudes
	float l1 = distance(o, p);
	float l2 = distance(o, helper);
	angle = acos(dot / (l1 * l2));
	angle = angle * 180 / 3.14159;

	return angle;

}
float Recognition::distance(CvPoint p1, CvPoint p2) {
	return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));

}

Recognition::~Recognition() {
}
