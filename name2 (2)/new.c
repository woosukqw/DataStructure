#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <malloc.h>

#define MAX_YEAR_DURATION	10	// 기간
#define LINEAR_SEARCH 0
#define BINARY_SEARCH 1

// 구조체 선언
typedef struct {
	char	name[20];		// 이름
	char	sex;			// 성별 'M' or 'F'
	int		freq[MAX_YEAR_DURATION]; // 연도별 빈도
} tName;

typedef struct {
	int		len;		// 배열에 저장된 이름의 수
	int		capacity;	// 배열의 용량 (배열에 저장 가능한 이름의 수)
	tName	*data;		// 이름 배열의 포인터
} tNames;

////////////////////////////////////////////////////////////////////////////////
// 함수 원형 선언(declaration)
int compare( const void *n1, const void *n2);

// 연도별 입력 파일을 읽어 이름 정보(이름, 성별, 빈도)를 이름 구조체에 저장
// 이미 구조체에 존재하는(저장된) 이름은 해당 연도의 빈도만 저장
// 새로 등장한 이름은 구조체에 추가
// 주의사항: 동일 이름이 남/여 각각 사용될 수 있으므로, 이름과 성별을 구별해야 함
// names->capacity는 2배씩 증가
// 선형탐색(linear search) 버전
// , -> \t 로 바꾸고 sscanf하기!
// 2009년껄 빠르게하려면 그건 그냥 구조체에 입력하기! 왜냐면 첫 파일이니 겹치는 이름이 없음
/* 주의할 점 
1. 포인터 메모리 할당 크기는 sizeof로 구하면 안됨.
2. 문자열 같은지 비교할 때(이름 비교) strcmp사용.
*/
void load_names_lsearch( FILE *fp, int year_index, tNames *names){
	char *fir;
	char sec;
	int thd;
	int is_duplication = 0;
	int cnt=0;
	char *buffer;
	buffer = (char*)malloc(sizeof(char)*40);
	fir =  (char*)malloc(sizeof(char)*22);
	if (year_index==0){ //맨 처음껀 이미 있는지 확인할 필요 없음!
		while(1){
			//fscanf(fp, "%[^','], %[^','],%d", fir, &sec, &thd);
			fgets(buffer, 40, fp);
			sscanf(buffer, "%[^','], %[^','],%d", fir, &sec, &thd); 
			// 맨 마지막 줄 일때 strlen(fir)==1임.  
			if (feof(fp)){
				break;
			}
			
			//printf("%s\t%c\t%d\n", fir, sec, thd);
			/*
			fgets(buffer, 40, fp);
			sscanf(buffer, "%[^','], %[^','],%d", fir, &sec, &thd); 
			printf("%s\n", buffer);
			*/

			if (names->capacity == names->len){
				//realloc, capacity*=2
				names->data = realloc(names->data, names->capacity*sizeof(tName)*2); // warning!!!!! sizeof(ptr)하면 64bit window에선 8크기나옴, _msize써야돼!!
				//printf("##### len is %d and capacity is %d /// memory size is %ld\n", names->len, names->capacity, sizeof(names->data));
				names->capacity *= 2;
			}

			strcpy(names->data[names->len].name, fir);
			names->data[names->len].sex = sec;
			memset(names->data[names->len].freq, 0, 10*sizeof(int)); //freq배열 값들 0으로 초기화
			names->data[names->len].freq[year_index] = thd;
			cnt++;
			///fprintf(stderr, "resr:%s%c%d:res\n", fir, sec, thd);
			/* 출력
			printf("%s\t%c\t", names->data[names->len].name, names->data[names->len].sex );
			for (int k=0; k<10; k++) printf("%d\t", names->data[names->len].freq[k]);
			printf("\n"); */
			names->len++;
		}
	}
	else{ 
		int length = names->len;
		while(1){	
			is_duplication = 0;
			//fscanf(fp, "%[^','], %[^','],%d", fir, &sec, &thd);
			fgets(buffer, 40, fp);
			sscanf(buffer, "%[^','], %[^','],%d", fir, &sec, &thd); 
			//if (strlen(fir)<2) break; // 라인 끝났는지 판별
			if (feof(fp)){ 
				break; // fgets 하고 나서 체크해야지!
			}
			
			if (names->capacity == names->len){
				names->data = realloc(names->data, names->capacity*sizeof(tName)*2);
				names->capacity *= 2;
			}
			//printf("%s\t%c\t%d\n", fir, sec, thd);
			for (int i=0; i<length; i++){
				if (!(strcmp(names->data[i].name, fir)) && names->data[i].sex==sec){ // 이름, 성별 같을 때
					if ( !(strcmp("Zzyzx", fir)) || !(strcmp("Zyus", fir)) ){
						fprintf(stderr, "%d %s %c:", i, fir, sec);
						for (int year=0; year<9; year++) fprintf(stderr,"%d\t", names->data[i].freq[year]);
						
						fprintf(stderr,"%d\n", names->data[i].freq[9]);
					}	
					names->data[i].freq[year_index] = thd;
					is_duplication = 1;
					break;
				}
			} // 새로운 이름들이 추가가 잘 안됨.;; + NULL문제

			if (is_duplication == 0){ // names->data에 이름 겹치는거 없을땐 새로 생성해서 넣어주기
				//fprintf(stderr, "%s\n", fir);
				strcpy(names->data[names->len].name, fir);
				names->data[names->len].sex = sec;
				memset(names->data[names->len].freq, 0, 10*sizeof(int));
				names->data[names->len].freq[year_index] = thd;
				cnt++;
				names->len++; // len++가 신규 생성에만 있어야됨.!!!!!
			}
			
		}
	}
	free(buffer);
	free(fir);
	fprintf(stderr, "cnt:%d\n", cnt);
}

// 이진탐색(binary search) 버전 (bsearch 함수 이용)
void load_names_bsearch( FILE *fp, int year_index, tNames *names){
	char *fir;
	char sec;
	int thd;
	int is_duplication = 0;
	int cnt=0;
	char *buffer;
	
	
	buffer = (char*)malloc(sizeof(char)*40);
	fir =  (char*)malloc(sizeof(char)*22);
	if (year_index==0){ //맨 처음껀 이미 있는지 확인할 필요 없음!
		while(1){
			//fscanf(fp, "%[^','], %[^','],%d", fir, &sec, &thd);
			fgets(buffer, 40, fp);
			sscanf(buffer, "%[^','], %[^','],%d", fir, &sec, &thd); 
			// 맨 마지막 줄 일때 strlen(fir)==1임.  
			if (feof(fp)){
				break;
			}
			
			//printf("%s\t%c\t%d\n", fir, sec, thd);
			/*
			fgets(buffer, 40, fp);
			sscanf(buffer, "%[^','], %[^','],%d", fir, &sec, &thd); 
			printf("%s\n", buffer);
			*/

			if (names->capacity == names->len){
				//realloc, capacity*=2
				names->data = realloc(names->data, names->capacity*sizeof(tName)*2); // warning!!!!! sizeof(ptr)하면 64bit window에선 8크기나옴, _msize써야돼!!
				//printf("##### len is %d and capacity is %d /// memory size is %ld\n", names->len, names->capacity, sizeof(names->data));
				names->capacity *= 2;
			}

			strcpy(names->data[names->len].name, fir);
			names->data[names->len].sex = sec;
			memset(names->data[names->len].freq, 0, 10*sizeof(int)); //freq배열 값들 0으로 초기화
			names->data[names->len].freq[year_index] = thd;
			cnt++;

			names->len++;
		}
	}
	else{ 
		int length = names->len;
		while(1){	
			tName p, *key;
			//fscanf(fp, "%[^','], %[^','],%d", fir, &sec, &thd);
			fgets(buffer, 40, fp);
			sscanf(buffer, "%[^','], %[^','],%d", fir, &sec, &thd); 
			if (feof(fp)){ 
				break; // fgets 하고 나서 체크해야지!
			}
			
			if (names->capacity == names->len){
				names->data = realloc(names->data, names->capacity*sizeof(tName)*2);
				names->capacity *= 2;
			}
			//printf("%s\t%c\t%d\n", fir, sec, thd);

			strcpy(p.name, fir);
			p.sex = sec;

			key = bsearch(&p, names->data, length, sizeof(tName), (int(*)(const void*, const void*)) compare);
			
			if (key!=NULL){ // 겹칠 때
				//fprintf(stderr, "%s %c\n", key->name, key->sex);
				key->freq[year_index] = thd;
			}
			else{
				fprintf(stderr, "%s %c\n", fir, sec);
				strcpy(names->data[names->len].name, fir);
				names->data[names->len].sex = sec;
				memset(names->data[names->len].freq, 0, 10*sizeof(int));
				names->data[names->len].freq[year_index] = thd;
				cnt++;
				names->len++; // len++가 신규 생성에만 있어야됨.!!!!!
			}
			/*
			if (is_duplication == 0){ // names->data에 이름 겹치는거 없을땐 새로 생성해서 넣어주기
				//fprintf(stderr, "%s\n", fir);
				strcpy(names->data[names->len].name, fir);
				names->data[names->len].sex = sec;
				memset(names->data[names->len].freq, 0, 10*sizeof(int));
				names->data[names->len].freq[year_index] = thd;
				cnt++;
				names->len++; // len++가 신규 생성에만 있어야됨.!!!!!
			}
			*/
			
		}
	}
	free(buffer);
	free(fir);
	fprintf(stderr, "cnt:%d\n", cnt);
}

// 구조체 배열을 화면에 출력
void print_names( tNames *names, int num_year){
	for (int i=0; i<names->len; i++){
		printf("%s\t%c\t", names->data[i].name, names->data[i].sex); // 이름 성별

		for (int year=0; year<9; year++){
			printf("%d\t", names->data[i].freq[year]);
		}

		printf("%d\n", names->data[i].freq[9]); //마지막 뒤에는 tap안들어가게..
	}
}

// qsort, bsearch를 위한 비교 함수
// 정렬 기준 : 이름(1순위), 성별(2순위)
/* 오름차순 하려면,
a < b일 때는 -1을 반환
a > b일 때는 1을 반환
a == b일 때는 0을 반환
*/
//string 타입의 멤버 변수를 이용해 정렬하려면 strcmp 함수를 이용하면 됨.
//(return 음수 if string1 < string2, return 0 if string1 == string2, return 양수 if string1 > string2)
//https://mryeo.tistory.com/20
//https://aossuper8.tistory.com/73
//https://dojang.io/mod/page/view.php?id=638
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

////////////////////////////////////////////////////////////////////////////////
// 함수 정의 (definition)

// 이름 구조체를 초기화
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
	int option;
	
	FILE *fp;
	int num_year = 0;
	
	if (argc <= 2)
	{
		fprintf( stderr, "Usage: %s option FILE...\n\n", argv[0]);
		fprintf( stderr, "option\n\t-l\n\t\twith linear search\n\t-b\n\t\twith binary search\n");
		return 1;
	}
	
	if (strcmp( argv[1], "-l") == 0) option = LINEAR_SEARCH;
	else if (strcmp( argv[1], "-b") == 0) option = BINARY_SEARCH;
	else {
		fprintf( stderr, "unknown option : %s\n", argv[1]);
		return 1;
	}
	
	// 이름 구조체 초기화
	names = create_names();

	// 첫 연도 알아내기 "yob2009.txt" -> 2009
	int start_year = atoi( &argv[2][strlen(argv[2])-8]);
	
	for (int i = 2; i < argc; i++)
	{
		num_year++;
		fp = fopen( argv[i], "r");
		if( !fp) {
			fprintf( stderr, "cannot open file : %s\n", argv[i]);
			return 1;
		}

		int year = atoi( &argv[i][strlen(argv[i])-8]); // ex) "yob2009.txt" -> 2009
		
		fprintf( stderr, "Processing [%s]..\n", argv[i]);
		
		if (option == LINEAR_SEARCH)
		{
			// 연도별 입력 파일(이름 정보)을 구조체에 저장
			// 선형탐색 모드
			load_names_lsearch( fp, year-start_year, names);
		
		}
		else // (option == BINARY_SEARCH)
		{
			// 이진탐색 모드
			load_names_bsearch( fp, year-start_year, names);
			
			// 정렬 (이름순 (이름이 같은 경우 성별순))
			qsort( names->data, names->len, sizeof(tName), compare);
		}
		fclose( fp);

	}
	
	// 정렬 (이름순 (이름이 같은 경우 성별순))
	qsort( names->data, names->len, sizeof(tName), compare);
	
	// 이름 구조체를 화면에 출력
	print_names( names, num_year);

	// 이름 구조체 해제
	destroy_names( names);
	
	return 0;
}



