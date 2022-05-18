#include <stdlib.h> // malloc, atoi, rand
#include <stdio.h>
#include <assert.h>
#include <time.h> // time

#define RANDOM_INPUT	1
#define FILE_INPUT		2
// ./a.out 파일명 || number
////////////////////////////////////////////////////////////////////////////////
// TREE type definition
typedef struct node
{
	int			data;
	struct node	*left;
	struct node	*right;
} NODE;

typedef struct
{
	NODE	*root;
} TREE; //얘가 헤드느낌

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations
TREE *BST_Create( void);
void BST_Destroy( TREE *pTree);
static void _destroy( NODE *root);
int BST_Insert( TREE *pTree, int data);
static void _insert( NODE *root, NODE *newPtr);
NODE *_makeNode( int data);
int BST_Delete( TREE *pTree, int dltKey);
static NODE *_delete( NODE *root, int dltKey, int *success);
int *BST_Retrieve( TREE *pTree, int key);
static NODE *_retrieve( NODE *root, int key);
void BST_Traverse( TREE *pTree);
static void _traverse( NODE *root);
void printTree( TREE *pTree);
static void _inorder_print( NODE *root, int level);
int BST_Empty( TREE *pTree);


/* Allocates dynamic memory for a tree head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
TREE *BST_Create( void){
	TREE *head = (TREE*)malloc(sizeof(TREE));
	head->root = NULL;


	return head;
}

/* Deletes all data in tree and recycles memory
*/
void BST_Destroy( TREE *pTree){
	if ( !(BST_Empty(pTree)) ){
		_destroy(pTree->root);
		free(pTree);
	}
}

/* internal function (not mandatory)
*/
// _destroy를 BST_Destroy에서 불러와서 없애는 식으로 교수님은 하심. 그러나 그렇게 안해도 괜찮대.
static void _destroy( NODE *root){
	if (root != NULL){
		_destroy(root->left);
		_destroy(root->right);
		free(root);
	}
}

/* Inserts new data into the tree
	return	1 success
			0 overflow
*/
int BST_Insert( TREE *pTree, int data){
	NODE *newNode = _makeNode(data);
	//if (!(newNode)) return 0;

	if (BST_Empty(pTree)){
		pTree->root = newNode;
		fprintf(stderr, "first insert: %d\n", data);
	}
	else{
		_insert(pTree->root, newNode);
	}
	//printf("insert res: %d %d\n", pTree->root->data, pTree->root->right->data);
	return 1;
}

/* internal function (not mandatory)
*/
// 얘도 
static void _insert( NODE *root, NODE *newPtr){
	// left, right가 연결이 안됨.. 왜...
	if (root == NULL){
		root = newPtr;
		fprintf(stderr, "insert: %d\n", root->data);
	}
	else if (newPtr->data < root->data){
		if (root->left == NULL){
			root->left = newPtr;
		}
		else{
			_insert(root->left, newPtr);
			//fprintf(stderr,"left!\n");
		}
	}
	else{
		if (root->right == NULL){
			root->right = newPtr;
			//fprintf(stderr,"\n");
		}
		else{
			_insert(root->right, newPtr);
		}
	}
}

// datanode하나 만들어서 _insert가 적절한 위치에 넣어주는~~
NODE *_makeNode( int data){
	NODE *newNode = (NODE*)malloc(sizeof(NODE));
	newNode->data = data;
	newNode->left = NULL;
	newNode->right = NULL;

	return newNode;
}

/* Deletes a node with dltKey from the tree
	return	1 success
			0 not found
*/
/*
1. 지우는 노드의 자식이 없는경우(leaf node)
--> 그냥 그 노드를 지움.
2. 지우려는 노드의 자식이 1개인경우
--> 노드의 부모와 노드의 자식을 이어줌.
3. 지우려는 노드의 자식이 2개인 경우
--> 오른쪽 서브트리에서 가장 작은 값을 노드 위치에 넣어줌. 
*/
int BST_Delete( TREE *pTree, int dltKey){
	int *suc;
	*suc = 0;
	if (BST_Retrieve(pTree, dltKey)==NULL) return 0;
	else{
		//_delete
		pTree->root = _delete(pTree->root, dltKey, suc);
	}
	return *suc;
}

/* internal function
	success is 1 if deleted; 0 if not
	return	pointer to root
*/
// 경우의 수에 따라 하기. 성공하면 succes에 1넣고 아니면 0. 여기서 탐색해서 지워야댐.
static NODE *_delete( NODE *root, int dltKey, int *success){
	//NODE *temp = (NODE*)malloc(sizeof(NODE));
	//temp = root;
	NODE *temp = root;
	if (temp==NULL) return NULL;
	else if ( dltKey < temp->data ) return _delete(temp->left, dltKey, success);
	else if ( dltKey > temp->data ) return _delete(temp->right, dltKey, success);
	else{
		if ( temp!=NULL ){
			if ( (temp->left==NULL) && (temp->right==NULL) ){
				//1. 지우는 노드의 자식이 없는경우(leaf node) --> 그냥 그 노드를 지움.
				NODE *soon_del = temp;
				*success = 1;
				return root;
			}
			else if ( (temp->left!=NULL) && (temp->right!=NULL) ){
				//3. 지우려는 노드의 자식이 2개인 경우 --> 오른쪽 서브트리에서 가장 작은 값을 노드 위치에 넣어줌. 
				*success = 1;
				return root;
			}
			else{
				//2. 지우려는 노드의 자식이 1개인경우 --> 노드의 부모와 노드의 자식을 이어줌.
				*success = 1;
				return root;
			}
		}		
	}

}

/* Retrieve tree for the node containing the requested key
	return	address of data of the node containing the key
			NULL not found
*/
// 검색. 리턴값이 노드의 data부분 포인터의 주소
int *BST_Retrieve( TREE *pTree, int key){
	if ( !(BST_Empty(pTree)) ) return NULL;
	else{
		NODE *keyNode;
		keyNode = _retrieve(pTree->root, key);
	}
	return &(keyNode->data);
}

/* internal function
	Retrieve node containing the requested key
	return	address of the node containing the key
			NULL not found
*/
static NODE *_retrieve( NODE *root, int key){
	if (root==NULL) return NULL;
	else if (key == root->data) return root;
	else if (key < root->data) return _retrieve(root->left, key);
	else return _retrieve(root->right, key);
}

/* prints tree using inorder traversal
*/
// Inorder Travesre하래. 숫자 오름차순으로 보기 위함.
void BST_Traverse( TREE *pTree){
	if (!(BST_Empty(pTree))){
		_traverse(pTree->root);
	}
	printf("\n");
}
static void _traverse( NODE *root){
	if (root != NULL){
		_traverse(root->left);
		printf("%d ", root->data);
		_traverse(root->right);
	}
}

/* Print tree using inorder right-to-left traversal
오른쪽에서 왼쪽방향으로의 inorder traversal을 하면서, depth에 대해 tap문자를 넣으면 됨.
*/
void printTree( TREE *pTree){
	
	if ( !(BST_Empty(pTree)) ){
		fprintf(stderr, "7-1_________________\n");
		_inorder_print(pTree->root, 0);
	}
}
/* internal traversal function
*/
// 이거 레벨 이용.
static void _inorder_print( NODE *root, int level){
	if (root != NULL){
		_inorder_print(root->right, ++level);
		for (int i=0; i<level-1; i++) printf("\t");
		printf("%d\n", root->data);
		_inorder_print(root->left, ++level);
	}
	
}

/* 
	return 1 if the tree is empty; 0 if not
*/
int BST_Empty( TREE *pTree){
	return pTree->root == NULL? 1 : 0;
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	int mode; // input mode
	TREE *tree;
	int data;
	
	if (argc != 2)
	{
		fprintf( stderr, "usage: %s FILE or %s number\n", argv[0], argv[0]);
		return 1;
	}
	
	FILE *fp;
	
	if ((fp = fopen(argv[1], "rt")) == NULL)
	{
		mode = RANDOM_INPUT;
	}
	else mode = FILE_INPUT;
	
	// creates a null tree
	tree = BST_Create();
	
	if (!tree)
	{
		printf( "Cannot create a tree!\n");
		return 100;
	}

	if (mode == RANDOM_INPUT)
	{
		int numbers;
		numbers = atoi(argv[1]);
		assert( numbers > 0);

		fprintf( stdout, "Inserting: ");
		srand( time(NULL));
		for (int i = 0; i < numbers; i++)
		{
			data = rand() % (numbers*3) + 1; // random number (1 ~ numbers * 3)
			
			fprintf( stdout, "%d ", data);
			
			// insert function call
			int ret = BST_Insert( tree, data);
			if (!ret) break;
		}
	}
	else if (mode == FILE_INPUT)
	{
		fprintf( stdout, "Inserting: ");
		
		while (fscanf( fp, "%d", &data) != EOF)
		{
			fprintf( stdout, "%d ", data);
			
			// insert function call
			int ret = BST_Insert( tree, data);
			if (!ret) break;
		}
		fclose( fp);
	}
	
	fprintf( stdout, "\n");
	
	if (BST_Empty( tree)) // 텍스트 파일이 빈 파일일 경우를 대비.
	{
		fprintf( stdout, "Empty tree!\n");
		BST_Destroy( tree);
		return 0;
	}	
	// inorder traversal --> 여기부턴 빈 tree가 아닌 트리가 만들어진거임.
	fprintf( stdout, "Inorder traversal: ");
	BST_Traverse( tree);
	fprintf( stdout, "\n");
	fprintf(stderr, "7_________________\n");
	// print tree with right-to-left inorder traversal
	fprintf( stdout, "Tree representation:\n");
	printTree(tree);
	fprintf(stderr, "8_________________\n");
	/*
	while (1)
	{
		fprintf( stdout, "Input a number to delete: "); 
		int num;
		if (scanf( "%d", &num) == EOF) break; // EOF: Ctrl+D
		
		int ret = BST_Delete( tree, num);
		if (!ret)
		{
			fprintf( stdout, "%d not found\n", num);
			continue;
		}
		
		// print tree with right-to-left inorder traversal
		fprintf( stdout, "Tree representation:\n");
		printTree(tree);
		
		if (BST_Empty( tree))
		{
			fprintf( stdout, "Empty tree!\n");
			break;
		}
	}
	*/
	BST_Destroy( tree);
	fprintf(stderr, "end_________________\n");
	return 0;
}

