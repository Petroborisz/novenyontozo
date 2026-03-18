# Automata Öntözőrendszer
Ez egy Arduino IDE-n fejlesztett okos öntözőrendszer, amely méri a talajnedvességet és annak megfelelően, szükség szerint öntöz. Szobanövényekre tervezve (egyelőre).

### Funkciók:
* Automatikus öntözés: 30% alatti nedvességnél bekapcsol.
* OLED kijelző: Élő adatmegjelenítés és animációk az egyes működési fázisokhoz.
* Szoftveres védelem:
   * Leállítja a rendszert, ha a tartály kiürülne, ezzel védve a vízpumpa légszívó, felmelegedett állapotától.
   * Figyeli az öntözések során történő nedvességszint-változást, ha a pumpa lecsatlakozik vagy nem jut el a virágföldig víz, hibaszámláló alapján a rendszer leáll.

### Hardver:
* ESP32 Microvezérlő
* HW-390 Kapacitív talajnedvesség szenzor
* 2 csatornás relé modul (alacsony triggerszintű)
* DC 5V Mini horizontális vízpumpa
* SSD1306 OLED kijelző
* Egyebek:
  * 1 m hosszú cső
  * jumper kábelek
  * passzív zümmögő a hangjelzéshez
  
### Továbbfejlesztési ötletek:
* Raspberry Pi alkalmazása a tényleges kötetlen működéshez
* AI vezérelt növényöntözés hosszútávú megfigyelés alapján
