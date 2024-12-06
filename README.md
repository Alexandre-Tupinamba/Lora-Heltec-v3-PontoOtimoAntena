## Descrição

Este script Python é projetado para ler dados de uma porta serial, processar entradas específicas e salvar as informações em um arquivo CSV. Especificamente, ele captura as informações de "Frequência" e "RSSI" provenientes de um dispositivo conectado via serial, adiciona um carimbo de tempo e salva esses dados para análise posterior. Para isso, você precisa ter os códigos `Teste_pontootimo_antena_LoRa_Reciever.ino` e `Teste_pontootimo_antena_LoRa_tranmissor.ino` carregados, respectivamente, em dois módulos ESP32 LoRa Heltec v3, um atuando como receptor e outro como transmissor. O script em Python então fará a leitura da porta serial do dispositivo receptor, processando e registrando as informações no CSV.

## Funcionalidades

- Conecta-se automaticamente à porta serial especificada.
- Lê e processa dados da porta serial em tempo real.
- Extrai informações de "Frequência" e "RSSI" das entradas recebidas.
- Salva os dados em um arquivo CSV com um carimbo de tempo.
- Registra eventos e erros em um arquivo de log.
- Possui código comentado para processar o "SNR" caso seja necessário no futuro.
- Funciona em conjunto com dois ESP32 LoRa Heltec v3: um rodando o código `Teste_pontootimo_antena_LoRa_tranmissor.ino` e outro rodando `Teste_pontootimo_antena_LoRa_Reciever.ino`.

## Requisitos

- Python 3.x
- Biblioteca `pyserial` para comunicação serial.
- Permissões adequadas para acessar a porta serial no sistema operacional.
- Dois ESP32 LoRa Heltec v3:
  - Um com o código `Teste_pontootimo_antena_LoRa_tranmissor.ino`
  - Outro com o código `Teste_pontootimo_antena_LoRa_Reciever.ino`

## Instalação

1. **Carregar os Códigos nos ESP32 LoRa**  
   - Conecte o ESP32 LoRa Heltec v3 ao seu computador e abra a IDE Arduino ou PlatformIO.
   - Carregue o sketch `Teste_pontootimo_antena_LoRa_tranmissor.ino` no ESP32 LoRa que será o transmissor.
   - Carregue o sketch `Teste_pontootimo_antena_LoRa_Reciever.ino` no ESP32 LoRa que será o receptor.
   - Certifique-se de que ambos os dispositivos estão alimentados e configurados corretamente.

2. **Clonar o Repositório ou Baixar o Script Python**

   Faça o download do script Python para o seu computador.

3. **Instalar Dependências**

   Abra um terminal ou prompt de comando e instale a biblioteca `pyserial` usando o pip:

   ```bash
   pip install pyserial
   ```

4. **Configurar o Script Python**

   - Abra o script em um editor de texto ou IDE.
   - Verifique e ajuste as seguintes configurações conforme necessário:
     - **Porta Serial**: Atualize a variável `SERIAL_PORT` com a porta serial correta (por exemplo, `'COM4'` no Windows ou `'/dev/ttyUSB0'` no Linux).
     - **Taxa de Baud**: Certifique-se de que a variável `BAUD_RATE` corresponda à configuração do seu dispositivo (padrão é `115200`).

## Uso

1. **Conectar o Dispositivo Receptor via Porta Serial**

   Conecte o ESP32 LoRa Heltec v3 (com o código `Teste_pontootimo_antena_LoRa_Reciever.ino`) à porta serial do seu computador. O outro ESP32, com o código de transmissor, deve estar ligado e enviando dados pela rede LoRa.

2. **Executar o Script Python**

   No terminal ou prompt de comando, navegue até o diretório onde o script está localizado e execute:

   ```bash
   python nome_do_script.py
   ```

   Substitua `nome_do_script.py` pelo nome real do arquivo do script.

3. **Monitorar a Execução**

   - O script tentará conectar-se à porta serial especificada.
   - Uma vez conectado, ele começará a ler e processar os dados recebidos do ESP32 LoRa Receptor.
   - As informações extraídas (Frequência e RSSI) serão exibidas no terminal e salvas no arquivo `transmissor_data.csv`.
   - Eventos e erros serão registrados no arquivo `arduino_log.txt`.

4. **Interromper a Execução**

   Para encerrar o script, pressione `Ctrl + C` no terminal. O script irá capturar a interrupção e finalizar a execução de forma graciosa.

## Arquivos Gerados

- **transmissor_data.csv**: Arquivo CSV onde os dados de "Frequência", "RSSI" e o carimbo de tempo são salvos.
- **arduino_log.txt**: Arquivo de log que registra eventos importantes e erros durante a execução do script.

## Personalização

### Ativar o Salvamento do SNR

Se você quiser também salvar os dados de "SNR":

1. **Atualizar o Cabeçalho CSV**

   Descomente "SNR" na variável `HEADERS`:

   ```python
   HEADERS = ["Timestamp", "Frequência", "RSSI", "SNR"]
   ```

2. **Descomentar o Código de Processamento do SNR**

   Na função `process_received_data()`, descomente as linhas relacionadas ao "SNR":

   ```python
   elif "SNR:" in line:
       snr = line.split(":", 1)[1].strip().split()[0]  # Apenas o valor numérico
       current_record['SNR'] = snr
   ```

3. **Salvar as Alterações**

   Salve o script após fazer as alterações.

### Ajustar o Intervalo de Salvamento

Por padrão, o script salva os novos dados no arquivo CSV a cada 10 segundos. Para alterar esse intervalo:

1. **Encontrar a Função `save_to_csv()`**

   Localize o seguinte trecho:

   ```python
   time.sleep(10)
   ```

2. **Alterar o Valor**

   Substitua `10` pelo número de segundos desejado.

## Resolução de Problemas

- **Erro ao Conectar à Porta Serial**

  Se o script não conseguir conectar-se à porta serial, verifique se:

  - O dispositivo receptor (ESP32 LoRa com o código `Teste_pontootimo_antena_LoRa_Reciever.ino`) está corretamente conectado.
  - A porta serial especificada em `SERIAL_PORT` está correta.
  - Você tem as permissões necessárias para acessar a porta serial.
  - O transmissor (ESP32 LoRa com o código `Teste_pontootimo_antena_LoRa_tranmissor.ino`) está funcionando corretamente, enviando dados.

- **Dados Não Estão Sendo Salvos no CSV**

  - Certifique-se de que o transmissor esteja enviando dados no formato esperado.
  - Verifique se o script está recebendo e processando as linhas corretas (elas devem começar com "Frequência:" e conter "RSSI:").

- **Exceções ou Erros Inesperados**

  - Consulte o arquivo `arduino_log.txt` para detalhes sobre o erro.
  - Verifique se todas as dependências estão instaladas corretamente.
  - Certifique-se de que os códigos nos ESP32 LoRa estão rodando sem erros.
