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

struct NAMELIST{
    char *name;
    int defline;
    struct NAMELIST *nextp;
}*name_list;


void init_namelist(){
    struct NAMELIST *nl;
    nl = add_namelist("",0,NULL);
    name_list = nl;
}

struct NAMELIST *add_namelist(char *_name,int _defline,struct NAMELIST *nl) {
    struct NAMELIST *new = (struct NAMELIST *) malloc(sizeof(struct NAMELIST));
    char *c = (char *) malloc(strlen(_name) + 1);
    if (new != NULL && c != NULL) {
        strcpy(c, _name);
        new->name = c;
        new->defline = _defline;
        new->nextp = nl;
    }
    return new;
}

int insert_namelist(char *_name,int _defline){
    if(search_namelist(_name)==ERROR){
        return ERROR;
    }
    struct NAMELIST *nl = name_list;
    while(nl->nextp != NULL)nl = nl->nextp;
    nl->nextp = add_namelist(_name,_defline,NULL);
    return NORMAL;
}

void print_namelist(){
    struct NAMELIST *nl = name_list->nextp;
    while(nl != NULL){
        printf("\n");
        printf("[namelist]%s\tat line:%d\n",nl->name,nl->defline);
        nl = nl->nextp;
    }
}

int search_namelist(char *_name){
    struct NAMELIST *nl = name_list->nextp;
    while(nl != NULL){
        if(strcmp(_name,nl->name) == 0) {
            return ERROR;
        }
        nl = nl->nextp;
    }
    return NORMAL;
}

void free_namelist(){
    struct NAMELIST *nl,*temp;
    nl = name_list->nextp;
    while(nl != NULL) {
        temp = nl->nextp;
        free(nl);
        nl = temp;
    }
    name_list->nextp = NULL;
 }

void init_globalidtab() {		/* Initialise the table */
    struct ID *il;
    il = add_idlist("\0","\0",0,0,0,0,0,0,NULL);
    globalidroot = il;
}

void init_localidtab() {		/* Initialise the table */
    struct ID *il;
    il = add_idlist("\0","\0",0,0,0,0,0,0,NULL);
    localidroot = il;
}

struct ID *add_idlist(char *_name,
                      char *_procname,
                      int _type,
                      int _ispara,
                      int _isproc,
                      int _defline,
                      int _isarray,
                      int arraysize,
                      struct ID *il) {
    struct ID *new;
    char *c,*pc;
    struct TYPE *typelist,*arraytype,*proctype;

    new = (struct ID *) malloc(sizeof(struct ID));
    typelist = (struct TYPE *) malloc(sizeof(struct TYPE));
    c = (char *) malloc(strlen(_name) + strlen(_procname) + 2);
    pc = (char *) malloc(strlen(_procname) + 1);

    strcpy(c, _name);
    new->name = c;
    strcpy(pc,_procname);
    new->procname = pc;
    new->itp = typelist;
    /*proc*/
    if(_isproc) {
        new->itp->ttype = TPPROC;
        proctype = (struct TYPE *) malloc(sizeof(struct TYPE));
        if(_isarray) {
            proctype->ttype = TPARRAY;
        }else{
            proctype->ttype = _type;
        }
        new->itp->paratp = proctype;
    }
    /*array*/
    if (_isarray) {
        new->itp->ttype = TPARRAY;
        arraytype = (struct TYPE *) malloc(sizeof(struct TYPE));
        switch (_type) {
            case TPINT:
                arraytype->ttype = TPARRAYINT;
                break;
            case TPCHAR:
                arraytype->ttype = TPARRAYCHAR;
                break;
            case TPBOOL:
                arraytype->ttype = TPARRAYBOOL;
                break;
            default:
                break;
        }
        if(_isproc) {
            new->itp->paratp->etp = arraytype;
            new->itp->paratp->etp->arraysize = arraysize;
        }
        else{
            new->itp->etp = arraytype;
            new->itp->arraysize = arraysize;
        }
    }
    if (!_isarray && !_isproc) {
        new->itp->ttype = _type;
    }

    new->is_para = _ispara;
    new->definenum = _defline;

    return new;
}

int insert_idlist(char *_procname,
                  int _type,
                  int _ispara,
                  int scope,
                  int _isarray,
                  int array_size){
    if(search_idlist(scope)==ERROR){
        return ERROR;
    }
    struct ID *il;
    struct NAMELIST *nl = name_list->nextp;

    if(scope == global)il =globalidroot;
    else il = localidroot;

    while(il->nextp != NULL)il = il->nextp;
    while(nl != NULL) {
        il->nextp = add_idlist(nl->name, _procname, _type,
                               _ispara, scope, nl->defline,
                               _isarray, array_size, NULL);
        il = il->nextp;
        nl = nl->nextp;
    }
    return NORMAL;
}

int search_idlist(int _scope){
    struct ID *il;
    struct NAMELIST *nl;
    if(_scope == global)il =globalidroot;
    else il = localidroot;

    while(il != NULL){
        nl = name_list->nextp;
        while(nl != NULL) {
            if(il->name == NULL)break;//todo
            if (strcmp(nl->name, il->name) == 0) {
                return ERROR;
            }
            nl = nl->nextp;
        }
        il = il->nextp;
    }
    return NORMAL;
}

void copy_locallist() {
    struct ID *gl, *ll;
    gl = globalidroot;
    ll = localidroot->nextp;
    char *str;
    while (gl->nextp != NULL)gl = gl->nextp;
    gl->nextp = ll;
    while(gl->nextp != NULL){
        gl = gl->nextp;
        str = strcat(gl->name,".");//todo . or :
        gl->name = strcpy(gl->name, strcat(str, gl->procname));
    }
    localidroot->nextp = NULL;
}

void print_idlist(int _scope){
    struct ID *il;
    if(_scope == 0){
        il = globalidroot->nextp;
        printf("\n[GLOBALLIST]\n");
    }
    else {
        il = localidroot->nextp;
        printf("\n[LOCALLIST]\n");
    }
    while(il != NULL){
        printf("NAME:%s\t",il->name);
        printf("TYPE:%d\t",il->itp->ttype);
        if(il->itp->ttype == TPARRAY){
            printf("ARRAYTYPE:%d\t",il->itp->etp->ttype);
            printf("ARRAYSIZE:%d\t",il->itp->arraysize);
        }
        else if(il->itp->ttype == TPPROC){
            if(il->itp->paratp->ttype == TPARRAY){
                printf("PROCARRAYTYPE:%d\t",il->itp->paratp->etp->ttype);
                printf("PROCARRAYSIZE:%d\t",il->itp->paratp->etp->arraysize);
            }
            printf("PROCTYPE:%d\t",il->itp->paratp->ttype);
        }
        printf("IS_PARA:%d\t",il->is_para);
        printf("DEFLINE:%d\t",il->definenum);
//        printf("REFLINE:%d",il->irefp->refinenum);
        printf("\n");
        il = il->nextp;
    }
    printf("\n");
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
