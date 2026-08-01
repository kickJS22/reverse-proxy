// Shows the ip address for a given host

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>


int main(int argc, char *argv[]) {
    // hints es una especie de filtro que uso para decirle que tipo de estructura quiero
    // en res se van a almacenar los resultados
    struct addrinfo hints, *res, *p;
    int status;
    char ipstr[INET6_ADDRSTRLEN];

    if (argc != 2){
        fprintf(stderr, "usage: showip hostname\n");
        return 1;
    }

    // llenamos un bloque de memoria con un valor 
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // Admito tanto ipv4 como ipv6
    hints.ai_socktype = SOCK_STREAM; // Tipo de socket stream (TCP)

    if ((status = getaddrinfo(argv[1], NULL, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 2;
    }

    printf("IP addresses for %s:\n\n", argv[1]);

    //res va a ser el resultado de la consulta de dominio
    // este va a ser una estructura addrinfo con la ip del dominio que haya obtenido
    // pero la estructura va a contener el siguiente resultado en ai_next, ya que res va a ser una lista linkeada por esa var
    // en cuanto me de NULL, osea no tengo mas resultados, paro el bucle

    for(p=res;p!=NULL; p = p->ai_next) {
        void *addr;
        char *ipver;

        struct sockaddr_in *ipv4;
        struct sockaddr_in6 *ipv6;

        //compruebo si es ipv4

        if (p->ai_family == AF_INET) {
            ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
            ipver = "IPv4";
        } else {
            ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipver = "IPv6";
        }

        // network to presentation, pasa de direccion ip a estructura
        inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
        printf(" %s: %s\n", ipver, ipstr);

        // libero memoria de la linked list
        freeaddrinfo(res);

        return 0;
    }
}