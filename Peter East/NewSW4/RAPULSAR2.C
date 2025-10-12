
/*rapulsar2.c	   pwe: 02/04/2015
Takes rtlsdr .bin files, applies averaging and folding algorithm in blocks. Outputs text file of folded average.

Command format:- rapulsar2 <infile> <outfile> <clock rate(MHz)) <No. output data points> <Pulsar period(ms)>*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>



long long int count[16384];
long long int coun;
int PTS;
long long int file_end,p_num;
float dats[16384],clck;
long double periodf,dt,res,tim;
long double smt,sumt[16384];
unsigned char ucha;

FILE *fptr;
FILE *fpto;

void out_dat(void);


main(int argc,char *argv[])

{

long long int ss;
int s;



/*check command line arguments*/
if(argc !=6)
    { printf("Format: rapulsar2 <infile> <outfile> <clock rate (MHz)> <output data points> < pulsar period(ms)>\n");
    exit(0);
    }

if((fptr=fopen(argv[1],"rb"))==NULL)
    {printf("Can't open file %s. \n",argv[1]);
    exit(0);
    }

clck=1/atof(argv[3]);
PTS=atoi(argv[4]);
periodf = (long double)atof(argv[5]);
dt=periodf/(long double)PTS;
                                        /*printf("PTS=%d   periodf =%ld\n",PTS,periodf);*/


/*find length of input file*/
fseeko(fptr,0L,SEEK_END);
file_end=(long long)(ftello(fptr));


p_num=(long long int)((file_end)/(PTS*2));


printf("No. Bytes = %lld\n",(long long)file_end);
printf("No. Blocks=%lld\n",(long long)p_num);
printf("PPeriod=%fms\n",(double)periodf);



fclose(fptr);

fptr=fopen(argv[1],"rb");


/*read input file,decode I and Q, determine power. Sum powers in folded period and on completion calculate bin average.
 At end of input file, output text file with averaged data*/

 {coun=0;

 for(ss=0;ss<p_num;ss++)
    {

    for(s=0;s<2*PTS;s+=2)
        {
        ucha=getc(fptr);
        dats[s]=(float)ucha+0.0;
        dats[s]=(dats[s]-127.5)/128.0;

        ucha=getc(fptr);
        dats[s+1]=(float)ucha+0.0;
        dats[s+1]=(dats[s+1]-127.5)/128.0;

        smt=dats[s]*dats[s]+dats[s+1]*dats[s+1];

        tim=(long double)(s/2+(ss*PTS));
        res=(long double)clck/(dt*(double)1000.0);
        res=(long double)tim*(double)res;
        tim=(long long int)res%(long long int)PTS;

        sumt[(int)tim]=sumt[(int)tim]+smt;
        count[(int)tim]=count[(int)tim]+1;
coun=coun+1;
                    /* printf("%ld  %ld   %d    %f   %ld   %4.0f\n",PTS,p_num,s/2,res,tim,count[tim]);*/
        }
    }

 }

printf("No. bins=%d  Count/bin=%lld\n",PTS,count[(int)tim]);

fpto = fopen(argv[2],"w");
out_dat();

fclose(fptr);
fclose(fpto);

printf("\nInfile=%s    Outfile=%s   End bin=%d   IQ count=%lld\n",argv[1],argv[2],(int)tim,coun);
exit(0);
}



void out_dat(void)
{
long int tt;

for(tt=0;tt<PTS;tt++)
    {

fprintf(fpto,"%ld    %3.5f   %lld\n",(tt),((float)sumt[tt]/(float)count[tt]),count[tt]);

    }/*printf("%d\n",tt);*/
}
