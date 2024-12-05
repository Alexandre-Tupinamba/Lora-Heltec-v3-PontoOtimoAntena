import serial 
import csv
import time
import threading
import logging
import os
from datetime import datetime

# Configuração de Logs
logging.basicConfig(level=logging.INFO, filename='arduino_log.txt', filemode='a',
                    format='%(asctime)s - %(levelname)s - %(message)s')

# Configuração de Porta Serial
SERIAL_PORT = 'COM4'  # Altere a porta aqui se necessário
BAUD_RATE = 115200

buffer = ""  # Buffer para leitura serial
new_data = []  # Dados a serem salvos no CSV

# Configuração do cabeçalho CSV
HEADERS = ["Timestamp", "Frequência", "RSSI"]  # "SNR" pode ser adicionado aqui se necessário

# Conexão Serial
def connect_serial():
    """
    Tenta conectar continuamente à porta serial até estabelecer a conexão.
    """
    global ser
    while True:
        try:
            ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
            print("Conexão com a porta serial estabelecida.")
            logging.info("Conexão com a porta serial estabelecida.")
            return
        except serial.SerialException as e:
            print(f"Erro ao conectar à porta serial: {e}. Tentando novamente em 5 segundos...")
            logging.error(f"Erro ao conectar à porta serial: {e}")
            time.sleep(5)

connect_serial()

# Criação do arquivo CSV
if not os.path.exists('transmissor_data.csv'):
    with open('transmissor_data.csv', 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(HEADERS)  # Escreve o cabeçalho no CSV

# Dicionário para manter o estado atual
current_record = {}

# Função para processar as linhas recebidas
def process_received_data(lines):
    """
    Processa as linhas relevantes e extrai os dados necessários.
    """
    global new_data, current_record
    try:
        for line in lines:
            print(f"Recebido: {line}")
            line = line.strip()
            if line.startswith("Frequência:"):
                current_record = {}  # Reinicia o dicionário para um novo registro
                frequencia = line.split(":", 1)[1].strip().split()[0]  # Extrai o valor numérico
                current_record['Frequência'] = frequencia
            elif "RSSI:" in line:
                rssi = line.split(":", 1)[1].strip().split()[0]  # Apenas o valor numérico
                current_record['RSSI'] = rssi
                # Adiciona o carimbo de tempo
                timestamp = datetime.now().strftime("%d/%m/%Y %H:%M:%S.%f")[:-3]
                current_record['Timestamp'] = timestamp
                # Verifica se todos os campos estão presentes
                if all(field in current_record for field in HEADERS):
                    # Adiciona o registro aos novos dados
                    new_data.append([current_record[field] for field in HEADERS])
                    print(f"Processado: {current_record}")
                    current_record = {}  # Reinicia para o próximo registro
            # Código para processar o SNR (comentado)
            # elif "SNR:" in line:
            #     snr = line.split(":", 1)[1].strip().split()[0]  # Apenas o valor numérico
            #     current_record['SNR'] = snr
    except Exception as e:
        logging.error(f"Erro ao processar linha: {line}. Erro: {e}")

# Leitura e Processamento
def read_and_process_data():
    """
    Lê dados da porta serial e processa as linhas.
    """
    global buffer
    try:
        buffer += ser.read(ser.in_waiting).decode('utf-8')
        if "\n" in buffer:
            lines = buffer.split("\n")
            buffer = lines[-1]  # Mantém a última linha incompleta no buffer
            process_received_data(lines[:-1])  # Processa todas as linhas completas
    except Exception as e:
        logging.error(f"Erro inesperado ao ler da porta serial: {e}")

# Salvamento Incremental para CSV
def save_to_csv():
    """
    Salva dados periodicamente no arquivo CSV.
    """
    global new_data
    while True:
        if new_data:  # Salva apenas se houver novos dados
            print("Salvando dados no CSV...")
            with open('transmissor_data.csv', 'a', newline='') as csvfile:
                writer = csv.writer(csvfile)
                writer.writerows(new_data)
            new_data = []  # Limpa dados já salvos
        time.sleep(5)

# Thread para salvamento periódico
threading.Thread(target=save_to_csv, daemon=True).start()

# Loop Principal
try:
    while True:
        read_and_process_data()
except KeyboardInterrupt:
    print("Encerrando o programa...")
    logging.info("Programa encerrado pelo usuário.")
