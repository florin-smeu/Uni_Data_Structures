#ifndef __SOLVE_H__
#define __SOLVE_H__

//a very large constant
#define MIN 0x3f3f3f3f

#include<stdlib.h>

typedef struct {
	int nn; // number of vertices
	int **Ma; // adjacency matrix
} TGraphM;

//the type of elements contained by the matrix we will operate on
typedef struct {
	int value;
	int state;
} TElem;

//this structure was created to store easier information about 
//the position of a specific element of the matrix
typedef struct {
	int row;
	int col;
} TInfo;

//function used to allocate 'n' sized matrices
TGraphM* allocate_matrix(int n)
{
	int i;
	
	TGraphM* G = malloc(sizeof(TGraphM));
	G->nn = n;
	G->Ma = malloc(n * sizeof(int*));
	for (i = 0; i < n; i++) {	
		G->Ma[i] = malloc(n * sizeof(int));
	}
	return G;  
}

//calculate the minimum value of a matrix column
int get_min_item_col(TGraphM *G, int idx) 
{
	int i;
	int min = G->Ma[0][idx];
	for (i = 1; i < G->nn; i++) {
		if (G->Ma[i][idx] < min) {
			min = G->Ma[i][idx];
		}
	}
	return min;
}

//calculate the minimum value of a matrix row 
int get_min_item_row(TGraphM *G, int idx)
{
	int j;
	int min = G->Ma[idx][G->nn / 2];
	for (j = G->nn / 2 + 1; j < G->nn; j++) {
		if (G->Ma[idx][j] < min) {
			min = G->Ma[idx][j];
		}
	}
	return min;
}


//function that substracts from each element of a row or a 
//column the minimum element of that row or column
TGraphM* create_zeros(TGraphM* G, int n) 
{	
	int i, j;
	int *minItemsRows = malloc(n * sizeof(int));
	int *minItemsCols = malloc(n * sizeof(int));

	for (i = 0; i < n; i++) {	
		minItemsRows[i] = get_min_item_row(G, i);
	}
	
	//row reduction
	for (i = 0; i < n; i++) { 
		for (j = n; j < 2 * n; j++) {
			G->Ma[i][j] -= minItemsRows[i];
		}
	}

	for (j = n; j < 2 * n; j++) {
		minItemsCols[j - n] = get_min_item_col(G, j);
	}
	
	//column reduction
	for (i = 0; i < n; i++) {	
		for (j = n; j < 2 * n; j++) {
			G->Ma[i][j] -= minItemsCols[j];
		}
	}
	return G;
}

int solve(char *testInputFileName) 
{
	int i, j, k, n;
	
	//read from input file
	//create adjancency matrix 'G->Ma'
	FILE *in = fopen(testInputFileName, "rt");
	fscanf(in, "%d", &n);
	
	TGraphM* G = allocate_matrix(2 * n);
	
	//since the graph is undirected, there is a link from 
	//every rabbit to every lair
	//however, the graph is also bipartite, therefore there
	//are no links between any two rabits or any two lairs 
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			G->Ma[i][j] = 0;
		}
	}
	for (i = n; i < 2 * n; i++) {
		for (j = n; j < 2 * n; j++) {
			G->Ma[i][j] = 0;
		}
	}
	for (i = 0; i < n; i++) {
		for (j = n; j < 2 * n; j++) {
			fscanf(in, "%d", &(G->Ma[i][j]));
			G->Ma[j][i] = G->Ma[i][j];
		}
	}
	fclose(in);
 	
	//allocate vector 'zerosPos' 
	//setting its fields to -1 means no information is stored
	TInfo* zerosPos = malloc(n * sizeof(TInfo));
	for (i = 0; i < n; i++) {
		zerosPos[i].row = -1;
		zerosPos[i].col = -1;
	}	
		
	//allocate matrix 'mat' used in further operations
	TElem** mat = malloc(n * sizeof(TElem*));
	for (i = 0; i < n; i++) {
		mat[i] = malloc(n * sizeof(TElem));
	}
	
	//as we have seen, three quarters of the information stored
	//in matrix 'G-Ma' is redundant, therefore it would be nicer
	//to work with a matrix that contains ONLY the relevant information
	//from 'G-ma' 
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			mat[i][j].value = G->Ma[i][j + n];
			mat[i][j].state = 0;
		}
	}
	
	//this sequence corresponds to steps 2 and 3 from README
	//'size' is the variable that stores the number of zeros saved in the 
	//vector 'zerosPos'
	int size = 0;
	while (size < n) {
		//'unmarked' means the number of zeros that are not marked
		//it is important to perform some operations until this 
		//number becomes 0
		int unmarked = 0;
		for (i = 0; i < n; i++) {
	 		for (j = 0; j < n; j++) {
				if (mat[i][j].value == 0 && 
				    mat[i][j].state == 0) {
					unmarked++;
				}
			}
		}
		int a = 0;
 		while (unmarked > 0) {
			unmarked = 0;
			//perform row scanning
			for (i = 0; i < n; i++) {	
				int pos, ctr = 0;
				for (j = 0; j < n; j++) { 
					if (mat[i][j].value == 0 &&
					    mat[i][j].state == 0) {
						pos = j;
						ctr++;
					}
				}
				if (ctr > 1) {
					//more than one zero was found
					//on that specific row, therefore
					//we skip that row
					continue;
				} else if (ctr == 1) {
					//exactly one zero was found, and we
					//mark elements of the column that
					//contains that zero
					for (k = 0; k < n; k++) {
						mat[k][pos].state++;
					}
					zerosPos[a].row = i;
					zerosPos[a].col = pos;
					a++;
				}
			  
			}
			//perform column scanning
			for (j = 0; j < n; j++) {
				int pos, ctr = 0;
				for (i = 0; i < n; i++) {
					if (mat[i][j].value == 0 && 
					    mat[i][j].state == 0) {
						pos = i;
						ctr++;
					}
				} 
				if (ctr > 1) {
					//more than one zero was found
					//on that specific column, therefore
					//we skip that column
					continue;
				} else if (ctr == 1) { 
					//exactly one zero was found, and we
					//mark elements of the row that 
					//contains that zero
					for (k = 0; k < n; k++) {
					mat[pos][k].state++;
					}
					zerosPos[a].row = pos;
					zerosPos[a].col = j;
					a++;
				}
			}
			//update 'unmarked'
			for (i = 0; i < n; i++) {
				for (j = 0; j < n; j++) {
					if (mat[i][j].value == 0 && 
					    mat[i][j].state == 0) {
						unmarked++;
					}
				}
			}
		}
		//compute 'size'
		size = 0;
		for (i = 0; i < n; i++) {
			if (zerosPos[i].row >= 0 && zerosPos[i].col >= 0) {
				size++;
			}
		}
		//if the number of zeros we saved in vector 'zerosPos' is 
		//smaller than the order of the matrix, we have to perform
		//operations explained at step 3 in README
		if (size < n) {
			//find the minimum unmarked element
			int minimum = MIN;
			for (i = 0; i < n; i++) {
				for (j = 0; j < n; j++) {
					if (mat[i][j].value < minimum && 
					    mat[i][j].state == 0) {
						minimum = mat[i][j].value;
					}
				}
			}
			for (i = 0; i < n; i++) {
				for (j = 0; j < n; j++) {
					//substract minimum from all the 
					//elements that are not marked
					if (mat[i][j].state == 0) {
						mat[i][j].value -= minimum;
					}
					//add minimum to elements marked twice
					if (mat[i][j].state == 2) {
						mat[i][j].value += minimum;
					}
				}
			}
			//reset 'state' field of all elements and delete 
			//information from 'zerosPos'
			for (i = 0; i < n; i++) {
				for (j = 0; j < n; j++) {	
					mat[i][j].state = 0;
				}
			}
			for (i = 0; i < n; i++) {
				zerosPos[i].row = -1;
				zerosPos[i].col = -1;
			}
		}
	}
	//we add the elements of the original matrix placed in the positions
	//stored in vector 'zerosPos', since their sum is the smallest
	int sum = 0;
	for (i = 0; i < n; i++) {
		sum += G->Ma[zerosPos[i].row][n + zerosPos[i].col];
	}
	return sum;
}

#endif
