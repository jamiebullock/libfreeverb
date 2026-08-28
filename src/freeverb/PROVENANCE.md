# Provenance

The files in this directory are Freeverb, written by Jezar at Dreampoint in June
2000. Each one carries the author's own grant in its header:

    This code is public domain

The upstream repository has no LICENSE file, so those per-file lines and the
author's readme are the whole of the grant. They must not be removed from these
files, and any code derived from them credits Jezar at Dreampoint.

## Where these came from

    https://github.com/sinshu/freeverb
    commit cfcea55553fb59ac57ebf2a237f72cad4296f2b0
    path Components/
    taken 2026-08-28

That copy was checked against an independent one before it was used. Its
constants are identical to the copy in muse-sequencer, and its `freezemode` of
0.5 and `initialwet` of `1/scalewet` are confirmed by two further unrelated
derivatives. Not every copy in circulation is faithful: the one in BespokeSynth
carries `initialwet 1.0f` and `freezemode 1.0f`.

## What was taken, and what has changed since

Every file below was committed byte for byte as upstream holds it. The git
object identifiers are upstream's own, so `git hash-object` on any of these
files reproduces them, and any later edit is visible in this repository's
history.

| File | Upstream object | Modified here |
| --- | --- | --- |
| `allpass.hpp` | `4fc294d1392e440ae31a828586fed7cde56bee09` | no |
| `comb.hpp` | `4a73b615fbbdf1b40afd2d006c8fd1b6fc1a7273` | no |
| `denormals.h` | `f8714127144a394d8cad46de3d9c5afe4456c4b9` | no |
| `tuning.h` | `baaa9ce0042626289ad2e324974944dbd0adc2cf` | no |
| `allpass.cpp` | `5d80eda2bd442c137a0e6949e4ae7dc2f823e769` | yes, one line |
| `comb.cpp` | `c05f5069c84f0cc57ca757ec84ed1feafd332ea3` | yes, one line |
| `revmodel.hpp` | `10fe7c67d5e8c14e632e29e3f40044e63a8ba0a0` | yes |
| `revmodel.cpp` | `a69e2f5b9924850c4e7cfb42330f928fe7446f14` | yes |

This is the whole of upstream's `Components/`, which is what the library is
built from.

`tuning.h` holds the constants the author arrived at by listening, which are not
this library's to change, and the two filter headers carry only the processing
loops, which are also unchanged. `revmodel` is where the delay lines are
declared as fixed arrays sized for 44100 Hz, so it is where following the sample
rate is done.

`comb::setbuffer` and `allpass::setbuffer` take a new buffer without resetting
the read index into it. Upstream calls each of them once, at construction, so
the index is always zero and this never shows. Following the sample rate means
calling them again on a buffer that may be shorter than the last, where a stale
index reads off the end, so each gains one line setting it back to zero. Neither
change touches the processing loops or the sound.

## Formatting

This directory sets `DisableFormat`, so the repository's clang-format rules stop
at its boundary. Reformatting these files would destroy the comparison the table
above rests on.
