# ENV-Werte und Tempo-Synchronisation

ENV 1–4 haben eine gemeinsame dunkle Wertleiste über den Reglern. Werte können
direkt eingegeben oder mit den Reglern geändert werden. Rechts schalten **BPM**
und **MS** nur die gerade ausgewählte Hüllkurve um.

MS verwendet die bisherigen Zeitparameter. BPM verwendet unabhängige gespeicherte
Notenwerte für Attack, Hold, Decay und Release; Sustain bleibt ein Prozentwert.
Es gibt gerade, punktierte (D) und triolische (T) Werte bis zu vier Takten.
Beim manuellen Umschalten werden die aktuellen Zeiten in passende Werte des
anderen Modus übertragen; die bestehenden MS-Reglergrenzen bleiben gültig.

Im Plugin gilt das vom Host gemeldete Tempo, in der Standalone das interne
Sequencer-Tempo. Tempoänderungen erhalten den Fortschritt laufender Phasen.
Die Kurve zeigt die tatsächlichen Basiszeiten; das Ziehen ihrer Zeitknoten wählt
im BPM-Modus passende Notenwerte. Modus und Notenwerte werden im Preset/Projekt
gespeichert. Alte Zustände bleiben standardmäßig im MS-Modus.
