/*
 ============================================================================
 Name        : Servidor-Prueba.c
 ============================================================================
 */

//Defino Las Macros A Utilizar
#define MIPUERTO "3250"
#define BACKLOG 3

//Incluyo Las Librerias Para Sockets, Network
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

int main(void) {

	//Defino Dos Socket Descriptor, sockfd para utilizar para las conexion y new_fd para obtener el que realizo
	//La Conexion con el servidor, a cual voy a  usar para comunicarme
	int sockfd, new_fd;

	//Variables y Estructuras Para Obtener La Info Para Los Sockets
	struct sockaddr_storage la_direccion;
	socklen_t tamanio_direccion;
	struct addrinfo hints, *res, *inicio;
	int estado;
	int yes = 1;
	char s[INET6_ADDRSTRLEN];

	//Inicia Los Bloques en 0, para despues rellenar sus campos para el Socket
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	//Consigue La Direccion y Llena Los Campos y si Estado no es Igual A 0, Notifica El Error
	if ((estado = getaddrinfo(NULL, MIPUERTO, &hints, &res)) != 0){

		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(estado));
		return 1;
	}

	for(inicio = res; inicio != NULL; inicio = inicio->ai_next){

		void *addr;

	//Consigo el valor del Socket Descriptor y si es Igual a -1, Notifico El Error
	if((sockfd = socket(inicio->ai_family, inicio->ai_socktype, inicio->ai_protocol)) == -1){

		perror("Servidor - Socket");
		continue;
	}

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {

		perror("setsockopt");
		exit(1);
	}

	//Asigno Una Direccion Para El Socket
	if (bind(sockfd, inicio->ai_addr, inicio->ai_addrlen) == -1) {

	close(sockfd);
	perror("Servidor - Bindeo");
	continue;
	}

	break;

	}

	//Libero La Memoria De La Estructura Que Utilize Para Conseguir La Info
	freeaddrinfo(res);

	//Si No Encontro Ninguno, Se Produjo Un Fallo En La Asignacion De La Direccion Al Socket
	if (inicio == NULL) {

		fprintf(stderr, "Servidor: Fallo Al Bindear\n");
		exit(1);
	}

	//Utilizo Esta Funcion Para Habilitar Al Socket A Aceptar Conexiones
	if (listen(sockfd, BACKLOG) == -1) {

		perror("Error Al Escuchar");
		exit(1);
	}

	 printf("Servidor: Esperando Por Conexiones...\n");


	while(1){

	tamanio_direccion = sizeof la_direccion;
	//Acepto Las Conexiones Que Traten De Conectarse Al Puerto De Mi Servidor
	new_fd = accept(sockfd, (struct sockaddr *) &la_direccion, &tamanio_direccion);

	if (new_fd == -1) {

		perror("Error Al Aceptar Conexion");
		continue;

	}

	//Convierte Una Direccion de Internet Desde La Network De Forma Binaria A Su Forma En Texto Plano
	inet_ntop(la_direccion.ss_family,&(((struct sockaddr_in *) &la_direccion)->sin_addr), s, sizeof s);

	//Al Realizarse La Conexion Con Exito, Muestro Cual Fue La IP Que La Realizo
	printf("Servidor: Conexion Establecida Con El Cliente: %s\n", s);

	//Envio Un Mensaje Al Cliente Que Se Conecto Para Ver Que La Comunicacion Funciona Sin Problemas
	if (send(new_fd, "Realizando El Handshake", 23, 0) == -1) {

		perror("Error Al Enviar Mensaje");
		close(new_fd);
		exit(0);
	}

	//Cierro El File Descriptor y Lo Libero
	close(new_fd);
	}

	return 0;
}


