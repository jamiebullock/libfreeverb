// Reverb model declaration
//
// Written by Jezar at Dreampoint, June 2000
// http://www.dreampoint.co.uk
// This code is public domain

#ifndef _revmodel_
#define _revmodel_

#include <vector>

#include "comb.hpp"
#include "allpass.hpp"
#include "tuning.h"

class revmodel
{
public:
					revmodel(float samplerate = 44100.0f);
			void	setsamplerate(float samplerate);
			float	getsamplerate();
			void	mute();
			void	processmix(float *inputL, float *inputR, float *outputL, float *outputR, long numsamples, int skip);
			void	processreplace(float *inputL, float *inputR, float *outputL, float *outputR, long numsamples, int skip);
			void	setroomsize(float value);
			float	getroomsize();
			void	setdamp(float value);
			float	getdamp();
			void	setwet(float value);
			float	getwet();
			void	setdry(float value);
			float	getdry();
			void	setwidth(float value);
			float	getwidth();
			void	setmode(float value);
			float	getmode();
private:
			void	update();
			void	setbuffers();
private:
	float	samplerate;
	float	gain;
	float	roomsize,roomsize1;
	float	damp,damp1;
	float	wet,wet1,wet2;
	float	dry;
	float	width;
	float	mode;

	// Comb filters
	comb	combL[numcombs];
	comb	combR[numcombs];

	// Allpass filters
	allpass	allpassL[numallpasses];
	allpass	allpassR[numallpasses];

	// Buffers for the combs. The tunings are quoted for 44100Hz, so the
	// lengths follow the sample rate and cannot be fixed at compile time
	// the way Jezar was able to declare them.
	std::vector<float>	bufcombL[numcombs];
	std::vector<float>	bufcombR[numcombs];

	// Buffers for the allpasses
	std::vector<float>	bufallpassL[numallpasses];
	std::vector<float>	bufallpassR[numallpasses];
};

#endif//_revmodel_

//ends
