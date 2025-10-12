/*rn_prgat.c	   pwe:  12/05/2014
Uses PC clock to run an .exe program at a pre-determined time
	   Command format:- rn_prgat <\"Runfile+ command line\"> <hr> <min> <sec>*/



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

FILE *fptin;

long int i,hr=0,mn=0,sc=0;
long int t,s,ss,t_hr,t_dy,t_ep,t_off,t_mn,t_sc;
void *__gxx_personality_v0;

main(int argc, char *argv[])
{
char filename[60];

FILE *fp;
int c,n=0;

    {time_t in_second;
    in_second = time(NULL);
    printf("%s",ctime(&in_second));
    }


if(argc <5)
 {
  printf("Format: run_prgat <\"Runfile\"> <hr> <min> <sec>");
  exit(0);}

hr=(long)atoi(argv[2]);
mn=(long)atoi(argv[3]);
sc=(long)atoi(argv[4]);
printf("Run Time: %ld hr  %ld min  %ld secs\n", hr,mn,sc );


strcpy(filename,argv[1]);
printf("executing file name: %s\n",filename );


{
 time_t in_seconds;
 in_seconds = time(NULL);
 t_hr = (long int)((in_seconds/3600));
 t_mn = (long int)((in_seconds%3600)/60);
 t_sc = (long int)(in_seconds%60);

 printf("Check: %ld hours %ld  mins %ld secs\n" , (t_hr-7)%24,t_mn,t_sc);

if((((t_hr-7)%24)*3600+(t_mn*60)+t_sc)>=((hr*3600)+(mn*60)+sc)){
    printf("\nToo late!");exit(0); }


while((long int)(in_seconds%60) !=sc || (long int)((in_seconds%3600)/60) !=mn || (long int)(((in_seconds/3600)+1)%24) != hr )
{
    in_seconds = time(NULL);
    {t_hr = (long int)in_seconds/3600;
    t_mn = (long int)(in_seconds%3600)/60;
    t_sc = (long int)in_seconds%60;

 }}


printf("Actual Run Time: %ld hours %ld  mins %ld secs\n" , (t_hr-7)%24,t_mn,t_sc);

}


/* run exe program*/
(void)system(filename);


return(0);
}
