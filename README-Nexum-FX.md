# Nexum FX

Nexum FX is the separate VST3 audio-effect version of the Nexum FX rack. Insert it on an FL Studio mixer track after a plugin scan. It accepts mono or stereo audio and has one Main rack, with the same effect algorithms, rack UI, presets and parameter automation as Nexum.

Installed VST3: `%CommonProgramFiles%/VST3/NEXUM/Nexum FX.vst3`.
Standalone preview: `%USERPROFILE%/Desktop/VST3 Plugins/Nexum FX/Nexum FX.exe`.
The standalone initially mutes its audio input, as provided by the JUCE audio settings. Use FL Studio to process mixer-track audio.

## Shared effects and library

Both products build the canonical `NEXUM_SHARED_SOURCES` source list. `Source/FxRack.cpp`, `FxPage.h`, `FxGraph.h`, `FxLibrary.h` and `AssetBrowser.h` are shared; do not create a second FX implementation. Adding an effect to the shared registry makes it available in both builds. The default build and Update.ps1 update both plugins. Their VST3 IDs are distinct, so existing Nexum projects retain their plugin identity.

The shared library is `%USERPROFILE%/Desktop/VST3 Plugins/Nexum/FX Library`. Both plugins read and write the same `.ncfx` documents and metadata. Entries added, renamed or removed in either are reflected in the other; open libraries refresh automatically without discarding unsaved metadata drafts. New effect types receive new default entries while deleted older defaults remain deleted.

Single presets append an effect to Main. Chain presets replace Main. Parameter values, bypass, custom curves, compressor settings and impulse responses are portable between the plugins. Nexum FX has no Bus 1/Bus 2 or synth controls. FL Studio stores its rack state inside the project independently of library files.

Each FX bypass indicator is grey while active and lights in that FX's colour when bypassed. The top BYPASS bypasses the complete rack; OUTPUT adjusts the final level. An empty rack at 0 dB passes input audio unchanged. Host tempo is used for synced effects. Right-click effect knobs for host automation.

Trancegate: add it with + FX > Trancegate. Draw levels directly across its 16 steps. RATE selects quarter, eighth, sixteenth, thirty-second or triplet steps; STEPS changes the pattern length (1–16). DEPTH controls the gating amount, GATE the open portion of each step, ATTACK/RELEASE soften transitions, and STEREO offsets the right channel by steps. The pattern follows host tempo and playback position; standalone playback runs at 120 BPM. Step patterns and controls are stored in projects and the shared FX Library.

## suit — gemeinsames NEXUM-Design
Die Oberflaechen sind mit Nexum Tune ueber denselben NexumDesign-SDK verbunden. suit unter `the installed standalone directory` bearbeitet Farben, Schriftgroessen, Rundungen und Stil-Vererbung. Veroeffentlichte Stile werden aus `%APPDATA%/NEXUM/suit/design.json` im UI-Thread live uebernommen. Die FX-Hardware-Schienen und Audiofunktionen bleiben erhalten.


## Linke Seitenleiste
Nexum FX verwendet dieselbe 92px-Seitenleiste wie Nexum: FX und Library als Reiter, darunter vier Makros mit Plus zum Erweitern, scrollbare Liste und Version unten. Makros steuern FX-Regler sowie Output, sind benennbar und werden im Pluginzustand gespeichert. Die ersten acht sind weiterhin DAW-Parameter. Library oeffnet die gemeinsame FX-Library.
