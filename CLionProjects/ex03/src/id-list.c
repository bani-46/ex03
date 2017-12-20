#include "compiler.h"

char *typestr[NUMOFTYPE+1] = {//todo
        "",
        "integer","char","boolean",
        "array",
        "integer[a]","char[a]","boolean[a]",
        "proc"
};

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
    il = add_idlist("\0","\0",0,0,0,0,NULL);
    globalidroot = il;
}

void init_localidtab() {		/* Initialise the table */
    struct ID *il;
    il = add_idlist("\0","\0",0,0,0,0,NULL);
    localidroot = il;
}

struct ID *add_idlist(char *_name,
                      char *_procname,
                      int _type,
                      int _ispara,
                      int _defline,
                      int arraysize,
                      struct ID *il) {
    struct ID *new;
    char *c,*pc;
    struct TYPE *typelist,*arraytype,*proctype;

    new = (struct ID *) malloc(sizeof(struct ID));
    typelist = (struct TYPE *) malloc(sizeof(struct TYPE));
    if(_ispara) c = (char *) malloc(strlen(_name) + strlen(_procname) + 2);
    else c = (char *) malloc(strlen(_name) + 1);
    pc = (char *) malloc(strlen(_procname) + 1);

    strcpy(c, _name);
    new->name = c;
    strcpy(pc,_procname);
    new->procname = pc;
    new->itp = typelist;

    if (_type > TPARRAY && _type < TPPROC) {
        new->itp->ttype = TPARRAY;
        arraytype = (struct TYPE *) malloc(sizeof(struct TYPE));
        arraytype->ttype = _type;
        new->itp->etp = arraytype;
        new->itp->arraysize = arraysize;
    }
    else{
        new->itp->ttype = _type;
    }

    new->is_para = _ispara;
    new->definenum = _defline;
    new->nextp = il;

    return new;
}

int insert_idlist(char *_procname,
                  int _type,
                  int _ispara,
                  int scope,
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
                               _ispara, nl->defline,
                                array_size, NULL);
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


void add_proc(char *_procname,int _line){
    struct ID *il,*new;
    struct TYPE *typelist;
    char *c,*pc;

    il = localidroot;
    while(il->nextp != NULL)il = il->nextp;

    new = (struct ID *) malloc(sizeof(struct ID));
    c = (char *) malloc(strlen(_procname) + 1);
    pc = (char *) malloc(strlen(_procname) + 1);
    typelist = (struct TYPE *) malloc(sizeof(struct TYPE));

    strcpy(c,_procname);
    strcpy(pc,_procname);
    new->name = c;
    new->procname = pc;
    new->itp = typelist;
    new->itp->ttype = TPPROC;
    new->is_para = 0;
    new->definenum = _line;
    new->nextp = NULL;
    il->nextp = new;
}

void copy_locallist() {
    struct ID *gl, *ll;
    char *str;
    gl = globalidroot;
    ll = localidroot->nextp;

    regist_proctype();
    while (gl->nextp != NULL)gl = gl->nextp;
    gl->nextp = ll;
    gl = gl->nextp;
    while(gl->nextp != NULL){
        gl = gl->nextp;
        str = strcat(gl->name,".");//todo . or :
        gl->name = strcpy(gl->name, strcat(str, gl->procname));
    }
    localidroot->nextp = NULL;
}

void regist_proctype(){
    struct ID *ll,*p;
    struct TYPE *tl,*new;
    ll = localidroot->nextp;
    p = localidroot->nextp->nextp;

    if(ll->itp->ttype == TPPROC){
        tl = ll->itp;
        while(p->is_para != 0){
            new = (struct TYPE *) malloc(sizeof(struct TYPE));
            new->ttype = p->itp->ttype;
            tl->paratp  = new;
            tl = tl->paratp;
            p = p->nextp;
        }
    }
    else{
        printf("\n[LIST_ERROR]locallist's head is not TPPROC\n");
    }
}

void print_idlist(int _scope){
    struct ID *il;
    struct TYPE *tl;
    struct LINE *rl;

    if(_scope == 0){
        il = globalidroot->nextp;
        printf("\n[GLOBALLIST]\n");
    }
    else {
        il = localidroot->nextp;
        printf("\n[LOCALLIST]\n");
    }
    while(il != NULL){
        rl = il->irefp;
        printf("NAME:%s\t",il->name);
        if(il->itp->ttype == TPARRAY){
            printf("TYPE:array [%d] of %s\t",
                   il->itp->arraysize,
                   typestr[il->itp->etp->ttype -4]);
        }
        else if(il->itp->ttype == TPPROC){
            printf("TYPE:PROCEDURE(");
            tl = il->itp;
            while(tl->paratp != NULL){
                printf("%s,",typestr[tl->paratp->ttype]);
                tl = tl->paratp;
            }
            printf("\b)\t");
        }
        else{
            printf("TYPE:%s\t",typestr[il->itp->ttype]);
        }
        printf("DEFLINE:%d\t",il->definenum);
        printf("REFLINE:");
        while(rl != NULL) {
            printf("%d,", rl->refinenum);
            rl = rl->nextlinep;
        }
        printf("\b");
        printf("\n");
        il = il->nextp;
    }
    printf("\n");
}

int id_count(char *_name,int _scope,int _line){
    struct ID *il;
    struct LINE *new,*rl;

    if(_scope == 0)il = globalidroot;
    else il = localidroot;

    while(il != NULL){
        if(strcmp(il->name,_name) == 0){
            rl = il->irefp;
            if(rl != NULL) {
                while (rl->nextlinep != NULL) rl = rl->nextlinep;
            }
            new = (struct LINE *) malloc(sizeof(struct LINE));
            new->refinenum = _line;
            new->nextlinep = NULL;
            if(rl == NULL) {
                il->irefp = new;
            }
            else{
                rl->nextlinep = new;
            }
            return NORMAL;
        }
        il = il->nextp;
    }
    return ERROR;
}