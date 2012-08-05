Internes Pflichtenheft für die Physik
=====================================

1. Beschreibende Elemente
-------------------------

Ein Element in der Physik von Schizophrenia bestehe aus folgenden
Attributen:
- Position
- Geschwindigkeit (für x und y gesondert)
- Attribute
  - Wirken Kräfte? (Verschieben, Schwerkraft, etc.)
  - Semipermiabilität für jede der 4 Richtungen
- Wie lange schon im freien Fall => Für Gravitation
Desweiteren besitzt jedes Element eine Sicherung des letzten korrekten
Zustandes ohne Kollision. Vor allem für das Zurücksetzen z.B. beim
Laufen gegen eines Wand oder Fallen gegen den Boden.
Wie ersichtlich ist, gibt es keine Beschleunigung. Die Gravitations-
beschleunigung wird über die Zeit, die im freien Fall zugebracht wurde,
angenähert. Auf diese Art und Weise kann man ein angenehmes, aber nicht
physikalisch korrektes Verhalten erreichen

2. Verhalten der Elemente
-------------------------

Die Physik erfolgt immer in 1 ms Schritten. Wenn das zu langsam ist,
können auch ganzzahlige Vielfache Verwendung finden.

Bevor Kollisionserkennung, Physik, usw. wirkt, wird davon ausgegangen,
dass die Szene KOLLISIONSFREI ist. Desweiteren muss die Geschwindigkeit
für jedes Element gesetzt werden, da sie NACH der vorherigen
Physikwirkung auf 0 gesetzt wurde.

Zuerst wird jedes Element unter Vorbehalt um seine Y-Geschwindigkeit
bewegt. Hierbei wirkt schon Gravitation (wenn sie wirkt).
Nun erfolgt eine Kollisionskontrolle. Aus naheliegenden Gründen können
diese hier nur oben und unten auftreten. Diese Liste von Kollisionen
wird gespeichert und nun kann erstmal von außen reagiert werden.
So kann ein Spieler sterben, wenn er von oben eine Kollision
registriert (aber bitte nicht, wenn er wo gegen springt...). Damit die
Engine nicht weiter Physik ausführt, muss die Kollision gelöscht und die
vorherige Position wieder ausgeführt werden. Nun führt die Engine ihre
Physik aus.
Kollisionen, wo ein Element "von unten" kommt, können eigentlich
ignoriert werden. Das einzige, was springt, ist der Spieler und der
hat eine Abhandlung über eine Kollision oben beim Sterbetest.
Interessanter sind Kollisionen, wo eine Bewegung nach unten vorweg
ging. Die entsprechenden Element werden zurückgesetzt, aber als
gravitativ-abhängig markiert => Wenn sie verschoben werden, wird alles
darüber auch verschoben.

Am Ende werden alles y-Geschwindigkeiten auf 0 gesetzt. Elemente, die
gefallen aber nicht kollidiert sind, bekommen ihren Fallcounter
inkrementiert.

Jedes Element wird nun unter Vorbehalt um seine X-Geschwindigkeit bewegt.
Es erfolgt eine Kollisionskontrolle. Aus naheliegenden Gründen können
Kollisionen hier nur links oder rechts auftreten.
An diesem Punkt kann es nun wieder Feedback von außen geben. Man kann
die Situation selbst von außen lösen, indem der Spieler z.B. stirbt, ein
Gegner umdreht OHNE was zu verschieben oder eine Platform sich umdreht.
Insofern die Physik"engine" hier nicht weiter agieren soll, muss die
Position der Kollisionspartner hier zurückgesetzt werden und die
Kollision entfernt (damit eine Kiste eben nicht verschoben wird ;-) ).
Noch übrige Kollisionen werden von der Physik behandelt. Bei dieser
links-rechts-Physik kann es eigentlich nur Verschiebungen geben.
Die Geschwindigkeitsdifferenz gibt an, welcher Part der Kollision
sich "reinbewegt" hat und die Verschiebung somit ausführt. Nun wird
geschaut, was alles verschoben wird. Die Implementierung muss noch
erdacht werden. Was klar ist: Wenn nur ein Element nicht verschoben
werden kann, steht alles still.
Wie auch immer. Am Ende hat man ein verschiebenes und verschobene
Elemente. Alle bekommen ihre Geschwindigkeit genullt und erhalten
stattdessen je einen Teil der Geschwindigkeit es Verschiebers. Fällt
diese Geschwindigkeit unter einen Grenzwert, wird die Geschwindigkeit
auf 0 gesetzt: Keine Verschiebung.

"Zerquetschungen" müssen von außen erkannt werden. Wenn eine Platform
z.B. Verschieben WÜRDE (also am Besten als Funktion definieren, die auch
von außen Verschiebecluster erkennen kann) und ein Spieler dabei ist,
sollte dieser sterben.

Nun kann es passieren, dass wir wieder eine Kollision erzeugt haben.
Deshalb werden die oberen Schritte wiederholt bis keine Kollision
auftritt bzw. (worst case) alle Positionen zurückgesetzt sind. Dieser
Test ist u.U. langsam (Testen!), aber optimierbar (bei Bedarf) und in
JEDEM Fall: endlich.

Am Ende werden die Geschwindigkeiten auf 0 gesetzt.

3. Abbildung der Elemente
-------------------------

Physik und Level sind mehr oder minder strikt getrennt. Auch Tiles sind
Elemente in der Physik, aber auf sie wirkt keine Kraft und ihre
Geschwindigkeit bleibt stets 0. Sollte die Physik zu langsam sein,
sind sie ein Punkt an dem natürlich eine Menge optimiert werden kann.

4. Kollisionskontrolle
----------------------

Kollisionskontrolle zwischen zwei unrotierten Rechtecken ist trivial
und komplett unabhängig, wo der Ortungspunkt des Rechtsecks ist (also ob
z.B. Links/Oben oder in der Mitte).

5. Klonen
---------

Klonen hat mit der Physik nichts zu tun, aber beim Klonen darf keine
Kollision erzeugt werden, da stets von Kollisionsfreiheit ausgegangen
wird.
