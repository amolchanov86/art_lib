#include "art_signals.h"

int ArtRTSignals:: SIGRTMIN_CORR = SIGRTMIN + ArtRTSignals:: ALREADY_OCCUP;
vector<bool> * ArtRTSignals:: occupied_signals = new vector<bool>(SIGRTMAX, false);

