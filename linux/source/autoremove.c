#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME_STRING_MAXLEN 164
#define LOG_FILE "log.txt"
#define SZS_FILES_DIR "szs-files/"
#define INFO_FILES_DIR "info-files/"

typedef enum{TRUE,FALSE}BOOL;

BOOL search_in_file(FILE *f,char string[]);
void remove_line_feed(char s[]);

int main(){
	char string[FILENAME_STRING_MAXLEN],command[200];
	FILE *log;
	BOOL check;
	log=fopen(LOG_FILE,"r");
	if(log){
		check=search_in_file(log,"Unneeded files:");
		if(check==FALSE){
			printf("\"Unneeded files:\" not found in log.txt\n");
			check=search_in_file(log,"Unneeded info files:");
			if(check==FALSE){
				printf("\"Unneeded info files:\" not found in log.txt\n");
				return 0;
			}
		}
		else{
			//get input line
			fgets(string,FILENAME_STRING_MAXLEN,log);
			remove_line_feed(string);
			while(check==TRUE){
				//check if line is empty
				if(strlen(string)!=0){
					//check if we're done with unneeded files
					if(0==strcmp(string,"Unneeded info files:")){
						printf("Done with unneeded files\n");
						check=FALSE;
					}
					else{
						//maybe different on Windows
						sprintf(command,"rm \"%s%s\"",SZS_FILES_DIR,string);
						system(command);
					}
				}
				if(check==TRUE){
					//get next input line
					fgets(string,FILENAME_STRING_MAXLEN,log);
					remove_line_feed(string);

					//check if file is over
					if(feof(log)){
						printf("Done with unneeded files\n");
						printf("\"Unneeded info files:\" not found in log.txt\n");
						return 0;
					}
				}
			}
		}
		//get input line
		fgets(string,FILENAME_STRING_MAXLEN,log);
		remove_line_feed(string);
		while(!feof(log)){
			//check if line is empty
			if(strlen(string)!=0){
				//maybe different on Windows
				sprintf(command,"rm \"%s%s\"",INFO_FILES_DIR,string);
				system(command);
			}
			//get next input line
			fgets(string,FILENAME_STRING_MAXLEN,log);
			remove_line_feed(string);
		}
		printf("Done with unneeded info files\n");
	}
	else{
		printf("log.txt not found\n");
	}
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

void remove_line_feed(char s[]){
	//remove final line feed
	//this might be different on Windows due to the carriage return thing
	if(s[strlen(s)-1]==10) s[strlen(s)-1]=0;
}