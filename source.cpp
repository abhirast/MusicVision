#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include <opencv2/nonfree/features2d.hpp> //Thanks to Alessandro
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "music.h"


using namespace cv;
using namespace std;

void generateRandom(vector<int> &vec, int high) {
    vec.clear();
    while (vec.size() != 4) {
        bool found = false;
        int r = rand() % high;
        for (int i = 0; i < vec.size(); i++) {
            if (r == vec[i]) found = true;
        }
        if (!found) vec.push_back(r);
    }
}

int main() {
    srand (time(NULL));
    string templ = "templates/piano.jpg";
    string vidfile = "data/single-piano-1.avi";
    // VideoCapture cp;
    // cp.open(vidfile);
    Mat image1, image2;
    // cp.read(image1);
    image1 = imread("test_image.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    image2 = imread(templ, CV_LOAD_IMAGE_GRAYSCALE);
    imwrite("test_image.jpg", image1);
    int minHessian = 60;
    SiftFeatureDetector detector;

    vector<KeyPoint> keypoints_1, keypoints_2;

    detector.detect( image1, keypoints_1 );
    detector.detect( image2, keypoints_2 );
    
    // Get descriptors
    Mat descriptors_1, descriptors_2;
    SiftDescriptorExtractor extractor;
    extractor.compute( image1, keypoints_1, descriptors_1 );
    extractor.compute( image2, keypoints_2, descriptors_2 );

    BFMatcher matcher(NORM_L2);
    vector<DMatch> matches;

    matcher.match(descriptors_1, descriptors_2, matches);

    // get matced keypoints
    vector<Point2f> pts1, pts2;
    for (int i = 0; i < matches.size(); i++) {
        pts1.push_back(keypoints_1[matches[i].queryIdx].pt);
        pts2.push_back(keypoints_2[matches[i].queryIdx].pt);
    }
    
    // try out various homographies
    int numIters = 10000;
    vector<int> randpos;
    int radius = 6, maxscore = 0;
    Mat bestH;
    cout << "size is " << pts1.size() << endl;
    for (int i = 0; i < numIters; i++) {
        generateRandom(randpos, pts1.size());
        vector<Point2f> projl, projr;
        for (int i = 0; i < randpos.size(); i++) {
            projl.push_back(pts1[randpos[i]]);
            projr.push_back(pts2[randpos[i]]);
        }
        // find the homography
        Mat H = findHomography( projl, projr, CV_RANSAC );
        // score the homography
        vector<Point2f> predicted;
        perspectiveTransform(pts1, predicted, H);
        int score = 0;
        for (int i = 0; i < predicted.size(); i++) {
            if (norm(pts2[i] - predicted[i]) < radius) score++;
        }
        if (score > maxscore) {
            maxscore = score;
            bestH = H;
            cout << maxscore << endl;
        }
    }
    vector<Point2f> src;
    vector<Point2f> dst;
    src.push_back(keypoints_2[0].pt);
    perspectiveTransform(src, dst, bestH);
    Point x((int) src[0].x, (int) src[0].y);
    rectangle(image1, x - Point(5, 5), x + Point(5, 5), 
                    Scalar( 0, 255, 255 ), -1, 8 );
    Point y((int) dst[0].x, (int) dst[0].y);
    rectangle(image2, y - Point(5, 5), y + Point(5, 5), 
                    Scalar( 0, 255, 255 ), -1, 8 );
    imshow("fig1", image1);
    imshow("fig2", image2);
    waitKey(0);
    return 0;
}


int main2() {
    string templ = "templates/template.1";
    string vidfile = "data/piano2.avi";
    InstrumentType itype = XYLO;
    // open the video source
    VideoCapture cp;
    InstrumentModel imodel(templ);
    Detector detector(imodel, cp);
    Player player(itype);
    cp.open(vidfile);
    detector.init();
    Mat image;
    MusicParams mparams;
    while (detector.next(mparams)) {
        player.play(mparams);
        if (waitKey(100) > 0) break;
    }
}

int main3() {
    string templ = "templates/piano.jpg";
    string vidfile = "data/single-piano-1.avi";
    // VideoCapture cp;
    // cp.open(vidfile);
    Mat image1, image2;
    // cp.read(image1);
    image1 = imread("test_image.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    image2 = imread(templ, CV_LOAD_IMAGE_GRAYSCALE);
    imwrite("test_image.jpg", image1);
    int minHessian = 60;
    SiftFeatureDetector detector;

    vector<KeyPoint> keypoints_1, keypoints_2;

    detector.detect( image1, keypoints_1 );
    detector.detect( image2, keypoints_2 );
    
    // Mat output1, output2;
    // drawKeypoints(image1, keypoints_1, output1);
    // namedWindow("temp", 0);
    // imshow("temp", output1);
    // drawKeypoints(image2, keypoints_2, output2);
    // namedWindow("temp2", 0);
    // imshow("temp2", output2);
    // waitKey(0);

    // Get descriptors
    Mat descriptors_1, descriptors_2;
    SiftDescriptorExtractor extractor;
    extractor.compute( image1, keypoints_1, descriptors_1 );
    extractor.compute( image2, keypoints_2, descriptors_2 );

    BFMatcher matcher(NORM_L2);
    vector<DMatch> matches;

    matcher.match(descriptors_1, descriptors_2, matches);

    // drawing the results
    namedWindow("matches", 1);
    Mat img_matches;
    drawMatches(image1, keypoints_1, image2, keypoints_2, matches, img_matches);
    imshow("matches", img_matches);
    waitKey(0);
    // FlannBasedMatcher matcher;
    // vector< DMatch > matches;
    // matcher.match( descriptors_1, descriptors_2, matches );
    // cout << descriptors_1 <<endl;
    // vector<Point2f> obj, scene;
    // for( int i = 0; i < matches.size(); i++ ) {
    //     obj.push_back( keypoints_1[matches[i].queryIdx].pt );
    //     scene.push_back( keypoints_2[matches[i].trainIdx].pt);
    // }
    // Mat img_matches;
    // drawMatches( image1, keypoints_1, image2, keypoints_2,
    //                matches, img_matches, Scalar::all(-1), Scalar::all(-1),
    //                vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
    // imshow("matches",img_matches );
    // waitKey(0);
    // Mat H = findHomography( obj, scene, CV_LMEDS );
    // Mat img_matches;
    // vector< DMatch > good_matches;
    // vector<Point2f> oldpoints;
    // for (int i = 0; i < keypoints_1.size(); i++) {
    //     oldpoints.push_back(keypoints_1[i].pt);
    //     DMatch d(i, i, 0.0);
    //     good_matches.push_back(d);
    // }
    // vector<Point2f> newpoints;
    // perspectiveTransform(oldpoints, newpoints, H);
    // for (int i = 0; i < newpoints.size(); i++) {
    //     keypoints_2[i].pt = newpoints[i];
    // }
    // drawMatches( image1, keypoints_1, image2, keypoints_2,
    //                good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
    //                vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
    // imshow("matches",img_matches );
    // waitKey(0);


//     //-- Step 2: Calculate descriptors (feature vectors)
//     SiftDescriptorExtractor extractor;

//       Mat descriptors_1, descriptors_2;

//       extractor.compute( image1, keypoints_1, descriptors_1 );
//       extractor.compute( image2, keypoints_2, descriptors_2 );

//       //-- Step 3: Matching descriptor vectors using FLANN matcher
//       FlannBasedMatcher matcher;
//       std::vector< DMatch > matches;
//       matcher.match( descriptors_1, descriptors_2, matches );

//       double max_dist = 0; double min_dist = 10;

//       //-- Quick calculation of max and min distances between keypoints
//       for( int i = 0; i < descriptors_1.rows; i++ )
//       { double dist = matches[i].distance;
//         if( dist < min_dist ) min_dist = dist;
//         if( dist > max_dist ) max_dist = dist;
//       }

//       printf("-- Max dist : %f \n", max_dist );
//       printf("-- Min dist : %f \n", min_dist );

//       //-- Draw only "good" matches (i.e. whose distance is less than 2*min_dist,
//       //-- or a small arbitary value ( 0.02 ) in the event that min_dist is very
//       //-- small)
//       //-- PS.- radiusMatch can also be used here.
//       std::vector< DMatch > good_matches;

//       for( int i = 0; i < descriptors_1.rows; i++ )
//       { if( matches[i].distance <= max(2*min_dist, 0.02) )
//         { good_matches.push_back( matches[i]); }
//       }

//       //-- Draw only "good" matches
      

//       vector<Point2f> obj, scene;
//       //-- Show detected matches
//       for( int i = 0; i < good_matches.size(); i++ )
// {
//     //-- Get the keypoints from the good matches
//     obj.push_back( keypoints_1[ good_matches[i].queryIdx ].pt );
//     scene.push_back( keypoints_2[ good_matches[i].trainIdx ].pt );
// }

// Mat H = findHomography( obj, scene, CV_RANSAC ); 
//     cout << H << endl;
    
//     Mat img_matches;
//     vector<Point2f> oldpoints;
//     for (int i = 0; i < keypoints_1.size(); i++) {
//         oldpoints.push_back(keypoints_1[i].pt);
//     }
//     vector<Point2f> newpoints;
//     perspectiveTransform(oldpoints, newpoints, H);
//     for (int i = 0; i < newpoints.size(); i++) {
//         keypoints_2[i].pt = newpoints[i];
//     }
//       drawMatches( image1, keypoints_1, image2, keypoints_2,
//                    good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
//                    vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
//     imshow("matches",img_matches );
//     waitKey(0);
    return 0;
}