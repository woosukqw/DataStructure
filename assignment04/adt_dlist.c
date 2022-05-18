#include <stdlib.h> // malloc

#include "adt_dlist.h"

/* internal insert function
	inserts data into a new node
	return	1 if successful
			0 if memory overflow
*/
static int _insert( LIST *pList, NODE *pPre, void *dataInPtr){
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
static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, void **dataOutPtr){
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
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, void *pArgu){
    int found;

    *pPre = NULL;
    *pLoc = pList->head;
    if ((pList->count) == 0) {
        return 0;
    }
    // (cmpName(pArgu, pList->rear->dataPtr) > 0)면 바꾸세요~ 인거니까 pArgu가 맨 마지막 위치에 있어야할때.
    if ( ((*pList->compare)(pArgu, pList->rear->dataPtr)) > 0){
        *pPre = pList->rear;
        *pLoc = NULL;
        return 0;
    }
    while ( 1 ){
        //found = cmpName(pArgu, (*pLoc)->dataPtr);
		found = ((*pList->compare)(pArgu, (*pLoc)->dataPtr));
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

LIST *createList( int (*compare)(const void *, const void *)){
	LIST *list = (LIST*)malloc(sizeof(LIST));
	if (list){
		list->count = 0;
		list->head = NULL;
		list->rear = NULL;
		list->compare = compare;
		return list;
	}
	return list;
	
}

void destroyList( LIST *pList, void (*callback)(void *)){
	NODE *temp;
	
    if (pList){
        while(pList->count > 0){
            free(pList->head->dataPtr);
            temp = pList->head;
            pList->head = pList->head->rlink;
            (pList->count)--;
            //callback(temp);
			free(temp);
        }
        free(pList);
    }
}

int addNode( LIST *pList, void *dataInPtr, void (*callback)(const void *, const void *)){
	NODE *pLoc;
	NODE *pPre;
    int found;
    int suc;

    found = _search(pList, &pPre, &pLoc, dataInPtr);
    if (found==1){ // 중복인 경우
        // freq 증가시키기!!
        //increase_freq(pLoc->dataPtr, dataInPtr);
		callback(pLoc->dataPtr, dataInPtr);
        return 2;
    }
    suc = _insert(pList, pPre, dataInPtr);
    if (!(suc)) return 0;
    else return 1;

}

int removeNode( LIST *pList, void *keyPtr, void **dataOutPtr){
	int found;
	NODE *pPre;
	NODE *pLoc;

	found = _search(pList, &pPre, &pLoc, keyPtr);
	if (found==1){
		_delete(pList, pPre, pLoc, dataOutPtr);
	}

	return found;
}

int searchList( LIST *pList, void *pArgu, void **dataOutPtr){
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

int countList( LIST *pList){
	return pList->count;
}

int emptyList( LIST *pList){
	return pList->count==0;
}

void traverseList( LIST *pList, void (*callback)(const void *)){
	//callback(pList->head);
	NODE *temp;
	temp = pList->head;
	while(temp != NULL){
		callback(temp->dataPtr);
		temp = temp->rlink;
	}	
}

void traverseListR( LIST *pList, void (*callback)(const void *)){
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
