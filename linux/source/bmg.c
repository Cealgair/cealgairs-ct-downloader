#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
	char s[500],command[1000];
	FILE *f;
	
	f=fopen("path-to-disc-root.txt","r");
	if(f==NULL){
		printf("Error: path-to-disc-root.txt not found!\n");
		return 0;
	}
	fgets(s,500,f);
	fclose(f);
	
	//remove final line feed
	//this might be different on Windows due to the carriage return thing
	if(s[strlen(s)-1]==10) s[strlen(s)-1]=0;
	
	sprintf(command,"wszst patch --quiet %s/files/Scene/UI/*_?.szs --patch-bmg \"overwrite=tracks.bmg.txt\"",s);
	system(command);
	
	sprintf(command,"wszst patch --quiet %s/files/Scene/UI/Race_?.szs --patch-bmg \"overwrite=intro.bmg.txt\"",s);
	system(command);
	return 0;
}