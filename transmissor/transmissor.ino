#define HELTEC_NO_DISPLAY
#include <heltec_unofficial.h>
#include "../comum.hh"

void setup() {
  heltec_setup();
  Serial.println("Transmissor iniciado");

  if (radio.begin() != RADIOLIB_ERR_NONE) {
    Serial.println("Erro ao inicializar o rádio.");
    while (true);
  }
}

bool esperarSync(float freq, int sf) {
  String receivedSync;
  Serial.printf("Aguardando sincronização para %.2f MHz, SF %d...\n", freq, sf);

  while (true) {
    if (radio.receive(receivedSync) == RADIOLIB_ERR_NONE && receivedSync == "SYNC") {
      Serial.println("Sincronização recebida. Enviando ACK...");
      radio.transmit("ACK");
      return true;
    }
  }
}

void transmitirPacotes(float freq, int sf, int numPacotes) {
  for (int i = 0; i < numPacotes; i++) {
    String message = String("FREQ: ") + freq + " SF: " + sf + " PKG: " + i;
    Serial.printf("Transmitindo: %s\n", message.c_str());

    if (radio.transmit(message) != RADIOLIB_ERR_NONE) {
      Serial.println("Erro ao transmitir pacote.");
    }

    // Verificar se o receptor confirmou a recepção de todos os pacotes
    String confirmation;
    if (radio.receive(confirmation) == RADIOLIB_ERR_NONE && confirmation == "DONE") {
      Serial.println("Todos os pacotes recebidos pelo receptor. Avançando...");
      break;
    }

    delay(200);  // Intervalo entre pacotes
  }
}

void loop() {
  heltec_loop();

  for (float freq = START_FREQ; freq <= END_FREQ; freq += STEP_FREQ) {
    for (int sf : {11, 12}) {  // Testar SF 11 e 12
      if (radio.setFrequency(freq) != RADIOLIB_ERR_NONE || radio.setSpreadingFactor(sf) != RADIOLIB_ERR_NONE) {
        Serial.printf("Erro ao configurar %.2f MHz, SF %d\n", freq, sf);
        continue;
      }

      if (esperarSync(freq, sf)) {
        transmitirPacotes(freq, sf, 10);
      } else {
        Serial.printf("Sincronização falhou para %.2f MHz, SF %d\n", freq, sf);
      }
    }
  }

  Serial.println("Teste concluído. Aguardando botão para reiniciar.");
  while (!button.isSingleClick()) {
    heltec_loop();
  }
}
