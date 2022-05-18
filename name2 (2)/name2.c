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
void print_names( tNames *names, int num_year);

// 함수 원형 선언

// 연도별 입력 파일을 읽어 이름 정보(연도, 이름, 성별, 빈도)를 이름 구조체에 저장
// 이미 구조체에 존재하는(저장된) 이름은 해당 연도의 빈도만 저장
// 새로 등장한 이름은 구조체에 추가
// 주의사항: 동일 이름이 남/여 각각 사용될 수 있으므로, 이름과 성별을 구별해야 함
// 주의사항: 정렬 리스트(ordered list)를 유지해야 함 (qsort 함수 사용하지 않음)
// 이미 등장한 이름인지 검사하기 위해 bsearch 함수를 사용
// 새로운 이름을 저장할 메모리 공간을 확보하기 위해 memmove 함수를 이용하여 메모리에 저장된 내용을 복사
// names->capacity는 2배씩 증가
void load_names( FILE *fp, int start_year, tNames *names){
	int year;
	char *fir, *buffer;
	char sec;
	int thd;
	tName *ptr;
	
	int idx;
	buffer = (char*)malloc(sizeof(char)*40);
	fir = (char*)malloc(sizeof(char)*21);

	fgets(buffer, 40, fp);
	sscanf(buffer, "%d\t%s\t%c\t%d", &year, fir, &sec, &thd);
	strcpy(names->data[0].name, fir);
	names->data[0].sex = sec;
	memset(names->data[0].freq, 0, 10*sizeof(int));
	names->data[0].freq[year-start_year] = thd;
	names->len++;
	//int length = names->len;
	for (int i=0; i<names->len+1; i++){
		printf("%s\t%c\t", names->data[i].name, names->data[i].sex); // 이름 성별

		for (int year=0; year<9; year++){
			printf("%d\t", names->data[i].freq[year]);
		}

		printf("%d\n", names->data[i].freq[9]); //마지막 뒤에는 tap안들어가게..
	}
	while (1){
		ptr = names->data;
		tName p, *key;
		fgets(buffer, 40, fp);
		sscanf(buffer, "%d\t%s\t%c\t%d", &year, fir, &sec, &thd);
		if (feof(fp)) break;

		//printf("%d\t%s\t%c\t%d\n", year, fir, sec, thd);

		if (names->capacity <= names->len+1){
				names->data = realloc(names->data, names->capacity*sizeof(tName)*2); 
				names->capacity *= 2;
		}
		
		strcpy(p.name, fir);
		p.sex = sec;

		key = bsearch(&p, names->data, (names->len)+1, sizeof(tName), (int(*)(const void*, const void*)) compare);

		if (key==NULL){ 
			idx = binary_search(&p, names->data, names->len, sizeof(tName), (int(*)(const void*, const void*)) compare);
			//fprintf(stderr,"idx:%d\n", idx);
			//idx--;
			//memmove(names->data+sizeof(tName)*(idx+1), names->data+sizeof(tName)*idx,sizeof(tName)*(names->len-idx));			
			//memmove((ptr+idx+1), (ptr+idx), sizeof(tName)*((names->len)-idx));
			/*
			for (int i=names->len; i>idx; i--){
				strcpy(names->data[i].name, names->data[i-1].name);
				names->data[i].sex = names->data[i-1].sex;
				for (int j=0; j<10; j++){
					names->data[i].freq[j] = names->data[i-1].freq[j];
				}
			}
			*/
			// names->data[idx+1]  이렇게 하면 주소가 아니라 tName에 해당하는 무언가의 값이 나와서 안됨.
			strcpy(names->data[idx].name, fir);
			names->data[idx].sex = sec;
			memset(names->data[idx].freq, 0, 10*sizeof(int));
			names->data[idx].freq[year-start_year] = thd;
			names->len++;
		}
		else{
			key->freq[year-start_year] = thd;
		}
		//qsort( names->data, names->len, sizeof(tName), compare);
	}
	free(fir);
	free(buffer);
}

// 구조체 배열을 화면에 출력
void print_names( tNames *names, int num_year){
	printf("============================\n");
	for (int i=0; i<names->len; i++){
		printf("%s\t%c\t", names->data[i].name, names->data[i].sex); // 이름 성별

		for (int year=0; year<9; year++){
			printf("%d\t", names->data[i].freq[year]);
		}

		printf("%d\n", names->data[i].freq[9]); //마지막 뒤에는 tap안들어가게..
	}
	printf("============================\n");
}

// bsearch를 위한 비교 함수

int compare( const void *n1, const void *n2){
	tName *ptr1 = (tName*)n1;
	tName *ptr2 = (tName*)n2;
	if (  strcmp( ((tName*)n1)->name,((tName*)n2)->name )==0  ){
		if ( ((tName*)n1)->sex < ((tName*)n2)->sex ) return -1;
		else if ( ((tName*)n1)->sex > ((tName*)n2)->sex ) return 1;
		else return 0;
	}
	else{
		return strcmp( ((tName*)n1)->name, ((tName*)n2)->name);
	}
}

// 이진탐색 함수
// return value: key가 발견되는 경우, 배열의 인덱스
//				key가 발견되지 않는 경우, key가 삽입되어야 할 배열의 인덱스
int binary_search( const void *key, const void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *)){
	int left = 0;
	int right = nmemb;
	int mid = (left+right)/2;
	tName *pt = (tName*)key;
	
	while (left <= right){
		//fprintf(stderr, "+%d+", compare( base+mid, key ));
		//fprintf(stderr, "%s\n", (tName(*))key->name);
		if (compare(base+mid*sizeof(tName), key ) <= -1){ 
			left = mid+1;
		} //base+mid*sizeof(tName)
		else if (compare(base+mid*sizeof(tName), key) >= 1){
			right = mid-1;
		}
		else{//0이나 양수면 발견된거임.
			printf( "[[[bSearch mid res:%d]]]\n", left);
			printf("%s\n", pt->name);
			return mid;
		}

		mid = (left+right)/2;
	}
	// 발견되지 않았을 떄는 음수로 보냄. 
	//fprintf(stderr, "!!\n");
	return left;
}

// 함수 정의

// 이름 구조체 초기화
// len를 0으로, capacity를 1로 초기화
// return : 구조체 포인터
tNames *create_names(void)
{
	tNames *pnames = (tNames *)malloc( sizeof(tNames));
	
	pnames->len = 0;
	pnames->capacity = 1;
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
	
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	tNames *names;
	FILE *fp;
	
	if (argc != 2)
	{
		fprintf( stderr, "Usage: %s FILE\n\n", argv[0]);
		return 1;
	}

	// 이름 구조체 초기화
	names = create_names();
	
	fp = fopen( argv[1], "r");
	if (!fp)
	{
		fprintf( stderr, "cannot open file : %s\n", argv[1]);
		return 1;
	}

	fprintf( stderr, "Processing [%s]..\n", argv[1]);
		
	// 연도별 입력 파일(이름 정보)을 구조체에 저장
	load_names( fp, 2009, names);
	
	fclose( fp);
	
	// 이름 구조체를 화면에 출력

	print_names( names, MAX_YEAR_DURATION);

	// 이름 구조체 해제
	destroy_names( names);
	
	return 0;
}










