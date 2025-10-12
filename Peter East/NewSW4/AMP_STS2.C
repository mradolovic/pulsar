
/*amp_sts2.c	   pwe: 02/06/13
Takes rtl_sdr bin files and outputs a text file of the adc data distribution
           Command format:- amp_sts <infile> <outfile>*/



#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SWAP(a,b) tempr=(a); (a)=(b); (b)=tempr
#define PI (6.28318530717959/2.0)
#define PTS 256



int pts;
long long int file_end,count,p_num;
double dats[8192],dato[4096];
unsigned char ucha;


FILE *fpto;
FILE *fptr;


void out_dat(void);



main(int argc, char *argv[])



{

int s;
long long int ss;

;
/*check command line arguments*/
if(argc !=3)
  { printf("Format: amp_sts2 <Infile>  <Outfile> \n");exit(0);}


if((fptr=fopen(argv[1],"rb"))==NULL)
   {printf("Can't open file %s. ",argv[1]);exit(0);}

pts=256;



/*find length of input file*/
fseeko(fptr,0L,SEEK_END);
file_end=(long long int)(ftello(fptr));
p_num=(long long int)(file_end/pts/2);
printf("Number of Bytes = %lld\n",file_end);
fclose(fptr);

fptr=fopen(argv[1],"rb");
fpto = fopen(argv[2],"w");
/*read input file, convert I and q, put in data file apply fft, apply running average. At end of input file, output text file with averaged data*/

 {

 for(ss=0;ss<p_num;ss++)
    {

 for(s=0;s<2*pts;s++)
	{
	ucha=getc(fptr);

       dats[s]=(double)ucha+0.0;
       dato[(int)dats[s]]=dato[(int)dats[s]]+1;


	count++;

	}

    }

}/*printf("\nInfile=%s    Outfile=%s\n",argv[1],argv[2]);*/

out_dat();
fclose(fptr);

printf("\nInfile=%s    Outfile=%s\n",argv[1],argv[2]);

exit(0);
}



void out_dat(void)
{
int tt;

float opp;

for(tt=0;tt<pts;tt++){

	   opp=(double)dato[tt];

fprintf(fpto,"%d    %3.3f\n",(tt-128),(double)(opp/(double)p_num));

}
fclose(fpto);
}
