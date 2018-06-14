/****************************************************/
/* ms2css : convert miniSEED file to wfdisc file.   */
/*                                                  */
/* Geun Young Kim                                   */
/* Korea Institute of Geoscieince Mineral Resources */
/* KIGAM                                            */
/*                                                  */
/* gandalf@kigam.re.kr                              */
/*                                                  */
/****************************************************/

/*
 * Copyright (c) 2006 KIGAM
 * All Rights Reserved.
 */


/** utility for data processing
 **/

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <netinet/in.h>
#include <time.h>
#include "sac.h"


/* a has no '\0' at the end. All n bytes are used as characters.
 * copy a to b, left-justifying and adding '\0'.
 */
static void
sacToString(char *a, char *b, int n)
{
        char c[20];

        if(a[0] == '\0')
        {
            strncpy(b, "-", n);
            return;
        }

        strncpy(c, a, n);
        c[n] = '\0';
        sscanf(c, "%s", b);
        if(!strcmp(b, KVAL_UNDEF))
        {
            strncpy(b, "-", n);
        }
}


int
sacReadHeader(FILE *fp, SAC *sac, SAC_HEADER *header)
{
        int pos, err;
        SAC_HEADER s, sac_header_null = SAC_HEADER_NULL;
/*
        int flip_bytes;
        union
        {
            char    a[4];
            float   f;
            long    i;
            short   s;
        } e1;

        e1.a[0] = 0; e1.a[1] = 0;
        e1.a[2] = 0; e1.a[3] = 1;
        flip_bytes = (e1.i == 1) ? 0 : 1;

*/
        memcpy(&s, &sac_header_null, sizeof(SAC_HEADER));
        /* try ascii first
         */
/*
        pos = ftell(fp);
        if(!(err = read_ascii_sac_header(fp, sac, lineno, err_msg))) {
            *binary = 0;
            return(0);
        }
        else if(err == EOF) {
            return(-1);
        }
        *binary = 1;
        memcpy(&s, &sac_header_null, sizeof(SAC_HEADER));
*/
//        fseek(fp, pos, 0);
        if(fread(&s, sizeof(SAC_HEADER), 1, fp) != 1)
        {
            return( feof(fp) ? -1 : -2 );
        }

        memcpy(sac, &s.a, sizeof(SAC_HEADER_A));
        memcpy(header, &sac_header_null, sizeof(SAC_HEADER));
        memcpy(header, &s, sizeof(SAC_HEADER));

//        if(flip_bytes) sacFlipHeader(sac);

        /* left justify all character strings
         */
        sacToString(s.b.kstnm, sac->kstnm, 8);
        sacToString(s.b.kevnm, sac->kevnm, 16);
        sacToString(s.b.khole, sac->khole, 8);
        sacToString(s.b.ko, sac->ko, 8);
        sacToString(s.b.ka, sac->ka, 8);
        sacToString(s.b.kt0, &sac->kt0[0], 8);
        sacToString(s.b.kt1, &sac->kt1[0], 8);
        sacToString(s.b.kt2, &sac->kt2[0], 8);
        sacToString(s.b.kt3, &sac->kt3[0], 8);
        sacToString(s.b.kt4, &sac->kt4[0], 8);
        sacToString(s.b.kt5, &sac->kt5[0], 8);
        sacToString(s.b.kt6, &sac->kt6[0], 8);
        sacToString(s.b.kt7, &sac->kt7[0], 8);
        sacToString(s.b.kt8, &sac->kt8[0], 8);
        sacToString(s.b.kt9, &sac->kt9[0], 8);
        sacToString(s.b.kf, sac->kf, 8);
        sacToString(s.b.kuser0, sac->kuser0, 8);
        sacToString(s.b.kuser1, sac->kuser1, 8);
        sacToString(s.b.kuser2, sac->kuser2, 8);
        sacToString(s.b.kcmpnm, sac->kcmpnm, 8);
        sacToString(s.b.knetwk, sac->knetwk, 8);
        sacToString(s.b.kdatrd, sac->kdatrd, 8);
        sacToString(s.b.kinst, sac->kinst, 8);

        return(0);
}

int writeSAC (SAC* sacheader, float *fdata, int npts, FILE *fp)
{
        int n, done;
        
        n = fwrite (sacheader, sizeof(SAC_HEADER), 1, fp);
        if (n <= 0) 
        {
            fprintf (stderr, "Error writing sac header\n");
            return (1);
        }
        done = 0;
        while (done < npts) 
        {
            n = fwrite (fdata+done, sizeof(float), npts-done, fp);
            if (n <= 0) 
            {
                fprintf (stderr, "Error writing sac data\n");
                return (1);
            }
            else done += n;
        }
        return (0);
}

