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
//void print_all(LIST *pList);
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


/* Removes data from list
	return	0 not found
			1 deleted
*/
int removeNode( LIST *pList, tName *keyPtr, tName **dataOutPtr){
	int found;
	NODE *pPre;
	NODE *pLoc;

	found = _search(pList, &pPre, &pLoc, keyPtr);
	if (found==1){
		_delete(pList, pPre, pLoc, dataOutPtr);
	}

	return found;
}


/* interface to search function
	Argu	key being sought
	dataOut	contains found data
	return	1 successful
			0 not found
*/
int searchList( LIST *pList, tName *pArgu, tName **dataOutPtr){
	int found;
	NODE *pPre;
	NODE *pLoc;

	found = _search(pList, &pPre, &pLoc, pArgu);
	if (found==1){
		*dataOutPtr = pLoc->dataPtr;
	}
	else{
		*dataOutPtr = NULL;
	}
	return found;
}


/* returns number of nodes in list
*/
int countList( LIST *pList){
	return pList->count;
}


/* returns	1 empty
			0 list has data
*/
int emptyList( LIST *pList){
	if (pList->count==0) return 1;
	else return 0;
}


/* traverses data from list (forward)
*/
// callback이라는 이름의 함수로 인자를 받아옴.
void traverseList( LIST *pList, void (*callback)(const tName *)){
	//callback(pList->head);
	NODE *temp;
	temp = pList->head;
	while(temp != NULL){
		callback(temp->dataPtr);
		temp = temp->rlink;
	}
	
}


/* traverses data from list (backward)
*/
void traverseListR( LIST *pList, void (*callback)(const tName *)){
	NODE *temp;
	temp = pList->rear;
	int cnt = pList->count;
	while(cnt > 1){
		callback(temp->dataPtr);
		temp = temp->llink;
		cnt--;
	}
	// haed에서 llink타면 오류나서 마지막것만 빼놓음.
	callback(pList->head->dataPtr);
	//if (pList->count != 0) callback(temp->dataPtr);
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
            pList->rear = newNode;
        }

    }
    
    else if (pPre->rlink==NULL){ //맨 뒤에 추가할 때, pPre->rlink->llink빼고 실행하도록 함.
        //맨 앞 이외
        newNode->llink = pPre;
        newNode->rlink = pPre->rlink;
        pPre->rlink = newNode;
        if (newNode->rlink == NULL){
            //맨 뒤에 insert하면
            pList->rear = newNode;
        }
    }
    else{
        newNode->llink = pPre;
        newNode->rlink = pPre->rlink;
        pPre->rlink->llink = newNode;
        pPre->rlink = newNode;
        if (newNode->rlink == NULL){
            //맨 뒤에 insert하면
            pList->rear = newNode;
        }
    }
    
    (pList->count)++;

    return 1;
}

/* internal delete function
	deletes data from a list and saves the (deleted) data to dataOut
*/
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

/* internal search function
	searches list and passes back address of node
	containing target and its logical predecessor
	return	1 found
			0 not found
*/
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, tName *pArgu){
    int found;

    *pPre = NULL;
    *pLoc = pList->head;
    if ((pList->count) == 0) {
        return 0;
    }
    // (cmpName(pArgu, pList->rear->dataPtr) > 0)면 바꾸세요~ 인거니까 pArgu가 맨 마지막 위치에 있어야할때.
    if (cmpName(pArgu, pList->rear->dataPtr) > 0){
        *pPre = pList->rear;
        *pLoc = NULL;
        return 0;
    }
    while ( 1 ){
        found = cmpName(pArgu, (*pLoc)->dataPtr);
        if (found == 0) {
            return 1;
        }
        else if (found < 0) {
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
	//qfree(pNode->name);
	free(pNode);
}

////////////////////////////////////////////////////////////////////////////////
/* gets user's input
*/
int get_action()
{
	char ch;
	scanf( "%c", &ch);
	ch = toupper( ch);
	switch( ch)
	{
		case 'Q':
			return QUIT;
		case 'P':
			return FORWARD_PRINT;
		case 'B':
			return BACKWARD_PRINT;
		case 'S':
			return SEARCH;
		case 'D':
			return DELETE;
		case 'C':
			return COUNT;
	}
	return 0; // undefined action
}

// compares two names in name structures
// for createList function
// a < b면 -1출력
int cmpName( const tName *pName1, const tName *pName2)
{
	int ret = strcmp( pName1->name, pName2->name);
	if (ret == 0) return pName1->sex - pName2->sex;
	else return ret;
}

// prints contents of name structure
// for traverseList and traverseListR functions
void print_name(const tName *dataPtr)
{
	printf( "%s\t%c\t%d\n", dataPtr->name, dataPtr->sex, dataPtr->freq);
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
	
	if (argc != 2){
		fprintf( stderr, "usage: %s FILE\n", argv[0]);
		return 1;
	}
	
	fp = fopen( argv[1], "rt");
	if (!fp)
	{
		fprintf( stderr, "Error: cannot open file [%s]\n", argv[1]);
		return 2;
	}
	
	// creates an empty list
	list = createList();
	if (!list)
	{
		printf( "Cannot create list\n");
		return 100;
	}
	
	while(fscanf( fp, "%*d\t%s\t%c\t%d", str, &sex, &freq) != EOF)
	{
		pName = createName( str, sex, freq);
		
		ret = addNode( list, pName);
		
		if (ret == 2) // duplicated
		{
			destroyName( pName);
		}
	}
	
	fclose( fp);
	
	fprintf( stderr, "Select Q)uit, P)rint, B)ackward print, S)earch, D)elete, C)ount: ");
	
	while (1)
	{
		tName *p;
		int action = get_action();
		
		switch( action)
		{
			case QUIT:
				destroyList( list);
				return 0;
			
			case FORWARD_PRINT:
				traverseList( list, print_name);
				break;
			
			case BACKWARD_PRINT:
				traverseListR( list, print_name);
				break;
			
			case SEARCH:
				fprintf( stderr, "Input a name/sex to find: ");
				fscanf( stdin, "%s", str);
				split_name_sex(str, &sex);
				
				pName = createName( str, sex, 0);

				if (searchList( list, pName, &p)) print_name( p);
				else fprintf( stdout, "%s not found\n", str);
				
				destroyName( pName);
				break;
				
			case DELETE:
				fprintf( stderr, "Input a name/sex to delete: ");
				fscanf( stdin, "%s", str);
				split_name_sex(str, &sex);
				
				pName = createName( str, sex, 0);

				if (removeNode( list, pName, &p))
				{
					fprintf( stdout, "(%s, %c, %d) deleted\n", p->name, p->sex, p->freq);
					destroyName( p);
				}
				else fprintf( stdout, "%s not found\n", str);
				
				destroyName( pName);
				break;
			
			case COUNT:
				fprintf( stdout, "%d\n", countList( list));
				break;
		}
		
		if (action) fprintf( stderr, "Select Q)uit, P)rint, B)ackward print, S)earch, D)elete, C)ount: ");
	}
	return 0;
}
