/*
 de-dispers22co_fast_fixed.c
 Fixed version: dynamic ddarray, per-thread buffers, divide-by-zero guards
 Uses OpenMP + FFTW
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <fftw3.h>
#include <stdint.h>
#include <string.h>

#define MAX_FTPTS 4096
#define DDARRAY_ROWS_MOD 65536   /* original code used modulo 65536 */
#define DEFAULT_DDARRY 32768

long long int count_out = 0;

int ftpts;
int invert = 1;
int offset = 0;
int ddarry = DEFAULT_DDARRY;

float clck, RF, DCF, DM, fo, dn;
float *datft = NULL;        /* sized 2*ftpts */

double *ddarray = NULL;     /* dynamic: rows * (2*ftpts) */

FILE *fptr = NULL;
FILE *fpto = NULL;

/* prototypes */
void compute_fft(double *data, int nn, int isign);
void ftorg_dat(double *dats, double *datr, int ftpts);
void ftreorg_dat(double *dats, double *datr, int ftpts);

int main(int argc, char *argv[]) {
    if (argc != 7) {
        fprintf(stderr, "Usage: %s <infile> <outfile> <clock MHz> <FFT pts> <RF MHz> <DM>\n", argv[0]);
        return 1;
    }

    /* open files */
    if ((fptr = fopen(argv[1], "rb")) == NULL) { perror("input file"); return 1; }
    if ((fpto = fopen(argv[2], "wb")) == NULL) { perror("output file"); fclose(fptr); return 1; }

    DCF   = atof(argv[3]);
    ftpts = atoi(argv[4]);
    RF    = atof(argv[5]);
    DM    = atof(argv[6]);

    if (DCF <= 0.0f || ftpts <= 0 || RF <= 0.0f) {
        fprintf(stderr, "Invalid numeric arguments (must be >0): DCF=%f ftpts=%d RF=%f\n", DCF, ftpts, RF);
        fclose(fptr); fclose(fpto); return 1;
    }
    if (ftpts > MAX_FTPTS) {
        fprintf(stderr, "ftpts too large (max %d)\n", MAX_FTPTS); fclose(fptr); fclose(fpto); return 1;
    }

    if (DM < 0) { invert = -1; DM = -DM; offset = 2 * ftpts - 1; }

    /* compute timing/freq */
    clck = 1.0f / DCF;                     /* us */
    dn   = 1.0f / ((float)ftpts * clck);   /* MHz-ish unit consistent with code */
    fo   = RF - dn * (ftpts - 1) / 2.0f;
    if (fo <= 0.0f) {
        fprintf(stderr, "Warning: fo (%.6f) <= 0, clamped to 1e-6\n", fo);
        fo = 1e-6f;
    }

    /* allocate datft safely (2 * ftpts entries as floats) */
    datft = (float *) malloc(sizeof(float) * 2 * ftpts);
    if (!datft) { fprintf(stderr,"datft malloc failed\n"); fclose(fptr); fclose(fpto); return 1; }

    for (int fn = 0; fn < ftpts; ++fn) {
        float freq = fo + (float)fn * dn;
        if (fabsf(freq) < 1e-6f) freq = 1e-6f;
        datft[2 * fn] = (1000.0f / (float)ftpts / clck) * DM * (10000000.0f / 2.410331f) *
                        ((1.0f / (fo * fo)) - (1.0f / (freq * freq)));
        datft[2 * fn + 1] = datft[2 * fn];
    }

    /* compute file size and blocks (same logic as earlier) */
    fseeko(fptr, 0L, SEEK_END);
    long long int file_end = ftello(fptr);
    rewind(fptr);

    if (file_end <= 0) { fprintf(stderr,"Input file empty or error\n"); free(datft); fclose(fptr); fclose(fpto); return 1; }

    long long int bytes_per_block = (long long)ddarry * (long long)ftpts * 2LL;
    long long int blocks = file_end / bytes_per_block;
    if (blocks <= 0) {
        /* if file smaller than a block, still process once with padded read below */
        blocks = 1;
    }
    fprintf(stderr,"file bytes=%lld  blocks=%lld  ddarry=%d  ftpts=%d\n", file_end, blocks, ddarry, ftpts);

    /* allocate ddarray: rows = DDARRAY_ROWS_MOD (65536) to match original modulo addressing,
       columns = 2*ftpts. Use double values to match original. */
    int rows = DDARRAY_ROWS_MOD;
    int cols = 2 * ftpts;
    size_t dd_size = (size_t)rows * (size_t)cols * sizeof(double);
    ddarray = (double *) malloc(dd_size);
    if (!ddarray) { fprintf(stderr,"ddarray malloc failed, need %zu bytes\n", dd_size); free(datft); fclose(fptr); fclose(fpto); return 1; }
    memset(ddarray, 0, dd_size);

    /* prepare FFTW multithreading */
    fftw_init_threads();
    fftw_plan_with_nthreads(omp_get_max_threads());

    /* allocate buffer for reading one block */
    size_t read_size = (size_t)ddarry * (size_t)ftpts * 2u;
    unsigned char *buffer = (unsigned char *) malloc(read_size);
    if (!buffer) { fprintf(stderr,"buffer malloc failed (%zu bytes)\n", read_size); free(datft); free(ddarray); fclose(fptr); fclose(fpto); return 1; }

    /* prepare per-thread temporary buffers to avoid large stack allocations */
    int nthreads = omp_get_max_threads();
    double **thread_dats = (double **) malloc(nthreads * sizeof(double *));
    double **thread_datr = (double **) malloc(nthreads * sizeof(double *));
    if (!thread_dats || !thread_datr) { fprintf(stderr,"thread buffers malloc failed\n"); free(buffer); free(datft); free(ddarray); fclose(fptr); fclose(fpto); return 1; }
    for (int t = 0; t < nthreads; ++t) {
        thread_dats[t] = (double *) malloc(sizeof(double) * (size_t)cols);
        thread_datr[t] = (double *) malloc(sizeof(double) * (size_t)cols);
        if (!thread_dats[t] || !thread_datr[t]) { fprintf(stderr,"thread buffer alloc failed for thread %d\n", t);
            for (int j = 0; j <= t; ++j) { free(thread_dats[j]); free(thread_datr[j]); }
            free(thread_dats); free(thread_datr); free(buffer); free(datft); free(ddarray); fclose(fptr); fclose(fpto);
            return 1;
        }
    }

    /* main processing loop */
    for (long long an = 0; an < blocks; ++an) {
        size_t got = fread(buffer, 1, read_size, fptr);
        if (got < read_size) {
            if (got == 0) break;
            /* pad remainder with 128 (so after -128 it's 0) - original program expects signed centered at 128 */
            memset(buffer + got, 128, read_size - got);
        }

        /* parallelize across bn (ddarry FFTs inside block) */
        #pragma omp parallel
        {
            int tid = omp_get_thread_num();
            double *dats = thread_dats[tid];
            double *datr = thread_datr[tid];

            #pragma omp for schedule(static)
            for (int bn = 0; bn < ddarry; ++bn) {
                int flag = (an % 2 == 0) ? 0 : 1;

                /* extract ftpts IQ pairs from buffer for this bn */
                /* buffer layout: consecutive IQ pairs: for bn=0: [I0 Q0][I1 Q1]...[I_{ftpts-1} Q_{ftpts-1}] then bn=1, ... */
                size_t base = (size_t)bn * (size_t)ftpts * 2u;
                for (int fn = 0; fn < ftpts; ++fn) {
                    size_t idx = base + (size_t)fn * 2u;
                    unsigned char ucha = buffer[idx];
                    unsigned char uchb = buffer[idx + 1];
                    dats[2 * fn]   = (double)((int)ucha - 128);
                    dats[2 * fn+1] = (double)((int)uchb - 128);
                }

                /* forward FFT: dats -> dats (in-place via compute_fft) */
                compute_fft(dats, ftpts, -1);
                ftorg_dat(dats, datr, ftpts);

                /* place into ddarray using delay */
                for (int cn = 0; cn < cols; ++cn) {
                    int delay_index = offset + invert * cn;
                    int delay = 0;
                    if (delay_index >= 0 && delay_index < 2 * ftpts) delay = (int) datft[delay_index];
                    int row = (bn + flag * 32768 + delay) % DDARRAY_ROWS_MOD;
                    ddarray[(size_t)row * cols + cn] = datr[cn];
                }
            } /* end for bn */
        } /* end parallel region */

        /* produce output for this block (mirrors out_dat) */
        /* We'll do this single-threaded to preserve output ordering; can be optimized later */
        for (int bnn = 0; bnn < ddarry; ++bnn) {
            /* build datr_local from ddarray row */
            double *datarow = &ddarray[(size_t)bnn * cols];
            double *dats_local = thread_dats[0];  /* reuse thread 0 buffers temporarily */
            double *datr_local = thread_datr[0];

            for (int tt = 0; tt < cols; ++tt) datr_local[tt] = datarow[tt];
            ftreorg_dat(dats_local, datr_local, ftpts);
            compute_fft(dats_local, ftpts, 1); /* inverse */

            /* write output bytes: choose a safe clipping */
            for (int cn = 0; cn < cols; ++cn) {
                int val = (int) (dats_local[cn] + 128.0);
                if (val < 0) val = 0;
                if (val > 255) val = 255;
                fputc((unsigned char) val, fpto);
                ++count_out;
            }
        }

    } /* end blocks loop */

    /* cleanup */
    for (int t = 0; t < nthreads; ++t) { free(thread_dats[t]); free(thread_datr[t]); }
    free(thread_dats); free(thread_datr);
    free(buffer);
    free(datft);
    free(ddarray);
    fclose(fptr);
    fclose(fpto);

    fprintf(stderr,"Done. Output bytes written: %lld\n", count_out);
    return 0;
}

/* ----- FFT wrapper using FFTW ----- */
void compute_fft(double *data, int nn, int isign)
{
    /* nn is number of complex points; data layout is interleaved real/im */
    fftw_complex *in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * (size_t)nn);
    fftw_complex *out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * (size_t)nn);
    if (!in || !out) {
        fprintf(stderr,"fftw_malloc failed\n");
        if (in) fftw_free(in);
        if (out) fftw_free(out);
        exit(1);
    }

    for (int i = 0; i < nn; ++i) {
        in[i][0] = data[2*i];
        in[i][1] = data[2*i + 1];
    }

    fftw_plan p = fftw_plan_dft_1d(nn, in, out, (isign == 1) ? FFTW_BACKWARD : FFTW_FORWARD, FFTW_ESTIMATE);
    if (!p) { fprintf(stderr,"fftw_plan failed\n"); fftw_free(in); fftw_free(out); exit(1); }

    fftw_execute(p);

    for (int i = 0; i < nn; ++i) {
        data[2*i]   = out[i][0] / (double) nn;
        data[2*i+1] = out[i][1] / (double) nn;
    }

    fftw_destroy_plan(p);
    fftw_free(in);
    fftw_free(out);
}

/* reorder spectrum (original logic) */
void ftorg_dat(double *dats, double *datr, int ftpts_local)
{
    for (int tt = 0; tt < ftpts_local; ++tt) {
        if (tt < ftpts_local/2) {
            datr[2*tt + ftpts_local]     = dats[2*tt];
            datr[2*tt + 1 + ftpts_local] = dats[2*tt + 1];
        } else {
            datr[2*tt - ftpts_local]     = dats[2*tt];
            datr[2*tt + 1 - ftpts_local] = dats[2*tt + 1];
        }
    }
}

/* inverse reorder */
void ftreorg_dat(double *dats, double *datr, int ftpts_local)
{
    for (int tt = 0; tt < ftpts_local; ++tt) {
        if (tt < ftpts_local/2) {
            dats[2*tt + ftpts_local]     = datr[2*tt];
            dats[2*tt + 1 + ftpts_local] = datr[2*tt + 1];
        } else {
            dats[2*tt - ftpts_local]     = datr[2*tt];
            dats[2*tt + 1 - ftpts_local] = datr[2*tt + 1];
        }
    }
}
