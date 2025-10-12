
/*filesum.c	   pwe: 02/06/2016
Takes .txt files, sums the amplitudes sample by sample and outputs a summed file

Command format:- filetrim <infile> <outfile> <Start Bytes cut>  <End Bytes cut>  */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>




long long int end, start,count=0;
int PTS,i,q;
long long int file_end,nfend;
unsigned char ucha;
float smt1,smt2,smt3,smt4,sumt;
FILE *fptr1;
FILE *fptr2;
FILE *fptr3;
FILE *fptr4;
FILE *fpto;

void out_dat(void);


main(int argc,char *argv[])

{

long long int ss,sss,tt;
int s;
int ags=0;

printf("No Files summed = %d\n",argc-1);
if(argc==1)exit(0);
/*check command line arguments*/
for(ags=0;ags<argc;ags++)
    { 
		//printf("No Files summed = %d\n",argc);
	    

if(ags==1 && (fptr1=fopen(argv[1],"rb"))==NULL)
    {printf("Can't open file %s. \n",argv[1]);
    exit(0);
    }

if(ags==2 && (fptr2=fopen(argv[2],"rb"))==NULL)
    {printf("Can't open file %s. \n",argv[2]);
    exit(0);
    }
if(ags==3 && (fptr3=fopen(argv[3],"rb"))==NULL)
    {printf("Can't open file %s. \n",argv[3]);
    exit(0);
    }
if(ags==4 && (fptr4=fopen(argv[4],"rb"))==NULL)
    {printf("Can't open file %s. \n",argv[4]);
    exit(0);
    }
}
//start=(long long int)atoll(argv[3]);
//end=atoll(argv[4]);
//printf("start=%lld\n",start);
while(fscanf(fptr1,"%f",&smt1) !=EOF){; //if (c == '\n') // Increment count if this character is newline fscanf(fptr,"%f",&smt);
		count = count + 1;}
 
    // Close the file
    fclose(fptr1); 
	fclose(fptr2);
	fclose(fptr3);
	fclose(fptr4);
file_end=(long long int)(count);

//p_num=(long long int)((file_end)/(PTS));

printf("No. Bytes = %llu\n",(long long int )(count));//(file_end));                                        
fptr1=fopen(argv[1],"r");
fptr2=fopen(argv[2],"r");
fptr3=fopen(argv[3],"r");
fptr4=fopen(argv[4],"r"); 




/*find length of input file*/
//fseeko(fptr1,0L,SEEK_END);
//file_end=(long long)(ftello(fptr1));

//nfend=(long long int)(file_end-end);



//printf("No. Bytes = %lld \n",((long long)(nfend-start)));
//printf("Start byte=%lld\n",(long long)start);
//printf("End Byte=%lld\n",(long long)nfend);
//p_num=(long long int)(p_num);

//printf("p_num=%lld\n",p_num);
//fclose(fptr1);

//fptr=fopen(argv[1],"rb");
//fseeko(fptr1,start,SEEK_SET);
fpto = fopen("sumout.txt","w");
/*read input file, go to start address, put binary data into output file, continue to the file end minus the number of 'end bytes'*/

/* for(sss=0;sss<nfend-start;sss+=1)
        {
        ucha=getc(fptr1);
        fputc(ucha,fpto);
        count+=1;
		}*/
//
//
printf("Output File Count=%lld\n",(long long)count);
 for(tt=0;tt<file_end;tt++)
    {fscanf(fptr1,"%f",&smt1);fscanf(fptr2,"%f",&smt2);fscanf(fptr3,"%f",&smt3);fscanf(fptr4,"%f",&smt4);
		sumt=(float)smt1+(float)smt2+(float)smt3+(float)smt4;//printf("%ld    %f   %Ld\n",(tt),((float)sumt[tt]),count[tt]);
fprintf(fpto,"%f\n",((float)sumt));
sumt=0;
    }

fclose(fptr1);
fclose(fptr2);
fclose(fptr3);
fclose(fptr4);
fclose(fpto);


}

