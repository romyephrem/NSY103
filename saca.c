/******* SERVEUR TCP **************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <netdb.h>
#include <stdbool.h>
#define PORTS "2058"

void EntrezCoordonne(char *nomAvion)
{
	int v;
	printf("Entrez Vx: (%s)", nomAvion);
	scanf("%d", &v);
	printf("Entrez Vy: (%s)", nomAvion);
	scanf("%d", &v);
	printf("Entrez Vz: (%s)", nomAvion);
	scanf("%d", &v);
	printf("Entrez le cap: (%s)", nomAvion);
	scanf("%d", &v);
	printf("Entrez l'altitude: (%s)", nomAvion);
	scanf("%d", &v);
}

char *RenouvelerCoordonnees(int vitx, int vity, int vitz, int cap, int alt)
{
	static char str;
	strcpy(str, vitx);
	strcat(str, "-");
	strcat(str, vity);
	strcat(str, "-");
	strcat(str, vitz);
	strcat(str, "-");
	strcat(str, cap);
	strcat(str, "-");
	strcat(str, alt);
	return str;
}

main()
{
	char buf[100];
	ssize_t numbytes;
	int sockfd, new_fd, rv, sin_size;
	pid_t numpid;
	struct addrinfo hints, *svinfo;
	struct sockaddr their_adr;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP
	rv = getaddrinfo(NULL, PORTS, &hints, & svinfo);

	// Création socket et attachement
	sockfd = socket(svinfo->ai_family, svinfo->ai_socktype,svinfo->ai_protocol);
	bind(sockfd, svinfo ->ai_addr, svinfo ->ai_addrlen);
	listen(sockfd, 5);
	bool b = true;
	char *s;

	numbytes = recv(sockfd, s, 100, 0);
	printf("Message reçu : %s\n",s);
	EntrezCoordonne("Avion");

	while(b)
	{
		sin_size = sizeof(their_adr);
		new_fd = accept(sockfd, &their_adr, &sin_size);
		numpid= fork();
		if (numpid==0) {
		{ close(sockfd);
		write(new_fd, s, sizeof(s));
		close(new_fd); exit(0); }
		}
		else
		close (new_fd);
	}
}
