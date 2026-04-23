#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN    5
#define RST_PIN   22

MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key; 

void setup() {
  Serial.begin(115200);
  while (!Serial); 
  SPI.begin();
  rfid.PCD_Init();

  // CORRECCIÓN: El miembro correcto es keyByte
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  Serial.println("--- Lector de Memoria y Escritura ---");
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) return;

  // 1. IDENTIFICAR TIPO Y MEMORIA
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.print("Tipo: ");
  Serial.println(rfid.PICC_GetTypeName(piccType));

  // 2. CONFIGURACIÓN DE ESCRITURA
  byte bloque = 4; 
  byte datosEscritura[16] = {"HOLA ESP32 RFID"}; 
  byte bufferLectura[18];
  byte tamano = sizeof(bufferLectura); // CORRECCIÓN: 'tamano' sin ñ
  MFRC522::StatusCode status;

  // Autenticación
  Serial.println("Autenticando...");
  status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, bloque, &key, &(rfid.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Error Auth: ");
    Serial.println(rfid.GetStatusCodeName(status));
    return;
  }

  // Escritura
  status = rfid.MIFARE_Write(bloque, datosEscritura, 16);
  if (status == MFRC522::STATUS_OK) {
    Serial.println("Dato guardado en Bloque 4.");
  } else {
    Serial.print("Error Write: ");
    Serial.println(rfid.GetStatusCodeName(status));
  }

  // Lectura de verificación
  Serial.print("Contenido actual: ");
  status = rfid.MIFARE_Read(bloque, bufferLectura, &tamano); // CORRECCIÓN: 'tamano' sin ñ
  if (status == MFRC522::STATUS_OK) {
    for (byte i = 0; i < 16; i++) {
      Serial.write(bufferLectura[i]);
    }
    Serial.println();
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  Serial.println("--- Listo ---\n");
}