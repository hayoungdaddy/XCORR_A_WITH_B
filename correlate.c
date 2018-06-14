#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <qlib2.h>
#include "timefuncs.h"
#include "sac.h"



/* Compute a correlation waveform.
 * @param[in] ref The reference waveform.
 * @param[in] target The target waveform.
 * @param[in] ends Set to 0 or 1 to ignore or include the correlation points
 * at the beginning and ending that are computed using only part of the
 * ref waveform.
 * @param[in] norm_type LOCAL_MEAN: the mean is recomputed for the data points
 * that contribute to each correlation. GLOBAL_MEAN: global means are
 * used.  TOTAL_AMP: the correlation values are normalized by the total
 * amplitudes of the waveforms.
 * @return Returns the correlation waveform. Returns NULL if the length of the
 * ref is 0 or the length of the target waveform is 0.
 */

/**
 * Compute correlation coefficient. Returns correlation coefficient of arrays
 * x & y. fx and fy are scaling factors to flip traces if desired. The
 * correlation coefficient returned is:
 * <pre>
 *     c = sum_xy / sqrt( (sum_xx * sum_yy) )
 * </pre>
 *
 * @param x Input array of n values.
 * @param fx x scaling factor. x will be multiplied by fx.
 * @param y Input array of n values.
 * @param fy y scaling factor. y will be multiplied by fy.
 * @param n Input number of values in x and y.
 * @return The correlation coefficient. Returns zero if x and y are all zero.
 */
void iir_filter( float *data, int nsamps, char *filter_type, float trbndw,
		  float a, int iord, char *operation_type, float flo, float fhi, float ts, int passes );
double
correlate(float *x, double fx, float *y, double fy, int n)
{
	register int i;
	double sum_xx=0., sum_yy=0., sum_xy=0., r, s, x0, y0;

	for (i = 0; i < n; i++) {
		x0 = x[i] * fx;
		y0 = y[i] * fy;
		sum_xy += (x0 * y0);
		sum_xx += (x0 * x0);
		sum_yy += (y0 * y0);
	}
	s = (sum_xx * sum_yy);
	r = (s != 0.) ? sum_xy / sqrt(s) : 0.;
	return(r);
}

int main(int argc, char **argv)
{
	char *aSacFile, *bSacFile;
	FILE *fpA, *fpB;
	SAC  aSachead, bSachead;
	SAC_HEADER aHeader, bHeader;
	float *aData, *bData;
	int sps;
	int numA, numB;
	int nASamp, nBSamp;
	int year,doy,month,day, hour, min;
	float second;
	double epochTime;
	int nLoop, nSamples;
	int count;
	int filter; 
	float coef;
	float startTime, timeWindow;
	int npoles, npass;
	float tranbw;
	float lowpass, highpass;

	switch (argc)
	{
	case 7:
		aSacFile = argv[1];
		if((fpA = fopen(aSacFile, "r")) == NULL)
		{
			fprintf(stderr, "unable to open SAC data file %s \n", aSacFile);
			exit(EXIT_FAILURE);
		}
		bSacFile = argv[2];
		if((fpB = fopen(bSacFile, "r")) == NULL)
		{
			fprintf(stderr, "unable to open SAC data file %s \n", bSacFile);
			exit(EXIT_FAILURE);
		}
		timeWindow = atof(argv[3]); 	// seconds
		filter     = atoi(argv[4]);
		lowpass    = atof(argv[5]);
		highpass   = atof(argv[6]);
		break;
	default:
		printf("%s A_sacfile B_sacfile timeWindow userfilter lowpass highpass\n",argv[0]);
		exit(1);
		break;
	}

	tranbw = 0.3;
	npoles = 3;
	npass  = 1;

	(void) sacReadHeader(fpA, &aSachead, &aHeader);
	(void) sacReadHeader(fpB, &bSachead, &bHeader);
	sps = (int) nearbyintf(1.0/aSachead.delta);
	nASamp = aSachead.npts;
	nBSamp = bSachead.npts;
	nSamples = sps * timeWindow;

	year = aSachead.nzyear;
	doy  = aSachead.nzjday;
	hour = aSachead.nzhour;
	min  = aSachead.nzmin;

	ValidateDOY(year, &doy);
	mnday(doy, isleap(year), &month, &day );
	second          = (float) aSachead.nzsec+0.001*((float) aSachead.nzmsec);
	epochTime       = tmMakeEpochTime(year, month, day, hour, min, second);
	epochTime       = epochTime+aSachead.b;

	aData = (float *) malloc(sizeof(float)*nSamples);
	bData = (float *) malloc(sizeof(float)*nSamples);

	nLoop = nASamp / nSamples;

	printf("%f %f %d %d %f %d\n", aSachead.delta, bSachead.delta, nASamp, nBSamp, epochTime, nLoop);

	count = 0;
	
	while(1)
	{
		numA = fread(aData, sizeof(float), (size_t)(nSamples), fpA);
		numB = fread(bData, sizeof(float), (size_t)(nSamples), fpB);

		if(feof(fpA) != 0 || feof(fpB) != 0)
			break;

		if (filter !=0)
		{
			iir_filter (aData, numA, "BU", tranbw, 30., npoles, "BP", lowpass, highpass, aSachead.delta, npass);
			iir_filter (bData, numB, "BU", tranbw, 30., npoles, "BP", lowpass, highpass, bSachead.delta, npass);
		}

		coef = (float) correlate(aData, 1.0, bData, 1.0, nSamples);
		coef = fabsf(coef);

		printf("%.2f %f\n",epochTime + (count*aSachead.delta), coef);

		fseek(fpA, -(sizeof(float) * nSamples - sizeof(float)), SEEK_CUR);
		fseek(fpB, -(sizeof(float) * nSamples - sizeof(float)), SEEK_CUR);
	
		count++;
	}
	
	fclose(fpA);
	fclose(fpB);
	free(aData);
	free(bData);
	return 0;
}
