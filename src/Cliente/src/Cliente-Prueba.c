/*
 ============================================================================
 Name        : Cliente-Prueba.c
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PUERTO "3250"
#define MAXTAMDATOS 100

int main(int argc, char *argv[]) {

	int sockfd, numeroDeBytes;
	char buffer[MAXTAMDATOS];
	struct addrinfo hints, *infoServer, *inicio;
	int estado;
	int estadoConexion;
	char s[INET6_ADDRSTRLEN];
	char ip[15] = "127.0.1.1";

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((estado = getaddrinfo(NULL, PUERTO, &hints, &infoServer)) != 0) {

		fprintf(stderr, "Error Al Conseguir La Info De La Direccion: %s\n",
				gai_strerror(estado));
		return 1;
	}

	for (inicio = infoServer; inicio != NULL; inicio = inicio->ai_next) {



		if ((sockfd = socket(inicio->ai_family, inicio->ai_socktype,
				inicio->ai_protocol)) == -1) {

			perror("Cliente - Socket");
			continue;
		}

		if ((estadoConexion = connect(sockfd, inicio->ai_addr, inicio->ai_addrlen)) == -1) {

			close(sockfd);
			perror("Cliente - Conexion");
			continue;
		}

		break;
	}

	if (inicio == NULL) {

		fprintf(stderr, "Fallo El Cliente Al Conectar\n");
		return 2;

	}

	inet_ntop(inicio->ai_family,&(((struct sockaddr_in *) &inicio)->sin_addr), s, sizeof s);

	printf("Cliente Conectandose Con El Ip Del Servidor: %s\n", s);

	freeaddrinfo(infoServer);

	if ((numeroDeBytes = recv(sockfd, buffer, MAXTAMDATOS-1, 0)) == -1) {

		perror("Error Al Recibir El Handshake");
		exit(1);
	}

	buffer[numeroDeBytes] = '\0';

	printf("El Cliente Recibio El Mensaje: %s\n", buffer);

	close(sockfd);

	return 0;
}


