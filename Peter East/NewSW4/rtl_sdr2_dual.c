/*
 * rtl-sdr, turns your Realtek RTL2832 based DVB dongle into a SDR receiver
 * Copyright (C) 2012 by Steve Markgraf <steve@steve-m.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <errno.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include "getopt/getopt.h"
#endif

#include "rtl-sdr.h"
#include "convenience/convenience.h"

#define DEFAULT_SAMPLE_RATE		2048000
#define DEFAULT_BUF_LENGTH		(16 * 16384)
#define MINIMAL_BUF_LENGTH		512
#define MAXIMAL_BUF_LENGTH		(256 * 16384) //4194304

pthread_t tid0,tid1;
static int do_exit = 0;
static int do_exit_2 = 0;
static uint64_t bytes_to_read = 0;
static uint64_t bytes_to_read_aux = 0;
static uint64_t bytes_to_read_2 = 0;
static rtlsdr_dev_t *dev = NULL;
static rtlsdr_dev_t *dev_2 = NULL;
char *filename = NULL;
char *filename_2 = NULL;
long int n_read;
int r, opt,r_2,len=0,len_2=0;
uint32_t out_block_size = DEFAULT_BUF_LENGTH;
FILE *file;
FILE *file_2;
uint8_t *buffer;
uint8_t *buffer_2;
	

void usage(void)
{
	fprintf(stderr,
		"rtl_sdr, an I/Q recorder for RTL2832 based DVB-T receivers\n\n"
		"Usage:\t -f 0 frequency_to_tune_to [Hz]\n"
		"\t[-u 1 frequency_to_tune_to [Hz]\n"
		"\t[-s samplerate (default: 2048000 Hz)]\n"
		"\t[-d device_index (default: 0)]\n"
		"\t[-h Use first two devices(use: -h 1)]\n"
		"\t[-g gain 0 (default: 0 for auto)]\n"
		"\t[-q gain 1 (default: 0 for auto)]\n"
		"\t[-p ppm_error (default: 0)]\n"
		"\t[-b output_block_size (default: 16 * 16384)]\n"
		"\t[-n number of samples to read (default: 0, infinite)]\n"
		"\t[-S force sync output (default: async)]\n"
		"\t[-D direct_sampling_mode, 0 (default/off), 1 (I), 2 (Q), 3 (no-mod)]\n"
		"\t[-N no dithering (default: use dithering)]\n"
		"\tfilename (a '-' dumps samples to stdout)\n\n");
	exit(1);
}

#ifdef _WIN32
BOOL WINAPI
sighandler(int signum)
{
	if (CTRL_C_EVENT == signum) {
		fprintf(stderr, "Signal caught, exiting!\n");
		do_exit = 1;
		rtlsdr_cancel_async(dev);
		return TRUE;
	}
	return FALSE;
}
#else
static void sighandler(int signum)
{
	fprintf(stderr, "Signal caught, exiting!\n");
	do_exit = 1;
	rtlsdr_cancel_async(dev);
}
#endif

static void rtlsdr_callback(unsigned char *buf, uint32_t len, void *ctx)
{
	if (ctx) {
		if (do_exit)
			return;

		if ((bytes_to_read > 0) && (bytes_to_read < len)) {
			len = (uint32_t)bytes_to_read;
			do_exit = 1;
			rtlsdr_cancel_async(dev);
		}

		if (fwrite(buf, 1, len, (FILE*)ctx) != len) {
			fprintf(stderr, "Short write, samples lost, exiting!\n");
			rtlsdr_cancel_async(dev);
		}

		if (bytes_to_read > 0)
			bytes_to_read -= (uint64_t)len;
	}
}
static void rtlsdr_callback_2(unsigned char *buf_2, uint32_t len_2, void *ctx_2)
{
	if (ctx_2) {
		if (do_exit_2)
			return;

		if ((bytes_to_read_2 > 0) && (bytes_to_read_2 < len_2)) {
			len_2 = (uint32_t)bytes_to_read_2;
			do_exit_2 = 1;
			rtlsdr_cancel_async(dev_2);
		}

		if (fwrite(buf_2, 1, len_2, (FILE*)ctx_2) != len_2) {
			fprintf(stderr, "Short write, samples lost, exiting!\n");
			rtlsdr_cancel_async(dev_2);
		}

		if (bytes_to_read_2 > 0)
			bytes_to_read_2 -= (uint64_t)len_2;
	}
}

void* read_0(void *arg)
{
	//fprintf(stderr, "0 Reading samples in async mode...\n");
	bytes_to_read=bytes_to_read_aux;
	do_exit = 0;
	r = rtlsdr_read_async(dev, rtlsdr_callback, (void *)file, 0, out_block_size);
	fprintf(stderr, "END 0 Reading...\n");
	return 0;
}
void* read_1(void *arg)
{
	//fprintf(stderr, "1 Reading samples in async mode...\n");
	bytes_to_read_2=bytes_to_read_aux;
	do_exit_2 = 0;
	r_2 = rtlsdr_read_async(dev_2, rtlsdr_callback_2, (void *)file_2, 0, out_block_size);
	fprintf(stderr, "END 1 Reading...\n");
	return 0;
}	

int main(int argc, char **argv)
{
#ifndef _WIN32
	struct sigaction sigact;
#endif

	int gain = 0,err0,err1;
	int gain_2 = 0;
	int ppm_error = 0;
	int sync_mode = 0;
	int direct_sampling = 0;
	int dithering = 1;

	int dev_index = 0;
	//int dev_given = 0;
	int dev_index_2 = 0;
	int dev_given_2 = 0;	
	uint32_t frequency = 100000000;
	uint32_t frequency_2 = 100000000;
	uint32_t samp_rate = DEFAULT_SAMPLE_RATE;
	
	int *read0_thread,*read1_thread;
	
	while ((opt = getopt(argc, argv, "d:h:f:u:g:q:s:p:b:n:S:D:N::")) != -1) {
		switch (opt) {
		case 'd':
			dev_index = verbose_device_search(optarg);
			//dev_given = 1;
			break;
		case 'h':
			dev_index = verbose_device_search("0");
			//dev_given = 2;		
			dev_index_2 = verbose_device_search("1");
			dev_given_2 = 2;
			break;			
		case 'f':
			frequency = (uint32_t)atofs(optarg);
			break;
		case 'u':
			frequency_2 = (uint32_t)atofs(optarg);
			break;			
		case 'g':
			gain = (int)(atof(optarg) * 10); /* tenths of a dB */
			break;
		case 'q':
			gain_2 = (int)(atof(optarg) * 10); /* tenths of a dB */
			break;			
		case 's':
			samp_rate = (uint32_t)atofs(optarg);
			break;
		case 'p':
			ppm_error = atoi(optarg);
			break;
		case 'b':
			out_block_size = (uint32_t)atof(optarg);
			break;
		case 'n':
			bytes_to_read_aux = (uint64_t)atofs(optarg) * 2;
			break;
		case 'S':
			sync_mode = 1;
			break;
		case 'D':
			direct_sampling = atoi(optarg);
			break;
		case 'N':
			dithering = 0;
			break;
		default:
			usage();
			break;
		}
	}

	if (argc <= optind) {
		usage();
	} else {
		filename = argv[optind];
		
	}

	if(out_block_size < MINIMAL_BUF_LENGTH ||
	   out_block_size > MAXIMAL_BUF_LENGTH ){
		fprintf(stderr,
			"Output block size wrong value, falling back to default\n");
		fprintf(stderr,
			"Minimal length: %u\n", MINIMAL_BUF_LENGTH);
		fprintf(stderr,
			"Maximal length: %u\n", MAXIMAL_BUF_LENGTH);
		out_block_size = DEFAULT_BUF_LENGTH;
	}

	buffer = malloc(out_block_size * sizeof(uint8_t));
	

	//if (!dev_given) {
	//	dev_index = verbose_device_search("0");
	//}

	if (dev_index < 0) {
		exit(1);
	}

	r = rtlsdr_open(&dev, (uint32_t)dev_index);
	if (r < 0) {
		fprintf(stderr, "Failed to open rtlsdr device #%d.\n", dev_index);
		exit(1);
	}
	
#ifndef _WIN32
	sigact.sa_handler = sighandler;
	sigemptyset(&sigact.sa_mask);
	sigact.sa_flags = 0;
	sigaction(SIGINT, &sigact, NULL);
	sigaction(SIGTERM, &sigact, NULL);
	sigaction(SIGQUIT, &sigact, NULL);
	sigaction(SIGPIPE, &sigact, NULL);
#else
	SetConsoleCtrlHandler( (PHANDLER_ROUTINE) sighandler, TRUE );
#endif

	if (!dithering) {
		fprintf(stderr, "Disabling dithering...  ");
		r = rtlsdr_set_dithering(dev, dithering);
		if (r) {
			fprintf(stderr, "0 failure\n");
		} else {
			fprintf(stderr, "0 success\n");
		}
	}

	if (direct_sampling) {
		verbose_direct_sampling(dev, direct_sampling);
	}

	/* Set the sample rate */
	verbose_set_sample_rate(dev, samp_rate);
	

	/* Set the frequency */
	verbose_set_frequency(dev, frequency);
	
	if (0 == gain) {
		 /* Enable automatic gain */
		verbose_auto_gain(dev);
	} else {
		/* Enable manual gain */
		gain = nearest_gain(dev, gain);
		verbose_gain_set(dev, gain);
	}

	verbose_ppm_set(dev, ppm_error);
	
	
	if(strcmp(filename, "-") == 0) { /* Write samples to stdout */
		file = stdout;
		file_2 = stdout;
#ifdef _WIN32
		_setmode(_fileno(stdin), _O_BINARY);
#endif
	} else {
		len = (strlen(filename));
		filename_2 = malloc(sizeof(char) * (len+3));
		memcpy(filename_2, filename,len-4);
		filename_2[len-4] = 0;
		strcat(filename_2,"_1.bin");	
		file = fopen(filename, "wb");
		
	
		
		if (!file) {
			fprintf(stderr, "Failed to open %s\n", filename);
			goto out;
		}
		
	}

	/* Reset endpoint before we start reading from it (mandatory) */
	verbose_reset_buffer(dev);
	

	if(dev_given_2 == 2)
	{
		buffer_2 = malloc(out_block_size * sizeof(uint8_t));
		file_2 = fopen(filename_2, "wb");
		if (!file_2) {
			fprintf(stderr, "Failed to open %s\n", filename_2);
			goto out;
		}			
		r_2 = rtlsdr_open(&dev_2, (uint32_t)dev_index_2);
		if (r_2 < 0) {
			fprintf(stderr, "Failed to open rtlsdr device #%d.\n", dev_index_2);
			exit(1);
		}			
		if (!dithering) {
			fprintf(stderr, "1 Disabling dithering...  ");
			r_2 = rtlsdr_set_dithering(dev_2, dithering);
			if (r_2) {
				fprintf(stderr, "1 failure\n");
			} else {
				fprintf(stderr, "1 success\n");
			}		
		}		
		if (direct_sampling) {
			verbose_direct_sampling(dev_2, direct_sampling);
		}
		verbose_set_sample_rate(dev_2, samp_rate);
		verbose_set_frequency(dev_2, frequency_2);
		if (0 == gain_2) {
			 /* Enable automatic gain */
			verbose_auto_gain(dev_2);
		} else {
			/* Enable manual gain */
			gain_2 = nearest_gain(dev_2, gain_2);
			verbose_gain_set(dev_2, gain_2);
		}		
		verbose_ppm_set(dev_2, ppm_error);
		
		verbose_reset_buffer(dev_2);
		}

	
	if (sync_mode) {
		fprintf(stderr, "Reading samples in sync mode...\n");
		while (!do_exit) {
			r = rtlsdr_read_sync(dev, buffer, out_block_size, &n_read);
			if (r < 0) {
				fprintf(stderr, "WARNING: sync read failed.\n");
				break;
			}

			if ((bytes_to_read > 0) && (bytes_to_read < (uint64_t)n_read)) {
				n_read = bytes_to_read;
				do_exit = 1;
			}

			if (fwrite(buffer, 1, n_read, file) != (size_t)n_read) {
				fprintf(stderr, "Short write, samples lost, exiting!\n");
				break;
			}

			if ((uint64_t)n_read < out_block_size) {
				fprintf(stderr, "Short read, samples lost, exiting!\n");
				break;
			}

			if (bytes_to_read > 0)
				bytes_to_read -= n_read;
		}
	} else {

		err0 = pthread_create(&(tid0), NULL, &read_0, NULL);//start thread process
		if (err0 != 0)
			{printf("\ncan't create 0 thread :[%s]",strerror(err0));}
		else
			{//printf("\nThread 0 created successfully\n");
				}	
		if(dev_given_2 == 2)
		{
			err1 = pthread_create(&(tid1), NULL, &read_1, NULL);//start thread process
			if (err1 != 0)
				{printf("\ncan't create 1 thread :[%s]",strerror(err1));}
			else
				{//printf("\nThread 1 created successfully\n");
					}				

		}
	}
	
	pthread_join(tid0, (void**)&(read0_thread)); //if dont ended previous , wait...
	if(dev_given_2 == 2)
		pthread_join(tid1, (void**)&(read1_thread)); //if dont ended previous , wait...
	
	if (do_exit)
		fprintf(stderr, "\nUser cancel, exiting...\n");
	else
		fprintf(stderr, "\nLibrary error %d, exiting...\n", r);

	if (file != stdout)
		fclose(file);

	rtlsdr_close(dev);
	free (buffer);
	
	if(dev_given_2 == 2)
	{
		if (file_2 != stdout)
			fclose(file_2);
		rtlsdr_close(dev_2);
		free (buffer_2);
	}
	
out:
	return r >= 0 ? r : -r;
}
