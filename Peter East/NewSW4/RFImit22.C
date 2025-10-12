
/*RFImit22.c	   pwe: 03/10/2015 - 03/02/2016 Coherent De-dispersion.
Takes rtlsdr .bin files, takes n-point fft, reorganises spectrum to remove negative frequency alias, 
outputs the spectrum, reads RFI blanking file, blanks as required, re-aliases spectrum data, takes inverse FFT, outputs data in original .bin form less RFI. 


Command format:- RFImit22 <infile> <outfile> <specfile> <blank file> <FFT points> */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SWAP(a,b) tempr=(a); (a)=(b); (b)=tempr
#define PI (6.28318530717959/2.0)

long long int count=0;

float res, sum,dsr,di,dq,dt,RF,DCF,df,DM;
int ftpts,fn,ddt,bn,cn,flag,ddarry=4096, invert=1,offset=0,noffts;
long long int file_end,an,p_num,DSR,couni=0,blocks;
float clck,datft[131072];
float smt;
int calf[131072];
double dats[131072],datr[131072],dato[131072],summ=0;
unsigned char ucha,uchb;

FILE *fptr;
FILE *fpto;
FILE *fptc;
FILE *fptd;


void out_dat(void);
void spec_out(void);
void write_spec(void);
void ftorg_dat(void);
void ftreorg_dat(void);
void sum_dat(void);
void four(double[],int,int);

main(int argc,char *argv[])

{

long long int ss;
long int s,sss;



/*check command line arguments*/
if(argc != 6)
    { printf("Format: RFImit2 <infile> <outfile> <specfile> <blankfile> <fft points> \n");
    exit(0);
    }

if((fptr=fopen(argv[1],"rb"))==NULL)
    {printf("Can't open file %s. ",argv[1]);
    exit(0);
    }
if((fptd=fopen(argv[4],"rb"))==NULL)
    {printf("Can't open file %s. ",argv[4]);
    exit(0);
    }


ftpts=atoi(argv[5]); //FFT points

if(ftpts != 1 && ftpts != 2 && ftpts != 4 && ftpts != 8 && ftpts != 16 && ftpts != 32 && ftpts != 64 && ftpts != 128 && ftpts != 256 && ftpts != 512 && ftpts != 1024 && ftpts != 2048 && ftpts != 4096 && ftpts != 8192 && ftpts != 16384 && ftpts != 32768 && ftpts != 65536)
    {printf("FFT points not a power of 2 or too big!");
    exit(0);
    }


	
/*find length of input file*/
fseeko(fptr,0L,SEEK_END);
file_end=(long long int)(ftello(fptr));


printf("No. Bytes = %lld\n",(file_end));
printf("FFT Points=%d\n",ftpts);


fclose(fptr);

fptr=fopen(argv[1],"rb");
fpto = fopen(argv[2],"wb");
fptc = fopen(argv[3],"w");
fptd = fopen(argv[4],"r");
blocks=(long long)(file_end/(long long)(ftpts*2));

char ch,chn;
for(bn=0;bn<ftpts;bn++) 
{
ch=(int)fgetc(fptd);	//gets 1/0
chn=(int)fgetc(fptd);	//gets newline character
//if(ch != '\n')
{
calf[bn]=(int)(ch)-48;
//printf("%d     %d\n",bn,calf[bn]);
}
}
fclose(fptd);


for(an=0;an<blocks;an++)
  {
	
	{ 
	
		for(fn=0;fn<ftpts;fn++)
		{//get sdr binfile data for FFT
		ucha=getc(fptr);
			dats[2*fn]=(float)ucha+0.0; /* printf("%f\n",dats[s]);*/
			dats[2*fn]=(dats[2*fn]-127.4);
		uchb=getc(fptr);
			dats[2*fn+1]=(float)uchb+0.0; /* printf("%f\n",dats[s]);*/
			dats[2*fn+1]=(dats[2*fn+1]-127.4);
			couni +=1;	
		}	//printf("%d   %d   %f    %f\n",(int)ucha,(int)uchb,dats[2*fn],dats[2*fn+1]);
		
		//printf("%f    %f \n",dats[2],dats[3]);
	
		/*take fourier transform*/
	four(dats-1,ftpts,-1);
	

	/*reorganise spectrum positive/negative data*/
	ftorg_dat();
		
	spec_out();	

	out_dat();
	}//printf("bn count=%d\n",bn);
	
	/*output summed FFT data to output file*/
	
 }

 write_spec();
 

fclose(fptr);
fclose(fpto);
fclose(fptc);


printf("Input IQ pairs =%Ld\n",(long long int)couni); 
printf("Output IQ pairs =%Ld\n",(long long int)count/2); 
printf("\n Infile=%s    Outfile=%s\n",argv[1],argv[2]);
exit(0);
}




void out_dat(void)
{
int tt,bnn;
float opp;
unsigned char  ocha;


	{
   
	//re-organise data to conform to input format
	ftreorg_dat();
	//take inverse FFT	
	four(dats-1,ftpts,1);
		
		{
	for(cn=0;cn<2*ftpts;cn++){

		ocha=(char)((int)((float)(dats[cn]+128.0)));
		fputc(ocha,fpto);
		count+=1;
		}
		}

	}
		

}

void spec_out(void)
{
int tt,bnn;
float opp;
unsigned char  ocha;

	
	{
	for(cn=0;cn<ftpts;cn++){

		dato[cn]=dato[cn]+datr[2*cn]*datr[2*cn]+datr[2*cn+1]*datr[2*cn+1];
							}
	}

}



void write_spec(void)
{
long int tt;
int num=0;
for(tt=0;tt<ftpts;tt++)
{

fprintf(fptc,"%1.8f\n",dato[tt]/(float)blocks/(float)blocks);

}
}

void ftorg_dat(void)
{
long int tt,tp;
float opp;

for(tt=0;tt<ftpts;tt++)
	{
	if(tt<(ftpts/2)){datr[2*tt+ftpts]=dats[2*tt];
						datr[2*tt+1+ftpts]=dats[2*tt+1];}
	else  {datr[2*tt-ftpts]=dats[2*tt];
				datr[2*tt+1-ftpts]=dats[2*tt+1];}  	
		
	} 

for(tt=0;tt<ftpts;tt++){
	datr[2*tt]=datr[2*tt]*calf[tt];
	datr[2*tt+1]=datr[2*tt+1]*calf[tt];}

}


void ftreorg_dat(void)
{
long int tt;
float opp;

for(tt=0;tt<ftpts;tt++)
	{
	if(tt<(ftpts/2)){dats[2*tt+ftpts]=datr[2*tt];
						dats[2*tt+1+ftpts]=datr[2*tt+1];}
	else  {dats[2*tt-ftpts]=datr[2*tt];
				dats[2*tt+1-ftpts]=datr[2*tt+1];}  	
			 	
	} 
}




/*fast fourier transform routine*/
void four( double data[], int nn,int isign)
/* float data[];
 int nn,isign;*/
 {
  long int n,mmax,m,j,istep,i,a;
  double wtemp,wr,wpr,wpi,wi,theta;
  double tempr,tempi;
  n=nn<<1;
  j=1;
  for(i=1;i<n;i+=2)
     {
     if(j>i){
	     SWAP(data[j],data[i]);
	     SWAP(data[j+1],data[i+1]);
	     }
     m=n>>1;
     while(m>=2 && j>m){
			j-=m;
			m>>=1;
			}
     j+=m;
     }
  mmax=2;

  while(n>mmax)
    {
     istep=2*mmax;
     theta=6.28318530717959/(isign*mmax);
     wtemp=sin(0.5*theta);
     wpr=-2.0*wtemp*wtemp;
     wpi=sin(theta);
     wr=1.0;
     wi=0.0;
     for(m=1;m<mmax;m+=2){
       for(i=m;i<=n;i+=istep){
	  j=i+mmax;
	  tempr=wr*data[j]-wi*data[j+1];
	  tempi=wr*data[j+1]+wi*data[j];
	  data[j]=data[i]-tempr;
	  data[j+1]=data[i+1]-tempi;
	  data[i]+=tempr;

	  data[i+1]+=tempi;
 if(j<0)j=0;

			      }
       wr=(wtemp=wr)*wpr-wi*wpi+wr;
       wi=wi*wpr+wtemp*wpi+wi;
			  }
     mmax=istep;
/* printf("%d    %d    %d    %d    %d\n",i,n,m,pts,istep);*/
     }
  if(isign==1){for(a=0;a<2*nn+1;a++){
  data[a]=data[a]/nn;}}
 }
