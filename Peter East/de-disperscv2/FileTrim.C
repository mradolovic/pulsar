
/*filetrim.c	   pwe: 02/04/2015 05/02/2016
Takes .bin files, cuts bytes from front and end of files.

Command format:- filetrim <infile> <outfile> <Start Bytes cut>  <End Bytes cut>  */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>




long long int end, start,count=0;
int PTS,i,q;
long long int file_end,nfend;
unsigned char ucha;

FILE *fptr;
FILE *fpto;

void out_dat(void);


main(int argc,char *argv[])

{

long long int ss,sss;
int s;



/*check command line arguments*/
if(argc !=5)
    { printf("Format: filetrim <infile> <outfile> <Start bytes cut>  <End bytes cut> \n");
    exit(0);
    }

if((fptr=fopen(argv[1],"rb"))==NULL)
    {printf("Can't open file %s. \n",argv[1]);
    exit(0);
    }


start=(long long int)atoll(argv[3]);
end=atoll(argv[4]);
//printf("start=%lld\n",start);

                                        


/*find length of input file*/
fseeko(fptr,0L,SEEK_END);
file_end=(long long)(ftello(fptr));

nfend=(long long int)(file_end-end);



printf("No. Bytes = %lld \n",((long long)(nfend-start)));
printf("Start byte=%lld\n",(long long)start);
printf("End Byte=%lld\n",(long long)nfend);
//p_num=(long long int)(p_num);

//printf("p_num=%lld\n",p_num);
fclose(fptr);

fptr=fopen(argv[1],"rb");
fseeko(fptr,start,SEEK_SET);
fpto = fopen(argv[2],"wb");
/*read input file, go to start address, put binary data into output file, continue to the file end minus the number of 'end bytes'*/

 for(sss=0;sss<nfend-start;sss+=1)
        {
        ucha=getc(fptr);
        fputc(ucha,fpto);
        count+=1;
		}
fclose(fptr);
fclose(fpto);
printf("Output File Count=%lld\n",(long long)count);
 }

