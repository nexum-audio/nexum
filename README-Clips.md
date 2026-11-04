# Nexum Clip Editor

Open CLIP by clicking its title above the small black launch keyboard. The editor is available independently of playback power. Its layout and playback controls follow the [official Nexum guide, pages 219–243](). The [EDMProd walkthrough]() provides additional visual examples.

## Editing

- Double-click the grid to add a note, or a note to remove it. Drag notes to move them; drag the right edge to resize. Lasso or Shift-click to select several notes; Alt-drag copies them.
- Ctrl+A/C/X/V selects, copies, cuts and pastes. Ctrl+D duplicates. Delete removes the selection. Drag a time range without including notes to copy or cut that range. Pasting a time range overwrites its destination and splits overlapping notes.
- Undo/Redo, Quantize, Legato, Chop, Conform to Scale, Mute, Reverse, time scaling and whole-clip duplication/scaling are available through Note Tools and the context menu. Keyboard shortcuts include Ctrl+Z/Y/Q/L/U/K/R/E and 0 for mute. Arrows move notes; Shift+Up/Down changes octaves and Shift+Left/Right changes length.
- FOLD reduces the visible pitches; FIT frames the notes. AUDITION sounds selected notes. The piano strip can also audition a pitch. Wheel scrolls vertically; Shift-wheel pans; Ctrl-wheel or the ruler wheel zooms time; Alt-wheel zooms vertically. Drag the overview after zooming to navigate.
- The colored flags set loop boundaries. The white marker sets the starting offset. Options includes offset quantization and Wrap Clip to Start Offset.

## Expression and automation

Choose Velocity, Chance, Octave Random, Expression X/Y/Z, MIDI CC, Pitch Bend, Pressure or a Macro lane. Controller… accepts any CC number from 0 to 127. Draw automation below the notes; right-click a point to remove it. Alt-drag a segment to change its curvature.

PITCH reveals per-note curves. Double-click a note to add points; drag the white handles. Alt-drag between handles curves the segment. Expressive chord notes use separate MIDI channels, including 24-semitone pitch-bend-range messages. Map MPE Expression X/Y/Z in the modulation matrix to hear expression affect the desired sound parameter.

## Playback and variations

Clicking a slot selects it for editing. Its separate PLAY/STOP button controls playback. The small CLIP keyboard also launches clips. Alt-drag copies clips and settings. Slot menus provide Rename, Copy/Paste, Erase, Save/Load and Set as Preview Clip.

The left panel provides Mono/Poly clip triggering, per-clip launch quantization, retrigger, velocity trigger, note gate, transpose, keyboard span, playback modes, synced/free rate and triplet/dotted timing. Options contains input trigger octave, key/scale and host transport. Queued launches keep the current clip playing until the boundary.

Edit All, or Alt during a parameter edit, applies settings across the twelve clips. The MACROS button exposes the eight shared macros with their drag handles.

LOCK MODULE retains clip data and clip settings when browsing sound presets. Oscillators and ARP still load the new preset. Loading a saved host project restores that project's own clip state.

## Recording and files

RECORD starts a performance. OVERDUB loops the active region; EXTEND grows the timeline. REC QUANT quantizes new recorded notes, leaving existing material unchanged. CLICK enables the metronome. COMMIT keeps the current take; UNDO TAKE discards material added since the last commit. Stopping recording commits it and leaves clip playback running.

MIDI files can be imported or dropped on the editor. EXPORT MIDI and DRAG MIDI include notes, MIDI automation and per-note expression/pitch curves. Internal macro automation stays in Nexum presets/clip banks. MIDI OUT can send the raw Clip Player or the routed Clip + Arp result to the host.

Individual clips use `.nxclip`; clip banks use `.nxclipbank`, with legacy `.ncseq` clip-bank import. Files are offered in `Documents/Nexum/Clips`. Loading these changes the clip area rather than the synth or ARP. Complete sound presets/projects include the clips as well.

Validation covers clip timing/loop boundaries, playback and launch modes, polyphonic/offset keyboard span, expression, MIDI files, isolated clip files, module lock/project recall, recording/quantization/metronome, editing/history/time-range paste, existing ARP/keyboard/modulation/automation behavior, and actual built VST3 loading.
