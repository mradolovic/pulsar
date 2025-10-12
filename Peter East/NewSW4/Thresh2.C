
/*filesum.c	   pwe: 02/09/2016
Takes .txt files, thresholds the amplitudes derived in filesum2 and rebuilds output file

Command format:- thresh2 <infile>  <mean> < max> <outfile> \n")  */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>




long long int end, start,count=0;
int PTS,i,q;
long long int file_end,nfend;
unsigned char ucha;
double smt1,smt2,smt3,smt4,sumt=0,mean=0,nmean=0,rms=0;
float ga1=1.0, ga2=1.0,ga3=1.0, ga4=1.0, max=1;
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

//if(printf("No Files summed = %d\n",argc-1);
if(argc!=5){printf("Format: thresh2 <infile>  <mean> < max> <outfile> \n"); exit(0);}
/*check command line arguments*/
for(ags=0;ags<argc;ags++)
    { 
		//printf("No Files summed = %d\n",argc);
	    

if(ags==1 && (fptr1=fopen(argv[1],"rb"))==NULL)
    {printf("Can't open file %s. \n",argv[1]);
    exit(0);
    }
mean=atof(argv[2]);

if(ags==3 && argv[3]!=NULL)
    {max=atof(argv[3]);
printf("Threshold= %f \n",max);
    
	}
}
//start=(long long int)atoll(argv[3]);
//end=atoll(argv[4]);
//printf("start=%lld\n",start);
while(fscanf(fptr1,"%f",&smt1) !=EOF){; //if (c == '\n') // Increment count if this character is newline fscanf(fptr,"%f",&smt);
		count = count + 1;}
 
    // Close the file
    fclose(fptr1); 
	
file_end=(long long int)(count);

//p_num=(long long int)((file_end)/(PTS));

printf("No. Bytes = %llu\n",(long long int )(count));//(file_end));                                        
fptr1=fopen(argv[1],"r");




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
fpto = fopen(argv[4],"w");
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
    {fscanf(fptr1,"%lf",&smt1);
		if(smt1>max) smt1=mean;        // =ga1*smt1+ga2*smt2+ga3*smt3+ga4*smt4;
		nmean=nmean+smt1;
		rms=rms+smt1*smt1;								//printf("%ld    %f   %Ld\n",(tt),((float)sumt[tt]),count[tt]);
	fprintf(fpto,"%.10f\n",(smt1));

    }
nmean=nmean/(double)file_end;
	rms=(rms)/(double)file_end;
	rms=sqrt(rms-nmean*nmean);

printf("Mean= %.10f  rms= %.10f\n",(float)nmean,(float)rms);

fclose(fptr1);



}

