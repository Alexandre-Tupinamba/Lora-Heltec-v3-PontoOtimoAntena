#define HELTEC_NO_DISPLAY
#include <heltec_unofficial.h>
#include "../comum.hh"

String receivedMessage;

void setup() {
  heltec_setup();
  Serial.println("Receptor iniciado");

  if (radio.begin() != RADIOLIB_ERR_NONE) {
    Serial.println("Erro ao inicializar o rádio.");
    while (true);
  }
}

void esperarBotao() {
  Serial.println("Aperte o botão PRG para iniciar o teste.");
  while (!button.isSingleClick()) {
    heltec_loop();
  }
  Serial.println("Iniciando teste...");
}

void receberPacotes(float freq, int sf, int numPacotes) {
  int pacotesRecebidos = 0;
  bool pacoteDuplicado = false;

  while (pacotesRecebidos < numPacotes) {
    if (radio.receive(receivedMessage) == RADIOLIB_ERR_NONE) {
      Serial.printf("Recebido: %s\n", receivedMessage.c_str());
      Serial.printf("RSSI: %.2f dBm\n", radio.getRSSI());
      Serial.printf("SNR: %.2f dB\n", radio.getSNR());

      // Verificar duplicação
      String pacoteId = receivedMessage.substring(receivedMessage.lastIndexOf("PKG: ") + 5);
      if (pacoteId.toInt() == pacotesRecebidos) {
        pacotesRecebidos++;
        pacoteDuplicado = false;
      } else {
        pacoteDuplicado = true;
      }

      // Confirmar pacote ao transmissor
      if (!pacoteDuplicado) {
        radio.transmit("ACK");
      }
    }
  }

  // Notificar transmissor que todos os pacotes foram recebidos
  Serial.printf("Todos os pacotes recebidos para %.2f MHz, SF %d\n", freq, sf);
  radio.transmit("DONE");
}

void loop() {
  esperarBotao();

  for (float freq = START_FREQ; freq <= END_FREQ; freq += STEP_FREQ) {
    for (int sf : {11, 12}) {
      if (radio.setFrequency(freq) != RADIOLIB_ERR_NONE || radio.setSpreadingFactor(sf) != RADIOLIB_ERR_NONE) {
        Serial.printf("Erro ao configurar %.2f MHz, SF %d\n", freq, sf);
        continue;
      }

      // Sincronizar com o transmissor
      Serial.printf("Sincronizando transmissor para %.2f MHz, SF %d...\n", freq, sf);
      if (radio.transmit("SYNC") == RADIOLIB_ERR_NONE) {
        String ackMessage;
        while (true) {
          if (radio.receive(ackMessage) == RADIOLIB_ERR_NONE && ackMessage == "ACK") {
            Serial.println("ACK recebido. Transmissor sincronizado.");
            break;
          }
        }
        receberPacotes(freq, sf, 10);
      }
    }
  }

  Serial.println("Teste concluído. Aguardando botão para reiniciar.");
}
