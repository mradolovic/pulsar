
/*pafft22.c	   pwe: 02/04/2015
Takes rtlsdr .bin files, averages power in blocks of size (clock rate)/(video band). 
Performs ffts on averaged data amplitude as required within length of data, averages multiple ffts.
Outputs text file of average of all ffts.

Command format:- pafft22 <infile> <outfile> <clock rate(MHz)> <video band(kHz)> <fft size> */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SWAP(a,b) tempr=(a); (a)=(b); (b)=tempr
#define PI (6.28318530717959/2.0)

long long int count=0;

float vid, sum,vpts,di,dq;
float divv;
long int fft,vppts,rat;
long long int file_end,v_num,fft_num;
long int s,t;
long int ss,sss;
float clck;
float smt;
double dats[4194304],datr[4194304];
unsigned char ucha;



FILE *fptr;
FILE *fpto;

void sum_dat(void);
void out_dat(void);
void four(double[],long int,int);


main(int argc,char *argv[])

{


/*check command line arguments*/
if(argc !=6)
    { printf("Format: pafft22 <infile> <outfile> <clock rate (MHz) <video band (kHz)> <fft size>\n");
    exit(0);
    }

if((fptr=fopen(argv[1],"rb"))==NULL)
    {printf("Can't open file %s. ",argv[1]);
    exit(0);
    }

clck=atof(argv[3]);
vid=atof(argv[4]);
vpts=(clck*1000.0/vid);

vppts=(long int)vpts;

clck=1/clck;
fft=atol(argv[5]);
if(fft>4194304)
 {
printf("Too many FFT points");exit(0);}
 /* printf("%ld  %1.2f   %d\n",vppts,clck,fft);*/



/*find length of input file*/
fseeko(fptr,0L,SEEK_END);
file_end=(long long int)(ftello(fptr));




v_num=(long long int)((double)file_end/(double)(vpts*2.0));
fft_num = (long int)(v_num/(long long)fft);
rat=fft_num*fft;
divv = (float)sqrt(2.0*(float)rat);
printf("Clock us=%1.4f \n",clck);
printf("No. FFTs=%lld\n",fft_num);
printf("No. Bytes = %lld \n",(file_end));
printf("Video Points=%lld \n",v_num);
printf("VideoSamples=%ld \n",vppts);
printf("FFT-points=%ld\n",fft);
fclose(fptr);

fptr=fopen(argv[1],"rb");


/*read input file,decode I and Q, determine power. Sum powers in clock rate/video band blocks.
 perform FFTs, average then output text file with averaged spectrum data*/

 {
 for(sss=0;sss<fft_num;sss++)
    { 
		for(ss=0;ss<fft;ss++)
		{ sum=0;
            
			for(s=(int)(ss*vpts);s<(int)((ss+1)*vpts);s+=1)
			{
			ucha=getc(fptr);
			di=(float)ucha+0.0;
			di=(di-127.5)/128.0;

			ucha=getc(fptr);
			dq=(float)ucha+0.0;
			dq=(dq-127.5)/128.0;

			smt=di*di+dq*dq;
			sum=sum+smt;					/*printf("%d  %ld\n",fft_num,v_num);*/		     
			}
		
        dats[2*ss]=sum/vpts; dats[2*ss+1]=0;
	
		/*fprintf(fpto,"%ld   %1.5f\n",count,((float)sum/(float)pts));*/
		count+=1;
    /*printf("%d  %f    %ld\n",(int)pts,sum,ss);*/
    }
/*printf("%lld\n",sss);*/
/*take fourier transform*/
	four(dats-1,fft,-1);
	sum_dat();
 }

 }
fclose(fptr);
fpto = fopen(argv[2],"w");
for(t=0;t<fft/2;t++){

/*printf("%d\n",tt);*/
fprintf(fpto,"%ld    %3.6f\n",t,(datr[t]/divv));

}/*printf("%ld\n",t);*/




printf("\nInfile=%s    Outfile=%s\n",argv[1],argv[2]);

/*printf("Infile=%lld  FFT points=%ld   Video Points=%lld\n",file_end, fft,v_num);*/
fclose(fpto);
exit(0);
}


void out_dat(void)
{
long int t;
float div;
divv = sqrt(2.0*(float)fft_num*(float)fft);
for(t=0;t<fft/2;t++){

/*printf("%d\n",tt);*/
fprintf(fpto,"%ld    %3.6f\n",t,(datr[t]/divv));

}printf("%ld\n",t);

}


void sum_dat(void)
{
long int tt;
			/* dats[tt];*/

for(tt=0;tt<fft/2;tt++){
datr[tt]=datr[tt]+(float) sqrt(dats[2*tt]*dats[2*tt]+dats[2*tt+1]*dats[2*tt+1]);

}
/*printf("%lld    %lld\n",sss,count);*/
}



/*fast fourier transform routine*/
void four( double data[], long int nn,int isign)
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
  if(isign==1){for(a=0;a<2*fft;a++){
  data[a]=data[a]/fft;}}
 }
