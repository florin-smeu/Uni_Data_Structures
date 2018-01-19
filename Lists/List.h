#ifndef SORTED_LIST_H
#define SORTED_LIST_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct ListNode {
	struct ListNode *urmator;
	struct ListNode *anterior;
	int prioritate;
	char *nume;
} ListNode;

typedef struct List {
	ListNode *primul;
	ListNode *ultimul;
} List;

//variabila dimensiune va retine numarul de ghisee deschise
unsigned int dimensiune = 0;

//functie care aloca un numar 'n' de ghisee
List **aloca_ghisee(int n) 
{
	int i;
		
	List **ghisee = (List **) malloc(n * sizeof(List*));
	for (i = 0; i < n; i++) {
		ghisee[i] = (List *) malloc(sizeof(List));
		ghisee[i]->primul = NULL;
		ghisee[i]->ultimul = NULL;
	}
	return ghisee;
}

//functie care interclaseaza doua liste
List *interclaseaza_liste(List *l1, List *l2)
{
	
	//lista r va contine listele 'l1' si 'l2' interclasate 
	List *r = malloc(sizeof(List));
	r->primul = malloc(sizeof(ListNode));
	//'k' va itera prin lista r, pe masura ce vom adauga noduri in ea
	ListNode *k = r->primul;
	
	//pointerii 'p' si 'q' sunt iteratori prin listele 'l1' si 'l2'
	ListNode *p = l1->primul;
	ListNode *q = l2->primul;
	
	//cea mai mica dintre dimensiunile celor doua liste
	while (p != NULL && q != NULL) {
		if (p->prioritate > q->prioritate ||
		    (p->prioritate == q->prioritate &&
		    strcmp(p->nume, q->nume) <= 0)) {
			
			ListNode *aux = p->urmator;
			k->urmator = p;
			p->anterior = k;
			p->urmator = NULL;
			p = aux;
		} else {
			ListNode *aux = q->urmator;
			k->urmator = q;
			q->anterior = k;
			q->urmator = NULL;
			q = aux;
		}
		k = k->urmator;
	}
	//dimensiunea listei 'l1' e mai mare decat cea a listei 'l2'
	// SAU 
	//dimensiunea listei 'l2' e mai mare decat cea a listei 'l1'
	if (p != NULL) {
		k->urmator = p;
		p->anterior = k;
	} else if (q != NULL) {
		k->urmator = q;
		q->anterior = k;
	}

	free(l1);
	free(l2);
	
	// stergem celula santinela de la inceputul listei 'r'
	p = r->primul->urmator;
	free(r->primul);
	r->primul = p;
	
	//daca lista contine cel putin un element
	if (p != NULL) p->anterior = NULL;
	
	//gasim ultimul element din lista 'r'	
	r->ultimul = NULL;
	for (p = r->primul; p != NULL; p = p->urmator) {
		r->ultimul = p;
	}
	return r;
}

//functie creata pentru redimensionarea vectorului de ghisee					
List **redimensioneaza_ghisee(List **ghisee, int n)
{
	int i;
	
	//vom retine in lista 'r' toate persoanele, ordonate
	//descrescator in functie de prioritate si alfabetic 	
	List *r = malloc(sizeof(List));
	r->primul = NULL;
	r->ultimul = NULL;
	
	//adaugam in lista 'r' toate persoanele care stau la
	//coada la un momentdat
	for (i = 0; i < dimensiune; i++) {
		r = interclaseaza_liste(r, ghisee[i]);
	}
	
	free(ghisee);
	ghisee = aloca_ghisee(n);
	
	//distribuim persoanele din lista 'r' la cele 'n' ghisee nou create
	while (r->primul != NULL) {
		for (i = 0; i < n; ++i) {
			if (r->primul == NULL) break;
			r->primul->anterior = ghisee[i]->ultimul;
			
			if (ghisee[i]->ultimul != NULL) {
				ghisee[i]->ultimul->urmator = r->primul;
			} else {
				ghisee[i]->primul = r->primul;
				ghisee[i]->ultimul = r->primul;
				r->primul = r->primul->urmator;
				ghisee[i]->ultimul->urmator = NULL;
				continue;
			}
			ghisee[i]->ultimul = r->primul;
			if (r->primul->urmator != NULL) {
				r->primul->urmator->anterior = NULL;
			}
			r->primul = r->primul->urmator;
			ghisee[i]->ultimul->urmator = NULL;
		}
	}
	return ghisee;
}

void deschide_ghisee(List ***ghisee, int N) 
{	
	//apar doua cazuri: 1) niciun ghiseu nu fusese deschis anterior
	//		    2) exista ghisee deja deschise
	if (dimensiune == 0) {
		*ghisee = aloca_ghisee(N);
	} else {
		*ghisee = redimensioneaza_ghisee(*ghisee, N);
	}
	
	//variabila retine numarul de ghisee deschise la un anumit moment
	dimensiune = N;
}

void inchide_ghisee(List ***ghisee) 
{
	
	int i;

	//pointer folosit pentru iteratie
	ListNode *p;

	for (i = 0; i < dimensiune; i++) {
		p = (*ghisee)[i]->primul;
		while (p != NULL) {
			ListNode *nod = p;
			p = p->urmator;
			free(nod);
		}
		free((*ghisee)[i]);
	}
	free(*ghisee);
	
	dimensiune = 0;
}

//functie pentru alocarea unui nod de lista 
ListNode *aloca_celula(char *nume, int prioritate)
 {
	ListNode *p = (ListNode*) malloc(sizeof(ListNode));
	
	if (p != NULL) {
		p->urmator = NULL;
		p->anterior = NULL;
		p->nume = malloc((strlen(nume) + 1) * sizeof(char));
		strcpy(p->nume, nume);
		p->prioritate = prioritate;
	}
	return p;
}

//functie care dezaloca o celula din lista
void dezaloca_celula(ListNode *p)
{
	free(p->nume);
	free(p);
}

void adauga_persoana(List ***ghisee, char *nume, int prioritate, 
		     int numar_ghiseu) 
{
	ListNode *celula = aloca_celula(nume, prioritate);
	
	//daca se doreste adaugarea unei persoane intr-un ghiseu inexistent
	if (numar_ghiseu >= dimensiune) {
		*ghisee = redimensioneaza_ghisee(*ghisee, numar_ghiseu + 1);
		dimensiune = numar_ghiseu + 1;
	}
	
	List *lista = (*ghisee)[numar_ghiseu];
	
	//daca ghiseul nu contine nicio persoana in asteptare
	if(lista->primul == NULL) {
		lista->primul = celula;
		lista->ultimul = celula;
		return;
	}
	
	//daca se doreste adaugarea unei persoane la inceputul listei
	ListNode *p;
	p = lista->primul;
	if (p->prioritate < prioritate ||
	    (p->prioritate == prioritate && strcmp(p->nume, nume) > 0)) {
		p->anterior = celula;
		celula->urmator = p;
		lista->primul = celula;
		return;
	}

	//se parcurge lista pana cand se gaseste pozitia in care trebuie 
	//adaugata persoana
	for (p = lista->primul->urmator; p != NULL; p = p->urmator) {
		if (p->prioritate < prioritate ||
		    (p->prioritate == prioritate && strcmp(p->nume, nume) < 0)) {
			p->anterior->urmator = celula;
			celula->anterior = p->anterior;
			celula->urmator = p;
			p->anterior = celula;
			return;
		}
	}
	lista->ultimul->urmator = celula;
	celula->anterior = lista->ultimul;
	lista->ultimul = celula;
}

void sterge_persoana(List ***ghisee, char *nume, int prioritate, 
		int numar_ghiseu) 
{
	List *lista = (*ghisee)[numar_ghiseu];
	ListNode *p;
	
	//se parcurge lista	
	for (p = lista->primul; p != NULL; p = p->urmator) {
		if (p->prioritate == prioritate && !strcmp(p->nume, nume)) {
			if (p->anterior == NULL) {
				//daca persoana de sters este prima din lista
				lista->primul = p->urmator;
				dezaloca_celula(p);
				if (lista->primul != NULL) 
					lista->primul->anterior = NULL;
			} else if (p->urmator == NULL) {
				//daca persoana de sters este ultima din lista
				lista->ultimul = p->anterior;
				dezaloca_celula(p);
				if (lista->ultimul != NULL)
					lista->ultimul->urmator = NULL;
			} else {
				p->urmator->anterior = p->anterior;
				p->anterior->urmator = p->urmator;
				dezaloca_celula(p);
			}
			return;
		}
	}
}

void sterge_primii_oameni(List ***ghisee) 
{
	int i;
	
	for (i = 0; i < dimensiune; i++) {
		if ((*ghisee)[i]->primul != NULL) {
			//ghiseul contine cel putin o persoana in asteptare		
			ListNode *p = (*ghisee)[i]->primul;
			(*ghisee)[i]->primul = p->urmator;
			if (p->urmator != NULL)
				//ghiseul contine mai mult de o persoana
				p->urmator->anterior = NULL;
			dezaloca_celula(p);
		}
	}
}

int calculeaza_numar_oameni_ghiseu(List **ghisee, int numar_ghiseu) 
{
	int contor = 0;
	
	//daca ghiseul nu exista
	if (numar_ghiseu >= dimensiune) return 0;
	
	ListNode *p = ghisee[numar_ghiseu]->primul;
	while (p != NULL) {
		contor++;
		p = p->urmator; 	
	}

	return contor;
}

int calculeaza_numar_total_oameni(List **ghisee) 
{
	int i,  contor = 0;
	
	for (i = 0; i < dimensiune; i++) {
		contor += calculeaza_numar_oameni_ghiseu(ghisee, i);
	}
	
	return contor;
}

int gaseste_pozitie_persoana (List **ghisee, char *nume, int prioritate, 
		int numar_ghiseu) 
{
	List *lista = ghisee[numar_ghiseu];
	int poz = 0;
	
	//daca ghiseul in care se doreste gasirea persoanei nu exista
	if (numar_ghiseu >= dimensiune) return -1;
	
	ListNode *p;
	for (p = lista->primul; p != NULL; p = p->urmator) {
		if (p->prioritate == prioritate && !strcmp(p->nume, nume)) {
			return poz;
		}
		++poz;
	}

	return -1;
}

char* gaseste_persoana_din_fata(List **ghisee, char *nume, int prioritate, 
		int numar_ghiseu) 
{
	List *lista = ghisee[numar_ghiseu];
	
	if (numar_ghiseu >= dimensiune)	return "Nu exista.";
	
	ListNode *p;
	for (p = lista->primul; p != NULL; p = p->urmator) {
		if (p->prioritate == prioritate && !strcmp(p->nume, nume)) {
			ListNode *q = p->anterior;
			if (q == NULL) return "Nu exista.";
			else return q->nume;
		}
	}

	return "Nu exista.";
}
	
void afiseaza_rand_ghiseu(FILE *f, List **ghisee, int numar_ghiseu) 
{
	//ghiseul nu exista
	if(numar_ghiseu >= dimensiune) {
		fprintf(f, "Ghiseul %d nu are nicio persoana in asteptare.\n",
		        numar_ghiseu);
		return;
	}

	ListNode *p = ghisee[numar_ghiseu]->primul;	
	
	//lista e goala
	if (p == NULL) {
		fprintf(f, "Ghiseul %d nu are nicio persoana in asteptare.\n", 
		        numar_ghiseu);
		return;
	}
	
	while (p != NULL) {
		fprintf(f, "%s-", p->nume);
		fprintf(f, "%d;", p->prioritate);
			
		p = p->urmator;
	}
	fprintf(f, "\n");
}
	

#endif
