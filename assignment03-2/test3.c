#include <stdlib.h> // malloc
#include <string.h> // strchr
#include <stdio.h>
#include <string.h> // strdup, strcmp
#include <ctype.h> // toupper

#define QUIT			1
#define FORWARD_PRINT	2
#define BACKWARD_PRINT	3
#define SEARCH			4
#define DELETE			5
#define COUNT			6

// User structure type definition
typedef struct 
{
	char	*name;
	char	sex;
	int		freq;
} tName;

////////////////////////////////////////////////////////////////////////////////
// LIST type definition
typedef struct node
{
	tName		*dataPtr;
	struct node	*llink;
	struct node	*rlink;
} NODE;

typedef struct
{
	int		count;
	NODE	*head;
	NODE	*rear;
} LIST;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations
static int _insert( LIST *pList, NODE *pPre, tName *dataInPtr);
static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, tName **dataOutPtr);
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, tName *pArgu);
int cmpName( const tName *pName1, const tName *pName2);
void increase_freq(tName *dataOutPtr, const tName *dataInPtr);
void print_all(LIST *pList);

/* Allocates dynamic memory for a list head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
LIST *createList(void){
	LIST *HEAD = (LIST*)malloc(sizeof(LIST));
    HEAD->head = NULL;
    HEAD->rear = NULL;
	HEAD->count = 0;
	HEAD->head->rlink = NULL;
	HEAD->rear->rlink = NULL;

	return HEAD;
}

/* Deletes all data in list and recycles memory
*/
void destroyList( LIST *pList){
	NODE *temp = (NODE*)malloc(sizeof(NODE));
    /*
	while(pList->head != NULL){
		temp = pList->head;
		pList->head = pList->head->rlink;
		free(temp);
	}
    */
    if (pList){
        while(pList->count > 0){
            free(pList->head->dataPtr);
            temp = pList->head;
            pList->head = pList->head->rlink;
            pList->count--;
            free(temp);
        }
        free(pList);
    }
}


/* Inserts data into list
	return	0 if overflow
			1 if successful
			2 if duplicated key
*/
int addNode( LIST *pList, tName *dataInPtr){
	NODE *pLoc = (NODE*)malloc(sizeof(NODE));
	NODE *pPre = (NODE*)malloc(sizeof(NODE));
	pLoc = pList->head;
	pPre = pLoc;
    fprintf(stderr, "init addNode1\n");
	//if (pLoc == NULL) return 0;
    fprintf(stderr, "init addNode2\n");
	// ????????? ?????? pLoc->rlink != NULL????????? ?????????, ?????? ????????? ????????? ??????.
	while ( pLoc != NULL ){
        fprintf(stderr, "init addNode2.3\n");
		pPre = pLoc;
        fprintf(stderr, "init addNode2.5\n");
		pLoc = pLoc->rlink;
        fprintf(stderr, "init addNode3\n");
        //_insert(pList, pPre, dataInPtr);
        return 1;
		if (cmpName(dataInPtr, pLoc->dataPtr)==-1){
			// a<b?????? ??? ?????? ?????? ?????? ?????? a<b: -1
			continue;
		}
		else if (cmpName(dataInPtr, pLoc->dataPtr)==1){
			// a>b?????? pPre??? temp????????? node??? ???????????? ?????? ?????? a>b:1
            fprintf(stderr, "insert: %s %s\n", pPre->dataPtr->name, dataInPtr->name);
			_insert(pList, pPre, dataInPtr);
			return 1;
		}
		else{
			// a==b?????? freq??? ????????? ?????? ??????
			increase_freq(pLoc->dataPtr, dataInPtr);
			return 2;
		}
	}
	_insert(pList, pPre, dataInPtr); //rear->rlink??? insert?????? ??????.
	return 1;
}

/* internal insert function
	inserts data into a new node
	return	1 if successful
			0 if memory overflow
*/
static int _insert( LIST *pList, NODE *pPre, tName *dataInPtr){
	NODE *newNode = (NODE*)malloc(sizeof(NODE));
    newNode->dataPtr = (tName*)malloc(sizeof(tName));
	newNode->dataPtr = dataInPtr;
    fprintf(stderr, "insert name: %s\n", newNode->dataPtr->name);
	newNode->llink = NULL;
	newNode->rlink = NULL;
    fprintf(stderr, "init insert 1\n");
	//if (pPre == NULL) return 0;
	// #1 ??????(?????? ?????? ???)??? ?????? ??????
	// #2 ??? ?????? ?????? ??????
	// #3 ??? ??? ????????? ?????? ??????
    /* ????????? ???????????? ??????
    if (pList->head->rlink == NULL){
        newNode->llink = pList->head;
        pList->head->rlink = newNode;
        pList->rear->rlink = newNode;
        fprintf(stderr, "init ????????? insert\n");
    }
    else{
        newNode->llink = pList->head;
        newNode->rlink = pList->head->rlink;
        pList->head->rlink->llink = newNode;
        pList->head->rlink = newNode;
        fprintf(stderr, "init ?????? insert\n");
    }

    return 1;
    */
    
	if (pPre == pList->head){
		// ?????????, newNode??? link??? ?????? ????????? ?????? ???????????? ????????? ???.
		if (pList->head->rlink == NULL){
			//??? ????????? ????????? ?????? ???
			newNode->llink = pList->head;

			pList->head->rlink = newNode;
			pList->rear->rlink = newNode;
		}
		else{
			newNode->llink = pList->head;
			newNode->rlink = pList->head->rlink;
			pList->head->rlink->llink = newNode; //head->rlink??? ?????? ?????? ?????????. ??? ????????? pre??? newNode??? ????????????.
			// newNode??? link?????? ????????? ??? ?????? ??????
			pList->head->rlink = newNode; //head??? newNode??? ????????????.
		}
	}
	else if (pPre == pList->rear){
		newNode->llink = pList->rear;
		pList->rear->rlink = newNode;
		pList->rear = newNode; //????????? ???????????? rear??? ?????????.
		//?????? newNode??? rlink??? NULL;
	}
	else{
		newNode->llink = pPre;
		newNode->rlink = pPre->rlink;
		pPre->rlink->llink = newNode;
		pPre->rlink = newNode;
	}
	pList->count++;
	return 1;
}
////////////////////////////////////////////////////////////////////////////////
/* Allocates dynamic memory for a name structure, initialize fields(name, freq) and returns its address to caller
	return	name structure pointer
			NULL if overflow
*/
tName *createName( char *str, char sex, int freq){   
	tName *temp;
    temp = (tName*)malloc(sizeof(tName));
    char *name_temp = (char*)malloc(sizeof(char)*30); // valgrind ??????.
    // *str????????? ?????????????????? ??????????????? ?????? temp->name??? ??????????????? ???????????????..
    strcpy(name_temp, str); 
    // strcpy()?????? ??????. ???????????? ???????????? ???????????? ????????? ???????????? ?????? ?????? ??????.
	//temp->name = str;
    temp->name = name_temp;
	temp->sex = sex;
	temp->freq = freq;

	return temp;
}

/* Deletes all data in name structure and recycles memory
*/
void destroyName( tName *pNode){
	free(pNode->name);
	free(pNode);
}

////////////////////////////////////////////////////////////////////////////////

// compares two names in name structures
// for createList function
// a < b??? -1??????
int cmpName( const tName *pName1, const tName *pName2)
{
    fprintf(stderr, "cmp: %s-%s\n", pName1->name, pName2->name);
	int ret = strcmp( pName1->name, pName2->name);
    if (ret==0 && (pName1->sex-pName2->sex)==0) fprintf(stderr, "DUPLLQWEQWEQWEQWEQWEQWEQWE\n");
	if (ret == 0) return pName1->sex - pName2->sex;
	else return ret;
}

// prints contents of name structure
// for traverseList and traverseListR functions
void print_name(const tName *dataPtr)
{
	printf( "%s\t%c\t%d\n", dataPtr->name, dataPtr->sex, dataPtr->freq);
}
void print_all(LIST *pList){
    
    NODE *pLoc = (NODE*)malloc(sizeof(NODE));
    //fprintf(stderr, "init1\n");
    pLoc = pList->head->rlink;
    //fprintf(stderr, "init2 ?????? ????????? insert??? ?????? ?????????.\n");
    while (pLoc != NULL){
        //fprintf(stderr, "init3\n");
        print_name(pLoc->dataPtr);
        pLoc = pLoc->rlink;
    }
    printf("successfully printed!\n\n");

}

// increases freq in name structure
// for addNode function
void increase_freq(tName *dataOutPtr, const tName *dataInPtr)
{
	dataOutPtr->freq += dataInPtr->freq;
}

// splits name and sex from string
// ex) "Zoe/M"
void split_name_sex(char *str, char *sex)
{
	char *p;
	
	p = strchr(str, '/');
	if (p == NULL) 
	{
		fprintf( stderr, "unexpected name/sex format\n");
		return;
	}
	else
	{
		*p++ = 0;
		*sex = *p;
	}
}
////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	LIST *list;
	
	char str[1024];
	char name[100];
	char sex;
	int freq;
	
	tName *pName;
	int ret;
	FILE *fp;
	
	fp = fopen( "test.txt", "rt");
	
	// creates an empty list
	list = createList();
	if (!list)
	{
		printf( "Cannot create list\n");
		return 100;
	}
	fprintf(stderr, "init\n");
	while(fscanf( fp, "%*d\t%s\t%c\t%d", str, &sex, &freq) != EOF)
	{
        fprintf(stderr, "init\n");
		pName = createName( str, sex, freq);
		//fprintf(stdout, "%s %c %d\n", pName->name, pName->sex, pName->freq);
		ret = addNode( list, pName);
		
		if (ret == 2) // duplicated
		{
            fprintf(stderr, "DUPLI\n");
			destroyName( pName);
		}
	}
    print_all(list);
	
	fclose( fp);
	
	return 0;
}
