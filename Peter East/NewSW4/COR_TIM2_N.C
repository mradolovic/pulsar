
/*cor_tim2_N.C	   pwe: 25/02/15, 28/04/2016
Takes rtlsdr bin file takes first block of 'sample length', N/4 IQ samples in and correlates
  this block along the second .bin file and vice versa, to find the correlation peak.
  Sample length < block length/2.


 Command format:- cor_tim2_N <infile1> <infile2> <outfile> <sample length> <Block size> <No of IQ blocks tested> */

#include <stdio.h>

#include <stdlib.h>

#include <math.h>

#define SWAP(a,b) tempr=(a); (a)=(b); (b)=tempr
#define PI (6.28318530717959/2.0)
#define PTS 256
#define M 2097152+1024


float r1,r2,rr1,rr2,mx1=0,mx2=0;
long int pts=256,blks,slen;
long int N=M-1024;
long int offset;
long long int file_end,file2_end,count=0,p_num,nmx1=0,nmx2=0;
float dats[M],dats2[M],dato1[M],dato2[M];
float  ra[M], rb[M],rar[M],rbr[M];
unsigned char ucha,ucha2;

 long int t,s,ss;
 long int sl;
 long int n,nn,tt,co;

FILE *fpto;
FILE *of;

void out_dat(long int, int);
/*void four(float[],long int,int);*/
void cor_dat(long int,int);

  main(int argc,char *argv[])
 {


FILE *fptr;
FILE *fptr2;
/*check command line arguments*/
if(argc !=7)
  { printf("Format: cor_tim2_n <infile1> <infile2> <outfile> <sample length> <block size> <No of IQ blocks tested>\n");exit(0); }



if((fptr=fopen(argv[1],"rb"))==NULL)
   {printf("Can't open file %s. \n",argv[1]);exit(0);}
if((fptr2=fopen(argv[2],"rb"))==NULL)
   {printf("Can't open file %s. \n",argv[2]);exit(0);}
slen=atol(argv[4]);
N=atol(argv[5]);
if(slen>=N/4)
   {printf("Sample length too large\n");exit(0);}
blks=atol(argv[6]);
//N=65536;
/*printf("pts=\n%d   %c\n",pts,argv[4]);*/
offset=N/4;
fpto = fopen(argv[3],"w");
pts=256;

/*find lengths of input files*/
fseeko(fptr,0L,SEEK_END);
file_end=(long long int)(ftello(fptr));
fseeko(fptr2,0L,SEEK_END);
file2_end=(long long int)(ftello(fptr2));
if(file_end != file2_end)
   {printf("Infiles unequal length\n");exit(0);}
//blks=10000;
p_num=(long long int)(file_end/N);
if(blks/2<p_num)p_num=blks/2;
printf("No. Input Bytes = %lld\n",file_end);
printf("No. of IQ Samples Checked = %lld\n\n",(p_num*N));
printf("Block	Sample	Corr	Offset	Corr	Offset\n");
printf("Size	Width	Peak1	1 on 2	Peak2   2 on 1\n\n");
fclose(fptr);


fptr=fopen(argv[1],"rb");
fptr2=fopen(argv[2],"rb");




/*p_num=4;*/
ss=0;
r1=0;rr1=0;
	r2=0;rr2=0;

   for(ss=0;ss<p_num*2;ss++)
    {

 for(s=0;s<N;s++)
	{
	ucha=getc(fptr);
	ucha2=getc(fptr2);
	dats[s]=(float)ucha+0.0;	/* printf("%f\n",dats[s]);*/
	dats2[s]=(float)ucha2+0.0;


	    dats[s]=(dats[s]-127.5)/128.0;
	    dats2[s]=(dats2[s]-127.5)/128.0;

				 /*printf("%d     %d     %d  %d   %d\n",PTS,da[s],ucha,ra,ra);*/

	/*printf("%f\n",dats[s]);*/
	 }
					/*printf("%d\n", N);*/
	 if(ss==0)
	    {
	    for(nn=0;nn<N/2;nn++)
		{			/*printf("%d   %d\n",nn, N);*/
	     ra[nn]= sqrt(dats[2*nn]*dats[2*nn]+dats[2*nn+1]*dats[2*nn+1]);
	     rb[nn]= sqrt(dats2[2*nn]*dats2[2*nn]+dats2[2*nn+1]*dats2[2*nn+1]);

	    if(nn>=offset && nn<slen+offset){
			r1=r1+(1.0/(slen))*ra[nn];
			r2=r2+(1.0/(slen))*rb[nn];
			rr1=rr1+(1.0/(slen))*ra[nn]*ra[nn];
			rr2=rr2+(1.0/(slen))*rb[nn]*rb[nn];
			       rar[nn-offset]=ra[nn];
			       rbr[nn-offset]=rb[nn];
			       /*	printf("%f  %f  %d\n",r1,r2,slen);*/
			}
		}rr1=sqrt(rr1-r1*r1);
		  rr2=sqrt(rr2-r2*r2);
	     /*printf("%f\n",dats[n]);*/
       cor_dat((long)(N/2-slen),0);
       out_dat(N/2-slen,0);
	     {

		 for(tt=0;tt<slen;tt++)
		     { ra[tt]=ra[N/2-slen+tt];
		       rb[tt]=ra[N/2-slen+tt];
		      }

		}

	     } /*printf("%f  %f  %d\n",r1,r2,slen);*/

	   else{

					    /*printf("%d\n", N);*/
      for(nn=0;nn<N/2;nn++)
	   {				    /*printf("%d   %d\n",nn, N);*/
	       ra[nn+slen]= sqrt(dats[2*nn]*dats[2*nn]+dats[2*nn+1]*dats[2*nn+1]);
	       rb[nn+slen]= sqrt(dats2[2*nn]*dats2[2*nn]+dats2[2*nn+1]*dats2[2*nn+1]);
	   }



      cor_dat((long)(N/2),1);
      out_dat(N/2,1);
   {
		 for(tt=0;tt<slen;tt++)
		     { ra[tt]=ra[N/2+tt];
		       rb[tt]=ra[N/2+tt];
		      }
   }
  }

   }

//N=65536;
/*printf("%d   %f    %f\n",N/2, mx1,mx2);*/

fclose(fptr);
fclose(fpto);
printf("\nInfile1=%s    Infile2=%s    Outfile=%s   No Samples=%ld\n",argv[1],argv[2],argv[3],atol(argv[4]));

/*getche();*/

exit(0);
}




void cor_dat(long int a,int c)
 /*int a,c;*/
 {
long int nnn,sl;

 //N=65536;


/*correlation*/
    for(nnn=0;nnn<(a);nnn++){ dato1[nnn]=0;dato2[nnn]=0;}
        for(nnn=0;nnn<(a);nnn++){
       for(sl=0;sl<slen;sl++){
	dato1[nnn]=dato1[nnn]+ (1.0/slen)*((rar[sl]-r1)/rr1)*((rb[nnn+sl]-r2)/rr2);
	dato2[nnn]=dato2[nnn]+ (1.0/(slen))*((rbr[sl]-r2)/rr2)*((ra[nnn+sl]-r1)/rr1);

			   }
		   if(dato1[nnn]>mx1){mx1=dato1[nnn];nmx1=(long)(nnn+count*N/2-c*slen);}
		   if(dato2[nnn]>mx2){mx2=dato2[nnn]; nmx2=(long)(nnn+count*N/2-c*slen);}
			}
   count++;
   printf("%ld	%ld	%1.3f	%ld	%1.3f	%ld\n",N/2,slen, mx1,(long)(nmx1-offset),mx2,(long)(nmx2-offset));
   //printf("%f   %f  %f  %f\n",r1,r2,mx1,dato2[slen/2]);
   //mx1=0; mx2=0;
   }


 void out_dat(long int b, int c)
 {
int tt;
float numb;

//N=65536;

for(tt=0;tt<b;tt++){
numb=(float)(1.0*tt+1.0*co*1.0*N/2-1.0*c*slen-1.0*offset);
fprintf(fpto,"%.0f    %3.7f    %3.7f\n",numb,(float)dato1[tt],(float)dato2[tt]);

}

co=co+1;
printf("No IQ amplitudes cross-correlated: = %.0f\n", (numb+offset));
/*fclose(fpto);*/
}
