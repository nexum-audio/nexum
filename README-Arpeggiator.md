# Nexum Arpeggiator

Open the editor by clicking ARP above the small black pattern keyboard. Power and editor visibility are separate. Supply a chord from MIDI or the piano to hear the arp.

The layout follows the pattern editor and controls in [EDMProd's guide](). Playback details were also checked against [Xfer's user guide](), pages 245–255.

- Pattern Settings: length in beats, playback mode, random jump time, step mode and independent wrap pitch/range.
- Double-click empty grid space to add an event; double-click an event to remove it. Drag notes to move them and their right edges to change length. Drag empty space to select several notes; Ctrl+A/C/X/V and Delete work in the grid. Alt-drag duplicates notes.
- Rows start at 0 and refer to the held chord, rather than fixed MIDI keys. When keys are held, note labels show the resolved pitches. Normal Wrap Range 0 repeats the held set; Range 1 and Pitch 12 add an octave to the next group.
- Drag the flags above the grid to set the active loop. Ctrl-wheel zooms time, Shift-wheel pans, and the wheel scrolls rows. Editing grid and playback speed are separate.
- Click an event's Accent strip to toggle emphasis. Drag its Strum strip to spread simultaneous attacks. The lane selector provides Velocity, Chance, Octave Random and Expression X/Y/Z.
- Enable Pitch Curves, double-click a note to insert a curve point, then drag the white handles. Simultaneous expressive notes receive independent MIDI channels and pitch bends. Expression X/Y/Z are available as modulation-matrix sources; they can drive cutoff, level and envelope decay. Pitch bend output uses the standard 24-semitone RPN range on expressive member channels.
- Playback modes include Normal, Reverse, Pendulum, Random, Random No Dup, Random Start/End, One Shot and Static. Step modes include Normal, New Only, Chord and Chord (new). New modes require incoming note-ons to coincide with the step.
- The lower sections provide Launch Quant, Edit All, shape, synced/free rate, pattern speed, triplet/dotted timing, transpose direction/shift/range, latch, thru, note offset, repeats, gate up to 200%, chance, retrigger mode/rate and velocity decay/target.
- Click a variation to request it. Launch Quant schedules its activation; queued slots are shown separately from the playing slot. Alt-drag copies a variation with its settings, including on the small black ARP keyboard. Edit All, or Alt while editing, propagates parameter edits to the twelve slots.
- The pattern-name menu contains Init, Rename, Save/Load Pattern and basic pattern starters. Pattern files use `.nxarp`; Save/Load Bank uses `.nxarpbank`. Both formats change arp data only. Complete synth presets/projects retain the arp together with the sound.

New pattern and bank files are offered in `Documents/Nexum/Arpeggiator`. All editing remains available after loading. Existing `.ncseq` clip banks remain available in the Clip editor.

Validation: dedicated MIDI timing/expression/editor tests; existing clip, keyboard, macro, modulation, automation, preset reset and FX-library regressions; actual built synth and Nexum FX VST3 loading and processing.
