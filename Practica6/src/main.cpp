#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

// Configuración de pines para ESP32 DevKit
#define SS_PIN    5
#define RST_PIN   22

MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key; 

void setup() {
  Serial.begin(115200);
  while (!Serial); 
  
  SPI.begin();
  rfid.PCD_Init();

  // 1. DIAGNÓSTICO DEL CHIP
  byte v = rfid.PCD_ReadRegister(MFRC522::VersionReg);
  Serial.println("\n------------------------------");
  Serial.print("Version del chip: 0x");
  Serial.println(v, HEX);
  
  // Cargar llave por defecto
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  Serial.println("Sistema listo. Acerca una tarjeta...");
  Serial.println("------------------------------\n");
}

void loop() {
  // Detectar tarjeta
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) return;

  Serial.println("********** TARJETA DETECTADA **********");

  // --- AQUI IMPRIMIMOS EL CODIGO DE LA TARJETA (UID) ---
  Serial.print("CODIGO HEX: ");
  String uidHex = "";
  unsigned long uidDec = 0;

  for (byte i = 0; i < rfid.uid.size; i++) {
    // Formato Hexadecimal
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(rfid.uid.uidByte[i], HEX);
    
    // Formato Decimal (Calculo acumulado)
    uidDec <<= 8;
    uidDec |= rfid.uid.uidByte[i];
  }
  Serial.println();
  
  Serial.print("CODIGO DEC: ");
  Serial.println(uidDec);
  // -----------------------------------------------------

  // Tipo y Memoria
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.print("Tipo: ");
  Serial.println(rfid.PICC_GetTypeName(piccType));

  // Operaciones de Memoria (Escritura en Bloque 4)
  byte bloque = 4;
  byte mensaje[16] = "ESP32_USER_2026"; 
  MFRC522::StatusCode status;

  Serial.println("\nAutenticando...");
  status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, bloque, &key, &(rfid.uid));
  
  if (status == MFRC522::STATUS_OK) {
    // Escribir
    rfid.MIFARE_Write(bloque, mensaje, 16);
    Serial.println("Mensaje escrito en bloque 4.");

    // Leer todo el Sector 1 (Bloques 4 al 7) para ver contenido
    Serial.println("\nLECTURA DE MEMORIA (Sector 1):");
    byte buffer[18];
    byte tamano = sizeof(buffer);

    for (byte b = 4; b <= 7; b++) {
      status = rfid.MIFARE_Read(b, buffer, &tamano);
      if (status == MFRC522::STATUS_OK) {
        Serial.print("B["); Serial.print(b); Serial.print("]: ");
        for (byte i = 0; i < 16; i++) {
          Serial.print(buffer[i] < 0x10 ? " 0" : " ");
          Serial.print(buffer[i], HEX);
        }
        Serial.print(" | ");
        for (byte i = 0; i < 16; i++) {
          Serial.write(buffer[i] >= 32 && buffer[i] <= 126 ? buffer[i] : '.');
        }
        Serial.println();
      }
    }
  } else {
    Serial.print("Error: ");
    Serial.println(rfid.GetStatusCodeName(status));
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  Serial.println("\n***************************************\n");
  delay(3000);
}