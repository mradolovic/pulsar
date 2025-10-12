
/*rapulsar3_plot.c	   pwe: 02/04/2015
Takes rtlsdr .bin files, applies averaging and folding algorithm in blocks of pulsar periods. Outputs text file of folded averages.

Command format:- rapulsar3_plot <infile> <outfile> <clock rate(MHz)) <No. output data points> <Pulsar period(ms)><No. of periods>
 gcc -lfftw3 -lm -Wall -lpthread  -DVERBOSE_MODE -D_FILE_OFFSET_BITS=64 RAPULSAR3_plot.C -o rapulsar3_plot*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


long long int count[16384];
long long int coun;

int PTS;
long int D;
long long int file_end,p_num;
long double dats[16384],clck;
double periodf,dt,res,tim,dd;
long double smt,sumt[16384];
unsigned char ucha;

FILE *fptr;
FILE *fpto;

void out_dat(void);


int main(int argc,char *argv[])

{

long long int ss;
long long int s;



/*check command line arguments*/
if(argc !=7)
    { printf("Format: rapulsan2 <infile> <outfile> <clock rate (MHz)> <output data points> < pulsar period(ms)>  <No. of periods>\n");
    exit(0);
    }

if((fptr=fopen(argv[1],"rb"))==NULL)
    {printf("Can't open file %s. \n",argv[1]);
    exit(0);
    }

clck=1/atof(argv[3]);
PTS=atoi(argv[4]);
periodf = atof(argv[5]);
dt=periodf/(double)PTS;
D=atoi(argv[6]);
                                        
dd=(double)((double)D*periodf*1000.0/clck);

/*find length of input file*/
fseeko(fptr,0L,SEEK_END);
file_end=(long long)(ftello(fptr));
//file_end=file_end/D;



p_num=(long long)(((file_end)/(2))/(int)dd);

printf("No. I/Q Samples = %lld \n",(file_end)/2);
printf("No. of Lines=%lld\n",p_num);
printf("PPeriod=%fms\n",periodf);
fclose(fptr);

fptr=fopen(argv[1],"rb");
fpto = fopen(argv[2],"w");

/*read input file,decode I and Q, determine power. Sum powers in folded period and on completion calculate bin average.
 At end of input file, output text file with averaged data*/

int rep=0;
for(rep=0;rep<p_num;rep++)
 {
 coun=0;
 tim=0;
 res=0;
 for(s=0;s<16384;s++){
    sumt[s]=0;
    count[s]=0;
    }

 for(s=0;s<(int)dd;s++)
    {
    //for(s=0;s<2*PTS;s+=2)
        {
        ucha=getc(fptr);
        dats[0]=(float)ucha+0.0;
        dats[0]=(dats[0]-127.5)/128.0;

        ucha=getc(fptr);
        dats[1]=(float)ucha+0.0;
        dats[1]=(dats[1]-127.5)/128.0;

        smt=dats[0]*dats[0]+dats[1]*dats[1];

        //tim=(double)((double)s/2.0+(double)(ss*PTS)+(double)((double)rep*10.0*(double)D));
		 tim=(double)((int)(rep*dd+s)%(int)dd);//+(double)((double)rep*20.0*(double)D));
        res=(double)clck/(dt*1000.0);
        res=(double)tim*(double)res;
        tim=(long long int)res%(long long int)PTS;

        sumt[(int)tim]=sumt[(int)tim]+smt;
        count[(int)tim]=count[(int)tim]+1;

       // printf("%ld  %ld   %d    %f   %d   %d\n",PTS,p_num,s/2,(float)res,(int)tim,(int)count[(int)tim]);
		//printf("%d    %d\n",(int)tim,(int)count[(int)tim]);
		//printf("%f    %d\n",(float)res,(int)tim);
        }
    }
 //}

//printf("No bins=%d    Count/bin=%lld\n",PTS,count[(int)tim]);
//fpto = fopen(argv[2],"w");
 out_dat();
}

printf("No bins=%d    Count/bin=%lld\n",PTS,count[(int)tim]);
fclose(fptr);
fclose(fpto);

printf("\nInfile=%s    Outfile=%s   End bin=%d\n",argv[1],argv[2],(int)tim);
exit(0);
}



void out_dat(void)
{
long int tt;

for(tt=0;tt<PTS;tt++)
    {

//fprintf(fpto,"%ld    %3.5f   %lld\n",(tt),((float)sumt[tt]/(float)count[tt]),count[tt]);
fprintf(fpto,"%3.5f\t",((float)sumt[tt]/(float)count[tt]));
    }/*printf("%d\n",tt);*/
fprintf(fpto,"\n");

}
