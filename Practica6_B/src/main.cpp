#include <Arduino.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>

#define SD_CS_PIN 5

// Función para listar y leer archivos
void explorarSD(File dir, int numTabs) {
  while (true) {
    File entry = dir.openNextFile();
    if (!entry) {
      // No hay más archivos
      break;
    }

    // Espaciado para jerarquía
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print("\t");
    }

    Serial.print(entry.name());

    if (entry.isDirectory()) {
      Serial.println("/");
      explorarSD(entry, numTabs + 1);
    } else {
      // Imprime el tamaño del archivo
      Serial.print("\t\t[TAMANO: ");
      Serial.print(entry.size(), DEC);
      Serial.println(" bytes]");

      // --- LEER EL CONTENIDO DEL FICHERO ---
      Serial.println("--- INICIO DEL CONTENIDO ---");
      while (entry.available()) {
        Serial.write(entry.read());
      }
      Serial.println("\n--- FIN DEL CONTENIDO ---\n");
    }
    entry.close();
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("\n--- EXPLORADOR DE TARJETA SD ---");

  // Inicialización lenta (1MHz) para mayor estabilidad en 3.3V
  if (!SD.begin(SD_CS_PIN, SPI, 1000000)) {
    Serial.println("Error: No se pudo inicializar la SD.");
    Serial.println("Prueba: Revisa que el pin CS sea el GPIO 5.");
    return;
  }

  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No se detecta tarjeta física.");
    return;
  }

  // Empezar la exploración desde la raíz
  File root = SD.open("/");
  if (!root) {
    Serial.println("No se pudo abrir el directorio raíz.");
    return;
  }

  Serial.println("Listando archivos y contenidos:");
  explorarSD(root, 0);
  
  Serial.println("---------------------------------");
  Serial.println("Exploracion finalizada.");
}

void loop() {
  // El programa solo corre una vez al iniciar
}