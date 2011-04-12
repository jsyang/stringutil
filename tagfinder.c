/*//////////////////////////////////////////////////////////////////////////////
Tagfinder -- Exact target string matcher.
Mar. 11, 2010, cleaner version Mar. 23, 2011
jsyang.ca@gmail.com

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

//////////////////////////////////////////////////////////////////////////////*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINESIZE 1<<16

void err_( const char *msgfmt, char *msg )
{
    fprintf(stderr,msgfmt,msg);
}

void printUsage()
{
    err_(
        "Find strings between occurences of HEAD and TAIL after matching KEY; stdout.\n"
        "Usage:\t%s [options] [file1] [file2] ... [fileN]\n"
        "Options:\n"        
        "\t--KEY \"_look_for_\"    String to match for starting bound\n"
        "\t--HEAD \"_start_from_\" Exact string to start from, after finding an instance of KEY\n"
        "\t--TAIL \"_end_at_\"     String to match for ending bound\n"
        "\n\tOptional arguments:\n"
        "\t--dhead ###           # of characters to skip before starting after HEAD\n"
        "\t--dtail ###           # of characters to skip before ending prior to TAIL\n"
    ,"");
}

int main( int argc, char* argv[] )
{
    FILE *f;

    // Required option values.
    char *Key=NULL;    int len_Key=0;
    char *Head=NULL;   int len_Head=0;
    char *Tail=NULL;   int len_Tail=0;
	
    // Default skips.
    int dhead=0;
    int dtail=0;
    
    // Temps.
    int i;
	
    // A line from a file.
    char line[LINESIZE]="";
    char buffer[LINESIZE]="";

    // Search operations.
    int j,k;
    int inTarget=0;            // Are we already printing the target string?
                               // If so, skip to finding the TAIL string.

    // Indices we're reading from, to.
    char *targetStart=NULL;
    char *targetEnd=NULL;

    // Syntax message.		
    if(argc<2)
    {
        printUsage();
        exit(1);
    }
		
    // Set up our options.
    for(i=0;++i<argc;)
    {
        // Switches.
        if(!strncmp(argv[i],"--",2))
        {
            if(!strncasecmp(argv[i],"--KEY",5)){    Key=argv[++i]; len_Key=strlen(Key); continue;   }
            if(!strncasecmp(argv[i],"--HEAD",6)){   Head=argv[++i];len_Head=strlen(Head); continue; }
            if(!strncasecmp(argv[i],"--TAIL",6)){   Tail=argv[++i];len_Tail=strlen(Tail); continue; }
            if(!strncasecmp(argv[i],"--dhead",7)){  dhead=atoi(argv[++i]); continue; }
            if(!strncasecmp(argv[i],"--dtail",7)){  dtail=atoi(argv[++i]); continue; }
        }
        
        // Check to see if all the required switches have been set.
        if(!Key || !Head || !Tail)
        {
            err_("ERROR: some required parameters not set: ","");
            if(!Head) err_("HEAD ","");
            if(!Key) err_("KEY ","");
            if(!Tail) err_("TAIL ","");
            err_("!\n","");
            printUsage();
            exit(1);
        }
        
        // Filenames begin at argv[i].
        i--; break;
    }        
    
    // Read all the files.
    for(;++i<argc;)
    {
        if(f=fopen(argv[i],"r"))
        {
            while(fgets(line,LINESIZE,f)!=NULL)
            {
                // If already printing the target, skip seeking KEY/HEAD.
                if(inTarget)
                {
                    targetStart=line;
                    goto SKIPKEYHEAD;
                }
                
                // Locate the KEY string.        		
                for(targetStart=strstr(line,Key);targetStart;)
                {       			
                    // If Head string exists, look for after the "key"
                    if(Head && strstr(targetStart,Head))
                    {
                        targetStart=strstr(targetStart,Head);
                        
                        // Skip the first dhead characters before reading target.
                        targetStart+=len_Head+dhead;
                    }
                    
                    // Found the target!
                    inTarget=1;
                
                ////////////
                SKIPKEYHEAD:

                    targetEnd=strstr(targetStart,Tail);
                    if(targetEnd)
                    {
                        // Reset our output buffer.
                        buffer[0]='\0';
                        // Skip the first dtail characters before ending target.
                        strncat(buffer,targetStart,strlen(targetStart)-strlen(targetEnd+dtail));
                        
                        // Print our match.
                        printf("%s\n",buffer);
                        targetStart=targetEnd+len_Tail;
                        
                        // Look for more matches in the same line.
                        targetStart=strstr(targetStart,Key);
                        inTarget=0;
                    }
                    else
                    {
                        // The rest of the line is considered a match!
                        printf("%s",targetStart);
                        targetStart=NULL;
                    }
                    
                }			
            }
        }
        else
        {
            err_("\nCould not load %s!\n",argv[i]);
        }        
    }

	if(f) fclose(f);
	return 0;
}
