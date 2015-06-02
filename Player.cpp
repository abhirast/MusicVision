#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "music.h"

using namespace std;

Player::Player(InstrumentType itype) {
	this->itype = itype;
}

bool Player::play(MusicParams &params){
    if (params.intensity <= 0 || params.note <= 0) return false;
    float volume = params.intensity/10.0;
    char cmd[100];
    sprintf(cmd, "play -q -v %f tones/%d.wav", volume, params.note);
    popen(cmd, "r");
    cout << "played";
    return true;
}

void close() {
}

