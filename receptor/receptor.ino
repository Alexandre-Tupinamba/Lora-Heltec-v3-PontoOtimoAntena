#include <heltec_unofficial.h>

// Configurações de Frequência
#define START_FREQ 863.0 // Início do espectro (MHz)
#define END_FREQ 928.0   // Fim do espectro (MHz)
#define STEP_FREQ 1.0    // Incremento (MHz)

// Variáveis globais
String receivedMessage;

void setup() {
  heltec_setup();
  Serial.println("Receptor iniciado");

  // Inicializar o rádio
  if (radio.begin() != RADIOLIB_ERR_NONE) {
    Serial.println("Erro ao inicializar o rádio.");
    while (true);
  }
}

void loop() {
  for (float freq = START_FREQ; freq <= END_FREQ; freq += STEP_FREQ) {
    // Configurar a frequência
    if (radio.setFrequency(freq) != RADIOLIB_ERR_NONE) {
      Serial.printf("Falha ao configurar frequência %.2f MHz\n", freq);
      continue;
    }

    // Esperar por uma mensagem
    if (radio.receive(receivedMessage) == RADIOLIB_ERR_NONE) {
      // Exibir dados recebidos
      Serial.printf("Frequência: %.2f MHz\n", freq);
      Serial.printf("Mensagem: %s\n", receivedMessage.c_str());
      Serial.printf("RSSI: %.2f dBm\n", radio.getRSSI());
      Serial.printf("SNR: %.2f dB\n", radio.getSNR());
    } else {
      Serial.printf("Sem mensagem em %.2f MHz\n", freq);
    }

    // Pausa para evitar conflitos de sincronização
    delay(500);
  }
}
