#include "hashtable.h"

//*** TIPUS ***//



Index hashFunction(Index i){
	return i%100;
}


Node_hash* searchFor(Hashtable h, Index i, Index queue){

	if(h[queue] == NULL){ return NULL; }
	if(i == 0){
		Node_hash* aux = h[queue];
		while(aux->next != NULL) aux = aux->next;
		return aux;
	}
	Node_hash* aux = h[queue];
	while(aux != NULL && aux->index != i) aux = aux->next;
	return aux;
}



int HASH_create(Hashtable h){

	memset(h, 0, 100 * sizeof(Node_hash*));
	return 1;
}


int HASH_insert(Hashtable h, Index i, Element v){

	Node_hash* n = (Node_hash*) malloc(sizeof(Node_hash));
	Node_hash* node = NULL;

	n->next = NULL;
	n->index = i;
	n->value = v;

	node = searchFor(h, 0, hashFunction(i));

	if(node){
		node->next = n;
		return 1;
	}

	h[hashFunction(i)] = n;
	return 1;
}


Element HASH_get(Hashtable h, Index i){

	Node_hash* node = searchFor(h, i, hashFunction(i));

	if(node == NULL) return NULL;

	return node->value;

}


int HASH_delete(Hashtable h, Index i){

	Node_hash* aux;
	Node_hash* node = h[hashFunction(i)];

	if(!node) return 0;

	if((node->index) == i){
		free(node);
		h[hashFunction(i)] = NULL;
		return 1;
	}

	while(1){

		if(node->next == NULL) return 0;

		if(node->next->index == i){
			aux = node->next;
			node->next = node->next->next;
			free(aux);
			return 1;
		}

		node = node->next;
	}


}


int HASH_destroy(Hashtable h){

	int i;
	Node_hash* aux = NULL;
	Node_hash* node = NULL;


	for( i = 0; i < 100 ; i++){

		node = h[i];

		while(node){

			aux = node;
			node = node->next;
			free(aux);

		}
	}

	return 1;

}

