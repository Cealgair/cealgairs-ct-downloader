#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <dirent.h>

#define WGET_STRING_MAXLEN 300
#define IN_STRING_MAXLEN 200
#define NAME_STRING_MAXLEN 150
#define FILENAME_STRING_MAXLEN 164
#define SEARCH_RESULTS_FILE ".search-results/search-results"
#define SEARCH_RESULTS_DIR ".search-results/"
#define OLD_DECISIONS_DIR "old-decisions/"
#define WBZ_FILES_DIR "wbz-files/" 
#define SZS_FILES_DIR "szs-files/"
#define INFO_FILES_DIR "info-files/"
#define OVERRIDE_FILES_DIR "override-files/"

typedef enum{TRUE,FALSE}BOOL;
typedef enum{BAD,BOOST,FAIL,FREEZE,INCOME,INVALID,SELECT,STOCK}TRACK_CLASS;
typedef enum{PARSE_INPUT,SLEEP,DL_WBZ,DL_INFO,CONVERT_SZS,USER_DECISION,W_USER_DECISION,DONE}BRAIN_EXIT;
typedef struct{
	char music[200],menuname[200],extra[200];
}OVERRIDE_INFO;
typedef struct{
	unsigned int id,fam,clan;
	char name[NAME_STRING_MAXLEN];
	TRACK_CLASS class;
	BOOL dl;
	OVERRIDE_INFO o;
}TRACK;
typedef struct TRACK_NODE{
	TRACK t;
	struct TRACK_NODE *next, *prev;
}TRACK_LIST;
typedef enum{
	NO_RESULTS,
	NO_100_RESULTS,
	NO_VALID_RESULTS,
	NO_GOOD_RESULTS,
	SINGLE_GOOD,
	SINGLE_SELECT,
	M_MULTIPLE_GOOD,
	M_SINGLE_SELECT,
	M_MULTIPLE_SELECT
}SEARCH_RESULTS_STATUS;

void check_directory(char s[]);

BRAIN_EXIT brain(BOOL is_input_done,time_t schedule,unsigned int dl,unsigned int szs,unsigned int info,unsigned int on_hold);
BOOL parse_input_file(FILE *in_list,TRACK_LIST *szs_queue,TRACK_LIST *info_queue,TRACK_LIST *dl_queue,TRACK_LIST *final_track_list,unsigned int *on_hold,OVERRIDE_INFO *override,BOOL *force_decision);

void initialize_override(OVERRIDE_INFO *override);
void initialize_track_list(TRACK_LIST *a);
void free_track_list(TRACK_LIST *a);
void add_track_list_new(TRACK_LIST *dest,TRACK_LIST *new,int n);
void merge_track_lists(TRACK_LIST *dest,TRACK_LIST *new);
TRACK_LIST *list_of_all(TRACK_LIST *l1,TRACK_LIST *l2,TRACK_LIST *l3);

void remove_line_feed(char s[]);
void normalize_in_string(char a[]);
void download_search_results(char a[]);
SEARCH_RESULTS_STATUS polish_search_results(TRACK_LIST *pol);
BOOL search_in_file(FILE *f,char string[]);
void print_search_status(SEARCH_RESULTS_STATUS status);

void print_track_list(TRACK_LIST *list, BOOL print_dl);
void print_track(TRACK t, BOOL print_dl);
void print_id_number(unsigned int n);

void make_decision(SEARCH_RESULTS_STATUS status,TRACK_LIST *results,TRACK_LIST *ids,TRACK_LIST *dl_queue,TRACK_LIST *szs_queue,TRACK_LIST *info_queue, unsigned int *on_hold,OVERRIDE_INFO override,BOOL *force_decision);
void find_single(BOOL v,TRACK_LIST *results,TRACK_LIST *ids,TRACK_LIST *dl,TRACK_LIST *szs,TRACK_LIST *info,OVERRIDE_INFO override);
BOOL find_id(unsigned int id,TRACK_LIST *results,TRACK_LIST *ids,TRACK_LIST *dl,TRACK_LIST *szs,TRACK_LIST *info,OVERRIDE_INFO override);
void check_if_downloaded(TRACK_LIST *results,TRACK_LIST *new,TRACK_LIST *ninfo,TRACK_LIST *ids,TRACK_LIST *dl,TRACK_LIST *szs,TRACK_LIST *info,OVERRIDE_INFO override);
void maybe_put_decision_on_hold(TRACK_LIST *results,TRACK_LIST *ids,TRACK_LIST *dl_list,TRACK_LIST *szs_list,TRACK_LIST *info_list,unsigned int *on_hold,OVERRIDE_INFO override);
void put_decision_on_hold(unsigned int *on_hold,OVERRIDE_INFO override);
void user_decision(TRACK_LIST *ids,TRACK_LIST *dl,TRACK_LIST *szs,TRACK_LIST *info, unsigned int *on_hold,BOOL wait,unsigned long int wait_amount);
void save_decision(TRACK_LIST *results,TRACK_LIST *selected,BOOL confirmed);
void get_on_hold_filename(unsigned int n,char file[]);
void id_list_to_string(char s[],TRACK_LIST *list);
BOOL yes_or_no();

void clear_search_results();

void download_wbz(TRACK t,time_t schedule[]);
void wbz_downloaded(TRACK_LIST **dl,TRACK_LIST *fin,TRACK_LIST *szs);
void szs_downloaded(TRACK_LIST **szs,TRACK_LIST *fin);
void download_info(TRACK t);
void polish_info(char filename[]);

BOOL is_user_there(unsigned long int t);

void print_override(OVERRIDE_INFO override,char s[]);
OVERRIDE_INFO scan_override(char s[]);
void parse_distro_setting(char c[],OVERRIDE_INFO *override,BOOL asterisk,BOOL *force_decision);
BOOL is_override_blank(OVERRIDE_INFO override);
void copy_override(OVERRIDE_INFO *to,OVERRIDE_INFO *from);
void strcpy_noprefix(char to[],char from[],char prefix[],BOOL asterisk);

BOOL string_begins_with(char a[],char b[],BOOL asterisk);
void print_unneeded_files(TRACK_LIST *list);

int main(){
	FILE *in_list;
	BRAIN_EXIT exit=PARSE_INPUT;
	TRACK_LIST *dl_queue=(TRACK_LIST *) malloc(sizeof(TRACK_LIST));
	initialize_track_list(dl_queue);
	TRACK_LIST *szs_queue=(TRACK_LIST *) malloc(sizeof(TRACK_LIST));
	initialize_track_list(szs_queue);
	TRACK_LIST *info_queue=(TRACK_LIST *) malloc(sizeof(TRACK_LIST));
	initialize_track_list(info_queue);
	TRACK_LIST *final_track_list=(TRACK_LIST *) malloc(sizeof(TRACK_LIST));
	initialize_track_list(final_track_list);
	unsigned int on_hold;
	BOOL is_input_done=FALSE,force_decision=FALSE;
	time_t schedule[20],now;
	OVERRIDE_INFO override;
	initialize_override(&override);
	for(on_hold=0;on_hold<20;on_hold++) schedule[on_hold]=0;
	on_hold=0;
	char command[250];
	
	printf("\n @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
	printf(" @                                                                  @\n");
	printf(" @                     CEALGAIR'S CT DOWNLOADER                     @\n");
	printf(" @                          v. beta 2.2-1                           @\n");
	printf(" @                      powered by ct.wiimm.de                      @\n");
	printf(" @                    port to Windows by Fynn93                     @\n");
	printf(" @                to report bugs, typos and feedback,               @\n");
	printf(" @         follow the instructions at the end of the readme         @\n");
	printf(" @                                                                  @\n");
	printf(" @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n\n");
	
	in_list=fopen("input.txt","r");
	//if input.txt does not exist, well...
	if(in_list==NULL){
		printf("[!] input.txt not found, aborting\n");
		return 0;
	}
	//create various directories if they don't exist, just to be safe
	check_directory(SEARCH_RESULTS_DIR);
	check_directory(OLD_DECISIONS_DIR);
	check_directory(WBZ_FILES_DIR);
	check_directory(SZS_FILES_DIR);
	check_directory(INFO_FILES_DIR);
	check_directory(OVERRIDE_FILES_DIR);
	
	do{
		exit=brain(is_input_done,schedule[0],dl_queue->t.id,szs_queue->t.id,info_queue->t.id,on_hold);
		switch(exit){
			case PARSE_INPUT:
				is_input_done=parse_input_file(in_list,szs_queue,info_queue,dl_queue,final_track_list,&on_hold,&override,&force_decision);
				break;
			case DL_WBZ:
				while(dl_queue->prev!=NULL) dl_queue=dl_queue->prev;
				download_wbz(dl_queue->t,schedule);
				wbz_downloaded(&dl_queue,final_track_list,szs_queue);
				break;
			case USER_DECISION:
				//don't wait
				user_decision(final_track_list,dl_queue,szs_queue,info_queue,&on_hold,FALSE,0);
				break;
			case W_USER_DECISION:
				//wait
				time(&now);
				user_decision(final_track_list,dl_queue,szs_queue,info_queue,&on_hold,TRUE,schedule[0]-now);
				break;
			case DL_INFO:
				while(info_queue->prev!=NULL) info_queue=info_queue->prev;
				download_info(info_queue->t);
				if(info_queue->next==NULL) initialize_track_list(info_queue);
				else{
					info_queue=info_queue->next;
					free(info_queue->prev);
					info_queue->prev=NULL;
				}
				break;
			case SLEEP:
				printf("\nThe next download is scheduled at %s\n",ctime(&schedule[0]));
				time(&now);
				sleep(schedule[0]-now);
				break;
			case DONE:
				printf("\nDone downloading\n");
		}
	}while(exit!=DONE);
	
	//some cleanup
	clear_search_results();
	
	if(final_track_list->t.id==0){
		printf("\nThe final track list is empty\n");
		return 0;
	}
	printf("\nThis is the final track list\nThe tracks marked with [!] are the newly downloaded tracks\n\n");
	print_track_list(final_track_list,TRUE);
	
	//make track list for the builder
	in_list=fopen("filename-list.txt","w");
	while(final_track_list->prev!=NULL) final_track_list=final_track_list->prev;
	while(final_track_list->next!=NULL){
		if((final_track_list->t.name[strlen(final_track_list->t.name)-1]!='d') || (final_track_list->t.name[strlen(final_track_list->t.name)-2]!='_')) fprintf(in_list,"%s\n",final_track_list->t.name);
		final_track_list=final_track_list->next;
	}
	if((final_track_list->t.name[strlen(final_track_list->t.name)-1]!='d') || (final_track_list->t.name[strlen(final_track_list->t.name)-2]!='_')) fprintf(in_list,"%s\n",final_track_list->t.name);
	fclose(in_list);
	
	//make override files for the builder
	while(final_track_list->prev!=NULL) final_track_list=final_track_list->prev;
	while(final_track_list->next!=NULL){
		sprintf(command,"%s%s.txt",OVERRIDE_FILES_DIR,final_track_list->t.name);
		print_override(final_track_list->t.o,command);
		final_track_list=final_track_list->next;
	}
	sprintf(command,"%s%s.txt",OVERRIDE_FILES_DIR,final_track_list->t.name);
	print_override(final_track_list->t.o,command);
	
	//call the builder
	
	return 0;
}

void check_directory(char s[]){
	//definitely different on Windows
	char command[200];
	sprintf(command,"[ ! -d \"%s\" ] && mkdir \"%s\"",s,s);
	system(command);
	return;
}

BRAIN_EXIT brain(BOOL is_input_done,time_t schedule,unsigned int dl,unsigned int szs,unsigned int info,unsigned int on_hold){
	time_t now;
	time(&now);
	if(dl!=0) if(now>=schedule) return DL_WBZ;
	if(is_input_done==FALSE) return PARSE_INPUT;
	if(info!=0) return DL_INFO;
	if(szs!=0) return CONVERT_SZS;
	if(on_hold!=0){
		if(dl!=0) return W_USER_DECISION;
		return USER_DECISION;
	}
	if(dl!=0){
		return SLEEP;
	}
	return DONE;
}

BOOL parse_input_file(FILE *in_list,TRACK_LIST *szs_queue,TRACK_LIST *info_queue,TRACK_LIST *dl_queue,TRACK_LIST *final_track_list,unsigned int *on_hold,OVERRIDE_INFO *override,BOOL *force_decision){
	FILE *log;
	SEARCH_RESULTS_STATUS search_status;
	TRACK_LIST *results=(TRACK_LIST *) malloc(sizeof(TRACK_LIST));
	initialize_track_list(results);
	char in_string[IN_STRING_MAXLEN],polished_in_string[IN_STRING_MAXLEN];
	while(!feof(in_list)){
		//get input line
		fgets(in_string,IN_STRING_MAXLEN,in_list);
		remove_line_feed(in_string);
		//check if line is empty
		if(strlen(in_string)==0){}
		//check if line is setting
		else if(in_string[0]=='*'){
			//check if setting is empty
			if(strlen(in_string)>1){
				//check if it is distro setting or program setting
				if(in_string[1]=='*'){
					//it is program setting
					//do stuff and
					return FALSE;
				}
				else{
					//it is about the following track
					parse_distro_setting(in_string,override,TRUE,force_decision);
					return FALSE;
				}
			}
		}
		//check if line is beginning of multi-line comment
		else if(strcmp(in_string,"##")==0){
			while(!feof(in_list)){
				fgets(in_string,IN_STRING_MAXLEN,in_list);
				remove_line_feed(in_string);
				if(strcmp(in_string,"###")==0) return FALSE;
			}
			printf("Done searching\n");
			return TRUE;
		}
		//check if line is comment
		else if(in_string[0]=='#') return FALSE;
		else{
			//polish the input string
			strcpy(polished_in_string,in_string);
			normalize_in_string(polished_in_string);
			printf("Searching results for %s...\n",in_string);
			//download the search results
			download_search_results(polished_in_string);
			//polish search results
			//returns the search status and puts the results in the list results
			search_status=polish_search_results(results);
			//print search results
			print_search_status(search_status);
			//decide what tracks to download. If decision is not made, increase on_hold
			make_decision(search_status,results,final_track_list,dl_queue,szs_queue,info_queue,on_hold,*override,force_decision);
			//free the track list
			free_track_list(results);
			//reset override
			initialize_override(override);
			return FALSE;
		}
	}
	fclose(in_list);
	printf("Done searching\n");
	return TRUE;
}

void free_track_list(TRACK_LIST *a){
	if(a->next!=NULL){
		free_track_list(a->next);
		free(a->next);
	}
	initialize_track_list(a);
	return;
}

void initialize_override(OVERRIDE_INFO *override){
	unsigned int i;
	for(i=0;i<200;i++){
		override->music[i]=0;
		override->menuname[i]=0;
		override->extra[i]=0;
	}
	return;
}

void initialize_track_list(TRACK_LIST *a){
	int i;
	for(i=0;i<NAME_STRING_MAXLEN;i++) a->t.name[i]=0;
	a->t.id=0;
	a->t.fam=0;
	a->t.clan=0;
	a->t.class=INVALID;
	a->t.dl=FALSE;
	a->next=NULL;
	a->prev=NULL;
	initialize_override(&(a->t.o));
	return;
}

void add_track_list_new(TRACK_LIST *dest,TRACK_LIST *new,int n){
	//n=0 means first iteration
	//n=1 means we came from next
	//n=-1 means we came from prev
	if((new->t.id)==(dest->t.id)){
		free(new);
		new=dest;
	}
	else if((dest->t.id)==0){
		dest->t=new->t;
		free(new);
		new=dest;
	}
	else if((new->t.id)>(dest->t.id)){
		if(n==-1){
			new->next=dest;
			new->prev=dest->prev;
			new->prev->next=new;
			dest->prev=new;
		}
		else{
			if(dest->prev!=NULL) add_track_list_new(dest->prev,new,1);
			else{
				new->next=dest;
				dest->prev=new;
			}
		}
	}
	else if(new->t.id!=0){
		if(n==1){
			new->prev=dest;
			new->next=dest->next;
			new->next->prev=new;
			dest->next=new;
		}
		else{
			if(dest->next!=NULL) add_track_list_new(dest->next,new,-1);
			else{
				new->prev=dest;
				dest->next=new;
			}
		}
	}
	return;
}

void merge_track_lists(TRACK_LIST *dest,TRACK_LIST *new){
	while(new->prev!=NULL) new=new->prev;
	if(new->t.id==0){
		/*
		free(new);
		new=dest;
		*/
	}
	else if(new->next==NULL){
		add_track_list_new(dest,new,0);
	}
	else{
		new=new->next;
		new->prev->next=NULL;
		add_track_list_new(dest,new->prev,0);
		new->prev=NULL;
		merge_track_lists(dest,new);
	}
	return;
}

TRACK_LIST *list_of_all(TRACK_LIST *l1,TRACK_LIST *l2,TRACK_LIST *l3){
	TRACK_LIST *all=(TRACK_LIST *) malloc(sizeof(TRACK_LIST)),*new;
	initialize_track_list(all);
	while(l1->prev!=NULL) l1=l1->prev;
	while(l1->next!=NULL){
		new=(TRACK_LIST *) malloc(sizeof(TRACK_LIST));
		*new=*l1;
		new->prev=NULL;
		new->next=NULL;
		add_track_list_new(all,new,0);
		l1=l1->next;
	}
	new=(TRACK_LIST *) malloc(sizeof(TRACK_LIST));
	*new=*l1;
	new->prev=NULL;
	new->next=NULL;
	add_track_list_new(all,new,0);
	
	while(l2->prev!=NULL) l2=l2->prev;
	while(l2->next!=NULL){
		new=(TRACK_LIST *) malloc(sizeof(TRACK_LIST));
		*new=*l2;
		new->prev=NULL;
		new->next=NULL;
		add_track_list_new(all,new,0);
		l2=l2->next;
	}
	new=(TRACK_LIST *) malloc(sizeof(TRACK_LIST));
	*new=*l2;
	new->prev=NULL;
	new->next=NULL;
	add_track_list_new(all,new,0);
	
	while(l3->prev!=NULL) l3=l3->prev;
	while(l3->next!=NULL){
		new=(TRACK_LIST *) malloc(sizeof(TRACK_LIST));
		*new=*l3;
		new->prev=NULL;
		new->next=NULL;
		add_track_list_new(all,new,0);
		l3=l3->next;
	}
	new=(TRACK_LIST *) malloc(sizeof(TRACK_LIST));
	*new=*l3;
	new->prev=NULL;
	new->next=NULL;
	add_track_list_new(all,new,0);
	return all;
}

void remove_line_feed(char s[]){
	//remove final line feed
	//this might be different on Windows due to the carriage return thing
	if(s[strlen(s)-1]==10) s[strlen(s)-1]=0;
}

void normalize_in_string(char a[]){
	//all lowercase, %25 instead of %, _ instead of space, remove some naughty symbols
	unsigned int n,i,j;
	n=strlen(a);//this line might be different on Windows
	for(i=0;i<n;i++){
		if(a[i]==' ') a[i]='_';
		else if(a[i]>='A'&&a[i]<='Z') a[i]+='a'-'A';
		else if(a[i]=='%'){
			n+=2;
			if(n>=IN_STRING_MAXLEN) a[IN_STRING_MAXLEN-1]=0;
			else a[IN_STRING_MAXLEN-1]=a[IN_STRING_MAXLEN-3];
			for(j=IN_STRING_MAXLEN-4;j>i;j--) a[j+2]=a[j];
			i++;
			a[i]='2';
			i++;
			a[i]='5';
		}
		else if(a[i]=='$' || a[i]=='\\' || a[i]=='/' || a[i]=='*' || a[i]=='&' || a[i]=='=' || (a[i]>0&&a[i]<32)){
			for(j=i;j<n;j++) a[j]=a[j+1];
			n-=1;
			i--;
		}
	}
	return;
}

void download_search_results(char a[]){
	//download the search results
	//definitely different on Windows
	char wget_string[WGET_STRING_MAXLEN];
	sprintf(wget_string,".\\bin\\wget -O \"%s\" --output-file=.garbage \"https://ct.wiimm.de/?t=1&s=%s\"",SEARCH_RESULTS_FILE,a);
	sleep(3);//we don't want to overload the server
	system(wget_string);
	system("rm .garbage");
	return;
}

SEARCH_RESULTS_STATUS polish_search_results(TRACK_LIST *pol){
	FILE *res;
	TRACK_LIST *new;
	BOOL flag,flag2;
	unsigned int goods=0, selects=0,i,j;
	char in,buff[9];
	res=fopen(SEARCH_RESULTS_FILE,"r");
	flag=search_in_file(res,"<div id=list");
	//if no results, return
	if(flag==FALSE){
		fclose(res);
		return NO_RESULTS;
	}
	//else, move on
	flag=search_in_file(res,"tbody");
	flag=search_in_file(res,"align=center>100%");
	//if no 100% results, return
	if(flag==FALSE){
		fclose(res);
		return NO_100_RESULTS;
	}
	//else, move on
	while(flag==TRUE){
		//create new item
		new=NULL;
		new=(TRACK_LIST *) malloc(sizeof(TRACK_LIST));
		initialize_track_list(new);
		//if flag2 ever becomes FALSE we're gonna ignore this track
		flag2=TRUE;
		//get id
		flag=search_in_file(res,"align=center>");
		do{
			in=fgetc(res);
		}while(in<'0'||in>'9');
		while(in>='0'&&in<='9'){
			new->t.id*=10;
			new->t.id+=(int)(in-'0');
			in=fgetc(res);
		}
		//get family
		flag=search_in_file(res,"align=center");
		flag=search_in_file(res,">");
		do{
			in=fgetc(res);
		}while(in<'0'||in>'9');
		while(in>='0'&&in<='9'){
			new->t.fam*=10;
			new->t.fam+=(int)(in-'0');
			in=fgetc(res);
		}
		//get clan
		flag=search_in_file(res,"align=center");
		in=fgetc(res);
		if(in!='>'){
			flag=search_in_file(res,"class=block>");
			in=fgetc(res);
			if(in<'0'||in>'9') new->t.clan=0;
			else while(in!='<'){
				new->t.clan*=10;
				new->t.clan+=(int)(in-'0');
				in=fgetc(res);
			}
		}
		//get class
		flag=search_in_file(res,"\">");
		in=fgetc(res);
		i=0;
		while(in!='<'){
			buff[i]=in;
			in=fgetc(res);
			i++;
		}
		buff[i]=0;
		if(strcmp(buff,"select")==0){
			goods++;
			selects++;
			new->t.class=SELECT;
		}
		else if(strcmp(buff,"stock")==0){
			goods++;
			new->t.class=STOCK;
		}
		else if(strcmp(buff,"boost")==0){
			goods++;
			new->t.class=BOOST;
		}
		else if(strcmp(buff,"bad")==0) new->t.class=BAD;
		else if(strcmp(buff,"fail")==0) new->t.class=FAIL;
		else if(strcmp(buff,"freeze")==0) new->t.class=FREEZE;
		else if(strcmp(buff,"invalid")==0) new->t.class=INVALID;
		else if(strcmp(buff,"income")==0) new->t.class=INCOME;
		//at this point it's either nintendo or private. In any case we're gonna ignore it
		else flag2=FALSE;
		//ignore date and get name
		flag=search_in_file(res,"<td");
		flag=search_in_file(res,"<td");
		flag=search_in_file(res,">");
		in=fgetc(res);
		i=0;
		while(in!='<'){
			new->t.name[i]=in;
			in=fgetc(res);
			i++;
		}
		new->t.name[i]=0;
		//check if file is nodl
		for(j=i-4;j>0;j--) if((new->t.name[j])=='n') if((new->t.name[j+1])=='o') if((new->t.name[j+2])=='d') if((new->t.name[j+3])=='l') flag2=FALSE;
		//if flag2 is still TRUE check if it has a _d file and then add to list
		if(flag2==TRUE){
			add_track_list_new(pol,new,0);
		}
		else{
			switch(new->t.class){
				case SELECT:
					selects--;
					goods--;
					break;
				case STOCK:
					goods--;
					break;
				case BOOST:
					goods--;
					break;
				default:
					break;
			}
			free(new);
		}
		//check if there are more results before starting over
		flag=search_in_file(res,"align=center>100%");
	}
	fclose(res);
	if(pol->t.id==0) return NO_VALID_RESULTS;
	if(goods==0) return NO_GOOD_RESULTS;
	if(goods==1){
		if(selects==0) return SINGLE_GOOD;
		return SINGLE_SELECT;
	}
	if(selects==1) return M_SINGLE_SELECT;
	if(selects>1) return M_MULTIPLE_SELECT;
	return M_MULTIPLE_GOOD;
}

BOOL search_in_file(FILE *f,char string[]){
	//if string is not found, returns FALSE and puts cursor to end of file
	//if string is found, returns TRUE and puts cursor to end of string
	char buff[30];
	unsigned int i,len=strlen(string);
	for(i=0;i<len;i++) buff[i]=fgetc(f);
	buff[len]='\0';
	while(!feof(f)){
		//test if string was found
		if(0==strcmp(buff,string)) return TRUE; 
		//get new character and move on
		for(i=0;i<len-1;i++) buff[i]=buff[i+1];
		buff[len-1]=fgetc(f);
	}
	return FALSE;
}

void print_search_status(SEARCH_RESULTS_STATUS status){
	switch(status){
		case NO_RESULTS:
			printf("No results found\nTry searching manually on ct.wiimm.de\n");
			return;
			break;
		case NO_100_RESULTS:
			printf("No perfect matches found\nTry searching manually on ct.wiimm.de\n");
			return;
		case NO_VALID_RESULTS:
			printf("No valid perfect matches found\nThis means that none of those is downloadable\nTry searching manually on ct.wiimm.de\n");
			return;
			break;
		case NO_GOOD_RESULTS:
			printf("No good results found\n\n");
			break;
		case SINGLE_GOOD:
			printf("One good result found\n\n");
			break;
		case SINGLE_SELECT:
			printf("One select result found\n\n");
			break;
		case M_MULTIPLE_GOOD:
			printf("Multiple good results found\n\n");
			break;
		case M_SINGLE_SELECT:
			printf("Multiple good results found, only one select\n\n");
			break;
		default:
		case M_MULTIPLE_SELECT:
			printf("Multiple select results found\n\n");
			break;
	}
	return;
}

void print_track_list(TRACK_LIST *list, BOOL print_dl){
	TRACK_LIST *cursor=list;
	if(print_dl==TRUE) printf("    ");
	printf("id    fam.  clan  class  name\n");
	while(cursor->prev!=NULL){
		cursor=cursor->prev;
	}
	while(cursor->next!=NULL){
		print_track(cursor->t, print_dl);
		cursor=cursor->next;
	}
	print_track(cursor->t, print_dl);
	
	if(print_dl==FALSE) return;
	
	return;
}

void print_track(TRACK t, BOOL print_dl){
	if(print_dl==TRUE){
		if(t.dl==TRUE) printf("[!] ");
		else printf("    ");
	}
	print_id_number(t.id);
	print_id_number(t.fam);
	print_id_number(t.clan);
	switch(t.class){
		case STOCK:
			printf("stock  ");
			break;
		case SELECT:
			printf("select ");
			break;
		case INCOME:
			printf("income ");
			break;
		case BAD:
			printf("bad    ");
			break;
		case FAIL:
			printf("fail   ");
			break;
		case FREEZE:
			printf("freeze ");
			break;
		case INVALID:
			printf("inv.   ");
			break;
		case BOOST:
			printf("boost  ");
			break;
	}
	printf("%s\n",t.name);
	return;
}

void print_id_number(unsigned int n){
	if(n==0) printf("-     ");
	else{
		printf("%u ",n);
		if(n<10) printf(" ");
		if(n<100) printf(" ");
		if(n<1000) printf(" ");
		if(n<10000) printf(" ");
	}
}

void make_decision(SEARCH_RESULTS_STATUS status,TRACK_LIST *results,TRACK_LIST *ids,TRACK_LIST *dl_queue,TRACK_LIST *szs_queue,TRACK_LIST *info_queue, unsigned int *on_hold,OVERRIDE_INFO override,BOOL *force_decision){
	//returns TRUE if decision is made, FALSE otherwise
	switch(status){
		case NO_RESULTS:
			//no decision to be made
			break;
		case NO_100_RESULTS:
			//no decision to be made
			break;
		case NO_VALID_RESULTS:
			//no decision to be made
			break;
		case NO_GOOD_RESULTS:
			//user decision required
			maybe_put_decision_on_hold(results,ids,dl_queue,szs_queue,info_queue,on_hold,override);
			break;
		case SINGLE_GOOD:
			//look for it and add it to ids
			find_single(FALSE,results,ids,dl_queue,szs_queue,info_queue,override);
			break;
		case SINGLE_SELECT:
			//look for it and add it to ids
			find_single(TRUE,results,ids,dl_queue,szs_queue,info_queue,override);
			break;
		case M_MULTIPLE_GOOD:
			//user decision required
			maybe_put_decision_on_hold(results,ids,dl_queue,szs_queue,info_queue,on_hold,override);
			break;
		case M_SINGLE_SELECT:
			//look for it and add it to ids
			if(*force_decision==FALSE) find_single(TRUE,results,ids,dl_queue,szs_queue,info_queue,override);
			else maybe_put_decision_on_hold(results,ids,dl_queue,szs_queue,info_queue,on_hold,override);
			break;
		case M_MULTIPLE_SELECT:
			//user decision required
			maybe_put_decision_on_hold(results,ids,dl_queue,szs_queue,info_queue,on_hold,override);
			break;
	}
	*force_decision=FALSE;
	return;
}

void find_single(BOOL v,TRACK_LIST *results,TRACK_LIST *ids,TRACK_LIST *dl,TRACK_LIST *szs,TRACK_LIST *info,OVERRIDE_INFO override){
	TRACK_LIST *new=(TRACK_LIST *) malloc(sizeof(TRACK_LIST));
	TRACK_LIST *ninfo=(TRACK_LIST *) malloc(sizeof(TRACK_LIST));
	while(results->prev!=NULL) results=results->prev;
	while(0==0){
		if((results->t.class==SELECT) || (v==FALSE && (results->t.class==STOCK || results->t.class==BOOST))){
			//check if we have already downloaded this file
			//this also calls add_track_list_new
			check_if_downloaded(results,new,ninfo,ids,dl,szs,info,override);
			return;
		}
		results=results->next;
	}
}

BOOL find_id(unsigned int id,TRACK_LIST *results,TRACK_LIST *ids,TRACK_LIST *dl,TRACK_LIST *szs,TRACK_LIST *info,OVERRIDE_INFO override){
	TRACK_LIST *new=(TRACK_LIST *) malloc(sizeof(TRACK_LIST));
	TRACK_LIST *ninfo=(TRACK_LIST *) malloc(sizeof(TRACK_LIST));
	while(results->prev!=NULL) results=results->prev;
	while(results->next!=NULL){
		if(results->t.id==id){			
			//check if we have already downloaded this file
			//this also calls add_track_list_new
			check_if_downloaded(results,new,ninfo,ids,dl,szs,info,override);
			return TRUE;
		}
		results=results->next;
	}
	if(results->t.id==id){
		//check if we have already downloaded this file
		//this also calls add_track_list_new
		check_if_downloaded(results,new,ninfo,ids,dl,szs,info,override);
		return TRUE;
	}
	return FALSE;
}

void check_if_downloaded(TRACK_LIST *results,TRACK_LIST *new,TRACK_LIST *ninfo,TRACK_LIST *ids,TRACK_LIST *dl,TRACK_LIST *szs,TRACK_LIST *info,OVERRIDE_INFO override){
	char filename[200];
	*new=*results;
	new->next=NULL;
	new->prev=NULL;
	copy_override(&(new->t.o),&override);
	*ninfo=*new;
	sprintf(filename,"%s%s.txt",INFO_FILES_DIR,results->t.name);
	//returns -1 if file doesn't exist
	if(access(filename,F_OK)==-1){
		add_track_list_new(info,ninfo,0);
	}
	
	new->t.dl=FALSE;
	sprintf(filename,"%s%s.szs",SZS_FILES_DIR,results->t.name);
	if(access(filename,F_OK)==-1){
		sprintf(filename,"%s%s.wbz",WBZ_FILES_DIR,results->t.name);
		if(access(filename,F_OK)==-1){
			new->t.dl=TRUE;
			add_track_list_new(dl,new,0);
		}
		else{
			new->t.dl=FALSE;
			add_track_list_new(szs,new,0);
		}
	}
	else{
		new->t.dl=FALSE;
		add_track_list_new(ids,new,0);
	}
	return;
}

void maybe_put_decision_on_hold(TRACK_LIST *results,TRACK_LIST *ids,TRACK_LIST *dl_list,TRACK_LIST *szs_list,TRACK_LIST *info_list,unsigned int *on_hold,OVERRIDE_INFO override){
	unsigned int id;
	char command[500];
	FILE *stored;
	TRACK_LIST *selected=(TRACK_LIST *) malloc(sizeof(TRACK_LIST));
	initialize_track_list(selected);
	TRACK_LIST *dl_selected=(TRACK_LIST *) malloc(sizeof(TRACK_LIST));
	initialize_track_list(dl_selected);
	while(results->prev!=NULL) results=results->prev;
	id_list_to_string(command,results);
	if(access(command,F_OK)!=-1){
		stored=fopen(command,"r");
		fgets(command,100,stored);
		remove_line_feed(command);
		if(strcmp(command,"twice")==0){
			printf("You have already permanently confirmed this decision, so I won't ask you anything\n\n");
			fscanf(stored,"%u",&id);
			while(!feof(stored)){
				find_id(id,results,ids,dl_list,szs_list,info_list,override);
				fscanf(stored,"%u",&id);
			}
			fclose(stored);
			return;
		}
		fclose(stored);
	}
	put_decision_on_hold(on_hold,override);
	return;
}

void put_decision_on_hold(unsigned int *on_hold,OVERRIDE_INFO override){
	char command[100],file[30];
	*on_hold=*on_hold+1;
	//this part might be different on Windows
	get_on_hold_filename(*on_hold,file);
	sprintf(command,"cp %s %s",SEARCH_RESULTS_FILE,file);
	system(command);
	
	get_on_hold_filename(*on_hold,command);
	strcat(command,".override");
	print_override(override,command);
	return;
}

void user_decision(TRACK_LIST *ids,TRACK_LIST *dl,TRACK_LIST *szs,TRACK_LIST *info, unsigned int *on_hold, BOOL wait,unsigned long int wait_amount){
	char command[500],file[100];
	unsigned int i,len,id;
	BOOL flag,skip=FALSE;
	if(*on_hold==0) return;//nothing to do here
	
	printf("\n");
	if(*on_hold!=1) printf("There are %u decisions on hold\n",*on_hold);
	else printf("There is one decision on hold\n");
	
	//if needed check if user is there
	if(wait==TRUE){
		if(is_user_there(wait_amount)==FALSE) return;
	}
	
	//get the override info
	OVERRIDE_INFO override;
	get_on_hold_filename(*on_hold,command);
	strcat(command,".override");
	override=scan_override(command);
	
	//now we get the results in a TRACK_LIST *
	TRACK_LIST *results=(TRACK_LIST *) malloc(sizeof(TRACK_LIST));
	initialize_track_list(results);
	TRACK_LIST *selected=(TRACK_LIST *) malloc(sizeof(TRACK_LIST));
	initialize_track_list(selected);
	TRACK_LIST *dl_selected=(TRACK_LIST *) malloc(sizeof(TRACK_LIST));
	initialize_track_list(dl_selected);
	TRACK_LIST *szs_selected=(TRACK_LIST *) malloc(sizeof(TRACK_LIST));
	initialize_track_list(szs_selected);
	TRACK_LIST *info_selected=(TRACK_LIST *) malloc(sizeof(TRACK_LIST));
	initialize_track_list(info_selected);
	
	//this part might be different on Windows
	get_on_hold_filename(*on_hold,file);
	sprintf(command,"cp %s %s",file,SEARCH_RESULTS_FILE);
	system(command);
	
	polish_search_results(results);
	
	print_track_list(results, FALSE);
	
	//now we check if user has already made this decision
	FILE *stored;
	while(results->prev!=NULL) results=results->prev;
	id_list_to_string(command,results);
	flag=FALSE;
	if(access(command,F_OK)!=-1){
		stored=fopen(command,"r");
		fgets(file,100,stored);
		remove_line_feed(file);
		if(strcmp(file,"twice")==0){
			printf("\nYou have already permanently confirmed this decision, so I won't ask you anything\n");
			fscanf(stored,"%u",&id);
			while(!feof(stored)){
				find_id(id,results,selected,dl_selected,szs_selected,info_selected,override);
				fscanf(stored,"%u",&id);
			}
			fclose(stored);
			skip=TRUE;
		}
		else{
			//ask user for confirmation
			fscanf(stored,"%u",&id);
			while(!feof(stored)){
				find_id(id,results,selected,dl_selected,szs_selected,info_selected,override);
				fscanf(stored,"%u",&id);
			}
			printf("\nYou have already made this decision once. Here's what you selected:\n");
			print_track_list(list_of_all(dl_selected,szs_selected,selected), FALSE);
			printf("\nDo you confirm this choice?");
			if(yes_or_no()==TRUE){
				skip=TRUE;
				printf("\nDo you confirm that permanently?");
				if(yes_or_no()==TRUE){
					fclose(stored);
					save_decision(results,selected,TRUE);
				}
			}
			else{
				while(selected->prev!=NULL) selected=selected->prev;
				free_track_list(selected);
				selected=(TRACK_LIST *) malloc(sizeof(TRACK_LIST));
				initialize_track_list(selected);
				while(dl_selected->prev!=NULL) dl_selected=dl_selected->prev;
				free_track_list(dl_selected);
				dl_selected=(TRACK_LIST *) malloc(sizeof(TRACK_LIST));
				initialize_track_list(dl_selected);
				while(szs_selected->prev!=NULL) szs_selected=szs_selected->prev;
				free_track_list(szs_selected);
				szs_selected=(TRACK_LIST *) malloc(sizeof(TRACK_LIST));
				initialize_track_list(szs_selected);
				while(info_selected->prev!=NULL) info_selected=info_selected->prev;
				free_track_list(info_selected);
				info_selected=(TRACK_LIST *) malloc(sizeof(TRACK_LIST));
				initialize_track_list(info_selected);
				fclose(stored);
			}
		}
	}
	if(skip==FALSE){
		printf("\nInput help to get help\n");
		flag=FALSE;
		while(flag==FALSE){
			printf("> ");
			fgets(file, 100, stdin);//this line needs improvements
			remove_line_feed(file);
			
			if(strcmp(file,"help")==0) printf("Input an id from this list to add it to the download list\nInput results to see the results again\nInput show to see what you've added so far\nInput cancel to start over\nInput done when you are done\nInput DONE if you don't want to be asked for confirmation\n");
			else if(strcmp(file,"results")==0){
				print_track_list(results, FALSE);
			}
			else if(strcmp(file,"show")==0){
				if(selected->t.id==0) printf("You haven't added any tracks yet\n");
				else{
					printf("Here are the tracks you've added so far:\n");
					print_track_list(list_of_all(dl_selected,szs_selected,selected), FALSE);
				}
			}
			else if(strcmp(file,"cancel")==0){
				while(selected->prev!=NULL) selected=selected->prev;
				free_track_list(selected);
				selected=(TRACK_LIST *) malloc(sizeof(TRACK_LIST));
				initialize_track_list(selected);
				while(dl_selected->prev!=NULL) dl_selected=dl_selected->prev;
				free_track_list(dl_selected);
				dl_selected=(TRACK_LIST *) malloc(sizeof(TRACK_LIST));
				initialize_track_list(dl_selected);
				while(szs_selected->prev!=NULL) szs_selected=szs_selected->prev;
				free_track_list(szs_selected);
				szs_selected=(TRACK_LIST *) malloc(sizeof(TRACK_LIST));
				initialize_track_list(szs_selected);
				while(info_selected->prev!=NULL) info_selected=info_selected->prev;
				free_track_list(info_selected);
				info_selected=(TRACK_LIST *) malloc(sizeof(TRACK_LIST));
				initialize_track_list(info_selected);
			}
			else if(strcmp(file,"done")==0){
				printf("Here are the tracks you've added:\n");
				print_track_list(list_of_all(dl_selected,szs_selected,selected), FALSE);
				printf("\nIs this ok?");
				if(yes_or_no()==TRUE) flag=TRUE;
			}
			else if(strcmp(file,"DONE")==0) flag=TRUE;
			else{
				id=0;
				len=strlen(file);
				for(i=0;i<len;i++){
					if(file[i]>='0'&&file[i]<='9'){
						id=id*10;
						id+=(unsigned int)(file[i]-'0');
					}
					else flag=TRUE;
				}
				if(flag==TRUE){
					printf("Invalid input\n");
					flag=FALSE;
				}
				else if(find_id(id,results,selected,dl_selected,szs_selected,info_selected,override)==FALSE) printf("id is not in the list\n");
			}
		}
		save_decision(results,list_of_all(dl_selected,szs_selected,selected),FALSE);
	}
	//add the selected tracks to ids
	merge_track_lists(ids,selected);
	merge_track_lists(dl,dl_selected);
	merge_track_lists(szs,szs_selected);
	merge_track_lists(info,info_selected);
	//remove the search result on hold
	//this part might be different on Windows
	get_on_hold_filename(*on_hold,file);
	sprintf(command,"rm -f %s",file);
	system(command);
	strcat(command,".override");
	system(command);
	
	while(results->prev!=NULL) results=results->prev;
	free_track_list(results);
	
	*on_hold=*on_hold-1;
	//make next user decision
	printf("\n");
	//user_decision(ids,dl,on_hold);
	return;
}

void get_on_hold_filename(unsigned int n,char file[]){
	sprintf(file,"%s%u",SEARCH_RESULTS_DIR,n);
	return;
}

void save_decision(TRACK_LIST *results,TRACK_LIST *selected,BOOL confirmed){
	char filename[500];
	FILE *f;
	id_list_to_string(filename,results);
	f=fopen(filename,"w");
	if(confirmed==TRUE) fprintf(f,"twice\n");
	else fprintf(f,"once\n");
	while(selected->prev!=NULL) selected=selected->prev;
	while(selected->next!=NULL){
		fprintf(f,"%u\n",selected->t.id);
		selected=selected->next;
	}
	fprintf(f,"%u\n",selected->t.id);
	return;
}

void id_list_to_string(char s[],TRACK_LIST *list){
	char a[8];
	strcpy(s,OLD_DECISIONS_DIR);
	TRACK_LIST *cursor=list;
	while(cursor->next!=NULL){
		sprintf(a,"%u-",cursor->t.id);
		strcat(s,a);
		cursor=cursor->next;
	}
	sprintf(a,"%u",cursor->t.id);
	strcat(s,a);
	return;
}

BOOL yes_or_no(){
	char s[300];
	printf(" [yes/no]\n");
	while(0==0){
		printf("> ");
		fgets(s, 300, stdin);//this line needs improvements
		remove_line_feed(s);
		if(strcmp(s,"yes")==0) return TRUE;
		if(strcmp(s,"no")==0) return FALSE;
		printf("Invalid input\n");
	}
}

void clear_search_results(){
	//this might be different on Windows
	char command[40];
	sprintf(command,"rm -f %s",SEARCH_RESULTS_FILE);
	system(command);
	return;
}


void download_wbz(TRACK t,time_t schedule[]){
	//definitely different on Windows
	char wget_string[WGET_STRING_MAXLEN];
	int i;
	for(i=0;i<19;i++) schedule[i]=schedule[i+1];
	printf("Downloading %s.wbz...\n\n",t.name);
	sprintf(wget_string,".\\bin\\wget -O \"%s%s.wbz\" --output-file=.garbage \"https://ct.wiimm.de/d/@Y1kNEXdi/%u\"",WBZ_FILES_DIR,t.name,t.id);
	sleep(3);//we don't want to overload the server
	system(wget_string);
	system("rm .garbage");
	time(&schedule[19]);
	schedule[19]+=3600;
	return;
}

void wbz_downloaded(TRACK_LIST **dl,TRACK_LIST *fin,TRACK_LIST *szs){
	TRACK_LIST *new;
	BOOL flag=TRUE;
	char command[300];
	new=*dl;
	if((*dl)->next!=NULL){
		flag=FALSE;
		*dl=(*dl)->next;
		(*dl)->prev=NULL;
	}
	new->next=NULL;
	
	sprintf(command,"%s%s.szs",SZS_FILES_DIR,new->t.name);
	if(access(command,F_OK)==-1) add_track_list_new(szs,new,0);
	else add_track_list_new(fin,new,0);
	if(flag==TRUE){
		*dl=(TRACK_LIST *) malloc(sizeof(TRACK_LIST));
		initialize_track_list(*dl);
	}
	return;
}

void szs_downloaded(TRACK_LIST **szs,TRACK_LIST *fin){
	TRACK_LIST *new;
	BOOL flag=TRUE;
	new=*szs;
	if((*szs)->next!=NULL){
		flag=FALSE;
		*szs=(*szs)->next;
		(*szs)->prev=NULL;
	}
	new->next=NULL;
	
	add_track_list_new(fin,new,0);
	if(flag==TRUE){
		*szs=(TRACK_LIST *) malloc(sizeof(TRACK_LIST));
		initialize_track_list(*szs);
	}
	return;
}

void download_info(TRACK t){
	char command[250];
	BOOL flag=TRUE;
	//check is file is _d
	if(t.name[strlen(t.name)-1]=='d') if(t.name[strlen(t.name)-2]=='_') flag=FALSE;
	//check if we already have info
	if(flag==TRUE){
		sprintf(command,"%s%s.txt",INFO_FILES_DIR,t.name);
		//returns -1 if file doesn't exist
		if(access(command,F_OK)!=-1) flag=FALSE;
	}
	//if flag is FALSE we don't need anything else
	if(flag==TRUE){
		printf("\nDownloading info for %s...\n",t.name);
		//get the info
		//this part is probably different on Windows
		sprintf(command,".\\bin\\wget -O \"%s%s\" --output-file=.garbage \"https://ct.wiimm.de/ajax/get-info?f=%u\"",INFO_FILES_DIR,t.name,t.id);
		sleep(3);//we don't want to overload the server
		system(command);
		system("rm .garbage");
		//polish the info
		strcpy(command,INFO_FILES_DIR);
		strcat(command,t.name);
		polish_info(command);
	}
	return;
}

void polish_info(char filename[]){
	FILE *in,*out;
	unsigned int id;
	BOOL flag,version=TRUE;
	char command[250],buff;
	in=fopen(filename,"r");
	sprintf(command,"%s.txt",filename);
	out=fopen(command,"w");
	//get id
	flag=search_in_file(in,"Type, Class and Id:<\\/td><td>");
	flag=search_in_file(in,"\\/");
	flag=search_in_file(in,"\\/");
	fscanf(in,"%u",&id);
	fprintf(out,"id %u\n",id);
	//get prefix
	flag=search_in_file(in,"Console or prefix:<\\/td><td><b>");
	if(flag==TRUE){
		fprintf(out,"prefix ");
		while(flag==TRUE){
			fscanf(in,"%c",&buff);
			if(buff=='<') flag=FALSE;
			else fprintf(out,"%c",buff);
		}
		fprintf(out,"\n");
	}
	fclose(in);
	in=fopen(filename,"r");
	//get name and version
	fprintf(out,"name ");
	flag=search_in_file(in,"Type, Class and Id:<\\/td><td>"); //to get the right occurrence of "name"
	flag=search_in_file(in,"name");
	fscanf(in,"%c",&buff);
	if(buff==':') version=FALSE;
	flag=search_in_file(in,"<\\/td><td><b>");
	while(flag==TRUE){
		fscanf(in,"%c",&buff);
		if(buff=='<') flag=FALSE;
		else fprintf(out,"%c",buff);
	}
	if(version==TRUE){
		fprintf(out,"\nversion ");
		flag=search_in_file(in,"&emsp;");
		while(flag==TRUE){
			fscanf(in,"%c",&buff);
			if(buff=='<') flag=FALSE;
			else fprintf(out,"%c",buff);
		}
	}
	fprintf(out,"\n");
	//get extra info
	flag=search_in_file(in,"Extra info:<\\/td><td>");
	if(flag==TRUE){
		fprintf(out,"extra ");
		while(flag==TRUE){
			fscanf(in,"%c",&buff);
			if(buff=='<') flag=FALSE;
			else fprintf(out,"%c",buff);
		}
		fprintf(out,"\n");
	}
	fclose(in);
	in=fopen(filename,"r");
	//get author
	fprintf(out,"author ");
	flag=search_in_file(in,"Created by:<\\/td><td>");
	while(flag==TRUE){
		fscanf(in,"%c",&buff);
		if(buff=='<') flag=FALSE;
		else fprintf(out,"%c",buff);
	}
	fprintf(out,"\n");
	//get editor
	flag=search_in_file(in,"Edited by:<\\/td><td>");
	if(flag==TRUE){
		fprintf(out,"editor ");
		while(flag==TRUE){
			fscanf(in,"%c",&buff);
			if(buff=='<') flag=FALSE;
			else fprintf(out,"%c",buff);
		}
		fprintf(out,"\n");
	}
	fclose(in);
	in=fopen(filename,"r");
	//get slots
	flag=search_in_file(in,"Property slot");
	if(flag==TRUE){
		fprintf(out,"slot ");
		flag=search_in_file(in,"(");
		while(flag==TRUE){
			fscanf(in,"%c",&buff);
			if(buff==')') flag=FALSE;
			else fprintf(out,"%c",buff);
		}
		fprintf(out,"\nmusic ");
		flag=search_in_file(in,"Music index");
		flag=search_in_file(in,", ");
		while(flag==TRUE){
			fscanf(in,"%c",&buff);
			if(buff==')') flag=FALSE;
			else fprintf(out,"%c",buff);
		}
		fprintf(out,"\n");
	}
	fclose(in);
	//get sha1
	/*
	in=fopen(filename,"r");
	fprintf(out,"sha1 ");
	flag=search_in_file(in,"SHA1");
	flag=search_in_file(in,"<tt>");
	while(flag==TRUE){
		fscanf(in,"%c",&buff);
		if(buff=='<') flag=FALSE;
		else fprintf(out,"%c",buff);
	}
	fprintf(out,"\n");
	fclose(in);
	*/
	fclose(out);
	//this cleanup part might be different on Windows
	sprintf(command,"rm -f \"%s\"",filename);
	system(command);
	return;
}

BOOL is_user_there(unsigned long int t){
    struct timeval tmo;

    printf("Press enter if you are there\n");
    fflush(stdout);

    tmo.tv_sec = t;
    tmo.tv_usec = 0;
}

void print_override(OVERRIDE_INFO override,char s[]){
	FILE *f;
	char command[300];
	
	//this part might be different on Windows
	sprintf(command,"rm -f \"%s\"",s);
	system(command);
	
	//don't use disk space if not needed thanks
	if(is_override_blank(override)==TRUE) return;
	
	f=fopen(s,"w");
	
	if(strcmp(override.music,"")!=0) fprintf(f,"music %s\n",override.music);
	if(strcmp(override.menuname,"")!=0) fprintf(f,"menuname %s\n",override.menuname);
	if(strcmp(override.extra,"")!=0) fprintf(f,"extra %s\n",override.extra);
	
	fclose(f);
	return;
}

OVERRIDE_INFO scan_override(char s[]){
	FILE *f;
	char c[200];
	OVERRIDE_INFO o;
	initialize_override(&o);
	f=fopen(s,"r");
	if(f==NULL) return o;
	fgets(c,200,f);
	remove_line_feed(c);
	while(!feof(f)){
		parse_distro_setting(c,&o,FALSE,NULL);
		fgets(c,200,f);
		remove_line_feed(c);
	}
	fclose(f);
	return o;
}

void parse_distro_setting(char c[],OVERRIDE_INFO *o,BOOL asterisk,BOOL *force_decision){
	if(string_begins_with(c,"music ",asterisk)==TRUE){
		strcpy_noprefix(o->music,c,"music ",asterisk);
	}
	else if(string_begins_with(c,"menuname ",asterisk)==TRUE) {
		strcpy_noprefix(o->menuname,c,"menuname ",asterisk);
	}
	else if(string_begins_with(c,"extra ",asterisk)==TRUE) {
		strcpy_noprefix(o->extra,c,"extra ",asterisk);
	}
	else if(force_decision!=NULL){
		if(strcmp("*force decision",c)==0) *force_decision=TRUE;
	}
	return;
}

BOOL is_override_blank(OVERRIDE_INFO override){
	if(strcmp(override.music,"")!=0) return FALSE;
	if(strcmp(override.menuname,"")!=0) return FALSE;
	if(strcmp(override.extra,"")!=0) return FALSE;
	return TRUE;
}

void copy_override(OVERRIDE_INFO *to,OVERRIDE_INFO *from){
	strcpy(to->music,from->music);
	strcpy(to->menuname,from->menuname);
	strcpy(to->extra,from->extra);
	return;
}

void strcpy_noprefix(char to[],char from[],char prefix[],BOOL asterisk){
	unsigned int i,f=0;
	if(asterisk==TRUE) f=1;
	f+=strlen(prefix);
	for(i=0;from[i+f-1]!=0;i++) to[i]=from[i+f];
	return;
}

BOOL string_begins_with(char a[],char b[],BOOL asterisk){
	unsigned int i,f=0;
	if(asterisk==TRUE){
		if(a[0]!='*') return FALSE;
		f=1;
	}
	if(strlen(a)<strlen(b)+f) return FALSE;
	for(i=0;i<strlen(b);i++) if(a[i+f]!=b[i]) return FALSE;
	return TRUE;
}

void print_unneeded_files(TRACK_LIST *list){
	FILE *log;
	BOOL flag;
	DIR *d;
	char command[200];
	struct dirent *dir;
	
	//we can vandalize the list by adding ".szs" at the end of each name, we won't use it afterwards
	while(list->prev!=NULL) list=list->prev;
	strcat(list->t.name,".szs");
	while(list->next!=NULL){
		list=list->next;
		strcat(list->t.name,".szs");
	}
	
	log=fopen(".gorbage","w");
	d=opendir(SZS_FILES_DIR);
	if(d){
		while ((dir = readdir(d))!=NULL){
			flag=FALSE;
			while(list->prev!=NULL) list=list->prev;
			if(strcmp(dir->d_name,".")==0) flag=TRUE;
			if(strcmp(dir->d_name,"..")==0) flag=TRUE;
			if(strcmp(dir->d_name,list->t.name)==0) flag=TRUE;
			while(list->next!=NULL){
				list=list->next;
				if(strcmp(dir->d_name,list->t.name)==0) flag=TRUE;
			}
			if(flag==FALSE) fprintf(log,"%s\n", dir->d_name);
		}
		closedir(d);
	}
	fclose(log);
	
	//This is different on Windows
	sprintf(command,"sort .gorbage >> %s; rm .gorbage");
	system(command);
	
	//strlen might not work on Windows
	while(list->prev!=NULL) list=list->prev;
	list->t.name[strlen(list->t.name)-3]='t';
	list->t.name[strlen(list->t.name)-2]='x';
	list->t.name[strlen(list->t.name)-1]='t';
	while(list->next!=NULL){
		list=list->next;
		list->t.name[strlen(list->t.name)-3]='t';
		list->t.name[strlen(list->t.name)-2]='x';
		list->t.name[strlen(list->t.name)-1]='t';
	}
	
	log=fopen(".gorbage","w");
	d=opendir(INFO_FILES_DIR);
	if(d){
		while ((dir = readdir(d))!=NULL){
			flag=FALSE;
			while(list->prev!=NULL) list=list->prev;
			if(strcmp(dir->d_name,".")==0) flag=TRUE;
			if(strcmp(dir->d_name,"..")==0) flag=TRUE;
			if(strcmp(dir->d_name,list->t.name)==0) flag=TRUE;
			while(list->next!=NULL){
				list=list->next;
				if(strcmp(dir->d_name,list->t.name)==0) flag=TRUE;
			}
			if(flag==FALSE) fprintf(log,"%s\n", dir->d_name);
		}
		closedir(d);
	}
	fclose(log);
	//This is different on Windows
	sprintf(command,"sort .gorbage >> %s; rm .gorbage");
	system(command);
}