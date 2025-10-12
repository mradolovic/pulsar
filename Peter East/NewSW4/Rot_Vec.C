
/*rtt_vec.c	   pwe: 03/10/2015 - 03/02/2016 Vector rotate
Takes rtlsdr .bin files, reads iq values adds to or subtracts wanted phase to iq and  outputs data in original .bin form. 


Command format:- rot_vec <infile> <outfile> <phase in rads> <0 - fixed, 1 multiplied by iq sample number> */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SWAP(a,b) tempr=(a); (a)=(b); (b)=tempr
#define PI (6.28318530717959/2.0)

long long int count=0;

float res, sum,dsr,di,dq,dt,RF,phase,df,DM;
int C,fn,ddt,bn,cn,flag,ddarry=4096, invert=1,offset=0,noffts;
long long int file_end,an,couni=0,blocks;

float smt;
double summ=0,I,Q,i,q,io,qo;

unsigned char ucha,uchb;
unsigned char  ocha,ochb;

FILE *fptr;
FILE *fpto;

void out_dat(void);
void ftorg_dat(void);
void ftreorg_dat(void);
void sum_dat(void);
void four(double[],int,int);

main(int argc,char *argv[])

{

long long int ss;
long int s,sss;



/*check command line arguments*/
if(argc != 5)
    { printf("Format: rto_vec <infile> <outfile> <phase> <fixed/xcount (0/1)> \n");
    exit(0);
    }

if((fptr=fopen(argv[1],"rb"))==NULL)
    {printf("Can't open file %s. ",argv[1]);
    exit(0);
    }

phase=atof(argv[3]); //phase increment


C=atoi(argv[4]); //fixed=0,proportional=1

   printf("Phase increment= %f\n",phase);
  	
/*find length of input file*/
fseeko(fptr,0L,SEEK_END);
file_end=(long long int)(ftello(fptr));


if(C==0)printf("Fixed offset =%f\n",phase);
else printf("Drift offset =%f per sample\n",phase);



fclose(fptr);

fptr=fopen(argv[1],"rb");
fpto = fopen(argv[2],"wb");

 for(an=0;an<file_end/2;an++)
  {
		
		//get sdr binfile i/q data 
		ucha=getc(fptr);
			i=(float)ucha+0.0; /* printf("%f\n",dats[s]);*/
			i=(i-128.0);
		uchb=getc(fptr);
			q=(float)uchb+0.0; /* printf("%f\n",dats[s]);*/
			q=(q-128.0);
			couni +=1;	
			//printf("%d   %d   %f    %f\n",(int)ucha,(int)uchb,dats[2*fn],dats[2*fn+1]);
		
		//printf("%f    %f \n",dats[2],dats[3]);
	if(C==0){	
				I=cos(phase);
				Q=sin(phase);
			}
	else{	I=cos(phase*(float)(couni-1));
			Q=sin(phase*(float)(couni-1));
			
			}
	
	if(an==0)
		{io=i;qo=q;}
	else{
			io=(i*I-q*Q);
			qo=(i*Q+q*I);
		}
	
	ocha=(char)((int)((float)(io+128.0)));
	ochb=(char)((int)((float)(qo+128.0)));
		
		
	/*output rotated data to file*/
	fputc(ocha,fpto);
	fputc(ochb,fpto);
 }

	//count+=1;

fclose(fptr);
fclose(fpto);


printf("Input IQ pairs =%Ld\n",(long long int)couni); 
//printf("Output IQ pairs =%Ld\n",(long long int)count/2); 
printf("\n Infile=%s    Outfile=%s\n",argv[1],argv[2]);
exit(0);
}


