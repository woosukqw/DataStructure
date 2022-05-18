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
LIST *createList(void);
void destroyList( LIST *pList);
int addNode( LIST *pList, tName *dataInPtr);
int removeNode( LIST *pList, tName *keyPtr, tName **dataOutPtr);
int searchList( LIST *pList, tName *pArgu, tName **dataOutPtr);
int countList( LIST *pList);
int emptyList( LIST *pList);
void traverseList( LIST *pList, void (*callback)(const tName *));
void traverseListR( LIST *pList, void (*callback)(const tName *));

static int _insert( LIST *pList, NODE *pPre, tName *dataInPtr);
static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, tName **dataOutPtr);
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, tName *pArgu);
tName *createName( char *str, char sex, int freq);
void destroyName( tName *pNode);

int cmpName( const tName *pName1, const tName *pName2);
void increase_freq(tName *dataOutPtr, const tName *dataInPtr);
void print_all(LIST *pList);

/* Allocates dynamic memory for a list head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
LIST *createList(void){
	LIST *list = (LIST*)malloc(sizeof(LIST));
	list->count = 0;
	list->head = NULL;
	list->rear = NULL;

	return list;
}

/* Deletes all data in list and recycles memory
*/
void destroyList( LIST *pList){
	NODE *temp;
	
    if (pList){
        while(pList->count > 0){
            free(pList->head->dataPtr);
            temp = pList->head;
            pList->head = pList->head->rlink;
            (pList->count)--;
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
	NODE *pLoc;
	NODE *pPre;
    int found;
    int suc;

    found = _search(pList, &pPre, &pLoc, dataInPtr);
    if (found==1){ // 중복인 경우
        // freq 증가시키기!!
        increase_freq(pLoc->dataPtr, dataInPtr);
        return 2;
    }
    suc = _insert(pList, pPre, dataInPtr);
    if (!(suc)) return 0;
    else return 1;

}

/* internal insert function
	inserts data into a new node
	return	1 if successful
			0 if memory overflow
*/
static int _insert( LIST *pList, NODE *pPre, tName *dataInPtr){
    // pPre와 pPre->rlink(pLoc)사이에 dataInPtr을 넣는다.
    NODE *newNode;
    newNode = (NODE*)malloc(sizeof(NODE));
    if (!(newNode)) return 0;
	newNode->dataPtr = dataInPtr;
	newNode->llink = NULL;
	newNode->rlink = NULL;
	if (pPre == NULL){
        //맨 앞에 넣는 경우
        newNode->rlink = pList->head;
        newNode->llink = NULL;
        pList->head = newNode;
        if((pList->count)==0) {
            fprintf(stderr, "rear set\n");
            pList->rear = newNode;
        }
        fprintf(stderr, "insert res[%s]: pPre==NULL\n", dataInPtr->name);
        /*
        newNode->rlink = pList->head;
        pList->head = newNode;
        if ((pList->count) == 0) pList->rear = newNode;
        */
    }
    
    else if (pPre->rlink==NULL){
        //맨 앞 이외
        newNode->llink = pPre;
        newNode->rlink = pPre->rlink;
        fprintf(stderr, "as set\n");
        //pPre->rlink->llink = newNode;
        fprintf(stderr, "as2 set\n");
        pPre->rlink = newNode;
        fprintf(stderr, "as3 set\n");
        if (newNode->rlink == NULL){
            //맨 뒤에 insert하면
            pList->rear = newNode;
        }
        fprintf(stderr, "insert res[%s]: pPre!=NULL\n", dataInPtr->name);
        /*
        newNode->rlink = pPre->rlink;
        pPre->rlink = newNode;
        // llink 관련 수정.
        if (newNode->rlink == NULL){
            pList->rear = newNode;
        }
        */
    }
    else{
        newNode->llink = pPre;
        newNode->rlink = pPre->rlink;
        fprintf(stderr, "bs set\n");
        pPre->rlink->llink = newNode;
        fprintf(stderr, "bs2 set\n");
        pPre->rlink = newNode;
        fprintf(stderr, "bs3 set\n");
        if (newNode->rlink == NULL){
            //맨 뒤에 insert하면
            pList->rear = newNode;
        }
    }
    
    (pList->count)++;
    fprintf(stderr, "count is %d\n", pList->count);
    return 1;
}

static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, tName **dataOutPtr){
	*dataOutPtr = pLoc->dataPtr;
    if (pPre == NULL){
        pList->head = pLoc->rlink;
    }
    else{
        pPre->rlink = pLoc->rlink;
    }
    if (pLoc->rlink == NULL){
        pList->rear = pPre;
    }
    (pList->count)--;
    free(pLoc);
}

static int _search( LIST *pList, NODE **pPre, NODE **pLoc, tName *pArgu){
    int found;

    *pPre = NULL;
    *pLoc = pList->head;
    if ((pList->count) == 0) {
        fprintf(stderr, "search res[%s]: 맨처음 노드 삽입\n", pArgu->name);
        return 0;
    }
    // (cmpName(pArgu, pList->rear->dataPtr) > 0)면 바꾸세요~ 인거니까 pArgu가 맨 마지막 위치에 있어야할때.
    if (cmpName(pArgu, pList->rear->dataPtr) > 0){
        fprintf(stderr, "search res[%s]: 1 맨 뒤 삽입\n", pArgu->name);
        *pPre = pList->rear;
        *pLoc = NULL;
        return 0;
    }
    while ( 1 ){
        found = cmpName(pArgu, (*pLoc)->dataPtr);
        if (found == 0) {
            fprintf(stderr, "search res[%s]: 0 %d 중복!!\n", pArgu->name, found);
            return 1;
        }
        else if (found < 0) {
            fprintf(stderr, "search res[%s]: -1 %d\n", pArgu->name, found);
            return 0;
        }

        *pPre = *pLoc;
        *pLoc = (*pLoc)->rlink;
    }
    if (found == 0){
        return 1;
    }
    else{
        return 0;
    }
}
////////////////////////////////////////////////////////////////////////////////
/* Allocates dynamic memory for a name structure, initialize fields(name, freq) and returns its address to caller
	return	name structure pointer
			NULL if overflow
*/
tName *createName( char *str, char sex, int freq){   
	tName *temp;
    temp = (tName*)malloc(sizeof(tName));
    char *name_temp = (char*)malloc(sizeof(char)*30); // valgrind 조심.
    // *str주소를 받아오다보니 마지막꺼에 모든 temp->name이 영향받아서 바뀌어버림..
    strcpy(name_temp, str); 
    // strcpy()쓰면 안됨. 포인터에 포인터가 가리키는 주소를 넘겨주면 되니 저거 안씀.
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
// a < b면 -1출력
int cmpName( const tName *pName1, const tName *pName2)
{
    //fprintf(stderr, "cmp: %s-%s\n", pName1->name, pName2->name);
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
    pLoc = pList->head;
    //fprintf(stderr, "init2 얘가 나오면 insert가 되긴 된거임.\n");
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
	
	fp = fopen( "name3.txt", "rt");
	
	// creates an empty list
	list = createList();
	if (!list)
	{
		printf( "Cannot create list\n");
		return 100;
	}

	while(fscanf( fp, "%*d\t%s\t%c\t%d", str, &sex, &freq) != EOF)
	{
        fprintf(stderr, "init[%d]\n", freq);
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
