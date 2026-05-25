#include "client_conn.h"
#include <arpa/inet.h>

int main(int argc, char *argv[]){

    char *server_port_str;
    char *backend_addr;
    char *backend_port_str;

    struct addrinfo hstAndServ;
    struct addrinfo *addrs;
    struct addrinfo *addr_iter;
    int getaddrinfo_error;

    int server_socket_fd;
    int client_socket_fd;

    int so_reuseaddr;

    if (argc != 4) {
        fprintf(stderr,"Usage: %s <server_port> <backend_addr> <backend_port>\n",argv[0]);
        exit(1);
    }
    
    server_port_str = argv[1]; // Tomo el puerto del servidor proxy
    backend_addr = argv[2]; // tomo la addr del backend
    backend_port_str = argv[3]; // tomo el puerto del backend

    printf("Socket iniciado en: http://%s:%s\n", backend_addr,server_port_str);
    fflush(stdout);

    memset(&hstAndServ, 0, sizeof(struct addrinfo));
    hstAndServ.ai_family = AF_UNSPEC;
    hstAndServ.ai_socktype = SOCK_STREAM;
    hstAndServ.ai_flags = AI_PASSIVE;

    getaddrinfo_error = getaddrinfo(NULL, server_port_str, &hstAndServ, &addrs);

    for (addr_iter = addrs; addr_iter != NULL; addr_iter = addr_iter->ai_next) {
        server_socket_fd = socket(addr_iter->ai_family,
                                  addr_iter->ai_socktype,
                                  addr_iter->ai_protocol);
        if (server_socket_fd == -1) {
            continue;
        }

        so_reuseaddr = 1;
        setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &so_reuseaddr, sizeof(so_reuseaddr));

        if (bind(server_socket_fd,
                 addr_iter->ai_addr,
                 addr_iter->ai_addrlen) == 0)
        {
            break;
        }

        close(server_socket_fd);
    }

    if (addr_iter == NULL) {
        fprintf(stderr, "Couldn't bind\n");
        exit(1);
    }

    freeaddrinfo(addrs);

    listen(server_socket_fd, MAX_LISTEN_BACKLOG);
    

    while (1) {
        struct sockaddr_in addr; 
        socklen_t addr_len = sizeof(addr);
        client_socket_fd = accept(server_socket_fd, (struct sockaddr *)&addr, &addr_len);

        char addr_str[50];
        
        inet_ntop(AF_INET, &addr.sin_addr, addr_str, sizeof(addr_str));

        printf("%s\n", addr_str);

        if (client_socket_fd == -1) {
            perror("Could not accept");
            exit(1);
        }
        manejar_conexiones_clientes(client_socket_fd, backend_addr, backend_port_str);
        
    }
}