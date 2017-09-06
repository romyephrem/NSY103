/******* SERVEUR TCP **************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <netdb.h>
#include <stdbool.h>
#include <pthread.h>
#define PORTS "2058"

int vitx, vity, vitz, cap, alt;

void EntrezCoordonne(int vitx, int vity, int vitz, int cap, int alt, char *nomAvion)
{
	printf("Entrez Vx: (%s)", nomAvion);
	scanf("%d", &vitx);
	printf("Entrez Vy: (%s)", nomAvion);
	scanf("%d", &vity);
	printf("Entrez Vz: (%s)", nomAvion);
	scanf("%d", &vitz);
	printf("Entrez le cap: (%s)", nomAvion);
	scanf("%d", &cap);
	printf("Entrez l'altitude: (%s)", nomAvion);
	scanf("%d", &alt);
}

char *RenouvelerCoordonnees(int vitx, int vity, int vitz, int cap, int alt)
{
	static char *str;
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

char *info[11] = {"id","nom","destination","posX","posY","posiZ","cap","vitX","vitY","vitZ","altitude","situation"};
char *listavion[1][11];
int k = 0;

bool Collision(char *listavion[1][11])
{
	for(int i=0;i<2;i++)
    {
		for(int j=i+1;j<2;j++)
	    {
			if (listavion[i][3] == listavion[j][3] && listavion[i][4] == listavion[j][4]
	            && listavion[i][5] == listavion[j][5] && listavion[i][10] == listavion[j][10])
	            return true;
	    }
	}
	return false;
}

void AffichageRadar(char * id, char * nom, char * destination, char * posX, char * posY, char * posZ, char * vitX, char * vitY, char * vitZ, char * cap, char * alt, char * text)
{
	bool avionPresent = false;
	for(int i=0;i<2;i++)
	{
		for(int j=0;j<12;j++)
		{
			if(listavion[i][j] == id && k < 2)
			{
				listavion[i][0] = id;
				listavion[i][1] = nom;
				listavion[i][2] = destination;
				listavion[i][3] = posX;
				listavion[i][4] = posY;
				listavion[i][5] = posZ;
				listavion[i][6] = vitX;
				listavion[i][7] = vitY;
				listavion[i][8] = vitZ;
				listavion[i][9] = cap;
				listavion[i][10] = alt;
				listavion[i][11] = text;

				avionPresent=true;
				break;
			}
		}
	}

	if (!avionPresent)
	{
		listavion[k][0] = id;
		listavion[k][1] = nom;
		listavion[k][2] = destination;
		listavion[k][3] = posX;
		listavion[k][4] = posY;
		listavion[k][5] = posZ;
		listavion[k][6] = vitX;
		listavion[k][7] = vitY;
		listavion[k][8] = vitZ;
		listavion[k][9] = cap;
		listavion[k][10] = alt;
		listavion[k][11] = text;
	}
	k++;

	printf("---------------------");
        for(int i=0;i<2;i++)
        {
            for(int j=0;j<12;j++)
            {
                printf("%s : %s",info[j], listavion[i][j]);
            }
            printf("---------------------");
        }
        if(Collision(listavion))
            printf("Collision détectée\n");
}



void *CreationThread(void *socket_desc)
{
	int sock = *(int*)socket_desc;
	char *message , avion_message[2000], *s;

	message = read(sock, avion_message,	2000, 0);
	char **token = str_split(message, "-");
	AffichageRadar(token[0], token[1], token[2], "0", "0", "0", "0", "0", "0", "0", "0", "Depart");

	EntrezCoordonne(&vitx, &vity, &vitz, &cap, &alt, token[1]);
	s = RenouvelerCoordonnees(vitx, vity, vitz, cap, alt);

	while(true)
	{
		write(sock, s, 2000);

		char *position = read(sock, avion_message, 2000, 0);
		char **pos = str_split(position, "-");
		char **vit = str_split(s, "-");
		if ((sizeof(pos) >= 4) && (pos[3] == "crash"))
		{
			AffichageRadar(token[0], token[1], token[2], pos[0], pos[1],pos[2], vit[0], vit[1], vit[2], vit[3], vit[4],"Crash de l'avion");
			close(sock);
		}
		else if ((sizeof(pos) >= 4) && (pos[3] == "fin"))
		{
			AffichageRadar(token[0], token[1], token[2], pos[0], pos[1],pos[2], vit[0], vit[1], vit[2], vit[3], vit[4],"Arrivé à destination");
			close(sock);
		}
		else if ((sizeof(pos) >= 4) && (pos[3] == "vitesse"))
		{
			AffichageRadar(token[0], token[1], token[2], "0", "0", "0", "0", "0", "0", "0", "0","Vitesse incorrect(très petite/grande)");
			close(sock);
		}
		else
		{
			AffichageRadar(token[0], token[1], token[2], pos[0], pos[1],pos[2], vit[0], vit[1], vit[2], vit[3], vit[4],"Dans l'air");

			EntrezCoordonne(&vitx, &vity, &vitz, &cap, &alt, token[1]);
			s = RenouvelerCoordonnees(vitx, vity, vitz, cap, alt);
		}
	}
}

main()
{
	int sock, i, sin_size, client_sock;
	struct sockaddr_in my_addr;
	sock = socket(AF_INET, SOCK_STREAM, 0);
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(PORTS);
    my_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    bind(sock, (struct sockaddr*)&my_addr, sizeof(struct sockaddr));
    listen(sock,256);
	bool b = true;
	pthread_t thread_id;
	sin_size = sizeof(my_addr);

	while(b)
	{
		i = accept(sock, &my_addr, &sin_size);
		pthread_create( &thread_id , NULL , CreationThread , &client_sock);
	}
}
