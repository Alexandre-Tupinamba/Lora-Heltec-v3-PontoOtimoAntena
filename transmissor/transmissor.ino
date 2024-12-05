// Desliga a funcionalidade do display OLED na biblioteca, já que
// não está sendo usado atualmente.
#define HELTEC_NO_DISPLAY

#include <heltec_unofficial.h>

// Intervalo de frequências e passo
#define START_FREQ 863.0 // Início do espectro (MHz)
#define END_FREQ 928.0   // Fim do espectro (MHz)
#define STEP_FREQ 1.0    // Incremento (MHz)

void setup() {
  heltec_setup();
  Serial.println("Transmissor iniciado");

  // Inicializar o rádio
  if (radio.begin() != RADIOLIB_ERR_NONE) {
    Serial.println("Erro ao inicializar o rádio.");
    while (true);
  }
}

// Espera a entrada do usuário para iniciar um novo teste e,
// depois, espera mais `n` segundos para retornar.
void esperarBotao(unsigned int n) {
  Serial.println("Aperte o botão para iniciar o teste");
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

    // Mensagem a ser enviada
    String message = "Hello on " + String(freq, 1) + " MHz";
    Serial.printf("Transmitindo: %s\n", message.c_str());

    // Transmitir a mensagem
    if (radio.transmit(message) != RADIOLIB_ERR_NONE) {
      Serial.printf("Falha ao transmitir em %.2f MHz\n", freq);
    }

    // Esperar o usuario apertar o botão
    esperarBotao(3);
  }
}
