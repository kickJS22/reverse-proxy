struct addrinfo hints, *res;

int sockfd;

// primero llenamos addinfo struct con la funcion getarrinfo

memset(&hints, 0, sizeof hints)
hints.ai_family = AF_UNSPEC; // No se si voy a usar ipv4 o ipv6
hints.ai_socktype = SOCK_STREAM;

getaddrinfo("www.google.com", "443", &hints, &res);

// creamos un socket y obtenemos el file decriptor

sockfd = socket(res->ai_family, res->ai_socketype, res->ai_protocol);

// Nos conectamos

connect(sockfd, res->ai_addr, res->ai_addrlen);


// No nos importa bind en este caso pq no tenemos que escuchar ninguna request entrante
// No tenemos que ubicarlo localmente, aca nos conectamos a un host fuera 
