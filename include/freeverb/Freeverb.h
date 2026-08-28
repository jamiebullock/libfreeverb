// libfreeverb - a portable, sample-rate independent Freeverb
//
// The reverberator is Freeverb, written by Jezar at Dreampoint in June 2000
// and placed by him in the public domain. See src/freeverb/PROVENANCE.md.
//
// This file is released under the zlib licence; see LICENSE.

#pragma once

#include <memory>

class revmodel;

namespace freeverb {

/// Freeverb: eight filtered-feedback comb filters in parallel per channel,
/// into four allpass filters in series.
///
/// Every parameter is taken and returned in the range Freeverb defines for it,
/// which is 0 to 1 throughout. Values outside that range are not rejected; the
/// original does not reject them either, and clamping would change what the
/// reverb does at the edges.
///
/// Parameters are applied the moment they are set. Nothing here ramps them, so
/// a host that changes one while audio is running should ramp it itself.
class Freeverb {
public:
  /// Constructs a reverberator running at the given sample rate.
  explicit Freeverb(float sampleRate = 44100.0f);
  ~Freeverb();

  Freeverb(const Freeverb &) = delete;
  Freeverb &operator=(const Freeverb &) = delete;
  Freeverb(Freeverb &&) noexcept;
  Freeverb &operator=(Freeverb &&) noexcept;

  /// Resizes the delay lines for a new sample rate, and clears them.
  ///
  /// The tunings Freeverb is built on were chosen by ear at 44100 Hz and are
  /// scaled from there, so the reverb is bit for bit the original at that
  /// rate. Setting the rate it already has does nothing.
  void setSampleRate(float sampleRate);
  float sampleRate() const;

  /// Clears the delay lines. Does nothing while frozen, so that freezing and
  /// clearing cannot fight each other.
  void mute();

  /// Writes the reverberated signal over the output buffers.
  ///
  /// The two input pointers may address the same buffer for a mono source.
  /// Each output pointer may alias the input pointer of the same channel,
  /// for processing in place; it may not alias the other channel's input,
  /// which is read after the first channel has been written. `skip` is the
  /// stride between consecutive frames, so interleaved buffers pass the
  /// channel count and planar buffers pass 1.
  void processReplace(const float *inputL, const float *inputR, float *outputL,
                      float *outputR, long numSamples, int skip = 1);

  /// As processReplace, but adds to what the output buffers already hold.
  void processMix(const float *inputL, const float *inputR, float *outputL,
                  float *outputR, long numSamples, int skip = 1);

  /// Length of the reverb tail, 0 to 1.
  void setRoomSize(float value);
  float roomSize() const;

  /// How quickly high frequencies decay, 0 to 1.
  void setDamp(float value);
  float damp() const;

  /// Level of the reverberated signal, 0 to 1.
  void setWet(float value);
  float wet() const;

  /// Level of the unprocessed signal, 0 to 1.
  void setDry(float value);
  float dry() const;

  /// Stereo spread of the reverberated signal, 0 for mono to 1 for wide.
  void setWidth(float value);
  float width() const;

  /// Freeze, holding the reverb tail indefinitely and admitting no new
  /// input. 0.5 and above freezes, which is Freeverb's own threshold.
  void setMode(float value);

  /// 1 while frozen and 0 otherwise, rather than the value last set.
  float mode() const;

private:
  std::unique_ptr<revmodel> m_model;
};

} // namespace freeverb
