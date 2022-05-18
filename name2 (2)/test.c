#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_YEAR_DURATION	10	// 기간

int compare( const void *n1, const void *n2);
int binary_search( const void *key, const void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));

// 구조체 선언
typedef struct {
	char	name[20];		// 이름
	char	sex;			// 성별 M or F
	int		freq[MAX_YEAR_DURATION]; // 연도별 빈도
} tName;

typedef struct {
	int		len;		// 배열에 저장된 이름의 수
	int		capacity;	// 배열의 용량 (배열에 저장 가능한 이름의 수)
	tName	*data;		// 이름 배열의 포인터
} tNames;

tNames *create_names(void)
{
	tNames *pnames = (tNames *)malloc( sizeof(tNames));
	
	pnames->len = 0;
	pnames->capacity = 8;
	pnames->data = (tName *)malloc(pnames->capacity * sizeof(tName));

	return pnames;
}

// 이름 구조체에 할당된 메모리를 해제
void destroy_names(tNames *pnames)
{
	free(pnames->data);
	pnames->len = 0;
	pnames->capacity = 0;

	free(pnames);
}

int main(){
    tNames *names;
    tName *ptr;

    names = create_names();
    /*
    char *arr[6];
    strcpy(arr[0], "aa");
    strcpy(arr[1], "bb");
    strcpy(arr[2], "cc");
    */
    //fprintf(stderr, "%d\n", sizeof(*(arr+1)));
    //memmove(arr+sizeof(arr[0])*2,arr, sizeof(arr[0])*5);
    ptr = names->data;
    
    
    strcpy(names->data[0].name, "aaa");
    names->data[0].sex = 'F';
    names->data[0].freq[0] = 1;
    strcpy(names->data[1].name, "bbb");
    names->data[1].sex = 'G';
    names->data[1].freq[0] = 2;
    strcpy(names->data[2].name, "ccc");
    names->data[2].sex = 'H';
    names->data[2].freq[0] = 3;
    printf("%s\n", (((ptr+1)->name)));
    for (int i=0; i<3; i++){
        printf("%s %c %d\n", names->data[i].name, names->data[i].sex, names->data[i].freq[0]);
    }
    fprintf(stderr, "%lu %lu\n", sizeof(names->data[0]) , sizeof(tName));
    printf("memory: %p %p\n", (names->data+sizeof(tName)*3), (names->data));
    printf("memory: %p %p\n", (tName*)(ptr+3), (tName*)(ptr+2));
    
    memmove((ptr+3), (ptr+2), sizeof(tName)*3);
    printf("============================\n");
    for (int i=0; i<7; i++){
        printf("%s %c %d\n", names->data[i].name, names->data[i].sex, names->data[i].freq[0]);
    }
    
    destroy_names( names);
}