#include "compiler.h"

struct TYPE{
	int ttype;
	int arraysize;/*size of array*/
	struct TYPE *etp;/*pointer to element type if TARRAY*/
	struct TYPE *paratp;/*pointer to parameter's type list if ttype os TPPROC*/
};

struct LINE{
	int refinenum;
	struct LINE *nextlinep;
};

struct ID {
	char *name;
	char *procname;
	struct TYPE *itp;
	int is_para;
	int definenum;
	struct LINE *irefp;
	struct ID *nextp;
}  *globalidroot,*localidroot;

void init_globalidtab() {		/* Initialise the table */
	globalidroot = NULL;
}

void init_localidtab() {		/* Initialise the table */
    localidroot = NULL;
}

struct ID *search_idtab(char *np,int scope) {	/* search the name pointed by np */
	struct ID *p;

    if(scope == global){
        p = globalidroot;
    }
    else{
        p = localidroot;
    }

	for(; p != NULL; p = p->nextp) {
		if(strcmp(np, p->name) == 0) return(p);
	}
	return(NULL);
}

void register_ID(char *_name){ /*use valiable declation */


}

void id_countup(char *np) {	/* Register and count up the name pointed by np */
//	struct ID *p;
//	char *cp;
//
//	if((p = search_idtab(np)) != NULL) p->count++;
//	else {
//		if((p = (struct ID *)malloc(sizeof(struct ID))) == NULL) {
//			printf("can not malloc in id_countup\n");
//			return;
//		}
//		if((cp = (char *)malloc(strlen(np)+1)) == NULL) {
//			printf("can not malloc-2 in id_countup\n");
//			return;
//		}
//		strcpy(cp, np);
//		p->name = cp;
//		p->count = 1;
//		p->nextp = globalidroot;
//		globalidroot = p;
//	}
}

void print_idtab() {	/* Output the registered data */
//	struct ID *p;
//
//	for(p = globalidroot; p != NULL; p = p->nextp) {
//		if(p->count != 0)
//			printf("\t\"Identifier\" \"%s\"\t%d\n", p->name, p->count);
//	}
}

void release_idtab() {	/* Release the data structure */
//	struct ID *p, *q;
//
//	for(p = globalidroot; p != NULL; p = q) {
//		free(p->name);
//		q = p->nextp;
//		free(p);
//	}
//	init_globalidtab();
}
