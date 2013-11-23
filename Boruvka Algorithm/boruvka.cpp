//A la fin de l'algo, on a un ensemble d'arêtes formant l'arbre couvrant minimal.
//Edmond La Chance UQAC 2010
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

///Timer stuff
#include "PerfTimer.h"
CPerfTimer t;

//Je dois ajouter le temps de qsort quand je compare avec BOOST pour que ça soit fair
//Preuve qu'on peut utiliser une class comme une struct si on veut
class arete
{
public:
	int sommet1;
	int sommet2;
	int poids;
	int choisie; //combien de fois elle est choisie

};
class sommet
{
public:
	arete * bestArete;
	int bestPoids; //optionel, on pourrait passer par le pointeur de l'arete
	int noGroupe; //initial, le groupe va changer avec le temps
};

class groupe
{
public:
	int noGroupe;
	arete * bestArete;
	int bestPoids; //optionel, on pourrait passer par le pointeur de l'arete
};

int main(int argc, char *argv[])
{ 

	printf("BORUVKA ALGORITHM - Brute force Disjoint Sets\n");

	//Lire le fichier texte
	FILE* fichier;

	//user used commandline to pass file name
	if (argc == 2) {
		fichier = fopen(argv[1], "r");
	}

	//no file name, search for graph_poids.txt
	else 
		fichier = fopen("graph_poids.txt", "r");

    static char ligne[50]; //pour lire la ligne du fichier
	bool premiereLignefaite = false; //on traite la premiere ligne du fichier differemment
	int N; //nbr de sommets
	int nb_aretes;
	arete * vecteurAretes; //tableau qui contient tous les objets aretes
	sommet * sommets; //tableau des sommets
	groupe * lesGroupes = NULL; //les groupes
	//Compteurs en tout genre
	int noGroupeCounter = 1; //Les no de groupes commencent au chiffre 1, le chiffre 0 est utilisé pour dire qu'il n'y a pas de représentant
	int i  = 0;
	int j = 0; //Compteur de groupes
	int nbr_groupes = 0; //nombre de groupes

	//Lire le fichier et mettre toutes les aretes dans le tableau
    while (fgets(ligne, 50, fichier) != NULL) //retourne 0 quand on a end-of-file
    {
        //La premiere ligne est différente
        if (premiereLignefaite == false) {
            //Initialiser une matrice d'adjacence NxN
            sscanf(ligne, "%d %d", &N, &nb_aretes );
			sommets = new sommet[N];
			memset(sommets, 0, N * sizeof(sommet));

			//Petit vecteur d'arêtes qui sera trié
			vecteurAretes = new arete[nb_aretes];
            premiereLignefaite = true;

			lesGroupes = new groupe[N+1];
			//Mettre les numéros de groupe
			//Quand le numéro != l'indice ça veut dire que c'est un sous-groupe
			for (int i = 1; i < N+1; i++) {
				lesGroupes[i].noGroupe = i;
				lesGroupes[i].bestPoids = 0;
				lesGroupes[i].bestArete = NULL;
			}

            continue;

        }
        //On construit notre liste d'arêtes
		int sommet1, sommet2;
		int poids;
        sscanf(ligne, "%d %d %d", &sommet1, &sommet2, &poids);
		vecteurAretes[i].poids = poids;
		vecteurAretes[i].sommet1 = sommet1; //faire -1 pour les tests bs
		vecteurAretes[i].sommet2 = sommet2;
		vecteurAretes[i].choisie = 0;
		i++;
    }

t.Start();

//1ere étape, on trouve les meilleures aretes pour chaque sommet. Ça va faire des ensembles disjoints
for (int i=0; i<nb_aretes; i++) //pour toutes les aretes du vecteur
{

	//Aller chercher le sommet1 et le sommet2 pour cette arête.
	arete * courante = &vecteurAretes[i];
	int s1 = courante->sommet1;
	int s2 = courante->sommet2;

	//Étape 1, on passe au travers de toutes les arêtes pour former les ensembles disjoints avec les meilleures arêtes

	//sommet non visité, on prend celle la comme meilleure (Peut etre que ce cas peut etre évité?
	if ( sommets[s1].bestPoids == 0) {
		sommets[s1].bestPoids = courante->poids;
		sommets[s1].bestArete = courante;
		courante->choisie++;
	}

	//Si l'arete est meilleure que la meilleure que le sommet connait, on change
	else if (sommets[s1].bestPoids > courante->poids) {
		sommets[s1].bestPoids = courante->poids;
		sommets[s1].bestArete->choisie--;
		sommets[s1].bestArete = courante;
		courante->choisie++;
	}

	//Meme chose pour le sommet2 touché par l'arete
	if ( sommets[s2].bestPoids == 0) {
	sommets[s2].bestPoids = courante->poids;
	sommets[s2].bestArete = courante;
	courante->choisie++;
	}

	//Same pour sommet2
	else if (sommets[s2].bestPoids > courante->poids) {
		sommets[s2].bestPoids = courante->poids;
		sommets[s2].bestArete->choisie--;
		sommets[s2].bestArete = courante;
		courante->choisie++;
	}

}

//2ème étape : Faire les groupes de sommets initiaux
//On passe à travers tous les sommets. Tous les autres sommets qu'on peut visiter sont dans le même ensemble disjoint
for (int i = 0; i < N ; i++)
{
	sommet * courant = &sommets[i];

	int g1 = sommets[courant->bestArete->sommet1].noGroupe;
	int g2 = sommets[courant->bestArete->sommet2].noGroupe;

	//Si l'arete qui part de ce sommet rejoint 2 sommets qui ne sont pas dans un groupe, on cree un groupe
	if ( g1 == 0 && g2 == 0)
	{
		sommets[courant->bestArete->sommet1].noGroupe = noGroupeCounter;
		sommets[courant->bestArete->sommet2].noGroupe = noGroupeCounter;
		noGroupeCounter++;
		nbr_groupes++;
	}

	//Si l'arete rejoint un groupe connu, alors les 2 sommets prennent ce groupe
	else if ( g1 != 0 && g2 == 0 )
	{
		sommets[courant->bestArete->sommet2].noGroupe = sommets[courant->bestArete->sommet1].noGroupe;
	}

	//Si l'arete rejoint un groupe connu, alors les 2 sommets prennent ce groupe
	else if ( g1 == 0 && g2 != 0 )
	{
		sommets[courant->bestArete->sommet1].noGroupe = sommets[courant->bestArete->sommet2].noGroupe;
	}

	//On doit fusionner les groupes
	else if ( g1 != g2)
	{
		int petit,grand;
		if (g1 < g2 ) {
			petit = g1;
			grand = g2;
		}
		else {
			petit = g2;
			grand = g1;
		}
		for (int j=0; j<N; j++) {
			if (sommets[j].noGroupe == grand )
				sommets[j].noGroupe = petit;
		}
		
		nbr_groupes--;

	}	

	//Autre cas pas besoin d'etre traité : les 2 sommets sont dans le même groupe.
}



for (int i=0; i<nb_aretes; i++) //pour toutes les aretes du vecteur
{
	arete * courante = &vecteurAretes[i];
	if (courante->choisie < 1) 
	{
		//Cette arête concerne quels groupes ?
		int g1 = sommets[ courante->sommet1 ].noGroupe;
		int g2 = sommets[ courante->sommet2 ].noGroupe;

		//Si c'est une arete du meme groupe (crée un cycle), on continue
		if (g1 == g2) 
			continue;


		//Si le groupe n'avait pas de meilleure arete connecteur, on choisit celle ci comme la meilleure
		if ( lesGroupes[g1].bestPoids == 0) {

			lesGroupes[g1].bestPoids = courante->poids;
			lesGroupes[g1].bestArete = courante;
			courante->choisie++;
			
		}

		//Si l'arete est meilleure, on change
		else if ( lesGroupes[g1].bestPoids > courante->poids)
		{
			lesGroupes[g1].bestPoids = courante->poids;
			lesGroupes[g1].bestArete->choisie--;
			lesGroupes[g1].bestArete = courante;
			courante->choisie++;
		}

		//Si le groupe n'avait pas de meilleure arete connecteur, on choisit celle ci comme la meilleure
		if ( lesGroupes[g2].bestPoids == 0) {

			lesGroupes[g2].bestPoids = courante->poids;
			lesGroupes[g2].bestArete = courante;
			courante->choisie++;
		}

		//Si l'arete est meilleure, on change
		else if ( lesGroupes[g2].bestPoids > courante->poids)
		{
			lesGroupes[g2].bestPoids = courante->poids;
			lesGroupes[g2].bestArete->choisie--;
			lesGroupes[g2].bestArete = courante;
			courante->choisie++;
		}

	}
}

while (nbr_groupes != 1)
{
	//Faire les nouveaux groupes
	for (int i = 1; i < noGroupeCounter ; i++)
	{
		groupe * courant = &lesGroupes[i];
		arete * meilleureAreteConnectantLeGroupe = courant->bestArete;

		if (meilleureAreteConnectantLeGroupe == NULL) 
			continue;

		//On va chercher les groupes initials que cette arete connecte
		int g1 =  sommets[meilleureAreteConnectantLeGroupe->sommet1].noGroupe;
		int g2 =  sommets[meilleureAreteConnectantLeGroupe->sommet2].noGroupe;

		//On va chercher les vrais groupes
		int groupe1 = lesGroupes[g1].noGroupe;
		int groupe2 = lesGroupes[g2].noGroupe;

		//Si c'est une arete qui connecte dans le même groupe on passe, le groupe est déja connecté
		if (groupe1 == groupe2) 
			continue;
		
		int petit, grand;
		if (groupe1 < groupe2)
		{
			petit = groupe1;
			grand = groupe2;
		}
		else
		{
			petit = groupe2;
			grand = groupe1;
		}

		//Update les groupes dans la table (optimiser cette partie). Ici je remplace le petit groupe par le grand
		for (int k = 1; k < noGroupeCounter; k++) {
			

			if (lesGroupes[k].noGroupe == grand)
				lesGroupes[k].noGroupe = petit;
		}

		//Diminuer le nombre de groupes
		nbr_groupes--;

		//Mettre le poids et la best Arete du groupe a 0 pour le next round
		courant->bestPoids = 0;
	}

	//Fin du problème
	if (nbr_groupes == 1) 
		break;

	//Pour tous les groupes restants, on trouve la meilleure arete qui connecte chaque groupe
	for (int i = 0; i < nb_aretes ; i++)
	{
		
		arete * courante = &vecteurAretes[i];
		if (courante->choisie > 0) continue;

		//Cette arête concerne quels groupes ?
		int g1 = sommets[ courante->sommet1 ].noGroupe;
		int g2 = sommets[ courante->sommet2 ].noGroupe;

		//Aller chercher les vrais groupes
		int groupe1 = lesGroupes[g1].noGroupe;
		int groupe2 = lesGroupes[g2].noGroupe;

		//Ça ne sert à rien d'utiliser cette arête
		if (groupe1 == groupe2)
		{
			continue;
		}

		//Si le groupe n'avait pas de meilleure arete connecteur, on choisit celle ci comme la meilleure
		if ( lesGroupes[groupe1].bestPoids == 0) {

			lesGroupes[groupe1].bestPoids = courante->poids;
			lesGroupes[groupe1].bestArete = courante;
			courante->choisie++;
		}

		//Si l'arete est meilleure, on change
		else if ( lesGroupes[groupe1].bestPoids > courante->poids)
		{
			lesGroupes[groupe1].bestPoids = courante->poids;
			lesGroupes[groupe1].bestArete->choisie--;
			lesGroupes[groupe1].bestArete = courante;
			courante->choisie++;
		}

		//Si le groupe n'avait pas de meilleure arete connecteur, on choisit celle ci comme la meilleure
		if ( lesGroupes[groupe2].bestPoids == 0) {

			lesGroupes[groupe2].bestPoids = courante->poids;
			lesGroupes[groupe2].bestArete = courante;
			courante->choisie++;
		}

		//Si l'arete est meilleure, on change
		else if ( lesGroupes[groupe2].bestPoids > courante->poids)
		{
			lesGroupes[groupe2].bestPoids = courante->poids;
			lesGroupes[groupe2].bestArete->choisie--;
			lesGroupes[groupe2].bestArete = courante;
			courante->choisie++;
		}

	}
	j = 0;
} //fin gros while !


printf("Time (ms) *%f*\n",t.Elapsedms());

//Imprimer les aretes choisies pour rire
int poidsTotal = 0;

for (int i = 0 ,j = 1; i < nb_aretes; i++)
{
	arete * courante = &vecteurAretes[i];
	if (courante->choisie > 0) {
		//printf("%d Arete#%d %d %d %d\n",j,i+1, courante->sommet1, courante->sommet2, courante->poids);
		j++;
		poidsTotal += courante->poids;
	}

}
printf("\nPoids total ou genetique : %d", poidsTotal);
printf("\nTaille du probleme : %d sommets et %d aretes\n", N, nb_aretes);


return 0;



}