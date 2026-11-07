# Nexum

Open-source Windows x64 hybrid synthesizer, VST3 and standalone.

Three main oscillators offer wavetable, sample, multisample, granular and spectral
modes, with Sub/Noise, two filters, envelopes, LFOs, macros, arpeggiator, clips and
a modular FX rack. User-imported sound packs are not included.

## Build
Visual Studio 2022 (Desktop development with C++), CMake 3.22 or newer.
JUCE 8 sources are included with their original notices.

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release --target NexumSynth_VST3 NexumSynth_Standalone --parallel 1
```

## Shared code and companion products
This repository is the canonical source of the synthesis/FX engine, JUCE and
the shared NexumDesign UI. Nexum FX pins it as a submodule rather than copying
effects. Tune and Gate use the shared UI from the same pinned source.

- https://github.com/nexum-audio/nexum-fx
- https://github.com/nexum-audio/nexum-tune
- https://github.com/nexum-audio/nexum-gate


## License
Original implementation: AGPL-3.0-only. See LICENSE.txt and
THIRD-PARTY-NOTICES.txt. Third-party components retain their own licenses.

## November 2026 development update
- Three horizontal oscillator cards, five source modes, two independent filters.
- Coloured patch cables and per-generator-to-filter connection FX racks.
- Focused live modulation/macro Matrix, growable preset-owned macros.
- 17 shared rack effects and a shared single-effect/chain library with Nexum FX.
- Editable preset, wavetable and SFZ libraries, arpeggiator and clip editor.
- Fresh standalone sessions start at Init; normal DAW state recall is retained.

External preset conversion remains experimental. Missing source assets and
unsupported modulation, filter or effect settings can change the sound. Third-party
preset packs, commercial assets and private comparison recordings are not included.

Nexum Rec is available at https://github.com/nexum-audio/nexum-rec.
Build checks: use SynthCheck --matrix, --connectionracks, --synthgraphs and
NexumFxCheck. The Windows development builds were checked before publication;
this source checkout has not been fully rebuilt from scratch on every platform.
