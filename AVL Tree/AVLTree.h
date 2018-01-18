#ifndef AVLTREE_H_
#define AVLTREE_H_

#include <stdlib.h>

#define MAX(a, b) (((a) >= (b))?(a):(b))

typedef struct node{
	void* elem;
	void* info;
	struct node *pt;
	struct node *lt;
	struct node *rt;
	struct node* next;
	struct node* prev;
	struct node* end;
	long height;
}TreeNode;

typedef struct TTree{
	TreeNode *root;
	TreeNode *nil;
	void* (*createElement)(void*);
	void (*destroyElement)(void*);
	void* (*createInfo)(void*);
	void (*destroyInfo)(void*);
	int (*compare)(void*, void*);
	long size;
}TTree;


TTree* createTree(void* (*createElement)(void*), void (*destroyElement)(void*),
	          void* (*createInfo)(void*), void (*destroyInfo)(void*),
	  	  int compare(void*, void*))
{	
	TTree* t = (TTree*) malloc(sizeof(TTree));
	
	//set-up sentinel
	t->nil = (TreeNode*) malloc(sizeof(TreeNode));
	
	t->nil->elem = t->nil;
	t->nil->info = t->nil;
	t->nil->pt = t->nil;
	t->nil->lt = t->nil;
	t->nil->rt = t->nil;
	t->nil->next = t->nil;
	t->nil->prev = t->nil;
	t->nil->end = t->nil;
	t->nil->height = 0;
	
	t->root = t->nil;
	 
	t->createElement = createElement;
	t->destroyElement = destroyElement;
	t->createInfo = createInfo;
	t->destroyInfo = destroyInfo;
	t->compare = compare;

	t->size = 0;
	
	return t;
}

TreeNode* createTreeNode(TTree *tree, void* value, void* info)
{
	TreeNode* newNode = (TreeNode*) malloc(sizeof(TreeNode));
	
	newNode->elem = tree->createElement(value);
	newNode->info = tree->createInfo(info);
	
	newNode->pt = tree->nil;
	newNode->lt = tree->nil;
	newNode->rt = tree->nil;
	newNode->next = tree->nil;
	newNode->prev = tree->nil;
	newNode->end = newNode;
	
	newNode->height = 1;

	return newNode;
}

void destroyTreeNode(TTree *tree, TreeNode* node)
{
	tree->destroyElement(node->elem);	
	tree->destroyInfo(node->info);
	
	free(node);
}

int isEmpty(TTree* tree)
{
	return (tree->root == tree->nil);
}

TreeNode* search(TTree* tree, TreeNode* x, void* elem)
{	
	//iterate troughout the tree until we find the node that is searched
	while (x != tree->nil && tree->compare(elem, x->elem) != 0) {
		if (tree->compare(elem, x->elem) < 0) {
			x = x->lt;
		} else { 
			x = x->rt;
		}
	}
	return x;
}

TreeNode* minimum(TTree*tree, TreeNode* x)
{
	//the smallest node is the left leaf 
	while (x->lt != tree->nil) {
		x = x->lt;
	}
	return x;			
}

TreeNode* maximum(TTree* tree, TreeNode* x)
{	
	//the biggest node is the right leaf	
	while (x->rt != tree->nil) {
		x = x->rt;
	}
	return x;
}

TreeNode* successor(TTree* tree, TreeNode* x)
{
	if (x == tree->nil) {
		return x;
	}
	if (x->rt != tree->nil) {
		return minimum(tree, x->rt);
	}
	TreeNode* p = x->pt;

	while (p != tree->nil && x == p->rt) {	
		x = p;
		p = p->pt;
	}
	return p; 	
}

TreeNode* predecessor(TTree* tree, TreeNode* x)
{
	if (x == tree->nil) {
		return x;
	}
	if (x->lt != tree->nil) {
		return maximum(tree, x->lt);
	}
	TreeNode* p = x->pt;

	while (p != tree->nil && x == p->lt) {
		x = p;
		p = p->pt;
	}
	return p;		
}

void avlRotateLeft(TTree* tree, TreeNode* x)
{
	TreeNode *y = x->rt;
	x->rt = y->lt;

	if(y->lt != tree->nil)
		y->lt->pt = x; // the parent of the y left subtree is x
	y->pt = x->pt; // update parent for y

	if(x->pt == tree->nil)
		tree->root = y;
	else if(x->pt->lt == x)
		x->pt->lt = y;
	else
		x->pt->rt = y;

	y->lt = x; // x is to the left of y
	x->pt = y; // x parent is y

	// Update heights
	y->height = MAX(y->lt->height, y->rt->height) + 1;
	x->height = MAX(x->lt->height, x->rt->height) + 1;

}

void avlRotateRight(TTree* tree, TreeNode* y)
{
	TreeNode *x = y->lt;

	y->lt = x->rt;
	if(x->rt != tree->nil)
		x->rt->pt = y;

	x->pt = y->pt;

	if(y->pt == tree->nil)
		tree->root = x;
	else if(y->pt->lt == y)
		y->pt->lt = x;
	else
		y->pt->rt = x;

	x->rt = y;
	y->pt = x;

	// Update heights
	y->height = MAX(y->lt->height, y->rt->height) + 1;
	x->height = MAX(x->lt->height, x->rt->height) + 1;
}

int avlGetBalance(TTree* tree, TreeNode *x)
{	
	//'x' is the sentinel
	if (x == tree->nil)
		return 0;
	
	//'x' is the root
	if (x->lt == tree->nil && x->rt == tree->nil)
		return 0;
	
	int leftHeight = 0;
	int rightHeight = 0;
	
	//'x' has a left and/or a right child
	if (x->lt != tree->nil) 
		leftHeight = x->lt->height;

	if (x->rt != tree->nil) 
		rightHeight = x->rt->height;
	
	return leftHeight - rightHeight;
}

void updateHeight(TTree* tree, TreeNode* y) 
{
	int leftHeight = 0;
	int rightHeight = 0;
	
	//if the node has no children
	if (y->lt == tree->nil && y->rt == tree->nil) {
		y->height = 1;	
		return;
	}
	
	//node 'y' has a left or a right child
	if (y->lt != tree->nil) 
		leftHeight = y->lt->height;
	
	if (y->rt != tree->nil)
		rightHeight = y->rt->height;

	y->height = MAX(leftHeight, rightHeight) + 1;
} 

void avlFixUp(TTree* tree, TreeNode* y)
{
	//we iterate upwards to sentinel, update height and get balance 
 	while (y != tree->nil) {
	  	updateHeight(tree, y);
	
		if (avlGetBalance(tree, y) > 1 && avlGetBalance(tree, y->lt) >= 0) {
			avlRotateRight(tree, y);
		}  
		if (avlGetBalance(tree, y) < -1 && avlGetBalance(tree, y->rt) <= 0) {
			avlRotateLeft(tree, y);
		}
		if (avlGetBalance(tree, y) > 1 && avlGetBalance(tree, y->lt) < 0) {	
			avlRotateRight(tree, y->lt);
			avlRotateLeft(tree, y);
		} 

		if (avlGetBalance(tree, y) < -1 && avlGetBalance(tree, y->rt) > 0) {
			avlRotateRight(tree, y->rt);
			avlRotateLeft(tree, y);
		}
		y = y->pt;
	}
}

void insert(TTree* tree, void* elem, void* info) 
{	
	//'node' is the node to be inserted
	TreeNode* x = tree->root;
	TreeNode* node = createTreeNode(tree, elem, info);
	
	//'y' is an auxiliary node
	TreeNode* y = tree->nil;
	
	if (x == tree->nil) {
		tree->root = node;
		return;
	}	
	
	while (x != tree->nil) {
		y = x;
		if (tree->compare(node->elem, x->elem) == -1) {
			x = x->lt;
		}
		if (tree->compare(node->elem, x->elem) == 1) {	
			x = x->rt;
		}
	
		//insert 'node' in list only
		if (tree->compare(node->elem, x->elem) == 0) {
			//we check if the previous node is duplicated or not
			if (x->end != x) {
				node->next = x->end->next;
				node->prev = x->end;
				x->end->next->prev = node;
				x->end->next = node;
				x->end = node;
				node->end = node; 		
				return;
			}	
			node->prev = x;
			node->next = x->next;
			x->next->prev = node;	
			x->next = node;	
			x->end = node;
			node->end = node;
			return;
		}	
	}		
	x = y;

	//insert 'node' as a left child
	if (tree->compare(node->elem, x->elem) == -1) {
		x->lt = node;
		node->pt = x;
		node->next = x;
		x->prev->next = node;
		node->prev = x->prev;
		x->prev = node;
		
	}
	//insert 'node' as a right child
	if (tree->compare(node->elem, x->elem) == 1) {	
		x->rt = node;
		node->pt = x;
		
		//we check if 'x' (the parent of node) is duplicated
		//the two cases must be treated separately
		if(x->end != x) {
			node->prev = x->end;
			node->next = x->end->next;
			x->end->next->prev = node;
			x->end->next = node;
		} else {
			node->prev = x;
			x->next->prev = node;
			node->next = x->next;
			x->next = node;
		}
	}	
		//increase tree's size, update height and re-balance the tree	
		tree->size++;
		updateHeight(tree,x);
		avlFixUp(tree, x); 

}

void delete(TTree* tree, void* elem)
{
	// 'z' is the node to be deleted
	TreeNode* z;	
	// 'y' is the node that is spliced out
	TreeNode* y; 	
	// 'x' is the child of the spliced out node
	TreeNode* x;

	// Find the node in the tree if it exists
	if ((z = search(tree,tree->root, elem)) == tree->nil) {
   		 return;
	}
 
	y = ( (z->lt == tree->nil) || (z->rt == tree->nil) ) ? z : successor(tree,z);
	
	//delete a duplicate node from the list only
	if (z->end != z) {
		TreeNode * aux = z->end;		 	
		
		z->end->prev->next = aux->next;
		z->end->next->prev = aux->prev;
		z->end = aux->prev;
		
		tree->destroyElement(aux->elem);
		tree->destroyInfo(aux->info);	
 	 	free(aux);
		return;
	}
	
	x = (y->lt == tree->nil) ? y->rt : y->lt;

 	if(x != tree->nil) {
  		 x->pt = y->pt;
	}
	
 	if(y->pt == tree->nil) { 
   		tree->root = x;
	} else if( y == y->pt->lt) {
   		y->pt->lt = x;
	} else {
		y->pt->rt = x;
	}

	//delete a unique node from the tree
	y->next->prev = y->prev;
	y->prev->next = y->next;
  
	if (y != z) {
		void * swap = z->elem;
		z->elem = y->elem;
		y->elem = swap;
		swap = z->info;
		z->info = y->info;
		y->info = swap;
	}  		
	TreeNode* aux = y;
	y = y->pt;
	
	destroyTreeNode(tree, aux);

	// Re-balance tree and decrease it's size
	tree->size--;
	avlFixUp(tree,y);
}

void destroyTree(TTree* tree)
{
	//'p' is set at the begining of the list 

	TreeNode* p = minimum(tree, tree->root);
	TreeNode* q = p;
	
	//we iterate troughout the list and delete it's nodes
	while (p != tree->nil) {
		q = p;
		p = p->next;
		destroyTreeNode(tree, q);
	}
	
	//destroy tree's sentinel
	free(tree->nil);
	
	//destroy tree
	free(tree);
}

#endif /* AVLTREE_H_ */
