// libfreeverb - a portable, sample-rate independent Freeverb
//
// This file is released under the zlib licence; see LICENSE.

#include "freeverb/Freeverb.h"

#include "freeverb/revmodel.hpp"

namespace freeverb {

// revmodel predates const correctness and takes its inputs by mutable pointer,
// though it only ever reads through them.
static float *readable(const float *p) { return const_cast<float *>(p); }

Freeverb::Freeverb(float sampleRate)
    : m_model(std::make_unique<revmodel>(sampleRate)) {}

Freeverb::~Freeverb() = default;
Freeverb::Freeverb(Freeverb &&) noexcept = default;
Freeverb &Freeverb::operator=(Freeverb &&) noexcept = default;

void Freeverb::setSampleRate(float sampleRate) {
  m_model->setsamplerate(sampleRate);
}

float Freeverb::sampleRate() const { return m_model->getsamplerate(); }

void Freeverb::mute() { m_model->mute(); }

void Freeverb::processReplace(const float *inputL, const float *inputR,
                              float *outputL, float *outputR, long numSamples,
                              int skip) {
  m_model->processreplace(readable(inputL), readable(inputR), outputL, outputR,
                          numSamples, skip);
}

void Freeverb::processMix(const float *inputL, const float *inputR,
                          float *outputL, float *outputR, long numSamples,
                          int skip) {
  m_model->processmix(readable(inputL), readable(inputR), outputL, outputR,
                      numSamples, skip);
}

void Freeverb::setRoomSize(float value) { m_model->setroomsize(value); }

float Freeverb::roomSize() const { return m_model->getroomsize(); }

void Freeverb::setDamp(float value) { m_model->setdamp(value); }

float Freeverb::damp() const { return m_model->getdamp(); }

void Freeverb::setWet(float value) { m_model->setwet(value); }

float Freeverb::wet() const { return m_model->getwet(); }

void Freeverb::setDry(float value) { m_model->setdry(value); }

float Freeverb::dry() const { return m_model->getdry(); }

void Freeverb::setWidth(float value) { m_model->setwidth(value); }

float Freeverb::width() const { return m_model->getwidth(); }

void Freeverb::setMode(float value) { m_model->setmode(value); }

float Freeverb::mode() const { return m_model->getmode(); }

} // namespace freeverb
