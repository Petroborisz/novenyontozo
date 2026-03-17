#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
// OLED objektum létrehozása (I2C)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const int airValue = 3165;  
const int waterValue = 1090; 
const int sensorPin = 35; 
const int relay1 = 26;
const int relay2 = 27;
const int threshold = 30; 

int waterErrorCount1 = 0; 
int waterErrorCount2 = 0; 

// --- SZOFTVERES VÍZSZINTMÉRŐHÖZ ---
float tankContent = 500.0;       // Induló vízmennyiség ml-ben (5 decis pohár teszteléshez)
const float waterPerPulse = 100; // Egy 3 mp-es öntözés során kijuttatott víz (gyártó szerint 120L/h = 33 ml/s)

//PUMPAMŰKÖDÉS TÉNYEZŐI:
1. A cső hossztól is függ, ellenállást kelt az áramlásban             I
2.Pontos áramfelvételtől is függ                                      I--------------> pontos meghatározás nehézkes, ezért nagy biztonsági ráhagyás
3. Magasságtól nagyban függ, ha fel kell nyomnia víztömeget például   I

// ------------------------------------------------

// 1. ÖNTÖZÉS ANIMÁCIÓ: 3 vízcsepp splash effekt 
void wateringAnimation(int seconds) {
  long startTime = millis();
  while (millis() - startTime < seconds * 1000) {
    for (int i = 0; i < 6; i++) { // 6 fázisú animáció
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(35, 0);
      display.print("ONTOZES...");
      
      // 3 vízcsepp kirajzolása 
      for (int j = 0; j < 3; j++) {
        // Cseppek pozíciója az idő függvényében 
        int xOffset = (j - 1) * (i * 5); // Középső fix, szélsők távolodnak 
        int yOffset = i * 4; // Lefelé mozgás 
        
        // Vízcsepp alakzat (háromszög + kör) 
        int dropX = 64 + xOffset; // Kijelző közepe (64) + offset
        int dropY = 20 + yOffset;
        
        display.fillTriangle(dropX, dropY, dropX - 4, dropY + 8, dropX + 4, dropY + 8, WHITE);
        display.fillCircle(dropX, dropY + 8, 4, WHITE);
      }
      
      display.display();
      delay(80); // animáció seb
    }
  }
}
// 2. FELSZÍVÓDÁS ANIMÁCIÓ: Lüktető vízcsepp 
void absorbingAnimation(int seconds) {
  long startTime = millis();
  while (millis() - startTime < seconds * 1000) {
    for (int r = 5; r < 20; r++) { 
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(25, 0);
      display.print("FELSZIVODAS...");
      
      display.drawCircle(64, 35, r, WHITE); 
      display.fillCircle(64, 35, 3, WHITE); 
      
      display.display();
      delay(100);
    }
  }
}

// 3. HIBA JELZÉS: Nagy X és kis felirat 
void errorStop() {
  display.clearDisplay();
  
  // Apró felirat felülre 
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("KRITIKUS HIBA: NINCS VIZ!");
  
  display.setTextSize(6); 
  display.setCursor(50, 17);
  display.println("X");
  
  display.display();
}

void setup() {
  Serial.begin(115200);

  // OLED inicializálás
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 OLED nem található"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  
  pinMode(relay1, INPUT); 
  pinMode(relay2, INPUT); 
  
  Serial.println("Rendszer indul OLED kijelzovel...");
} 

void loop() {
  int rawValue = analogRead(sensorPin);
  int moisturePercent = map(rawValue, airValue, waterValue, 0, 100);
  moisturePercent = constrain(moisturePercent, 0, 100);

  // --- OLED KIÍRÁS ---
  display.clearDisplay();
  display.setTextSize(1); 
  display.setCursor(0,0);
  display.println("TALAJNEDVESSEG:");
  
  display.setTextSize(4); 
  display.setCursor(10, 20);
  display.print(moisturePercent);
  display.print("%");
  display.display(); 

  // Serial Monitorra is 
  Serial.print("Nedvesség: ");
  Serial.print(moisturePercent);
  Serial.println("%");

  // --- ÖNTÖZÉSI LOGIKA ---
  if (moisturePercent < threshold) {
    
    // Biztonsági ellenőrzés a tartály elméleti szintjére (2x)
    if (tankContent < (2 * waterPerPulse)) {
      Serial.println("VÉSZLEÁLLÁS !");
      errorStop();
      while(1); 
    }

    int beforeWatering = moisturePercent;

    // --- FÁZIS 1: ÖNTÖZÉS ---
    pinMode(relay1, OUTPUT); digitalWrite(relay1, LOW); 
    pinMode(relay2, OUTPUT); digitalWrite(relay2, LOW); 
    
    wateringAnimation(3); // 3 másodperces ontozes

    pinMode(relay1, INPUT); pinMode(relay2, INPUT); 

    // Levonjuk az elhasznált vizet az elméleti tartályból
    tankContent -= waterPerPulse;
    Serial.print("Megmaradt becsült víz: ");
    Serial.print(tankContent);
    Serial.println(" ml");

    // --- FÁZIS 2: FELSZÍVÓDÁS ---
    absorbingAnimation(7); // 7 másodpercig felszivodas

    // --- FÁZIS 3: ELLENŐRZÉS ---
    int afterWatering = map(analogRead(sensorPin), airValue, waterValue, 0, 100);
    afterWatering = constrain(afterWatering, 0, 100);

    if (afterWatering <= beforeWatering + 2) {
      waterErrorCount1++;
    } else {
      waterErrorCount1 = 0; 
    }

    // HIBA JELZÉS: Nagy X és leállás 
    if (waterErrorCount1 >= 3) {
      errorStop(); 
      while(1); 
    }
  }

  // --- MANUÁLIS VEZÉRLÉS --- 
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd == "be1") { pinMode(relay1, OUTPUT); digitalWrite(relay1, LOW); } 
    else if (cmd == "ki1") { pinMode(relay1, INPUT); }
    else if (cmd == "be2") { pinMode(relay2, OUTPUT); digitalWrite(relay2, LOW); }
    else if (cmd == "ki2") { pinMode(relay2, INPUT); }
    else if (cmd == "0") { pinMode(relay1, INPUT); pinMode(relay2, INPUT); }
  }

  delay(2000);
}

