/*****************************************************
TAGFINDER - JSYANG - Mar. 11, 2010

Finds all text within exact string search bounds for
a "key" string, then returns the contents between a
"head" string further down and a "tail" end string.
The bounds are adjustable in the [pattern file].

Ex: <a><center><b>Almighty</a> bungholio!</b></center>

key: <a>
head: >
tail: </a>
dhead: 0
dtail: 0

returns: "<center><b>Almighty"

*****************************************************/

#include <stdio.h>
#include <string.h>

/**** MAXLINES: number of lines in a pattern file ****

line	desc. of pattern file line	var

0	lineContainsExactString		key
1	startFromThisNextString		head
2	endAtThisNextString		tail
3	+headIndex			dhead
4	+tailIndex			dtail

*****************************************************/
#define MAXLINES 5
#define MAXLINESIZE 8192

//void quit(int quitIfTrue, char *quitMSG);
// abort with a msg if condition is true
void quit(int quitIfTrue, char *quitMSG){
	if(!quitIfTrue) return;
	fprintf(stderr,"%s \n",quitMSG);
	exit(1);
}

int main(int argc, char* argv[]){
	FILE *f;
	int i=0;
	int j=0;
	int head_len=0;
	int tail_len=0;
	char *startBlok;
	char *endBlok;
	char fLine[MAXLINESIZE]="";
	char content[MAXLINESIZE]="";
	char patternFile[MAXLINES][MAXLINESIZE];
		
	if(argc>2){
		
		quit(!(f=fopen(argv[1],"r")), "TAGFINDER: couldn't load pattern file!");

		// read the pattern file
		while(i<MAXLINES && fgets(fLine,MAXLINESIZE,f)!=NULL){
			
			// init the string
			if(!j++) patternFile[i][0]='\0';
			
			strncat(patternFile[i],(const char*) fLine, MAXLINESIZE);
					
			// keep reading the file until we get the real end of the line
			if(strchr(fLine,'\n')!=NULL){
				
				// remove trailing \n
				patternFile[i][strlen(patternFile[i])-1]='\0';
				i++;
				j=0;
				
			}else if(i==MAXLINES-1){
				
				// the last line is missing a \n, proceed
				i++;
				break;
			}
		}

		quit(i<MAXLINES,"TAGFINDER: pattern file incomplete!");

		quit(!(f=fopen(argv[2],"r")), "TAGFINDER: couldn't load data file!");

		// search queries
		const char *key=patternFile[0];
		const char *head=patternFile[1];
		const char *tail=patternFile[2];
		head_len=strlen(head);
		tail_len=strlen(tail);
		int dhead=atoi(patternFile[3]);
		int dtail=atoi(patternFile[4]);	
		
		// flag for if we're in a blok
		j=0;
		
		while(fgets(fLine,MAXLINESIZE,f)!=NULL){
						
			// if we're in a block, look for the tail
			if(j){ 	startBlok=fLine; goto inBlok; }
			
			// can we find a key? // DEBUG: fprintf(stderr,"fLine: %s",fLine);
			startBlok=strstr(fLine,key);
			while(startBlok){
				
				// DEBUG: fprintf(stderr,"FOUND %s @ %d\n",key,startBlok);
				
				// if we have a "head" pattern to look for after the "key"
				if(head_len && strstr(startBlok,head)){
					startBlok=strstr(startBlok,head);
					startBlok+=head_len;
				}
				
				// adjust content start by dhead
				startBlok+=dhead;
				
				// we're in a blok
				j=1;
				
				inBlok:
							
				endBlok=strstr(startBlok,tail);
				if(endBlok){
					content[0]='\0';
					endBlok+=dtail;
					strncat(content,startBlok,strlen(startBlok)-strlen(endBlok));
					printf("%s\n",content);
					startBlok=endBlok;
					startBlok+=tail_len;
					startBlok=strstr(startBlok,key);
					j=0;
				}else{
					printf("%s",startBlok);
					startBlok=NULL;
				}
				
			}			
		}
		
		fclose(f);
	}else{
		fprintf(stderr,"Syntax: %s [pattern file] [data file]",argv[0]);
		quit(1,"");
	}
	return 0;
}
