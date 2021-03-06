#ifndef TH_GENERIC_FILE
#define TH_GENERIC_FILE "generic/THTensorMath.cpp"
#else

#include <TH/generic/THTensorApply.hpp>

// HEY YOU!
//
// Looking for a function which used to be in THTensorMath.cpp, but
// can't find it anymore?  Check THTensorMoreMath.cpp and
// THTensorEvenMoreMath.cpp.  These source files have been split up
// because they were getting too big (a whopping 4669 lines at time
// of writing) and causing MSVC to run out of memory.  Did you come
// here because you saw:
//
//    fatal error C1002: compiler is out of heap space in pass 2
//
// Try splitting up the file some more.
//
// At some point, we should reorganize these files in a way that makes
// sense (rather than just having cut the file down the middle, which is
// what I did when I split these up originally).


// Should wrap if the value (a) has a different sign than the divisor (b), but is not 0.
static inline bool modulo_wrap(real a, real b) {
  return (a != 0) && (a < 0) != (b < 0);
}

void THTensor_(bitor)(THTensor *r_, THTensor *t, real value)
{
#if defined(TH_REAL_IS_FLOAT) || defined(TH_REAL_IS_DOUBLE) || defined(TH_REAL_IS_HALF)
  (void)r_;
  (void)t;
  (void)value;
  return THError("bitor is only supported for integer type tensors");
#else
  THTensor_(resizeAs)(r_, t);
  int64_t r_Size = THTensor_(nElement)(r_);
  int r_Contig = THTensor_(isContiguous)(r_);
  int tContig = THTensor_(isContiguous)(t);
  int serial_path = 0;
  if (r_Contig && tContig) {
    real *tp = THTensor_(data)(t);
    real *rp = THTensor_(data)(r_);
    int64_t i;
    #pragma omp parallel for if(r_Size > TH_OMP_OVERHEAD_THRESHOLD * 100) private(i)
    for (i=0; i<r_Size; i++) {
      rp[i] = tp[i] | value;
    }
  } else {
#ifdef _OPENMP
    int inOMP = omp_in_parallel();
    if (inOMP) {
      serial_path = 1;
    } else {
      TH_TENSOR_APPLY2_OMP(r_Size, r_Contig, tContig, real, r_, real, t, *r__data = *t_data | value;, UNCERTAIN_TH_OMP_OVERHEAD_THRESHOLD);
    }
#else
    serial_path = 1;
#endif
  }
  if (serial_path) {
    TH_TENSOR_APPLY2(real, r_, real, t, *r__data = *t_data | value;);
  }
#endif
}

void THTensor_(bitxor)(THTensor *r_, THTensor *t, real value)
{
#if defined(TH_REAL_IS_FLOAT) || defined(TH_REAL_IS_DOUBLE) || defined(TH_REAL_IS_HALF)
  (void)r_;
  (void)t;
  (void)value;
  return THError("bitxor is only supported for integer type tensors");
#else
  THTensor_(resizeAs)(r_, t);
  int64_t r_Size = THTensor_(nElement)(r_);
  int r_Contig = THTensor_(isContiguous)(r_);
  int tContig = THTensor_(isContiguous)(t);
  int serial_path = 0;
  if (r_Contig && tContig) {
    real *tp = THTensor_(data)(t);
    real *rp = THTensor_(data)(r_);
    int64_t i;
    #pragma omp parallel for if(r_Size > TH_OMP_OVERHEAD_THRESHOLD * 100) private(i)
    for (i=0; i<r_Size; i++) {
      rp[i] = tp[i] ^ value;
    }
  } else {
#ifdef _OPENMP
    int inOMP = omp_in_parallel();
    if (inOMP) {
      serial_path = 1;
    } else {
      TH_TENSOR_APPLY2_OMP(r_Size, r_Contig, tContig, real, r_, real, t, *r__data = *t_data ^ value;, UNCERTAIN_TH_OMP_OVERHEAD_THRESHOLD);
    }
#else
    serial_path = 1;
#endif
  }
  if (serial_path) {
    TH_TENSOR_APPLY2(real, r_, real, t, *r__data = *t_data ^ value;);
  }
#endif
}

void THTensor_(clamp)(THTensor *r_, THTensor *t, real min_value, real max_value)
{
  THTensor_(resizeAs)(r_, t);
  int64_t r_Size = THTensor_(nElement)(r_);
  int r_Contig = THTensor_(isContiguous)(r_);
  int tContig = THTensor_(isContiguous)(t);
  int serial_path = 0;
  if (r_Contig && tContig) {
    real *tp = THTensor_(data)(t);
    real *rp = THTensor_(data)(r_);
    /* real t_val; */
    int64_t i;
    #pragma omp parallel for if(r_Size > TH_OMP_OVERHEAD_THRESHOLD) private(i)
    for (i=0; i<r_Size; i++)
      rp[i] = (tp[i] < min_value) ? min_value : (tp[i] > max_value ? max_value : tp[i]);
  } else {
#ifdef _OPENMP
    int inOMP = omp_in_parallel();
    if (inOMP) {
      serial_path = 1;
    } else {
      TH_TENSOR_APPLY2_OMP(r_Size, r_Contig, tContig, real, r_, real, t, *r__data = (*t_data < min_value) ? min_value : (*t_data > max_value ? max_value : *t_data);, UNCERTAIN_TH_OMP_OVERHEAD_THRESHOLD);
    }
#else
    serial_path = 1;
#endif
  }
  if (serial_path) {
    TH_TENSOR_APPLY2(real, r_, real, t, *r__data = (*t_data < min_value) ? min_value : (*t_data > max_value ? max_value : *t_data););
  }
}

void THTensor_(cadd)(THTensor *r_, THTensor *t, real value, THTensor *src)
{
  THTensor_(resizeAs)(r_, t);
  int64_t r_Size = THTensor_(nElement)(r_);
  int64_t srcSize = THTensor_(nElement)(src);
  int r_Contig = THTensor_(isContiguous)(r_);
  int tContig = THTensor_(isContiguous)(t);
  int srcContig = THTensor_(isContiguous)(src);
  int serial_path = 0;
  if (srcSize == r_Size){
    if (r_Contig && tContig && srcContig) {
      if(r_ == t) {
        THBlas_(axpy)(THTensor_(nElement)(t), value, THTensor_(data)(src), 1, THTensor_(data)(r_), 1);
      } else {
        TH_TENSOR_APPLY3_CONTIG(real, r_, real, t, real, src, THVector_(cadd)(r__data, t_data, src_data, value, r__len););
      }
    } else {
#if _OPENMP
      int inOMP = omp_in_parallel();
      if (inOMP) {
        serial_path = 1;
      } else {
        TH_TENSOR_APPLY3_OMP(r_Size, r_Contig, tContig, srcContig, real, r_, real, t, real, src, *r__data = *t_data + value * *src_data;, UNCERTAIN_TH_OMP_OVERHEAD_THRESHOLD);
      }
#else
      serial_path = 1;
#endif
    }
  } else {
    serial_path = 1;
  }
  if (serial_path) {
    TH_TENSOR_APPLY3(real, r_, real, t, real, src, *r__data = *t_data + value * *src_data;);
  }
}

void THTensor_(csub)(THTensor *r_, THTensor *t, real value, THTensor *src)
{
  THTensor_(cadd)(r_, t, -value, src);
}

void THTensor_(cmul)(THTensor *r_, THTensor *t, THTensor *src)
{
  THTensor_(resizeAs)(r_, t);
  int64_t r_Size = THTensor_(nElement)(r_);
  int64_t srcSize = THTensor_(nElement)(src);
  int r_Contig = THTensor_(isContiguous)(r_);
  int tContig = THTensor_(isContiguous)(t);
  int srcContig = THTensor_(isContiguous)(src);
  int serial_path = 0;
  if (srcSize == r_Size){
    if (r_Contig && tContig && srcContig) {
      TH_TENSOR_APPLY3_CONTIG(real, r_, real, t, real, src, THVector_(cmul)(r__data, t_data, src_data, r__len););
    } else {
#if _OPENMP
      int inOMP = omp_in_parallel();
      if (inOMP) {
        serial_path = 1;
      } else {
        TH_TENSOR_APPLY3_OMP(r_Size, r_Contig, tContig, srcContig, real, r_, real, t, real, src, *r__data = *t_data * *src_data;, UNCERTAIN_TH_OMP_OVERHEAD_THRESHOLD);
      }
#else
      serial_path = 1;
#endif
    }
  } else {
    serial_path = 1;
  }
  if (serial_path) {
    TH_TENSOR_APPLY3(real, r_, real, t, real, src, *r__data = *t_data * *src_data;);
  }
}

void THTensor_(pow)(THTensor *r_, THTensor *t, real value)
{
  THTensor_(resizeAs)(r_, t);
  if(value == 1){
    THTensor_(copy)(r_, t);
  }
  else if(value == 2){
    THTensor_(cmul)(r_, t, t);
  }
  else if(value == 3){
    TH_TENSOR_APPLY2(real, r_, real, t, *r__data = *t_data * *t_data * *t_data;);
  }
#if defined(TH_REAL_IS_FLOAT) || defined(TH_REAL_IS_DOUBLE)
#if defined (TH_REAL_IS_FLOAT)
#define TH_MATH_NAME(fn) fn##f
#else
#define TH_MATH_NAME(fn) fn
#endif
  else if(value == 0.5){
    THTensor_(sqrt)(r_, t);
  }
  else if(value == -0.5){
    THTensor_(rsqrt)(r_, t);
  }
  else if(value == -1){
    THTensor_(cinv)(r_, t);
  }
  else if(value == -2){
    TH_TENSOR_APPLY2(real, r_, real, t, *r__data = TH_MATH_NAME(1.0) / (*t_data * *t_data););
  }
  else{
    TH_TENSOR_APPLY2(real, r_, real, t, *r__data = TH_MATH_NAME(pow)(*t_data, value););
  }
#undef TH_MATH_NAME
#else
  else {
    TH_TENSOR_APPLY2(real, r_, real, t, *r__data = THTensor_(powOne)(*t_data, value););
  }
#endif
}

void THTensor_(cpow)(THTensor *r_, THTensor *t, THTensor *src)
{
  THTensor_(resizeAs)(r_, t);
  int64_t r_Size = THTensor_(nElement)(r_);
  int64_t srcSize = THTensor_(nElement)(src);
  int r_Contig = THTensor_(isContiguous)(r_);
  int tContig = THTensor_(isContiguous)(t);
  int srcContig = THTensor_(isContiguous)(src);
  int serial_path = 0;
  if (srcSize == r_Size){
    if (r_Contig && tContig && srcContig) {
      real *tp = THTensor_(data)(t);
      real *sp = THTensor_(data)(src);
      real *rp = THTensor_(data)(r_);
      int64_t i;
      #pragma omp parallel for if(r_Size > TH_OMP_OVERHEAD_THRESHOLD) private(i)
      for (i=0; i<r_Size; i++)
        rp[i] = THTensor_(powOne)(tp[i], sp[i]);
    } else {
#if _OPENMP
      int inOMP = omp_in_parallel();
      if (inOMP) {
        serial_path = 1;
      } else {
        TH_TENSOR_APPLY3_OMP(r_Size, r_Contig, tContig, srcContig, real, r_, real, t, real, src, *r__data = THTensor_(powOne)(*t_data, *src_data);, UNCERTAIN_TH_OMP_OVERHEAD_THRESHOLD);
      }
#else
      serial_path = 1;
#endif
    }
  } else {
    serial_path = 1;
  }
  if (serial_path) {
    TH_TENSOR_APPLY3(real, r_, real, t, real, src, *r__data = THTensor_(powOne)(*t_data, *src_data););
  }
}

void THTensor_(cdiv)(THTensor *r_, THTensor *t, THTensor *src)
{
  THTensor_(resizeAs)(r_, t);
  int64_t r_Size = THTensor_(nElement)(r_);
  int64_t srcSize = THTensor_(nElement)(src);
  int r_Contig = THTensor_(isContiguous)(r_);
  int tContig = THTensor_(isContiguous)(t);
  int srcContig = THTensor_(isContiguous)(src);
  int serial_path = 0;
  if (srcSize == r_Size){
    if (r_Contig && tContig && srcContig) {
      TH_TENSOR_APPLY3_CONTIG(real, r_, real, t, real, src, THVector_(cdiv)(r__data, t_data, src_data, r__len););
    } else {
#if _OPENMP
      int inOMP = omp_in_parallel();
      if (inOMP) {
        serial_path = 1;
      } else {
        TH_TENSOR_APPLY3_OMP(r_Size, r_Contig, tContig, srcContig, real, r_, real, t, real, src, *r__data = *t_data / *src_data;, UNCERTAIN_TH_OMP_OVERHEAD_THRESHOLD);
      }
#else
      serial_path = 1;
#endif
    }
  } else {
    serial_path = 1;
  }
  if (serial_path) {
    TH_TENSOR_APPLY3(real, r_, real, t, real, src, *r__data = *t_data / *src_data;);
  }
}

void THTensor_(clshift)(THTensor *r_, THTensor *t, THTensor *src)
{
#if defined(TH_REAL_IS_HALF)
  return THError("clshift is not supported for torch.HalfTensor");
#endif
  THTensor_(resizeAs)(r_, t);
  int64_t r_Size = THTensor_(nElement)(r_);
  int64_t srcSize = THTensor_(nElement)(src);
  int r_Contig = THTensor_(isContiguous)(r_);
  int tContig = THTensor_(isContiguous)(t);
  int srcContig = THTensor_(isContiguous)(src);
  int serial_path = 0;
  if (srcSize == r_Size){
    if (r_Contig && tContig && srcContig) {
      real *tp = THTensor_(data)(t);
      real *sp = THTensor_(data)(src);
      real *rp = THTensor_(data)(r_);
      int64_t i;
      #pragma omp parallel for if(r_Size > TH_OMP_OVERHEAD_THRESHOLD) private(i)
      for (i=0; i<r_Size; i++) {
#if defined(TH_REAL_IS_FLOAT)
        rp[i] = tp[i] * powf(2, sp[i]);
#elif defined(TH_REAL_IS_DOUBLE)
        rp[i] = tp[i] * pow(2, sp[i]);
#elif defined(TH_REAL_IS_BYTE)
        rp[i] = ((real) tp[i]) << sp[i];
#else
        rp[i] = ((ureal) tp[i]) << sp[i];
#endif
      }
    } else {
#if _OPENMP
      int inOMP = omp_in_parallel();
      if (inOMP) {
        serial_path = 1;
      } else {
#if defined(TH_REAL_IS_FLOAT)
        TH_TENSOR_APPLY3_OMP(r_Size, r_Contig, tContig, srcContig, real, r_, real, t, real, src, *r__data = *t_data * powf(2, *src_data);, UNCERTAIN_TH_OMP_OVERHEAD_THRESHOLD);
#elif defined(TH_REAL_IS_DOUBLE)
        TH_TENSOR_APPLY3_OMP(r_Size, r_Contig, tContig, srcContig, real, r_, real, t, real, src, *r__data = *t_data * pow(2, *src_data);, UNCERTAIN_TH_OMP_OVERHEAD_THRESHOLD);
#elif defined(TH_REAL_IS_BYTE)
        TH_TENSOR_APPLY3_OMP(r_Size, r_Contig, tContig, srcContig, real, r_, real, t, real, src, *r__data = ((real)*t_data) << *src_data;, UNCERTAIN_TH_OMP_OVERHEAD_THRESHOLD);
#else
        TH_TENSOR_APPLY3_OMP(r_Size, r_Contig, tContig, srcContig, real, r_, real, t, real, src, *r__data = ((ureal)*t_data) << *src_data;, UNCERTAIN_TH_OMP_OVERHEAD_THRESHOLD);
#endif
      }
#else
      serial_path = 1;
#endif
    }
  } else {
    serial_path = 1;
  }
  if (serial_path) {
#if defined(TH_REAL_IS_FLOAT)
      TH_TENSOR_APPLY3(real, r_, real, t, real, src, *r__data = *t_data * powf(2, *src_data););
#elif defined(TH_REAL_IS_DOUBLE)
      TH_TENSOR_APPLY3(real, r_, real, t, real, src, *r__data = *t_data * pow(2, *src_data););
#elif defined(TH_REAL_IS_BYTE)
      TH_TENSOR_APPLY3(real, r_, real, t, real, src, *r__data = ((real)*t_data) << *src_data;);
#else
      TH_TENSOR_APPLY3(real, r_, real, t, real, src, *r__data = ((ureal)*t_data) << *src_data;);
#endif
  }
}

void THTensor_(crshift)(THTensor *r_, THTensor *t, THTensor *src)
{
#if defined(TH_REAL_IS_HALF)
  return THError("crshift is not supported for torch.HalfTensor");
#endif
  THTensor_(resizeAs)(r_, t);
  int64_t r_Size = THTensor_(nElement)(r_);
  int64_t srcSize = THTensor_(nElement)(src);
  int r_Contig = THTensor_(isContiguous)(r_);
  int tContig = THTensor_(isContiguous)(t);
  int srcContig = THTensor_(isContiguous)(src);
  int serial_path = 0;
  if (srcSize == r_Size){
    if (r_Contig && tContig && srcContig) {
      real *tp = THTensor_(data)(t);
      real *sp = THTensor_(data)(src);
      real *rp = THTensor_(data)(r_);
      int64_t i;
      #pragma omp parallel for if(r_Size > TH_OMP_OVERHEAD_THRESHOLD) private(i)
      for (i=0; i<r_Size; i++) {
#if defined(TH_REAL_IS_FLOAT)
        rp[i] = tp[i] / powf(2, sp[i]);
#elif defined(TH_REAL_IS_DOUBLE)
        rp[i] = tp[i] / pow(2, sp[i]);
#elif defined(TH_REAL_IS_BYTE)
        rp[i] = ((real) tp[i]) >> sp[i];
#else
        rp[i] = ((ureal) tp[i]) >> sp[i];
#endif
      }
    } else {
#if _OPENMP
      int inOMP = omp_in_parallel();
      if (inOMP) {
        serial_path = 1;
      } else {
#if defined(TH_REAL_IS_FLOAT)
        TH_TENSOR_APPLY3_OMP(r_Size, r_Contig, tContig, srcContig, real, r_, real, t, real, src, *r__data = *t_data / powf(2, *src_data);, UNCERTAIN_TH_OMP_OVERHEAD_THRESHOLD);
#elif defined(TH_REAL_IS_DOUBLE)
        TH_TENSOR_APPLY3_OMP(r_Size, r_Contig, tContig, srcContig, real, r_, real, t, real, src, *r__data = *t_data / pow(2, *src_data);, UNCERTAIN_TH_OMP_OVERHEAD_THRESHOLD);
#elif defined(TH_REAL_IS_BYTE)
        TH_TENSOR_APPLY3_OMP(r_Size, r_Contig, tContig, srcContig, real, r_, real, t, real, src, *r__data = ((real)*t_data) >> *src_data;, UNCERTAIN_TH_OMP_OVERHEAD_THRESHOLD);
#else
        TH_TENSOR_APPLY3_OMP(r_Size, r_Contig, tContig, srcContig, real, r_, real, t, real, src, *r__data = ((ureal)*t_data) >> *src_data;, UNCERTAIN_TH_OMP_OVERHEAD_THRESHOLD);
#endif
      }
#else
      serial_path = 1;
#endif
    }
  } else {
    serial_path = 1;
  }
  if (serial_path) {
#if defined(TH_REAL_IS_FLOAT)
      TH_TENSOR_APPLY3(real, r_, real, t, real, src, *r__data = *t_data / powf(2, *src_data););
#elif defined(TH_REAL_IS_DOUBLE)
      TH_TENSOR_APPLY3(real, r_, real, t, real, src, *r__data = *t_data / pow(2, *src_data););
#elif defined(TH_REAL_IS_BYTE)
      TH_TENSOR_APPLY3(real, r_, real, t, real, src, *r__data = ((real)*t_data) >> *src_data;);
#else
      TH_TENSOR_APPLY3(real, r_, real, t, real, src, *r__data = ((ureal)*t_data) >> *src_data;);
#endif
  }
}

void THTensor_(cfmod)(THTensor *r_, THTensor *t, THTensor *src)
{
  THTensor_(resizeAs)(r_, t);
  int64_t r_Size = THTensor_(nElement)(r_);
  int64_t srcSize = THTensor_(nElement)(src);
  int r_Contig = THTensor_(isContiguous)(r_);
  int tContig = THTensor_(isContiguous)(t);
  int srcContig = THTensor_(isContiguous)(src);
  int serial_path = 0;
  if (srcSize == r_Size){
    if (r_Contig && tContig && srcContig) {
      real *tp = THTensor_(data)(t);
      real *sp = THTensor_(data)(src);
      real *rp = THTensor_(data)(r_);
      int64_t i;
      #pragma omp parallel for if(r_Size > TH_OMP_OVERHEAD_THRESHOLD) private(i)
      for (i=0; i<r_Size; i++) {
#if defined(TH_REAL_IS_FLOAT) || defined(TH_REAL_IS_DOUBLE)
        rp[i] = fmod(tp[i], sp[i]);
#else
        rp[i] = tp[i] % sp[i];
#endif
      }
    } else {
#if _OPENMP
      int inOMP = omp_in_parallel();
      if (inOMP) {
        serial_path = 1;
      } else {
#if defined(TH_REAL_IS_FLOAT) || defined(TH_REAL_IS_DOUBLE)
        TH_TENSOR_APPLY3_OMP(r_Size, r_Contig, tContig, srcContig,real, r_, real, t, real, src, *r__data = fmod(*t_data, *src_data);, UNCERTAIN_TH_OMP_OVERHEAD_THRESHOLD);
#else
        TH_TENSOR_APPLY3_OMP(r_Size, r_Contig, tContig, srcContig, real, r_, real, t, real, src, *r__data = (*t_data % *src_data);, UNCERTAIN_TH_OMP_OVERHEAD_THRESHOLD);
#endif
      }
#else
      serial_path = 1;
#endif
    }
  } else {
    serial_path = 1;
  }
  if (serial_path) {
#if defined(TH_REAL_IS_FLOAT) || defined(TH_REAL_IS_DOUBLE)
    TH_TENSOR_APPLY3(real, r_, real, t, real, src, *r__data = fmod(*t_data, *src_data););
#else
    TH_TENSOR_APPLY3(real, r_, real, t, real, src, *r__data = (*t_data % *src_data););
#endif
  }
}

void THTensor_(cremainder)(THTensor *r_, THTensor *t, THTensor *src)
{
  THTensor_(resizeAs)(r_, t);
  int64_t r_Size = THTensor_(nElement)(r_);
  int64_t srcSize = THTensor_(nElement)(src);
  int r_Contig = THTensor_(isContiguous)(r_);
  int tContig = THTensor_(isContiguous)(t);
  int srcContig = THTensor_(isContiguous)(src);
  int serial_path = 0;
  if (srcSize == r_Size){
    if (r_Contig && tContig && srcContig) {
      real *tp = THTensor_(data)(t);
      real *sp = THTensor_(data)(src);
      real *rp = THTensor_(data)(r_);
      int64_t i;
      #pragma omp parallel for if(r_Size > TH_OMP_OVERHEAD_THRESHOLD) private(i)
      for (i=0; i<r_Size; i++) {
#if defined(TH_REAL_IS_FLOAT) || defined(TH_REAL_IS_DOUBLE)
        rp[i] = (sp[i] == 0)? NAN : tp[i] - sp[i] * floor(tp[i] / sp[i]);
#else
        // There is no NAN for integers
        rp[i] = tp[i] % sp[i];
        if (modulo_wrap(rp[i], sp[i]))
          rp[i] += sp[i];
#endif
      }
    } else {
#if _OPENMP
      int inOMP = omp_in_parallel();
      if (inOMP) {
        serial_path = 1;
      } else {
#if defined(TH_REAL_IS_FLOAT) || defined(TH_REAL_IS_DOUBLE)
        TH_TENSOR_APPLY3_OMP(r_Size, r_Contig, tContig, srcContig, real, r_, real, t, real, src, *r__data = (*src_data == 0)? NAN : *t_data - *src_data * floor(*t_data / *src_data);, UNCERTAIN_TH_OMP_OVERHEAD_THRESHOLD);
#else
        TH_TENSOR_APPLY3_OMP(r_Size, r_Contig, tContig, srcContig, real, r_, real, t, real, src, *r__data = *t_data % *src_data;
                                                     if (modulo_wrap(*r__data, *src_data)) *r__data += *src_data;, UNCERTAIN_TH_OMP_OVERHEAD_THRESHOLD);
#endif
      }
#else
      serial_path = 1;
#endif
    }
  } else {
    serial_path = 1;
  }
  if (serial_path) {
#if defined(TH_REAL_IS_FLOAT) || defined(TH_REAL_IS_DOUBLE)
    TH_TENSOR_APPLY3(real, r_, real, t, real, src, *r__data = (*src_data == 0)? NAN : *t_data - *src_data * floor(*t_data / *src_data););
#else
    // There is no NAN for integers
    TH_TENSOR_APPLY3(real, r_, real, t, real, src, *r__data = *t_data % *src_data;
                                                     if (modulo_wrap(*r__data, *src_data)) *r__data += *src_data;);
#endif

  }
}

void THTensor_(cbitand)(THTensor *r_, THTensor *t, THTensor *src)
{
#if defined(TH_REAL_IS_FLOAT) || defined(TH_REAL_IS_DOUBLE) || defined(TH_REAL_IS_HALF)
  (void)r_;
  (void)t;
  (void)src;
  return THError("cbitand is only supported for integer type tensors");
#else
  THTensor_(resizeAs)(r_, t);
  int64_t r_Size = THTensor_(nElement)(r_);
  int64_t srcSize = THTensor_(nElement)(src);
  int r_Contig = THTensor_(isContiguous)(r_);
  int tContig = THTensor_(isContiguous)(t);
  int srcContig = THTensor_(isContiguous)(src);
  int serial_path = 0;
  if (srcSize == r_Size){
    if (r_Contig && tContig && srcContig) {
      real *tp = THTensor_(data)(t);
      real *sp = THTensor_(data)(src);
      real *rp = THTensor_(data)(r_);
      int64_t i;
      #pragma omp parallel for if(r_Size > TH_OMP_OVERHEAD_THRESHOLD) private(i)
      for (i=0; i<r_Size; i++) {
        rp[i] = tp[i] & sp[i];
      }
    } else {
#if _OPENMP
      int inOMP = omp_in_parallel();
      if (inOMP) {
        serial_path = 1;
      } else {
        TH_TENSOR_APPLY3_OMP(r_Size, r_Contig, tContig, srcContig, real, r_, real, t, real, src, *r__data = *t_data & *src_data;, UNCERTAIN_TH_OMP_OVERHEAD_THRESHOLD);
      }
#else
      serial_path = 1;
#endif
    }
  } else {
    serial_path = 1;
  }
  if (serial_path) {
      TH_TENSOR_APPLY3(real, r_, real, t, real, src, *r__data = *t_data & *src_data;);
  }
#endif
}

void THTensor_(cbitor)(THTensor *r_, THTensor *t, THTensor *src)
{
#if defined(TH_REAL_IS_FLOAT) || defined(TH_REAL_IS_DOUBLE) || defined(TH_REAL_IS_HALF)
  (void)r_;
  (void)t;
  (void)src;
  return THError("cbitor is only supported for integer type tensors");
#else
  THTensor_(resizeAs)(r_, t);
  int64_t r_Size = THTensor_(nElement)(r_);
  int64_t srcSize = THTensor_(nElement)(src);
  int r_Contig = THTensor_(isContiguous)(r_);
  int tContig = THTensor_(isContiguous)(t);
  int srcContig = THTensor_(isContiguous)(src);
  int serial_path = 0;
  if (srcSize == r_Size){
    if (r_Contig && tContig && srcContig) {
      real *tp = THTensor_(data)(t);
      real *sp = THTensor_(data)(src);
      real *rp = THTensor_(data)(r_);
      int64_t i;
      #pragma omp parallel for if(r_Size > TH_OMP_OVERHEAD_THRESHOLD) private(i)
      for (i=0; i<r_Size; i++) {
        rp[i] = tp[i] | sp[i];
      }
    } else {
#if _OPENMP
      int inOMP = omp_in_parallel();
      if (inOMP) {
        serial_path = 1;
      } else {
        TH_TENSOR_APPLY3_OMP(r_Size, r_Contig, tContig, srcContig, real, r_, real, t, real, src, *r__data = *t_data | *src_data;, UNCERTAIN_TH_OMP_OVERHEAD_THRESHOLD);
      }
#else
      serial_path = 1;
#endif
    }
  } else {
    serial_path = 1;
  }
  if (serial_path) {
      TH_TENSOR_APPLY3(real, r_, real, t, real, src, *r__data = *t_data | *src_data;);
  }
#endif
}

void THTensor_(cbitxor)(THTensor *r_, THTensor *t, THTensor *src)
{
#if defined(TH_REAL_IS_FLOAT) || defined(TH_REAL_IS_DOUBLE) || defined(TH_REAL_IS_HALF)
  (void)r_;
  (void)t;
  (void)src;
  return THError("cbitxor is only supported for integer type tensors");
#else
  THTensor_(resizeAs)(r_, t);
  int64_t r_Size = THTensor_(nElement)(r_);
  int64_t srcSize = THTensor_(nElement)(src);
  int r_Contig = THTensor_(isContiguous)(r_);
  int tContig = THTensor_(isContiguous)(t);
  int srcContig = THTensor_(isContiguous)(src);
  int serial_path = 0;
  if (srcSize == r_Size){
    if (r_Contig && tContig && srcContig) {
      real *tp = THTensor_(data)(t);
      real *sp = THTensor_(data)(src);
      real *rp = THTensor_(data)(r_);
      int64_t i;
      #pragma omp parallel for if(r_Size > TH_OMP_OVERHEAD_THRESHOLD) private(i)
      for (i=0; i<r_Size; i++) {
        rp[i] = tp[i] ^ sp[i];
      }
    } else {
#if _OPENMP
      int inOMP = omp_in_parallel();
      if (inOMP) {
        serial_path = 1;
      } else {
        TH_TENSOR_APPLY3_OMP(r_Size, r_Contig, tContig, srcContig, real, r_, real, t, real, src, *r__data = *t_data ^ *src_data;, UNCERTAIN_TH_OMP_OVERHEAD_THRESHOLD);
      }
#else
      serial_path = 1;
#endif
    }
  } else {
    serial_path = 1;
  }
  if (serial_path) {
      TH_TENSOR_APPLY3(real, r_, real, t, real, src, *r__data = *t_data ^ *src_data;);
  }
#endif
}

void THTensor_(tpow)(THTensor *r_, real value, THTensor *t)
{
  THTensor_(resizeAs)(r_, t);
  int64_t r_Size = THTensor_(nElement)(r_);
  int r_Contig = THTensor_(isContiguous)(r_);
  int tContig = THTensor_(isContiguous)(t);
  int serial_path = 0;
  if (r_Contig && tContig) {
    real *tp = THTensor_(data)(t);
    real *rp = THTensor_(data)(r_);
    int64_t i;
    #pragma omp parallel for if(r_Size > TH_OMP_OVERHEAD_THRESHOLD) private(i)
    for (i=0; i<r_Size; i++)
      rp[i] = THTensor_(powOne)(value, tp[i]);
  } else {
#if _OPENMP
    int inOMP = omp_in_parallel();
    if (inOMP) {
      serial_path = 1;
    } else {
      TH_TENSOR_APPLY2_OMP(r_Size, r_Contig, tContig, real, r_, real, t, *r__data = THTensor_(powOne)(value, *t_data);, UNCERTAIN_TH_OMP_OVERHEAD_THRESHOLD);
    }
#else
    serial_path = 1;
#endif
  }
  if (serial_path) {
    TH_TENSOR_APPLY2(real, r_, real, t, *r__data = THTensor_(powOne)(value, *t_data););
  }
}

void THTensor_(addcmul)(THTensor *r_, THTensor *t, real value, THTensor *src1, THTensor *src2)
{
  if(r_ != t)
  {
    THTensor_(resizeAs)(r_, t);
    THTensor_(copy)(r_, t);
  }
  int64_t r_Size = THTensor_(nElement)(r_);
  int64_t src1Size = THTensor_(nElement)(src1);
  int64_t src2Size = THTensor_(nElement)(src2);
  int r_Contig = THTensor_(isContiguous)(r_);
  int src1Contig = THTensor_(isContiguous)(src1);
  int src2Contig = THTensor_(isContiguous)(src2);
  int serial_path = 0;
  if( (src1Size == src2Size) && (src1Size == r_Size) ){
#if _OPENMP
    int inOMP = omp_in_parallel();
    if (inOMP) {
      serial_path = 1;
    } else {
      TH_TENSOR_APPLY3_OMP(r_Size, r_Contig, src1Contig, src2Contig, real, r_, real, src1, real, src2, *r__data += value * *src1_data * *src2_data;, UNCERTAIN_TH_OMP_OVERHEAD_THRESHOLD);
    }
#else
    (void)r_Contig;
    (void)src1Contig;
    (void)src2Contig;
    serial_path = 1;
#endif
  } else {
    serial_path = 1;
  }
  if (serial_path) {
    TH_TENSOR_APPLY3(real, r_, real, src1, real, src2, *r__data += value * *src1_data * *src2_data;);
  }
}

void THTensor_(addcdiv)(THTensor *r_, THTensor *t, real value, THTensor *src1, THTensor *src2)
{
  if(r_ != t)
  {
    THTensor_(resizeAs)(r_, t);
    THTensor_(copy)(r_, t);
  }
  int64_t r_Size = THTensor_(nElement)(r_);
  int64_t src1Size = THTensor_(nElement)(src1);
  int64_t src2Size = THTensor_(nElement)(src2);
  int r_Contig = THTensor_(isContiguous)(r_);
  int src1Contig = THTensor_(isContiguous)(src1);
  int src2Contig = THTensor_(isContiguous)(src2);
  int serial_path = 0;
  if( (src1Size == src2Size) && (src1Size == r_Size) ){
#if _OPENMP
    int inOMP = omp_in_parallel();
    if (inOMP) {
      serial_path = 1;
    } else {
      TH_TENSOR_APPLY3_OMP(r_Size, r_Contig, src1Contig, src2Contig, real, r_, real, src1, real, src2, *r__data += value * *src1_data / *src2_data;, UNCERTAIN_TH_OMP_OVERHEAD_THRESHOLD);
    }
#else
    (void)r_Contig;
    (void)src1Contig;
    (void)src2Contig;
    serial_path = 1;
#endif
  } else {
    serial_path = 1;
  }
  if (serial_path) {
    TH_TENSOR_APPLY3(real, r_, real, src1, real, src2, *r__data += value * *src1_data / *src2_data;);
  }
}

void THTensor_(addmv)(THTensor *r_, real beta, THTensor *t, real alpha, THTensor *mat, THTensor *vec)
{
  if( (mat->dim() != 2) || (THTensor_nDimensionLegacyNoScalars(vec) != 1) )
    THError("matrix and vector expected, got %dD, %dD",
      mat->dim(), THTensor_nDimensionLegacyNoScalars(vec));

  if( mat->size(1) != THTensor_sizeLegacyNoScalars(vec, 0) ) {
    THDescBuff bm = THTensor_(sizeDesc)(mat);
    THDescBuff bv = THTensor_(sizeDesc)(vec);
    THError("size mismatch, %s, %s", bm.str, bv.str);
  }

  if(t->dim() != 1)
    THError("vector expected, got t: %dD", t->dim());

  if(t->size(0) != mat->size(0)) {
    THDescBuff bt = THTensor_(sizeDesc)(t);
    THDescBuff bm = THTensor_(sizeDesc)(mat);
    THError("size mismatch, t: %s, mat: %s", bt.str, bm.str);
  }

  if(r_ != t)
  {
    THTensor_(resizeAs)(r_, t);
    THTensor_(copy)(r_, t);
  }

  // n == 1 || lda >= max(1, m)
  #define LDA_COND(M, N, LDA) ((N) == 1 || (LDA) >= THMax(1, (M)))

  if(mat->stride(0) == 1 && LDA_COND(mat->size(0), mat->size(1), mat->stride(1)))
  {
    THBlas_(gemv)('n', mat->size(0), mat->size(1),
                  alpha, THTensor_(data)(mat), mat->stride(1),
                  THTensor_(data)(vec), THTensor_strideLegacyNoScalars(vec, 0),
                  beta, THTensor_(data)(r_), r_->stride(0));
  }
  else if(mat->stride(1) == 1 && LDA_COND(mat->size(1), mat->size(0), mat->stride(0)))
  {
    THBlas_(gemv)('t',  mat->size(1), mat->size(0),
                  alpha, THTensor_(data)(mat), mat->stride(0),
                  THTensor_(data)(vec), THTensor_strideLegacyNoScalars(vec, 0),
                  beta, THTensor_(data)(r_), r_->stride(0));
  }
  else
  {
    THTensor *cmat = THTensor_(newContiguous)(mat);

    THBlas_(gemv)('t',  mat->size(1), mat->size(0),
                  alpha, THTensor_(data)(cmat), cmat->stride(0),
                  THTensor_(data)(vec), THTensor_strideLegacyNoScalars(vec, 0),
                  beta, THTensor_(data)(r_), r_->stride(0));

    THTensor_(free)(cmat);
  }

  // In gemv (x,0).mv(0) does not
  // handle beta, whereas gemm does for case where (x,0).mm(0,y).
  if (THTensor_sizeLegacyNoScalars(vec, 0) == 0 && mat->size(0) != 0) {
    if (beta == 0) {
      THTensor_(zero)(r_);
    } else if (beta != 1) {
      THTensor_(mul)(r_, r_, beta);
    }
  }

  #undef LDA_COND
}

void THTensor_(match)(THTensor *r_, THTensor *m1, THTensor *m2, real gain)
{
  int64_t N1 = m1->size(0);
  int64_t N2 = m2->size(0);
  int64_t dim;
  real *m1_p;
  real *m2_p;
  real *r_p;
  int64_t i;

  THTensor_(resize2d)(r_, N1, N2);

  m1 = THTensor_(newContiguous)(m1);
  m2 = THTensor_(newContiguous)(m2);

  THTensor_(resize2d)(m1, N1, THTensor_(nElement)(m1) / N1);
  THTensor_(resize2d)(m2, N2, THTensor_(nElement)(m2) / N2);

  dim = m1->size(1);
  THArgCheck(m1->size(1) == m2->size(1), 3, "m1 and m2 must have the same inner vector dim");

  m1_p = THTensor_(data)(m1);
  m2_p = THTensor_(data)(m2);
  r_p = THTensor_(data)(r_);

#pragma omp parallel for private(i)
  for (i=0; i<N1; i++) {
    int64_t j,k;
    for (j=0; j<N2; j++) {
      real sum = 0;
      for (k=0; k<dim; k++) {
        real term = m1_p[ i*dim + k ] - m2_p[ j*dim + k ];
        sum += term*term;
      }
      r_p[ i*N2 + j ] = gain * sum;
    }
  }

  THTensor_(free)(m1);
  THTensor_(free)(m2);
}

void THTensor_(addmm)(THTensor *r_, real beta, THTensor *t, real alpha, THTensor *m1, THTensor *m2)
{
  char transpose_r, transpose_m1, transpose_m2;
  THTensor *r__, *m1_, *m2_;
  int free_m1 = 0;
  int free_m2 = 0;

  if( (m1->dim() != 2) || (m2->dim() != 2))
    THError("matrices expected, got %dD, %dD tensors", m1->dim(), m2->dim());

  if(m1->size(1) != m2->size(0)) {
    THDescBuff bm1 = THTensor_(sizeDesc)(m1);
    THDescBuff bm2 = THTensor_(sizeDesc)(m2);
    THError("size mismatch, m1: %s, m2: %s", bm1.str, bm2.str);
  }

  if( t->dim() != 2 )
    THError("matrix expected, got %dD tensor for t", t->dim());

  if( (t->size(0) != m1->size(0)) || (t->size(1) != m2->size(1)) ) {
    THDescBuff bt  = THTensor_(sizeDesc)(t);
    THDescBuff bm1 = THTensor_(sizeDesc)(m1);
    THDescBuff bm2 = THTensor_(sizeDesc)(m2);
    THError("size mismatch, t: %s, m1: %s, m2: %s", bt.str, bm1.str, bm2.str);
  }

  if(t != r_)
  {
    THTensor_(resizeAs)(r_, t);
    if (beta != 0.0) {
      THTensor_(copy)(r_, t);
    }
  }

  // n == 1 || ldc >= max(1, m)
  #define LDC_COND(M, N, LDC) ((N) == 1 || (LDC) >= THMax(1, M))

  /* r_ */
  if(r_->stride(0) == 1 &&
     LDC_COND(r_->size(0), r_->size(1), r_->stride(1)))
  {
    transpose_r = 'n';
    r__ = r_;
  }
  else if(r_->stride(1) == 1 &&
          LDC_COND(r_->size(1), r_->size(0), r_->stride(0)))
  {
    THTensor *swap = m2;
    m2 = m1;
    m1 = swap;
    transpose_r = 't';
    r__ = r_;
  }
  else
  {
    transpose_r = 'n';
    // make r__ FORTRAN contiguous
    THTensor *transp_r_ = THTensor_(newTranspose)(r_, 0, 1);
    r__ = THTensor_(newClone)(transp_r_);
    THTensor_(free)(transp_r_);
    THTensor_(transpose)(r__, NULL, 0, 1);
  }

  #undef LDC_COND

  int64_t m = r__->size((transpose_r == 'n' ? 0 : 1));
  int64_t n = r__->size((transpose_r == 'n' ? 1 : 0));
  int64_t k = m1->size((transpose_r == 'n' ? 1 : 0));
  int64_t ldr__ = r__->stride((transpose_r == 'n' ? 1 : 0));

  /* m1 */
  /* Need ldm1_ >= max(1, (transpose_m1 == 'n' ? m : k)) */
  if(m1->stride((transpose_r == 'n' ? 0 : 1)) == 1 &&
     m1->stride((transpose_r == 'n' ? 1 : 0)) >= THMax(1, m))
  {
    transpose_m1 = 'n';
    m1_ = m1;
  }
  else if(m1->stride((transpose_r == 'n' ? 1 : 0)) == 1 &&
          m1->stride((transpose_r == 'n' ? 0 : 1)) >= THMax(1, k))
  {
    transpose_m1 = 't';
    m1_ = m1;
  }
  else
  {
    transpose_m1 = (transpose_r == 'n' ? 't' : 'n');
    m1_ = THTensor_(newContiguous)(m1);
    free_m1 = 1;
  }

  /* m2 */
  /* Need ldm2_ >= max(1, (transpose_m2 == 'n' ? k : n)) */
  if(m2->stride((transpose_r == 'n' ? 0 : 1)) == 1 &&
     m2->stride((transpose_r == 'n' ? 1 : 0)) >= THMax(1, k))
  {
    transpose_m2 = 'n';
    m2_ = m2;
  }
  else if(m2->stride((transpose_r == 'n' ? 1 : 0)) == 1 &&
          m2->stride((transpose_r == 'n' ? 0 : 1)) >= THMax(1, n))
  {
    transpose_m2 = 't';
    m2_ = m2;
  }
  else
  {
    transpose_m2 = (transpose_r == 'n' ? 't' : 'n');
    m2_ = THTensor_(newContiguous)(m2);
    free_m2 = 1;
  }

  int64_t ldm1_ = (transpose_m1 == 'n' ? m1_->stride((transpose_r == 'n' ? 1 : 0)) : m1_->stride((transpose_r == 'n' ? 0 : 1)));
  int64_t ldm2_ = (transpose_m2 == 'n' ? m2_->stride((transpose_r == 'n' ? 1 : 0)) : m2_->stride((transpose_r == 'n' ? 0 : 1)));

#pragma omp critical(blasgemm)
  /* do the operation */
  THBlas_(gemm)(transpose_m1,
                transpose_m2,
                m,
                n,
                k,
                alpha,
                THTensor_(data)(m1_),
                ldm1_,
                THTensor_(data)(m2_),
                ldm2_,
                beta,
                THTensor_(data)(r__),
                ldr__);

  /* free intermediate variables */
  if(free_m1)
    THTensor_(free)(m1_);

  if(free_m2)
    THTensor_(free)(m2_);

  if(r__ != r_)
    THTensor_(freeCopyTo)(r__, r_);
}

void THTensor_(addr)(THTensor *r_, real beta, THTensor *t, real alpha, THTensor *vec1, THTensor *vec2)
{
  if( (THTensor_nDimensionLegacyNoScalars(vec1) != 1) || (THTensor_nDimensionLegacyNoScalars(vec2) != 1) )
    THError("vector and vector expected, got %dD, %dD tensors",
        THTensor_nDimensionLegacyNoScalars(vec1), THTensor_nDimensionLegacyNoScalars(vec2));

  if(t->dim() != 2)
    THError("expected matrix, got %dD tensor for t", t->dim());

  auto vec1_size = THTensor_sizeLegacyNoScalars(vec1, 0);
  auto vec2_size = THTensor_sizeLegacyNoScalars(vec2, 0);
  auto vec1_stride = THTensor_strideLegacyNoScalars(vec1, 0);
  auto vec2_stride = THTensor_strideLegacyNoScalars(vec2, 0);

  if( (t->size(0) != vec1_size) || (t->size(1) != vec2_size) ) {
    THDescBuff bt  = THTensor_(sizeDesc)(t);
    THDescBuff bv1 = THTensor_(sizeDesc)(vec1);
    THDescBuff bv2 = THTensor_(sizeDesc)(vec2);
    THError("size mismatch, t: %s, vec1: %s, vec2: %s", bt.str, bv1.str, bv2.str);
  }

  if(r_ != t)
  {
    THTensor_(resizeAs)(r_, t);
    THTensor_(copy)(r_, t);
  }

  if(beta == 0) {
    THTensor_(zero)(r_);
  }
  else if(beta != 1)
    THTensor_(mul)(r_, r_, beta);

  // n == 1 || lda >= max(1, m)
  #define LDA_COND(M, N, LDA) ((N) == 1 || (LDA) >= THMax(1, (M)))

  if(r_->stride(0) == 1 && LDA_COND(vec1_size, vec2_size, r_->stride(1)))
  {
    THBlas_(ger)(vec1_size, vec2_size,
                 alpha, THTensor_(data)(vec1), vec1_stride,
                 THTensor_(data)(vec2), vec2_stride,
                 THTensor_(data)(r_), r_->stride(1));
  }
  else if(r_->stride(1) == 1 && LDA_COND(vec2->size(0), vec1_size, r_->stride(0)))
  {
    THBlas_(ger)(vec2_size, vec1_size,
                 alpha, THTensor_(data)(vec2), vec2_stride,
                 THTensor_(data)(vec1), vec1_stride,
                 THTensor_(data)(r_), r_->stride(0));
  }
  else
  {
    THTensor *cr = THTensor_(newClone)(r_);

    THBlas_(ger)(vec2_size, vec1_size,
                 alpha, THTensor_(data)(vec2), vec2_stride,
                 THTensor_(data)(vec1), vec1_stride,
                 THTensor_(data)(cr), cr->stride(0));

    THTensor_(freeCopyTo)(cr, r_);
  }

  #undef LDA_COND
}

void THTensor_(addbmm)(THTensor *result, real beta, THTensor *t, real alpha, THTensor *batch1, THTensor *batch2)
{
  int64_t batch;

  THArgCheck(THTensor_(nDimensionLegacyNoScalars)(batch1) == 3, 1, "expected 3D tensor");
  THArgCheck(THTensor_(nDimensionLegacyNoScalars)(batch2) == 3, 2, "expected 3D tensor");
  THArgCheck(THTensor_(size)(batch1, 0) == THTensor_(size)(batch2, 0), 2,
             "equal number of batches expected, got %d, %d",
             THTensor_(size)(batch1, 0), THTensor_(size)(batch2, 0));
  THArgCheck(THTensor_(size)(batch1, 2) == THTensor_(size)(batch2, 1), 2,
             "wrong matrix size, batch1: %dx%d, batch2: %dx%d",
             THTensor_(size)(batch1, 1), THTensor_(size)(batch1,2),
             THTensor_(size)(batch2, 1), THTensor_(size)(batch2,2));

  int64_t dim1 = THTensor_(size)(batch1, 1);
  int64_t dim2 = THTensor_(size)(batch2, 2);
  THArgCheck(THTensor_(size)(t, 0) == dim1, 1, "output tensor of incorrect size");
  THArgCheck(THTensor_(size)(t, 1) == dim2, 1, "output tensor of incorrect size");

  if (t != result) {
    THTensor_(resizeAs)(result, t);
    if (beta != 0.0) {
      THTensor_(copy)(result, t);
    }
  }

  THTensor *matrix1 = THTensor_(new)();
  THTensor *matrix2 = THTensor_(new)();

  for (batch = 0; batch < THTensor_(size)(batch1, 0); ++batch) {
    THTensor_(select)(matrix1, batch1, 0, batch);
    THTensor_(select)(matrix2, batch2, 0, batch);

    THTensor_(addmm)(result, beta, result, alpha, matrix1, matrix2);
    beta = 1; // accumulate output once
  }

  THTensor_(free)(matrix1);
  THTensor_(free)(matrix2);
}

#endif /* TH_GENERIC_FILE */
