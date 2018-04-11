#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>


#define N_MAIN_PROCESSES	3
#define N_CON_PROTOCOLS		3
#define MAIN_PROCESS		1
#define UI_PROCESS 		0
#define MONIT_PROCESS		2


typedef struct node{

	long  pid;
	struct node* parent;
	struct node** childs;
	int nchilds;
}Node;


typedef	Node* processTree;


processTree	 PT_create(long);
processTree	 PT_destroy(processTree pt);
processTree	 PT_getChild(processTree pt, int child);
processTree	 PT_getParent(processTree pt);
int		 	 PT_addChild(processTree pt, long pid);				//funcio generica. no interessa?
int 		 PT_removeChildByIndex(processTree pt, int child);			//funcio generica. no interessa?
int 		 PT_removeChildByPid(processTree pt, long child);			//funcio generica. no interessa?
int		 	 PT_addChildToParent(processTree pt, long parent, long child);
processTree	 PT_searchPID(processTree pt, long pid);
long		 PT_getPPID(processTree pt);
long		 PT_getPID(processTree pt);
long 		 PT_getConnectionPID(processTree pt, int user_id, int con_id);
long		 PT_getUserPID(processTree pt, int user_id);
long		 PT_getProtocol(processTree pt, int user_id, int con_id, int prtcl_id);

//fer funcions que encapsulin la eliminacio de connexions
//i usuaris. Altre tipus d'eliminacions ja implican
//destruccio completa de larbre i per tant petar el programa

