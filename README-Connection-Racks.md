# Connection FX racks

On the MIX page, click a cable from OSC A/B/C, Sub or Noise to Filter 1/2.
Choose **Add FX rack**, **Open FX rack**, or **Remove FX rack**.
Adding a rack opens its named tab on the FX page. Use + FX or the shared FX
Library to add effects or a saved chain. The MIX cable shows a compact chain summary.
Right-click its FX tab to remove a rack even if its original cable has been rerouted.
Removing the rack deletes its effects and modulation links while retaining the audio connection.

Each connection has its own shared stereo chain, before its destination filter.
It processes the summed notes on that connection; downstream filter modulation
follows the most recent active note on that connection. Paths without an active
insert retain the existing per-voice filtering. An empty or fully bypassed rack
passes through the original path. Effect tails continue after notes release.
Normal filter sends, serial filter routing, buses and output routing remain available.

Racks, effects, settings, impulse responses and modulation are stored in the synth
preset/DAW state. Init clears the connection racks. FX Library presets work in
connection racks as well as Main/Bus racks. All racks share the existing limit of
64 simultaneous effect slots; no host parameter IDs or ordering have changed.

## Patch sockets and cable colours

The MIX cards have a metal jack bay at the bottom: inputs on the left,
outputs on the right. Drag from an OUT jack to an IN jack; Shift-drag adds
a parallel filter or bus send. The connected plugs use straight bodies
with colour-coded strain reliefs. Generators only expose outputs; Main and
Direct expose their input sockets.

Click any cable and open **Cable colour**. Choose a palette colour, use
**Custom colour...** and Apply, or reset that wire to **Automatic**.
Colours belong to the specific source/destination/send connection and are
saved with the synth preset or DAW project. Colour changes do not restart
the effects or change the audio routing. Init resets custom colours.
