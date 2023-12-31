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
from ctypes import *

dummy_os = os.system("clear")
    

client_socket_tcp = 0
data = 0
data_sensor = 0
plot_close_event = 0
x = 0

TAM_MAX_DAT = 2048

ax_1 = []
ax_2 = []
ax_3 = []
ax_4 = []
ax_5 = []
ax_6 = []
ax_7 = []

class data_t(Structure):
    """
    typedef struct {
        float rax;
        float ray;
        float raz;
        float rtmp;
        float rgx;
        float rgy;
        float rgz;

        float fax;
        float fay;
        float faz;
        float ftmp;
        float fgx;
        float fgy;
        float fgz;
    } data_t;
    """
    _fields_ = [
        ('rax', c_float),
        ('ray', c_float),
        ('raz', c_float),
        ('rtmp',c_float),
        ('rgx', c_float),
        ('rgy', c_float),
        ('rgz', c_float),
        ('fax', c_float),
        ('fay', c_float),
        ('faz', c_float),
        ('ftmp',c_float),
        ('fgx', c_float),
        ('fgy', c_float),
        ('fgz', c_float)
    ]

def handle_close(evt):
    global plot_close_event
    plot_close_event = 1

fig = plt.figure(figsize=(24,15))
def real_time_plot(x_vec,accel_x,accel_y,accel_z,gyro_x,gyro_y,gyro_z,temp,
                accel_xb,accel_yb,accel_zb,gyro_xb,gyro_yb,gyro_zb,tempb,
                line1,line2,line3,line4,line5,line6,line7,line1b,line2b,line3b,line4b,line5b,line6b,line7b,pause_time=0.1):
    global ax_1
    global ax_2
    global ax_3
    global ax_4
    global ax_5
    global ax_6
    global ax_7

    if line1==[]:
        
        # plt.ion()
        # fig = plt.figure()
        # Esto es para trapear cuando se cierre el plot, y asi terminar la aplicación cliente
        fig.canvas.mpl_connect('close_event', handle_close)
        fig.canvas.manager.set_window_title('TD3 - UTN - FRBA - 2021')

        plt.suptitle('Trabajo Práctico - 2ºC - 2023')
        #----------------------------------------#
        # plot 1
        #----------------------------------------#
        ax_1 = fig.add_subplot(331)        
        line1, = ax_1.plot(x_vec,accel_x,'r--',alpha=0.4,label='Accel. X - Raw')    
        line1b, = ax_1.plot(x_vec,accel_xb,'r-',alpha=0.8,label='Accel. X - MVA')    
        plt.ylabel('Amplitud [g]')
        plt.xlabel('Muestra')
        plt.grid()
        plt.legend(loc='upper left')

        ax_2 = fig.add_subplot(332)                
        line2, = ax_2.plot(x_vec,accel_y,'g--',alpha=0.4,label='Accel. Y - Raw')                       
        line2b, = ax_2.plot(x_vec,accel_yb,'g-',alpha=0.8,label='Accel. Y - MVA')                       
        plt.ylabel('Amplitud [g]')
        plt.xlabel('Muestra')
        plt.grid()
        plt.legend(loc='upper left')

        ax_3 = fig.add_subplot(333)                            
        line3, = ax_3.plot(x_vec,accel_z,'b--',alpha=0.4,label='Accel. Z - Raw')            
        line3b, = ax_3.plot(x_vec,accel_zb,'b-',alpha=0.8,label='Accel. Z - MVA')            
        plt.ylabel('Amplitud [g]')
        plt.xlabel('Muestra')
        plt.grid()
        plt.legend(loc='upper left')
    
        #----------------------------------------#
        # plot 2
        #----------------------------------------#
        ax_4 = fig.add_subplot(334)
        line4, = ax_4.plot(x_vec,gyro_x,'r--',alpha=0.4,label='Giro. X - Raw')
        line4b, = ax_4.plot(x_vec,gyro_xb,'r-',alpha=0.8,label='Giro. X - MVA')
        plt.ylabel('Amplitud [º/s]')
        plt.xlabel('Muestra')
        plt.grid()
        plt.legend(loc='upper left')

        ax_5 = fig.add_subplot(335)
        line5, = ax_5.plot(x_vec,gyro_y,'g--',alpha=0.4,label='Giro. Y - Raw') 
        line5b, = ax_5.plot(x_vec,gyro_yb,'g-',alpha=0.8,label='Giro. Y - MVA') 
        plt.ylabel('Amplitud [º/s]')
        plt.xlabel('Muestra')
        plt.grid()
        plt.legend(loc='upper left')

        ax_6 = fig.add_subplot(336)
        line6, = ax_6.plot(x_vec,gyro_z,'b--',alpha=0.4,label='Giro. Z - Raw')      
        line6b, = ax_6.plot(x_vec,gyro_zb,'b-',alpha=0.8,label='Giro. Z - MVA')      
        plt.ylabel('Amplitud [º/s]')
        plt.xlabel('Muestra')
        plt.grid()
        plt.legend(loc='upper left')
        #----------------------------------------#
        # plot 3
        #----------------------------------------#
        ax_7 = fig.add_subplot(313)

        line7, = ax_7.plot(x_vec,temp,'g--',alpha=0.4,label='Temperatura - Raw')       
        line7b, = ax_7.plot(x_vec,tempb,'g-',alpha=0.8,label='Temperatura - MVA')       
        plt.ylabel('Amplitud [ºC]')
        plt.xlabel('Muestra')
        plt.grid()
        plt.legend(loc='upper left')

        plt.pause(1)
        # plt.show()
    
    # Se actualiza y_data
    line1.set_ydata(accel_x)
    line2.set_ydata(accel_y)
    line3.set_ydata(accel_z)
    line4.set_ydata(gyro_x)
    line5.set_ydata(gyro_y)
    line6.set_ydata(gyro_z)
    line7.set_ydata(temp)

    line1b.set_ydata(accel_xb)
    line2b.set_ydata(accel_yb)
    line3b.set_ydata(accel_zb)
    line4b.set_ydata(gyro_xb)
    line5b.set_ydata(gyro_yb)
    line6b.set_ydata(gyro_zb)
    line7b.set_ydata(tempb)

    # Ajuste de limites
    if np.min(accel_x)<=line1.axes.get_ylim()[0] or np.max(accel_x)>=line1.axes.get_ylim()[1]:
        # plt.ylim([np.min(y1_data)-np.std(y1_data),np.max(y1_data)+np.std(y1_data)])
        ax_1.set_ylim([np.min(accel_x)-np.std(accel_x),np.max(accel_x)+np.std(accel_x)])
        
    if np.min(accel_y)<=line2.axes.get_ylim()[0] or np.max(accel_y)>=line2.axes.get_ylim()[1]:
        ax_2.set_ylim([np.min(accel_y)-np.std(accel_y),np.max(accel_y)+np.std(accel_y)])

    if np.min(accel_z)<=line3.axes.get_ylim()[0] or np.max(accel_z)>=line3.axes.get_ylim()[1]:
        ax_3.set_ylim([np.min(accel_z)-np.std(accel_z),np.max(accel_z)+np.std(accel_z)])

    if np.min(gyro_x)<=line4.axes.get_ylim()[0] or np.max(gyro_x)>=line4.axes.get_ylim()[1]:
        ax_4.set_ylim([np.min(gyro_x)-np.std(gyro_x),np.max(gyro_x)+np.std(gyro_x)])

    if np.min(gyro_y)<=line5.axes.get_ylim()[0] or np.max(gyro_y)>=line5.axes.get_ylim()[1]:
        ax_5.set_ylim([np.min(gyro_y)-np.std(gyro_y),np.max(gyro_y)+np.std(gyro_y)])

    if np.min(gyro_z)<=line6.axes.get_ylim()[0] or np.max(gyro_z)>=line6.axes.get_ylim()[1]:
        ax_6.set_ylim([np.min(gyro_z)-np.std(gyro_z),np.max(gyro_z)+np.std(gyro_z)])

    if np.min(temp)<=line7.axes.get_ylim()[0] or np.max(temp)>=line7.axes.get_ylim()[1]:
        ax_7.set_ylim([np.min(temp)-np.std(temp),np.max(temp)+np.std(temp)])



    if np.min(accel_xb)<=line1b.axes.get_ylim()[0] or np.max(accel_xb)>=line1b.axes.get_ylim()[1]:
        # plt.ylim([np.min(y1_datab)-np.std(y1_data),np.max(y1_data)+np.std(y1_data)])
        ax_1.set_ylim([np.min(accel_xb)-np.std(accel_xb),np.max(accel_xb)+np.std(accel_xb)])
        
    if np.min(accel_yb)<=line2b.axes.get_ylim()[0] or np.max(accel_yb)>=line2b.axes.get_ylim()[1]:
        ax_2.set_ylim([np.min(accel_yb)-np.std(accel_yb),np.max(accel_yb)+np.std(accel_yb)])

    if np.min(accel_zb)<=line3b.axes.get_ylim()[0] or np.max(accel_zb)>=line3b.axes.get_ylim()[1]:
        ax_3.set_ylim([np.min(accel_zb)-np.std(accel_zb),np.max(accel_zb)+np.std(accel_zb)])

    if np.min(gyro_xb)<=line4b.axes.get_ylim()[0] or np.max(gyro_xb)>=line4b.axes.get_ylim()[1]:
        ax_4.set_ylim([np.min(gyro_xb)-np.std(gyro_xb),np.max(gyro_xb)+np.std(gyro_xb)])

    if np.min(gyro_yb)<=line5b.axes.get_ylim()[0] or np.max(gyro_yb)>=line5b.axes.get_ylim()[1]:
        ax_5.set_ylim([np.min(gyro_yb)-np.std(gyro_yb),np.max(gyro_yb)+np.std(gyro_yb)])

    if np.min(gyro_zb)<=line6b.axes.get_ylim()[0] or np.max(gyro_zb)>=line6b.axes.get_ylim()[1]:
        ax_6.set_ylim([np.min(gyro_zb)-np.std(gyro_zb),np.max(gyro_zb)+np.std(gyro_zb)])

    if np.min(tempb)<=line7b.axes.get_ylim()[0] or np.max(tempb)>=line7b.axes.get_ylim()[1]:
        ax_7.set_ylim([np.min(tempb)-np.std(tempb),np.max(tempb)+np.std(tempb)])

    plt.pause(pause_time)


    # Vuelven los objetos para poder actualizarlos después
    return line1,line2,line3,line4,line5,line6,line7,line1b,line2b,line3b,line4b,line5b,line6b,line7b

try: 
    parser= argparse.ArgumentParser()
    parser.add_argument("server_ip",help="Define server IP")
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
    client_socket_tcp.connect((args.server_ip,int(args.server_port)))
    datos = data_t()
    
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

        if data != 'OK':
            print('[ Cliente ]-$ Hubo algún error al establecer la comunicación UDP')
            client_socket_tcp.close()
        else:
            
            time.sleep(2)

            print('[ Cliente ]-$ Comenzando recepción de datos y lanzando graficador ...')

            size = 300
            x_vec = np.linspace(0,1,size+1)[0:-1]
            accel_x = np.zeros(len(x_vec))
            accel_y = np.zeros(len(x_vec))
            accel_z = np.zeros(len(x_vec))
            accel_xb = np.zeros(len(x_vec))
            accel_yb = np.zeros(len(x_vec))
            accel_zb = np.zeros(len(x_vec))
            gyro_x = np.zeros(len(x_vec))
            gyro_y = np.zeros(len(x_vec))
            gyro_z = np.zeros(len(x_vec))
            gyro_xb = np.zeros(len(x_vec))
            gyro_yb = np.zeros(len(x_vec))
            gyro_zb = np.zeros(len(x_vec))
            temp = np.zeros(len(x_vec))
            tempb = np.zeros(len(x_vec))
            
            line1 = []
            line2 = []
            line3 = []
            line4 = []
            line5 = []
            line6 = []
            line7 = []
            
            line1b = []
            line2b = []
            line3b = []
            line4b = []
            line5b = []
            line6b = []
            line7b = []

            while (plot_close_event == 0):
                # Envío KA (Keep Alive) al server
                client_socket_tcp.send(b'KA')

                client_socket_tcp.recv_into(datos, sizeof(datos))
                data_sensor = {
                    'rax':  datos.rax,
                    'ray':  datos.ray,
                    'raz':  datos.raz,
                    'rtmp': datos.rtmp,
                    'rgx':  datos.rgx,
                    'rgy':  datos.rgy,
                    'rgz':  datos.rgz,
                    'fax':  datos.fax,
                    'fay':  datos.fay,
                    'faz':  datos.faz,
                    'ftmp': datos.ftmp,
                    'fgx':  datos.fgx,
                    'fgy':  datos.fgy,
                    'fgz':  datos.fgz
                }

                
                accel_x[-1] = float(data_sensor['rax'])
                accel_y[-1] = float(data_sensor['ray'])
                accel_z[-1] = float(data_sensor['raz'])
                temp[-1] = float(data_sensor['rtmp'])
                gyro_x[-1] = float(data_sensor['rgx'])
                gyro_y[-1] = float(data_sensor['rgy'])
                gyro_z[-1] = float(data_sensor['rgz'])

                accel_xb[-1] = float(data_sensor['fax'])
                accel_yb[-1] = float(data_sensor['fay'])
                accel_zb[-1] = float(data_sensor['faz'])
                tempb[-1] = float(data_sensor['ftmp'])
                gyro_xb[-1] = float(data_sensor['fgx'])
                gyro_yb[-1] = float(data_sensor['fgy'])
                gyro_zb[-1] = float(data_sensor['fgz'])

                line1,line2,line3,line4,line5,line6,line7,line1b,line2b,line3b,line4b,line5b,line6b,line7b = real_time_plot(x_vec,accel_x,accel_y,accel_z,gyro_x,gyro_y,gyro_z,temp,
                                                                                                                            accel_xb,accel_yb,accel_zb,gyro_xb,gyro_yb,gyro_zb,tempb,
                                                                                                                            line1,line2,line3,line4,line5,line6,line7,
                                                                                                                            line1b,line2b,line3b,line4b,line5b,line6b,line7b,
                                                                                                                            0.05)

                accel_x = np.append(accel_x[1:],0.0)
                accel_y = np.append(accel_y[1:],0.0)
                accel_z = np.append(accel_z[1:],0.0)
                gyro_x = np.append(gyro_x[1:],0.0)
                gyro_y = np.append(gyro_y[1:],0.0)
                gyro_z = np.append(gyro_z[1:],0.0)
                temp = np.append(temp[1:],0.0)    
                accel_xb = np.append(accel_xb[1:],0.0)
                accel_yb = np.append(accel_yb[1:],0.0)
                accel_zb = np.append(accel_zb[1:],0.0)
                gyro_xb = np.append(gyro_xb[1:],0.0)
                gyro_yb = np.append(gyro_yb[1:],0.0)
                gyro_zb = np.append(gyro_zb[1:],0.0)
                tempb = np.append(tempb[1:],0.0)
                
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
