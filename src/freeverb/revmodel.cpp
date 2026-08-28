// Reverb model implementation
//
// Written by Jezar at Dreampoint, June 2000
// http://www.dreampoint.co.uk
// This code is public domain

#include "revmodel.hpp"

// Jezar quoted one constant per delay line. They are gathered here so the
// lengths can be scaled by a loop; the values are his, unchanged.
static const int combtuningL[numcombs] =
	{ combtuningL1, combtuningL2, combtuningL3, combtuningL4,
	  combtuningL5, combtuningL6, combtuningL7, combtuningL8 };
static const int combtuningR[numcombs] =
	{ combtuningR1, combtuningR2, combtuningR3, combtuningR4,
	  combtuningR5, combtuningR6, combtuningR7, combtuningR8 };
static const int allpasstuningL[numallpasses] =
	{ allpasstuningL1, allpasstuningL2, allpasstuningL3, allpasstuningL4 };
static const int allpasstuningR[numallpasses] =
	{ allpasstuningR1, allpasstuningR2, allpasstuningR3, allpasstuningR4 };

// The tunings are quoted for 44100Hz. Dividing by that rate before scaling
// leaves them untouched there, so the reverb is unchanged at the rate Jezar
// tuned it by ear. Elsewhere the nearest whole sample is taken rather than the
// one below, which is both the closer delay and what Pd's delread~ arrives at
// for the same tuning.
static int scaletuning(int tuning, float samplerate)
{
	int scaled = (int)((double)tuning * (double)samplerate / 44100.0 + 0.5);
	return scaled < 1 ? 1 : scaled;
}

revmodel::revmodel(float samplerate)
{
	this->samplerate = samplerate > 0 ? samplerate : 44100.0f;
	setbuffers();

	// Set default values
	allpassL[0].setfeedback(0.5f);
	allpassR[0].setfeedback(0.5f);
	allpassL[1].setfeedback(0.5f);
	allpassR[1].setfeedback(0.5f);
	allpassL[2].setfeedback(0.5f);
	allpassR[2].setfeedback(0.5f);
	allpassL[3].setfeedback(0.5f);
	allpassR[3].setfeedback(0.5f);
	setwet(initialwet);
	setroomsize(initialroom);
	setdry(initialdry);
	setdamp(initialdamp);
	setwidth(initialwidth);
	setmode(initialmode);

	// Buffer will be full of rubbish - so we MUST mute them
	mute();
}

void revmodel::setbuffers()
{
	for (int i = 0; i < numcombs; i++)
	{
		bufcombL[i].assign(scaletuning(combtuningL[i], samplerate), 0.0f);
		bufcombR[i].assign(scaletuning(combtuningR[i], samplerate), 0.0f);
		combL[i].setbuffer(&bufcombL[i][0], (int)bufcombL[i].size());
		combR[i].setbuffer(&bufcombR[i][0], (int)bufcombR[i].size());
	}
	for (int i = 0; i < numallpasses; i++)
	{
		bufallpassL[i].assign(scaletuning(allpasstuningL[i], samplerate), 0.0f);
		bufallpassR[i].assign(scaletuning(allpasstuningR[i], samplerate), 0.0f);
		allpassL[i].setbuffer(&bufallpassL[i][0], (int)bufallpassL[i].size());
		allpassR[i].setbuffer(&bufallpassR[i][0], (int)bufallpassR[i].size());
	}
}

void revmodel::setsamplerate(float samplerate)
{
	if (samplerate <= 0 || samplerate == this->samplerate)
		return;

	this->samplerate = samplerate;
	setbuffers();

	// The delay lines are new, so the coefficients held by the old ones have
	// to be put back from the parameters, which the sample rate does not
	// change.
	update();
}

float revmodel::getsamplerate()
{
	return samplerate;
}

void revmodel::mute()
{
	if (getmode() >= freezemode)
		return;

	for (int i=0;i<numcombs;i++)
	{
		combL[i].mute();
		combR[i].mute();
	}
	for (int i=0;i<numallpasses;i++)
	{
		allpassL[i].mute();
		allpassR[i].mute();
	}
}

void revmodel::processreplace(float *inputL, float *inputR, float *outputL, float *outputR, long numsamples, int skip)
{
	float outL,outR,input;

	while(numsamples-- > 0)
	{
		outL = outR = 0;
		input = (*inputL + *inputR) * gain;

		// Accumulate comb filters in parallel
		for(int i=0; i<numcombs; i++)
		{
			outL += combL[i].process(input);
			outR += combR[i].process(input);
		}

		// Feed through allpasses in series
		for(int i=0; i<numallpasses; i++)
		{
			outL = allpassL[i].process(outL);
			outR = allpassR[i].process(outR);
		}

		// Calculate output REPLACING anything already there
		*outputL = outL*wet1 + outR*wet2 + *inputL*dry;
		*outputR = outR*wet1 + outL*wet2 + *inputR*dry;

		// Increment sample pointers, allowing for interleave (if any)
		inputL += skip;
		inputR += skip;
		outputL += skip;
		outputR += skip;
	}
}

void revmodel::processmix(float *inputL, float *inputR, float *outputL, float *outputR, long numsamples, int skip)
{
	float outL,outR,input;

	while(numsamples-- > 0)
	{
		outL = outR = 0;
		input = (*inputL + *inputR) * gain;

		// Accumulate comb filters in parallel
		for(int i=0; i<numcombs; i++)
		{
			outL += combL[i].process(input);
			outR += combR[i].process(input);
		}

		// Feed through allpasses in series
		for(int i=0; i<numallpasses; i++)
		{
			outL = allpassL[i].process(outL);
			outR = allpassR[i].process(outR);
		}

		// Calculate output MIXING with anything already there
		*outputL += outL*wet1 + outR*wet2 + *inputL*dry;
		*outputR += outR*wet1 + outL*wet2 + *inputR*dry;

		// Increment sample pointers, allowing for interleave (if any)
		inputL += skip;
		inputR += skip;
		outputL += skip;
		outputR += skip;
	}
}

void revmodel::update()
{
// Recalculate internal values after parameter change

	int i;

	wet1 = wet*(width/2 + 0.5f);
	wet2 = wet*((1-width)/2);

	if (mode >= freezemode)
	{
		roomsize1 = 1;
		damp1 = 0;
		gain = muted;
	}
	else
	{
		roomsize1 = roomsize;
		damp1 = damp;
		gain = fixedgain;
	}

	for(i=0; i<numcombs; i++)
	{
		combL[i].setfeedback(roomsize1);
		combR[i].setfeedback(roomsize1);
	}

	for(i=0; i<numcombs; i++)
	{
		combL[i].setdamp(damp1);
		combR[i].setdamp(damp1);
	}
}

// The following get/set functions are not inlined, because
// speed is never an issue when calling them, and also
// because as you develop the reverb model, you may
// wish to take dynamic action when they are called.

void revmodel::setroomsize(float value)
{
	roomsize = (value*scaleroom) + offsetroom;
	update();
}

float revmodel::getroomsize()
{
	return (roomsize-offsetroom)/scaleroom;
}

void revmodel::setdamp(float value)
{
	damp = value*scaledamp;
	update();
}

float revmodel::getdamp()
{
	return damp/scaledamp;
}

void revmodel::setwet(float value)
{
	wet = value*scalewet;
	update();
}

float revmodel::getwet()
{
	return wet/scalewet;
}

void revmodel::setdry(float value)
{
	dry = value*scaledry;
}

float revmodel::getdry()
{
	return dry/scaledry;
}

void revmodel::setwidth(float value)
{
	width = value;
	update();
}

float revmodel::getwidth()
{
	return width;
}

void revmodel::setmode(float value)
{
	mode = value;
	update();
}

float revmodel::getmode()
{
	if (mode >= freezemode)
		return 1;
	else
		return 0;
}

//ends
