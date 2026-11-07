# Nexum

Nexum ist der Synthesizer der NEXUM-Familie, als eigenes VST3-Instrument und Standalone.
Die bisherige Nexum-Version bleibt daneben installiert. Bestehende FL-Studio-Projekte
verwenden weiterhin Nexum mit seiner bisherigen Plugin-Kennung. Nexum hat eine neue,
eigene Kennung und ersetzt keine Instanzen in alten Projekten.

In FL Studio: Options > Manage plugins > Find installed plugins. Danach das Instrument
**Nexum** auswählen. VST3: `C:/Program Files/Common Files/VST3/NEXUM/Nexum.vst3`.
Standalone: `the installed standalone directory`.

Eine durchgehende linke Seitenleiste enthält OSC, MIX, FX, MATRIX und PACKS
untereinander. Darunter stehen standardmaessig vier Makros und ein Plus-Feld zum Erweitern auf
bis zu acht Makros in einer einzigen Spalte, auf
allen Seiten verfügbar. Namen, Farben, Zuweisungsringe und Automation bleiben
erhalten. Die frühere Makro-Karte entfällt; ENV und LFO nutzen den zusätzlichen
Platz. Der Preset-/Master-Bereich erstreckt sich über den oberen Inhaltsbereich.

Die Generator-/Filterseite verwendet durchgehende dunkle Displaybereiche bis zum
Kartenrand. OSC-, Noise- und Filterkurven teilen sich Höhe und Plot-Bereich, auch
im Multisample-Modus. Die Regler und Wertfelder darunter liegen auf gemeinsamen
Zeilen; Sub und Noise verwenden dieselben Pan-/Level-Zeilen.

Sub: OCT verschiebt die Oktave, CRS stimmt in Halbtönen (-48 bis +48), und Φ legt
die Startphase pro Anschlag in Grad fest. Die sechs Wellenformen sind direkt in
einem Raster mit Trennlinien wählbar; die ausgewählte Welle leuchtet blau.
CRS und Phase sind automatisierbar und werden im Preset/Projekt gespeichert.
Alte Zustände erhalten für beide neuen Regler den neutralen Wert 0.

Sub und Noise besitzen rechts im Header dieselbe kompakte Routing-Anzeige wie die
OSC. Anklicken wählt Filter 1, Filter 2, beide Filter, Main, Direct, None oder Bus 1/2.
Auto behält das ursprüngliche Preset-Routing; die blauen Filterbalken zeigen dessen
Filter-Ziele. Header und MIX-Tab bearbeiten denselben gespeicherten Parameter.

Rechts unten ersetzen VELO-/NOTE-Kurven und der Voicing-/Portamento-Bereich das
bisherige Voicing/FX-Panel. Kurvenringe auf Regler ziehen, um sie zuzuweisen.
Mono/Legato, Always/Scaled und die Portamento-Kurve arbeiten in der Synth-Engine.
Die bisherigen FX-Schnellregler und Import-Informationen stehen im „…“-Menü.
Details: `README-Voicing.md`.

ENV 1–4 besitzen eine dunkle gemeinsame Wertleiste und eine eigene BPM/MS-Umschaltung.
BPM synchronisiert Attack, Hold, Decay und Release auf Host-/Standalone-Tempo,
einschließlich Triolen und punktierter Notenwerte. Details: `README-ENV.md`.

Synth-Engine, Parameter-IDs und Preset-/Zustandsformat werden mit Nexum geteilt.
Eigene .ncsynth-Presets können in beiden Versionen geöffnet werden. Ein Wechsel des
Plugins in einem vorhandenen Projekt geschieht ausdrücklich manuell; DAW-Automationen
werden dadurch nicht automatisch auf eine neue Instanz übertragen.

Packs, Wavetables, Multisamples und FX-Library verwenden weiterhin ihre bestehenden
Ordner unter `Desktop/VST3 Plugins/Nexum`. Dadurch bleiben alle importierten Inhalte
und Metadaten verfügbar. Diesen Ordner weiterhin behalten. Die FX-Implementierungen
und FX-Library sind auch mit Nexum FX gemeinsam.

Build: CMake-Targets `NexumSynth_VST3` und `NexumSynth_Standalone`.
`Install-Nexum.ps1` installiert ausschließlich Nexum, ohne Nexum oder Nexum FX zu
ersetzen. `Update.ps1` aktualisiert bei späteren vollständigen Builds alle drei Produkte.


## Dynamische Makros
Vier Makros sind standardmaessig sichtbar. Das Plus fuegt weitere Makros ohne feste Acht-Makro-Grenze hinzu; die Liste scrollt unabhaengig von Navigation und Versionsnummer. Werte, Namen, Farben und Zuweisungen werden im Preset und im DAW-Projekt gespeichert. Zuweisen wie bisher durch Ziehen der Verknuepfungsanzeige auf einen Regler oder Makro auswaehlen und Shift-Klick auf das Ziel.
Die ersten acht Makros behalten ihre DAW-Automationsparameter. Weitere Makros steuern ihre zugewiesenen Regler innerhalb des Plugins und werden im Preset gespeichert; sie erscheinen nicht als zusaetzliche FL-Studio-Automationsparameter.
