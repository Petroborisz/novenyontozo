# ESP32 Automata Öntözőrendszer
Ez egy okos öntözőrendszer, amely méri a talajnedvességet és szoftveres vízszintfigyeléssel védi a pumpát.

### Funkciók:
* Automatikus öntözés: 30% alatti nedvességnél bekapcsol.
* OLED kijelző: Élő adatmegjelenítés és animációk az egyes működési fázisokhoz.
* Szoftveres védelem:
    1. Leállítja a rendszert, ha a tartály kiürülne, ezzel védve a vízpumpa légszívó, felmelegedett állapotától.
    2. Figyeli az öntözések során történő nedvességszint-változást, ha a pumpa lecsatlakozik vagy nem jut el a virágföldig víz, hibaszámláló alapján a rendszer leáll.

### Hardver:
* ESP32 Microvezérlő
* HW-390 Kapacitív talajnedvesség szenzor
* 2 csatornás relé modul (alacsony triggerszintű)
* DC 5V Mini horizontális vízpumpa
* SSD1306 OLED kijelző
* Egyebek:
    *1 m hosszú cső
    *jumper kábelek
    *passzív zümmögő a hangjelzéshez
