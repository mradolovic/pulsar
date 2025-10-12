
/*rapulsar2_avg2.c	   pwe: 02/04/2015 G.G. 01/07/2015 05/02/2016
Takes rtlsdr .bin converted to .txt files, applies averaging and folding algorithm in blocks. Outputs text file of folded average.
Command format:- rapulsar2_avg2 <infile> <outfile> <clock rate(MHz)) <No. output data points> <Pulsar period(ms)>*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


long long int count[16384];
long long int coun,cnt;
char c;
int PTS;
unsigned long long int file_end,p_num;
long double dats[16384],clck;
double periodf,dt,res,tim;
long double sumt[16384];
unsigned char ucha;
float smt;
float value[1];
FILE *fptr;
FILE *fpto;

void out_dat(void);


int main(int argc,char **argv)

{

long long int ss;
int s;


/*check command line arguments*/
if(argc !=6)
    { printf("Format: rapulsar2_avg2 <infile> <outfile> <clock rate (MHz)> <output data points> < pulsar period(ms)>\n");
    exit(0);
    }

if((fptr=fopen(argv[1],"r"))==NULL)
    {printf("Can't open file %s. \n",argv[1]);
    exit(0);
    }

clck=1/atof(argv[3]);
PTS=atoi(argv[4]);
periodf = atof(argv[5]);
dt=periodf/(double)PTS;

// Open the file 
   
 
    // Check if file exists
    if (fptr == NULL)
    {
        printf("Could not open file \n");
        return 0;
    }
 
    // Extract characters from file and store in character c
    //for (c = 0; c != EOF; c++){
       while(fscanf(fptr,"%f",&smt) !=EOF){; //if (c == '\n') // Increment count if this character is newline fscanf(fptr,"%f",&smt);
		cnt = cnt + 1;}
 
    // Close the file
    fclose(fptr); 

	//printf("%f  %lld\n",(float) smt, cnt);
/*find length of input file*/
fseeko(fptr,0L,SEEK_END);


file_end=(long long int)(ftello(fptr));
file_end=(long long int)(cnt);

p_num=(long long int)((file_end)/(PTS));

printf("No. Bytes = %llu\n",(long long int )(cnt));//(file_end));
printf("No. Blocks=%lld\n",p_num);
printf("PPeriod=%fms\n",periodf);
//fclose(fptr);

fptr=fopen(argv[1],"r");
fseeko(fptr, 0L, SEEK_SET);

/*read input file,decode I and Q, determine power. Sum powers in folded period and on completion calculate bin average.
 At end of input file, output text file with averaged data*/

 {coun=0;

 for(ss=0;ss<p_num;ss++)
    {

    for(s=0;s<PTS;s++)
        {value[0]=0;
		fscanf(fptr,"%f",&smt);    
		
		//printf("%f\n",smt);
        tim=(double)(s+(ss*PTS));
        res=(double)clck/(dt*(double)1000.0);
        res=(double)tim*(double)res;
        tim=(long long int)res%(long long int)PTS;
		
        sumt[(int)tim]=sumt[(int)tim]+(long double)smt;
		//printf("%f\n",(float)sumt[(int)tim]);
        count[(int)tim]=count[(int)tim]+1;
		coun=coun+1;
        }
    }

 }

printf("No. bins=%d  Count/bin=%lld\n",PTS,count[(int)tim]);
fpto = fopen(argv[2],"w");
out_dat();
fclose(fptr);
fclose(fpto);
printf("\nInfile=%s    Outfile=%s   End bin=%d   count=%lld\n",argv[1],argv[2],(int)tim,coun);
exit(0);
}



void out_dat(void)
{
long int tt;

for(tt=0;tt<PTS;tt++)
    {//printf("%ld    %f   %Ld\n",(tt),((float)sumt[tt]),count[tt]);
fprintf(fpto,"%ld    %f   %Ld\n",(tt),((float)sumt[tt]/count[tt]),count[tt]);
    }
}