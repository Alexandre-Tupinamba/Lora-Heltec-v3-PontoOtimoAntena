// Desliga a funcionalidade do display OLED na biblioteca, já que
// não está sendo usado atualmente.
#define HELTEC_NO_DISPLAY
#include <heltec_unofficial.h>
#include "../comum.hh"

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

// Espera a entrada do usuário para iniciar um novo teste e,
// depois, espera mais `n` segundos para retornar.
void esperarBotao(unsigned int n) {
  Serial.println("Aperte o botão PRG para iniciar o teste");

  while (!button.isSingleClick()) {
    heltec_loop(); // Necessário para atualizar o estado do botão
  }

  Serial.printf("Iniciando em %us...\n", n);
  heltec_delay(n * 1000);
}

void loop() {
  heltec_loop();

  // Esperar mais tempo para o primeiro teste
  esperarBotao(5);

  for (float freq = START_FREQ; freq <= END_FREQ; freq += STEP_FREQ) {
    // Configurar a frequência
    if (radio.setFrequency(freq) != RADIOLIB_ERR_NONE) {
      Serial.printf("Falha ao configurar frequência %.2f MHz\n", freq);
      continue;
    }

    // Esperar por uma mensagem e ligar o indicador visual
    heltec_led(30);
    if (radio.receive(receivedMessage) == RADIOLIB_ERR_NONE) {
      // Exibir dados recebidos
      Serial.printf("Frequência: %.2f MHz\n", freq);
      Serial.printf("Mensagem: %s\n", receivedMessage.c_str());
      Serial.printf("RSSI: %.2f dBm\n", radio.getRSSI());
      Serial.printf("SNR: %.2f dB\n", radio.getSNR());
    } else {
      Serial.printf("Sem mensagem em %.2f MHz\n", freq);
    }

    heltec_led(0);

    // Esperar o usuario apertar o botão
    esperarBotao(3);
  }
}
