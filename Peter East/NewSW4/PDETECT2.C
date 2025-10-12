
/*pdetect2.c	   pwe: 02/04/2015
Takes rtlsdr .bin files, averages power in blocks of size (clock rate)/(video band). Outputs text file of block averages at 1/(video band) intervals.

Command format:- pdetect2 <infile> <outfile> <clock rate(MHz)> <video band(kHz) <file divisor>>*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>



long int count=0L;

float PTS, sum,pts,di,dq;
int D;
long long int file_end,p_num,ppts;
float clck;
float smt;
unsigned char ucha;

FILE *fptr;
FILE *fpto;

void out_dat(void);


main(int argc,char *argv[])

{

long long int ss;
long int s;



/*check command line arguments*/
if(argc !=6)
    { printf("Format: pdetect2 <infile> <outfile> <clock rate (MHz) <video band (kHz)> <file divisor>\n");
    exit(0);
    }

if((fptr=fopen(argv[1],"rb"))==NULL)
    {printf("Can't open file %s. ",argv[1]);
    exit(0);
    }

clck=atof(argv[3]);
PTS=atof(argv[4]);
pts=(clck*1000.0/PTS);

ppts=(long int)pts;

clck=1/clck;
D=atoi(argv[5]);

  printf("RF/Video=%lld \n",ppts);
  printf("Data Clock=%1.2fus\n",clck);
  printf("Video BW=%fkHz\n",PTS);



/*find length of input file*/
fseeko(fptr,0L,SEEK_END);
file_end=(long long int)(ftello(fptr));
file_end=file_end/(long long int)D;



p_num=(long long int)(file_end/ppts/2);

printf("No. Bytes = %lld \n",(file_end));
printf("No. Blocks=%lld \n",p_num);
printf("VideoSamples=%lld\n",ppts);
printf("File Divisor=%d\n",D);
fclose(fptr);

fptr=fopen(argv[1],"rb");
fpto = fopen(argv[2],"w");

/*read input file,decode I and Q, determine power. Sum powers in clock rate/video band blocks.
 At end of input file, output text file with averaged data*/

 {

 for(ss=0;ss<p_num;ss++)
    { sum=0;

    for(s=0;s<(ppts);s+=1)
	{
        ucha=getc(fptr);
	di=(float)ucha+0.0;
	di=(di-127.5)/128.0;

        ucha=getc(fptr);
	dq=(float)ucha+0.0;
	dq=(dq-127.5)/128.0;

	smt=di*di+dq*dq;

	sum=sum+smt;


		     /*printf("%f  %ld\n",PTS,p_num);*/
    }//fprintf(fpto,"%ld   %1.5f\n",count,((float)sum/(float)pts));
	fprintf(fpto,"%1.8f\n",((float)sum/(float)pts));
    count+=1;
    /*printf("%d  %f    %ld\n",(int)pts,sum,ss);*/
    }

 }

/*printf("%lld  %f   %lld\n",file_end, pts,p_num);*/


fclose(fptr);
fclose(fpto);

printf("\nInfile=%s    Outfile=%s\n",argv[1],argv[2]);
exit(0);
}
