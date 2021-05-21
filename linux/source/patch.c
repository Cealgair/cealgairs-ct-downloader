#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
	char s[500],r[500],command[2000];
	FILE *f;
	
	f=fopen("path-to-disc-root.txt","r");
	if(f==NULL){
		printf("Error: path-to-disc-root.txt not found!\n");
		return 0;
	}
	fgets(s,500,f);
	fclose(f);
	f=fopen("path-to-lecode-build.txt","r");
	if(f==NULL){
		printf("Error: path-to-lecode-build.txt not found!\n");
		return 0;
	}
	fgets(r,500,f);
	fclose(f);
	
	//remove final line feed
	//this might be different on Windows due to the carriage return thing
	if(s[strlen(s)-1]==10) s[strlen(s)-1]=0;
	if(r[strlen(r)-1]==10) r[strlen(r)-1]=0;
	
	sprintf(command,"wlect patch %s -od %s/files/rel/lecode-PAL.bin --le-define CTFILE.txt --track-dir %s/files/Race/Course --copy-tracks szs-files/ --copy-tracks other-szs-files/ --custom-tt --speedometer=2",r,s,s);
	system(command);
	sprintf(command,"wszst xcommon %s/files/Race/Course/*.szs -qiod %s/files/Race/Common/%cN/",s,s,'%');
	return 0;
}
