#include "processTree.h"



static int n_users;
static int* n_connections;


processTree      PT_create(long parentPid){

	processTree pt = (processTree)malloc(sizeof(Node));
	pt->pid = parentPid;
	pt->parent = NULL;
	pt->childs = NULL;
	pt->nchilds = 0;
	assert(pt);
	return pt;
}


processTree	 PT_destroy(processTree pt){

	int i;

	if(pt->childs){
		for(i = 0 ; i < pt->nchilds ; i++){
			PT_destroy(PT_getChild(pt, i));
		}
	}
	free(pt->childs);
	free(pt);
	pt = NULL;
	return pt;

}


processTree      PT_getChild(processTree pt, int child){

	return pt->childs[child - 1];
}


processTree      PT_getParent(processTree pt){
	return pt->parent;
}


int              PT_addChild(processTree pt, long pid){

	Node * child = (Node*)malloc(sizeof(Node));

	child->pid = pid;
	child->parent = pt;
	child->nchilds = 0;
	child->childs = NULL;

	pt->childs = realloc(pt->childs, sizeof(Node*) * (++(pt->nchilds)));
	pt->childs[(pt->nchilds) - 1] = child;

	if(pt->childs[(pt->nchilds) - 1]){
		return 1;
	}
	return 0;
}


int              PT_removeChildByIndex(processTree pt, int child){

	if(child < 0 || child >=  pt->nchilds){
		return 0;
	}
	PT_destroy(pt->childs[child]);
	if(child == pt->nchilds - 1){
		(pt->nchilds)--;
		return 1;
	}
	(pt->nchilds)--;
	memmove(&(pt->childs[child]), &(pt->childs[child + 1]), sizeof(Node*) * (pt->nchilds - child));
	pt->childs = realloc(pt->childs, sizeof(Node*) * pt->nchilds);
	return 1;

}


int              PT_removeChildByPid(processTree pt, long child){

	int i;

	if(child < 0 || child >=  pt->nchilds){
		return 0;
	}
	for(i = 0 ; i < pt->nchilds ; i++){
		if(pt->childs[i]->pid == child){
			break;
		}
	}
	if(pt->nchilds == i){
		return 0;
	}
	PT_destroy(pt->childs[i]);
	if(i == pt->nchilds - 1){
		(pt->nchilds)--;
		return 1;
	}
	(pt->nchilds)--;
	memmove(&(pt->childs[i]), &(pt->childs[i + 1]), sizeof(Node*) * (pt->nchilds - i));
	return 1;
}


int		 PT_addChildToParent(processTree pt, long parent, long pid){


	Node * child = (Node*)malloc(sizeof(Node));

	pt = PT_searchPID(pt, parent);

	child->pid = pid;
	child->parent = pt;
	child->nchilds = 0;
	child->childs = NULL;

	pt->childs = realloc(pt->childs, sizeof(Node*) * (++(pt->nchilds)));
	pt->childs[(pt->nchilds) - 1] = child;

	if(pt->childs[(pt->nchilds) - 1]){
		return 1;
	}
	return 0;


}


processTree PT_searchPID(processTree pt, long pid){

	int i;
	processTree retval = NULL;

	//printf("Searching...\n");

	if(pt->pid == pid){
		return pt;
	}
	if(pt->nchilds == 0){
		return NULL;
	}
	for(i = 0 ; i < pt->nchilds ; i++){
		retval = PT_searchPID(pt->childs[i], pid);
		if(retval){
			if(retval->pid == pid){
				//printf("Found!...\n");
				return retval;
			}
		}

	}
	return NULL;
}


long		 PT_getPPID(processTree pt){
	return pt->pid;
}


long		 PT_getPID(processTree pt){

	return pt->pid;
}


long 		 PT_getConnectionPID(processTree pt, int user_id, int con_id){

	if(user_id < 0 || user_id >= n_users){
		return -1;
	}
	if(con_id < 0 || con_id >= n_connections[user_id]){
		return -1;
	}
	return pt->childs[MAIN_PROCESS]->childs[user_id]->childs[con_id]->pid;
}


long		 PT_getUserPID(processTree pt, int user_id){

	if(user_id < 0 || user_id >= n_users){
		return -1;
	}
	return pt->childs[MAIN_PROCESS]->childs[user_id]->pid;
}


long		 PT_getProtocol(processTree pt, int user_id, int con_id, int prtcl_id){

	if(user_id < 0 || user_id >= n_users){
		return -1;
	}
	if(con_id < 0 || con_id >= n_connections[user_id]){
		return -1;
	}
	if(prtcl_id < 0 || prtcl_id >= N_CON_PROTOCOLS){
		return -1;
	}
	return pt->childs[MAIN_PROCESS]->childs[user_id]->childs[con_id]->childs[prtcl_id]->pid;
}




