#!/nico/.local/lib python3
# -*- coding: utf-8 -*-

import socket
import struct
import sys
import time
import argparse
import os
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np

dummy_os = os.system("clear")
    

client_socket_tcp = 0
client_socket_udp = 0
data = 0
data_sensor = 0
time_interleave_aux = 0
plot_close_event = 0
x = 0

PUERTO_TCP_COMU = 23400
PUERTO_UDP_1 = 23401
PUERTO_UDP_2 = 23402
TAM_MAX_DAT = 2048

ax_1 = []
ax_2 = []
ax_3 = []
ax_4 = []
ax_5 = []
ax_6 = []
ax_7 = []


try: 
    parser= argparse.ArgumentParser()
    # parser.add_argument("server_ip",help="Define server IP")
    server_ip = '192.168.7.2'
    # server_ip = '127.0.0.1'
    parser.add_argument("server_port",help="Define server Port connection")
    args = parser.parse_args()

    print('·-----------------------------------------------------------------------·')
    print('|                                                                       |')
    print('|                            TD3 - Client                               |')
    print('|                          T.P.: Lucas Liaño                            |')
    print('|                             UTN - FRBA                                |')
    print('|                             2ºC - 2023                                |')
    print('|                                                                       |')
    print('·-----------------------------------------------------------------------·')

    # Creating socket
    client_socket_tcp = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
    client_socket_tcp.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR,1)    
    # client_socket_tcp.connect((args.server_ip,int(args.server_port)))
    client_socket_tcp.connect((server_ip,int(args.server_port)))
    
    print('[ Cliente ]-$ Esperando al servidor')
    data = client_socket_tcp.recv(TAM_MAX_DAT).decode()

    if data != 'OK':
        print('[ Cliente ]-$ El servidor no está disponible')
        client_socket_tcp.close()
    else:
        print('[ Cliente ]-$ El servidor se encuentra disponible')
        input('[ Cliente ]-$ Presionar \"enter\" para continuar ...')
    
        client_socket_tcp.send(b'AK')
        data = client_socket_tcp.recv(TAM_MAX_DAT).decode()
        print(f'data: {data}')
        print(f'data == "OK": {data == "OK"}')

        if data != 'OK':
            print('[ Cliente ]-$ Hubo algún error al establecer la comunicación UDP')
            client_socket_tcp.close()
        else:
            
            time.sleep(2)

            print('[ Cliente ]-$ Comenzando recepción de datos y lanzando graficador ...')
            
            while (plot_close_event == 0):
                # Envío KA (Keep Alive) al server
                client_socket_tcp.send(b'KA')
                print('KA sent')
                
                data_sensor = client_socket_tcp.recv(TAM_MAX_DAT).decode()        

                print(f"Recibi: \n{data_sensor}")

except OSError as e:
    # Envíamos END al server para avisar que cierre la conexión
    client_socket_tcp.send(b'EN')
    client_socket_tcp.close()
    print('[ Cliente ] - Error en cliente: %s.' %(sys.stderr))
except KeyboardInterrupt:
    # Envíamos END al server para avisar que cierre la conexión
    client_socket_tcp.send(b'EN')
    client_socket_tcp.close()
    print('[ Cliente ] - Ctrl-C hitted - Aplicación cliente finalizada')
finally:
    if client_socket_tcp != 0:
        # Envíamos END al server para avisar que cierre la conexión
        client_socket_tcp.send(b'EN')
        client_socket_tcp.close()
        print('[ Cliente ] - Aplicación cliente finalizada')