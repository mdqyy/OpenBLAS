/*********************************************************************/
/* Copyright 2009, 2010 The University of Texas at Austin.           */
/* All rights reserved.                                              */
/*                                                                   */
/* Redistribution and use in source and binary forms, with or        */
/* without modification, are permitted provided that the following   */
/* conditions are met:                                               */
/*                                                                   */
/*   1. Redistributions of source code must retain the above         */
/*      copyright notice, this list of conditions and the following  */
/*      disclaimer.                                                  */
/*                                                                   */
/*   2. Redistributions in binary form must reproduce the above      */
/*      copyright notice, this list of conditions and the following  */
/*      disclaimer in the documentation and/or other materials       */
/*      provided with the distribution.                              */
/*                                                                   */
/*    THIS  SOFTWARE IS PROVIDED  BY THE  UNIVERSITY OF  TEXAS AT    */
/*    AUSTIN  ``AS IS''  AND ANY  EXPRESS OR  IMPLIED WARRANTIES,    */
/*    INCLUDING, BUT  NOT LIMITED  TO, THE IMPLIED  WARRANTIES OF    */
/*    MERCHANTABILITY  AND FITNESS FOR  A PARTICULAR  PURPOSE ARE    */
/*    DISCLAIMED.  IN  NO EVENT SHALL THE UNIVERSITY  OF TEXAS AT    */
/*    AUSTIN OR CONTRIBUTORS BE  LIABLE FOR ANY DIRECT, INDIRECT,    */
/*    INCIDENTAL,  SPECIAL, EXEMPLARY,  OR  CONSEQUENTIAL DAMAGES    */
/*    (INCLUDING, BUT  NOT LIMITED TO,  PROCUREMENT OF SUBSTITUTE    */
/*    GOODS  OR  SERVICES; LOSS  OF  USE,  DATA,  OR PROFITS;  OR    */
/*    BUSINESS INTERRUPTION) HOWEVER CAUSED  AND ON ANY THEORY OF    */
/*    LIABILITY, WHETHER  IN CONTRACT, STRICT  LIABILITY, OR TORT    */
/*    (INCLUDING NEGLIGENCE OR OTHERWISE)  ARISING IN ANY WAY OUT    */
/*    OF  THE  USE OF  THIS  SOFTWARE,  EVEN  IF ADVISED  OF  THE    */
/*    POSSIBILITY OF SUCH DAMAGE.                                    */
/*                                                                   */
/* The views and conclusions contained in the software and           */
/* documentation are those of the authors and should not be          */
/* interpreted as representing official policies, either expressed   */
/* or implied, of The University of Texas at Austin.                 */
/*********************************************************************/

#include <stdio.h>
#include <ctype.h>
#include "common.h"

const static FLOAT dm1 = -1.;

int CNAME(BLASLONG m, FLOAT *a, FLOAT *b, BLASLONG incb, void *buffer){
  
  BLASLONG i;
#if (TRANSA == 2) || (TRANSA == 4)
  FLOAT _Complex result;
#endif
#ifndef UNIT
  FLOAT ar, ai, br, bi, ratio, den;
#endif
  FLOAT *gemvbuffer = (FLOAT *)buffer;
  FLOAT *B = b;

  if (incb != 1) {
    B = buffer;
    gemvbuffer = (FLOAT *)(((BLASLONG)buffer + m * sizeof(FLOAT) * 2 + 4095) & ~4095);
    COPY_K(m, b, incb, buffer, 1);
  }

  for (i = 0; i < m; i++) {
    
#if (TRANSA == 2) || (TRANSA == 4)
    if (i > 0) {
#if TRANSA == 2
      result = DOTU_K(i, a, 1, B, 1);
#else
      result = DOTC_K(i, a, 1, B, 1);
#endif
      
      B[i * COMPSIZE + 0] -= CREAL(result);
      B[i * COMPSIZE + 1] -= CIMAG(result);
    }
#endif

#ifndef UNIT
#if (TRANSA == 1) || (TRANSA == 3)
      ar = a[0];
      ai = a[1];
#else
      ar = a[i * COMPSIZE + 0];
      ai = a[i * COMPSIZE + 1];
#endif
      
      if (fabs(ar) >= fabs(ai)){
	ratio = ai / ar;
	den = 1./(ar * ( 1 + ratio * ratio));
	
	ar =  den;
#if TRANSA < 3
	ai = -ratio * den;
#else
	ai =  ratio * den;
#endif
      } else {
	ratio = ar / ai;
	den = 1./(ai * ( 1 + ratio * ratio));
	ar =  ratio * den;
#if TRANSA < 3
	ai = -den;
#else
	ai =  den;
#endif
    }

      br = B[i * COMPSIZE + 0];
      bi = B[i * COMPSIZE + 1];
      
      B[i * COMPSIZE + 0] = ar*br - ai*bi;
      B[i * COMPSIZE + 1] = ar*bi + ai*br;
#endif

#if (TRANSA == 1) || (TRANSA == 3)
      if (i < m - 1) {
#if TRANSA == 1
	AXPYU_K(m - i  - 1 , 0, 0,
	       - B[i * COMPSIZE + 0], - B[i * COMPSIZE + 1],
	       a + COMPSIZE, 1, B + (i + 1) * COMPSIZE, 1, NULL, 0);
#else
	AXPYC_K(m - i  - 1 , 0, 0,
	       - B[i * COMPSIZE + 0], - B[i * COMPSIZE + 1],
	       a + COMPSIZE, 1, B + (i + 1) * COMPSIZE, 1, NULL, 0);
#endif
    }
#endif

#if (TRANSA == 1) || (TRANSA == 3)
    a += (m - i) * 2;
#else
    a += (i + 1) * 2;
#endif
    }

  if (incb != 1) {
    COPY_K(m, buffer, 1, b, incb);
  }

  return 0;
}

