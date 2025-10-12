
/*de-dispers2.c	   pwe: 03/10/2015 - 03/02/2016
Takes rtlsdr .bin files, takes n-point fft, de-disperses frequency components in time, sums component amplitudes, ready for folding. 
Outputs text file of dispersion corrected data at the initial clock rate, reduced by a factor equal to the number of fft points.

Command format:- de-dispers2 <infile> <outfile> <clock rate(MHz)> <FFT points> <RF(MHz)><DM>*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SWAP(a,b) tempr=(a); (a)=(b); (b)=tempr
#define PI (6.28318530717959/2.0)

long long int count=0;

float res, sum,dsr,di,dq,dt,RF,DCF,df;
int ftpts,DM,fn,ddt,bn,cn,flag,ddrry=4096,offset=0,invert=1,noffts;
long long int file_end,an,p_num,DSR;
float clck,datft[8192];
float smt;
double dats[16384],datr[16384],dato[8192],summ=0;
double ddarray[10000][8192];
unsigned char ucha;

FILE *fptr;
FILE *fpto;

void out_dat(void);
void ftorg_dat(void);
void sum_dat(void);
void four(double[],int,int);

main(int argc,char *argv[])

{

long long int ss;
long int s,sss;



/*check command line arguments*/
if(argc != 7)
    { printf("Format: de-dispers2 <infile> <outfile> <clock rate (MHz) <fft points> <RF centre(MHz)> <Dispersion measure, +,norma;; -,invert>\n");
    exit(0);
    }

if((fptr=fopen(argv[1],"rb"))==NULL)
    {printf("Can't open file %s. ",argv[1]);
    exit(0);
    }

DCF=atof(argv[3]); //data clock frequency
ftpts=atoi(argv[4]); //FFT points
//res=atof(argv[4]);  //output data time resolution
//dsr=(DCF*1000.0*res); // ratio of clock to video number of data points averaged - or downsampling ratio
if(ftpts != 1 && ftpts != 2 && ftpts != 4 && ftpts != 8 && ftpts != 16 && ftpts != 32 && ftpts != 64 && ftpts != 128 && ftpts != 256 && ftpts != 512 && ftpts != 1024 && ftpts != 2048 && ftpts != 4096 && ftpts != 8192)
    {printf("FFT points not a power of 2 or too big!");
    exit(0);
    }
//DSR=(long int)dsr; //downsampling ratio

clck=1/DCF; //data clock interval
RF=atof(argv[5]);
DM=atof(argv[6]); //dispersion measure
if(DM<0){invert=-1;
offset = ftpts-1;
DM = invert*DM;}
df=DCF/2; //+- frequency band limits
dt=((10000000.0)/2.410331)*(4*RF*df)*DM/(RF*RF-df*df)/(RF*RF-df*df);// dispersion time across frequency band (ms)
ddt=(int)(1000.0*dt); //dispersion time in us
noffts=ddt/2/ftpts/clck-1;
	{for(fn=0;fn<ftpts;fn+=1)
	    {datft[fn]=((float)fn*ddt/(float)ftpts/(float)ftpts/clck+.5); //fft bin delay required to de-disperse in FFT time blocks  
	printf("FFT bin %d	Dispersion %d\n",(int)fn,(int)(datft[fn]));
		}
	}

   printf("Data Clock Interval=%1.2fus\n",clck);
   printf("Band Dispersion=%1.3fms\n",dt);
   printf("FFT bin Dispersion intervals=%d\n",((int)datft[1]-(int)datft[0]));

	
/*find length of input file*/
fseeko(fptr,0L,SEEK_END);
file_end=(long long int)(ftello(fptr));


printf("No. Bytes = %lld\n",(file_end));
printf("FFT Points=%d\n",ftpts);
printf("New Data Clock Rate=%1.9fMHz\n",(float)DCF/(float)ftpts);


fclose(fptr);

fptr=fopen(argv[1],"rb");
fpto = fopen(argv[2],"w");

/*read input file,decode I and Q, determine power. Sum powers in clock rate/video band blocks.
 At end of input file, output text file with averaged data*/
ddrry=4096;
 for(an=0;an<(file_end/(long long)(ddrry*ftpts*2));an++)
  {
	for(bn=0;bn<ddrry;bn++)
	{ if(an%2==0)flag=0; else flag=1;
	
		for(fn=0;fn<2*ftpts;fn++)
		{//get sdr binfile data for FFT
		ucha=getc(fptr);
			dats[fn]=(float)ucha+0.0; /* printf("%f\n",dats[s]);*/
			dats[fn]=(dats[fn]-127.5)/128.0;
		
		}
	/*take fourier transform*/
	four(dats-1,ftpts,-1);
	/*reorganise spectrum positive/negative data*/
	ftorg_dat();
		//de-disperse data, place in array
		for(cn=0;cn<ftpts;cn++)
		{
		  ddarray[(bn+flag*4096+(int)datft[offset+invert*cn])%8192][cn]=datr[cn];
		}
	}//printf("bn count=%d\n",bn);
	
	/*output summed FFT data to output file*/
	out_dat();
 }

	//count+=1;

fclose(fptr);
fclose(fpto);

printf("\n Infile=%s    Outfile=%s\n",argv[1],argv[2]);

exit(0);
}




void out_dat(void)
{
int tt,bnn;
float opp;

for(bnn=(0+flag*ddrry);bnn<(ddrry+flag*ddrry);bnn++)
	{opp=0;
   //sum data across spectrum
	for(tt=0;tt<ftpts;tt++)
		{
		opp=opp+ddarray[bnn][tt];
		}
/*output data to file*/
	
    //count+=1;
	if((an==0 && bnn>noffts)||an>0)//datft[ftpts/2])||an>0)
		
		fprintf(fpto,"%.4f\n",(float)(opp/(float)ftpts/(float)ftpts));
	} //printf("%d	%f\n",(int)bnn, opp); 
}



void ftorg_dat(void)
{
long int tt;
float opp;

for(tt=0;tt<ftpts;tt++)
	{
		if(tt<(ftpts/2)){datr[tt+ftpts/2]=dats[2*tt]*dats[2*tt]+dats[2*tt+1]*dats[2*tt+1];}
		else  {datr[tt-ftpts/2]=dats[2*tt]*dats[2*tt]+dats[2*tt+1]*dats[2*tt+1];}  	
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
  if(isign==1){for(a=0;a<2*nn;a++){
  data[a]=data[a]/nn;}}
 }
