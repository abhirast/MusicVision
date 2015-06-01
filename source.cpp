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
    string templ = "templates/piano";
    string vidfile = "data2/19.avi";
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