////////////////////////////////////////////////////////////////////////////////
//  Deletes all newlines '\n' from a file.
//  Used for overcoming shell / WSH filesize limits.
//  jsyang.ca@gmail.com
//
//  Originally written ~ Mar. 12, 2010.
//  Code cleaned and revised Mar. 23, 2011.
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>

void err_( const char *msgfmt, char *msg )
{
    fprintf(stderr,msgfmt,msg);
}

int main(int argc, char* argv[])
{
	FILE *f;
	char c;
	int i;    
    
	if(argc>1)
    {	
        for(i=0;++i<argc;)
        {
            if(f=fopen(argv[i],"r"))
            {
                while(!feof(f))
                {
                    if( (c=fgetc(f)) !='\n') printf("%c",c);
                }
            }
            else
            {
                err_("\nCouldn't open %s!",argv[i]);
                exit(1);
            }
            fclose(f);
		}
	}
    else
    {
        err_("Usage: \t%s [file1] [file2] ... [fileN]\n",argv[0]);
		err_("\tRemoves all '\\n' from files; into stdout.","");
	}
	return 0;
}



