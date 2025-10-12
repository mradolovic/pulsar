/*f_align2.c	   shortens binary files  31/7/91  27/02/15
         \Format: C> f_align2 <Oldfile> <Newfile> <No of 8192 IQ blocks> <Align
			 Address(0)>*/
#include <stdio.h>
#include <stdlib.h>
char ch,fl[32768];	  /*maximum of 32768 elements transferred per loop*/
long long int sa,st,n,p,t,tt,rr;
int g=1,r;


 main(int argc,char *argv[])
/*int argc;
char *argv[];*/

{
FILE *fpta,*fptb;              /*set up file structures*/

if(argc !=5)                   /*check correct No of command elements*/
			       /*print command line instructions if wrong*/
   {printf("Format: C>f_align2 <Oldfile> <Newfile> <No 8192 IQ blocks> <Align address,0->\n");
    exit(0);}

st=(atoll(argv[4]))*2+14000;		/*integer value of start address*/



p=(long long int)(atoll(argv[3])*8192*2);		/*integer value of No of bytes to transfer*/
printf("Start=%lld   Length=%lld\n",st,p);
if((fpta=fopen(argv[1],"rb"))==NULL)   /*open Oldfile*/
  {printf("Can't open file %s.\n",argv[1]);exit(0);}

fptb = fopen(argv[2],"wb");            /*open Newfile*/

/*handles offset start address*/
if(st>0) { for(sa=0;sa<st;sa++){fl[0]=getc(fpta);}}

while(g)		       /*handles case of more than 32768 elements*/
   {
   if(p>=32768){n=32768;p=p-n;} else {n=p; g=0;}



/*for(t=0;t<1;t++)*/
{             /*byte transfer routine*/
for(tt=0;tt<n;tt++){fl[tt]=getc(fpta);}
for(rr=0;rr<n;rr++){putc(fl[rr],fptb);}}}

fclose(fpta);
fclose(fptb);
exit(0);
 }
