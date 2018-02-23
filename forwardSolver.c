#include "util.h"

typedef int MATRICE;

#define MAX_VAR	(100)
#define MAX_DOM	(100)
/*les variables de tailles*/

int _nb_variable;	/*le nombre de variable ajouter 1*/
int _nb_domaine;	/*la taille maximale de domaine */

struct{
	MATRICE relation[MAX_DOM][MAX_DOM];
}contrainte[MAX_VAR][MAX_DOM];

int domaine[MAX_VAR][MAX_DOM];

struct{
	int a, b;
}indice_contrainte[MAX_VAR];

int debut[MAX_VAR];

int nb_contrainte = 0;


void init_dame(int nb_var){
	int i, j, k, l;
	_nb_domaine = nb_var;
	_nb_variable = nb_var;

	for(i = 1; i<= _nb_variable; i++){
		debut[i] = 1;//init de la table de debut
		for(j= 1; j<=_nb_domaine; j++){
			domaine[i][j] = 1;
		}
	}
	int a,b, _a, _b;
	for(i = 1; i<= _nb_variable; i++){
		a = i;
		for(j=i+1; j<=_nb_variable; j++){
			_a = j;
			for( k = 1; k<=_nb_domaine ; k++){
				b = k;
				for(l=1; l<=_nb_domaine; l++){
					_b = l;
					if(a==_a || b == _b || abs(a-_a) == abs (b - _b)){
						contrainte[i][j].relation[k][l] = 0;
					}
					else{
						contrainte[i][j].relation[k][l] = 1;

						/*
						pour optimisation, on stocke les contrainte Ci,j dans le tableau indice
						*/
						nb_contrainte ++;
						indice_contrainte[nb_contrainte].a = i;
						indice_contrainte[nb_contrainte].b = j;
						
					}
				}
			}
		}
	}
}


void affiche_tout(){
	int i,j,l,m;
	printf("Domaine %x%d \n", _nb_variable,_nb_domaine);
	/*for(i=1; i<=_nb_variable; i++){
		for(j=1; j<=_nb_domaine; j++){
			printf("%d ",domaine[i][j]);
		}
		printf("\n");
	}
	printf("Contrainte \n");
	for(i=1; i<=_nb_variable; i++){
		for(j=i+1; j<=_nb_variable; j++){
			printf("Contrainte C[%d][%d] \n", i,j);
			for(l = 1; l<= _nb_domaine; l++){
				for(m=1; m<=_nb_domaine; m++){
					printf("%d ", contrainte[i][j].relation[l][m]);
				}
				printf("\n");
			}
		}
	}*/
}



void affiche_domaine(){
	int i,j,l,m;
	printf("Domaine %x%d \n", _nb_variable,_nb_domaine);
	for(i=1; i<=_nb_variable; i++){
		for(j=1; j<=_nb_domaine; j++){
			printf("%d ",domaine[i][j]);
		}
		printf("\n");
	}
}

#define SUCCESS		(0)
#define ECHEC		(-1)
#define RETRO		(-2)


int instancie(int * tab, int n, int d){
	int i;

	if(d > _nb_domaine){
		return RETRO;
	}
	
	for(i=d; i<=_nb_domaine; i++){
		if(domaine[n][i] == 1){
			tab[n] = i;
			debut[n] = i+1;
			//domaine[n][i] = -n;//auto filtrage
			return SUCCESS;
		}
	}
	debut[n] = d+1;
	return ECHEC;
}


int est_vide(int b){
	int i;
	int cpt = 0;
	for(i = 1; i<= _nb_domaine; i++){
		if(domaine[b][i] != 1){
			cpt ++;
		}
	}
	return (cpt == _nb_domaine)?SUCCESS:ECHEC;
}

void restauration(int a, int b){
	int i;
	for(i=1; i<=_nb_domaine ; i++){
		if(domaine[b][i] == -a ){
			domaine[b][i] = 1;
		}
	}
}




void restauration_tout(int a){
	int i, next_val, j;
	for(i=1; i<=nb_contrainte ; i++){
		if(a == indice_contrainte[i].a)
		next_val = indice_contrainte[i].b;
		restauration(a, next_val);
		debut[next_val] = 1;//repartir a zero avec le prochain test
	}
}

void filtrage(int current_var,  int next_var, int * instanciation){
	int i, j;
	int r;
	for(i = 1; i <= nb_contrainte ; i ++){//recherche des prochaines variables ayant une contrainte avec current_var
			if(current_var == indice_contrainte[i].a){
				next_var = indice_contrainte[i].b;
			
				r = instanciation[current_var];//pour les dames, correspond à la colonne de la dame courante

				/*Traitement des domaines de la variable future*/
				for(j = 1; j<= _nb_domaine ; j++){
					if(contrainte[current_var][next_var].relation[r][j] != 1){
						if(domaine[next_var][j] == 1 ){
							domaine[next_var][j] = -current_var;//diminution du domaine de la future variable
						}
						
						//pas besoin de else, car le domaine a été déja filtré par un autre
					}
				}
			}
		}
}

int forward_fc(int * instanciation){
	

	int i,k, j, test_vide = 0, tmp;
	int a_restaure = false;
	int val_courante = 1;
	int test_fin= 0;
	int test_next_val = 0;
	int current_var;
	int inst_state = -1;
	int cij;
	int next_var;
	//int instanciation[_nb_variable+1];

	int nb_solution = 0;

	instanciation[1] = 1;
	int taille_i = 0;
	int r = -1;
	//int debut = 1;
	current_var = 1;
	int inc_test = 0;

	printf("Forward solving...\n");
	for(i=1; i<= _nb_variable; i++){
		printf("%d\t",instanciation[i]);
	}
	printf("\n");

	do{
		inc_test ++;
		test_next_val = 0;	
		inst_state = instancie(instanciation, current_var, debut[current_var]);
		/*pas de solution*/
		if(inst_state == ECHEC && current_var == 1){
			printf("pas de solution\n");
			return ECHEC;
		}

		/*necessité de retrograder*/
		if(inst_state == ECHEC){
			printf("Echec domaine  %d  %d\n", current_var, debut[current_var]);
			continue;
		}


		if(inst_state == RETRO){
			printf("Echec retro %d  %d\n", current_var, debut[current_var]);
			restauration_tout(current_var);//restauration avant application de la valeure suivante
			debut[current_var] = 1;
			current_var --;
			continue;
		}

		if(current_var == _nb_variable){//condition de succès
			
			/*nb_solution ++;
			printf("\033[032m solution %d\n\033[0m", nb_solution );
			current_var = 1;
			if(instanciation[current_var] == _nb_variable){
				//printf("\033[032m solution %d\n\033[0m", nb_solution );
				return nb_solution;
			}
			debut[current_var] = instanciation[current_var] +1;
			continue;*/
			return SUCCESS;
		}

		printf("%d <-- %d  %d \n", current_var, instanciation[current_var], debut[current_var]);

		//pour la prochaine recherche de variable

		/*--------Etape 1 filtrage des domaines-------*/
		filtrage(current_var, next_var, instanciation);


		/*------Etape 2 recherche domaine vide------*/
		for(i = 1; i <= nb_contrainte ; i ++){
			if(current_var == indice_contrainte[i].a){//génial la restauration s'effectuera sur toute variable en contrainte avec current_var
				next_var = indice_contrainte[i].b;
				if(est_vide(next_var) == SUCCESS){
					//affiche_domaine();
					test_next_val = 1;
					break;
					//xaffiche_domaine();
				}
				
			}
		}

		/*instanciation sur la prochaine valeur du domaine
		un domaine a ete vide
		necessite de reprendre une autre valeur*/
		if(test_next_val == 1){
			restauration_tout(current_var);
			//debut[current_var] ++;
		}
		
		else{//nouvelle instanciation pour la pprochaine varible
			current_var ++;
			debut[current_var] = 1;
		}

	}while( current_var > 0 && current_var <= _nb_variable );

	for(i = 1; i <= _nb_variable; i++){
		if(instanciation[i] = 1);
		printf("pas de solution verification\n");
		return ECHEC;
	}

}



int main(int argc, char **argv){
	
	
	int i;
	printf("Initialisation ... \n");
	init_dame(10);
	printf("Initialisation terminée %d... \n", _nb_variable);
	int instanciation[_nb_variable];
	for(i=1; i<= _nb_variable; i++){
		instanciation[i] = -1;
	}

	printf("Affichage... \n");
	affiche_tout(domaine, contrainte);
	printf("Affichage terminée ... \n");
	forward_fc(instanciation);

	for(i=1; i<= _nb_variable; i++){
		printf("%d\t",instanciation[i]);
	}

	return 0;
}