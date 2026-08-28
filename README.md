# libfreeverb

A portable, sample-rate independent implementation of the Freeverb reverberator,
as a static library with no dependencies and no ties to any plugin format or
audio host.

The reverberator is Freeverb, written by Jezar at Dreampoint in June 2000 and
placed by him in the public domain: eight filtered-feedback comb filters in
parallel per channel, into four allpass filters in series. His sources are in
`src/freeverb/`, and `src/freeverb/PROVENANCE.md` records where they came from
and every line that has been changed since.

## What this adds

Freeverb as published assumes 44100 Hz. Its delay lines are fixed arrays sized
from constants the author chose by ear at that rate, so running it at any other
rate shortens the reverb in proportion. This library scales the delay lines with
the sample rate and allocates them accordingly, which is the whole of the
difference. At 44100 Hz the output is bit for bit what Freeverb produces;
elsewhere each delay takes the nearest whole sample to the scaled length.

Nothing else about the reverb is changed: same topology, same tunings, same
parameter ranges and scaling. There is no smoothing, no predelay and no
equalisation, none of which Freeverb has.

## Building

```sh
cmake -B build
cmake --build build
```

To use it from another CMake project:

```cmake
add_subdirectory(libfreeverb)
target_link_libraries(your_target PRIVATE freeverb::freeverb)
```

## Using it

```cpp
#include <freeverb/Freeverb.h>

freeverb::Freeverb reverb(48000.0f);
reverb.setRoomSize(0.7f);
reverb.setDamp(0.4f);
reverb.setWet(0.3f);
reverb.setDry(0.7f);

reverb.processReplace(inL, inR, outL, outR, numFrames);
```

Every parameter takes and returns the range Freeverb defines for it, which is 0
to 1 throughout. Parameters apply the moment they are set and nothing here ramps
them, so a host that changes one while audio is running should ramp it itself.

## Licence

zlib; see `LICENSE`. Freeverb itself is public domain, and the notices in
`src/freeverb/` say so where they stand.
