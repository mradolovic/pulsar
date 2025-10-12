
/*bin_txtr2.c	   pwe: 29/01/15
Takes rtlsdr bin files and outputs a flat text file of the data samples
Command format:- bin_txtr2 <infile> <outfile>
<number of 8192 sample blocks-start> <number of 8192 sample blocks-end>*/

#include <stdio.h>
#include <time.h>

#include <stdlib.h>
#include <math.h>



int pts;
long long int file_end,count,p_num,blks,blke,sss, blkstart,blkend;
int num;
float dats[16384],dato[16384];
unsigned char ucha;

FILE *fpto;

/*void sum_dat(void);*/

void out_dat(void);
void *__gxx_personality_v0;


int main(int argc, char *argv[])
{

 long long int s,ss;

FILE *fptr;
/*check command line arguments*/
if(argc !=5)
  { printf("Format: bin_txtr2 <Infile>  <Outfile> <number of 8192 I/Q blocks-start> <number of 8192 I/Q blocks-end>\n");
    exit(0);}
blks=(long long)atol(argv[3]);
blke=(long long)atol(argv[4]);
num=16384;
//printf("Start = %ld	%lld	%lld\n",blkstart,blks,blke);
if((fptr=fopen(argv[1],"rb"))==NULL)
   {printf("Can't open file %s. \n",argv[1]);exit(0);}

//pts=256;

fpto = fopen(argv[2],"w");

/*find length of input file*/
fseeko(fptr,0L,SEEK_END);
file_end=(long long int)(ftello(fptr));
//p_num=(long long int)(file_end/pts/2);
printf("Number of I/P Bytes = %lld\n",file_end);
printf("Number of O/P Samples = %lld\n",(long long)(num*(blke-blks)));
//printf("Start = %lld\n",blke);
fclose(fptr);

fptr=fopen(argv[1],"rb");
fseeko(fptr,(long long)(blks*num),SEEK_SET);
//blkstart=(long long int)(ftello(fptr));
//printf("Start = %lld	%lld	%lld\n",blkstart,blks,blke);
/*read input .bin file,decodes I and Q, outputs as text file. <sample number>	<Imaginary compt> <Real compt>*/


for(sss=0;sss<(long long)(blke-blks);sss++){
 {
printf("sss = %lld\n",sss);
 for(ss=0;ss<num;ss++)
    {

	ucha=getc(fptr);

	dats[ss]=(float)ucha+0.0;
	
	dats[ss]=(dats[ss]-127.5)/128.0;
	//printf("Dat = %d	%f\n",(int)ucha,dats[ss]);
	count++;
    }

 //for(s=0;s<num;s++){
  //       dato[(int)s]=dats[(int)(s)];

	//		 }
/*if(sss>=blks)*/
out_dat();
 }
}
fclose(fptr);
fclose(fpto);
printf("\nInfile=%s    Outfile=%s\n",argv[1],argv[2]);

exit(0);
}



void out_dat(void)
{
int tt;

float opp1, opp2;

for(tt=0;tt<(num)/2;tt++){
	   opp1=(float)dats[2*tt];

	   opp2=(float)dats[2*tt+1];
//printf("%lld	%1.4f	%1.4f\n",((long long)(tt+8192*(sss+blks))),(float)opp1, (float)opp2);
fprintf(fpto,"%ld	%1.4f	%1.4f\n",((long)(tt+8192*(sss+blks))),(float)opp1, (float)opp2);

 }

}
