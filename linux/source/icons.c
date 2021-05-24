#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum{TRUE,FALSE}BOOL;

BOOL search_in_file(FILE *f,char string[]);

int main(){
	FILE *f,*g;
	char c,s[500],command[1000];
	f=fopen("CTFILE.txt","r");
	g=fopen("bin/cup-list.txt","w");
	while(search_in_file(f,"C \"")==TRUE){
		search_in_file(f,"; \"");
		search_in_file(f,"; \"");
		fprintf(g,"w");
		fscanf(f,"%c",&c);
		fprintf(g,"%c",c);
		fscanf(f,"%c",&c);
		fprintf(g,"%c",c);
		fscanf(f,"%c",&c);
		fprintf(g,"%c",c);
		fprintf(g,";\n");
	}
	fclose(f);
	fclose(g);
	f=fopen("path-to-disc-root.txt","r");
	fgets(s,500,f);
	fclose(f);
	
	//remove final line feed
	//this might be different on Windows due to the carriage return thing
	if(s[strlen(s)-1]==10) s[strlen(s)-1]=0;
	
	system("cd bin;./generate-icons");
	
	sprintf(command,"wimgt copy bin/cusu.png -rqo %sfiles/Scene/UI/ct_icons.tpl -x tpl.CMPR",s);
	system(command);
	
	sprintf(command,"cd %sfiles/Scene/UI/; for i in Channel MenuMulti MenuSingle; do wszst extract $i\\.szs; cp ct_icons.tpl $i\\.d/button/timg/; cp ct_icons.tpl $i\\.d/control/timg/; rm $i\\.szs; wszst create $i\\.d; rm -r $i\\.d; done; rm ct_icons.tpl",s);
	system(command);
	
	system("rm bin/cusu.png; rm bin/cup-list.txt");
	return 0;
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