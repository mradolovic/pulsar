
/*rn_rtlat2.C	   pwe:  20/09/2014
Uses PC clock to run an rtl_sdr program at a set time, generates .bin file.
Uses .bin file to calculate N-point FFT and averages these for the output
text file.

Command format:-
	rn_rtlat2 <\"rtl_sdr2 data_file.bin + command line\"> <No FFT points> <hr> <min> <sec>*/



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define SWAP(a,b) tempr=(a); (a)=(b); (b)=tempr
#define PI (6.28318530717959/2.0)

FILE *fpto;
FILE *fptr;

char binfile[20];
char textfile[20];
char sdrprog[60];
long int pts;
long long int p_num;
double dats[524288],datr[262144];

void ext_bin(void);
void ext_txt(void);
void sum_dat(void);
void out_dat(void);
void four(double[],long int,int);
void rafft22(void);

main(int argc, char *argv[])
{

int c,n=0,tcor;
long int i,hr=0,mn=0,sc=0;
long int t,s,t_hr,t_dy,t_ep,t_off,t_mn,t_sc;
long long int ss;



   {time_t in_second;
    in_second = time(NULL);
    printf("%s\n",ctime(&in_second));
   }




if(argc <7 )
 {
printf("Format: rn_rtlat2 <\"./rtl_sdr2 Binfile.bin + rtl commands\"> <No FFT points> <hr> <min> <sec> <Hr offset>\n");
{time_t in_second;
 in_second = time(NULL);
 printf("%s\n",ctime(&in_second));
}
exit(0);
 }

hr=(long)atoi(argv[3]);
mn=(long)atoi(argv[4]);
sc=(long)atoi(argv[5]);
tcor=atoi(argv[6]);

pts=atol(argv[2]);

if(pts>262144)
 {
printf("Too many FFT points\n");exit(0);}


printf("Program Run Time: %ld hr  %ld min  %ld secs\n", hr,mn,sc );


strcpy(sdrprog,argv[1]);

/*find bin file*/
ext_bin();
/*make txt output file*/
ext_txt();

/* start rtl_sdr at nominated time*/
{
 time_t in_seconds;
in_seconds = time(NULL);
t_hr = (long int)((in_seconds/3600));
t_mn = (long int)((in_seconds%3600)/60);
t_sc = (long int)(in_seconds%60);

 printf("Check: %ld hours %ld  mins %ld secs\n" , (t_hr+tcor)%24,t_mn,t_sc);

if((((t_hr+tcor)%24)*3600+(t_mn*60)+t_sc)>=((hr*3600)+(mn*60)+sc)){
                printf("\nToo late!\n");exit(0); }

/* Wait until nominated time*/
while(in_seconds%60 !=sc || (in_seconds%3600)/60 !=mn || (in_seconds/3600+tcor)%24 != hr )

in_seconds = time(NULL);


/*Run rtl_sdr software*/
(void)system(sdrprog);

{

in_seconds = time(NULL);
   t_hr = in_seconds/3600;
   t_mn = (in_seconds%3600)/60;
   t_sc = in_seconds%60;


printf("Binfile complete:  %ldhours %ldmins %ldsecs\n" , (t_hr+tcor)%24,t_mn,t_sc);
 }

/* run fft averaging software*/
rafft22();
/*printf("Data Files: %s  %s %d\n",binfile,textfile, p_num);*/

{

in_seconds = time(NULL);
   t_hr = in_seconds/3600;
   t_mn = (in_seconds%3600)/60;
   t_sc = in_seconds%60;


printf("Finish time:  %ldhours %ldmins %ldsecs\n" , (t_hr+tcor)%24,t_mn,t_sc);
 }
}

return(0);
}/*end*/



/* find bin file */
void ext_bin(void)
{

int i,j,letxt,lebin;
char st;
st=' ';
    letxt=strlen(sdrprog);

    i=0;
    while(sdrprog[i] !=st)
    {/*printf("%d",i);*/i++;}

    j=i;
    while(sdrprog[i+1] !=st)
    {binfile[i-j]=sdrprog[i+1];
    /*printf("%d\n",j);*/ i++;} 
    /* printf("%s \n",binfile);*/

}


/* use bin file root, attach .txt*/
void ext_txt(void)
{

int ii,lebin;
char st;
st='.';
    lebin=strlen(binfile);

    ii=0;
    while(binfile[ii] !=st)
    {textfile[ii]=binfile[ii]; ii++;}

    strcat(textfile, ".txt");


   /*printf("%s %s %d %d\n",binfile,textfile,ii,lebin);*/

}




/*rafft22.c     pwe: 02/06/13
Takes rtlsdr bin files applies FFT in blocks and averages over the input data length*/


void rafft22(void)
{


long long int file_end,count;

unsigned char ucha;

long int t,s;
long long int ss;

if((fptr=fopen(binfile,"rb"))==NULL)
        {printf("Can't open file %s. \n",binfile);
	    exit(0);
	    }

fpto = fopen(textfile,"w");

fptr=fopen(binfile,"rb");

/*find length of input file*/
fseeko(fptr,0L,SEEK_END);
file_end=(long long int)(ftello(fptr));

p_num=(long long int)(file_end/pts/2);

printf("No. of Bytes = %lld   No. of FFTs=%lld\n",file_end,p_num);
fclose(fptr);

fptr=fopen(binfile,"rb");
/*read bin file, decode I and q, put in data file apply fft, apply running average.
  At end of input file, output text file with averaged data*/

 {

 for(ss=0;ss<p_num;ss++)
    {

 for(s=0;s<2*pts;s++)
	{
    ucha=getc(fptr);

	dats[s]=(float)ucha+0.0; /* printf("%f\n",dats[s]);*/

    dats[s]=(dats[s]-127.5)/128.0;


				 /*printf("%d     %d     %d  %d   %d\n",PTS,da[s],ucha,ra,ra);*/
	count++;
	/*printf("%d\n",count);*/
	}


/*take fourier transform*/
	four(dats-1,pts,-1);

/* sum fft's*/
	sum_dat();
    }

}
/* copy final average to output txt file*/
out_dat();
fclose(fptr);

printf("\nInfile=%s    Outfile=%s   FFT Points=%ld\n",binfile,textfile,pts);
}

void *__gxx_personality_v0;

/*txt data out function*/
void out_dat(void)
{
int tt;
			/* dats[tt];*/
float opp;

for(tt=0;tt<pts;tt++){
if(tt<(pts/2))opp=(float)datr[tt+pts/2];
	   else  opp=(float)datr[tt-pts/2];

/*printf("%d\n",tt);*/
fprintf(fpto,"%ld    %3.3f\n",(tt-pts/2),(float)(opp/p_num));

		     }/* printf("%d\n",tt);*/
fclose(fpto);
}

/*fft summing function*/
void sum_dat(void)
{
int tt;
			/* dats[tt];*/
float opp;

for(tt=0;tt<pts;tt++){
datr[tt]=datr[tt]+(float) (dats[2*tt]*dats[2*tt]+dats[2*tt+1]*dats[2*tt+1]);


		     }

}





/*fast fourier transform routine*/
void four( double data[], long int nn,int isign)

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

     }
  if(isign==1){for(a=0;a<2*pts;a++){
  data[a]=data[a]/pts;}}
 }
