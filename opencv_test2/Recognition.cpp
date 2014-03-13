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
	sort(fingers.begin(), fingers.end(), compare);
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
		//	gesture_name = more_than_five();
		break;
	}
	return gesture_name;
}

string Recognition::no_fingers() {

	return "Closed";
}
string Recognition::one_finger_gestures() {
	string name = "Not Defined";
	float angle = 0;
	CvPoint finger = fingers.at(0);
	if (finger.y <= center.y) {//finger is up
		if (finger.x >= center.x) {//finger is right
			//get angle made by line from finger to center with x-axis
			angle = get_angle(center, finger, true);
			if (angle >= 0 && angle <= 30 && (abs(finger.y - center.y) <= 30))
				//point to right
				name = "right";
			else if (angle >= 70 && angle <= 90)
				//point to up
				name = "up";
		} else { //finger is left and up
			angle = get_angle(center, finger, true);
			if (angle >= 90 && angle <= 120)
				name = "up";
			else if (angle >= 160 && angle <= 180 && (abs(finger.y - center.y)
					<= 30))
				name = "left";
		}

	} else {
		//apply min distance between finger and center
		if (finger.y - center.y >= 30) {
			//finger is down..
			angle = get_angle(center, finger, true);
			if (angle >= 90 && angle <= 110)
				name = "Down";
		}
	}

	return name;
}

string Recognition::two_fingers_gestures() {
	string name = "Not Defined";
	bool leftHand = false;
	bool rightHand = false;
	CvPoint first = fingers.at(0);
	CvPoint second = fingers.at(1);
	float angle = 0;
	//call gesture
	//left hand
	if ((second.y >= center.y && second.x > center.x) && (first.y < center.y
			&& first.x < center.x)) {
		//get the angle to be with fixed interval
		angle = get_angle(center, first, second);
		//		if (angle <= 125 && angle >= 95) {
		cout << "Left Angle " << angle << endl;
		leftHand = true;
		//		}
	} else if ((second.y < center.y && second.x > center.x) && (first.y
			>= center.y && first.x < center.x)) {
		//second finger must be within min distance with center according to y values
		if (center.y - second.y >= 20) {
			//get the angle to be with fixed interval
			angle = get_angle(center, first, second);
			if (angle <= 125 && angle >= 95) {
				cout << "Right Angle " << angle << endl;
				rightHand = true;
			}
		}
	}
	if (leftHand || rightHand)
		name = "call";
	else {
		/*capture gesture: C shape inverted
		 * the two fingers are in the same direction of x and opposite in y
		 * + the angle between lines <=90 & >=30
		 */
		float angle = 0;
		if (first.x < center.x && second.x < center.x) { //right_hand
			//check y
			if ((first.y > center.y && second.y < center.y) || (first.y
					< center.y && second.y > center.y)) {
				angle = get_angle(center, first, second);
			}

		} else if (first.x > center.x && second.x > center.x) {///left_hand
			//check y
			if ((first.y > center.y && second.y < center.y) || (first.y
					< center.y && second.y > center.y)) {
				angle = get_angle(center, first, second);
			}
		}
		if (angle >= 30 && angle <= 90)//thresh distance here
			name = "Capture";
		//		cout << "Angle : " << angle << endl;
	}
	//	cout << "=========================================================="
	//			<< endl;
	//	cout << "center y : " << center.y << " first y : " << first.y << endl;
	//	cout << "center x : " << center.x << " first x : " << first.x << endl;
	//	cout << "center x : " << center.x << " second x : " << second.x << endl;
	//	cout << "=========================================================="
	//			<< endl;
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
	/*
	 * try to detect angle made by each finger line to the center
	 * assuming right hand
	 */
	CvPoint first = fingers.at(0), second = fingers.at(1), third =
			fingers.at(2), fourth = fingers.at(3), fifth = fingers.at(4);
	//first check that all fingers are up
	if (first.y < center.y && second.y < center.y && third.y < center.y
			&& fourth.y < center.y && (fifth.y < center.y || (fifth.y
			- center.y) < 10)/*or equal*/) {
		//check for x axes if need this is a way
		//another way check angles as we said before.(try it first)
		float f_angle = get_angle(center, first, true);
		cout << "First " << f_angle << endl;
		if (f_angle >= 150 && f_angle < 180) {
			float s_angle = get_angle(center, second, true);
			cout << "Second " << s_angle << endl;
			if (s_angle >= 100 && s_angle < 150) {
				float t_angle = get_angle(center, third, true);
				cout << "third " << t_angle << endl;
				if (t_angle > 90 && t_angle <= 130) {
					float fr_angle = get_angle(center, fourth, true);
					cout << "Fourth " << fr_angle << endl;
					if (fr_angle >= 70 && fr_angle <= 90) {
						float fi_angle = get_angle(center, fifth, true);
						cout << "Fifth " << fi_angle << endl;
						if (fi_angle <= 40 && fi_angle >= 0)
							return "Open";
					}
				}
			}
		}
	} else {
		// not yet
	}

	return name;
}
string Recognition::more_than_five() {
	string name = "Not Defined";
	vector<CvPoint> temp(fingers);
	fingers.clear();
	if (temp.size() > 5 && temp.size() <= 7) {
		// remove points with y > center to be 5 and then test 5 fingers gesture
		for (unsigned int i = 0; i < temp.size(); i++) {
			if (temp.at(i).y - center.y < 10) {
				fingers.push_back(temp.at(i));
			}
		}
		// need to update new center
		if (fingers.size() == 5) {
			center .x = (fingers.at(1).x + fingers.at(2).x) / 2;
			center.y = fingers.at(4).y - 10;
		}
		return detect_gesture();
	}
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
float Recognition::get_angle(CvPoint o, CvPoint p1, CvPoint p2) {
	float angle = 0;
	float dot = (p1.x - o.x) * (p2.x - o.x) + (p1.y - o.y) * (p2.y - o.y);
	float l1 = distance(o, p1);
	float l2 = distance(o, p2);
	angle = acos(dot / (l1 * l2));
	angle = angle * 180 / 3.14159;

	return angle;
}
float Recognition::distance(CvPoint p1, CvPoint p2) {
	return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));

}

bool Recognition::compare(const CvPoint &p1, const CvPoint &p2) {
	return p1.x < p2.x;
}
Recognition::~Recognition() {
}
