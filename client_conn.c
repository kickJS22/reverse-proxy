#include "client_conn.h"

void manejar_conexiones_clientes(int socket_cliente_fd, char *backend_host, char *backend_port_str) {
    
    struct addrinfo hstAndService; // Voy a agarrar hostname y servicio
    struct addrinfo *addrs;
    struct addrinfo *addrs_iter;

    int getaddrinfo_error;

    int backend_socket_fd;

    char buffer[BUFFER_SIZE];
    int bytes_read;
    
    // Asigno espacio en memoria estatica, le paso la direccion de mem de mi variable, el valor que se va a copiar a cada byte (0) y el tamanio de mi variable
    memset(&hstAndService, 0, sizeof(struct addrinfo));

    hstAndService.ai_family = AF_UNSPEC; // Le digo que acepte IPv4 e IPv6
    hstAndService.ai_socktype = SOCK_STREAM; // Le digo que soporte la utilizacion de sockets

    // Funcion para probar la conexion hacia el servidor de destino, lo que hago es almacenar una lista de los posibles nodos de ese servidor en addrs
    getaddrinfo_error = getaddrinfo(backend_host,backend_port_str, &hstAndService, &addrs);
    if ( getaddrinfo_error != 0){
        fprintf(stderr, "No se pudo encontrar el backend: %s\n", gai_strerror(getaddrinfo_error));
        exit(1);
    }

    // En la variable addrs es un puntero a todas las posibles direcciones a las que nos podemos conectar
    // Todas tienen asocionadas una ip y servicio. Loopeamos entre ellas para descubrir a cual nos podemos conectar
    for (addrs_iter = addrs; addrs_iter != NULL; addrs_iter = addrs_iter->ai_next){

        // Por cada una tratamos de crear un socket, si falla pasamos a la siguiente

        backend_socket_fd = socket (addrs_iter->ai_family,addrs_iter->ai_socktype,addrs_iter->ai_protocol);

        if(backend_socket_fd == -1) {
            continue;
        }

        // Si puedo conectarme, salgo del loop
        if (connect(backend_socket_fd, addrs_iter->ai_addr, addrs_iter->ai_addrlen) != -1) {
            break;
        }

        // Si no cierro la conexion
        close(backend_socket_fd);

    }

    // Chequeamos si se encontraron nodos
    if (addrs_iter == NULL) {
        fprintf(stderr,"No se encontraron nodos disponibles para el backend");
        exit(1);
    }

    freeaddrinfo(addrs);

    // Leemos los datos del cliente y los escribimos en el backend, una lectura es suficiente para traernos todos los headers
    bytes_read = read(socket_cliente_fd, buffer, BUFFER_SIZE);
    write(backend_socket_fd, buffer, bytes_read);

    // Leemos todo lo que viene del backend y lo escribimos en el cliente
    // while (bytes_read = read(backend_socket_fd, buffer, BUFFER_SIZE)) {
    //     write(socket_cliente_fd, buffer, bytes_read);
    // }
    bytes_read = read(backend_socket_fd,buffer,BUFFER_SIZE);
    // En el ultimo byte le digo que corte el string
    buffer[bytes_read] = '\0';

    char respuesta[BUFFER_SIZE + 1000];

    snprintf(respuesta, sizeof(respuesta),
    "HTTP/1.1 200 OK\r\n"
    "Server: Juani-proxy\r\n"
    "X-Forwarded-For: "
    "%s",
    buffer);

    write(socket_cliente_fd, respuesta, strlen(respuesta));

}