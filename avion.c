#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "avion.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <netdb.h>
#include <stdbool.h>
#define SERVEUR "127.0.0.1"
#define PORTS "2058"
#define PORTC "4096"


struct avion avion;
// numéro de vol de l'avion : code sur 5 caractéres
char numero_vol[6];

/********************************
 ***  3 fonctions à implémenter
 ********************************/

int ouvrir_communication() {
    // fonction à implémenter qui permet d'entrer en communication via TCP
    // avec le gestionnaire de vols
    return 1;
}

void fermer_communication() {
    // fonction à implémenter qui permet de fermer la communication
    // avec le gestionnaire de vols
}

void envoyer_caracteristiques() {
    // fonction à implémenter qui envoie l'ensemble des caractéristiques
    // courantes de l'avion au gestionnaire de vols
}

/********************************
 ***  Fonctions gérant le déplacement de l'avion : ne pas modifier
 ********************************/

// initialise aléatoirement les paramétres initiaux de l'avion

void initialiser_avion() {
    // initialisation al?atoire du compteur aléatoire
    int seed;
    time(&seed);
    srandom(seed);

    // intialisation des paramétres de l'avion
    avion.position.x = 1000 + random() % 1000;
    avion.position.y = 1000 + random() % 1000;
    avion.position.z = 1000 + random() % 1000;
    avion.position.altitude = 900 + random() % 100;

    avion.dep.cap = random() % 360;
    avion.dep.vitesse = 600 + random() % 200;

    // initialisation du numero de l'avion : chaine de 5 caract?res
    // formée de 2 lettres puis 3 chiffres
    numero_vol[0] = (random() % 26) + 'A';
    numero_vol[1] = (random() % 26) + 'A';
    sprintf(&numero_vol[2], "%03d", (random() % 999) + 1);
    numero_vol[5] = 0;
}

// modifie la valeur de l'avion avec la valeur pass?e en param?tre

void changer_vitesse(int vitesse) {
    if (vitesse < 0)
        avion.dep.vitesse = 0;
    else if (vitesse > VITMAX)
        avion.dep.vitesse = VITMAX;
    else avion.dep.vitesse = vitesse;
}

// modifie le cap de l'avion avec la valeur passée en paramètre

void changer_cap(int cap) {
    if ((cap >= 0) && (cap < 360))
        avion.dep.cap = cap;
}

// modifie l'altitude de l'avion avec la valeur passée en paramètre

void changer_altitude(int alt) {
    if (alt < 0)
        avion.position.altitude = 0;
    else if (alt > ALTMAX)
        avion.position.altitude = ALTMAX;
    else avion.position.altitude = alt;
}

// affiche les caractéristiques courantes de l'avion

void afficher_donnees() {
    printf("Avion %s -> localisation : (%d,%d,%d), altitude : %d, vitesse : %d, cap : %d\n",
            numero_vol, avion.position.x, avion.position.y, avion.position.z, avion.position.altitude, avion.dep.vitesse, avion.dep.cap);
}

// recalcule la localisation de l'avion en fonction de sa vitesse et de son cap

void calcul_deplacement() {
    float cosinus, sinus;
    float dep_x, dep_y;

    if (avion.dep.vitesse < VITMIN) {
        printf("Vitesse trop faible : crash de l'avion\n");
        fermer_communication();
        exit(2);
    }
    if (avion.position.altitude == 0) {
        printf("L'avion s'est ecrase au sol\n");
        fermer_communication();
        exit(3);
    }
    //cos et sin ont un paramétre en radian, dep.cap en degré nos habitudes francophone
    /* Angle en radian = pi * (angle en degré) / 180
       Angle en radian = pi * (angle en grade) / 200
       Angle en grade = 200 * (angle en degré) / 180
       Angle en grade = 200 * (angle en radian) / pi
       Angle en degré = 180 * (angle en radian) / pi
       Angle en degré = 180 * (angle en grade) / 200
     */

    cosinus = cos(avion.dep.cap * 2 * M_PI / 360);
    sinus = sin(avion.dep.cap * 2 * M_PI / 360);

    //newPOS = oldPOS + Vt
    dep_x = cosinus * avion.dep.vitesse * 10 / VITMIN;
    dep_y = sinus * avion.dep.vitesse * 10 / VITMIN;

    // on se d?place d'au moins une case quels que soient le cap et la vitesse
    // sauf si cap est un des angles droit
    if ((dep_x > 0) && (dep_x < 1)) dep_x = 1;
    if ((dep_x < 0) && (dep_x > -1)) dep_x = -1;

    if ((dep_y > 0) && (dep_y < 1)) dep_y = 1;
    if ((dep_y < 0) && (dep_y > -1)) dep_y = -1;

    //printf(" x : %f y : %f\n", dep_x, dep_y);

    avion.position.x = avion.position.x + (int) dep_x;
    avion.position.y = avion.position.y + (int) dep_y;

    afficher_donnees();
}

// fonction principale : gère l'exécution de l'avion au fil du temps

void se_deplacer() {
    while (1) {
        sleep(PAUSE);
        calcul_deplacement();
        envoyer_caracteristiques();
    }
}

int getPosX(){
	return avion.position.x;
}

int getPosY(){
	return avion.position.y;
}

int getPosZ(){
	return avion.position.z;
}

int getIdAv(){
	return avion.id;
}

char *getDestination(){
	static char str;
	strcpy(str, avion.destX);
	strcat(str, ";");
	strcat(str, avion.destY);
	strcat(str, ";");
	strcat(str, avion.destZ);
	return str;
}

char *getNomAv(){
	return avion.nom;
}

void setVitesse (int vx , int vy , int vz ){
	avion.vitesse.vitX=vx;
	avion.vitesse.vitY=vy;
	avion.vitesse.vitZ=vz;
	avion.dep.vitesse = (int)sqrt(vx*vx + vy*vy + vz*vz);
}

void setCap (int cap){
	avion.dep.cap = cap;
}

void setAltitude (int alt){
	avion.position.altitude = alt;
}

void setPosition (int vx, int vy, int vz){
	avion.position.x = avion.position.x + vx;
	avion.position.y = avion.position.y + vy;
	avion.position.z = avion.position.y + vz;
}

int changeVitesse(int v)
{
	if (avion.position.x == avion.destX && avion.position.y == avion.destY && avion.position.z == avion.destZ)
	        {
	            printf("arrivé à destination");
	            return -1;
	        }
	        else if (avion.position.altitude > avion.altituteMax)
	        {
	            printf("crash de l'avion");
	            return -2;
	        }
	        else if(avion.status == "on" && (avion.dep.vitesse + v) <= avion.vitMax && (avion.dep.vitesse + v) >= avion.vitMin) {
	               avion.dep.vitesse = avion.dep.vitesse + v ;
	               printf("l'avion est dans l'air");
	               return 0;
	            }
	        else if ( avion.status == "on" && (avion.dep.vitesse + v) <= avion.vitMin){
	                avion.dep.vitesse = 0 ;
	                avion.status = "off";
	                printf("crash de l'avion");
	                return -2;
	            }
	        else if ( avion.status == "on" && (avion.dep.vitesse + v) >= avion.vitMax){
	                avion.dep.vitesse = 0 ;
	                avion.status = "off";
	                printf("crash de l'avion");
	                return -2;
	            }
	        else if(avion.status == "off" && (avion.dep.vitesse + v) >= avion.vitMin && (avion.dep.vitesse + v) <= avion.vitMax){
	             //demarrage de l'avion avec une vitesse v
	                avion.dep.vitesse = avion.dep.vitesse + v ;
	                avion.status = "on";
	                printf("l'avion a demarré");
	                return 0;
	            }
	        else if (avion.status == "off" && (avion.dep.vitesse + v) >= avion.vitMax){
	            avion.dep.vitesse = 0;
	            printf("l'avion ne peut pas demarrer avec cette grande vitesse");
	            return -3;
	        }
	        else if (avion.status == "off" && (avion.dep.vitesse + v) <= avion.vitMin){
	            avion.dep.vitesse = 0;
	            printf("l'avion ne peut pas demarrer avec cette petite vitesse");
	            return -3;
	        }
	        else{
	            printf("la valeur entrer pour changer la vitesse est refusé");
	            return -3;
	        }
}

int ChangerCoordonnees(int vitX , int vitY, int vitZ, int cap , int alt){
	setVitesse(vitX, vitY, vitZ);
	setCap(cap);
	setAltitude(alt);
	setPosition(vitX, vitY, vitZ);
	int vitesse = (int)sqrt(vitX*vitX + vitY*vitY + vitZ*vitZ);

	return changeVitesse(vitesse);
}

main ()
{
	avion.altituteMax = 20000;
	avion.vitMax = 1000;
	avion.vitMin = 200;
	avion.destX = 1500;
	avion.destY = 2000;
	avion.destZ = 3000;
	avion.nom = "Avion2";
	avion.id = 2;

	int sockfd, new_fd, rv, sin_size, status;
	struct addrinfo hints, *svinfo, *res;
	ssize_t numbytes;
	char buf[100];
	// Construction adresse serveur pour connect//
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	rv = getaddrinfo(SERVEUR, PORTS, &hints,&svinfo);

	// Création socket et attachement
	memset(&hints, 0, sizeof (hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	status = getaddrinfo(NULL, PORTC, &hints, &res);
	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	bind (sockfd, res->ai_addr, res->ai_addrlen);
	connect(sockfd, svinfo ->ai_addr, svinfo ->ai_addrlen);
	bool b = true;

	write(sockfd, avion.position.x, sizeof(avion.position.x));

	while(b)
	{
		numbytes = recv(sockfd, buf, 100, 0);
		close(sockfd);
	}
}

