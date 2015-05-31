#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <algorithm>
#include <string>
#include "music.h"

using namespace cv;
using namespace std;

int main() {
    string templ = "templates/template.1";
    string vidfile = "data/floor-three-drums.avi";
    // open the video source
    VideoCapture cp;
    cp.open(vidfile);
    // initialize template, open the appropriate template
    InstrumentModel imodel(templ);
    // initialize the detector
    Detector detector(imodel, cp);
    detector.init();
    MusicParams mparams;
    while(detector.next(mparams)) {

    }
    return 0;
}
