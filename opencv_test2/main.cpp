/*
 * main.cpp
 *
 *  Created on: Feb 8, 2014
 *      Author: Mohamed
 */
#include "test1.h"
#include "test3.h"
#include "Sift_Impl.h"
int main (){
//
test1 t (false); //yccrb is better as it use ranges
////	test3 t;
//	Sift_Impl d;
	return 0;
}
//#include "opencv2\opencv.hpp"
//
//#include <stdio.h>
//#include <opencv2/legacy/legacy.hpp>
//#include <opencv2/objdetect/objdetect.hpp>
//#include <opencv2/nonfree/nonfree.hpp>
//#include <opencv2/nonfree/features2d.hpp>
//#include <algorithm>
//
//using namespace std;
//using namespace cv;
//
////
//int main(  )
//{
//	initModule_nonfree();
//        //source image
//    char* img1_file = "pic5t.jpg";
//    char* img2_file = "pic5t.jpg";
//
//    // image read
//    Mat tmp = cv::imread( img1_file, 1 );
//    Mat in  = cv::imread( img2_file, 1 );
//
//    /* threshold      = 0.04;
//       edge_threshold = 10.0;
//       magnification  = 3.0;    */
//
//    // SIFT feature detector and feature extractor
//    cv::SiftFeatureDetector detector( 0.05, 5.0 );
//    cv::SiftDescriptorExtractor extractor( 3.0 );
//
//    /* In case of SURF, you apply the below two lines
//    cv::SurfFeatureDetector detector();
//    cv::SurfDescriptorExtractor extractor();
//    */
//
//    // Feature detection
//    std::vector<KeyPoint> keypoints1, keypoints2;
//    detector.detect( tmp, keypoints1 );
//    detector.detect( in, keypoints2 );
//
//    // Feature display
//    Mat feat1,feat2;
//    drawKeypoints(tmp,keypoints1,feat1,Scalar(255, 255, 255),DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
//    drawKeypoints(in,keypoints2,feat2,Scalar(255, 255, 255),DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
//    imwrite( "feat1.bmp", feat1 );
//    imwrite( "feat2.bmp", feat2 );
//    int key1 = keypoints1.size();
//    int key2 = keypoints2.size();
//    printf("Keypoint1=%d \nKeypoint2=%d", key1, key2);
//
//    // Feature descriptor computation
//    Mat descriptor1,descriptor2;
//    extractor.compute( tmp, keypoints1, descriptor1 );
//    extractor.compute( in, keypoints2, descriptor2 );
//    /*int desc1 = descriptor1.size;
//    int desc2 = descriptor2.size;
//    printf("Descriptor1=%d \nDescriptor2=%d", desc1, desc2);*/
//
//    // corresponded points
//    std::vector<vector<DMatch> > matches;
//
//    // L2 distance based matching. Brute Force Matching
//    BFMatcher matcher;
//
//    // Flann-based matching
//    //FlannBasedMatcher matcher;
//
//    // display of corresponding points
//    matcher.knnMatch( descriptor1, descriptor2, matches,2);
//
//    // matching result
//    Mat result;
//    drawMatches( tmp, keypoints1, in, keypoints2, matches, result );
//
//    // output file
//    imwrite( "result.bmp", result );
//
//    // display the result
//    namedWindow("SIFT", CV_WINDOW_AUTOSIZE );
//    imshow("SIFT", result);
//    waitKey(0); //press any key to quit
//
//    return 0;
//}
