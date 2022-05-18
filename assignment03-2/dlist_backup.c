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

/* Allocates dynamic memory for a list head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
LIST *createList(void){
	LIST *HEAD = (LIST*)malloc(sizeof(LIST));
	HEAD->count = 0;
	HEAD->head = NULL;
	HEAD->rear = NULL;

	return HEAD;
}

/* Deletes all data in list and recycles memory
*/
void destroyList( LIST *pList){
	NODE *temp = (NODE*)malloc(sizeof(NODE));
	while(pList->head != NULL){
		temp = pList->head;
		pList->head = pList->head->rlink;
		free(temp);
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
	if (pLoc == NULL) return 0;
	// 딴데서 보면 pLoc->rlink != NULL인것도 있던데, 만약 안되면 그것도 확인.
	while ( pLoc != NULL ){
		pPre = pLoc;
		pLoc = pLoc->rlink;
		if (cmpName(dataInPtr, pLoc->dataPtr)==-1){
			// a<b여서 더 뒤로 가야 하는 경우 a<b: -1
			continue;
		}
		else if (cmpName(dataInPtr, pLoc->dataPtr)==1){
			// a>b여서 pPre와 temp사이에 node를 추가하면 되는 경우 a>b:1
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


/* Removes data from list
	return	0 not found
			1 deleted
*/
int removeNode( LIST *pList, tName *keyPtr, tName **dataOutPtr);


/* interface to search function
	Argu	key being sought
	dataOut	contains found data
	return	1 successful
			0 not found
*/
int searchList( LIST *pList, tName *pArgu, tName **dataOutPtr);


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
void traverseList( LIST *pList, void (*callback)(const tName *));


/* traverses data from list (backward)
*/
void traverseListR( LIST *pList, void (*callback)(const tName *));


/* internal insert function
	inserts data into a new node
	return	1 if successful
			0 if memory overflow
*/
static int _insert( LIST *pList, NODE *pPre, tName *dataInPtr){
	NODE *newNode = (NODE*)malloc(sizeof(NODE));
	newNode->dataPtr = dataInPtr;
	newNode->llink = NULL;
	newNode->rlink = NULL;
	if (pPre == NULL) return 0;
	// #1 처음(헤드 바로 뒤)에 넣는 경우
	// #2 맨 뒤에 넣는 경우
	// #3 그 외 위치에 넣는 경우
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
		pList->rear = newNode;
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


/* internal delete function
	deletes data from a list and saves the (deleted) data to dataOut
*/
static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, tName **dataOutPtr){
	tName *temp = (tName*)malloc(sizeof(tName));
	NODE *ptr = pList->head->rlink;
	//_search(pList, pPre, pLoc, &temp);
}

/* internal search function
	searches list and passes back address of node
	containing target and its logical predecessor
	return	1 found
			0 not found
*/
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, tName *pArgu);

////////////////////////////////////////////////////////////////////////////////
/* Allocates dynamic memory for a name structure, initialize fields(name, freq) and returns its address to caller
	return	name structure pointer
			NULL if overflow
*/
tName *createName( char *str, char sex, int freq){
	tName *temp = (tName*)malloc(sizeof(tName));
	//strcpy(temp->name, str);
	temp->name = str;
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
				//destroyList( list);
				return 0;
			
			case FORWARD_PRINT:
				//traverseList( list, print_name);
				break;
			
			case BACKWARD_PRINT:
				//traverseListR( list, print_name);
				break;
			
			case SEARCH:
				fprintf( stderr, "Input a name/sex to find: ");
				fscanf( stdin, "%s", str);
				split_name_sex(str, &sex);
				
				//pName = createName( str, sex, 0);

				//if (searchList( list, pName, &p)) print_name( p);
				//else fprintf( stdout, "%s not found\n", str);
				
				//destroyName( pName);
				break;
				
			case DELETE:
				fprintf( stderr, "Input a name/sex to delete: ");
				fscanf( stdin, "%s", str);
				split_name_sex(str, &sex);
				
				//pName = createName( str, sex, 0);

				//if (removeNode( list, pName, &p))
				{
					//fprintf( stdout, "(%s, %c, %d) deleted\n", p->name, p->sex, p->freq);
					//destroyName( p);
				}
				//else fprintf( stdout, "%s not found\n", str);
				
				//destroyName( pName);
				break;
			
			case COUNT:
				//fprintf( stdout, "%d\n", countList( list));
				break;
		}
		
		if (action) fprintf( stderr, "Select Q)uit, P)rint, B)ackward print, S)earch, D)elete, C)ount: ");
	}
	return 0;
}
