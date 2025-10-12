
/*bin_txt_sam.c	   pwe: 29/01/15
Takes rtlsdr bin files and outputs a flat text file of every Nth I/Q pair
Command format:- bin_txt_sam <infile> <outfile> <number of missed I/Q samples N> */

#include <stdio.h>
#include <time.h>

#include <stdlib.h>
#include <math.h>



int pts;
long long int file_end,count,p_num,num,sss;
long int blks,blke,offset;
float dats[16384],dato[16384];
unsigned char ucha;

FILE *fpto;

/*void sum_dat(void);*/

void out_dat(void);
void *__gxx_personality_v0;


int main(int argc, char *argv[])
{

 long int s,ss,t_hr,t_dy,t_ep,t_off,t_mn;

FILE *fptr;
/*check command line arguments*/
if(argc !=4)
  { printf("Format: bin_txt_sam <Infile>  <Outfile> <number of missed I/Q samples, N> \n");
    exit(0);}
blks=(long)atoi(argv[3]);
//blke=atoi(argv[4]);
num=16384;

if((fptr=fopen(argv[1],"rb"))==NULL)
   {printf("Can't open file %s. \n",argv[1]);exit(0);}

pts=256;

fpto = fopen(argv[2],"w");

/*find length of input file*/
fseeko(fptr,0L,SEEK_END);
file_end=(ftello(fptr));
p_num=(long long int)(file_end/blks/2);
printf("Number of I/P Bytes = %lld\n",file_end);
printf("Number of O/P IQ pairs = %lld\n",p_num);
{time_t in_seconds;
in_seconds = time(NULL);
printf("%s\n", ctime(&in_seconds)); }

fclose(fptr);

fptr=fopen(argv[1],"rb");


/*read input .bin file,decodes I and Q, outputs as text file. <sample number>	<Imaginary compt> <Real compt>*/


 for(sss=0;sss<p_num;sss++){
 {offset=blks*2*sss;

 //for(ss=0;ss<num;ss++)
   fseeko(fptr,offset,SEEK_SET);
 {

	ucha=getc(fptr);
    dats[0]=(float)ucha+0.0;
    ucha=getc(fptr);
    dats[1]=(float)ucha+0.0;

    dats[0]=(dats[0]-127.5)/128.0;
    dats[1]=(dats[1]-127.5)/128.0;

	count++;
    }

 //for(s=0;s<2;s++){
//	     dato[s]=dats[(int)(s)];

//			 }
//if(sss>=blks)
    out_dat();

 }
}
fclose(fptr);
fclose(fpto);
printf("\nInfile=%s    Outfile=%s\n",argv[1],argv[2]);
printf("Number of IQ pairs = %lld\n",count);
printf("Last Byte = %lld\n",(2*blks*(p_num)));
printf("Divisor = %ld\n",(blks));
exit(0);
}



void out_dat(void)
{
int tt;

float opp1, opp2;

//for(tt=0;tt<(num)/;tt++){
//	   opp1=(float)dato[2*tt];

//	   opp2=(float)dato[2*tt+1];

fprintf(fpto,"%lld	%1.4f	%1.4f\n",((long long)(sss)),(float)dats[0], (float)dats[1]);

// }

}
