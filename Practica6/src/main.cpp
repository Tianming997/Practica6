#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

// Definición de pines para ESP32
#define SS_PIN    5
#define RST_PIN   22

MFRC522 rfid(SS_PIN, RST_PIN); // Instancia del lector

void setup() {
  Serial.begin(115200);
  SPI.begin();           // Inicializa el bus SPI
  rfid.PCD_Init();       // Inicializa el lector MFRC522
  
  Serial.println("--- Lector RFID listo ---");
  Serial.println("Acerca una tarjeta o llavero...");
}

void loop() {
  // Revisa si hay una nueva tarjeta presente
  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }

  // Selecciona la tarjeta
  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  // Mostrar el UID en el monitor serie
  Serial.print("UID de la tarjeta: ");
  String content = "";
  
  for (byte i = 0; i < rfid.uid.size; i++) {
     Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(rfid.uid.uidByte[i], HEX);
     content.concat(String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(rfid.uid.uidByte[i], HEX));
  }
  
  Serial.println();
  Serial.print("Mensaje: ");
  content.toUpperCase();

  // Ejemplo de validación por UID (reemplaza con tu ID)
  if (content.substring(1) == "XX XX XX XX") { // Sustituye por tu UID real
    Serial.println("Acceso Concedido");
  } else {
    Serial.println("Acceso Denegado");
  }

  // Detener lectura para no repetir el proceso constantemente
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}