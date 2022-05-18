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
	// 딴데서 보면 pLoc->rlink != NULL인것도 있던데, 만약 안되면 그것도 확인.
	while ( pLoc != NULL ){
        fprintf(stderr, "init addNode2.3\n");
		pPre = pLoc;
        fprintf(stderr, "init addNode2.5\n");
		pLoc = pLoc->rlink;
        fprintf(stderr, "init addNode3\n");
        //_insert(pList, pPre, dataInPtr);
        return 1;
		if (cmpName(dataInPtr, pLoc->dataPtr)==-1){
			// a<b여서 더 뒤로 가야 하는 경우 a<b: -1
			continue;
		}
		else if (cmpName(dataInPtr, pLoc->dataPtr)==1){
			// a>b여서 pPre와 temp사이에 node를 추가하면 되는 경우 a>b:1
            fprintf(stderr, "insert: %s %s\n", pPre->dataPtr->name, dataInPtr->name);
			_insert(pList, pPre, dataInPtr);
			return 1;
		}
		else{
			// a==b여서 freq만 바꾸면 되는 경우
			increase_freq(pLoc->dataPtr, dataInPtr);
			return 2;
		}
	}
	_insert(pList, pPre, dataInPtr); //rear->rlink에 insert하는 경우.
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
	// #1 처음(헤드 바로 뒤)에 넣는 경우
	// #2 맨 뒤에 넣는 경우
	// #3 그 외 위치에 넣는 경우
    /* 무지성 테스트용 코드
    if (pList->head->rlink == NULL){
        newNode->llink = pList->head;
        pList->head->rlink = newNode;
        pList->rear->rlink = newNode;
        fprintf(stderr, "init 맨처음 insert\n");
    }
    else{
        newNode->llink = pList->head;
        newNode->rlink = pList->head->rlink;
        pList->head->rlink->llink = newNode;
        pList->head->rlink = newNode;
        fprintf(stderr, "init 이후 insert\n");
    }

    return 1;
    */
    
	if (pPre == pList->head){
		// 경험상, newNode의 link를 먼저 채워준 후에 다른것들 바꾸면 됨.
		if (pList->head->rlink == NULL){
			//빈 링크드 리스트 였을 때
			newNode->llink = pList->head;

			pList->head->rlink = newNode;
			pList->rear->rlink = newNode;
		}
		else{
			newNode->llink = pList->head;
			newNode->rlink = pList->head->rlink;
			pList->head->rlink->llink = newNode; //head->rlink면 원래 있던 노드임. 그 노드의 pre를 newNode를 가리키게.
			// newNode의 link채운 후에는 긴 애들 먼저
			pList->head->rlink = newNode; //head가 newNode를 가리키게.
		}
	}
	else if (pPre == pList->rear){
		newNode->llink = pList->rear;
		pList->rear->rlink = newNode;
		pList->rear = newNode; //맨뒤에 추가하니 rear을 바꿔줌.
		//이미 newNode의 rlink는 NULL;
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
