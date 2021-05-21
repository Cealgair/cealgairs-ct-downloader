#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum{TRUE,FALSE}BOOL;

typedef struct PREFIX_NODE{
	char s[50];
	unsigned int used;
	struct PREFIX_NODE *next, *prev;
}PREFIX;

typedef struct{
	char filename[200], name[200], version[30], music[30], slot[30], author[200], editor[200], extra[30], menuname[300];
	BOOL other;
	PREFIX *prefix;
}TRACK;

typedef struct TRACK_NODE{
	TRACK t;
	struct TRACK_NODE *next, *prev;
}TRACK_LIST;

void remove_line_feed(char s[]);
void initialize_track_list(TRACK_LIST *l);
void add_track_list_new(TRACK_LIST *dest,TRACK_LIST *new);
void initialize_prefix(PREFIX *l);
void add_prefix_new(PREFIX *dest,PREFIX **new);
void get_filenames_from_file(char file[],BOOL other,TRACK_LIST *l);
void get_info_from_dir(char dir[],TRACK_LIST *l,PREFIX *p);
void parse_info_string(char c[],TRACK_LIST *l,PREFIX *p);
char *szs_dir(BOOL other);
void figure_menunames(TRACK_LIST *l);
void figure_intronames(TRACK_LIST *l);
BOOL fix_menunames(TRACK_LIST *l,TRACK_LIST *aux);
BOOL string_begins_with(char a[],char b[]);
void strcpy_noprefix(char to[],char from[],char prefix[]);
void figure_info(TRACK_LIST *l);

void sort_by_menuname(TRACK_LIST *l);
BOOL first_comes_first(char s1[],char s2[]);

void make_ctfile(TRACK_LIST *l, char target[]);

int main(){
	TRACK_LIST *list=malloc(sizeof(TRACK_LIST));
	PREFIX *prefs=malloc(sizeof(PREFIX));
	initialize_track_list(list);
	initialize_prefix(prefs);
	
	printf("Parsing filename-list.txt\n");
	get_filenames_from_file("filename-list.txt",FALSE,list);
	printf("Parsing info-files/\n");
	get_info_from_dir("info-files/",list,prefs);
	printf("Parsing override-files/\n");
	get_info_from_dir("override-files/",list,prefs);
	printf("Parsing other-filename-list.txt\n");
	get_filenames_from_file("other-filename-list.txt",TRUE,list);
	printf("Parsing other-info-files/\n");
	get_info_from_dir("other-info-files/",list,prefs);
	
	printf("I have all the info I need\n");
	
	while(list->prev!=NULL) list=list->prev;
	figure_menunames(list);
	figure_info(list);
	sort_by_menuname(list);
	while(list->prev!=NULL) list=list->prev;
	printf("I'm making CTFILE.txt\n");
	make_ctfile(list,"CTFILE.txt");
	printf("I'm making tracks.bmg.txt\n");
	system("wctct bmg --le-code --long CTFILE.txt > tracks.bmg.txt");
	
	figure_intronames(list);
	printf("I'm making CTFILE.INTRO.txt\n");
	make_ctfile(list,"CTFILE.INTRO.txt");
	printf("I'm making intro.bmg.txt\n");
	system("wctct bmg --le-code --long CTFILE.INTRO.txt > intro.bmg.txt");
	
	return 0;
}

void remove_line_feed(char s[]){
	//remove final line feed
	//this might be different on Windows due to the carriage return thing
	if(s[strlen(s)-1]==10) s[strlen(s)-1]=0;
}

void initialize_track_list(TRACK_LIST *l){
	l->prev=NULL;
	l->next=NULL;
	
	strcpy(l->t.filename,"");
	strcpy(l->t.name,"");
	strcpy(l->t.version,"");
	strcpy(l->t.author,"");
	strcpy(l->t.editor,"");
	strcpy(l->t.extra,"");
	strcpy(l->t.menuname,"");
	strcpy(l->t.music,"");
	strcpy(l->t.slot,"");
	
	l->t.prefix=NULL;
	
	return;
}

void add_track_list_new(TRACK_LIST *dest,TRACK_LIST *new){
	if(strcmp(new->t.filename,"")==0){
		free(new);
	}
	else if(strcmp(dest->t.filename,"")==0){
		dest->t=new->t;
		free(new);
	}
	else{
		while(dest->next!=NULL) dest=dest->next;
		dest->next=new;
		new->prev=dest;
	}
	return;
}

void initialize_prefix(PREFIX *l){
	l->prev=NULL;
	l->next=NULL;
	l->used=1;
	strcpy(l->s,"");	
	return;
}

void add_prefix_new(PREFIX *dest,PREFIX **new){
	if(strcmp(dest->s,"")==0){
		strcpy(dest->s,(*new)->s);
		free(*new);
		*new=dest;
	}
	else{
		while(dest->next!=NULL) dest=dest->next;
		dest->next=*new;
		(*new)->prev=dest;
	}
	return;
}

void get_filenames_from_file(char file[],BOOL other,TRACK_LIST *l){
	FILE *f;
	
	f=fopen(file,"r");
	if(f==NULL) return;
	
	TRACK_LIST *new=malloc(sizeof(TRACK_LIST));
	initialize_track_list(new);
	new->t.other=other;
	fgets(new->t.filename,200,f);
	remove_line_feed(new->t.filename);
	while(!feof(f)){
		add_track_list_new(l,new);
		new=malloc(sizeof(TRACK_LIST));
		initialize_track_list(new);
		new->t.other=other;
		fgets(new->t.filename,200,f);
		remove_line_feed(new->t.filename);
	}
	add_track_list_new(l,new);
	
	fclose(f);
	return;
}

void get_info_from_dir(char dir[],TRACK_LIST *l,PREFIX *p){
	char command[300],c[200];
	FILE *f;
	while(l->prev!=NULL) l=l->prev;
	while(l->next!=NULL){
		sprintf(command,"%s%s.txt",dir,l->t.filename);
		f=fopen(command,"r");
		if(f!=NULL){
			fgets(c,200,f);
			while(!feof(f)){
				remove_line_feed(c);
				parse_info_string(c,l,p);
				fgets(c,200,f);
			}
			fclose(f);
		}
		l=l->next;
	}
	sprintf(command,"%s%s.txt",dir,l->t.filename);
	f=fopen(command,"r");
	if(f!=NULL){
		fgets(c,200,f);
		while(!feof(f)){
			remove_line_feed(c);
			parse_info_string(c,l,p);
			fgets(c,200,f);
		}
		fclose(f);
	}
	return;
}

void parse_info_string(char c[],TRACK_LIST *l,PREFIX *p){
	char aux[200];
	PREFIX *new;
	if(string_begins_with(c,"name ")==TRUE){
		strcpy_noprefix(l->t.name,c,"name ");
	}
	else if(string_begins_with(c,"version ")==TRUE){
		strcpy_noprefix(l->t.version,c,"version ");
	}
	else if(string_begins_with(c,"author ")==TRUE){
		strcpy_noprefix(l->t.author,c,"author ");
	}
	else if(string_begins_with(c,"editor ")==TRUE){
		strcpy_noprefix(l->t.editor,c,"editor ");
	}
	else if(string_begins_with(c,"slot ")==TRUE){
		strcpy_noprefix(l->t.slot,c,"slot ");
	}
	else if(string_begins_with(c,"music ")==TRUE){
		strcpy_noprefix(l->t.music,c,"music ");
	}
	else if(string_begins_with(c,"menuname ")==TRUE){
		strcpy_noprefix(l->t.menuname,c,"menuname ");
	}
	else if(string_begins_with(c,"extra ")==TRUE){
		strcpy_noprefix(l->t.extra,c,"extra ");
	}
	else if(string_begins_with(c,"prefix ")==TRUE){
		strcpy_noprefix(aux,c,"prefix ");
		if(strcmp(aux,"")==0) return;
		while(p->prev!=NULL) p=p->prev;
		while(p->next!=NULL){
			if(strcmp(aux,p->s)==0){
				l->t.prefix=p;
				(p->used)++;
				return;
			}
			p=p->next;
		}
		if(strcmp(aux,p->s)==0){
			l->t.prefix=p;
			(p->used)++;
			return;
		}
		new=malloc(sizeof(PREFIX));
		initialize_prefix(new);
		strcpy(new->s,aux);
		add_prefix_new(p,&new);
		l->t.prefix=new;
	}
	return;
}

void strcpy_noprefix(char to[],char from[],char prefix[]){
	unsigned int i,f=strlen(prefix);
	for(i=0;from[i+f-1]!=0;i++) to[i]=from[i+f];
	return;
}

BOOL string_begins_with(char a[],char b[]){
	unsigned int i;
	if(strlen(a)<strlen(b)) return FALSE;
	for(i=0;i<strlen(b);i++) if(a[i]!=b[i]) return FALSE;
	return TRUE;
}

char *szs_dir(BOOL other){
	if(other==TRUE) return "other-szs-files/";
	return "szs-files/";
}

void figure_menunames(TRACK_LIST *l){
	if(l==NULL) return;
	if(strcmp(l->t.menuname,"")!=0){
		figure_menunames(l->next);
		return;
	}
	if(strcmp(l->t.name,"")!=0){
		if(l->t.prefix!=NULL) sprintf(l->t.menuname,"%s %s",l->t.prefix->s,l->t.name);
		else strcpy(l->t.menuname,l->t.name);
	}
	else strcpy(l->t.menuname,l->t.filename);
	while(fix_menunames(l,l->prev)==FALSE){};
	figure_menunames(l->next);
	return;
}

void figure_intronames(TRACK_LIST *l){
	if(l==NULL) return;
	strcpy(l->t.menuname,"");
	if(l->t.prefix!=NULL) sprintf(l->t.menuname,"%s ",l->t.prefix->s);
	if(strcmp(l->t.name,"")!=0) sprintf(l->t.menuname,"%s%s",l->t.menuname,l->t.name);
	if(strcmp(l->t.extra,"")!=0) sprintf(l->t.menuname,"%s (%s)",l->t.menuname,l->t.extra);
	if(strcmp(l->t.version,"")!=0) sprintf(l->t.menuname,"%s %s",l->t.menuname,l->t.version);
	if(strcmp(l->t.author,"")!=0){
		sprintf(l->t.menuname,"%s\\n%s",l->t.menuname,l->t.author);
		if(strcmp(l->t.editor,"")!=0) sprintf(l->t.menuname,"%s; %s",l->t.menuname,l->t.editor);
	}
	else if(strcmp(l->t.editor,"")!=0) sprintf(l->t.menuname,"%s\\n%s",l->t.menuname,l->t.editor);
	if(strcmp(l->t.name,"")==0) strcpy(l->t.menuname,l->t.filename);
	figure_intronames(l->next);
	return;
}

BOOL fix_menunames(TRACK_LIST *l,TRACK_LIST *aux){
	char command[500];
	if(aux==NULL) return TRUE;
	if(strcmp(l->t.menuname,aux->t.menuname)!=0){
		return fix_menunames(l,aux->prev);
	}
	if(strcmp(l->t.author,aux->t.author)==0) if(strcmp(l->t.version,aux->t.version)!=0){
		if(strcmp(l->t.version,"")!=0){
			sprintf(command,"%s (%s)",l->t.menuname,l->t.version);
			strcpy(l->t.menuname,command);
		}
		if(strcmp(aux->t.version,"")!=0){
			sprintf(command,"%s (%s)",aux->t.menuname,aux->t.version);
			strcpy(aux->t.menuname,command);
		}
		return FALSE;
	}
	if(strcmp(l->t.extra,aux->t.extra)!=0){
		if(strcmp(l->t.extra,"")!=0){
			sprintf(command,"%s (%s)",l->t.menuname,l->t.extra);
			strcpy(l->t.menuname,command);
		}
		if(strcmp(aux->t.extra,"")!=0){
			sprintf(command,"%s (%s)",aux->t.menuname,aux->t.extra);
			strcpy(aux->t.menuname,command);
		}
		return FALSE;
	}
	if(strcmp(l->t.author,aux->t.author)!=0){
		if(strcmp(l->t.author,"")!=0){
			sprintf(command,"%s (%s)",l->t.menuname,l->t.author);
			strcpy(l->t.menuname,command);
		}
		if(strcmp(aux->t.author,"")!=0){
			sprintf(command,"%s (%s)",aux->t.menuname,aux->t.author);
			strcpy(aux->t.menuname,command);
		}
		return FALSE;
	}
	return fix_menunames(l,aux->prev);
}

void figure_info(TRACK_LIST *l){
	if(l==NULL) return;
	if(strcmp(l->t.slot,"")==0){
		if(strcmp(l->t.music,"")==0) strcpy(l->t.slot,"LC");
		else strcpy(l->t.slot,l->t.music);
	}
	if(strcmp(l->t.music,"")==0) strcpy(l->t.music,l->t.slot);
	figure_info(l->next);
	return;
}

void sort_by_menuname(TRACK_LIST *l){
	int n=0,i;
	while(l->next!=NULL) l=l->next;
	while(l->prev!=NULL){
		l=l->prev;
		n++;
	}
	while(n>0){
		for(i=0;i<n;i++){
			if(first_comes_first(l->t.menuname,l->next->t.menuname)==FALSE){
				l->next->prev=l->prev;
				if(l->prev!=NULL) l->prev->next=l->next;
				l->prev=l->next;
				l->next=l->prev->next;
				l->prev->next=l;
				if(l->next!=NULL) l->next->prev=l;
				l=l->prev;
			}
			l=l->next;
		}
		n--;
		if(n==0) return;
		l=l->prev;
		for(i=n;i>0;i--){
			if(first_comes_first(l->prev->t.menuname,l->t.menuname)==FALSE){
				l->prev->next=l->next;
				if(l->next!=NULL) l->next->prev=l->prev;
				l->next=l->prev;
				l->prev=l->next->prev;
				l->next->prev=l;
				if(l->prev!=NULL) l->prev->next=l;
				l=l->next;
			}
			l=l->prev;
		}
		n--;
		if(n==0) return;
		l=l->next;
	}
	return;
}

BOOL first_comes_first(char s1[],char s2[]){
	unsigned int n1=strlen(s1),n2=strlen(s2),i1=0,i2=0,k1,k2;
	char c1,c2;
	while(i1<n1&&i2<n2){
		do{
			if(i1==n1) return TRUE;
			c1=s1[i1];
			if(c1>='A') if(c1<='Z') c1+='a'-'A';
			i1++;
		}while(c1==' '||c1=='-'||c1=='\'');
		do{
			if(i2==n2) return FALSE;
			c2=s2[i2];
			if(c2>='A') if(c2<='Z') c2+='a'-'A';
			i2++;
		}while(i2<n2&&(c2==' '||c2=='-'||c2=='\''));
		if(c1>='a'&&c1<='z') k1=500+(unsigned int)(c1-'a');
		else k1=c1;
		if(c2>='a'&&c2<='z') k2=500+(unsigned int)(c2-'a');
		else k2=c2;
		if(k1<k2) return TRUE;
		if(k2<k1) return FALSE;
	}
	if(i2==n2) return FALSE;
	return TRUE;
}

void make_ctfile(TRACK_LIST *l, char target[]){
	FILE *f;
	unsigned int n=0;
	f=fopen(target,"w");
	fprintf(f,"#CT-CODE\n[RACING-TRACK-LIST]\n%cLE-FLAGS  = 1\n%cWIIMM-CUP = 1\nN N$SWAP | N$F_WII\n",'%','%');
	while(l->prev!=NULL) l=l->prev;
	while(l->next!=NULL){
		if(n%4==0) fprintf(f, "\nC \"c%u\"\n",n);
		fprintf(f,"T %s ; %s ; 0x00 ; \"%s\" ; \"%s\" ; \"%u\"\n",l->t.music,l->t.slot,l->t.filename,l->t.menuname,n);
		l=l->next;
		n++;
	}
	if(n%4==0) fprintf(f, "\nC \"c%u\"\n",n);
	fprintf(f,"T %s ; %s ; 0x00 ; \"%s\" ; \"%s\" ; \"%u\"\n",l->t.music,l->t.slot,l->t.filename,l->t.menuname,n);
	fclose(f);
	return;
}