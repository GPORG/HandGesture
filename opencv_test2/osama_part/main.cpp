//#include <iostream>
//#include <opencv2/highgui/highgui.hpp>
//#include "opencv2/opencv.hpp"
//#include "opencv2/core/core.hpp"
//#include "opencv/cv.h"
//using namespace std;
//using namespace cv;
//int main(int argc, char ** argv) {
//	const char *fileName = "pic5t.jpg";
//	if (argc > 1) {
//		fileName = argv[1];
//	}
//	//For your reference: Some image types of interest:
//	cout << "CV_8UC1 = " << CV_8UC1 << endl;
//	cout << "CV_8UC3 = " << CV_8UC3 << endl;
//	cout << "CV_32FC1 = " << CV_32FC1 << endl;
//	cout << "CV_32FC3 = " << CV_32FC3 << endl;
//	cout << "CV_64FC1 = " << CV_64FC1 << endl;
//	cout << "CV_64FC3 = " << CV_64FC3 << endl;
//	Mat originalImage = imread(fileName, 1);
//	// Check to make sure it's valid
//	if (originalImage.empty() || (originalImage.data == NULL)) {
//		cout << "Can't load image from " << fileName << "!" << endl;
//		exit(EXIT_FAILURE);
//	}
//	int n = 8;
//	int unitWidth = originalImage.cols / n; // you had image.rows / n;
//	int unitHeight = originalImage.rows / n;
//	Mat dctImage = originalImage.clone();
//	for (int i = 0; i < n; i++) { //i is row index
//		// inner loop added so that more than one row of tiles written
//		for (int j = 0; j < n; j++) { // j is col index
//			Mat subImage = dctImage(
//					Rect(j * unitWidth, i * unitHeight, unitWidth, unitHeight));
//			ostringstream oss;
//			oss << i << "_" << j << ".jpg";
//			string name = oss.str();
//			imwrite(name, subImage);
//		}
//	}
//	cvNamedWindow("ll",CV_WINDOW_AUTOSIZE);
//	Mat mine = imread("4_7.jpg");
//	Mat graay;
//	cvtColor(mine, graay, CV_BGR2GRAY);
//	imshow("ll",graay);
//	threshold(graay, graay, 131, 255, CV_THRESH_BINARY);
////	imshow("ll",graay);
//	int non = countNonZero(graay); // bt3d e white
//	IplImage gg=graay;
//	int white = (gg.width* gg.height) - non;
//	cout << white << endl;
//	cout<<non<<endl;
//	waitKey(0);
//	destroyAllWindows();
//	return 0;
//}


#include "detection.h"
#include "My_SVM.h"
#include "trainner.h"
int main() {
	detection t(true); // param test_mood
	//	My_SVM s;
	//	s.train_data();
	//	s.test_data();
	//	trainner tr;
	return 0;

}
