#include <stdio.h>
#include <stdlib.h>

char trova(FILE *f,char *a){
	while(0==0){
		fscanf(f,"%c",a);
		if(feof(f)) return 0;
		if(*a>='a') if(*a<='z') return 1;
	}
}

int main(){
	FILE *f,*g;
	int n=0;
	char a=';',i;
	f=fopen("cup-list.txt","r");
	g=fopen("temp","w");
	while(0==0){
		i=trova(f,&a);
		if(i==0){
			fclose(f);
			fclose(g);
			g=fopen("cusu.svg","w");
			fprintf(g,"<svg xmlns=\"http://www.w3.org/2000/svg\" height=\"%d\" width=\"128\">\n",128*n);
			fclose(g);
			system("cat ../cup-icons-style.txt >> cusu.svg");
			g=fopen("cusu.svg","a");
			fprintf(g,"\n\t<rect fill=\"#000001\" height=\"%d\" width=\"128\"/>\n",128*n);
			fclose(g);
			system("cat temp >> cusu.svg");
			system("echo \\<\\/svg\\> >> cusu.svg");
			system("convert -append BASE.png cusu.svg -transparent \"rgb(0,0,1)\" -background none cusu.png");
			system("rm temp;rm cusu.svg");
			return 0;
		}
		fprintf(g,"\t<text class=\"%c\" text-anchor=\"middle\" x=\"64\" y=\"%d\">",a,78+128*n);
		fscanf(f,"%c",&a);
		while(a!=';'){
			fprintf(g,"%c",a);
			fscanf(f,"%c",&a);
		}
		fprintf(g,"</text>\n");
		n++;
	}
}