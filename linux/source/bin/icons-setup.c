#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void remove_line_feed(char s[]);

int main(){
	char path[400],command[1000],cups[8][10];
	int i=-1;
	
	if(access("arrows.png",F_OK)!=0){
		printf("arrows.png not found. Be sure to execute this file from the bin folder.\n");
		return 0;
	}
	if(access("rand.png",F_OK)!=0){
		printf("rand.png not found. Be sure to execute this file from the bin folder.\n");
		return 0;
	}
	
	sprintf(cups[0],"kinoko");
	sprintf(cups[1],"koura");
	sprintf(cups[2],"flower");
	sprintf(cups[3],"banana");
	sprintf(cups[4],"star");
	sprintf(cups[5],"konoha");
	sprintf(cups[6],"oukan");
	sprintf(cups[7],"thunder");
	
	while(i==-1){
		printf("Input the path to original disc root (do not use ~):\n");//remove the warning about ~ in Windows, it doesn't make sense there
		fgets(path,400,stdin);
		remove_line_feed(path);
		//check if path is ok
		sprintf(command,"%s/files/Scene/UI/MenuSingle.szs",path);
		printf("%s\n", command);
		i=access(path,F_OK);
		if(i!=0) printf("That path doesn't work.\n\n");
	}
	sprintf(command,"wszst extract %s/files/Scene/UI/MenuSingle.szs -d MenuSingle.d",path);
	system(command);
	for(i=0;i<8;i++){
		sprintf(command,"wimgt convert MenuSingle.d/button/timg/tt_cup_icon_%s_00.tpl -d %d.png",cups[i],i+1);
		system(command);
	}
	system("rm -rf MenuSingle.d; convert -append arrows.png 1.png 2.png 3.png 4.png 5.png 6.png 7.png 8.png rand.png BASE.png; rm -f ?.png");
	return 0;
}

void remove_line_feed(char s[]){
	//remove final line feed
	//this might be different on Windows due to the carriage return thing
	if(s[strlen(s)-1]==10) s[strlen(s)-1]=0;
}