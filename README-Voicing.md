# Nexum: VELO / NOTE und Voicing

Rechts unten stehen zwei editierbare Modulationskurven. **VELO** verarbeitet die
Anschlagstärke, **NOTE** die gespielte MIDI-Note (0–127). Die Kurven ändern die
Modulationswerte; sie transponieren keine Noten und ändern ohne Zuweisung nicht
automatisch die Lautstärke. Den Ring rechts am Tab auf einen Regler ziehen, um
eine Zuweisung anzulegen. Die Modulationsbereiche lassen sich wie bei ENV/LFO
bearbeiten. Jede Stimme verwendet ihre eigenen Noten-/Velocity-Werte.

In die Kurve zeichnen; Doppelklick setzt sie auf linear zurück. Rechtsklick bietet
Linear, Inverse und Constant 50%. VELO und NOTE haben unabhängige Kurven und zeigen
den letzten gespielten Eingang als weißen Marker. Kurven und Zuweisungen werden
mit dem Preset/DAW-Zustand gespeichert. Alte Zustände erhalten lineare Kurven.

**MONO** schaltet auf monophones Spiel mit Last-Note-Priority. **LEGATO** erhält
bei überlappenden Noten die laufende Stimme, Oszillatorphase und Hüllkurven. Ohne
Legato werden die Hüllkurven neu gestartet. Beim Loslassen der neuesten Note wird
zur zuletzt noch gehaltenen Note zurückgekehrt. Die letzte Note löst Release aus.
**POLY** begrenzt die polyphonen Stimmen; daneben steht aktiv / Limit.

**ALWAYS**, **SCALED**, **PORTA** und **CURVE** stehen unten rechts direkt neben der
Klaviatur. **PORTA** ist die Gleitzeit in Millisekunden. Ohne **ALWAYS** gleiten nur überlappende
Noten; mit Always auch getrennte Noten ab der zuletzt gespielten Tonhöhe. **SCALED**
skaliert die Dauer mit dem Tonabstand: der eingestellte Wert gilt für eine Oktave,
ein Halbton benötigt ein Zwölftel davon. Das **CURVE**-Feld verändert den Verlauf:
vertikal ziehen, Doppelklick für einen linearen Verlauf in Halbtönen. Positive
Krümmung verzögert den Anstieg, negative Krümmung beschleunigt ihn am Anfang.

Die früheren Synth-FX-Regler bleiben im Menü **… > Synth FX controls…** verfügbar.
Dort liegen auch Arpeggiator-Schalter und Import-Informationen. Der Hauptpegel
steht oben im Master-Bereich, Arp/Clip weiterhin am Mini-Keyboard. Bestehende
Parameter-IDs und Indizes bleiben erhalten; neue Parameter stehen am Listenende.

Die Struktur orientiert sich an der [Nexum-Anleitung]().
Die Kurven, Stimmenverwaltung und Gleitberechnung sind eigene Nexum-Implementierungen.
