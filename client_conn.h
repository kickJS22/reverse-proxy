#ifndef CLIENT_CONN_H
#define CLIENT_CONN_H

#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>  /* para tipos de datos usados por el sistema y sockets */
#include <sys/socket.h> /* funciones para trabajar con sockets */
#include <netinet/in.h> /* estructuras y constantes para direcciones ips y puertos */
#include <netdb.h> /* para resolver nombres de hosts */
#include <unistd.h> /* Para manejar aperturas del socket*/

#define MAX_LISTEN_BACKLOG 1
#define BUFFER_SIZE 4096

// Defino un fd (file decriptor, usado por c para leer un file-like object, en este caso una conexion del socket)
void manejar_conexiones_clientes (int socket_cliente_fd, char *backend_host, char *backend_port_str);

#endif