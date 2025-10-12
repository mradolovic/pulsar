
/*pdetfilt2.c	   pwe: 02/04/2015
Takes rtlsdr .txt files from pdetect program, takes n-point fft to produce video spectrum, cal file 0,1 blanks part of spectrum with interference, 
spect file outputs new video spectrum, outfile contains interference removed data for viewing with rapulsar_avg. 

Command format:- pdetfilt2 <infile> <outfile> <specfile> <calfile> <fft points> */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SWAP(a,b) tempr=(a); (a)=(b); (b)=tempr
#define PI (6.28318530717959/2.0)

long long int count=0,blocks;

float PTS, sum,pts,di,dq;
int ftpts,D,flag,bn,cn,tt;
long long int file_end,p_num,ppts,cnt;
float clck;
float smt=0,mean=0,cal=0;
float calf[131072];

double dats[131072],datr[131072],dato[131072];
unsigned char ucha;

FILE *fptr;
FILE *fpto;
FILE *fptc;
FILE *fptd;

void out_dat(void);
void sum_dat(void);
void four(double[],int,int);
void spec_out(void);
void write_spec(void);


main(int argc,char *argv[])

{

long long int ss;
long int s,sss;



/*check command line arguments*/
if(argc !=6)
    { printf("Format: pdetfilt2 <infile> <outfile> <specfile> <calfile> <fft points> \n");
    exit(0);
    }

if((fptr=fopen(argv[1],"r"))==NULL)
    {printf("Can't open file %s. ",argv[1]);
    exit(0);
    }
if((fptd=fopen(argv[4],"r"))==NULL)
    {printf("Can't open file %s. ",argv[4]);
    exit(0);
    }

//fptr=fopen(argv[1],"r");
//clck=atof(argv[3]); //data clock frequency
ftpts=atoi(argv[5]);
//PTS=atof(argv[4]);  //video bandwidth
//pts=(clck*1000.0/PTS); // ratio of clock to video number of data points averaged - or downsampling ratio
if(ftpts != 1 && ftpts != 2 && ftpts != 4 && ftpts != 8 && ftpts != 16 && ftpts != 32 && ftpts != 64 && ftpts != 128 && ftpts != 256 && ftpts != 512 && ftpts != 1024 && ftpts != 2048 && ftpts != 4096 && ftpts != 8192 && ftpts != 16384 && ftpts != 32768 && ftpts != 65536)
    {printf("FFT points not a power of 2 or too big!");
    exit(0);
    }
while(fscanf(fptr,"%f",&smt) !=EOF)
	{ //if (c == '\n') // Increment count if this character is newline fscanf(fptr,"%f",&smt);
		
		{//printf("FileDat=%f\n",smt);
		mean=mean+(float)smt;}
		cnt = cnt + 1;
		}
mean=(float)mean/(float)cnt;

//ppts=(long int)pts;

//clck=1/clck;
//D=atoi(argv[6]);


   //printf("Data Clock=%1.2fus\n",clck);
   // printf("Video BW=%1.3fkHz\n",PTS);
	//printf("RF/Video=%lld\n",ppts);
	//printf("FileDivisor=%d\n",D);
	printf("FileCount=%d\n",cnt);
	printf("FileMean=%f\n",mean);

/*find length of input file*/
//fseeko(fptr,0L,SEEK_END);
//file_end=(long long int)(ftello(fptr));
file_end=cnt;



p_num=(long long int)(file_end/ftpts); //number of data blocks

printf("No. Bytes = %lld\n",(file_end));
printf("FFT Points=%d\n",ftpts);
printf("No. FFT Blocks=%d\n",p_num);
//printf("No. of Blocks=%lld\n",p_num);
//printf("DownSamples/Channel=%lld\n",ppts/ftpts);

//;
fclose(fptr);

fptr =fopen(argv[1],"r"); //infile
fpto = fopen(argv[2],"w");//outfile
fptc = fopen(argv[3],"w");//spectrum file
fptd = fopen(argv[4],"r");// cal file
blocks=(long long)(file_end/(long long)(ftpts*2));
/*read input file,decode I and Q, determine power. Sum powers in clock rate/video band blocks.
 At end of input file, output text file with averaged data*/

char ch,chn;
for(bn=0;bn<ftpts;bn++) 
{fscanf(fptd,"%f",&cal);
fscanf(fptd,"%f",&cal);
//ch=(int)fgetc(fptd);
//chn=(int)fgetc(fptd);
//if(ch != '\n')
//if(bn%2==1)
{
calf[bn]=(float)(cal);
//if(calf[bn]<1)calf[bn]=calf[0];
printf("%d     %f\n",bn,calf[bn]);
}
}
 

 //for(ss=0;ss<p_num;ss++)  //number of data blocks
   { 
 
 for(s=0;s<p_num;s+=1) //number of data samples averaged
	{
 
		for(sss=0;sss<ftpts;sss++) //number of data samples for fft
			{
			//ucha=getc(fptr);
			fscanf(fptr,"%f",&smt); 
			
			{dats[2*sss]=(float)(smt-mean); /* printf("%f\n",dats[s]);*/
			dats[2*sss+1]=(float)0.0;}
			//dats[sss]=(dats[sss]-127.5)/128.0;
			}

 /*take fourier transform*/
		four(dats-1,ftpts,-1);

for(tt=0;tt<ftpts;tt++){
	dats[2*tt]=dats[2*tt]*calf[tt];//+(1-(int)calf[tt])*dats[2*1000];//(int)(calf[tt])+(1-(int)calf[tt])
	dats[2*tt+1]=dats[(2*tt+1)]*calf[tt];//+(1-(int)calf[tt])*dats[2*1000+1];//*calf[tt];

	//dats[2*(ftpts-tt)]=dats[(2*(ftpts-tt))];//*calf[tt]+(1-(int)calf[tt])*dats[2*1000];
	//dats[2*(ftpts-tt)+1]=dats[(2*(ftpts-tt)+1)]*calf[tt];//+(1-(int)calf[tt])*dats[2*1000+1];
		}		

spec_out();
//if(s==10){for(tt=0;tt<ftpts/2;tt++){
	

//fprintf(fptc,"%3.5f	\n",(float)(dats[2*tt])*dats[2*tt]); //datr[2*tt]*datr[2*tt]+datr[2*tt+1]*datr[2*tt+1])
//}}		
	


four(dats-1,ftpts,1);	
		
			
for(tt=0;tt<ftpts;tt++){
	
fprintf(fpto,"%3.5f	\n",(float)(dats[2*tt]/ftpts+mean)); //datr[2*tt]*datr[2*tt]+datr[2*tt+1]*datr[2*tt+1])
} 	
	
	//sum_dat();
	
	}
	
	count+=1;
		     /*printf("%f  %ld\n",PTS,p_num);*/
  }//fprintf(fpto,"%lld   %1.8f\n",count,((float)sum/(float)pts));
 out_dat();   
    /*printf("%d  %f    %ld\n",(int)pts,sum,ss);*/
  //out_dat();
printf("\n Infile=%s    Outfile=%s\n",argv[1],argv[2]);
printf("Blocks=%lld\n",count);
fclose(fptr);
fclose(fpto);
fclose(fptc);
fclose(fptd);

exit(0);
}



void sum_dat(void)
{
long int tt;
			/* dats[tt];*/
float opp;

for(tt=0;tt<ftpts;tt++){
datr[2*tt]=datr[2*tt]+dats[2*tt]*dats[2*tt];
datr[2*tt+1]=datr[2*tt+1]+dats[2*tt+1]*dats[2*tt+1];
dats[2*tt]=0;dats[2*tt+1]=0;
/*printf("%d\n",tt);*/

						}

}

void out_dat(void)
{
long int tt;
float opp;

//for(tt=0;tt<ftpts;tt++){
//		opp=(float)datr[tt];
//		datr[tt]=0;
for(tt=0;tt<ftpts/2;tt++){
	//if(tt<(ftpts/2)){opp=(float)datr[tt+ftpts/2];datr[tt+ftpts/2]=0;}
	//else  {opp=(float)datr[tt-ftpts/2];	datr[tt-ftpts/2]=0;}  

/*printf("%d\n",tt);*/
//fprintf(fpto,"%3.5f	",(float)(opp*(float)ftpts/(float)ppts));   //*(float)ftpts/(float)p_num
fprintf(fptc,"%3.5f	\n",(float)(dato[tt])/p_num); //datr[2*tt]*datr[2*tt]+datr[2*tt+1]*datr[2*tt+1])
} //fprintf(fpto,"\n");
//fclose(fpto);
}

void spec_out(void)
{
int tt,bnn;
float opp;
unsigned char  ocha;


	{//if((an==0 && bnn>=datft[ftpts])||an>0){	
	for(cn=0;cn<ftpts/2;cn++){

		dato[cn]=dato[cn]+dats[2*cn]*dats[2*cn];
		}
		}
}

float gau(float x, float a, float b)
{
float res;
 res=exp(-((x-a)/b)*((x-a)/b));
return(res);	
}

void write_spec(void)
{
long int tt;
int num=0;
for(tt=0;tt<ftpts;tt++)
{

//fprintf(fptc,"%1.8f\n",dato[tt]/(float)blocks/(float)blocks);
fprintf(fptc,"%3.5f	\n",(float)(datr[2*tt]+datr[2*tt+1])/p_num);

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
  if(isign==1){for(a=0;a<2*pts+1;a++){
  data[a]=data[a]/pts;}}
 }
