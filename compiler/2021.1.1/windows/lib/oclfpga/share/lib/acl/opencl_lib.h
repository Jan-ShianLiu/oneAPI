#ifndef __ACL_OPENCL_PREDEFINES__
#define __ACL_OPENCL_PREDEFINES__
#pragma clang system_header
#define CL_VERSION_1_0 100
#if __OPENCL_C_VERSION__ >= 110
#define CL_VERSION_1_1 110
#endif
#if __OPENCL_C_VERSION__ >= 120
#define CL_VERSION_1_2 120
#endif
#if __OPENCL_C_VERSION__ >= 200
#define CL_VERSION_2_0 200
#endif
#define __OPENCL_VERSION__ CL_VERSION_1_0
#if __OPENCL_C_VERSION__ >= 120
#define __CONST_CHAR_STAR_COMPATIBILITY_VERSION__ CL_VERSION_1_2
#endif
#define __EMBEDDED_PROFILE__ 1
#define __ENDIAN_LITTLE__ 1
#define __kernel_exec(X,typen) __kernel __attribute__((work_group_size_hint(X,1,1))) __attribute__((vec_type_hint(typen)))

#define kernel_exec(X,typen) __kernel_exec(X,typen)
#ifdef __FPGA_EMULATION_X86__
   #define __fpga_reg(x) (x)
#elif __has_builtin(__builtin_fpga_reg)
   #ifndef __fpga_reg
       #define __fpga_reg __builtin_fpga_reg
   #endif
#endif

#define CHAR_BIT 8
#define SCHAR_MAX 127
#define SCHAR_MIN (-127-1)
#define CHAR_MAX 127
#define CHAR_MIN (-127-1)
#define SHRT_MAX 32767
#define SHRT_MIN (-32767-1)
#define INT_MAX 0x7fffffff
#define INT_MIN (-0x7fffffff-1)
#define LONG_MAX 0x7fffffffffffffffLL
#define LONG_MIN (-0x7fffffffffffffffLL-1)
#define UCHAR_MAX 255
#define USHRT_MAX 65535
#define UINT_MAX 0xffffffff
#define ULONG_MAX 0xffffffffffffffffULL
#define FP_ILOGBNAN INT_MAX
#define FP_ILOGB0 -INT_MAX
#pragma OPENCL EXTENSION cl_khr_fp64 : enable
#if __OPENCL_C_VERSION__ >= 120
int printf(__constant const char*, ...) __attribute__((format(printf, 1, 2)));
#endif
typedef unsigned __INT64_TYPE__ size_t;
typedef __INT64_TYPE__ ptrdiff_t;
typedef unsigned __INT64_TYPE__ uintptr_t;
typedef __INT64_TYPE__ intptr_t;
#define SIZE_MAX ((size_t)-1)
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned __INT64_TYPE__ ulong;

#define _AV(_T,_n,_a) typedef _T _T##_n __attribute__(( ext_vector_type(_n),aligned(_a) ));
#define _AVP2(_T,_n) _AV(_T,_n,sizeof(_T)*_n)
#if __OPENCL_C_VERSION__ >= 110
#define _IHC_VDECLARES(_T) _AVP2(_T,2) _AVP2(_T,4) _AVP2(_T,8) _AVP2(_T,16) _AV(_T,3,sizeof(_T)*4)
#else
#define _IHC_VDECLARES(_T) _AVP2(_T,2) _AVP2(_T,4) _AVP2(_T,8) _AVP2(_T,16)
#endif
_IHC_VDECLARES(char)
_IHC_VDECLARES(uchar)
_IHC_VDECLARES(short)
_IHC_VDECLARES(ushort)
_IHC_VDECLARES(int)
_IHC_VDECLARES(uint)
_IHC_VDECLARES(long)
_IHC_VDECLARES(ulong)
_IHC_VDECLARES(half)
_IHC_VDECLARES(float)
_IHC_VDECLARES(double)
#undef _AV
#undef _AVP2
#undef _IHC_VDECLARES
size_t __attribute__((__overloadable__,__always_inline__)) get_global_size(uint __d);
size_t __attribute__((__overloadable__,__always_inline__)) get_global_id(uint __d);
size_t __attribute__((__overloadable__,__always_inline__)) get_local_size(uint __d);
size_t __attribute__((__overloadable__,__always_inline__)) get_local_id(uint __d);
size_t __attribute__((__overloadable__,__always_inline__)) get_num_groups(uint __d);
size_t __attribute__((__overloadable__,__always_inline__)) get_group_id(uint __d);
#if __OPENCL_C_VERSION__ >= 110
size_t __attribute__((__overloadable__,__always_inline__)) get_global_offset(uint __d);
#endif
size_t __attribute__((__overloadable__,__always_inline__)) get_local_linear_id();
uint __attribute__((__overloadable__,__always_inline__)) get_work_dim();
typedef enum {  CLK_LOCAL_MEM_FENCE=1,CLK_GLOBAL_MEM_FENCE=2,CLK_CHANNEL_MEM_FENCE=4 } cl_mem_fence_flags;

void __attribute__((  noinline, convergent )) __acl_barrier(cl_mem_fence_flags, uint, uint);
void __attribute__((__overloadable__,__always_inline__)) barrier(cl_mem_fence_flags _f);
void __attribute((noinline, convergent )) __acl_mem_fence(cl_mem_fence_flags);
void __attribute((__overloadable__, convergent )) mem_fence(cl_mem_fence_flags f);
void __attribute((__overloadable__, convergent )) mem_fence(unsigned int f);
void __attribute((__overloadable__, convergent )) mem_fence(int f);
void __attribute(( __always_inline__, convergent )) read_mem_fence(cl_mem_fence_flags f);
void __attribute(( __always_inline__, convergent )) write_mem_fence(cl_mem_fence_flags f);
#define M_E 0X1.5BF0A8B145769P+1
#define M_LOG2E 0X1.71547652B82FEP+0
#define M_LOG10E 0X1.BCB7B1526E50EP-2
#define M_LN2 0X1.62E42FEFA39EFP-1
#define M_1_LN2 1.4426950408889634073599246810019
#define M_LN10 0X1.26BB1BBB55516P+1
#define M_1_LN10 0.43429448190325182765112891891661
#define M_PI 0X1.921FB54442D18P+1
#define M_PI_2 0X1.921FB54442D18P+0
#define M_PI_4 0X1.921FB54442D18P-1
#define M_1_PI 0X1.45F306DC9C883P-2
#define M_2_PI 0X1.45F306DC9C883P-1
#define M_2_SQRTPI 0X1.20DD750429B6DP+0
#define M_SQRT2 0X1.6A09E667F3BCDP+0
#define M_SQRT1_2 0X1.6A09E667F3BCDP-1
#define M_E_F 0X1.5BF0A8B145769P+1f
#define M_E_2_F 0X1.5BF0A8B145769P+0f
#define M_LOG2E_F 0X1.71547652B82FEP+0f
#define M_LOG10E_F 0X1.BCB7B1526E50EP-2f
#define M_LN2_F 0X1.62E42FEFA39EFP-1f
#define M_1_LN2_F 1.4426950408889634073599246810019f
#define M_LN10_F 0X1.26BB1BBB55516P+1f
#define M_1_LN10_F 0.43429448190325182765112891891661f
#define M_PI_F 0X1.921FB54442D18P+1f
#define M_PI_2_F 0X1.921FB54442D18P+0f
#define M_PI_4_F 0X1.921FB54442D18P-1f
#define M_1_PI_F 0X1.45F306DC9C883P-2f
#define M_2_PI_F 0X1.45F306DC9C883P-1f
#define M_2_SQRTPI_F 0X1.20DD750429B6DP+0f
#define M_SQRT2_F 0X1.6A09E667F3BCDP+0f
#define M_SQRT1_2_F 0X1.6A09E667F3BCDP-1f
#define FLT_DIG 6
#define FLT_MANT_DIG 24
#define FLT_MAX_10_EXP +38
#define FLT_MAX_EXP +128
#define FLT_MIN_10_EXP -37
#define FLT_MIN_EXP -125
#define FLT_RADIX 2
#define FLT_MAX 0x1.fffffep127f
#define FLT_MIN 0x1.0p-126f
#define FLT_EPSILON 0x1.0p-23f
#define DBL_DIG 15
#define DBL_MANT_DIG 53
#define DBL_MAX_10_EXP +308
#define DBL_MAX_EXP +1024
#define DBL_MIN_10_EXP -307
#define DBL_MIN_EXP -1021
#define DBL_MAX 0x1.fffffffffffffp1023
#define DBL_MIN 0x1.0p-1022
#define DBL_EPSILON 0x1.0p-52
#define HUGE_VAL 0x1.0p+1024
#define MAXFLOAT __FLT_MAX__
#define HUGE_VALF as_float(0x7f800000)
#define INFINITY HUGE_VALF
#define NAN as_float(0x7fffffff)
#define as_char(X) __builtin_astype(X,char)
#define as_char2(X) __builtin_astype(X,char2)
#if __OPENCL_C_VERSION__ >= 110
#define as_char3(X) __builtin_astype(X,char3)
#endif
#define as_char4(X) __builtin_astype(X,char4)
#define as_char8(X) __builtin_astype(X,char8)
#define as_char16(X) __builtin_astype(X,char16)
#define as_uchar(X) __builtin_astype(X,uchar)
#define as_uchar2(X) __builtin_astype(X,uchar2)
#if __OPENCL_C_VERSION__ >= 110
#define as_uchar3(X) __builtin_astype(X,uchar3)
#endif
#define as_uchar4(X) __builtin_astype(X,uchar4)
#define as_uchar8(X) __builtin_astype(X,uchar8)
#define as_uchar16(X) __builtin_astype(X,uchar16)
#define as_short(X) __builtin_astype(X,short)
#define as_short2(X) __builtin_astype(X,short2)
#if __OPENCL_C_VERSION__ >= 110
#define as_short3(X) __builtin_astype(X,short3)
#endif
#define as_short4(X) __builtin_astype(X,short4)
#define as_short8(X) __builtin_astype(X,short8)
#define as_short16(X) __builtin_astype(X,short16)
#define as_ushort(X) __builtin_astype(X,ushort)
#define as_ushort2(X) __builtin_astype(X,ushort2)
#if __OPENCL_C_VERSION__ >= 110
#define as_ushort3(X) __builtin_astype(X,ushort3)
#endif
#define as_ushort4(X) __builtin_astype(X,ushort4)
#define as_ushort8(X) __builtin_astype(X,ushort8)
#define as_ushort16(X) __builtin_astype(X,ushort16)
#define as_int(X) __builtin_astype(X,int)
#define as_int2(X) __builtin_astype(X,int2)
#if __OPENCL_C_VERSION__ >= 110
#define as_int3(X) __builtin_astype(X,int3)
#endif
#define as_int4(X) __builtin_astype(X,int4)
#define as_int8(X) __builtin_astype(X,int8)
#define as_int16(X) __builtin_astype(X,int16)
#define as_uint(X) __builtin_astype(X,uint)
#define as_uint2(X) __builtin_astype(X,uint2)
#if __OPENCL_C_VERSION__ >= 110
#define as_uint3(X) __builtin_astype(X,uint3)
#endif
#define as_uint4(X) __builtin_astype(X,uint4)
#define as_uint8(X) __builtin_astype(X,uint8)
#define as_uint16(X) __builtin_astype(X,uint16)
#define as_long(X) __builtin_astype(X,long)
#define as_long2(X) __builtin_astype(X,long2)
#if __OPENCL_C_VERSION__ >= 110
#define as_long3(X) __builtin_astype(X,long3)
#endif
#define as_long4(X) __builtin_astype(X,long4)
#define as_long8(X) __builtin_astype(X,long8)
#define as_long16(X) __builtin_astype(X,long16)
#define as_ulong(X) __builtin_astype(X,ulong)
#define as_ulong2(X) __builtin_astype(X,ulong2)
#if __OPENCL_C_VERSION__ >= 110
#define as_ulong3(X) __builtin_astype(X,ulong3)
#endif
#define as_ulong4(X) __builtin_astype(X,ulong4)
#define as_ulong8(X) __builtin_astype(X,ulong8)
#define as_ulong16(X) __builtin_astype(X,ulong16)
#define as_float(X) __builtin_astype(X,float)
#define as_float2(X) __builtin_astype(X,float2)
#if __OPENCL_C_VERSION__ >= 110
#define as_float3(X) __builtin_astype(X,float3)
#endif
#define as_float4(X) __builtin_astype(X,float4)
#define as_float8(X) __builtin_astype(X,float8)
#define as_float16(X) __builtin_astype(X,float16)
#define as_double(X) __builtin_astype(X,double)
#define as_double2(X) __builtin_astype(X,double2)
#if __OPENCL_C_VERSION__ >= 110
#define as_double3(X) __builtin_astype(X,double3)
#endif
#define as_double4(X) __builtin_astype(X,double4)
#define as_double8(X) __builtin_astype(X,double8)
#define as_double16(X) __builtin_astype(X,double16)

float __attribute__((__always_inline__)) __acl__flush_denorm(float x);
float __attribute__((__always_inline__)) __acl__flush_denorm_signed_zero(float x);
float __attribute__((__overloadable__)) ldexp(float __x, int __y);
float2 __attribute__((__overloadable__)) ldexp(float2 __x, int2 __y);
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) ldexp(float3 __x, int3 __y);
#endif
float4 __attribute__((__overloadable__)) ldexp(float4 __x, int4 __y);
float8 __attribute__((__overloadable__)) ldexp(float8 __x, int8 __y);
float16 __attribute__((__overloadable__)) ldexp(float16 __x, int16 __y);
float __attribute__((__overloadable__)) ldexpf(float __x, int __y);
float2 __attribute__((__overloadable__)) ldexpf(float2 __x, int2 __y);
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) ldexpf(float3 __x, int3 __y);
#endif
float4 __attribute__((__overloadable__)) ldexpf(float4 __x, int4 __y);
float8 __attribute__((__overloadable__)) ldexpf(float8 __x, int8 __y);
float16 __attribute__((__overloadable__)) ldexpf(float16 __x, int16 __y);
float __attribute__((__overloadable__)) sqrtf(float __x);
float2 __attribute__((__overloadable__)) sqrtf(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) sqrtf(float3 __x );
#endif
float4 __attribute__((__overloadable__)) sqrtf(float4 __x );
float8 __attribute__((__overloadable__)) sqrtf(float8 __x );
float16 __attribute__((__overloadable__)) sqrtf(float16 __x );
float __attribute__((__overloadable__)) floorf(float __x);
float2 __attribute__((__overloadable__)) floorf(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) floorf(float3 __x );
#endif
float4 __attribute__((__overloadable__)) floorf(float4 __x );
float8 __attribute__((__overloadable__)) floorf(float8 __x );
float16 __attribute__((__overloadable__)) floorf(float16 __x );
float __attribute__((__overloadable__)) ceilf(float __x);
float2 __attribute__((__overloadable__)) ceilf(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) ceilf(float3 __x );
#endif
float4 __attribute__((__overloadable__)) ceilf(float4 __x );
float8 __attribute__((__overloadable__)) ceilf(float8 __x );
float16 __attribute__((__overloadable__)) ceilf(float16 __x );
float __attribute__((__overloadable__)) sinf(float __x);
float2 __attribute__((__overloadable__)) sinf(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) sinf(float3 __x );
#endif
float4 __attribute__((__overloadable__)) sinf(float4 __x );
float8 __attribute__((__overloadable__)) sinf(float8 __x );
float16 __attribute__((__overloadable__)) sinf(float16 __x );
float __attribute__((__overloadable__)) cosf(float __x);
float2 __attribute__((__overloadable__)) cosf(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) cosf(float3 __x );
#endif
float4 __attribute__((__overloadable__)) cosf(float4 __x );
float8 __attribute__((__overloadable__)) cosf(float8 __x );
float16 __attribute__((__overloadable__)) cosf(float16 __x );
float __attribute__((__overloadable__)) logf(float __x);
float2 __attribute__((__overloadable__)) logf(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) logf(float3 __x );
#endif
float4 __attribute__((__overloadable__)) logf(float4 __x );
float8 __attribute__((__overloadable__)) logf(float8 __x );
float16 __attribute__((__overloadable__)) logf(float16 __x );
float __attribute__((__overloadable__)) expf(float __x);
float2 __attribute__((__overloadable__)) expf(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) expf(float3 __x );
#endif
float4 __attribute__((__overloadable__)) expf(float4 __x );
float8 __attribute__((__overloadable__)) expf(float8 __x );
float16 __attribute__((__overloadable__)) expf(float16 __x );
float __attribute__((__overloadable__)) fabsf(float __x);
float2 __attribute__((__overloadable__)) fabsf(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) fabsf(float3 __x );
#endif
float4 __attribute__((__overloadable__)) fabsf(float4 __x );
float8 __attribute__((__overloadable__)) fabsf(float8 __x );
float16 __attribute__((__overloadable__)) fabsf(float16 __x );
float __attribute__((__overloadable__)) asinf(float __x);
float2 __attribute__((__overloadable__)) asinf(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) asinf(float3 __x );
#endif
float4 __attribute__((__overloadable__)) asinf(float4 __x );
float8 __attribute__((__overloadable__)) asinf(float8 __x );
float16 __attribute__((__overloadable__)) asinf(float16 __x );
float __attribute__((__overloadable__)) acosf(float __x);
float2 __attribute__((__overloadable__)) acosf(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) acosf(float3 __x );
#endif
float4 __attribute__((__overloadable__)) acosf(float4 __x );
float8 __attribute__((__overloadable__)) acosf(float8 __x );
float16 __attribute__((__overloadable__)) acosf(float16 __x );
float __attribute__((__overloadable__)) atanf(float __x);
float2 __attribute__((__overloadable__)) atanf(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) atanf(float3 __x );
#endif
float4 __attribute__((__overloadable__)) atanf(float4 __x );
float8 __attribute__((__overloadable__)) atanf(float8 __x );
float16 __attribute__((__overloadable__)) atanf(float16 __x );
float __attribute__((__overloadable__)) fabs(float __x);
float2 __attribute__((__overloadable__)) fabs(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) fabs(float3 __x );
#endif
float4 __attribute__((__overloadable__)) fabs(float4 __x );
float8 __attribute__((__overloadable__)) fabs(float8 __x );
float16 __attribute__((__overloadable__)) fabs(float16 __x );
double __attribute__((__overloadable__)) fabs(double __x);
double2 __attribute__((__overloadable__)) fabs(double2 __x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) fabs(double3 __x );
#endif
double4 __attribute__((__overloadable__)) fabs(double4 __x );
double8 __attribute__((__overloadable__)) fabs(double8 __x );
double16 __attribute__((__overloadable__)) fabs(double16 __x );
float __attribute__((__overloadable__)) floor(float __x);
float2 __attribute__((__overloadable__)) floor(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) floor(float3 __x );
#endif
float4 __attribute__((__overloadable__)) floor(float4 __x );
float8 __attribute__((__overloadable__)) floor(float8 __x );
float16 __attribute__((__overloadable__)) floor(float16 __x );
float __attribute__((__overloadable__)) ceil(float __x);
float2 __attribute__((__overloadable__)) ceil(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) ceil(float3 __x );
#endif
float4 __attribute__((__overloadable__)) ceil(float4 __x );
float8 __attribute__((__overloadable__)) ceil(float8 __x );
float16 __attribute__((__overloadable__)) ceil(float16 __x );
float __attribute__((__overloadable__)) trunc(float __x);
float2 __attribute__((__overloadable__)) trunc(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) trunc(float3 __x );
#endif
float4 __attribute__((__overloadable__)) trunc(float4 __x );
float8 __attribute__((__overloadable__)) trunc(float8 __x );
float16 __attribute__((__overloadable__)) trunc(float16 __x );
double __attribute__((__overloadable__)) floor(double __x);
double2 __attribute__((__overloadable__)) floor(double2 __x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) floor(double3 __x );
#endif
double4 __attribute__((__overloadable__)) floor(double4 __x );
double8 __attribute__((__overloadable__)) floor(double8 __x );
double16 __attribute__((__overloadable__)) floor(double16 __x );
double __attribute__((__overloadable__)) ceil(double __x);
double2 __attribute__((__overloadable__)) ceil(double2 __x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) ceil(double3 __x );
#endif
double4 __attribute__((__overloadable__)) ceil(double4 __x );
double8 __attribute__((__overloadable__)) ceil(double8 __x );
double16 __attribute__((__overloadable__)) ceil(double16 __x );
double __attribute__((__overloadable__)) trunc(double __x);
double2 __attribute__((__overloadable__)) trunc(double2 __x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) trunc(double3 __x );
#endif
double4 __attribute__((__overloadable__)) trunc(double4 __x );
double8 __attribute__((__overloadable__)) trunc(double8 __x );
double16 __attribute__((__overloadable__)) trunc(double16 __x );
float __attribute__((__overloadable__)) exp(float __x);
float2 __attribute__((__overloadable__)) exp(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) exp(float3 __x );
#endif
float4 __attribute__((__overloadable__)) exp(float4 __x );
float8 __attribute__((__overloadable__)) exp(float8 __x );
float16 __attribute__((__overloadable__)) exp(float16 __x );
float __attribute__((__overloadable__)) exp10f(float __x);
float2 __attribute__((__overloadable__)) exp10f(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) exp10f(float3 __x );
#endif
float4 __attribute__((__overloadable__)) exp10f(float4 __x );
float8 __attribute__((__overloadable__)) exp10f(float8 __x );
float16 __attribute__((__overloadable__)) exp10f(float16 __x );
float __attribute__((__overloadable__)) exp2f(float __x);
float2 __attribute__((__overloadable__)) exp2f(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) exp2f(float3 __x );
#endif
float4 __attribute__((__overloadable__)) exp2f(float4 __x );
float8 __attribute__((__overloadable__)) exp2f(float8 __x );
float16 __attribute__((__overloadable__)) exp2f(float16 __x );
double __attribute__((__overloadable__)) ldexp(double __x, int __y);
double2 __attribute__((__overloadable__)) ldexp(double2 __x, int2 __y);
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) ldexp(double3 __x, int3 __y);
#endif
double4 __attribute__((__overloadable__)) ldexp(double4 __x, int4 __y);
double8 __attribute__((__overloadable__)) ldexp(double8 __x, int8 __y);
double16 __attribute__((__overloadable__)) ldexp(double16 __x, int16 __y);
double __attribute__((__overloadable__)) exp(double __x);
double2 __attribute__((__overloadable__)) exp(double2 __x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) exp(double3 __x );
#endif
double4 __attribute__((__overloadable__)) exp(double4 __x );
double8 __attribute__((__overloadable__)) exp(double8 __x );
double16 __attribute__((__overloadable__)) exp(double16 __x );
double __attribute__((__overloadable__)) sqrt(double __x);
double2 __attribute__((__overloadable__)) sqrt(double2 __x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) sqrt(double3 __x );
#endif
double4 __attribute__((__overloadable__)) sqrt(double4 __x );
double8 __attribute__((__overloadable__)) sqrt(double8 __x );
double16 __attribute__((__overloadable__)) sqrt(double16 __x );
double __attribute__((__overloadable__)) log(double __x);
double2 __attribute__((__overloadable__)) log(double2 __x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) log(double3 __x );
#endif
double4 __attribute__((__overloadable__)) log(double4 __x );
double8 __attribute__((__overloadable__)) log(double8 __x );
double16 __attribute__((__overloadable__)) log(double16 __x );
double __attribute__((__overloadable__)) log10(double __x);
double2 __attribute__((__overloadable__)) log10(double2 __x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) log10(double3 __x );
#endif
double4 __attribute__((__overloadable__)) log10(double4 __x );
double8 __attribute__((__overloadable__)) log10(double8 __x );
double16 __attribute__((__overloadable__)) log10(double16 __x );
double __attribute__((__overloadable__)) log2(double __x);
double2 __attribute__((__overloadable__)) log2(double2 __x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) log2(double3 __x );
#endif
double4 __attribute__((__overloadable__)) log2(double4 __x );
double8 __attribute__((__overloadable__)) log2(double8 __x );
double16 __attribute__((__overloadable__)) log2(double16 __x );
double __attribute__((__overloadable__)) log1p(double __x);
double2 __attribute__((__overloadable__)) log1p(double2 __x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) log1p(double3 __x );
#endif
double4 __attribute__((__overloadable__)) log1p(double4 __x );
double8 __attribute__((__overloadable__)) log1p(double8 __x );
double16 __attribute__((__overloadable__)) log1p(double16 __x );
double __attribute__((__overloadable__)) exp10(double __x);
double2 __attribute__((__overloadable__)) exp10(double2 __x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) exp10(double3 __x );
#endif
double4 __attribute__((__overloadable__)) exp10(double4 __x );
double8 __attribute__((__overloadable__)) exp10(double8 __x );
double16 __attribute__((__overloadable__)) exp10(double16 __x );
double __attribute__((__overloadable__)) exp2(double __x);
double2 __attribute__((__overloadable__)) exp2(double2 __x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) exp2(double3 __x );
#endif
double4 __attribute__((__overloadable__)) exp2(double4 __x );
double8 __attribute__((__overloadable__)) exp2(double8 __x );
double16 __attribute__((__overloadable__)) exp2(double16 __x );
double __attribute__((__overloadable__)) rsqrt(double __x);
double2 __attribute__((__overloadable__)) rsqrt(double2 __x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) rsqrt(double3 __x );
#endif
double4 __attribute__((__overloadable__)) rsqrt(double4 __x );
double8 __attribute__((__overloadable__)) rsqrt(double8 __x );
double16 __attribute__((__overloadable__)) rsqrt(double16 __x );
 __attribute__((const)) float __attribute__((__overloadable__,__always_inline__,const)) copysign(float x, float y );
float2 __attribute__((__overloadable__,__always_inline__,const)) copysign(float2 x, float2 y );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__,__always_inline__,const)) copysign(float3 x, float3 y );
#endif
float4 __attribute__((__overloadable__,__always_inline__,const)) copysign(float4 x, float4 y );
float8 __attribute__((__overloadable__,__always_inline__,const)) copysign(float8 x, float8 y );
float16 __attribute__((__overloadable__,__always_inline__,const)) copysign(float16 x, float16 y );
 __attribute__((const)) double __attribute__((__overloadable__,__always_inline__,const)) copysign(double x, double y );
double2 __attribute__((__overloadable__,__always_inline__,const)) copysign(double2 x, double2 y );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__,__always_inline__,const)) copysign(double3 x, double3 y );
#endif
double4 __attribute__((__overloadable__,__always_inline__,const)) copysign(double4 x, double4 y );
double8 __attribute__((__overloadable__,__always_inline__,const)) copysign(double8 x, double8 y );
double16 __attribute__((__overloadable__,__always_inline__,const)) copysign(double16 x, double16 y );
#if __OPENCL_C_VERSION__ >= 110
char __attribute__((__overloadable__)) max(char _x, char  _y);
char __attribute__((__overloadable__)) min(char _x, char  _y);
uchar __attribute__((__overloadable__)) max(uchar _x, uchar  _y);
uchar __attribute__((__overloadable__)) min(uchar _x, uchar  _y);
short __attribute__((__overloadable__)) max(short _x, short  _y);
short __attribute__((__overloadable__)) min(short _x, short  _y);
ushort __attribute__((__overloadable__)) max(ushort _x, ushort  _y);
ushort __attribute__((__overloadable__)) min(ushort _x, ushort  _y);
int __attribute__((__overloadable__)) max(int _x, int  _y);
int __attribute__((__overloadable__)) min(int _x, int  _y);
uint __attribute__((__overloadable__)) max(uint _x, uint  _y);
uint __attribute__((__overloadable__)) min(uint _x, uint  _y);
long __attribute__((__overloadable__)) max(long _x, long  _y);
long __attribute__((__overloadable__)) min(long _x, long  _y);
ulong __attribute__((__overloadable__)) max(ulong _x, ulong  _y);
ulong __attribute__((__overloadable__)) min(ulong _x, ulong  _y);
#endif
float __attribute__((__overloadable__)) max(float _x, float  _y);
float __attribute__((__overloadable__)) min(float _x, float  _y);
double __attribute__((__overloadable__)) max(double _x, double  _y);
double __attribute__((__overloadable__)) min(double _x, double  _y);
#if __OPENCL_C_VERSION__ >= 110
char2 __attribute__((__overloadable__)) min( char2 _x, char2 _y);
char2 __attribute__((__overloadable__)) min( char2 _x, char _y);
#if __OPENCL_C_VERSION__ >= 110
char3 __attribute__((__overloadable__)) min( char3 _x, char3 _y);
char3 __attribute__((__overloadable__)) min( char3 _x, char _y);
#endif
char4 __attribute__((__overloadable__)) min( char4 _x, char4 _y);
char4 __attribute__((__overloadable__)) min( char4 _x, char _y);
char8 __attribute__((__overloadable__)) min( char8 _x, char8 _y);
char8 __attribute__((__overloadable__)) min( char8 _x, char _y);
char16 __attribute__((__overloadable__)) min( char16 _x, char16 _y);
char16 __attribute__((__overloadable__)) min( char16 _x, char _y);
char2 __attribute__((__overloadable__)) max( char2 _x, char2 _y);
char2 __attribute__((__overloadable__)) max( char2 _x, char _y);
#if __OPENCL_C_VERSION__ >= 110
char3 __attribute__((__overloadable__)) max( char3 _x, char3 _y);
char3 __attribute__((__overloadable__)) max( char3 _x, char _y);
#endif
char4 __attribute__((__overloadable__)) max( char4 _x, char4 _y);
char4 __attribute__((__overloadable__)) max( char4 _x, char _y);
char8 __attribute__((__overloadable__)) max( char8 _x, char8 _y);
char8 __attribute__((__overloadable__)) max( char8 _x, char _y);
char16 __attribute__((__overloadable__)) max( char16 _x, char16 _y);
char16 __attribute__((__overloadable__)) max( char16 _x, char _y);
uchar2 __attribute__((__overloadable__)) min( uchar2 _x, uchar2 _y);
uchar2 __attribute__((__overloadable__)) min( uchar2 _x, uchar _y);
#if __OPENCL_C_VERSION__ >= 110
uchar3 __attribute__((__overloadable__)) min( uchar3 _x, uchar3 _y);
uchar3 __attribute__((__overloadable__)) min( uchar3 _x, uchar _y);
#endif
uchar4 __attribute__((__overloadable__)) min( uchar4 _x, uchar4 _y);
uchar4 __attribute__((__overloadable__)) min( uchar4 _x, uchar _y);
uchar8 __attribute__((__overloadable__)) min( uchar8 _x, uchar8 _y);
uchar8 __attribute__((__overloadable__)) min( uchar8 _x, uchar _y);
uchar16 __attribute__((__overloadable__)) min( uchar16 _x, uchar16 _y);
uchar16 __attribute__((__overloadable__)) min( uchar16 _x, uchar _y);
uchar2 __attribute__((__overloadable__)) max( uchar2 _x, uchar2 _y);
uchar2 __attribute__((__overloadable__)) max( uchar2 _x, uchar _y);
#if __OPENCL_C_VERSION__ >= 110
uchar3 __attribute__((__overloadable__)) max( uchar3 _x, uchar3 _y);
uchar3 __attribute__((__overloadable__)) max( uchar3 _x, uchar _y);
#endif
uchar4 __attribute__((__overloadable__)) max( uchar4 _x, uchar4 _y);
uchar4 __attribute__((__overloadable__)) max( uchar4 _x, uchar _y);
uchar8 __attribute__((__overloadable__)) max( uchar8 _x, uchar8 _y);
uchar8 __attribute__((__overloadable__)) max( uchar8 _x, uchar _y);
uchar16 __attribute__((__overloadable__)) max( uchar16 _x, uchar16 _y);
uchar16 __attribute__((__overloadable__)) max( uchar16 _x, uchar _y);
short2 __attribute__((__overloadable__)) min( short2 _x, short2 _y);
short2 __attribute__((__overloadable__)) min( short2 _x, short _y);
#if __OPENCL_C_VERSION__ >= 110
short3 __attribute__((__overloadable__)) min( short3 _x, short3 _y);
short3 __attribute__((__overloadable__)) min( short3 _x, short _y);
#endif
short4 __attribute__((__overloadable__)) min( short4 _x, short4 _y);
short4 __attribute__((__overloadable__)) min( short4 _x, short _y);
short8 __attribute__((__overloadable__)) min( short8 _x, short8 _y);
short8 __attribute__((__overloadable__)) min( short8 _x, short _y);
short16 __attribute__((__overloadable__)) min( short16 _x, short16 _y);
short16 __attribute__((__overloadable__)) min( short16 _x, short _y);
short2 __attribute__((__overloadable__)) max( short2 _x, short2 _y);
short2 __attribute__((__overloadable__)) max( short2 _x, short _y);
#if __OPENCL_C_VERSION__ >= 110
short3 __attribute__((__overloadable__)) max( short3 _x, short3 _y);
short3 __attribute__((__overloadable__)) max( short3 _x, short _y);
#endif
short4 __attribute__((__overloadable__)) max( short4 _x, short4 _y);
short4 __attribute__((__overloadable__)) max( short4 _x, short _y);
short8 __attribute__((__overloadable__)) max( short8 _x, short8 _y);
short8 __attribute__((__overloadable__)) max( short8 _x, short _y);
short16 __attribute__((__overloadable__)) max( short16 _x, short16 _y);
short16 __attribute__((__overloadable__)) max( short16 _x, short _y);
ushort2 __attribute__((__overloadable__)) min( ushort2 _x, ushort2 _y);
ushort2 __attribute__((__overloadable__)) min( ushort2 _x, ushort _y);
#if __OPENCL_C_VERSION__ >= 110
ushort3 __attribute__((__overloadable__)) min( ushort3 _x, ushort3 _y);
ushort3 __attribute__((__overloadable__)) min( ushort3 _x, ushort _y);
#endif
ushort4 __attribute__((__overloadable__)) min( ushort4 _x, ushort4 _y);
ushort4 __attribute__((__overloadable__)) min( ushort4 _x, ushort _y);
ushort8 __attribute__((__overloadable__)) min( ushort8 _x, ushort8 _y);
ushort8 __attribute__((__overloadable__)) min( ushort8 _x, ushort _y);
ushort16 __attribute__((__overloadable__)) min( ushort16 _x, ushort16 _y);
ushort16 __attribute__((__overloadable__)) min( ushort16 _x, ushort _y);
ushort2 __attribute__((__overloadable__)) max( ushort2 _x, ushort2 _y);
ushort2 __attribute__((__overloadable__)) max( ushort2 _x, ushort _y);
#if __OPENCL_C_VERSION__ >= 110
ushort3 __attribute__((__overloadable__)) max( ushort3 _x, ushort3 _y);
ushort3 __attribute__((__overloadable__)) max( ushort3 _x, ushort _y);
#endif
ushort4 __attribute__((__overloadable__)) max( ushort4 _x, ushort4 _y);
ushort4 __attribute__((__overloadable__)) max( ushort4 _x, ushort _y);
ushort8 __attribute__((__overloadable__)) max( ushort8 _x, ushort8 _y);
ushort8 __attribute__((__overloadable__)) max( ushort8 _x, ushort _y);
ushort16 __attribute__((__overloadable__)) max( ushort16 _x, ushort16 _y);
ushort16 __attribute__((__overloadable__)) max( ushort16 _x, ushort _y);
int2 __attribute__((__overloadable__)) min( int2 _x, int2 _y);
int2 __attribute__((__overloadable__)) min( int2 _x, int _y);
#if __OPENCL_C_VERSION__ >= 110
int3 __attribute__((__overloadable__)) min( int3 _x, int3 _y);
int3 __attribute__((__overloadable__)) min( int3 _x, int _y);
#endif
int4 __attribute__((__overloadable__)) min( int4 _x, int4 _y);
int4 __attribute__((__overloadable__)) min( int4 _x, int _y);
int8 __attribute__((__overloadable__)) min( int8 _x, int8 _y);
int8 __attribute__((__overloadable__)) min( int8 _x, int _y);
int16 __attribute__((__overloadable__)) min( int16 _x, int16 _y);
int16 __attribute__((__overloadable__)) min( int16 _x, int _y);
int2 __attribute__((__overloadable__)) max( int2 _x, int2 _y);
int2 __attribute__((__overloadable__)) max( int2 _x, int _y);
#if __OPENCL_C_VERSION__ >= 110
int3 __attribute__((__overloadable__)) max( int3 _x, int3 _y);
int3 __attribute__((__overloadable__)) max( int3 _x, int _y);
#endif
int4 __attribute__((__overloadable__)) max( int4 _x, int4 _y);
int4 __attribute__((__overloadable__)) max( int4 _x, int _y);
int8 __attribute__((__overloadable__)) max( int8 _x, int8 _y);
int8 __attribute__((__overloadable__)) max( int8 _x, int _y);
int16 __attribute__((__overloadable__)) max( int16 _x, int16 _y);
int16 __attribute__((__overloadable__)) max( int16 _x, int _y);
uint2 __attribute__((__overloadable__)) min( uint2 _x, uint2 _y);
uint2 __attribute__((__overloadable__)) min( uint2 _x, uint _y);
#if __OPENCL_C_VERSION__ >= 110
uint3 __attribute__((__overloadable__)) min( uint3 _x, uint3 _y);
uint3 __attribute__((__overloadable__)) min( uint3 _x, uint _y);
#endif
uint4 __attribute__((__overloadable__)) min( uint4 _x, uint4 _y);
uint4 __attribute__((__overloadable__)) min( uint4 _x, uint _y);
uint8 __attribute__((__overloadable__)) min( uint8 _x, uint8 _y);
uint8 __attribute__((__overloadable__)) min( uint8 _x, uint _y);
uint16 __attribute__((__overloadable__)) min( uint16 _x, uint16 _y);
uint16 __attribute__((__overloadable__)) min( uint16 _x, uint _y);
uint2 __attribute__((__overloadable__)) max( uint2 _x, uint2 _y);
uint2 __attribute__((__overloadable__)) max( uint2 _x, uint _y);
#if __OPENCL_C_VERSION__ >= 110
uint3 __attribute__((__overloadable__)) max( uint3 _x, uint3 _y);
uint3 __attribute__((__overloadable__)) max( uint3 _x, uint _y);
#endif
uint4 __attribute__((__overloadable__)) max( uint4 _x, uint4 _y);
uint4 __attribute__((__overloadable__)) max( uint4 _x, uint _y);
uint8 __attribute__((__overloadable__)) max( uint8 _x, uint8 _y);
uint8 __attribute__((__overloadable__)) max( uint8 _x, uint _y);
uint16 __attribute__((__overloadable__)) max( uint16 _x, uint16 _y);
uint16 __attribute__((__overloadable__)) max( uint16 _x, uint _y);
long2 __attribute__((__overloadable__)) min( long2 _x, long2 _y);
long2 __attribute__((__overloadable__)) min( long2 _x, long _y);
#if __OPENCL_C_VERSION__ >= 110
long3 __attribute__((__overloadable__)) min( long3 _x, long3 _y);
long3 __attribute__((__overloadable__)) min( long3 _x, long _y);
#endif
long4 __attribute__((__overloadable__)) min( long4 _x, long4 _y);
long4 __attribute__((__overloadable__)) min( long4 _x, long _y);
long8 __attribute__((__overloadable__)) min( long8 _x, long8 _y);
long8 __attribute__((__overloadable__)) min( long8 _x, long _y);
long16 __attribute__((__overloadable__)) min( long16 _x, long16 _y);
long16 __attribute__((__overloadable__)) min( long16 _x, long _y);
long2 __attribute__((__overloadable__)) max( long2 _x, long2 _y);
long2 __attribute__((__overloadable__)) max( long2 _x, long _y);
#if __OPENCL_C_VERSION__ >= 110
long3 __attribute__((__overloadable__)) max( long3 _x, long3 _y);
long3 __attribute__((__overloadable__)) max( long3 _x, long _y);
#endif
long4 __attribute__((__overloadable__)) max( long4 _x, long4 _y);
long4 __attribute__((__overloadable__)) max( long4 _x, long _y);
long8 __attribute__((__overloadable__)) max( long8 _x, long8 _y);
long8 __attribute__((__overloadable__)) max( long8 _x, long _y);
long16 __attribute__((__overloadable__)) max( long16 _x, long16 _y);
long16 __attribute__((__overloadable__)) max( long16 _x, long _y);
ulong2 __attribute__((__overloadable__)) min( ulong2 _x, ulong2 _y);
ulong2 __attribute__((__overloadable__)) min( ulong2 _x, ulong _y);
#if __OPENCL_C_VERSION__ >= 110
ulong3 __attribute__((__overloadable__)) min( ulong3 _x, ulong3 _y);
ulong3 __attribute__((__overloadable__)) min( ulong3 _x, ulong _y);
#endif
ulong4 __attribute__((__overloadable__)) min( ulong4 _x, ulong4 _y);
ulong4 __attribute__((__overloadable__)) min( ulong4 _x, ulong _y);
ulong8 __attribute__((__overloadable__)) min( ulong8 _x, ulong8 _y);
ulong8 __attribute__((__overloadable__)) min( ulong8 _x, ulong _y);
ulong16 __attribute__((__overloadable__)) min( ulong16 _x, ulong16 _y);
ulong16 __attribute__((__overloadable__)) min( ulong16 _x, ulong _y);
ulong2 __attribute__((__overloadable__)) max( ulong2 _x, ulong2 _y);
ulong2 __attribute__((__overloadable__)) max( ulong2 _x, ulong _y);
#if __OPENCL_C_VERSION__ >= 110
ulong3 __attribute__((__overloadable__)) max( ulong3 _x, ulong3 _y);
ulong3 __attribute__((__overloadable__)) max( ulong3 _x, ulong _y);
#endif
ulong4 __attribute__((__overloadable__)) max( ulong4 _x, ulong4 _y);
ulong4 __attribute__((__overloadable__)) max( ulong4 _x, ulong _y);
ulong8 __attribute__((__overloadable__)) max( ulong8 _x, ulong8 _y);
ulong8 __attribute__((__overloadable__)) max( ulong8 _x, ulong _y);
ulong16 __attribute__((__overloadable__)) max( ulong16 _x, ulong16 _y);
ulong16 __attribute__((__overloadable__)) max( ulong16 _x, ulong _y);
#endif
float2 __attribute__((__overloadable__)) min( float2 _x, float2 _y);
float2 __attribute__((__overloadable__)) min( float2 _x, float _y);
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) min( float3 _x, float3 _y);
float3 __attribute__((__overloadable__)) min( float3 _x, float _y);
#endif
float4 __attribute__((__overloadable__)) min( float4 _x, float4 _y);
float4 __attribute__((__overloadable__)) min( float4 _x, float _y);
float8 __attribute__((__overloadable__)) min( float8 _x, float8 _y);
float8 __attribute__((__overloadable__)) min( float8 _x, float _y);
float16 __attribute__((__overloadable__)) min( float16 _x, float16 _y);
float16 __attribute__((__overloadable__)) min( float16 _x, float _y);
float2 __attribute__((__overloadable__)) max( float2 _x, float2 _y);
float2 __attribute__((__overloadable__)) max( float2 _x, float _y);
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) max( float3 _x, float3 _y);
float3 __attribute__((__overloadable__)) max( float3 _x, float _y);
#endif
float4 __attribute__((__overloadable__)) max( float4 _x, float4 _y);
float4 __attribute__((__overloadable__)) max( float4 _x, float _y);
float8 __attribute__((__overloadable__)) max( float8 _x, float8 _y);
float8 __attribute__((__overloadable__)) max( float8 _x, float _y);
float16 __attribute__((__overloadable__)) max( float16 _x, float16 _y);
float16 __attribute__((__overloadable__)) max( float16 _x, float _y);
double2 __attribute__((__overloadable__)) min( double2 _x, double2 _y);
double2 __attribute__((__overloadable__)) min( double2 _x, double _y);
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) min( double3 _x, double3 _y);
double3 __attribute__((__overloadable__)) min( double3 _x, double _y);
#endif
double4 __attribute__((__overloadable__)) min( double4 _x, double4 _y);
double4 __attribute__((__overloadable__)) min( double4 _x, double _y);
double8 __attribute__((__overloadable__)) min( double8 _x, double8 _y);
double8 __attribute__((__overloadable__)) min( double8 _x, double _y);
double16 __attribute__((__overloadable__)) min( double16 _x, double16 _y);
double16 __attribute__((__overloadable__)) min( double16 _x, double _y);
double2 __attribute__((__overloadable__)) max( double2 _x, double2 _y);
double2 __attribute__((__overloadable__)) max( double2 _x, double _y);
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) max( double3 _x, double3 _y);
double3 __attribute__((__overloadable__)) max( double3 _x, double _y);
#endif
double4 __attribute__((__overloadable__)) max( double4 _x, double4 _y);
double4 __attribute__((__overloadable__)) max( double4 _x, double _y);
double8 __attribute__((__overloadable__)) max( double8 _x, double8 _y);
double8 __attribute__((__overloadable__)) max( double8 _x, double _y);
double16 __attribute__((__overloadable__)) max( double16 _x, double16 _y);
double16 __attribute__((__overloadable__)) max( double16 _x, double _y);
int  __attribute__((__overloadable__,__always_inline__,const)) isnan(float x);
int2 __attribute__((__overloadable__,__always_inline__,const)) isnan(float2 x);
#if __OPENCL_C_VERSION__ >= 110
int3 __attribute__((__overloadable__,__always_inline__,const)) isnan(float3 x);
#endif
int4 __attribute__((__overloadable__,__always_inline__,const)) isnan(float4 x);
int8 __attribute__((__overloadable__,__always_inline__,const)) isnan(float8 x);
int16 __attribute__((__overloadable__,__always_inline__,const)) isnan(float16 x);
int  __attribute__((__overloadable__,__always_inline__,const)) isnan(double x);
long2 __attribute__((__overloadable__,__always_inline__,const)) isnan(double2 x);
#if __OPENCL_C_VERSION__ >= 110
long3 __attribute__((__overloadable__,__always_inline__,const)) isnan(double3 x);
#endif
long4 __attribute__((__overloadable__,__always_inline__,const)) isnan(double4 x);
long8 __attribute__((__overloadable__,__always_inline__,const)) isnan(double8 x);
long16 __attribute__((__overloadable__,__always_inline__,const)) isnan(double16 x);
int  __attribute__((__overloadable__,__always_inline__,const)) isinf(float x);
int2 __attribute__((__overloadable__,__always_inline__,const)) isinf(float2 x);
#if __OPENCL_C_VERSION__ >= 110
int3 __attribute__((__overloadable__,__always_inline__,const)) isinf(float3 x);
#endif
int4 __attribute__((__overloadable__,__always_inline__,const)) isinf(float4 x);
int8 __attribute__((__overloadable__,__always_inline__,const)) isinf(float8 x);
int16 __attribute__((__overloadable__,__always_inline__,const)) isinf(float16 x);
int  __attribute__((__overloadable__,__always_inline__,const)) isinf(double x);
long2 __attribute__((__overloadable__,__always_inline__,const)) isinf(double2 x);
#if __OPENCL_C_VERSION__ >= 110
long3 __attribute__((__overloadable__,__always_inline__,const)) isinf(double3 x);
#endif
long4 __attribute__((__overloadable__,__always_inline__,const)) isinf(double4 x);
long8 __attribute__((__overloadable__,__always_inline__,const)) isinf(double8 x);
long16 __attribute__((__overloadable__,__always_inline__,const)) isinf(double16 x);
int  __attribute__((__overloadable__,__always_inline__,const)) isfinite(float x);
int2 __attribute__((__overloadable__,__always_inline__,const)) isfinite(float2 x);
#if __OPENCL_C_VERSION__ >= 110
int3 __attribute__((__overloadable__,__always_inline__,const)) isfinite(float3 x);
#endif
int4 __attribute__((__overloadable__,__always_inline__,const)) isfinite(float4 x);
int8 __attribute__((__overloadable__,__always_inline__,const)) isfinite(float8 x);
int16 __attribute__((__overloadable__,__always_inline__,const)) isfinite(float16 x);
int  __attribute__((__overloadable__,__always_inline__,const)) isfinite(double x);
long2 __attribute__((__overloadable__,__always_inline__,const)) isfinite(double2 x);
#if __OPENCL_C_VERSION__ >= 110
long3 __attribute__((__overloadable__,__always_inline__,const)) isfinite(double3 x);
#endif
long4 __attribute__((__overloadable__,__always_inline__,const)) isfinite(double4 x);
long8 __attribute__((__overloadable__,__always_inline__,const)) isfinite(double8 x);
long16 __attribute__((__overloadable__,__always_inline__,const)) isfinite(double16 x);
float  __attribute__((__overloadable__,__always_inline__,const)) logb(float x );
float2 __attribute__((__overloadable__,__always_inline__,const)) logb(float2 x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__,__always_inline__,const)) logb(float3 x );
#endif
float4 __attribute__((__overloadable__,__always_inline__,const)) logb(float4 x );
float8 __attribute__((__overloadable__,__always_inline__,const)) logb(float8 x );
float16 __attribute__((__overloadable__,__always_inline__,const)) logb(float16 x );
double  __attribute__((__overloadable__,__always_inline__,const)) logb(double x );
double2 __attribute__((__overloadable__,__always_inline__,const)) logb(double2 x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__,__always_inline__,const)) logb(double3 x );
#endif
double4 __attribute__((__overloadable__,__always_inline__,const)) logb(double4 x );
double8 __attribute__((__overloadable__,__always_inline__,const)) logb(double8 x );
double16 __attribute__((__overloadable__,__always_inline__,const)) logb(double16 x );
int  __attribute__((__overloadable__,__always_inline__,const)) ilogb(float x);
int2 __attribute__((__overloadable__,__always_inline__,const)) ilogb(float2 x );
#if __OPENCL_C_VERSION__ >= 110
int3 __attribute__((__overloadable__,__always_inline__,const)) ilogb(float3 x );
#endif
int4 __attribute__((__overloadable__,__always_inline__,const)) ilogb(float4 x );
int8 __attribute__((__overloadable__,__always_inline__,const)) ilogb(float8 x );
int16 __attribute__((__overloadable__,__always_inline__,const)) ilogb(float16 x );
int  __attribute__((__overloadable__,__always_inline__,const)) ilogb(double x);
int2 __attribute__((__overloadable__,__always_inline__,const)) ilogb(double2 x );
#if __OPENCL_C_VERSION__ >= 110
int3 __attribute__((__overloadable__,__always_inline__,const)) ilogb(double3 x );
#endif
int4 __attribute__((__overloadable__,__always_inline__,const)) ilogb(double4 x );
int8 __attribute__((__overloadable__,__always_inline__,const)) ilogb(double8 x );
int16 __attribute__((__overloadable__,__always_inline__,const)) ilogb(double16 x );
float __attribute__((__overloadable__,__always_inline__,const)) nan(uint x);
float2 __attribute__((__overloadable__,__always_inline__,const)) nan(uint2 x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__,__always_inline__,const)) nan(uint3 x );
#endif
float4 __attribute__((__overloadable__,__always_inline__,const)) nan(uint4 x );
float8 __attribute__((__overloadable__,__always_inline__,const)) nan(uint8 x );
float16 __attribute__((__overloadable__,__always_inline__,const)) nan(uint16 x );
double __attribute__((__overloadable__,__always_inline__,const)) nan(ulong x);
double2 __attribute__((__overloadable__,__always_inline__,const)) nan(ulong2 x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__,__always_inline__,const)) nan(ulong3 x );
#endif
double4 __attribute__((__overloadable__,__always_inline__,const)) nan(ulong4 x );
double8 __attribute__((__overloadable__,__always_inline__,const)) nan(ulong8 x );
double16 __attribute__((__overloadable__,__always_inline__,const)) nan(ulong16 x );
float __attribute__((const))  __acl__rintf(float x);
float __attribute__((__overloadable__,__always_inline__,const)) rint(float x);
float2 __attribute__((__overloadable__,__always_inline__,const)) rint(float2 x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__,__always_inline__,const)) rint(float3 x );
#endif
float4 __attribute__((__overloadable__,__always_inline__,const)) rint(float4 x );
float8 __attribute__((__overloadable__,__always_inline__,const)) rint(float8 x );
float16 __attribute__((__overloadable__,__always_inline__,const)) rint(float16 x );
double __attribute__((const))  __acl__rintfd(double x);
double __attribute__((__overloadable__,__always_inline__,const)) rint(double x);
double2 __attribute__((__overloadable__,__always_inline__,const)) rint(double2 x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__,__always_inline__,const)) rint(double3 x );
#endif
double4 __attribute__((__overloadable__,__always_inline__,const)) rint(double4 x );
double8 __attribute__((__overloadable__,__always_inline__,const)) rint(double8 x );
double16 __attribute__((__overloadable__,__always_inline__,const)) rint(double16 x );
float __attribute__((__overloadable__)) powf(float __x, float __y);
float2 __attribute__((__overloadable__)) powf(float2 __x, float2 __y);
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) powf(float3 __x, float3 __y);
#endif
float4 __attribute__((__overloadable__)) powf(float4 __x, float4 __y);
float8  __attribute__((__overloadable__)) powf(float8 __x, float8 __y);
float16  __attribute__((__overloadable__)) powf(float16 __x, float16 __y);
float __attribute__((__overloadable__)) powrf(float __x, float __y);
float2 __attribute__((__overloadable__)) powrf(float2 __x, float2 __y);
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) powrf(float3 __x, float3 __y);
#endif
float4 __attribute__((__overloadable__)) powrf(float4 __x, float4 __y);
float8  __attribute__((__overloadable__)) powrf(float8 __x, float8 __y);
float16  __attribute__((__overloadable__)) powrf(float16 __x, float16 __y);
float __attribute__((__overloadable__)) pown(float __x, int __y);
float2 __attribute__((__overloadable__)) pown(float2 __x, int2 __y);
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) pown(float3 __x, int3 __y);
#endif
float4 __attribute__((__overloadable__)) pown(float4 __x, int4 __y);
float8  __attribute__((__overloadable__)) pown(float8 __x, int8 __y);
float16  __attribute__((__overloadable__)) pown(float16 __x, int16 __y);
float __attribute__((__overloadable__)) rootn(float __x, int __y);
float2 __attribute__((__overloadable__)) rootn(float2 __x, int2 __y);
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) rootn(float3 __x, int3 __y);
#endif
float4 __attribute__((__overloadable__)) rootn(float4 __x, int4 __y);
float8  __attribute__((__overloadable__)) rootn(float8 __x, int8 __y);
float16  __attribute__((__overloadable__)) rootn(float16 __x, int16 __y);
float __attribute__((__overloadable__)) tanf(float __x);
float2 __attribute__((__overloadable__)) tanf(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) tanf(float3 __x );
#endif
float4 __attribute__((__overloadable__)) tanf(float4 __x );
float8 __attribute__((__overloadable__)) tanf(float8 __x );
float16 __attribute__((__overloadable__)) tanf(float16 __x );
float __attribute__((__overloadable__)) log10f(float __x);
float2 __attribute__((__overloadable__)) log10f(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) log10f(float3 __x );
#endif
float4 __attribute__((__overloadable__)) log10f(float4 __x );
float8 __attribute__((__overloadable__)) log10f(float8 __x );
float16 __attribute__((__overloadable__)) log10f(float16 __x );
float __attribute__((__overloadable__)) log2f(float __x);
float2 __attribute__((__overloadable__)) log2f(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) log2f(float3 __x );
#endif
float4 __attribute__((__overloadable__)) log2f(float4 __x );
float8 __attribute__((__overloadable__)) log2f(float8 __x );
float16 __attribute__((__overloadable__)) log2f(float16 __x );
float __attribute__((__overloadable__)) log1pf(float __x);
float2 __attribute__((__overloadable__)) log1pf(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) log1pf(float3 __x );
#endif
float4 __attribute__((__overloadable__)) log1pf(float4 __x );
float8 __attribute__((__overloadable__)) log1pf(float8 __x );
float16 __attribute__((__overloadable__)) log1pf(float16 __x );
float __attribute__((__overloadable__)) coshf(float __x);
float2 __attribute__((__overloadable__)) coshf(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) coshf(float3 __x );
#endif
float4 __attribute__((__overloadable__)) coshf(float4 __x );
float8 __attribute__((__overloadable__)) coshf(float8 __x );
float16 __attribute__((__overloadable__)) coshf(float16 __x );
float __attribute__((__overloadable__)) sqrt(float __x);
float2 __attribute__((__overloadable__)) sqrt(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) sqrt(float3 __x );
#endif
float4 __attribute__((__overloadable__)) sqrt(float4 __x );
float8 __attribute__((__overloadable__)) sqrt(float8 __x );
float16 __attribute__((__overloadable__)) sqrt(float16 __x );
float __attribute__((__overloadable__)) sinhf(float __x);
float2 __attribute__((__overloadable__)) sinhf(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) sinhf(float3 __x );
#endif
float4 __attribute__((__overloadable__)) sinhf(float4 __x );
float8 __attribute__((__overloadable__)) sinhf(float8 __x );
float16 __attribute__((__overloadable__)) sinhf(float16 __x );
float __attribute__((__overloadable__)) degrees(float __x);
float2 __attribute__((__overloadable__)) degrees(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) degrees(float3 __x );
#endif
float4 __attribute__((__overloadable__)) degrees(float4 __x );
float8 __attribute__((__overloadable__)) degrees(float8 __x );
float16 __attribute__((__overloadable__)) degrees(float16 __x );
float __attribute__((__overloadable__)) radians(float __x);
float2 __attribute__((__overloadable__)) radians(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) radians(float3 __x );
#endif
float4 __attribute__((__overloadable__)) radians(float4 __x );
float8 __attribute__((__overloadable__)) radians(float8 __x );
float16 __attribute__((__overloadable__)) radians(float16 __x );
float __attribute__((__overloadable__)) step(float __x, float __y);
float2 __attribute__((__overloadable__)) step(float2 __x, float2 __y);
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) step(float3 __x, float3 __y);
#endif
float4 __attribute__((__overloadable__)) step(float4 __x, float4 __y);
float8 __attribute__((__overloadable__)) step(float8 __x, float8 __y);
float16 __attribute__((__overloadable__)) step(float16 __x, float16 __y);
float __attribute__((__overloadable__)) log10(float __x);
float2 __attribute__((__overloadable__)) log10(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) log10(float3 __x );
#endif
float4 __attribute__((__overloadable__)) log10(float4 __x );
float8 __attribute__((__overloadable__)) log10(float8 __x );
float16 __attribute__((__overloadable__)) log10(float16 __x );
float __attribute__((__overloadable__)) log2(float __x);
float2 __attribute__((__overloadable__)) log2(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) log2(float3 __x );
#endif
float4 __attribute__((__overloadable__)) log2(float4 __x );
float8 __attribute__((__overloadable__)) log2(float8 __x );
float16 __attribute__((__overloadable__)) log2(float16 __x );
float __attribute__((__overloadable__)) log1p(float __x);
float2 __attribute__((__overloadable__)) log1p(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) log1p(float3 __x );
#endif
float4 __attribute__((__overloadable__)) log1p(float4 __x );
float8 __attribute__((__overloadable__)) log1p(float8 __x );
float16 __attribute__((__overloadable__)) log1p(float16 __x );
float __attribute__((__overloadable__)) tanhf(float __x);
float2 __attribute__((__overloadable__)) tanhf(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) tanhf(float3 __x );
#endif
float4 __attribute__((__overloadable__)) tanhf(float4 __x );
float8 __attribute__((__overloadable__)) tanhf(float8 __x );
float16 __attribute__((__overloadable__)) tanhf(float16 __x );
float __attribute__((__overloadable__)) hypotf(float __x, float __y);
float2 __attribute__((__overloadable__)) hypotf(float2 __x, float2 __y);
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) hypotf(float3 __x, float3 __y);
#endif
float4 __attribute__((__overloadable__)) hypotf(float4 __x, float4 __y);
float8 __attribute__((__overloadable__)) hypotf(float8 __x, float8 __y);
float16 __attribute__((__overloadable__)) hypotf(float16 __x, float16 __y);
float __attribute__((__overloadable__)) rsqrt(float __x);
float2 __attribute__((__overloadable__)) rsqrt(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) rsqrt(float3 __x );
#endif
float4 __attribute__((__overloadable__)) rsqrt(float4 __x );
float8 __attribute__((__overloadable__)) rsqrt(float8 __x );
float16 __attribute__((__overloadable__)) rsqrt(float16 __x );
float __attribute__((__overloadable__)) asin(float __x);
float2 __attribute__((__overloadable__)) asin(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) asin(float3 __x );
#endif
float4 __attribute__((__overloadable__)) asin(float4 __x );
float8 __attribute__((__overloadable__)) asin(float8 __x );
float16 __attribute__((__overloadable__)) asin(float16 __x );
float __attribute__((__overloadable__)) asinh(float __x);
float2 __attribute__((__overloadable__)) asinh(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) asinh(float3 __x );
#endif
float4 __attribute__((__overloadable__)) asinh(float4 __x );
float8  __attribute__((__overloadable__)) asinh(float8 __x);
float16  __attribute__((__overloadable__)) asinh(float16 __x);
float __attribute__((__overloadable__)) asinpi(float __x);
float2 __attribute__((__overloadable__)) asinpi(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) asinpi(float3 __x );
#endif
float4 __attribute__((__overloadable__)) asinpi(float4 __x );
float8 __attribute__((__overloadable__)) asinpi(float8 __x );
float16 __attribute__((__overloadable__)) asinpi(float16 __x );
float __attribute__((__overloadable__)) sin(float __x);
float2 __attribute__((__overloadable__)) sin(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) sin(float3 __x );
#endif
float4 __attribute__((__overloadable__)) sin(float4 __x );
float8 __attribute__((__overloadable__)) sin(float8 __x );
float16 __attribute__((__overloadable__)) sin(float16 __x );
float __attribute__((__overloadable__)) sinpi(float __x);
float2 __attribute__((__overloadable__)) sinpi(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) sinpi(float3 __x );
#endif
float4 __attribute__((__overloadable__)) sinpi(float4 __x );
float8 __attribute__((__overloadable__)) sinpi(float8 __x );
float16 __attribute__((__overloadable__)) sinpi(float16 __x );
float __attribute__((__overloadable__)) acos(float __x);
float2 __attribute__((__overloadable__)) acos(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) acos(float3 __x );
#endif
float4 __attribute__((__overloadable__)) acos(float4 __x );
float8 __attribute__((__overloadable__)) acos(float8 __x );
float16 __attribute__((__overloadable__)) acos(float16 __x );
float __attribute__((__overloadable__)) acosh(float __x);
float2 __attribute__((__overloadable__)) acosh(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) acosh(float3 __x );
#endif
float4 __attribute__((__overloadable__)) acosh(float4 __x );
float8 __attribute__((__overloadable__)) acosh(float8 __x );
float16 __attribute__((__overloadable__)) acosh(float16 __x );
float __attribute__((__overloadable__)) acospi(float __x);
float2 __attribute__((__overloadable__)) acospi(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) acospi(float3 __x );
#endif
float4 __attribute__((__overloadable__)) acospi(float4 __x );
float8 __attribute__((__overloadable__)) acospi(float8 __x );
float16 __attribute__((__overloadable__)) acospi(float16 __x );
float __attribute__((__overloadable__)) cos(float __x);
float2 __attribute__((__overloadable__)) cos(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) cos(float3 __x );
#endif
float4 __attribute__((__overloadable__)) cos(float4 __x );
float8 __attribute__((__overloadable__)) cos(float8 __x );
float16 __attribute__((__overloadable__)) cos(float16 __x );
float __attribute__((__overloadable__)) cospi(float __x);
float2 __attribute__((__overloadable__)) cospi(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) cospi(float3 __x );
#endif
float4 __attribute__((__overloadable__)) cospi(float4 __x );
float8 __attribute__((__overloadable__)) cospi(float8 __x );
float16 __attribute__((__overloadable__)) cospi(float16 __x );
float __attribute__((__overloadable__)) log(float __x);
float2 __attribute__((__overloadable__)) log(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) log(float3 __x );
#endif
float4 __attribute__((__overloadable__)) log(float4 __x );
float8 __attribute__((__overloadable__)) log(float8 __x );
float16 __attribute__((__overloadable__)) log(float16 __x );
float __attribute__((__overloadable__)) expm1(float __x);
float2 __attribute__((__overloadable__)) expm1(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) expm1(float3 __x );
#endif
float4 __attribute__((__overloadable__)) expm1(float4 __x );
float8 __attribute__((__overloadable__)) expm1(float8 __x );
float16 __attribute__((__overloadable__)) expm1(float16 __x );
float __attribute__((__overloadable__)) maxmag(float __x, float __y);
float2 __attribute__((__overloadable__)) maxmag(float2 __x, float2 __y);
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) maxmag(float3 __x, float3 __y);
#endif
float4 __attribute__((__overloadable__)) maxmag(float4 __x, float4 __y);
float8 __attribute__((__overloadable__)) maxmag(float8 __x, float8 __y);
float16 __attribute__((__overloadable__)) maxmag(float16 __x, float16 __y);
float __attribute__((__overloadable__)) minmag(float __x, float __y);
float2 __attribute__((__overloadable__)) minmag(float2 __x, float2 __y);
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) minmag(float3 __x, float3 __y);
#endif
float4 __attribute__((__overloadable__)) minmag(float4 __x, float4 __y);
float8 __attribute__((__overloadable__)) minmag(float8 __x, float8 __y);
float16 __attribute__((__overloadable__)) minmag(float16 __x, float16 __y);
float __attribute__((__overloadable__)) lgamma(float __x);
float2 __attribute__((__overloadable__)) lgamma(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) lgamma(float3 __x );
#endif
float4 __attribute__((__overloadable__)) lgamma(float4 __x );
float8 __attribute__((__overloadable__)) lgamma(float8 __x );
float16 __attribute__((__overloadable__)) lgamma(float16 __x );
float __attribute__((__overloadable__)) tgamma(float __x);
float2 __attribute__((__overloadable__)) tgamma(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) tgamma(float3 __x );
#endif
float4 __attribute__((__overloadable__)) tgamma(float4 __x );
float8 __attribute__((__overloadable__)) tgamma(float8 __x );
float16 __attribute__((__overloadable__)) tgamma(float16 __x );
float __attribute__((__overloadable__)) pow(float __x, float __y);
float2 __attribute__((__overloadable__)) pow(float2 __x, float2 __y);
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) pow(float3 __x, float3 __y);
#endif
float4 __attribute__((__overloadable__)) pow(float4 __x, float4 __y);
float8  __attribute__((__overloadable__)) pow(float8 __x, float8 __y);
float16  __attribute__((__overloadable__)) pow(float16 __x, float16 __y);
float __attribute__((__overloadable__)) powr(float __x, float __y);
float2 __attribute__((__overloadable__)) powr(float2 __x, float2 __y);
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) powr(float3 __x, float3 __y);
#endif
float4 __attribute__((__overloadable__)) powr(float4 __x, float4 __y);
float8  __attribute__((__overloadable__)) powr(float8 __x, float8 __y);
float16  __attribute__((__overloadable__)) powr(float16 __x, float16 __y);
float __attribute__((__overloadable__)) half_powr(float __x, float __y);
float2 __attribute__((__overloadable__)) half_powr(float2 __x, float2 __y);
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) half_powr(float3 __x, float3 __y);
#endif
float4 __attribute__((__overloadable__)) half_powr(float4 __x, float4 __y);
float8  __attribute__((__overloadable__)) half_powr(float8 __x, float8 __y);
float16  __attribute__((__overloadable__)) half_powr(float16 __x, float16 __y);
float __attribute__((__overloadable__)) fdim(float __x, float __y);
float2 __attribute__((__overloadable__)) fdim(float2 __x, float2 __y);
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) fdim(float3 __x, float3 __y);
#endif
float4 __attribute__((__overloadable__)) fdim(float4 __x, float4 __y);
float8 __attribute__((__overloadable__)) fdim(float8 __x, float8 __y);
float16 __attribute__((__overloadable__)) fdim(float16 __x, float16 __y);
float __attribute__((__overloadable__)) cbrt(float __x);
float2 __attribute__((__overloadable__)) cbrt(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) cbrt(float3 __x );
#endif
float4 __attribute__((__overloadable__)) cbrt(float4 __x );
float8 __attribute__((__overloadable__)) cbrt(float8 __x );
float16 __attribute__((__overloadable__)) cbrt(float16 __x );
float __attribute__((__overloadable__)) atan(float __x);
float2 __attribute__((__overloadable__)) atan(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) atan(float3 __x );
#endif
float4 __attribute__((__overloadable__)) atan(float4 __x );
float8 __attribute__((__overloadable__)) atan(float8 __x );
float16 __attribute__((__overloadable__)) atan(float16 __x );
float __attribute__((__overloadable__)) atan2(float __x, float __y);
float2 __attribute__((__overloadable__)) atan2(float2 __x, float2 __y);
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) atan2(float3 __x, float3 __y);
#endif
float4 __attribute__((__overloadable__)) atan2(float4 __x, float4 __y);
float8 __attribute__((__overloadable__)) atan2(float8 __x, float8 __y);
float16 __attribute__((__overloadable__)) atan2(float16 __x, float16 __y);
float __attribute__((__overloadable__)) atan2pi(float __x, float __y);
float2 __attribute__((__overloadable__)) atan2pi(float2 __x, float2 __y);
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) atan2pi(float3 __x, float3 __y);
#endif
float4 __attribute__((__overloadable__)) atan2pi(float4 __x, float4 __y);
float8 __attribute__((__overloadable__)) atan2pi(float8 __x, float8 __y);
float16 __attribute__((__overloadable__)) atan2pi(float16 __x, float16 __y);
float __attribute__((__overloadable__)) atanh(float __x);
float2 __attribute__((__overloadable__)) atanh(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) atanh(float3 __x );
#endif
float4 __attribute__((__overloadable__)) atanh(float4 __x );
float8  __attribute__((__overloadable__)) atanh(float8 __x);
float16  __attribute__((__overloadable__)) atanh(float16 __x);
float __attribute__((__overloadable__)) atanpi(float __x);
float2 __attribute__((__overloadable__)) atanpi(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) atanpi(float3 __x );
#endif
float4 __attribute__((__overloadable__)) atanpi(float4 __x );
float8 __attribute__((__overloadable__)) atanpi(float8 __x );
float16 __attribute__((__overloadable__)) atanpi(float16 __x );
float __attribute__((__overloadable__)) tan(float __x);
float2 __attribute__((__overloadable__)) tan(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) tan(float3 __x );
#endif
float4 __attribute__((__overloadable__)) tan(float4 __x );
float8 __attribute__((__overloadable__)) tan(float8 __x );
float16 __attribute__((__overloadable__)) tan(float16 __x );
float __attribute__((__overloadable__)) tanpi(float __x);
float2 __attribute__((__overloadable__)) tanpi(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) tanpi(float3 __x );
#endif
float4 __attribute__((__overloadable__)) tanpi(float4 __x );
float8 __attribute__((__overloadable__)) tanpi(float8 __x );
float16 __attribute__((__overloadable__)) tanpi(float16 __x );
float __attribute__((__overloadable__)) cosh(float __x);
float2 __attribute__((__overloadable__)) cosh(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) cosh(float3 __x );
#endif
float4 __attribute__((__overloadable__)) cosh(float4 __x );
float8 __attribute__((__overloadable__)) cosh(float8 __x );
float16 __attribute__((__overloadable__)) cosh(float16 __x );
float __attribute__((__overloadable__)) sinh(float __x);
float2 __attribute__((__overloadable__)) sinh(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) sinh(float3 __x );
#endif
float4 __attribute__((__overloadable__)) sinh(float4 __x );
float8  __attribute__((__overloadable__)) sinh(float8 __x);
float16  __attribute__((__overloadable__)) sinh(float16 __x);
float __attribute__((__overloadable__)) tanh(float __x);
float2 __attribute__((__overloadable__)) tanh(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) tanh(float3 __x );
#endif
float4 __attribute__((__overloadable__)) tanh(float4 __x );
float8  __attribute__((__overloadable__)) tanh(float8 __x);
float16  __attribute__((__overloadable__)) tanh(float16 __x);
float __attribute__((__overloadable__)) exp10(float __x);
float2 __attribute__((__overloadable__)) exp10(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) exp10(float3 __x );
#endif
float4 __attribute__((__overloadable__)) exp10(float4 __x );
float8 __attribute__((__overloadable__)) exp10(float8 __x );
float16 __attribute__((__overloadable__)) exp10(float16 __x );
float __attribute__((__overloadable__)) exp2(float __x);
float2 __attribute__((__overloadable__)) exp2(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) exp2(float3 __x );
#endif
float4 __attribute__((__overloadable__)) exp2(float4 __x );
float8 __attribute__((__overloadable__)) exp2(float8 __x );
float16 __attribute__((__overloadable__)) exp2(float16 __x );
float __attribute__((__overloadable__)) hypot(float __x, float __y);
float2 __attribute__((__overloadable__)) hypot(float2 __x, float2 __y);
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) hypot(float3 __x, float3 __y);
#endif
float4 __attribute__((__overloadable__)) hypot(float4 __x, float4 __y);
float8 __attribute__((__overloadable__)) hypot(float8 __x, float8 __y);
float16 __attribute__((__overloadable__)) hypot(float16 __x, float16 __y);
float __attribute__((__overloadable__)) erf(float __x);
float2 __attribute__((__overloadable__)) erf(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) erf(float3 __x );
#endif
float4 __attribute__((__overloadable__)) erf(float4 __x );
float8 __attribute__((__overloadable__)) erf(float8 __x );
float16 __attribute__((__overloadable__)) erf(float16 __x );
float __attribute__((__overloadable__)) erfc(float __x);
float2 __attribute__((__overloadable__)) erfc(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) erfc(float3 __x );
#endif
float4 __attribute__((__overloadable__)) erfc(float4 __x );
float8 __attribute__((__overloadable__)) erfc(float8 __x );
float16 __attribute__((__overloadable__)) erfc(float16 __x );
float __attribute__((__overloadable__)) native_recip(float __x);
float2 __attribute__((__overloadable__)) native_recip(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) native_recip(float3 __x );
#endif
float4 __attribute__((__overloadable__)) native_recip(float4 __x );
float8 __attribute__((__overloadable__)) native_recip(float8 __x );
float16 __attribute__((__overloadable__)) native_recip(float16 __x );
float __attribute__((__overloadable__)) native_powr(float __x, float __y);
float2 __attribute__((__overloadable__)) native_powr(float2 __x, float2 __y);
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) native_powr(float3 __x, float3 __y);
#endif
float4 __attribute__((__overloadable__)) native_powr(float4 __x, float4 __y);
float8  __attribute__((__overloadable__)) native_powr(float8 __x, float8 __y);
float16  __attribute__((__overloadable__)) native_powr(float16 __x, float16 __y);
float __attribute__((__overloadable__)) native_divide(float __x, float __y);
float2 __attribute__((__overloadable__)) native_divide(float2 __x, float2 __y);
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) native_divide(float3 __x, float3 __y);
#endif
float4 __attribute__((__overloadable__)) native_divide(float4 __x, float4 __y);
float8 __attribute__((__overloadable__)) native_divide(float8 __x, float8 __y);
float16 __attribute__((__overloadable__)) native_divide(float16 __x, float16 __y);
double __attribute__((__overloadable__)) hypot(double __x, double __y);
double2 __attribute__((__overloadable__)) hypot(double2 __x, double2 __y);
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) hypot(double3 __x, double3 __y);
#endif
double4 __attribute__((__overloadable__)) hypot(double4 __x, double4 __y);
double8 __attribute__((__overloadable__)) hypot(double8 __x, double8 __y);
double16 __attribute__((__overloadable__)) hypot(double16 __x, double16 __y);
double __attribute__((__overloadable__)) sin(double __x);
double2 __attribute__((__overloadable__)) sin(double2 __x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) sin(double3 __x );
#endif
double4 __attribute__((__overloadable__)) sin(double4 __x );
double8 __attribute__((__overloadable__)) sin(double8 __x );
double16 __attribute__((__overloadable__)) sin(double16 __x );
double __attribute__((__overloadable__)) cos(double __x);
double2 __attribute__((__overloadable__)) cos(double2 __x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) cos(double3 __x );
#endif
double4 __attribute__((__overloadable__)) cos(double4 __x );
double8 __attribute__((__overloadable__)) cos(double8 __x );
double16 __attribute__((__overloadable__)) cos(double16 __x );
double __attribute__((__overloadable__)) tan(double __x);
double2 __attribute__((__overloadable__)) tan(double2 __x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) tan(double3 __x );
#endif
double4 __attribute__((__overloadable__)) tan(double4 __x );
double8 __attribute__((__overloadable__)) tan(double8 __x );
double16 __attribute__((__overloadable__)) tan(double16 __x );
double __attribute__((__overloadable__)) sinpi(double __x);
double2 __attribute__((__overloadable__)) sinpi(double2 __x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) sinpi(double3 __x );
#endif
double4 __attribute__((__overloadable__)) sinpi(double4 __x );
double8 __attribute__((__overloadable__)) sinpi(double8 __x );
double16 __attribute__((__overloadable__)) sinpi(double16 __x );
double __attribute__((__overloadable__)) cospi(double __x);
double2 __attribute__((__overloadable__)) cospi(double2 __x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) cospi(double3 __x );
#endif
double4 __attribute__((__overloadable__)) cospi(double4 __x );
double8 __attribute__((__overloadable__)) cospi(double8 __x );
double16 __attribute__((__overloadable__)) cospi(double16 __x );
double __attribute__((__overloadable__)) pow(double __x, double __y);
double2 __attribute__((__overloadable__)) pow(double2 __x, double2 __y);
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) pow(double3 __x, double3 __y);
#endif
double4 __attribute__((__overloadable__)) pow(double4 __x, double4 __y);
double8 __attribute__((__overloadable__)) pow(double8 __x, double8 __y);
double16 __attribute__((__overloadable__)) pow(double16 __x, double16 __y);
double __attribute__((__overloadable__)) powr(double __x, double __y);
double2 __attribute__((__overloadable__)) powr(double2 __x, double2 __y);
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) powr(double3 __x, double3 __y);
#endif
double4 __attribute__((__overloadable__)) powr(double4 __x, double4 __y);
double8 __attribute__((__overloadable__)) powr(double8 __x, double8 __y);
double16 __attribute__((__overloadable__)) powr(double16 __x, double16 __y);
double __attribute__((__overloadable__)) pown(double __x, int __y);
double2 __attribute__((__overloadable__)) pown(double2 __x, int2 __y);
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) pown(double3 __x, int3 __y);
#endif
double4 __attribute__((__overloadable__)) pown(double4 __x, int4 __y);
double8 __attribute__((__overloadable__)) pown(double8 __x, int8 __y);
double16 __attribute__((__overloadable__)) pown(double16 __x, int16 __y);
double __attribute__((__overloadable__)) half_powr(double __x, double __y);
double2 __attribute__((__overloadable__)) half_powr(double2 __x, double2 __y);
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) half_powr(double3 __x, double3 __y);
#endif
double4 __attribute__((__overloadable__)) half_powr(double4 __x, double4 __y);
double8 __attribute__((__overloadable__)) half_powr(double8 __x, double8 __y);
double16 __attribute__((__overloadable__)) half_powr(double16 __x, double16 __y);
double __attribute__((__overloadable__)) cbrt(double __x);
double2 __attribute__((__overloadable__)) cbrt(double2 __x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) cbrt(double3 __x );
#endif
double4 __attribute__((__overloadable__)) cbrt(double4 __x );
double8 __attribute__((__overloadable__)) cbrt(double8 __x );
double16 __attribute__((__overloadable__)) cbrt(double16 __x );
double __attribute__((__overloadable__)) rootn(double __x, int __y);
double2 __attribute__((__overloadable__)) rootn(double2 __x, int2 __y);
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) rootn(double3 __x, int3 __y);
#endif
double4 __attribute__((__overloadable__)) rootn(double4 __x, int4 __y);
double8 __attribute__((__overloadable__)) rootn(double8 __x, int8 __y);
double16 __attribute__((__overloadable__)) rootn(double16 __x, int16 __y);
double __attribute__((__overloadable__)) cosh(double __x);
double2 __attribute__((__overloadable__)) cosh(double2 __x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) cosh(double3 __x );
#endif
double4 __attribute__((__overloadable__)) cosh(double4 __x );
double8 __attribute__((__overloadable__)) cosh(double8 __x );
double16 __attribute__((__overloadable__)) cosh(double16 __x );
double __attribute__((__overloadable__)) sinh(double __x);
double2 __attribute__((__overloadable__)) sinh(double2 __x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) sinh(double3 __x );
#endif
double4 __attribute__((__overloadable__)) sinh(double4 __x );
double8 __attribute__((__overloadable__)) sinh(double8 __x );
double16 __attribute__((__overloadable__)) sinh(double16 __x );
double __attribute__((__overloadable__)) tanpi(double __x);
double2 __attribute__((__overloadable__)) tanpi(double2 __x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) tanpi(double3 __x );
#endif
double4 __attribute__((__overloadable__)) tanpi(double4 __x );
double8 __attribute__((__overloadable__)) tanpi(double8 __x );
double16 __attribute__((__overloadable__)) tanpi(double16 __x );
double __attribute__((__overloadable__)) expm1(double __x);
double2 __attribute__((__overloadable__)) expm1(double2 __x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) expm1(double3 __x );
#endif
double4 __attribute__((__overloadable__)) expm1(double4 __x );
double8 __attribute__((__overloadable__)) expm1(double8 __x );
double16 __attribute__((__overloadable__)) expm1(double16 __x );
double __attribute__((__overloadable__)) tanh(double __x);
double2 __attribute__((__overloadable__)) tanh(double2 __x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) tanh(double3 __x );
#endif
double4 __attribute__((__overloadable__)) tanh(double4 __x );
double8 __attribute__((__overloadable__)) tanh(double8 __x );
double16 __attribute__((__overloadable__)) tanh(double16 __x );
double __attribute__((__overloadable__)) asinh(double __x);
double2 __attribute__((__overloadable__)) asinh(double2 __x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) asinh(double3 __x );
#endif
double4 __attribute__((__overloadable__)) asinh(double4 __x );
double8 __attribute__((__overloadable__)) asinh(double8 __x );
double16 __attribute__((__overloadable__)) asinh(double16 __x );
double __attribute__((__overloadable__)) acosh(double __x);
double2 __attribute__((__overloadable__)) acosh(double2 __x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) acosh(double3 __x );
#endif
double4 __attribute__((__overloadable__)) acosh(double4 __x );
double8 __attribute__((__overloadable__)) acosh(double8 __x );
double16 __attribute__((__overloadable__)) acosh(double16 __x );
double __attribute__((__overloadable__)) atanh(double __x);
double2 __attribute__((__overloadable__)) atanh(double2 __x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) atanh(double3 __x );
#endif
double4 __attribute__((__overloadable__)) atanh(double4 __x );
double8 __attribute__((__overloadable__)) atanh(double8 __x );
double16 __attribute__((__overloadable__)) atanh(double16 __x );
double __attribute__((__overloadable__)) lgamma(double __x);
double2 __attribute__((__overloadable__)) lgamma(double2 __x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) lgamma(double3 __x );
#endif
double4 __attribute__((__overloadable__)) lgamma(double4 __x );
double8 __attribute__((__overloadable__)) lgamma(double8 __x );
double16 __attribute__((__overloadable__)) lgamma(double16 __x );
double __attribute__((__overloadable__)) tgamma(double __x);
double2 __attribute__((__overloadable__)) tgamma(double2 __x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) tgamma(double3 __x );
#endif
double4 __attribute__((__overloadable__)) tgamma(double4 __x );
double8 __attribute__((__overloadable__)) tgamma(double8 __x );
double16 __attribute__((__overloadable__)) tgamma(double16 __x );
double __attribute__((__overloadable__)) maxmag(double __x, double __y);
double2 __attribute__((__overloadable__)) maxmag(double2 __x, double2 __y);
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) maxmag(double3 __x, double3 __y);
#endif
double4 __attribute__((__overloadable__)) maxmag(double4 __x, double4 __y);
double8 __attribute__((__overloadable__)) maxmag(double8 __x, double8 __y);
double16 __attribute__((__overloadable__)) maxmag(double16 __x, double16 __y);
double __attribute__((__overloadable__)) minmag(double __x, double __y);
double2 __attribute__((__overloadable__)) minmag(double2 __x, double2 __y);
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) minmag(double3 __x, double3 __y);
#endif
double4 __attribute__((__overloadable__)) minmag(double4 __x, double4 __y);
double8 __attribute__((__overloadable__)) minmag(double8 __x, double8 __y);
double16 __attribute__((__overloadable__)) minmag(double16 __x, double16 __y);
double __attribute__((__overloadable__)) fdim(double __x, double __y);
double2 __attribute__((__overloadable__)) fdim(double2 __x, double2 __y);
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) fdim(double3 __x, double3 __y);
#endif
double4 __attribute__((__overloadable__)) fdim(double4 __x, double4 __y);
double8 __attribute__((__overloadable__)) fdim(double8 __x, double8 __y);
double16 __attribute__((__overloadable__)) fdim(double16 __x, double16 __y);
double __attribute__((__overloadable__)) erf(double __x);
double2 __attribute__((__overloadable__)) erf(double2 __x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) erf(double3 __x );
#endif
double4 __attribute__((__overloadable__)) erf(double4 __x );
double8 __attribute__((__overloadable__)) erf(double8 __x );
double16 __attribute__((__overloadable__)) erf(double16 __x );
double __attribute__((__overloadable__)) erfc(double __x);
double2 __attribute__((__overloadable__)) erfc(double2 __x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) erfc(double3 __x );
#endif
double4 __attribute__((__overloadable__)) erfc(double4 __x );
double8 __attribute__((__overloadable__)) erfc(double8 __x );
double16 __attribute__((__overloadable__)) erfc(double16 __x );
double __attribute__((__overloadable__)) native_recip(double __x);
double2 __attribute__((__overloadable__)) native_recip(double2 __x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) native_recip(double3 __x );
#endif
double4 __attribute__((__overloadable__)) native_recip(double4 __x );
double8 __attribute__((__overloadable__)) native_recip(double8 __x );
double16 __attribute__((__overloadable__)) native_recip(double16 __x );
double __attribute__((__overloadable__)) native_powr(double __x, double __y);
double2 __attribute__((__overloadable__)) native_powr(double2 __x, double2 __y);
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) native_powr(double3 __x, double3 __y);
#endif
double4 __attribute__((__overloadable__)) native_powr(double4 __x, double4 __y);
double8 __attribute__((__overloadable__)) native_powr(double8 __x, double8 __y);
double16 __attribute__((__overloadable__)) native_powr(double16 __x, double16 __y);
double __attribute__((__overloadable__)) native_divide(double __x, double __y);
double2 __attribute__((__overloadable__)) native_divide(double2 __x, double2 __y);
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) native_divide(double3 __x, double3 __y);
#endif
double4 __attribute__((__overloadable__)) native_divide(double4 __x, double4 __y);
double8 __attribute__((__overloadable__)) native_divide(double8 __x, double8 __y);
double16 __attribute__((__overloadable__)) native_divide(double16 __x, double16 __y);
double __attribute__((__overloadable__)) acos(double __x);
double2 __attribute__((__overloadable__)) acos(double2 __x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) acos(double3 __x );
#endif
double4 __attribute__((__overloadable__)) acos(double4 __x );
double8 __attribute__((__overloadable__)) acos(double8 __x );
double16 __attribute__((__overloadable__)) acos(double16 __x );
double __attribute__((__overloadable__)) acospi(double __x);
double2 __attribute__((__overloadable__)) acospi(double2 __x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) acospi(double3 __x );
#endif
double4 __attribute__((__overloadable__)) acospi(double4 __x );
double8 __attribute__((__overloadable__)) acospi(double8 __x );
double16 __attribute__((__overloadable__)) acospi(double16 __x );
double __attribute__((__overloadable__)) asin(double __x);
double2 __attribute__((__overloadable__)) asin(double2 __x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) asin(double3 __x );
#endif
double4 __attribute__((__overloadable__)) asin(double4 __x );
double8 __attribute__((__overloadable__)) asin(double8 __x );
double16 __attribute__((__overloadable__)) asin(double16 __x );
double __attribute__((__overloadable__)) asinpi(double __x);
double2 __attribute__((__overloadable__)) asinpi(double2 __x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) asinpi(double3 __x );
#endif
double4 __attribute__((__overloadable__)) asinpi(double4 __x );
double8 __attribute__((__overloadable__)) asinpi(double8 __x );
double16 __attribute__((__overloadable__)) asinpi(double16 __x );
double __attribute__((__overloadable__)) atan(double __x);
double2 __attribute__((__overloadable__)) atan(double2 __x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) atan(double3 __x );
#endif
double4 __attribute__((__overloadable__)) atan(double4 __x );
double8 __attribute__((__overloadable__)) atan(double8 __x );
double16 __attribute__((__overloadable__)) atan(double16 __x );
double __attribute__((__overloadable__)) atanpi(double __x);
double2 __attribute__((__overloadable__)) atanpi(double2 __x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) atanpi(double3 __x );
#endif
double4 __attribute__((__overloadable__)) atanpi(double4 __x );
double8 __attribute__((__overloadable__)) atanpi(double8 __x );
double16 __attribute__((__overloadable__)) atanpi(double16 __x );
double __attribute__((__overloadable__)) atan2(double __x, double __y);
double2 __attribute__((__overloadable__)) atan2(double2 __x, double2 __y);
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) atan2(double3 __x, double3 __y);
#endif
double4 __attribute__((__overloadable__)) atan2(double4 __x, double4 __y);
double8 __attribute__((__overloadable__)) atan2(double8 __x, double8 __y);
double16 __attribute__((__overloadable__)) atan2(double16 __x, double16 __y);
double __attribute__((__overloadable__)) atan2pi(double __x, double __y);
double2 __attribute__((__overloadable__)) atan2pi(double2 __x, double2 __y);
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__)) atan2pi(double3 __x, double3 __y);
#endif
double4 __attribute__((__overloadable__)) atan2pi(double4 __x, double4 __y);
double8 __attribute__((__overloadable__)) atan2pi(double8 __x, double8 __y);
double16 __attribute__((__overloadable__)) atan2pi(double16 __x, double16 __y);
float __attribute__((__overloadable__)) half_cos(float __x);
float2 __attribute__((__overloadable__)) half_cos(float2 __x);
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) half_cos(float3 __x);
#endif
float4 __attribute__((__overloadable__)) half_cos(float4 __x);
float8 __attribute__((__overloadable__)) half_cos(float8 __x);
float16 __attribute__((__overloadable__)) half_cos(float16 __x);
float __attribute__((__overloadable__)) half_exp(float __x);
float2 __attribute__((__overloadable__)) half_exp(float2 __x);
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) half_exp(float3 __x);
#endif
float4 __attribute__((__overloadable__)) half_exp(float4 __x);
float8 __attribute__((__overloadable__)) half_exp(float8 __x);
float16 __attribute__((__overloadable__)) half_exp(float16 __x);
float __attribute__((__overloadable__)) half_exp2(float __x);
float2 __attribute__((__overloadable__)) half_exp2(float2 __x);
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) half_exp2(float3 __x);
#endif
float4 __attribute__((__overloadable__)) half_exp2(float4 __x);
float8 __attribute__((__overloadable__)) half_exp2(float8 __x);
float16 __attribute__((__overloadable__)) half_exp2(float16 __x);
float __attribute__((__overloadable__)) half_exp10(float __x);
float2 __attribute__((__overloadable__)) half_exp10(float2 __x);
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) half_exp10(float3 __x);
#endif
float4 __attribute__((__overloadable__)) half_exp10(float4 __x);
float8 __attribute__((__overloadable__)) half_exp10(float8 __x);
float16 __attribute__((__overloadable__)) half_exp10(float16 __x);
float __attribute__((__overloadable__)) half_log(float __x);
float2 __attribute__((__overloadable__)) half_log(float2 __x);
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) half_log(float3 __x);
#endif
float4 __attribute__((__overloadable__)) half_log(float4 __x);
float8 __attribute__((__overloadable__)) half_log(float8 __x);
float16 __attribute__((__overloadable__)) half_log(float16 __x);
float __attribute__((__overloadable__)) half_log2(float __x);
float2 __attribute__((__overloadable__)) half_log2(float2 __x);
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) half_log2(float3 __x);
#endif
float4 __attribute__((__overloadable__)) half_log2(float4 __x);
float8 __attribute__((__overloadable__)) half_log2(float8 __x);
float16 __attribute__((__overloadable__)) half_log2(float16 __x);
float __attribute__((__overloadable__)) half_log10(float __x);
float2 __attribute__((__overloadable__)) half_log10(float2 __x);
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) half_log10(float3 __x);
#endif
float4 __attribute__((__overloadable__)) half_log10(float4 __x);
float8 __attribute__((__overloadable__)) half_log10(float8 __x);
float16 __attribute__((__overloadable__)) half_log10(float16 __x);
float __attribute__((__overloadable__)) half_rsqrt(float __x);
float2 __attribute__((__overloadable__)) half_rsqrt(float2 __x);
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) half_rsqrt(float3 __x);
#endif
float4 __attribute__((__overloadable__)) half_rsqrt(float4 __x);
float8 __attribute__((__overloadable__)) half_rsqrt(float8 __x);
float16 __attribute__((__overloadable__)) half_rsqrt(float16 __x);
float __attribute__((__overloadable__)) half_sin(float __x);
float2 __attribute__((__overloadable__)) half_sin(float2 __x);
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) half_sin(float3 __x);
#endif
float4 __attribute__((__overloadable__)) half_sin(float4 __x);
float8 __attribute__((__overloadable__)) half_sin(float8 __x);
float16 __attribute__((__overloadable__)) half_sin(float16 __x);
float __attribute__((__overloadable__)) half_sqrt(float __x);
float2 __attribute__((__overloadable__)) half_sqrt(float2 __x);
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) half_sqrt(float3 __x);
#endif
float4 __attribute__((__overloadable__)) half_sqrt(float4 __x);
float8 __attribute__((__overloadable__)) half_sqrt(float8 __x);
float16 __attribute__((__overloadable__)) half_sqrt(float16 __x);
float __attribute__((__overloadable__)) half_tan(float __x);
float2 __attribute__((__overloadable__)) half_tan(float2 __x);
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) half_tan(float3 __x);
#endif
float4 __attribute__((__overloadable__)) half_tan(float4 __x);
float8 __attribute__((__overloadable__)) half_tan(float8 __x);
float16 __attribute__((__overloadable__)) half_tan(float16 __x);
float __attribute__((__overloadable__)) half_divide(float __x, float __y);
float2 __attribute__((__overloadable__)) half_divide(float2 __x, float2 __y);
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) half_divide(float3 __x, float3 __y);
#endif
float4 __attribute__((__overloadable__)) half_divide(float4 __x, float4 __y);
float8 __attribute__((__overloadable__)) half_divide(float8 __x, float8 __y);
float16 __attribute__((__overloadable__)) half_divide(float16 __x, float16 __y);
float __attribute__((__overloadable__)) half_recip(float __x);
float2 __attribute__((__overloadable__)) half_recip(float2 __x);
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) half_recip(float3 __x);
#endif
float4 __attribute__((__overloadable__)) half_recip(float4 __x);
float8 __attribute__((__overloadable__)) half_recip(float8 __x);
float16 __attribute__((__overloadable__)) half_recip(float16 __x);
float __attribute__((__overloadable__,__always_inline__)) remquo(float x, float y,  int *quo );
float __attribute__((__overloadable__,__always_inline__)) remainder(float x, float y);
float2 __attribute__((__overloadable__,__always_inline__)) remquo(float2 x, float2 y,  int2 *quo );
float2 __attribute__((__overloadable__,__always_inline__)) remainder(float2 x, float2 y);
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__,__always_inline__)) remquo(float3 x, float3 y,  int3 *quo );
float3 __attribute__((__overloadable__,__always_inline__)) remainder(float3 x, float3 y);
#endif
float4 __attribute__((__overloadable__,__always_inline__)) remquo(float4 x, float4 y,  int4 *quo );
float4 __attribute__((__overloadable__,__always_inline__)) remainder(float4 x, float4 y);
float8 __attribute__((__overloadable__,__always_inline__)) remquo(float8 x, float8 y,  int8 *quo );
float8 __attribute__((__overloadable__,__always_inline__)) remainder(float8 x, float8 y);
float16 __attribute__((__overloadable__,__always_inline__)) remquo(float16 x, float16 y,  int16 *quo );
float16 __attribute__((__overloadable__,__always_inline__)) remainder(float16 x, float16 y);
float __attribute__((__overloadable__,__always_inline__)) remquo(float x, float y, global int *quo );
float2 __attribute__((__overloadable__,__always_inline__)) remquo(float2 x, float2 y, global int2 *quo );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__,__always_inline__)) remquo(float3 x, float3 y, global int3 *quo );
#endif
float4 __attribute__((__overloadable__,__always_inline__)) remquo(float4 x, float4 y, global int4 *quo );
float8 __attribute__((__overloadable__,__always_inline__)) remquo(float8 x, float8 y, global int8 *quo );
float16 __attribute__((__overloadable__,__always_inline__)) remquo(float16 x, float16 y, global int16 *quo );
float __attribute__((__overloadable__,__always_inline__)) remquo(float x, float y, local int *quo );
float2 __attribute__((__overloadable__,__always_inline__)) remquo(float2 x, float2 y, local int2 *quo );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__,__always_inline__)) remquo(float3 x, float3 y, local int3 *quo );
#endif
float4 __attribute__((__overloadable__,__always_inline__)) remquo(float4 x, float4 y, local int4 *quo );
float8 __attribute__((__overloadable__,__always_inline__)) remquo(float8 x, float8 y, local int8 *quo );
float16 __attribute__((__overloadable__,__always_inline__)) remquo(float16 x, float16 y, local int16 *quo );
double __attribute__((__overloadable__,__always_inline__)) remquo(double x, double y,  int *quo );
double __attribute__((__overloadable__,__always_inline__)) remainder(double x, double y);
double2 __attribute__((__overloadable__,__always_inline__)) remquo(double2 x, double2 y,  int2 *quo );
double2 __attribute__((__overloadable__,__always_inline__)) remainder(double2 x, double2 y);
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__,__always_inline__)) remquo(double3 x, double3 y,  int3 *quo );
double3 __attribute__((__overloadable__,__always_inline__)) remainder(double3 x, double3 y);
#endif
double4 __attribute__((__overloadable__,__always_inline__)) remquo(double4 x, double4 y,  int4 *quo );
double4 __attribute__((__overloadable__,__always_inline__)) remainder(double4 x, double4 y);
double8 __attribute__((__overloadable__,__always_inline__)) remquo(double8 x, double8 y,  int8 *quo );
double8 __attribute__((__overloadable__,__always_inline__)) remainder(double8 x, double8 y);
double16 __attribute__((__overloadable__,__always_inline__)) remquo(double16 x, double16 y,  int16 *quo );
double16 __attribute__((__overloadable__,__always_inline__)) remainder(double16 x, double16 y);
double __attribute__((__overloadable__,__always_inline__)) remquo(double x, double y, global int *quo );
double2 __attribute__((__overloadable__,__always_inline__)) remquo(double2 x, double2 y, global int2 *quo );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__,__always_inline__)) remquo(double3 x, double3 y, global int3 *quo );
#endif
double4 __attribute__((__overloadable__,__always_inline__)) remquo(double4 x, double4 y, global int4 *quo );
double8 __attribute__((__overloadable__,__always_inline__)) remquo(double8 x, double8 y, global int8 *quo );
double16 __attribute__((__overloadable__,__always_inline__)) remquo(double16 x, double16 y, global int16 *quo );
double __attribute__((__overloadable__,__always_inline__)) remquo(double x, double y, local int *quo );
double2 __attribute__((__overloadable__,__always_inline__)) remquo(double2 x, double2 y, local int2 *quo );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__,__always_inline__)) remquo(double3 x, double3 y, local int3 *quo );
#endif
double4 __attribute__((__overloadable__,__always_inline__)) remquo(double4 x, double4 y, local int4 *quo );
double8 __attribute__((__overloadable__,__always_inline__)) remquo(double8 x, double8 y, local int8 *quo );
double16 __attribute__((__overloadable__,__always_inline__)) remquo(double16 x, double16 y, local int16 *quo );
float __attribute__((__overloadable__,__always_inline__)) fmod(float __x, float __y);
float2 __attribute__((__overloadable__,__always_inline__)) fmod(float2 x, float2 y);
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__,__always_inline__)) fmod(float3 x, float3 y);
#endif
float4 __attribute__((__overloadable__,__always_inline__)) fmod(float4 x, float4 y);
float8 __attribute__((__overloadable__,__always_inline__)) fmod(float8 x, float8 y);
float16 __attribute__((__overloadable__,__always_inline__)) fmod(float16 x, float16 y);
double __attribute__((__overloadable__,__always_inline__)) fmod(double __x, double __y);
double2 __attribute__((__overloadable__,__always_inline__)) fmod(double2 x, double2 y);
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__,__always_inline__)) fmod(double3 x, double3 y);
#endif
double4 __attribute__((__overloadable__,__always_inline__)) fmod(double4 x, double4 y);
double8 __attribute__((__overloadable__,__always_inline__)) fmod(double8 x, double8 y);
double16 __attribute__((__overloadable__,__always_inline__)) fmod(double16 x, double16 y);
#if __OPENCL_C_VERSION__ >= 110
#define __CVAttrs __attribute__((__overloadable__,__always_inline__,const))
char2 __CVAttrs shuffle(char2 _x, uchar2 _m);
char2 __CVAttrs shuffle2(char2 _x, char2 _y, uchar2 _m);
char4 __CVAttrs shuffle(char2 _x, uchar4 _m);
char4 __CVAttrs shuffle2(char2 _x, char2 _y, uchar4 _m);
char8 __CVAttrs shuffle(char2 _x, uchar8 _m);
char8 __CVAttrs shuffle2(char2 _x, char2 _y, uchar8 _m);
char16 __CVAttrs shuffle(char2 _x, uchar16 _m);
char16 __CVAttrs shuffle2(char2 _x, char2 _y, uchar16 _m);
char2 __CVAttrs shuffle(char4 _x, uchar2 _m);
char2 __CVAttrs shuffle2(char4 _x, char4 _y, uchar2 _m);
char4 __CVAttrs shuffle(char4 _x, uchar4 _m);
char4 __CVAttrs shuffle2(char4 _x, char4 _y, uchar4 _m);
char8 __CVAttrs shuffle(char4 _x, uchar8 _m);
char8 __CVAttrs shuffle2(char4 _x, char4 _y, uchar8 _m);
char16 __CVAttrs shuffle(char4 _x, uchar16 _m);
char16 __CVAttrs shuffle2(char4 _x, char4 _y, uchar16 _m);
char2 __CVAttrs shuffle(char8 _x, uchar2 _m);
char2 __CVAttrs shuffle2(char8 _x, char8 _y, uchar2 _m);
char4 __CVAttrs shuffle(char8 _x, uchar4 _m);
char4 __CVAttrs shuffle2(char8 _x, char8 _y, uchar4 _m);
char8 __CVAttrs shuffle(char8 _x, uchar8 _m);
char8 __CVAttrs shuffle2(char8 _x, char8 _y, uchar8 _m);
char16 __CVAttrs shuffle(char8 _x, uchar16 _m);
char16 __CVAttrs shuffle2(char8 _x, char8 _y, uchar16 _m);
char2 __CVAttrs shuffle(char16 _x, uchar2 _m);
char2 __CVAttrs shuffle2(char16 _x, char16 _y, uchar2 _m);
char4 __CVAttrs shuffle(char16 _x, uchar4 _m);
char4 __CVAttrs shuffle2(char16 _x, char16 _y, uchar4 _m);
char8 __CVAttrs shuffle(char16 _x, uchar8 _m);
char8 __CVAttrs shuffle2(char16 _x, char16 _y, uchar8 _m);
char16 __CVAttrs shuffle(char16 _x, uchar16 _m);
char16 __CVAttrs shuffle2(char16 _x, char16 _y, uchar16 _m);
uchar2 __CVAttrs shuffle(uchar2 _x, uchar2 _m);
uchar2 __CVAttrs shuffle2(uchar2 _x, uchar2 _y, uchar2 _m);
uchar4 __CVAttrs shuffle(uchar2 _x, uchar4 _m);
uchar4 __CVAttrs shuffle2(uchar2 _x, uchar2 _y, uchar4 _m);
uchar8 __CVAttrs shuffle(uchar2 _x, uchar8 _m);
uchar8 __CVAttrs shuffle2(uchar2 _x, uchar2 _y, uchar8 _m);
uchar16 __CVAttrs shuffle(uchar2 _x, uchar16 _m);
uchar16 __CVAttrs shuffle2(uchar2 _x, uchar2 _y, uchar16 _m);
uchar2 __CVAttrs shuffle(uchar4 _x, uchar2 _m);
uchar2 __CVAttrs shuffle2(uchar4 _x, uchar4 _y, uchar2 _m);
uchar4 __CVAttrs shuffle(uchar4 _x, uchar4 _m);
uchar4 __CVAttrs shuffle2(uchar4 _x, uchar4 _y, uchar4 _m);
uchar8 __CVAttrs shuffle(uchar4 _x, uchar8 _m);
uchar8 __CVAttrs shuffle2(uchar4 _x, uchar4 _y, uchar8 _m);
uchar16 __CVAttrs shuffle(uchar4 _x, uchar16 _m);
uchar16 __CVAttrs shuffle2(uchar4 _x, uchar4 _y, uchar16 _m);
uchar2 __CVAttrs shuffle(uchar8 _x, uchar2 _m);
uchar2 __CVAttrs shuffle2(uchar8 _x, uchar8 _y, uchar2 _m);
uchar4 __CVAttrs shuffle(uchar8 _x, uchar4 _m);
uchar4 __CVAttrs shuffle2(uchar8 _x, uchar8 _y, uchar4 _m);
uchar8 __CVAttrs shuffle(uchar8 _x, uchar8 _m);
uchar8 __CVAttrs shuffle2(uchar8 _x, uchar8 _y, uchar8 _m);
uchar16 __CVAttrs shuffle(uchar8 _x, uchar16 _m);
uchar16 __CVAttrs shuffle2(uchar8 _x, uchar8 _y, uchar16 _m);
uchar2 __CVAttrs shuffle(uchar16 _x, uchar2 _m);
uchar2 __CVAttrs shuffle2(uchar16 _x, uchar16 _y, uchar2 _m);
uchar4 __CVAttrs shuffle(uchar16 _x, uchar4 _m);
uchar4 __CVAttrs shuffle2(uchar16 _x, uchar16 _y, uchar4 _m);
uchar8 __CVAttrs shuffle(uchar16 _x, uchar8 _m);
uchar8 __CVAttrs shuffle2(uchar16 _x, uchar16 _y, uchar8 _m);
uchar16 __CVAttrs shuffle(uchar16 _x, uchar16 _m);
uchar16 __CVAttrs shuffle2(uchar16 _x, uchar16 _y, uchar16 _m);
short2 __CVAttrs shuffle(short2 _x, ushort2 _m);
short2 __CVAttrs shuffle2(short2 _x, short2 _y, ushort2 _m);
short4 __CVAttrs shuffle(short2 _x, ushort4 _m);
short4 __CVAttrs shuffle2(short2 _x, short2 _y, ushort4 _m);
short8 __CVAttrs shuffle(short2 _x, ushort8 _m);
short8 __CVAttrs shuffle2(short2 _x, short2 _y, ushort8 _m);
short16 __CVAttrs shuffle(short2 _x, ushort16 _m);
short16 __CVAttrs shuffle2(short2 _x, short2 _y, ushort16 _m);
short2 __CVAttrs shuffle(short4 _x, ushort2 _m);
short2 __CVAttrs shuffle2(short4 _x, short4 _y, ushort2 _m);
short4 __CVAttrs shuffle(short4 _x, ushort4 _m);
short4 __CVAttrs shuffle2(short4 _x, short4 _y, ushort4 _m);
short8 __CVAttrs shuffle(short4 _x, ushort8 _m);
short8 __CVAttrs shuffle2(short4 _x, short4 _y, ushort8 _m);
short16 __CVAttrs shuffle(short4 _x, ushort16 _m);
short16 __CVAttrs shuffle2(short4 _x, short4 _y, ushort16 _m);
short2 __CVAttrs shuffle(short8 _x, ushort2 _m);
short2 __CVAttrs shuffle2(short8 _x, short8 _y, ushort2 _m);
short4 __CVAttrs shuffle(short8 _x, ushort4 _m);
short4 __CVAttrs shuffle2(short8 _x, short8 _y, ushort4 _m);
short8 __CVAttrs shuffle(short8 _x, ushort8 _m);
short8 __CVAttrs shuffle2(short8 _x, short8 _y, ushort8 _m);
short16 __CVAttrs shuffle(short8 _x, ushort16 _m);
short16 __CVAttrs shuffle2(short8 _x, short8 _y, ushort16 _m);
short2 __CVAttrs shuffle(short16 _x, ushort2 _m);
short2 __CVAttrs shuffle2(short16 _x, short16 _y, ushort2 _m);
short4 __CVAttrs shuffle(short16 _x, ushort4 _m);
short4 __CVAttrs shuffle2(short16 _x, short16 _y, ushort4 _m);
short8 __CVAttrs shuffle(short16 _x, ushort8 _m);
short8 __CVAttrs shuffle2(short16 _x, short16 _y, ushort8 _m);
short16 __CVAttrs shuffle(short16 _x, ushort16 _m);
short16 __CVAttrs shuffle2(short16 _x, short16 _y, ushort16 _m);
ushort2 __CVAttrs shuffle(ushort2 _x, ushort2 _m);
ushort2 __CVAttrs shuffle2(ushort2 _x, ushort2 _y, ushort2 _m);
ushort4 __CVAttrs shuffle(ushort2 _x, ushort4 _m);
ushort4 __CVAttrs shuffle2(ushort2 _x, ushort2 _y, ushort4 _m);
ushort8 __CVAttrs shuffle(ushort2 _x, ushort8 _m);
ushort8 __CVAttrs shuffle2(ushort2 _x, ushort2 _y, ushort8 _m);
ushort16 __CVAttrs shuffle(ushort2 _x, ushort16 _m);
ushort16 __CVAttrs shuffle2(ushort2 _x, ushort2 _y, ushort16 _m);
ushort2 __CVAttrs shuffle(ushort4 _x, ushort2 _m);
ushort2 __CVAttrs shuffle2(ushort4 _x, ushort4 _y, ushort2 _m);
ushort4 __CVAttrs shuffle(ushort4 _x, ushort4 _m);
ushort4 __CVAttrs shuffle2(ushort4 _x, ushort4 _y, ushort4 _m);
ushort8 __CVAttrs shuffle(ushort4 _x, ushort8 _m);
ushort8 __CVAttrs shuffle2(ushort4 _x, ushort4 _y, ushort8 _m);
ushort16 __CVAttrs shuffle(ushort4 _x, ushort16 _m);
ushort16 __CVAttrs shuffle2(ushort4 _x, ushort4 _y, ushort16 _m);
ushort2 __CVAttrs shuffle(ushort8 _x, ushort2 _m);
ushort2 __CVAttrs shuffle2(ushort8 _x, ushort8 _y, ushort2 _m);
ushort4 __CVAttrs shuffle(ushort8 _x, ushort4 _m);
ushort4 __CVAttrs shuffle2(ushort8 _x, ushort8 _y, ushort4 _m);
ushort8 __CVAttrs shuffle(ushort8 _x, ushort8 _m);
ushort8 __CVAttrs shuffle2(ushort8 _x, ushort8 _y, ushort8 _m);
ushort16 __CVAttrs shuffle(ushort8 _x, ushort16 _m);
ushort16 __CVAttrs shuffle2(ushort8 _x, ushort8 _y, ushort16 _m);
ushort2 __CVAttrs shuffle(ushort16 _x, ushort2 _m);
ushort2 __CVAttrs shuffle2(ushort16 _x, ushort16 _y, ushort2 _m);
ushort4 __CVAttrs shuffle(ushort16 _x, ushort4 _m);
ushort4 __CVAttrs shuffle2(ushort16 _x, ushort16 _y, ushort4 _m);
ushort8 __CVAttrs shuffle(ushort16 _x, ushort8 _m);
ushort8 __CVAttrs shuffle2(ushort16 _x, ushort16 _y, ushort8 _m);
ushort16 __CVAttrs shuffle(ushort16 _x, ushort16 _m);
ushort16 __CVAttrs shuffle2(ushort16 _x, ushort16 _y, ushort16 _m);
int2 __CVAttrs shuffle(int2 _x, uint2 _m);
int2 __CVAttrs shuffle2(int2 _x, int2 _y, uint2 _m);
int4 __CVAttrs shuffle(int2 _x, uint4 _m);
int4 __CVAttrs shuffle2(int2 _x, int2 _y, uint4 _m);
int8 __CVAttrs shuffle(int2 _x, uint8 _m);
int8 __CVAttrs shuffle2(int2 _x, int2 _y, uint8 _m);
int16 __CVAttrs shuffle(int2 _x, uint16 _m);
int16 __CVAttrs shuffle2(int2 _x, int2 _y, uint16 _m);
int2 __CVAttrs shuffle(int4 _x, uint2 _m);
int2 __CVAttrs shuffle2(int4 _x, int4 _y, uint2 _m);
int4 __CVAttrs shuffle(int4 _x, uint4 _m);
int4 __CVAttrs shuffle2(int4 _x, int4 _y, uint4 _m);
int8 __CVAttrs shuffle(int4 _x, uint8 _m);
int8 __CVAttrs shuffle2(int4 _x, int4 _y, uint8 _m);
int16 __CVAttrs shuffle(int4 _x, uint16 _m);
int16 __CVAttrs shuffle2(int4 _x, int4 _y, uint16 _m);
int2 __CVAttrs shuffle(int8 _x, uint2 _m);
int2 __CVAttrs shuffle2(int8 _x, int8 _y, uint2 _m);
int4 __CVAttrs shuffle(int8 _x, uint4 _m);
int4 __CVAttrs shuffle2(int8 _x, int8 _y, uint4 _m);
int8 __CVAttrs shuffle(int8 _x, uint8 _m);
int8 __CVAttrs shuffle2(int8 _x, int8 _y, uint8 _m);
int16 __CVAttrs shuffle(int8 _x, uint16 _m);
int16 __CVAttrs shuffle2(int8 _x, int8 _y, uint16 _m);
int2 __CVAttrs shuffle(int16 _x, uint2 _m);
int2 __CVAttrs shuffle2(int16 _x, int16 _y, uint2 _m);
int4 __CVAttrs shuffle(int16 _x, uint4 _m);
int4 __CVAttrs shuffle2(int16 _x, int16 _y, uint4 _m);
int8 __CVAttrs shuffle(int16 _x, uint8 _m);
int8 __CVAttrs shuffle2(int16 _x, int16 _y, uint8 _m);
int16 __CVAttrs shuffle(int16 _x, uint16 _m);
int16 __CVAttrs shuffle2(int16 _x, int16 _y, uint16 _m);
uint2 __CVAttrs shuffle(uint2 _x, uint2 _m);
uint2 __CVAttrs shuffle2(uint2 _x, uint2 _y, uint2 _m);
uint4 __CVAttrs shuffle(uint2 _x, uint4 _m);
uint4 __CVAttrs shuffle2(uint2 _x, uint2 _y, uint4 _m);
uint8 __CVAttrs shuffle(uint2 _x, uint8 _m);
uint8 __CVAttrs shuffle2(uint2 _x, uint2 _y, uint8 _m);
uint16 __CVAttrs shuffle(uint2 _x, uint16 _m);
uint16 __CVAttrs shuffle2(uint2 _x, uint2 _y, uint16 _m);
uint2 __CVAttrs shuffle(uint4 _x, uint2 _m);
uint2 __CVAttrs shuffle2(uint4 _x, uint4 _y, uint2 _m);
uint4 __CVAttrs shuffle(uint4 _x, uint4 _m);
uint4 __CVAttrs shuffle2(uint4 _x, uint4 _y, uint4 _m);
uint8 __CVAttrs shuffle(uint4 _x, uint8 _m);
uint8 __CVAttrs shuffle2(uint4 _x, uint4 _y, uint8 _m);
uint16 __CVAttrs shuffle(uint4 _x, uint16 _m);
uint16 __CVAttrs shuffle2(uint4 _x, uint4 _y, uint16 _m);
uint2 __CVAttrs shuffle(uint8 _x, uint2 _m);
uint2 __CVAttrs shuffle2(uint8 _x, uint8 _y, uint2 _m);
uint4 __CVAttrs shuffle(uint8 _x, uint4 _m);
uint4 __CVAttrs shuffle2(uint8 _x, uint8 _y, uint4 _m);
uint8 __CVAttrs shuffle(uint8 _x, uint8 _m);
uint8 __CVAttrs shuffle2(uint8 _x, uint8 _y, uint8 _m);
uint16 __CVAttrs shuffle(uint8 _x, uint16 _m);
uint16 __CVAttrs shuffle2(uint8 _x, uint8 _y, uint16 _m);
uint2 __CVAttrs shuffle(uint16 _x, uint2 _m);
uint2 __CVAttrs shuffle2(uint16 _x, uint16 _y, uint2 _m);
uint4 __CVAttrs shuffle(uint16 _x, uint4 _m);
uint4 __CVAttrs shuffle2(uint16 _x, uint16 _y, uint4 _m);
uint8 __CVAttrs shuffle(uint16 _x, uint8 _m);
uint8 __CVAttrs shuffle2(uint16 _x, uint16 _y, uint8 _m);
uint16 __CVAttrs shuffle(uint16 _x, uint16 _m);
uint16 __CVAttrs shuffle2(uint16 _x, uint16 _y, uint16 _m);
long2 __CVAttrs shuffle(long2 _x, ulong2 _m);
long2 __CVAttrs shuffle2(long2 _x, long2 _y, ulong2 _m);
long4 __CVAttrs shuffle(long2 _x, ulong4 _m);
long4 __CVAttrs shuffle2(long2 _x, long2 _y, ulong4 _m);
long8 __CVAttrs shuffle(long2 _x, ulong8 _m);
long8 __CVAttrs shuffle2(long2 _x, long2 _y, ulong8 _m);
long16 __CVAttrs shuffle(long2 _x, ulong16 _m);
long16 __CVAttrs shuffle2(long2 _x, long2 _y, ulong16 _m);
long2 __CVAttrs shuffle(long4 _x, ulong2 _m);
long2 __CVAttrs shuffle2(long4 _x, long4 _y, ulong2 _m);
long4 __CVAttrs shuffle(long4 _x, ulong4 _m);
long4 __CVAttrs shuffle2(long4 _x, long4 _y, ulong4 _m);
long8 __CVAttrs shuffle(long4 _x, ulong8 _m);
long8 __CVAttrs shuffle2(long4 _x, long4 _y, ulong8 _m);
long16 __CVAttrs shuffle(long4 _x, ulong16 _m);
long16 __CVAttrs shuffle2(long4 _x, long4 _y, ulong16 _m);
long2 __CVAttrs shuffle(long8 _x, ulong2 _m);
long2 __CVAttrs shuffle2(long8 _x, long8 _y, ulong2 _m);
long4 __CVAttrs shuffle(long8 _x, ulong4 _m);
long4 __CVAttrs shuffle2(long8 _x, long8 _y, ulong4 _m);
long8 __CVAttrs shuffle(long8 _x, ulong8 _m);
long8 __CVAttrs shuffle2(long8 _x, long8 _y, ulong8 _m);
long16 __CVAttrs shuffle(long8 _x, ulong16 _m);
long16 __CVAttrs shuffle2(long8 _x, long8 _y, ulong16 _m);
long2 __CVAttrs shuffle(long16 _x, ulong2 _m);
long2 __CVAttrs shuffle2(long16 _x, long16 _y, ulong2 _m);
long4 __CVAttrs shuffle(long16 _x, ulong4 _m);
long4 __CVAttrs shuffle2(long16 _x, long16 _y, ulong4 _m);
long8 __CVAttrs shuffle(long16 _x, ulong8 _m);
long8 __CVAttrs shuffle2(long16 _x, long16 _y, ulong8 _m);
long16 __CVAttrs shuffle(long16 _x, ulong16 _m);
long16 __CVAttrs shuffle2(long16 _x, long16 _y, ulong16 _m);
ulong2 __CVAttrs shuffle(ulong2 _x, ulong2 _m);
ulong2 __CVAttrs shuffle2(ulong2 _x, ulong2 _y, ulong2 _m);
ulong4 __CVAttrs shuffle(ulong2 _x, ulong4 _m);
ulong4 __CVAttrs shuffle2(ulong2 _x, ulong2 _y, ulong4 _m);
ulong8 __CVAttrs shuffle(ulong2 _x, ulong8 _m);
ulong8 __CVAttrs shuffle2(ulong2 _x, ulong2 _y, ulong8 _m);
ulong16 __CVAttrs shuffle(ulong2 _x, ulong16 _m);
ulong16 __CVAttrs shuffle2(ulong2 _x, ulong2 _y, ulong16 _m);
ulong2 __CVAttrs shuffle(ulong4 _x, ulong2 _m);
ulong2 __CVAttrs shuffle2(ulong4 _x, ulong4 _y, ulong2 _m);
ulong4 __CVAttrs shuffle(ulong4 _x, ulong4 _m);
ulong4 __CVAttrs shuffle2(ulong4 _x, ulong4 _y, ulong4 _m);
ulong8 __CVAttrs shuffle(ulong4 _x, ulong8 _m);
ulong8 __CVAttrs shuffle2(ulong4 _x, ulong4 _y, ulong8 _m);
ulong16 __CVAttrs shuffle(ulong4 _x, ulong16 _m);
ulong16 __CVAttrs shuffle2(ulong4 _x, ulong4 _y, ulong16 _m);
ulong2 __CVAttrs shuffle(ulong8 _x, ulong2 _m);
ulong2 __CVAttrs shuffle2(ulong8 _x, ulong8 _y, ulong2 _m);
ulong4 __CVAttrs shuffle(ulong8 _x, ulong4 _m);
ulong4 __CVAttrs shuffle2(ulong8 _x, ulong8 _y, ulong4 _m);
ulong8 __CVAttrs shuffle(ulong8 _x, ulong8 _m);
ulong8 __CVAttrs shuffle2(ulong8 _x, ulong8 _y, ulong8 _m);
ulong16 __CVAttrs shuffle(ulong8 _x, ulong16 _m);
ulong16 __CVAttrs shuffle2(ulong8 _x, ulong8 _y, ulong16 _m);
ulong2 __CVAttrs shuffle(ulong16 _x, ulong2 _m);
ulong2 __CVAttrs shuffle2(ulong16 _x, ulong16 _y, ulong2 _m);
ulong4 __CVAttrs shuffle(ulong16 _x, ulong4 _m);
ulong4 __CVAttrs shuffle2(ulong16 _x, ulong16 _y, ulong4 _m);
ulong8 __CVAttrs shuffle(ulong16 _x, ulong8 _m);
ulong8 __CVAttrs shuffle2(ulong16 _x, ulong16 _y, ulong8 _m);
ulong16 __CVAttrs shuffle(ulong16 _x, ulong16 _m);
ulong16 __CVAttrs shuffle2(ulong16 _x, ulong16 _y, ulong16 _m);
float2 __CVAttrs shuffle(float2 _x, uint2 _m);
float2 __CVAttrs shuffle2(float2 _x, float2 _y, uint2 _m);
float4 __CVAttrs shuffle(float2 _x, uint4 _m);
float4 __CVAttrs shuffle2(float2 _x, float2 _y, uint4 _m);
float8 __CVAttrs shuffle(float2 _x, uint8 _m);
float8 __CVAttrs shuffle2(float2 _x, float2 _y, uint8 _m);
float16 __CVAttrs shuffle(float2 _x, uint16 _m);
float16 __CVAttrs shuffle2(float2 _x, float2 _y, uint16 _m);
float2 __CVAttrs shuffle(float4 _x, uint2 _m);
float2 __CVAttrs shuffle2(float4 _x, float4 _y, uint2 _m);
float4 __CVAttrs shuffle(float4 _x, uint4 _m);
float4 __CVAttrs shuffle2(float4 _x, float4 _y, uint4 _m);
float8 __CVAttrs shuffle(float4 _x, uint8 _m);
float8 __CVAttrs shuffle2(float4 _x, float4 _y, uint8 _m);
float16 __CVAttrs shuffle(float4 _x, uint16 _m);
float16 __CVAttrs shuffle2(float4 _x, float4 _y, uint16 _m);
float2 __CVAttrs shuffle(float8 _x, uint2 _m);
float2 __CVAttrs shuffle2(float8 _x, float8 _y, uint2 _m);
float4 __CVAttrs shuffle(float8 _x, uint4 _m);
float4 __CVAttrs shuffle2(float8 _x, float8 _y, uint4 _m);
float8 __CVAttrs shuffle(float8 _x, uint8 _m);
float8 __CVAttrs shuffle2(float8 _x, float8 _y, uint8 _m);
float16 __CVAttrs shuffle(float8 _x, uint16 _m);
float16 __CVAttrs shuffle2(float8 _x, float8 _y, uint16 _m);
float2 __CVAttrs shuffle(float16 _x, uint2 _m);
float2 __CVAttrs shuffle2(float16 _x, float16 _y, uint2 _m);
float4 __CVAttrs shuffle(float16 _x, uint4 _m);
float4 __CVAttrs shuffle2(float16 _x, float16 _y, uint4 _m);
float8 __CVAttrs shuffle(float16 _x, uint8 _m);
float8 __CVAttrs shuffle2(float16 _x, float16 _y, uint8 _m);
float16 __CVAttrs shuffle(float16 _x, uint16 _m);
float16 __CVAttrs shuffle2(float16 _x, float16 _y, uint16 _m);
double2 __CVAttrs shuffle(double2 _x, ulong2 _m);
double2 __CVAttrs shuffle2(double2 _x, double2 _y, ulong2 _m);
double4 __CVAttrs shuffle(double2 _x, ulong4 _m);
double4 __CVAttrs shuffle2(double2 _x, double2 _y, ulong4 _m);
double8 __CVAttrs shuffle(double2 _x, ulong8 _m);
double8 __CVAttrs shuffle2(double2 _x, double2 _y, ulong8 _m);
double16 __CVAttrs shuffle(double2 _x, ulong16 _m);
double16 __CVAttrs shuffle2(double2 _x, double2 _y, ulong16 _m);
double2 __CVAttrs shuffle(double4 _x, ulong2 _m);
double2 __CVAttrs shuffle2(double4 _x, double4 _y, ulong2 _m);
double4 __CVAttrs shuffle(double4 _x, ulong4 _m);
double4 __CVAttrs shuffle2(double4 _x, double4 _y, ulong4 _m);
double8 __CVAttrs shuffle(double4 _x, ulong8 _m);
double8 __CVAttrs shuffle2(double4 _x, double4 _y, ulong8 _m);
double16 __CVAttrs shuffle(double4 _x, ulong16 _m);
double16 __CVAttrs shuffle2(double4 _x, double4 _y, ulong16 _m);
double2 __CVAttrs shuffle(double8 _x, ulong2 _m);
double2 __CVAttrs shuffle2(double8 _x, double8 _y, ulong2 _m);
double4 __CVAttrs shuffle(double8 _x, ulong4 _m);
double4 __CVAttrs shuffle2(double8 _x, double8 _y, ulong4 _m);
double8 __CVAttrs shuffle(double8 _x, ulong8 _m);
double8 __CVAttrs shuffle2(double8 _x, double8 _y, ulong8 _m);
double16 __CVAttrs shuffle(double8 _x, ulong16 _m);
double16 __CVAttrs shuffle2(double8 _x, double8 _y, ulong16 _m);
double2 __CVAttrs shuffle(double16 _x, ulong2 _m);
double2 __CVAttrs shuffle2(double16 _x, double16 _y, ulong2 _m);
double4 __CVAttrs shuffle(double16 _x, ulong4 _m);
double4 __CVAttrs shuffle2(double16 _x, double16 _y, ulong4 _m);
double8 __CVAttrs shuffle(double16 _x, ulong8 _m);
double8 __CVAttrs shuffle2(double16 _x, double16 _y, ulong8 _m);
double16 __CVAttrs shuffle(double16 _x, ulong16 _m);
double16 __CVAttrs shuffle2(double16 _x, double16 _y, ulong16 _m);
#undef __CVAttrs

#endif
char2 __attribute__((__overloadable__,pure,__always_inline__))  vload2( size_t offset, const global char* p);
char2 __attribute__((__overloadable__,pure,__always_inline__))  vload2( size_t offset, const local char* p);
char2 __attribute__((__overloadable__,pure,__always_inline__))  vload2( size_t offset, const constant char* p);
char2 __attribute__((__overloadable__,pure,__always_inline__))  vload2( size_t offset, const private char* p);
#if __OPENCL_C_VERSION__ >= 110
char3 __attribute__((__overloadable__,pure,__always_inline__))  vload3( size_t offset, const global char* p);
char3 __attribute__((__overloadable__,pure,__always_inline__))  vload3( size_t offset, const local char* p);
char3 __attribute__((__overloadable__,pure,__always_inline__))  vload3( size_t offset, const constant char* p);
char3 __attribute__((__overloadable__,pure,__always_inline__))  vload3( size_t offset, const private char* p);
#endif
char4 __attribute__((__overloadable__,pure,__always_inline__))  vload4( size_t offset, const global char* p);
char4 __attribute__((__overloadable__,pure,__always_inline__))  vload4( size_t offset, const local char* p);
char4 __attribute__((__overloadable__,pure,__always_inline__))  vload4( size_t offset, const constant char* p);
char4 __attribute__((__overloadable__,pure,__always_inline__))  vload4( size_t offset, const private char* p);
char8 __attribute__((__overloadable__,pure,__always_inline__))  vload8( size_t offset, const global char* p);
char8 __attribute__((__overloadable__,pure,__always_inline__))  vload8( size_t offset, const local char* p);
char8 __attribute__((__overloadable__,pure,__always_inline__))  vload8( size_t offset, const constant char* p);
char8 __attribute__((__overloadable__,pure,__always_inline__))  vload8( size_t offset, const private char* p);
char16 __attribute__((__overloadable__,pure,__always_inline__))  vload16( size_t offset, const global char* p);
char16 __attribute__((__overloadable__,pure,__always_inline__))  vload16( size_t offset, const local char* p);
char16 __attribute__((__overloadable__,pure,__always_inline__))  vload16( size_t offset, const constant char* p);
char16 __attribute__((__overloadable__,pure,__always_inline__))  vload16( size_t offset, const private char* p);
uchar2 __attribute__((__overloadable__,pure,__always_inline__))  vload2( size_t offset, const global uchar* p);
uchar2 __attribute__((__overloadable__,pure,__always_inline__))  vload2( size_t offset, const local uchar* p);
uchar2 __attribute__((__overloadable__,pure,__always_inline__))  vload2( size_t offset, const constant uchar* p);
uchar2 __attribute__((__overloadable__,pure,__always_inline__))  vload2( size_t offset, const private uchar* p);
#if __OPENCL_C_VERSION__ >= 110
uchar3 __attribute__((__overloadable__,pure,__always_inline__))  vload3( size_t offset, const global uchar* p);
uchar3 __attribute__((__overloadable__,pure,__always_inline__))  vload3( size_t offset, const local uchar* p);
uchar3 __attribute__((__overloadable__,pure,__always_inline__))  vload3( size_t offset, const constant uchar* p);
uchar3 __attribute__((__overloadable__,pure,__always_inline__))  vload3( size_t offset, const private uchar* p);
#endif
uchar4 __attribute__((__overloadable__,pure,__always_inline__))  vload4( size_t offset, const global uchar* p);
uchar4 __attribute__((__overloadable__,pure,__always_inline__))  vload4( size_t offset, const local uchar* p);
uchar4 __attribute__((__overloadable__,pure,__always_inline__))  vload4( size_t offset, const constant uchar* p);
uchar4 __attribute__((__overloadable__,pure,__always_inline__))  vload4( size_t offset, const private uchar* p);
uchar8 __attribute__((__overloadable__,pure,__always_inline__))  vload8( size_t offset, const global uchar* p);
uchar8 __attribute__((__overloadable__,pure,__always_inline__))  vload8( size_t offset, const local uchar* p);
uchar8 __attribute__((__overloadable__,pure,__always_inline__))  vload8( size_t offset, const constant uchar* p);
uchar8 __attribute__((__overloadable__,pure,__always_inline__))  vload8( size_t offset, const private uchar* p);
uchar16 __attribute__((__overloadable__,pure,__always_inline__))  vload16( size_t offset, const global uchar* p);
uchar16 __attribute__((__overloadable__,pure,__always_inline__))  vload16( size_t offset, const local uchar* p);
uchar16 __attribute__((__overloadable__,pure,__always_inline__))  vload16( size_t offset, const constant uchar* p);
uchar16 __attribute__((__overloadable__,pure,__always_inline__))  vload16( size_t offset, const private uchar* p);
short2 __attribute__((__overloadable__,pure,__always_inline__))  vload2( size_t offset, const global short* p);
short2 __attribute__((__overloadable__,pure,__always_inline__))  vload2( size_t offset, const local short* p);
short2 __attribute__((__overloadable__,pure,__always_inline__))  vload2( size_t offset, const constant short* p);
short2 __attribute__((__overloadable__,pure,__always_inline__))  vload2( size_t offset, const private short* p);
#if __OPENCL_C_VERSION__ >= 110
short3 __attribute__((__overloadable__,pure,__always_inline__))  vload3( size_t offset, const global short* p);
short3 __attribute__((__overloadable__,pure,__always_inline__))  vload3( size_t offset, const local short* p);
short3 __attribute__((__overloadable__,pure,__always_inline__))  vload3( size_t offset, const constant short* p);
short3 __attribute__((__overloadable__,pure,__always_inline__))  vload3( size_t offset, const private short* p);
#endif
short4 __attribute__((__overloadable__,pure,__always_inline__))  vload4( size_t offset, const global short* p);
short4 __attribute__((__overloadable__,pure,__always_inline__))  vload4( size_t offset, const local short* p);
short4 __attribute__((__overloadable__,pure,__always_inline__))  vload4( size_t offset, const constant short* p);
short4 __attribute__((__overloadable__,pure,__always_inline__))  vload4( size_t offset, const private short* p);
short8 __attribute__((__overloadable__,pure,__always_inline__))  vload8( size_t offset, const global short* p);
short8 __attribute__((__overloadable__,pure,__always_inline__))  vload8( size_t offset, const local short* p);
short8 __attribute__((__overloadable__,pure,__always_inline__))  vload8( size_t offset, const constant short* p);
short8 __attribute__((__overloadable__,pure,__always_inline__))  vload8( size_t offset, const private short* p);
short16 __attribute__((__overloadable__,pure,__always_inline__))  vload16( size_t offset, const global short* p);
short16 __attribute__((__overloadable__,pure,__always_inline__))  vload16( size_t offset, const local short* p);
short16 __attribute__((__overloadable__,pure,__always_inline__))  vload16( size_t offset, const constant short* p);
short16 __attribute__((__overloadable__,pure,__always_inline__))  vload16( size_t offset, const private short* p);
ushort2 __attribute__((__overloadable__,pure,__always_inline__))  vload2( size_t offset, const global ushort* p);
ushort2 __attribute__((__overloadable__,pure,__always_inline__))  vload2( size_t offset, const local ushort* p);
ushort2 __attribute__((__overloadable__,pure,__always_inline__))  vload2( size_t offset, const constant ushort* p);
ushort2 __attribute__((__overloadable__,pure,__always_inline__))  vload2( size_t offset, const private ushort* p);
#if __OPENCL_C_VERSION__ >= 110
ushort3 __attribute__((__overloadable__,pure,__always_inline__))  vload3( size_t offset, const global ushort* p);
ushort3 __attribute__((__overloadable__,pure,__always_inline__))  vload3( size_t offset, const local ushort* p);
ushort3 __attribute__((__overloadable__,pure,__always_inline__))  vload3( size_t offset, const constant ushort* p);
ushort3 __attribute__((__overloadable__,pure,__always_inline__))  vload3( size_t offset, const private ushort* p);
#endif
ushort4 __attribute__((__overloadable__,pure,__always_inline__))  vload4( size_t offset, const global ushort* p);
ushort4 __attribute__((__overloadable__,pure,__always_inline__))  vload4( size_t offset, const local ushort* p);
ushort4 __attribute__((__overloadable__,pure,__always_inline__))  vload4( size_t offset, const constant ushort* p);
ushort4 __attribute__((__overloadable__,pure,__always_inline__))  vload4( size_t offset, const private ushort* p);
ushort8 __attribute__((__overloadable__,pure,__always_inline__))  vload8( size_t offset, const global ushort* p);
ushort8 __attribute__((__overloadable__,pure,__always_inline__))  vload8( size_t offset, const local ushort* p);
ushort8 __attribute__((__overloadable__,pure,__always_inline__))  vload8( size_t offset, const constant ushort* p);
ushort8 __attribute__((__overloadable__,pure,__always_inline__))  vload8( size_t offset, const private ushort* p);
ushort16 __attribute__((__overloadable__,pure,__always_inline__))  vload16( size_t offset, const global ushort* p);
ushort16 __attribute__((__overloadable__,pure,__always_inline__))  vload16( size_t offset, const local ushort* p);
ushort16 __attribute__((__overloadable__,pure,__always_inline__))  vload16( size_t offset, const constant ushort* p);
ushort16 __attribute__((__overloadable__,pure,__always_inline__))  vload16( size_t offset, const private ushort* p);
int2 __attribute__((__overloadable__,pure,__always_inline__))  vload2( size_t offset, const global int* p);
int2 __attribute__((__overloadable__,pure,__always_inline__))  vload2( size_t offset, const local int* p);
int2 __attribute__((__overloadable__,pure,__always_inline__))  vload2( size_t offset, const constant int* p);
int2 __attribute__((__overloadable__,pure,__always_inline__))  vload2( size_t offset, const private int* p);
#if __OPENCL_C_VERSION__ >= 110
int3 __attribute__((__overloadable__,pure,__always_inline__))  vload3( size_t offset, const global int* p);
int3 __attribute__((__overloadable__,pure,__always_inline__))  vload3( size_t offset, const local int* p);
int3 __attribute__((__overloadable__,pure,__always_inline__))  vload3( size_t offset, const constant int* p);
int3 __attribute__((__overloadable__,pure,__always_inline__))  vload3( size_t offset, const private int* p);
#endif
int4 __attribute__((__overloadable__,pure,__always_inline__))  vload4( size_t offset, const global int* p);
int4 __attribute__((__overloadable__,pure,__always_inline__))  vload4( size_t offset, const local int* p);
int4 __attribute__((__overloadable__,pure,__always_inline__))  vload4( size_t offset, const constant int* p);
int4 __attribute__((__overloadable__,pure,__always_inline__))  vload4( size_t offset, const private int* p);
int8 __attribute__((__overloadable__,pure,__always_inline__))  vload8( size_t offset, const global int* p);
int8 __attribute__((__overloadable__,pure,__always_inline__))  vload8( size_t offset, const local int* p);
int8 __attribute__((__overloadable__,pure,__always_inline__))  vload8( size_t offset, const constant int* p);
int8 __attribute__((__overloadable__,pure,__always_inline__))  vload8( size_t offset, const private int* p);
int16 __attribute__((__overloadable__,pure,__always_inline__))  vload16( size_t offset, const global int* p);
int16 __attribute__((__overloadable__,pure,__always_inline__))  vload16( size_t offset, const local int* p);
int16 __attribute__((__overloadable__,pure,__always_inline__))  vload16( size_t offset, const constant int* p);
int16 __attribute__((__overloadable__,pure,__always_inline__))  vload16( size_t offset, const private int* p);
uint2 __attribute__((__overloadable__,pure,__always_inline__))  vload2( size_t offset, const global uint* p);
uint2 __attribute__((__overloadable__,pure,__always_inline__))  vload2( size_t offset, const local uint* p);
uint2 __attribute__((__overloadable__,pure,__always_inline__))  vload2( size_t offset, const constant uint* p);
uint2 __attribute__((__overloadable__,pure,__always_inline__))  vload2( size_t offset, const private uint* p);
#if __OPENCL_C_VERSION__ >= 110
uint3 __attribute__((__overloadable__,pure,__always_inline__))  vload3( size_t offset, const global uint* p);
uint3 __attribute__((__overloadable__,pure,__always_inline__))  vload3( size_t offset, const local uint* p);
uint3 __attribute__((__overloadable__,pure,__always_inline__))  vload3( size_t offset, const constant uint* p);
uint3 __attribute__((__overloadable__,pure,__always_inline__))  vload3( size_t offset, const private uint* p);
#endif
uint4 __attribute__((__overloadable__,pure,__always_inline__))  vload4( size_t offset, const global uint* p);
uint4 __attribute__((__overloadable__,pure,__always_inline__))  vload4( size_t offset, const local uint* p);
uint4 __attribute__((__overloadable__,pure,__always_inline__))  vload4( size_t offset, const constant uint* p);
uint4 __attribute__((__overloadable__,pure,__always_inline__))  vload4( size_t offset, const private uint* p);
uint8 __attribute__((__overloadable__,pure,__always_inline__))  vload8( size_t offset, const global uint* p);
uint8 __attribute__((__overloadable__,pure,__always_inline__))  vload8( size_t offset, const local uint* p);
uint8 __attribute__((__overloadable__,pure,__always_inline__))  vload8( size_t offset, const constant uint* p);
uint8 __attribute__((__overloadable__,pure,__always_inline__))  vload8( size_t offset, const private uint* p);
uint16 __attribute__((__overloadable__,pure,__always_inline__))  vload16( size_t offset, const global uint* p);
uint16 __attribute__((__overloadable__,pure,__always_inline__))  vload16( size_t offset, const local uint* p);
uint16 __attribute__((__overloadable__,pure,__always_inline__))  vload16( size_t offset, const constant uint* p);
uint16 __attribute__((__overloadable__,pure,__always_inline__))  vload16( size_t offset, const private uint* p);
long2 __attribute__((__overloadable__,pure,__always_inline__))  vload2( size_t offset, const global long* p);
long2 __attribute__((__overloadable__,pure,__always_inline__))  vload2( size_t offset, const local long* p);
long2 __attribute__((__overloadable__,pure,__always_inline__))  vload2( size_t offset, const constant long* p);
long2 __attribute__((__overloadable__,pure,__always_inline__))  vload2( size_t offset, const private long* p);
#if __OPENCL_C_VERSION__ >= 110
long3 __attribute__((__overloadable__,pure,__always_inline__))  vload3( size_t offset, const global long* p);
long3 __attribute__((__overloadable__,pure,__always_inline__))  vload3( size_t offset, const local long* p);
long3 __attribute__((__overloadable__,pure,__always_inline__))  vload3( size_t offset, const constant long* p);
long3 __attribute__((__overloadable__,pure,__always_inline__))  vload3( size_t offset, const private long* p);
#endif
long4 __attribute__((__overloadable__,pure,__always_inline__))  vload4( size_t offset, const global long* p);
long4 __attribute__((__overloadable__,pure,__always_inline__))  vload4( size_t offset, const local long* p);
long4 __attribute__((__overloadable__,pure,__always_inline__))  vload4( size_t offset, const constant long* p);
long4 __attribute__((__overloadable__,pure,__always_inline__))  vload4( size_t offset, const private long* p);
long8 __attribute__((__overloadable__,pure,__always_inline__))  vload8( size_t offset, const global long* p);
long8 __attribute__((__overloadable__,pure,__always_inline__))  vload8( size_t offset, const local long* p);
long8 __attribute__((__overloadable__,pure,__always_inline__))  vload8( size_t offset, const constant long* p);
long8 __attribute__((__overloadable__,pure,__always_inline__))  vload8( size_t offset, const private long* p);
long16 __attribute__((__overloadable__,pure,__always_inline__))  vload16( size_t offset, const global long* p);
long16 __attribute__((__overloadable__,pure,__always_inline__))  vload16( size_t offset, const local long* p);
long16 __attribute__((__overloadable__,pure,__always_inline__))  vload16( size_t offset, const constant long* p);
long16 __attribute__((__overloadable__,pure,__always_inline__))  vload16( size_t offset, const private long* p);
ulong2 __attribute__((__overloadable__,pure,__always_inline__))  vload2( size_t offset, const global ulong* p);
ulong2 __attribute__((__overloadable__,pure,__always_inline__))  vload2( size_t offset, const local ulong* p);
ulong2 __attribute__((__overloadable__,pure,__always_inline__))  vload2( size_t offset, const constant ulong* p);
ulong2 __attribute__((__overloadable__,pure,__always_inline__))  vload2( size_t offset, const private ulong* p);
#if __OPENCL_C_VERSION__ >= 110
ulong3 __attribute__((__overloadable__,pure,__always_inline__))  vload3( size_t offset, const global ulong* p);
ulong3 __attribute__((__overloadable__,pure,__always_inline__))  vload3( size_t offset, const local ulong* p);
ulong3 __attribute__((__overloadable__,pure,__always_inline__))  vload3( size_t offset, const constant ulong* p);
ulong3 __attribute__((__overloadable__,pure,__always_inline__))  vload3( size_t offset, const private ulong* p);
#endif
ulong4 __attribute__((__overloadable__,pure,__always_inline__))  vload4( size_t offset, const global ulong* p);
ulong4 __attribute__((__overloadable__,pure,__always_inline__))  vload4( size_t offset, const local ulong* p);
ulong4 __attribute__((__overloadable__,pure,__always_inline__))  vload4( size_t offset, const constant ulong* p);
ulong4 __attribute__((__overloadable__,pure,__always_inline__))  vload4( size_t offset, const private ulong* p);
ulong8 __attribute__((__overloadable__,pure,__always_inline__))  vload8( size_t offset, const global ulong* p);
ulong8 __attribute__((__overloadable__,pure,__always_inline__))  vload8( size_t offset, const local ulong* p);
ulong8 __attribute__((__overloadable__,pure,__always_inline__))  vload8( size_t offset, const constant ulong* p);
ulong8 __attribute__((__overloadable__,pure,__always_inline__))  vload8( size_t offset, const private ulong* p);
ulong16 __attribute__((__overloadable__,pure,__always_inline__))  vload16( size_t offset, const global ulong* p);
ulong16 __attribute__((__overloadable__,pure,__always_inline__))  vload16( size_t offset, const local ulong* p);
ulong16 __attribute__((__overloadable__,pure,__always_inline__))  vload16( size_t offset, const constant ulong* p);
ulong16 __attribute__((__overloadable__,pure,__always_inline__))  vload16( size_t offset, const private ulong* p);
float2 __attribute__((__overloadable__,pure,__always_inline__))  vload2( size_t offset, const global float* p);
float2 __attribute__((__overloadable__,pure,__always_inline__))  vload2( size_t offset, const local float* p);
float2 __attribute__((__overloadable__,pure,__always_inline__))  vload2( size_t offset, const constant float* p);
float2 __attribute__((__overloadable__,pure,__always_inline__))  vload2( size_t offset, const private float* p);
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__,pure,__always_inline__))  vload3( size_t offset, const global float* p);
float3 __attribute__((__overloadable__,pure,__always_inline__))  vload3( size_t offset, const local float* p);
float3 __attribute__((__overloadable__,pure,__always_inline__))  vload3( size_t offset, const constant float* p);
float3 __attribute__((__overloadable__,pure,__always_inline__))  vload3( size_t offset, const private float* p);
#endif
float4 __attribute__((__overloadable__,pure,__always_inline__))  vload4( size_t offset, const global float* p);
float4 __attribute__((__overloadable__,pure,__always_inline__))  vload4( size_t offset, const local float* p);
float4 __attribute__((__overloadable__,pure,__always_inline__))  vload4( size_t offset, const constant float* p);
float4 __attribute__((__overloadable__,pure,__always_inline__))  vload4( size_t offset, const private float* p);
float8 __attribute__((__overloadable__,pure,__always_inline__))  vload8( size_t offset, const global float* p);
float8 __attribute__((__overloadable__,pure,__always_inline__))  vload8( size_t offset, const local float* p);
float8 __attribute__((__overloadable__,pure,__always_inline__))  vload8( size_t offset, const constant float* p);
float8 __attribute__((__overloadable__,pure,__always_inline__))  vload8( size_t offset, const private float* p);
float16 __attribute__((__overloadable__,pure,__always_inline__))  vload16( size_t offset, const global float* p);
float16 __attribute__((__overloadable__,pure,__always_inline__))  vload16( size_t offset, const local float* p);
float16 __attribute__((__overloadable__,pure,__always_inline__))  vload16( size_t offset, const constant float* p);
float16 __attribute__((__overloadable__,pure,__always_inline__))  vload16( size_t offset, const private float* p);
double2 __attribute__((__overloadable__,pure,__always_inline__))  vload2( size_t offset, const global double* p);
double2 __attribute__((__overloadable__,pure,__always_inline__))  vload2( size_t offset, const local double* p);
double2 __attribute__((__overloadable__,pure,__always_inline__))  vload2( size_t offset, const constant double* p);
double2 __attribute__((__overloadable__,pure,__always_inline__))  vload2( size_t offset, const private double* p);
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__,pure,__always_inline__))  vload3( size_t offset, const global double* p);
double3 __attribute__((__overloadable__,pure,__always_inline__))  vload3( size_t offset, const local double* p);
double3 __attribute__((__overloadable__,pure,__always_inline__))  vload3( size_t offset, const constant double* p);
double3 __attribute__((__overloadable__,pure,__always_inline__))  vload3( size_t offset, const private double* p);
#endif
double4 __attribute__((__overloadable__,pure,__always_inline__))  vload4( size_t offset, const global double* p);
double4 __attribute__((__overloadable__,pure,__always_inline__))  vload4( size_t offset, const local double* p);
double4 __attribute__((__overloadable__,pure,__always_inline__))  vload4( size_t offset, const constant double* p);
double4 __attribute__((__overloadable__,pure,__always_inline__))  vload4( size_t offset, const private double* p);
double8 __attribute__((__overloadable__,pure,__always_inline__))  vload8( size_t offset, const global double* p);
double8 __attribute__((__overloadable__,pure,__always_inline__))  vload8( size_t offset, const local double* p);
double8 __attribute__((__overloadable__,pure,__always_inline__))  vload8( size_t offset, const constant double* p);
double8 __attribute__((__overloadable__,pure,__always_inline__))  vload8( size_t offset, const private double* p);
double16 __attribute__((__overloadable__,pure,__always_inline__))  vload16( size_t offset, const global double* p);
double16 __attribute__((__overloadable__,pure,__always_inline__))  vload16( size_t offset, const local double* p);
double16 __attribute__((__overloadable__,pure,__always_inline__))  vload16( size_t offset, const constant double* p);
double16 __attribute__((__overloadable__,pure,__always_inline__))  vload16( size_t offset, const private double* p);
float __attribute__((__overloadable__,pure,__always_inline__))  vload_half( size_t offset, const global half* p );
float __attribute__((__overloadable__,pure,__always_inline__))  vloada_half( size_t offset, const global half* p );
float2 __attribute__((__overloadable__,pure,__always_inline__))  vload_half2( size_t offset, const global half* p );
float2 __attribute__((__overloadable__,pure,__always_inline__))  vloada_half2( size_t offset, const global half* p );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__,pure,__always_inline__))  vload_half3( size_t offset, const global half* p );
float3 __attribute__((__overloadable__,pure,__always_inline__))  vloada_half3( size_t offset, const global half* p );
#endif
float4 __attribute__((__overloadable__,pure,__always_inline__))  vload_half4( size_t offset, const global half* p );
float4 __attribute__((__overloadable__,pure,__always_inline__))  vloada_half4( size_t offset, const global half* p );
float8 __attribute__((__overloadable__,pure,__always_inline__))  vload_half8( size_t offset, const global half* p );
float8 __attribute__((__overloadable__,pure,__always_inline__))  vloada_half8( size_t offset, const global half* p );
float16 __attribute__((__overloadable__,pure,__always_inline__))  vload_half16( size_t offset, const global half* p );
float16 __attribute__((__overloadable__,pure,__always_inline__))  vloada_half16( size_t offset, const global half* p );
float __attribute__((__overloadable__,pure,__always_inline__))  vload_half( size_t offset, const local half* p );
float __attribute__((__overloadable__,pure,__always_inline__))  vloada_half( size_t offset, const local half* p );
float2 __attribute__((__overloadable__,pure,__always_inline__))  vload_half2( size_t offset, const local half* p );
float2 __attribute__((__overloadable__,pure,__always_inline__))  vloada_half2( size_t offset, const local half* p );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__,pure,__always_inline__))  vload_half3( size_t offset, const local half* p );
float3 __attribute__((__overloadable__,pure,__always_inline__))  vloada_half3( size_t offset, const local half* p );
#endif
float4 __attribute__((__overloadable__,pure,__always_inline__))  vload_half4( size_t offset, const local half* p );
float4 __attribute__((__overloadable__,pure,__always_inline__))  vloada_half4( size_t offset, const local half* p );
float8 __attribute__((__overloadable__,pure,__always_inline__))  vload_half8( size_t offset, const local half* p );
float8 __attribute__((__overloadable__,pure,__always_inline__))  vloada_half8( size_t offset, const local half* p );
float16 __attribute__((__overloadable__,pure,__always_inline__))  vload_half16( size_t offset, const local half* p );
float16 __attribute__((__overloadable__,pure,__always_inline__))  vloada_half16( size_t offset, const local half* p );
float __attribute__((__overloadable__,pure,__always_inline__))  vload_half( size_t offset, const constant half* p );
float __attribute__((__overloadable__,pure,__always_inline__))  vloada_half( size_t offset, const constant half* p );
float2 __attribute__((__overloadable__,pure,__always_inline__))  vload_half2( size_t offset, const constant half* p );
float2 __attribute__((__overloadable__,pure,__always_inline__))  vloada_half2( size_t offset, const constant half* p );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__,pure,__always_inline__))  vload_half3( size_t offset, const constant half* p );
float3 __attribute__((__overloadable__,pure,__always_inline__))  vloada_half3( size_t offset, const constant half* p );
#endif
float4 __attribute__((__overloadable__,pure,__always_inline__))  vload_half4( size_t offset, const constant half* p );
float4 __attribute__((__overloadable__,pure,__always_inline__))  vloada_half4( size_t offset, const constant half* p );
float8 __attribute__((__overloadable__,pure,__always_inline__))  vload_half8( size_t offset, const constant half* p );
float8 __attribute__((__overloadable__,pure,__always_inline__))  vloada_half8( size_t offset, const constant half* p );
float16 __attribute__((__overloadable__,pure,__always_inline__))  vload_half16( size_t offset, const constant half* p );
float16 __attribute__((__overloadable__,pure,__always_inline__))  vloada_half16( size_t offset, const constant half* p );
float __attribute__((__overloadable__,pure,__always_inline__))  vload_half( size_t offset, const private half* p );
float __attribute__((__overloadable__,pure,__always_inline__))  vloada_half( size_t offset, const private half* p );
float2 __attribute__((__overloadable__,pure,__always_inline__))  vload_half2( size_t offset, const private half* p );
float2 __attribute__((__overloadable__,pure,__always_inline__))  vloada_half2( size_t offset, const private half* p );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__,pure,__always_inline__))  vload_half3( size_t offset, const private half* p );
float3 __attribute__((__overloadable__,pure,__always_inline__))  vloada_half3( size_t offset, const private half* p );
#endif
float4 __attribute__((__overloadable__,pure,__always_inline__))  vload_half4( size_t offset, const private half* p );
float4 __attribute__((__overloadable__,pure,__always_inline__))  vloada_half4( size_t offset, const private half* p );
float8 __attribute__((__overloadable__,pure,__always_inline__))  vload_half8( size_t offset, const private half* p );
float8 __attribute__((__overloadable__,pure,__always_inline__))  vloada_half8( size_t offset, const private half* p );
float16 __attribute__((__overloadable__,pure,__always_inline__))  vload_half16( size_t offset, const private half* p );
float16 __attribute__((__overloadable__,pure,__always_inline__))  vloada_half16( size_t offset, const private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore2( char2 data, size_t offset, global char* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore2( char2 data, size_t offset, local char* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore2( char2 data, size_t offset, private char* p);
#if __OPENCL_C_VERSION__ >= 110
void  __attribute__((__overloadable__,__always_inline__)) vstore3( char3 data, size_t offset, global char* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore3( char3 data, size_t offset, local char* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore3( char3 data, size_t offset, private char* p);
#endif
void  __attribute__((__overloadable__,__always_inline__)) vstore4( char4 data, size_t offset, global char* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore4( char4 data, size_t offset, local char* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore4( char4 data, size_t offset, private char* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore8( char8 data, size_t offset, global char* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore8( char8 data, size_t offset, local char* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore8( char8 data, size_t offset, private char* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore16( char16 data, size_t offset, global char* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore16( char16 data, size_t offset, local char* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore16( char16 data, size_t offset, private char* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore2( uchar2 data, size_t offset, global uchar* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore2( uchar2 data, size_t offset, local uchar* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore2( uchar2 data, size_t offset, private uchar* p);
#if __OPENCL_C_VERSION__ >= 110
void  __attribute__((__overloadable__,__always_inline__)) vstore3( uchar3 data, size_t offset, global uchar* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore3( uchar3 data, size_t offset, local uchar* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore3( uchar3 data, size_t offset, private uchar* p);
#endif
void  __attribute__((__overloadable__,__always_inline__)) vstore4( uchar4 data, size_t offset, global uchar* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore4( uchar4 data, size_t offset, local uchar* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore4( uchar4 data, size_t offset, private uchar* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore8( uchar8 data, size_t offset, global uchar* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore8( uchar8 data, size_t offset, local uchar* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore8( uchar8 data, size_t offset, private uchar* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore16( uchar16 data, size_t offset, global uchar* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore16( uchar16 data, size_t offset, local uchar* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore16( uchar16 data, size_t offset, private uchar* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore2( short2 data, size_t offset, global short* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore2( short2 data, size_t offset, local short* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore2( short2 data, size_t offset, private short* p);
#if __OPENCL_C_VERSION__ >= 110
void  __attribute__((__overloadable__,__always_inline__)) vstore3( short3 data, size_t offset, global short* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore3( short3 data, size_t offset, local short* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore3( short3 data, size_t offset, private short* p);
#endif
void  __attribute__((__overloadable__,__always_inline__)) vstore4( short4 data, size_t offset, global short* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore4( short4 data, size_t offset, local short* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore4( short4 data, size_t offset, private short* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore8( short8 data, size_t offset, global short* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore8( short8 data, size_t offset, local short* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore8( short8 data, size_t offset, private short* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore16( short16 data, size_t offset, global short* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore16( short16 data, size_t offset, local short* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore16( short16 data, size_t offset, private short* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore2( ushort2 data, size_t offset, global ushort* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore2( ushort2 data, size_t offset, local ushort* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore2( ushort2 data, size_t offset, private ushort* p);
#if __OPENCL_C_VERSION__ >= 110
void  __attribute__((__overloadable__,__always_inline__)) vstore3( ushort3 data, size_t offset, global ushort* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore3( ushort3 data, size_t offset, local ushort* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore3( ushort3 data, size_t offset, private ushort* p);
#endif
void  __attribute__((__overloadable__,__always_inline__)) vstore4( ushort4 data, size_t offset, global ushort* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore4( ushort4 data, size_t offset, local ushort* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore4( ushort4 data, size_t offset, private ushort* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore8( ushort8 data, size_t offset, global ushort* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore8( ushort8 data, size_t offset, local ushort* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore8( ushort8 data, size_t offset, private ushort* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore16( ushort16 data, size_t offset, global ushort* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore16( ushort16 data, size_t offset, local ushort* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore16( ushort16 data, size_t offset, private ushort* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore2( int2 data, size_t offset, global int* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore2( int2 data, size_t offset, local int* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore2( int2 data, size_t offset, private int* p);
#if __OPENCL_C_VERSION__ >= 110
void  __attribute__((__overloadable__,__always_inline__)) vstore3( int3 data, size_t offset, global int* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore3( int3 data, size_t offset, local int* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore3( int3 data, size_t offset, private int* p);
#endif
void  __attribute__((__overloadable__,__always_inline__)) vstore4( int4 data, size_t offset, global int* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore4( int4 data, size_t offset, local int* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore4( int4 data, size_t offset, private int* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore8( int8 data, size_t offset, global int* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore8( int8 data, size_t offset, local int* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore8( int8 data, size_t offset, private int* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore16( int16 data, size_t offset, global int* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore16( int16 data, size_t offset, local int* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore16( int16 data, size_t offset, private int* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore2( uint2 data, size_t offset, global uint* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore2( uint2 data, size_t offset, local uint* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore2( uint2 data, size_t offset, private uint* p);
#if __OPENCL_C_VERSION__ >= 110
void  __attribute__((__overloadable__,__always_inline__)) vstore3( uint3 data, size_t offset, global uint* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore3( uint3 data, size_t offset, local uint* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore3( uint3 data, size_t offset, private uint* p);
#endif
void  __attribute__((__overloadable__,__always_inline__)) vstore4( uint4 data, size_t offset, global uint* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore4( uint4 data, size_t offset, local uint* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore4( uint4 data, size_t offset, private uint* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore8( uint8 data, size_t offset, global uint* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore8( uint8 data, size_t offset, local uint* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore8( uint8 data, size_t offset, private uint* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore16( uint16 data, size_t offset, global uint* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore16( uint16 data, size_t offset, local uint* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore16( uint16 data, size_t offset, private uint* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore2( long2 data, size_t offset, global long* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore2( long2 data, size_t offset, local long* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore2( long2 data, size_t offset, private long* p);
#if __OPENCL_C_VERSION__ >= 110
void  __attribute__((__overloadable__,__always_inline__)) vstore3( long3 data, size_t offset, global long* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore3( long3 data, size_t offset, local long* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore3( long3 data, size_t offset, private long* p);
#endif
void  __attribute__((__overloadable__,__always_inline__)) vstore4( long4 data, size_t offset, global long* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore4( long4 data, size_t offset, local long* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore4( long4 data, size_t offset, private long* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore8( long8 data, size_t offset, global long* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore8( long8 data, size_t offset, local long* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore8( long8 data, size_t offset, private long* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore16( long16 data, size_t offset, global long* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore16( long16 data, size_t offset, local long* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore16( long16 data, size_t offset, private long* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore2( ulong2 data, size_t offset, global ulong* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore2( ulong2 data, size_t offset, local ulong* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore2( ulong2 data, size_t offset, private ulong* p);
#if __OPENCL_C_VERSION__ >= 110
void  __attribute__((__overloadable__,__always_inline__)) vstore3( ulong3 data, size_t offset, global ulong* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore3( ulong3 data, size_t offset, local ulong* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore3( ulong3 data, size_t offset, private ulong* p);
#endif
void  __attribute__((__overloadable__,__always_inline__)) vstore4( ulong4 data, size_t offset, global ulong* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore4( ulong4 data, size_t offset, local ulong* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore4( ulong4 data, size_t offset, private ulong* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore8( ulong8 data, size_t offset, global ulong* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore8( ulong8 data, size_t offset, local ulong* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore8( ulong8 data, size_t offset, private ulong* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore16( ulong16 data, size_t offset, global ulong* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore16( ulong16 data, size_t offset, local ulong* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore16( ulong16 data, size_t offset, private ulong* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore2( float2 data, size_t offset, global float* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore2( float2 data, size_t offset, local float* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore2( float2 data, size_t offset, private float* p);
#if __OPENCL_C_VERSION__ >= 110
void  __attribute__((__overloadable__,__always_inline__)) vstore3( float3 data, size_t offset, global float* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore3( float3 data, size_t offset, local float* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore3( float3 data, size_t offset, private float* p);
#endif
void  __attribute__((__overloadable__,__always_inline__)) vstore4( float4 data, size_t offset, global float* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore4( float4 data, size_t offset, local float* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore4( float4 data, size_t offset, private float* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore8( float8 data, size_t offset, global float* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore8( float8 data, size_t offset, local float* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore8( float8 data, size_t offset, private float* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore16( float16 data, size_t offset, global float* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore16( float16 data, size_t offset, local float* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore16( float16 data, size_t offset, private float* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore2( double2 data, size_t offset, global double* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore2( double2 data, size_t offset, local double* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore2( double2 data, size_t offset, private double* p);
#if __OPENCL_C_VERSION__ >= 110
void  __attribute__((__overloadable__,__always_inline__)) vstore3( double3 data, size_t offset, global double* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore3( double3 data, size_t offset, local double* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore3( double3 data, size_t offset, private double* p);
#endif
void  __attribute__((__overloadable__,__always_inline__)) vstore4( double4 data, size_t offset, global double* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore4( double4 data, size_t offset, local double* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore4( double4 data, size_t offset, private double* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore8( double8 data, size_t offset, global double* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore8( double8 data, size_t offset, local double* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore8( double8 data, size_t offset, private double* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore16( double16 data, size_t offset, global double* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore16( double16 data, size_t offset, local double* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore16( double16 data, size_t offset, private double* p);
void  __attribute__((__overloadable__,__always_inline__)) vstore_half( float data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half( float data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half_rte( float data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half_rte( float data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half_rtz( float data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half_rtz( float data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half_rtp( float data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half_rtp( float data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half_rtn( float data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half_rtn( float data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half2( float2 data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half2( float2 data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half2_rte( float2 data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half2_rte( float2 data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half2_rtz( float2 data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half2_rtz( float2 data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half2_rtp( float2 data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half2_rtp( float2 data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half2_rtn( float2 data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half2_rtn( float2 data, size_t offset, global half* p );
#if __OPENCL_C_VERSION__ >= 110
void  __attribute__((__overloadable__,__always_inline__)) vstore_half3( float3 data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half3( float3 data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half3_rte( float3 data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half3_rte( float3 data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half3_rtz( float3 data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half3_rtz( float3 data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half3_rtp( float3 data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half3_rtp( float3 data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half3_rtn( float3 data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half3_rtn( float3 data, size_t offset, global half* p );
#endif
void  __attribute__((__overloadable__,__always_inline__)) vstore_half4( float4 data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half4( float4 data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half4_rte( float4 data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half4_rte( float4 data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half4_rtz( float4 data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half4_rtz( float4 data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half4_rtp( float4 data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half4_rtp( float4 data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half4_rtn( float4 data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half4_rtn( float4 data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half8( float8 data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half8( float8 data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half8_rte( float8 data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half8_rte( float8 data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half8_rtz( float8 data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half8_rtz( float8 data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half8_rtp( float8 data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half8_rtp( float8 data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half8_rtn( float8 data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half8_rtn( float8 data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half16( float16 data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half16( float16 data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half16_rte( float16 data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half16_rte( float16 data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half16_rtz( float16 data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half16_rtz( float16 data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half16_rtp( float16 data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half16_rtp( float16 data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half16_rtn( float16 data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half16_rtn( float16 data, size_t offset, global half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half( float data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half( float data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half_rte( float data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half_rte( float data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half_rtz( float data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half_rtz( float data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half_rtp( float data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half_rtp( float data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half_rtn( float data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half_rtn( float data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half2( float2 data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half2( float2 data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half2_rte( float2 data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half2_rte( float2 data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half2_rtz( float2 data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half2_rtz( float2 data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half2_rtp( float2 data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half2_rtp( float2 data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half2_rtn( float2 data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half2_rtn( float2 data, size_t offset, local half* p );
#if __OPENCL_C_VERSION__ >= 110
void  __attribute__((__overloadable__,__always_inline__)) vstore_half3( float3 data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half3( float3 data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half3_rte( float3 data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half3_rte( float3 data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half3_rtz( float3 data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half3_rtz( float3 data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half3_rtp( float3 data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half3_rtp( float3 data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half3_rtn( float3 data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half3_rtn( float3 data, size_t offset, local half* p );
#endif
void  __attribute__((__overloadable__,__always_inline__)) vstore_half4( float4 data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half4( float4 data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half4_rte( float4 data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half4_rte( float4 data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half4_rtz( float4 data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half4_rtz( float4 data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half4_rtp( float4 data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half4_rtp( float4 data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half4_rtn( float4 data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half4_rtn( float4 data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half8( float8 data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half8( float8 data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half8_rte( float8 data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half8_rte( float8 data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half8_rtz( float8 data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half8_rtz( float8 data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half8_rtp( float8 data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half8_rtp( float8 data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half8_rtn( float8 data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half8_rtn( float8 data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half16( float16 data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half16( float16 data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half16_rte( float16 data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half16_rte( float16 data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half16_rtz( float16 data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half16_rtz( float16 data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half16_rtp( float16 data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half16_rtp( float16 data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half16_rtn( float16 data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half16_rtn( float16 data, size_t offset, local half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half( float data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half( float data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half_rte( float data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half_rte( float data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half_rtz( float data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half_rtz( float data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half_rtp( float data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half_rtp( float data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half_rtn( float data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half_rtn( float data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half2( float2 data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half2( float2 data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half2_rte( float2 data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half2_rte( float2 data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half2_rtz( float2 data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half2_rtz( float2 data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half2_rtp( float2 data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half2_rtp( float2 data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half2_rtn( float2 data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half2_rtn( float2 data, size_t offset, private half* p );
#if __OPENCL_C_VERSION__ >= 110
void  __attribute__((__overloadable__,__always_inline__)) vstore_half3( float3 data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half3( float3 data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half3_rte( float3 data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half3_rte( float3 data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half3_rtz( float3 data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half3_rtz( float3 data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half3_rtp( float3 data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half3_rtp( float3 data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half3_rtn( float3 data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half3_rtn( float3 data, size_t offset, private half* p );
#endif
void  __attribute__((__overloadable__,__always_inline__)) vstore_half4( float4 data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half4( float4 data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half4_rte( float4 data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half4_rte( float4 data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half4_rtz( float4 data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half4_rtz( float4 data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half4_rtp( float4 data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half4_rtp( float4 data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half4_rtn( float4 data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half4_rtn( float4 data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half8( float8 data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half8( float8 data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half8_rte( float8 data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half8_rte( float8 data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half8_rtz( float8 data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half8_rtz( float8 data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half8_rtp( float8 data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half8_rtp( float8 data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half8_rtn( float8 data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half8_rtn( float8 data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half16( float16 data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half16( float16 data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half16_rte( float16 data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half16_rte( float16 data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half16_rtz( float16 data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half16_rtz( float16 data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half16_rtp( float16 data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half16_rtp( float16 data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstore_half16_rtn( float16 data, size_t offset, private half* p );
void  __attribute__((__overloadable__,__always_inline__)) vstorea_half16_rtn( float16 data, size_t offset, private half* p );
int __attribute__((__overloadable__,__always_inline__,const)) any( char x);
int __attribute__((__overloadable__,__always_inline__,const)) any( char2 x);
#if __OPENCL_C_VERSION__ >= 110
int __attribute__((__overloadable__,__always_inline__,const)) any( char3 x);
#endif
int __attribute__((__overloadable__,__always_inline__,const)) any( char4 x);
int __attribute__((__overloadable__,__always_inline__,const)) any( char8 x);
int __attribute__((__overloadable__,__always_inline__,const)) any( char16 x);
int __attribute__((__overloadable__,__always_inline__,const)) all( char x);
int __attribute__((__overloadable__,__always_inline__,const)) all( char2 x);
#if __OPENCL_C_VERSION__ >= 110
int __attribute__((__overloadable__,__always_inline__,const)) all( char3 x);
#endif
int __attribute__((__overloadable__,__always_inline__,const)) all( char4 x);
int __attribute__((__overloadable__,__always_inline__,const)) all( char8 x);
int __attribute__((__overloadable__,__always_inline__,const)) all( char16 x);
int __attribute__((__overloadable__,__always_inline__,const)) any( short x);
int __attribute__((__overloadable__,__always_inline__,const)) any( short2 x);
#if __OPENCL_C_VERSION__ >= 110
int __attribute__((__overloadable__,__always_inline__,const)) any( short3 x);
#endif
int __attribute__((__overloadable__,__always_inline__,const)) any( short4 x);
int __attribute__((__overloadable__,__always_inline__,const)) any( short8 x);
int __attribute__((__overloadable__,__always_inline__,const)) any( short16 x);
int __attribute__((__overloadable__,__always_inline__,const)) all( short x);
int __attribute__((__overloadable__,__always_inline__,const)) all( short2 x);
#if __OPENCL_C_VERSION__ >= 110
int __attribute__((__overloadable__,__always_inline__,const)) all( short3 x);
#endif
int __attribute__((__overloadable__,__always_inline__,const)) all( short4 x);
int __attribute__((__overloadable__,__always_inline__,const)) all( short8 x);
int __attribute__((__overloadable__,__always_inline__,const)) all( short16 x);
int __attribute__((__overloadable__,__always_inline__,const)) any( int x);
int __attribute__((__overloadable__,__always_inline__,const)) any( int2 x);
#if __OPENCL_C_VERSION__ >= 110
int __attribute__((__overloadable__,__always_inline__,const)) any( int3 x);
#endif
int __attribute__((__overloadable__,__always_inline__,const)) any( int4 x);
int __attribute__((__overloadable__,__always_inline__,const)) any( int8 x);
int __attribute__((__overloadable__,__always_inline__,const)) any( int16 x);
int __attribute__((__overloadable__,__always_inline__,const)) all( int x);
int __attribute__((__overloadable__,__always_inline__,const)) all( int2 x);
#if __OPENCL_C_VERSION__ >= 110
int __attribute__((__overloadable__,__always_inline__,const)) all( int3 x);
#endif
int __attribute__((__overloadable__,__always_inline__,const)) all( int4 x);
int __attribute__((__overloadable__,__always_inline__,const)) all( int8 x);
int __attribute__((__overloadable__,__always_inline__,const)) all( int16 x);
int __attribute__((__overloadable__,__always_inline__,const)) any( long x);
int __attribute__((__overloadable__,__always_inline__,const)) any( long2 x);
#if __OPENCL_C_VERSION__ >= 110
int __attribute__((__overloadable__,__always_inline__,const)) any( long3 x);
#endif
int __attribute__((__overloadable__,__always_inline__,const)) any( long4 x);
int __attribute__((__overloadable__,__always_inline__,const)) any( long8 x);
int __attribute__((__overloadable__,__always_inline__,const)) any( long16 x);
int __attribute__((__overloadable__,__always_inline__,const)) all( long x);
int __attribute__((__overloadable__,__always_inline__,const)) all( long2 x);
#if __OPENCL_C_VERSION__ >= 110
int __attribute__((__overloadable__,__always_inline__,const)) all( long3 x);
#endif
int __attribute__((__overloadable__,__always_inline__,const)) all( long4 x);
int __attribute__((__overloadable__,__always_inline__,const)) all( long8 x);
int __attribute__((__overloadable__,__always_inline__,const)) all( long16 x);
char __attribute__((__overloadable__,__always_inline__,const)) bitselect( char a, char b, char c ) ;
uchar __attribute__((__overloadable__,__always_inline__,const)) bitselect( uchar a, uchar b, uchar c ) ;
short __attribute__((__overloadable__,__always_inline__,const)) bitselect( short a, short b, short c ) ;
ushort __attribute__((__overloadable__,__always_inline__,const)) bitselect( ushort a, ushort b, ushort c ) ;
int __attribute__((__overloadable__,__always_inline__,const)) bitselect( int a, int b, int c ) ;
uint __attribute__((__overloadable__,__always_inline__,const)) bitselect( uint a, uint b, uint c ) ;
long __attribute__((__overloadable__,__always_inline__,const)) bitselect( long a, long b, long c ) ;
ulong __attribute__((__overloadable__,__always_inline__,const)) bitselect( ulong a, ulong b, ulong c ) ;
float __attribute__((__overloadable__,__always_inline__,const)) bitselect( float a, float b, float c ) ;
double __attribute__((__overloadable__,__always_inline__,const)) bitselect( double a, double b, double c ) ;
char2 __attribute__((__overloadable__,__always_inline__,const)) bitselect( char2 a, char2 b, char2 c );
#if __OPENCL_C_VERSION__ >= 110
char3 __attribute__((__overloadable__,__always_inline__,const)) bitselect( char3 a, char3 b, char3 c );
#endif
char4 __attribute__((__overloadable__,__always_inline__,const)) bitselect( char4 a, char4 b, char4 c );
char8 __attribute__((__overloadable__,__always_inline__,const)) bitselect( char8 a, char8 b, char8 c );
char16 __attribute__((__overloadable__,__always_inline__,const)) bitselect( char16 a, char16 b, char16 c );
uchar2 __attribute__((__overloadable__,__always_inline__,const)) bitselect( uchar2 a, uchar2 b, uchar2 c );
#if __OPENCL_C_VERSION__ >= 110
uchar3 __attribute__((__overloadable__,__always_inline__,const)) bitselect( uchar3 a, uchar3 b, uchar3 c );
#endif
uchar4 __attribute__((__overloadable__,__always_inline__,const)) bitselect( uchar4 a, uchar4 b, uchar4 c );
uchar8 __attribute__((__overloadable__,__always_inline__,const)) bitselect( uchar8 a, uchar8 b, uchar8 c );
uchar16 __attribute__((__overloadable__,__always_inline__,const)) bitselect( uchar16 a, uchar16 b, uchar16 c );
short2 __attribute__((__overloadable__,__always_inline__,const)) bitselect( short2 a, short2 b, short2 c );
#if __OPENCL_C_VERSION__ >= 110
short3 __attribute__((__overloadable__,__always_inline__,const)) bitselect( short3 a, short3 b, short3 c );
#endif
short4 __attribute__((__overloadable__,__always_inline__,const)) bitselect( short4 a, short4 b, short4 c );
short8 __attribute__((__overloadable__,__always_inline__,const)) bitselect( short8 a, short8 b, short8 c );
short16 __attribute__((__overloadable__,__always_inline__,const)) bitselect( short16 a, short16 b, short16 c );
ushort2 __attribute__((__overloadable__,__always_inline__,const)) bitselect( ushort2 a, ushort2 b, ushort2 c );
#if __OPENCL_C_VERSION__ >= 110
ushort3 __attribute__((__overloadable__,__always_inline__,const)) bitselect( ushort3 a, ushort3 b, ushort3 c );
#endif
ushort4 __attribute__((__overloadable__,__always_inline__,const)) bitselect( ushort4 a, ushort4 b, ushort4 c );
ushort8 __attribute__((__overloadable__,__always_inline__,const)) bitselect( ushort8 a, ushort8 b, ushort8 c );
ushort16 __attribute__((__overloadable__,__always_inline__,const)) bitselect( ushort16 a, ushort16 b, ushort16 c );
int2 __attribute__((__overloadable__,__always_inline__,const)) bitselect( int2 a, int2 b, int2 c );
#if __OPENCL_C_VERSION__ >= 110
int3 __attribute__((__overloadable__,__always_inline__,const)) bitselect( int3 a, int3 b, int3 c );
#endif
int4 __attribute__((__overloadable__,__always_inline__,const)) bitselect( int4 a, int4 b, int4 c );
int8 __attribute__((__overloadable__,__always_inline__,const)) bitselect( int8 a, int8 b, int8 c );
int16 __attribute__((__overloadable__,__always_inline__,const)) bitselect( int16 a, int16 b, int16 c );
uint2 __attribute__((__overloadable__,__always_inline__,const)) bitselect( uint2 a, uint2 b, uint2 c );
#if __OPENCL_C_VERSION__ >= 110
uint3 __attribute__((__overloadable__,__always_inline__,const)) bitselect( uint3 a, uint3 b, uint3 c );
#endif
uint4 __attribute__((__overloadable__,__always_inline__,const)) bitselect( uint4 a, uint4 b, uint4 c );
uint8 __attribute__((__overloadable__,__always_inline__,const)) bitselect( uint8 a, uint8 b, uint8 c );
uint16 __attribute__((__overloadable__,__always_inline__,const)) bitselect( uint16 a, uint16 b, uint16 c );
long2 __attribute__((__overloadable__,__always_inline__,const)) bitselect( long2 a, long2 b, long2 c );
#if __OPENCL_C_VERSION__ >= 110
long3 __attribute__((__overloadable__,__always_inline__,const)) bitselect( long3 a, long3 b, long3 c );
#endif
long4 __attribute__((__overloadable__,__always_inline__,const)) bitselect( long4 a, long4 b, long4 c );
long8 __attribute__((__overloadable__,__always_inline__,const)) bitselect( long8 a, long8 b, long8 c );
long16 __attribute__((__overloadable__,__always_inline__,const)) bitselect( long16 a, long16 b, long16 c );
ulong2 __attribute__((__overloadable__,__always_inline__,const)) bitselect( ulong2 a, ulong2 b, ulong2 c );
#if __OPENCL_C_VERSION__ >= 110
ulong3 __attribute__((__overloadable__,__always_inline__,const)) bitselect( ulong3 a, ulong3 b, ulong3 c );
#endif
ulong4 __attribute__((__overloadable__,__always_inline__,const)) bitselect( ulong4 a, ulong4 b, ulong4 c );
ulong8 __attribute__((__overloadable__,__always_inline__,const)) bitselect( ulong8 a, ulong8 b, ulong8 c );
ulong16 __attribute__((__overloadable__,__always_inline__,const)) bitselect( ulong16 a, ulong16 b, ulong16 c );
float2 __attribute__((__overloadable__,__always_inline__,const)) bitselect( float2 a, float2 b, float2 c );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__,__always_inline__,const)) bitselect( float3 a, float3 b, float3 c );
#endif
float4 __attribute__((__overloadable__,__always_inline__,const)) bitselect( float4 a, float4 b, float4 c );
float8 __attribute__((__overloadable__,__always_inline__,const)) bitselect( float8 a, float8 b, float8 c );
float16 __attribute__((__overloadable__,__always_inline__,const)) bitselect( float16 a, float16 b, float16 c );
double2 __attribute__((__overloadable__,__always_inline__,const)) bitselect( double2 a, double2 b, double2 c );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__,__always_inline__,const)) bitselect( double3 a, double3 b, double3 c );
#endif
double4 __attribute__((__overloadable__,__always_inline__,const)) bitselect( double4 a, double4 b, double4 c );
double8 __attribute__((__overloadable__,__always_inline__,const)) bitselect( double8 a, double8 b, double8 c );
double16 __attribute__((__overloadable__,__always_inline__,const)) bitselect( double16 a, double16 b, double16 c );
char __attribute__((__overloadable__,__always_inline__,const)) select( char a, char b, uchar c ) ;
char __attribute__((__overloadable__,__always_inline__,const)) select( char a, char b, char c ) ;
uchar __attribute__((__overloadable__,__always_inline__,const)) select( uchar a, uchar b, uchar c ) ;
uchar __attribute__((__overloadable__,__always_inline__,const)) select( uchar a, uchar b, char c ) ;
short __attribute__((__overloadable__,__always_inline__,const)) select( short a, short b, ushort c ) ;
short __attribute__((__overloadable__,__always_inline__,const)) select( short a, short b, short c ) ;
ushort __attribute__((__overloadable__,__always_inline__,const)) select( ushort a, ushort b, ushort c ) ;
ushort __attribute__((__overloadable__,__always_inline__,const)) select( ushort a, ushort b, short c ) ;
int __attribute__((__overloadable__,__always_inline__,const)) select( int a, int b, uint c ) ;
int __attribute__((__overloadable__,__always_inline__,const)) select( int a, int b, int c ) ;
uint __attribute__((__overloadable__,__always_inline__,const)) select( uint a, uint b, uint c ) ;
uint __attribute__((__overloadable__,__always_inline__,const)) select( uint a, uint b, int c ) ;
long __attribute__((__overloadable__,__always_inline__,const)) select( long a, long b, ulong c ) ;
long __attribute__((__overloadable__,__always_inline__,const)) select( long a, long b, long c ) ;
ulong __attribute__((__overloadable__,__always_inline__,const)) select( ulong a, ulong b, ulong c ) ;
ulong __attribute__((__overloadable__,__always_inline__,const)) select( ulong a, ulong b, long c ) ;
float __attribute__((__overloadable__,__always_inline__,const)) select( float a, float b, uint c ) ;
float __attribute__((__overloadable__,__always_inline__,const)) select( float a, float b, int c ) ;
double __attribute__((__overloadable__,__always_inline__,const)) select( double a, double b, ulong c ) ;
double __attribute__((__overloadable__,__always_inline__,const)) select( double a, double b, long c ) ;
char2 __attribute__((__overloadable__,__always_inline__,const)) select( char2 a, char2 b, uchar2 c );
char2 __attribute__((__overloadable__,__always_inline__,const)) select( char2 a, char2 b, char2 c );
#if __OPENCL_C_VERSION__ >= 110
char3 __attribute__((__overloadable__,__always_inline__,const)) select( char3 a, char3 b, uchar3 c );
char3 __attribute__((__overloadable__,__always_inline__,const)) select( char3 a, char3 b, char3 c );
#endif
char4 __attribute__((__overloadable__,__always_inline__,const)) select( char4 a, char4 b, uchar4 c );
char4 __attribute__((__overloadable__,__always_inline__,const)) select( char4 a, char4 b, char4 c );
char8 __attribute__((__overloadable__,__always_inline__,const)) select( char8 a, char8 b, uchar8 c );
char8 __attribute__((__overloadable__,__always_inline__,const)) select( char8 a, char8 b, char8 c );
char16 __attribute__((__overloadable__,__always_inline__,const)) select( char16 a, char16 b, uchar16 c );
char16 __attribute__((__overloadable__,__always_inline__,const)) select( char16 a, char16 b, char16 c );
uchar2 __attribute__((__overloadable__,__always_inline__,const)) select( uchar2 a, uchar2 b, uchar2 c );
uchar2 __attribute__((__overloadable__,__always_inline__,const)) select( uchar2 a, uchar2 b, char2 c );
#if __OPENCL_C_VERSION__ >= 110
uchar3 __attribute__((__overloadable__,__always_inline__,const)) select( uchar3 a, uchar3 b, uchar3 c );
uchar3 __attribute__((__overloadable__,__always_inline__,const)) select( uchar3 a, uchar3 b, char3 c );
#endif
uchar4 __attribute__((__overloadable__,__always_inline__,const)) select( uchar4 a, uchar4 b, uchar4 c );
uchar4 __attribute__((__overloadable__,__always_inline__,const)) select( uchar4 a, uchar4 b, char4 c );
uchar8 __attribute__((__overloadable__,__always_inline__,const)) select( uchar8 a, uchar8 b, uchar8 c );
uchar8 __attribute__((__overloadable__,__always_inline__,const)) select( uchar8 a, uchar8 b, char8 c );
uchar16 __attribute__((__overloadable__,__always_inline__,const)) select( uchar16 a, uchar16 b, uchar16 c );
uchar16 __attribute__((__overloadable__,__always_inline__,const)) select( uchar16 a, uchar16 b, char16 c );
short2 __attribute__((__overloadable__,__always_inline__,const)) select( short2 a, short2 b, ushort2 c );
short2 __attribute__((__overloadable__,__always_inline__,const)) select( short2 a, short2 b, short2 c );
#if __OPENCL_C_VERSION__ >= 110
short3 __attribute__((__overloadable__,__always_inline__,const)) select( short3 a, short3 b, ushort3 c );
short3 __attribute__((__overloadable__,__always_inline__,const)) select( short3 a, short3 b, short3 c );
#endif
short4 __attribute__((__overloadable__,__always_inline__,const)) select( short4 a, short4 b, ushort4 c );
short4 __attribute__((__overloadable__,__always_inline__,const)) select( short4 a, short4 b, short4 c );
short8 __attribute__((__overloadable__,__always_inline__,const)) select( short8 a, short8 b, ushort8 c );
short8 __attribute__((__overloadable__,__always_inline__,const)) select( short8 a, short8 b, short8 c );
short16 __attribute__((__overloadable__,__always_inline__,const)) select( short16 a, short16 b, ushort16 c );
short16 __attribute__((__overloadable__,__always_inline__,const)) select( short16 a, short16 b, short16 c );
ushort2 __attribute__((__overloadable__,__always_inline__,const)) select( ushort2 a, ushort2 b, ushort2 c );
ushort2 __attribute__((__overloadable__,__always_inline__,const)) select( ushort2 a, ushort2 b, short2 c );
#if __OPENCL_C_VERSION__ >= 110
ushort3 __attribute__((__overloadable__,__always_inline__,const)) select( ushort3 a, ushort3 b, ushort3 c );
ushort3 __attribute__((__overloadable__,__always_inline__,const)) select( ushort3 a, ushort3 b, short3 c );
#endif
ushort4 __attribute__((__overloadable__,__always_inline__,const)) select( ushort4 a, ushort4 b, ushort4 c );
ushort4 __attribute__((__overloadable__,__always_inline__,const)) select( ushort4 a, ushort4 b, short4 c );
ushort8 __attribute__((__overloadable__,__always_inline__,const)) select( ushort8 a, ushort8 b, ushort8 c );
ushort8 __attribute__((__overloadable__,__always_inline__,const)) select( ushort8 a, ushort8 b, short8 c );
ushort16 __attribute__((__overloadable__,__always_inline__,const)) select( ushort16 a, ushort16 b, ushort16 c );
ushort16 __attribute__((__overloadable__,__always_inline__,const)) select( ushort16 a, ushort16 b, short16 c );
int2 __attribute__((__overloadable__,__always_inline__,const)) select( int2 a, int2 b, uint2 c );
int2 __attribute__((__overloadable__,__always_inline__,const)) select( int2 a, int2 b, int2 c );
#if __OPENCL_C_VERSION__ >= 110
int3 __attribute__((__overloadable__,__always_inline__,const)) select( int3 a, int3 b, uint3 c );
int3 __attribute__((__overloadable__,__always_inline__,const)) select( int3 a, int3 b, int3 c );
#endif
int4 __attribute__((__overloadable__,__always_inline__,const)) select( int4 a, int4 b, uint4 c );
int4 __attribute__((__overloadable__,__always_inline__,const)) select( int4 a, int4 b, int4 c );
int8 __attribute__((__overloadable__,__always_inline__,const)) select( int8 a, int8 b, uint8 c );
int8 __attribute__((__overloadable__,__always_inline__,const)) select( int8 a, int8 b, int8 c );
int16 __attribute__((__overloadable__,__always_inline__,const)) select( int16 a, int16 b, uint16 c );
int16 __attribute__((__overloadable__,__always_inline__,const)) select( int16 a, int16 b, int16 c );
uint2 __attribute__((__overloadable__,__always_inline__,const)) select( uint2 a, uint2 b, uint2 c );
uint2 __attribute__((__overloadable__,__always_inline__,const)) select( uint2 a, uint2 b, int2 c );
#if __OPENCL_C_VERSION__ >= 110
uint3 __attribute__((__overloadable__,__always_inline__,const)) select( uint3 a, uint3 b, uint3 c );
uint3 __attribute__((__overloadable__,__always_inline__,const)) select( uint3 a, uint3 b, int3 c );
#endif
uint4 __attribute__((__overloadable__,__always_inline__,const)) select( uint4 a, uint4 b, uint4 c );
uint4 __attribute__((__overloadable__,__always_inline__,const)) select( uint4 a, uint4 b, int4 c );
uint8 __attribute__((__overloadable__,__always_inline__,const)) select( uint8 a, uint8 b, uint8 c );
uint8 __attribute__((__overloadable__,__always_inline__,const)) select( uint8 a, uint8 b, int8 c );
uint16 __attribute__((__overloadable__,__always_inline__,const)) select( uint16 a, uint16 b, uint16 c );
uint16 __attribute__((__overloadable__,__always_inline__,const)) select( uint16 a, uint16 b, int16 c );
long2 __attribute__((__overloadable__,__always_inline__,const)) select( long2 a, long2 b, ulong2 c );
long2 __attribute__((__overloadable__,__always_inline__,const)) select( long2 a, long2 b, long2 c );
#if __OPENCL_C_VERSION__ >= 110
long3 __attribute__((__overloadable__,__always_inline__,const)) select( long3 a, long3 b, ulong3 c );
long3 __attribute__((__overloadable__,__always_inline__,const)) select( long3 a, long3 b, long3 c );
#endif
long4 __attribute__((__overloadable__,__always_inline__,const)) select( long4 a, long4 b, ulong4 c );
long4 __attribute__((__overloadable__,__always_inline__,const)) select( long4 a, long4 b, long4 c );
long8 __attribute__((__overloadable__,__always_inline__,const)) select( long8 a, long8 b, ulong8 c );
long8 __attribute__((__overloadable__,__always_inline__,const)) select( long8 a, long8 b, long8 c );
long16 __attribute__((__overloadable__,__always_inline__,const)) select( long16 a, long16 b, ulong16 c );
long16 __attribute__((__overloadable__,__always_inline__,const)) select( long16 a, long16 b, long16 c );
ulong2 __attribute__((__overloadable__,__always_inline__,const)) select( ulong2 a, ulong2 b, ulong2 c );
ulong2 __attribute__((__overloadable__,__always_inline__,const)) select( ulong2 a, ulong2 b, long2 c );
#if __OPENCL_C_VERSION__ >= 110
ulong3 __attribute__((__overloadable__,__always_inline__,const)) select( ulong3 a, ulong3 b, ulong3 c );
ulong3 __attribute__((__overloadable__,__always_inline__,const)) select( ulong3 a, ulong3 b, long3 c );
#endif
ulong4 __attribute__((__overloadable__,__always_inline__,const)) select( ulong4 a, ulong4 b, ulong4 c );
ulong4 __attribute__((__overloadable__,__always_inline__,const)) select( ulong4 a, ulong4 b, long4 c );
ulong8 __attribute__((__overloadable__,__always_inline__,const)) select( ulong8 a, ulong8 b, ulong8 c );
ulong8 __attribute__((__overloadable__,__always_inline__,const)) select( ulong8 a, ulong8 b, long8 c );
ulong16 __attribute__((__overloadable__,__always_inline__,const)) select( ulong16 a, ulong16 b, ulong16 c );
ulong16 __attribute__((__overloadable__,__always_inline__,const)) select( ulong16 a, ulong16 b, long16 c );
float2 __attribute__((__overloadable__,__always_inline__,const)) select( float2 a, float2 b, uint2 c );
float2 __attribute__((__overloadable__,__always_inline__,const)) select( float2 a, float2 b, int2 c );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__,__always_inline__,const)) select( float3 a, float3 b, uint3 c );
float3 __attribute__((__overloadable__,__always_inline__,const)) select( float3 a, float3 b, int3 c );
#endif
float4 __attribute__((__overloadable__,__always_inline__,const)) select( float4 a, float4 b, uint4 c );
float4 __attribute__((__overloadable__,__always_inline__,const)) select( float4 a, float4 b, int4 c );
float8 __attribute__((__overloadable__,__always_inline__,const)) select( float8 a, float8 b, uint8 c );
float8 __attribute__((__overloadable__,__always_inline__,const)) select( float8 a, float8 b, int8 c );
float16 __attribute__((__overloadable__,__always_inline__,const)) select( float16 a, float16 b, uint16 c );
float16 __attribute__((__overloadable__,__always_inline__,const)) select( float16 a, float16 b, int16 c );
double2 __attribute__((__overloadable__,__always_inline__,const)) select( double2 a, double2 b, ulong2 c );
double2 __attribute__((__overloadable__,__always_inline__,const)) select( double2 a, double2 b, long2 c );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__,__always_inline__,const)) select( double3 a, double3 b, ulong3 c );
double3 __attribute__((__overloadable__,__always_inline__,const)) select( double3 a, double3 b, long3 c );
#endif
double4 __attribute__((__overloadable__,__always_inline__,const)) select( double4 a, double4 b, ulong4 c );
double4 __attribute__((__overloadable__,__always_inline__,const)) select( double4 a, double4 b, long4 c );
double8 __attribute__((__overloadable__,__always_inline__,const)) select( double8 a, double8 b, ulong8 c );
double8 __attribute__((__overloadable__,__always_inline__,const)) select( double8 a, double8 b, long8 c );
double16 __attribute__((__overloadable__,__always_inline__,const)) select( double16 a, double16 b, ulong16 c );
double16 __attribute__((__overloadable__,__always_inline__,const)) select( double16 a, double16 b, long16 c );
float __attribute__((__overloadable__,__always_inline__,const)) dot( float a, float b );
float __attribute__((__overloadable__,__always_inline__,const)) length( float p );
float __attribute__((__overloadable__,__always_inline__,const)) distance(float a, float b);
float __attribute__((__overloadable__,__always_inline__,const)) normalize( float p );
float __attribute__((__overloadable__,__always_inline__,const)) dot( float2 a, float2 b );
float __attribute__((__overloadable__,__always_inline__,const)) length(float2 p);
float __attribute__((__overloadable__,__always_inline__,const)) distance( float2 a,float2 b );
float2 __attribute__((__overloadable__,__always_inline__,const)) normalize(float2 p);
#if __OPENCL_C_VERSION__ >= 110
float __attribute__((__overloadable__,__always_inline__,const)) dot( float3 a, float3 b );
float __attribute__((__overloadable__,__always_inline__,const)) length(float3 p);
float __attribute__((__overloadable__,__always_inline__,const)) distance( float3 a,float3 b );
float3 __attribute__((__overloadable__,__always_inline__,const)) normalize(float3 p);
#endif
float __attribute__((__overloadable__,__always_inline__,const)) dot( float4 a, float4 b );
float __attribute__((__overloadable__,__always_inline__,const)) length(float4 p);
float __attribute__((__overloadable__,__always_inline__,const)) distance( float4 a,float4 b );
float4 __attribute__((__overloadable__,__always_inline__,const)) normalize(float4 p);
double __attribute__((__overloadable__,__always_inline__,const)) dot( double a, double b );
double __attribute__((__overloadable__,__always_inline__,const)) length( double p );
double __attribute__((__overloadable__,__always_inline__,const)) distance(double a, double b);
double __attribute__((__overloadable__,__always_inline__,const)) normalize( double p );
double __attribute__((__overloadable__,__always_inline__,const)) dot( double2 a, double2 b );
double __attribute__((__overloadable__,__always_inline__,const)) length(double2 p);
double __attribute__((__overloadable__,__always_inline__,const)) distance( double2 a,double2 b );
double2 __attribute__((__overloadable__,__always_inline__,const)) normalize(double2 p);
#if __OPENCL_C_VERSION__ >= 110
double __attribute__((__overloadable__,__always_inline__,const)) dot( double3 a, double3 b );
double __attribute__((__overloadable__,__always_inline__,const)) length(double3 p);
double __attribute__((__overloadable__,__always_inline__,const)) distance( double3 a,double3 b );
double3 __attribute__((__overloadable__,__always_inline__,const)) normalize(double3 p);
#endif
double __attribute__((__overloadable__,__always_inline__,const)) dot( double4 a, double4 b );
double __attribute__((__overloadable__,__always_inline__,const)) length(double4 p);
double __attribute__((__overloadable__,__always_inline__,const)) distance( double4 a,double4 b );
double4 __attribute__((__overloadable__,__always_inline__,const)) normalize(double4 p);
float __attribute__((__overloadable__,__always_inline__,const)) fast_length(float a);
float __attribute__((__overloadable__,__always_inline__,const)) fast_distance(float p0, float p1);
float __attribute__((__overloadable__,__always_inline__,const)) fast_normalize(float p);
float __attribute__((__overloadable__,__always_inline__,const)) fast_length(float2 a);
float __attribute__((__overloadable__,__always_inline__,const)) fast_distance(float2 p0, float2 p1 );
float2 __attribute__((__overloadable__,__always_inline__,const)) fast_normalize(float2 p);
#if __OPENCL_C_VERSION__ >= 110
float __attribute__((__overloadable__,__always_inline__,const)) fast_length(float3 a);
float __attribute__((__overloadable__,__always_inline__,const)) fast_distance(float3 p0, float3 p1 );
float3 __attribute__((__overloadable__,__always_inline__,const)) fast_normalize(float3 p);
#endif
float __attribute__((__overloadable__,__always_inline__,const)) fast_length(float4 a);
float __attribute__((__overloadable__,__always_inline__,const)) fast_distance(float4 p0, float4 p1 );
float4 __attribute__((__overloadable__,__always_inline__,const)) fast_normalize(float4 p);
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__,__always_inline__,const)) cross(float3 a, float3 b);
#endif
float4 __attribute__((__overloadable__,__always_inline__,const)) cross(float4 a, float4 b);
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__,__always_inline__,const)) cross(double3 a, double3 b);
#endif
double4 __attribute__((__overloadable__,__always_inline__,const)) cross(double4 a, double4 b);
float __attribute__((__overloadable__,__always_inline__,const)) mix(float x, float y, float a);
float2 __attribute__((__overloadable__,__always_inline__,const)) mix(float2 x, float2 y, float2 a);
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__,__always_inline__,const)) mix(float3 x, float3 y, float3 a);
#endif
float4 __attribute__((__overloadable__,__always_inline__,const)) mix(float4 x, float4 y, float4 a);
float8 __attribute__((__overloadable__,__always_inline__,const)) mix(float8 x, float8 y, float8 a);
float16 __attribute__((__overloadable__,__always_inline__,const)) mix(float16 x, float16 y, float16 a);
double __attribute__((__overloadable__,__always_inline__,const)) mix(double x, double y, double a);
double2 __attribute__((__overloadable__,__always_inline__,const)) mix(double2 x, double2 y, double2 a);
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__,__always_inline__,const)) mix(double3 x, double3 y, double3 a);
#endif
double4 __attribute__((__overloadable__,__always_inline__,const)) mix(double4 x, double4 y, double4 a);
double8 __attribute__((__overloadable__,__always_inline__,const)) mix(double8 x, double8 y, double8 a);
double16 __attribute__((__overloadable__,__always_inline__,const)) mix(double16 x, double16 y, double16 a);
int  __attribute__((__overloadable__,__always_inline__,const)) signbit( float x );
int2 __attribute__((__overloadable__,__always_inline__,const)) signbit(float2 x );
#if __OPENCL_C_VERSION__ >= 110
int3 __attribute__((__overloadable__,__always_inline__,const)) signbit(float3 x );
#endif
int4 __attribute__((__overloadable__,__always_inline__,const)) signbit(float4 x );
int8 __attribute__((__overloadable__,__always_inline__,const)) signbit(float8 x );
int16 __attribute__((__overloadable__,__always_inline__,const)) signbit(float16 x );
int  __attribute__((__overloadable__,__always_inline__,const)) signbit( double x );
long2 __attribute__((__overloadable__,__always_inline__,const)) signbit(double2 x );
#if __OPENCL_C_VERSION__ >= 110
long3 __attribute__((__overloadable__,__always_inline__,const)) signbit(double3 x );
#endif
long4 __attribute__((__overloadable__,__always_inline__,const)) signbit(double4 x );
long8 __attribute__((__overloadable__,__always_inline__,const)) signbit(double8 x );
long16 __attribute__((__overloadable__,__always_inline__,const)) signbit(double16 x );
float __attribute__((__overloadable__)) sign(float __x);
float2 __attribute__((__overloadable__)) sign(float2 __x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__)) sign(float3 __x );
#endif
float4 __attribute__((__overloadable__)) sign(float4 __x );
float8 __attribute__((__overloadable__)) sign(float8 __x );
float16 __attribute__((__overloadable__)) sign(float16 __x );
uchar __attribute__((__overloadable__,__always_inline__,const)) abs(char x);
uchar2 __attribute__((__overloadable__,__always_inline__,const)) abs(char2 x);
#if __OPENCL_C_VERSION__ >= 110
uchar3 __attribute__((__overloadable__,__always_inline__,const)) abs(char3 x);
#endif
uchar4 __attribute__((__overloadable__,__always_inline__,const)) abs(char4 x);
uchar8 __attribute__((__overloadable__,__always_inline__,const)) abs(char8 x);
uchar16 __attribute__((__overloadable__,__always_inline__,const)) abs(char16 x);
uchar __attribute__((__overloadable__,__always_inline__,const)) abs(uchar x);
uchar2 __attribute__((__overloadable__,__always_inline__,const)) abs(uchar2 x);
#if __OPENCL_C_VERSION__ >= 110
uchar3 __attribute__((__overloadable__,__always_inline__,const)) abs(uchar3 x);
#endif
uchar4 __attribute__((__overloadable__,__always_inline__,const)) abs(uchar4 x);
uchar8 __attribute__((__overloadable__,__always_inline__,const)) abs(uchar8 x);
uchar16 __attribute__((__overloadable__,__always_inline__,const)) abs(uchar16 x);
ushort __attribute__((__overloadable__,__always_inline__,const)) abs(short x);
ushort2 __attribute__((__overloadable__,__always_inline__,const)) abs(short2 x);
#if __OPENCL_C_VERSION__ >= 110
ushort3 __attribute__((__overloadable__,__always_inline__,const)) abs(short3 x);
#endif
ushort4 __attribute__((__overloadable__,__always_inline__,const)) abs(short4 x);
ushort8 __attribute__((__overloadable__,__always_inline__,const)) abs(short8 x);
ushort16 __attribute__((__overloadable__,__always_inline__,const)) abs(short16 x);
ushort __attribute__((__overloadable__,__always_inline__,const)) abs(ushort x);
ushort2 __attribute__((__overloadable__,__always_inline__,const)) abs(ushort2 x);
#if __OPENCL_C_VERSION__ >= 110
ushort3 __attribute__((__overloadable__,__always_inline__,const)) abs(ushort3 x);
#endif
ushort4 __attribute__((__overloadable__,__always_inline__,const)) abs(ushort4 x);
ushort8 __attribute__((__overloadable__,__always_inline__,const)) abs(ushort8 x);
ushort16 __attribute__((__overloadable__,__always_inline__,const)) abs(ushort16 x);
uint __attribute__((__overloadable__,__always_inline__,const)) abs(int x);
uint2 __attribute__((__overloadable__,__always_inline__,const)) abs(int2 x);
#if __OPENCL_C_VERSION__ >= 110
uint3 __attribute__((__overloadable__,__always_inline__,const)) abs(int3 x);
#endif
uint4 __attribute__((__overloadable__,__always_inline__,const)) abs(int4 x);
uint8 __attribute__((__overloadable__,__always_inline__,const)) abs(int8 x);
uint16 __attribute__((__overloadable__,__always_inline__,const)) abs(int16 x);
uint __attribute__((__overloadable__,__always_inline__,const)) abs(uint x);
uint2 __attribute__((__overloadable__,__always_inline__,const)) abs(uint2 x);
#if __OPENCL_C_VERSION__ >= 110
uint3 __attribute__((__overloadable__,__always_inline__,const)) abs(uint3 x);
#endif
uint4 __attribute__((__overloadable__,__always_inline__,const)) abs(uint4 x);
uint8 __attribute__((__overloadable__,__always_inline__,const)) abs(uint8 x);
uint16 __attribute__((__overloadable__,__always_inline__,const)) abs(uint16 x);
ulong __attribute__((__overloadable__,__always_inline__,const)) abs(long x);
ulong2 __attribute__((__overloadable__,__always_inline__,const)) abs(long2 x);
#if __OPENCL_C_VERSION__ >= 110
ulong3 __attribute__((__overloadable__,__always_inline__,const)) abs(long3 x);
#endif
ulong4 __attribute__((__overloadable__,__always_inline__,const)) abs(long4 x);
ulong8 __attribute__((__overloadable__,__always_inline__,const)) abs(long8 x);
ulong16 __attribute__((__overloadable__,__always_inline__,const)) abs(long16 x);
ulong __attribute__((__overloadable__,__always_inline__,const)) abs(ulong x);
ulong2 __attribute__((__overloadable__,__always_inline__,const)) abs(ulong2 x);
#if __OPENCL_C_VERSION__ >= 110
ulong3 __attribute__((__overloadable__,__always_inline__,const)) abs(ulong3 x);
#endif
ulong4 __attribute__((__overloadable__,__always_inline__,const)) abs(ulong4 x);
ulong8 __attribute__((__overloadable__,__always_inline__,const)) abs(ulong8 x);
ulong16 __attribute__((__overloadable__,__always_inline__,const)) abs(ulong16 x);
uchar __attribute__((__overloadable__,__always_inline__,const)) abs_diff(char x, char y );
uchar2 __attribute__((__overloadable__,__always_inline__,const)) abs_diff(char2 x, char2 y );
#if __OPENCL_C_VERSION__ >= 110
uchar3 __attribute__((__overloadable__,__always_inline__,const)) abs_diff(char3 x, char3 y );
#endif
uchar4 __attribute__((__overloadable__,__always_inline__,const)) abs_diff(char4 x, char4 y );
uchar8 __attribute__((__overloadable__,__always_inline__,const)) abs_diff(char8 x, char8 y );
uchar16 __attribute__((__overloadable__,__always_inline__,const)) abs_diff(char16 x, char16 y );
uchar __attribute__((__overloadable__,__always_inline__,const)) abs_diff(uchar x, uchar y );
uchar2 __attribute__((__overloadable__,__always_inline__,const)) abs_diff(uchar2 x, uchar2 y );
#if __OPENCL_C_VERSION__ >= 110
uchar3 __attribute__((__overloadable__,__always_inline__,const)) abs_diff(uchar3 x, uchar3 y );
#endif
uchar4 __attribute__((__overloadable__,__always_inline__,const)) abs_diff(uchar4 x, uchar4 y );
uchar8 __attribute__((__overloadable__,__always_inline__,const)) abs_diff(uchar8 x, uchar8 y );
uchar16 __attribute__((__overloadable__,__always_inline__,const)) abs_diff(uchar16 x, uchar16 y );
ushort __attribute__((__overloadable__,__always_inline__,const)) abs_diff(short x, short y );
ushort2 __attribute__((__overloadable__,__always_inline__,const)) abs_diff(short2 x, short2 y );
#if __OPENCL_C_VERSION__ >= 110
ushort3 __attribute__((__overloadable__,__always_inline__,const)) abs_diff(short3 x, short3 y );
#endif
ushort4 __attribute__((__overloadable__,__always_inline__,const)) abs_diff(short4 x, short4 y );
ushort8 __attribute__((__overloadable__,__always_inline__,const)) abs_diff(short8 x, short8 y );
ushort16 __attribute__((__overloadable__,__always_inline__,const)) abs_diff(short16 x, short16 y );
ushort __attribute__((__overloadable__,__always_inline__,const)) abs_diff(ushort x, ushort y );
ushort2 __attribute__((__overloadable__,__always_inline__,const)) abs_diff(ushort2 x, ushort2 y );
#if __OPENCL_C_VERSION__ >= 110
ushort3 __attribute__((__overloadable__,__always_inline__,const)) abs_diff(ushort3 x, ushort3 y );
#endif
ushort4 __attribute__((__overloadable__,__always_inline__,const)) abs_diff(ushort4 x, ushort4 y );
ushort8 __attribute__((__overloadable__,__always_inline__,const)) abs_diff(ushort8 x, ushort8 y );
ushort16 __attribute__((__overloadable__,__always_inline__,const)) abs_diff(ushort16 x, ushort16 y );
uint __attribute__((__overloadable__,__always_inline__,const)) abs_diff(int x, int y );
uint2 __attribute__((__overloadable__,__always_inline__,const)) abs_diff(int2 x, int2 y );
#if __OPENCL_C_VERSION__ >= 110
uint3 __attribute__((__overloadable__,__always_inline__,const)) abs_diff(int3 x, int3 y );
#endif
uint4 __attribute__((__overloadable__,__always_inline__,const)) abs_diff(int4 x, int4 y );
uint8 __attribute__((__overloadable__,__always_inline__,const)) abs_diff(int8 x, int8 y );
uint16 __attribute__((__overloadable__,__always_inline__,const)) abs_diff(int16 x, int16 y );
uint __attribute__((__overloadable__,__always_inline__,const)) abs_diff(uint x, uint y );
uint2 __attribute__((__overloadable__,__always_inline__,const)) abs_diff(uint2 x, uint2 y );
#if __OPENCL_C_VERSION__ >= 110
uint3 __attribute__((__overloadable__,__always_inline__,const)) abs_diff(uint3 x, uint3 y );
#endif
uint4 __attribute__((__overloadable__,__always_inline__,const)) abs_diff(uint4 x, uint4 y );
uint8 __attribute__((__overloadable__,__always_inline__,const)) abs_diff(uint8 x, uint8 y );
uint16 __attribute__((__overloadable__,__always_inline__,const)) abs_diff(uint16 x, uint16 y );
ulong __attribute__((__overloadable__,__always_inline__,const)) abs_diff(long x, long y );
ulong2 __attribute__((__overloadable__,__always_inline__,const)) abs_diff(long2 x, long2 y );
#if __OPENCL_C_VERSION__ >= 110
ulong3 __attribute__((__overloadable__,__always_inline__,const)) abs_diff(long3 x, long3 y );
#endif
ulong4 __attribute__((__overloadable__,__always_inline__,const)) abs_diff(long4 x, long4 y );
ulong8 __attribute__((__overloadable__,__always_inline__,const)) abs_diff(long8 x, long8 y );
ulong16 __attribute__((__overloadable__,__always_inline__,const)) abs_diff(long16 x, long16 y );
ulong __attribute__((__overloadable__,__always_inline__,const)) abs_diff(ulong x, ulong y );
ulong2 __attribute__((__overloadable__,__always_inline__,const)) abs_diff(ulong2 x, ulong2 y );
#if __OPENCL_C_VERSION__ >= 110
ulong3 __attribute__((__overloadable__,__always_inline__,const)) abs_diff(ulong3 x, ulong3 y );
#endif
ulong4 __attribute__((__overloadable__,__always_inline__,const)) abs_diff(ulong4 x, ulong4 y );
ulong8 __attribute__((__overloadable__,__always_inline__,const)) abs_diff(ulong8 x, ulong8 y );
ulong16 __attribute__((__overloadable__,__always_inline__,const)) abs_diff(ulong16 x, ulong16 y );
/* This is not real fma, because we do it in two operations for now. */
float __attribute__((__overloadable__,__always_inline__,const)) fma(float a, float b, float c );
float2 __attribute__((__overloadable__,__always_inline__,const)) fma(float2 a, float2 b, float2 c );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__,__always_inline__,const)) fma(float3 a, float3 b, float3 c );
#endif
float4 __attribute__((__overloadable__,__always_inline__,const)) fma(float4 a, float4 b, float4 c );
float8 __attribute__((__overloadable__,__always_inline__,const)) fma(float8 a, float8 b, float8 c );
float16 __attribute__((__overloadable__,__always_inline__,const)) fma(float16 a, float16 b, float16 c );
double __attribute__((__overloadable__,__always_inline__,const)) fma(double a, double b, double c );
double2 __attribute__((__overloadable__,__always_inline__,const)) fma(double2 a, double2 b, double2 c );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__,__always_inline__,const)) fma(double3 a, double3 b, double3 c );
#endif
double4 __attribute__((__overloadable__,__always_inline__,const)) fma(double4 a, double4 b, double4 c );
double8 __attribute__((__overloadable__,__always_inline__,const)) fma(double8 a, double8 b, double8 c );
double16 __attribute__((__overloadable__,__always_inline__,const)) fma(double16 a, double16 b, double16 c );
/* This is not real mad, because we do it in two operations for now. */
float __attribute__((__overloadable__,__always_inline__,const)) mad(float a, float b, float c );
float2 __attribute__((__overloadable__,__always_inline__,const)) mad(float2 a, float2 b, float2 c );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__,__always_inline__,const)) mad(float3 a, float3 b, float3 c );
#endif
float4 __attribute__((__overloadable__,__always_inline__,const)) mad(float4 a, float4 b, float4 c );
float8 __attribute__((__overloadable__,__always_inline__,const)) mad(float8 a, float8 b, float8 c );
float16 __attribute__((__overloadable__,__always_inline__,const)) mad(float16 a, float16 b, float16 c );
double __attribute__((__overloadable__,__always_inline__,const)) mad(double a, double b, double c );
double2 __attribute__((__overloadable__,__always_inline__,const)) mad(double2 a, double2 b, double2 c );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__,__always_inline__,const)) mad(double3 a, double3 b, double3 c );
#endif
double4 __attribute__((__overloadable__,__always_inline__,const)) mad(double4 a, double4 b, double4 c );
double8 __attribute__((__overloadable__,__always_inline__,const)) mad(double8 a, double8 b, double8 c );
double16 __attribute__((__overloadable__,__always_inline__,const)) mad(double16 a, double16 b, double16 c );
float __attribute__((__overloadable__,__always_inline__,const)) fmax(float x, float y );
float __attribute__((__overloadable__,__always_inline__,const)) fmax_common(float x, float y );
float __attribute__((__overloadable__,__always_inline__,const)) fmin(float x, float y );
float __attribute__((__overloadable__,__always_inline__,const)) fmin_common(float x, float y );
float2 __attribute__((__overloadable__,__always_inline__,const)) fmax(float2 x, float2 y );
float2 __attribute__((__overloadable__,__always_inline__,const)) fmax_common(float2 x, float2 y );
float2 __attribute__((__overloadable__,__always_inline__,const)) fmin(float2 x, float2 y );
float2 __attribute__((__overloadable__,__always_inline__,const)) fmin_common(float2 x, float2 y );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__,__always_inline__,const)) fmax(float3 x, float3 y );
float3 __attribute__((__overloadable__,__always_inline__,const)) fmax_common(float3 x, float3 y );
float3 __attribute__((__overloadable__,__always_inline__,const)) fmin(float3 x, float3 y );
float3 __attribute__((__overloadable__,__always_inline__,const)) fmin_common(float3 x, float3 y );
#endif
float4 __attribute__((__overloadable__,__always_inline__,const)) fmax(float4 x, float4 y );
float4 __attribute__((__overloadable__,__always_inline__,const)) fmax_common(float4 x, float4 y );
float4 __attribute__((__overloadable__,__always_inline__,const)) fmin(float4 x, float4 y );
float4 __attribute__((__overloadable__,__always_inline__,const)) fmin_common(float4 x, float4 y );
float8 __attribute__((__overloadable__,__always_inline__,const)) fmax(float8 x, float8 y );
float8 __attribute__((__overloadable__,__always_inline__,const)) fmax_common(float8 x, float8 y );
float8 __attribute__((__overloadable__,__always_inline__,const)) fmin(float8 x, float8 y );
float8 __attribute__((__overloadable__,__always_inline__,const)) fmin_common(float8 x, float8 y );
float16 __attribute__((__overloadable__,__always_inline__,const)) fmax(float16 x, float16 y );
float16 __attribute__((__overloadable__,__always_inline__,const)) fmax_common(float16 x, float16 y );
float16 __attribute__((__overloadable__,__always_inline__,const)) fmin(float16 x, float16 y );
float16 __attribute__((__overloadable__,__always_inline__,const)) fmin_common(float16 x, float16 y );
double __attribute__((__overloadable__,__always_inline__,const)) fmax(double x, double y );
double __attribute__((__overloadable__,__always_inline__,const)) fmax_common(double x, double y );
double __attribute__((__overloadable__,__always_inline__,const)) fmin(double x, double y );
double __attribute__((__overloadable__,__always_inline__,const)) fmin_common(double x, double y );
double2 __attribute__((__overloadable__,__always_inline__,const)) fmax(double2 x, double2 y );
double2 __attribute__((__overloadable__,__always_inline__,const)) fmax_common(double2 x, double2 y );
double2 __attribute__((__overloadable__,__always_inline__,const)) fmin(double2 x, double2 y );
double2 __attribute__((__overloadable__,__always_inline__,const)) fmin_common(double2 x, double2 y );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__,__always_inline__,const)) fmax(double3 x, double3 y );
double3 __attribute__((__overloadable__,__always_inline__,const)) fmax_common(double3 x, double3 y );
double3 __attribute__((__overloadable__,__always_inline__,const)) fmin(double3 x, double3 y );
double3 __attribute__((__overloadable__,__always_inline__,const)) fmin_common(double3 x, double3 y );
#endif
double4 __attribute__((__overloadable__,__always_inline__,const)) fmax(double4 x, double4 y );
double4 __attribute__((__overloadable__,__always_inline__,const)) fmax_common(double4 x, double4 y );
double4 __attribute__((__overloadable__,__always_inline__,const)) fmin(double4 x, double4 y );
double4 __attribute__((__overloadable__,__always_inline__,const)) fmin_common(double4 x, double4 y );
double8 __attribute__((__overloadable__,__always_inline__,const)) fmax(double8 x, double8 y );
double8 __attribute__((__overloadable__,__always_inline__,const)) fmax_common(double8 x, double8 y );
double8 __attribute__((__overloadable__,__always_inline__,const)) fmin(double8 x, double8 y );
double8 __attribute__((__overloadable__,__always_inline__,const)) fmin_common(double8 x, double8 y );
double16 __attribute__((__overloadable__,__always_inline__,const)) fmax(double16 x, double16 y );
double16 __attribute__((__overloadable__,__always_inline__,const)) fmax_common(double16 x, double16 y );
double16 __attribute__((__overloadable__,__always_inline__,const)) fmin(double16 x, double16 y );
double16 __attribute__((__overloadable__,__always_inline__,const)) fmin_common(double16 x, double16 y );
#if __OPENCL_C_VERSION__ >= 110
char __attribute__((__overloadable__,__always_inline__,const)) clamp(char x, char minval, char maxval );
char2 __attribute__((__overloadable__,__always_inline__,const)) clamp(char2 x, char2 minval, char2 maxval );
char2 __attribute__((__overloadable__,__always_inline__,const)) clamp(char2 x, char minval, char maxval );
char3 __attribute__((__overloadable__,__always_inline__,const)) clamp(char3 x, char3 minval, char3 maxval );
char3 __attribute__((__overloadable__,__always_inline__,const)) clamp(char3 x, char minval, char maxval );
char4 __attribute__((__overloadable__,__always_inline__,const)) clamp(char4 x, char4 minval, char4 maxval );
char4 __attribute__((__overloadable__,__always_inline__,const)) clamp(char4 x, char minval, char maxval );
char8 __attribute__((__overloadable__,__always_inline__,const)) clamp(char8 x, char8 minval, char8 maxval );
char8 __attribute__((__overloadable__,__always_inline__,const)) clamp(char8 x, char minval, char maxval );
char16 __attribute__((__overloadable__,__always_inline__,const)) clamp(char16 x, char16 minval, char16 maxval );
char16 __attribute__((__overloadable__,__always_inline__,const)) clamp(char16 x, char minval, char maxval );
uchar __attribute__((__overloadable__,__always_inline__,const)) clamp(uchar x, uchar minval, uchar maxval );
uchar2 __attribute__((__overloadable__,__always_inline__,const)) clamp(uchar2 x, uchar2 minval, uchar2 maxval );
uchar2 __attribute__((__overloadable__,__always_inline__,const)) clamp(uchar2 x, uchar minval, uchar maxval );
uchar3 __attribute__((__overloadable__,__always_inline__,const)) clamp(uchar3 x, uchar3 minval, uchar3 maxval );
uchar3 __attribute__((__overloadable__,__always_inline__,const)) clamp(uchar3 x, uchar minval, uchar maxval );
uchar4 __attribute__((__overloadable__,__always_inline__,const)) clamp(uchar4 x, uchar4 minval, uchar4 maxval );
uchar4 __attribute__((__overloadable__,__always_inline__,const)) clamp(uchar4 x, uchar minval, uchar maxval );
uchar8 __attribute__((__overloadable__,__always_inline__,const)) clamp(uchar8 x, uchar8 minval, uchar8 maxval );
uchar8 __attribute__((__overloadable__,__always_inline__,const)) clamp(uchar8 x, uchar minval, uchar maxval );
uchar16 __attribute__((__overloadable__,__always_inline__,const)) clamp(uchar16 x, uchar16 minval, uchar16 maxval );
uchar16 __attribute__((__overloadable__,__always_inline__,const)) clamp(uchar16 x, uchar minval, uchar maxval );
short __attribute__((__overloadable__,__always_inline__,const)) clamp(short x, short minval, short maxval );
short2 __attribute__((__overloadable__,__always_inline__,const)) clamp(short2 x, short2 minval, short2 maxval );
short2 __attribute__((__overloadable__,__always_inline__,const)) clamp(short2 x, short minval, short maxval );
short3 __attribute__((__overloadable__,__always_inline__,const)) clamp(short3 x, short3 minval, short3 maxval );
short3 __attribute__((__overloadable__,__always_inline__,const)) clamp(short3 x, short minval, short maxval );
short4 __attribute__((__overloadable__,__always_inline__,const)) clamp(short4 x, short4 minval, short4 maxval );
short4 __attribute__((__overloadable__,__always_inline__,const)) clamp(short4 x, short minval, short maxval );
short8 __attribute__((__overloadable__,__always_inline__,const)) clamp(short8 x, short8 minval, short8 maxval );
short8 __attribute__((__overloadable__,__always_inline__,const)) clamp(short8 x, short minval, short maxval );
short16 __attribute__((__overloadable__,__always_inline__,const)) clamp(short16 x, short16 minval, short16 maxval );
short16 __attribute__((__overloadable__,__always_inline__,const)) clamp(short16 x, short minval, short maxval );
ushort __attribute__((__overloadable__,__always_inline__,const)) clamp(ushort x, ushort minval, ushort maxval );
ushort2 __attribute__((__overloadable__,__always_inline__,const)) clamp(ushort2 x, ushort2 minval, ushort2 maxval );
ushort2 __attribute__((__overloadable__,__always_inline__,const)) clamp(ushort2 x, ushort minval, ushort maxval );
ushort3 __attribute__((__overloadable__,__always_inline__,const)) clamp(ushort3 x, ushort3 minval, ushort3 maxval );
ushort3 __attribute__((__overloadable__,__always_inline__,const)) clamp(ushort3 x, ushort minval, ushort maxval );
ushort4 __attribute__((__overloadable__,__always_inline__,const)) clamp(ushort4 x, ushort4 minval, ushort4 maxval );
ushort4 __attribute__((__overloadable__,__always_inline__,const)) clamp(ushort4 x, ushort minval, ushort maxval );
ushort8 __attribute__((__overloadable__,__always_inline__,const)) clamp(ushort8 x, ushort8 minval, ushort8 maxval );
ushort8 __attribute__((__overloadable__,__always_inline__,const)) clamp(ushort8 x, ushort minval, ushort maxval );
ushort16 __attribute__((__overloadable__,__always_inline__,const)) clamp(ushort16 x, ushort16 minval, ushort16 maxval );
ushort16 __attribute__((__overloadable__,__always_inline__,const)) clamp(ushort16 x, ushort minval, ushort maxval );
int __attribute__((__overloadable__,__always_inline__,const)) clamp(int x, int minval, int maxval );
int2 __attribute__((__overloadable__,__always_inline__,const)) clamp(int2 x, int2 minval, int2 maxval );
int2 __attribute__((__overloadable__,__always_inline__,const)) clamp(int2 x, int minval, int maxval );
int3 __attribute__((__overloadable__,__always_inline__,const)) clamp(int3 x, int3 minval, int3 maxval );
int3 __attribute__((__overloadable__,__always_inline__,const)) clamp(int3 x, int minval, int maxval );
int4 __attribute__((__overloadable__,__always_inline__,const)) clamp(int4 x, int4 minval, int4 maxval );
int4 __attribute__((__overloadable__,__always_inline__,const)) clamp(int4 x, int minval, int maxval );
int8 __attribute__((__overloadable__,__always_inline__,const)) clamp(int8 x, int8 minval, int8 maxval );
int8 __attribute__((__overloadable__,__always_inline__,const)) clamp(int8 x, int minval, int maxval );
int16 __attribute__((__overloadable__,__always_inline__,const)) clamp(int16 x, int16 minval, int16 maxval );
int16 __attribute__((__overloadable__,__always_inline__,const)) clamp(int16 x, int minval, int maxval );
uint __attribute__((__overloadable__,__always_inline__,const)) clamp(uint x, uint minval, uint maxval );
uint2 __attribute__((__overloadable__,__always_inline__,const)) clamp(uint2 x, uint2 minval, uint2 maxval );
uint2 __attribute__((__overloadable__,__always_inline__,const)) clamp(uint2 x, uint minval, uint maxval );
uint3 __attribute__((__overloadable__,__always_inline__,const)) clamp(uint3 x, uint3 minval, uint3 maxval );
uint3 __attribute__((__overloadable__,__always_inline__,const)) clamp(uint3 x, uint minval, uint maxval );
uint4 __attribute__((__overloadable__,__always_inline__,const)) clamp(uint4 x, uint4 minval, uint4 maxval );
uint4 __attribute__((__overloadable__,__always_inline__,const)) clamp(uint4 x, uint minval, uint maxval );
uint8 __attribute__((__overloadable__,__always_inline__,const)) clamp(uint8 x, uint8 minval, uint8 maxval );
uint8 __attribute__((__overloadable__,__always_inline__,const)) clamp(uint8 x, uint minval, uint maxval );
uint16 __attribute__((__overloadable__,__always_inline__,const)) clamp(uint16 x, uint16 minval, uint16 maxval );
uint16 __attribute__((__overloadable__,__always_inline__,const)) clamp(uint16 x, uint minval, uint maxval );
long __attribute__((__overloadable__,__always_inline__,const)) clamp(long x, long minval, long maxval );
long2 __attribute__((__overloadable__,__always_inline__,const)) clamp(long2 x, long2 minval, long2 maxval );
long2 __attribute__((__overloadable__,__always_inline__,const)) clamp(long2 x, long minval, long maxval );
long3 __attribute__((__overloadable__,__always_inline__,const)) clamp(long3 x, long3 minval, long3 maxval );
long3 __attribute__((__overloadable__,__always_inline__,const)) clamp(long3 x, long minval, long maxval );
long4 __attribute__((__overloadable__,__always_inline__,const)) clamp(long4 x, long4 minval, long4 maxval );
long4 __attribute__((__overloadable__,__always_inline__,const)) clamp(long4 x, long minval, long maxval );
long8 __attribute__((__overloadable__,__always_inline__,const)) clamp(long8 x, long8 minval, long8 maxval );
long8 __attribute__((__overloadable__,__always_inline__,const)) clamp(long8 x, long minval, long maxval );
long16 __attribute__((__overloadable__,__always_inline__,const)) clamp(long16 x, long16 minval, long16 maxval );
long16 __attribute__((__overloadable__,__always_inline__,const)) clamp(long16 x, long minval, long maxval );
ulong __attribute__((__overloadable__,__always_inline__,const)) clamp(ulong x, ulong minval, ulong maxval );
ulong2 __attribute__((__overloadable__,__always_inline__,const)) clamp(ulong2 x, ulong2 minval, ulong2 maxval );
ulong2 __attribute__((__overloadable__,__always_inline__,const)) clamp(ulong2 x, ulong minval, ulong maxval );
ulong3 __attribute__((__overloadable__,__always_inline__,const)) clamp(ulong3 x, ulong3 minval, ulong3 maxval );
ulong3 __attribute__((__overloadable__,__always_inline__,const)) clamp(ulong3 x, ulong minval, ulong maxval );
ulong4 __attribute__((__overloadable__,__always_inline__,const)) clamp(ulong4 x, ulong4 minval, ulong4 maxval );
ulong4 __attribute__((__overloadable__,__always_inline__,const)) clamp(ulong4 x, ulong minval, ulong maxval );
ulong8 __attribute__((__overloadable__,__always_inline__,const)) clamp(ulong8 x, ulong8 minval, ulong8 maxval );
ulong8 __attribute__((__overloadable__,__always_inline__,const)) clamp(ulong8 x, ulong minval, ulong maxval );
ulong16 __attribute__((__overloadable__,__always_inline__,const)) clamp(ulong16 x, ulong16 minval, ulong16 maxval );
ulong16 __attribute__((__overloadable__,__always_inline__,const)) clamp(ulong16 x, ulong minval, ulong maxval );
float __attribute__((__overloadable__,__always_inline__,const)) clamp(float x, float minval, float maxval );
float2 __attribute__((__overloadable__,__always_inline__,const)) clamp(float2 x, float2 minval, float2 maxval );
float2 __attribute__((__overloadable__,__always_inline__,const)) clamp(float2 x, float minval, float maxval );
float3 __attribute__((__overloadable__,__always_inline__,const)) clamp(float3 x, float3 minval, float3 maxval );
float3 __attribute__((__overloadable__,__always_inline__,const)) clamp(float3 x, float minval, float maxval );
float4 __attribute__((__overloadable__,__always_inline__,const)) clamp(float4 x, float4 minval, float4 maxval );
float4 __attribute__((__overloadable__,__always_inline__,const)) clamp(float4 x, float minval, float maxval );
float8 __attribute__((__overloadable__,__always_inline__,const)) clamp(float8 x, float8 minval, float8 maxval );
float8 __attribute__((__overloadable__,__always_inline__,const)) clamp(float8 x, float minval, float maxval );
float16 __attribute__((__overloadable__,__always_inline__,const)) clamp(float16 x, float16 minval, float16 maxval );
float16 __attribute__((__overloadable__,__always_inline__,const)) clamp(float16 x, float minval, float maxval );
double __attribute__((__overloadable__,__always_inline__,const)) clamp(double x, double minval, double maxval );
double2 __attribute__((__overloadable__,__always_inline__,const)) clamp(double2 x, double2 minval, double2 maxval );
double2 __attribute__((__overloadable__,__always_inline__,const)) clamp(double2 x, double minval, double maxval );
double3 __attribute__((__overloadable__,__always_inline__,const)) clamp(double3 x, double3 minval, double3 maxval );
double3 __attribute__((__overloadable__,__always_inline__,const)) clamp(double3 x, double minval, double maxval );
double4 __attribute__((__overloadable__,__always_inline__,const)) clamp(double4 x, double4 minval, double4 maxval );
double4 __attribute__((__overloadable__,__always_inline__,const)) clamp(double4 x, double minval, double maxval );
double8 __attribute__((__overloadable__,__always_inline__,const)) clamp(double8 x, double8 minval, double8 maxval );
double8 __attribute__((__overloadable__,__always_inline__,const)) clamp(double8 x, double minval, double maxval );
double16 __attribute__((__overloadable__,__always_inline__,const)) clamp(double16 x, double16 minval, double16 maxval );
double16 __attribute__((__overloadable__,__always_inline__,const)) clamp(double16 x, double minval, double maxval );
#endif
float __attribute__((__overloadable__,__always_inline__,const)) smoothstep(float edge0, float edge1, float x );
float2 __attribute__((__overloadable__,__always_inline__,const)) smoothstep(float2 edge0, float2 edge1, float2 x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__,__always_inline__,const)) smoothstep(float3 edge0, float3 edge1, float3 x );
#endif
float4 __attribute__((__overloadable__,__always_inline__,const)) smoothstep(float4 edge0, float4 edge1, float4 x );
float8 __attribute__((__overloadable__,__always_inline__,const)) smoothstep(float8 edge0, float8 edge1, float8 x );
float16 __attribute__((__overloadable__,__always_inline__,const)) smoothstep(float16 edge0, float16 edge1, float16 x );
double __attribute__((__overloadable__,__always_inline__,const)) smoothstep(double edge0, double edge1, double x );
double2 __attribute__((__overloadable__,__always_inline__,const)) smoothstep(double2 edge0, double2 edge1, double2 x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__,__always_inline__,const)) smoothstep(double3 edge0, double3 edge1, double3 x );
#endif
double4 __attribute__((__overloadable__,__always_inline__,const)) smoothstep(double4 edge0, double4 edge1, double4 x );
double8 __attribute__((__overloadable__,__always_inline__,const)) smoothstep(double8 edge0, double8 edge1, double8 x );
double16 __attribute__((__overloadable__,__always_inline__,const)) smoothstep(double16 edge0, double16 edge1, double16 x );
float __attribute__((__overloadable__,__always_inline__,const)) native_cos( float x );
float2 __attribute__((__overloadable__,__always_inline__,const)) native_cos( float2 x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__,__always_inline__,const)) native_cos( float3 x );
#endif
float4 __attribute__((__overloadable__,__always_inline__,const)) native_cos( float4 x );
float8 __attribute__((__overloadable__,__always_inline__,const)) native_cos( float8 x );
float16 __attribute__((__overloadable__,__always_inline__,const)) native_cos( float16 x );
double __attribute__((__overloadable__,__always_inline__,const)) native_cos( double x );
double2 __attribute__((__overloadable__,__always_inline__,const)) native_cos( double2 x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__,__always_inline__,const)) native_cos( double3 x );
#endif
double4 __attribute__((__overloadable__,__always_inline__,const)) native_cos( double4 x );
double8 __attribute__((__overloadable__,__always_inline__,const)) native_cos( double8 x );
double16 __attribute__((__overloadable__,__always_inline__,const)) native_cos( double16 x );
float __attribute__((__overloadable__,__always_inline__,const)) native_exp( float x );
float2 __attribute__((__overloadable__,__always_inline__,const)) native_exp( float2 x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__,__always_inline__,const)) native_exp( float3 x );
#endif
float4 __attribute__((__overloadable__,__always_inline__,const)) native_exp( float4 x );
float8 __attribute__((__overloadable__,__always_inline__,const)) native_exp( float8 x );
float16 __attribute__((__overloadable__,__always_inline__,const)) native_exp( float16 x );
double __attribute__((__overloadable__,__always_inline__,const)) native_exp( double x );
double2 __attribute__((__overloadable__,__always_inline__,const)) native_exp( double2 x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__,__always_inline__,const)) native_exp( double3 x );
#endif
double4 __attribute__((__overloadable__,__always_inline__,const)) native_exp( double4 x );
double8 __attribute__((__overloadable__,__always_inline__,const)) native_exp( double8 x );
double16 __attribute__((__overloadable__,__always_inline__,const)) native_exp( double16 x );
float __attribute__((__overloadable__,__always_inline__,const)) native_exp2( float x );
float2 __attribute__((__overloadable__,__always_inline__,const)) native_exp2( float2 x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__,__always_inline__,const)) native_exp2( float3 x );
#endif
float4 __attribute__((__overloadable__,__always_inline__,const)) native_exp2( float4 x );
float8 __attribute__((__overloadable__,__always_inline__,const)) native_exp2( float8 x );
float16 __attribute__((__overloadable__,__always_inline__,const)) native_exp2( float16 x );
double __attribute__((__overloadable__,__always_inline__,const)) native_exp2( double x );
double2 __attribute__((__overloadable__,__always_inline__,const)) native_exp2( double2 x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__,__always_inline__,const)) native_exp2( double3 x );
#endif
double4 __attribute__((__overloadable__,__always_inline__,const)) native_exp2( double4 x );
double8 __attribute__((__overloadable__,__always_inline__,const)) native_exp2( double8 x );
double16 __attribute__((__overloadable__,__always_inline__,const)) native_exp2( double16 x );
float __attribute__((__overloadable__,__always_inline__,const)) native_exp10( float x );
float2 __attribute__((__overloadable__,__always_inline__,const)) native_exp10( float2 x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__,__always_inline__,const)) native_exp10( float3 x );
#endif
float4 __attribute__((__overloadable__,__always_inline__,const)) native_exp10( float4 x );
float8 __attribute__((__overloadable__,__always_inline__,const)) native_exp10( float8 x );
float16 __attribute__((__overloadable__,__always_inline__,const)) native_exp10( float16 x );
double __attribute__((__overloadable__,__always_inline__,const)) native_exp10( double x );
double2 __attribute__((__overloadable__,__always_inline__,const)) native_exp10( double2 x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__,__always_inline__,const)) native_exp10( double3 x );
#endif
double4 __attribute__((__overloadable__,__always_inline__,const)) native_exp10( double4 x );
double8 __attribute__((__overloadable__,__always_inline__,const)) native_exp10( double8 x );
double16 __attribute__((__overloadable__,__always_inline__,const)) native_exp10( double16 x );
float __attribute__((__overloadable__,__always_inline__,const)) native_log( float x );
float2 __attribute__((__overloadable__,__always_inline__,const)) native_log( float2 x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__,__always_inline__,const)) native_log( float3 x );
#endif
float4 __attribute__((__overloadable__,__always_inline__,const)) native_log( float4 x );
float8 __attribute__((__overloadable__,__always_inline__,const)) native_log( float8 x );
float16 __attribute__((__overloadable__,__always_inline__,const)) native_log( float16 x );
double __attribute__((__overloadable__,__always_inline__,const)) native_log( double x );
double2 __attribute__((__overloadable__,__always_inline__,const)) native_log( double2 x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__,__always_inline__,const)) native_log( double3 x );
#endif
double4 __attribute__((__overloadable__,__always_inline__,const)) native_log( double4 x );
double8 __attribute__((__overloadable__,__always_inline__,const)) native_log( double8 x );
double16 __attribute__((__overloadable__,__always_inline__,const)) native_log( double16 x );
float __attribute__((__overloadable__,__always_inline__,const)) native_log2( float x );
float2 __attribute__((__overloadable__,__always_inline__,const)) native_log2( float2 x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__,__always_inline__,const)) native_log2( float3 x );
#endif
float4 __attribute__((__overloadable__,__always_inline__,const)) native_log2( float4 x );
float8 __attribute__((__overloadable__,__always_inline__,const)) native_log2( float8 x );
float16 __attribute__((__overloadable__,__always_inline__,const)) native_log2( float16 x );
double __attribute__((__overloadable__,__always_inline__,const)) native_log2( double x );
double2 __attribute__((__overloadable__,__always_inline__,const)) native_log2( double2 x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__,__always_inline__,const)) native_log2( double3 x );
#endif
double4 __attribute__((__overloadable__,__always_inline__,const)) native_log2( double4 x );
double8 __attribute__((__overloadable__,__always_inline__,const)) native_log2( double8 x );
double16 __attribute__((__overloadable__,__always_inline__,const)) native_log2( double16 x );
float __attribute__((__overloadable__,__always_inline__,const)) native_log10( float x );
float2 __attribute__((__overloadable__,__always_inline__,const)) native_log10( float2 x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__,__always_inline__,const)) native_log10( float3 x );
#endif
float4 __attribute__((__overloadable__,__always_inline__,const)) native_log10( float4 x );
float8 __attribute__((__overloadable__,__always_inline__,const)) native_log10( float8 x );
float16 __attribute__((__overloadable__,__always_inline__,const)) native_log10( float16 x );
double __attribute__((__overloadable__,__always_inline__,const)) native_log10( double x );
double2 __attribute__((__overloadable__,__always_inline__,const)) native_log10( double2 x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__,__always_inline__,const)) native_log10( double3 x );
#endif
double4 __attribute__((__overloadable__,__always_inline__,const)) native_log10( double4 x );
double8 __attribute__((__overloadable__,__always_inline__,const)) native_log10( double8 x );
double16 __attribute__((__overloadable__,__always_inline__,const)) native_log10( double16 x );
float __attribute__((__overloadable__,__always_inline__,const)) native_rsqrt( float x );
float2 __attribute__((__overloadable__,__always_inline__,const)) native_rsqrt( float2 x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__,__always_inline__,const)) native_rsqrt( float3 x );
#endif
float4 __attribute__((__overloadable__,__always_inline__,const)) native_rsqrt( float4 x );
float8 __attribute__((__overloadable__,__always_inline__,const)) native_rsqrt( float8 x );
float16 __attribute__((__overloadable__,__always_inline__,const)) native_rsqrt( float16 x );
double __attribute__((__overloadable__,__always_inline__,const)) native_rsqrt( double x );
double2 __attribute__((__overloadable__,__always_inline__,const)) native_rsqrt( double2 x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__,__always_inline__,const)) native_rsqrt( double3 x );
#endif
double4 __attribute__((__overloadable__,__always_inline__,const)) native_rsqrt( double4 x );
double8 __attribute__((__overloadable__,__always_inline__,const)) native_rsqrt( double8 x );
double16 __attribute__((__overloadable__,__always_inline__,const)) native_rsqrt( double16 x );
float __attribute__((__overloadable__,__always_inline__,const)) native_sin( float x );
float2 __attribute__((__overloadable__,__always_inline__,const)) native_sin( float2 x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__,__always_inline__,const)) native_sin( float3 x );
#endif
float4 __attribute__((__overloadable__,__always_inline__,const)) native_sin( float4 x );
float8 __attribute__((__overloadable__,__always_inline__,const)) native_sin( float8 x );
float16 __attribute__((__overloadable__,__always_inline__,const)) native_sin( float16 x );
double __attribute__((__overloadable__,__always_inline__,const)) native_sin( double x );
double2 __attribute__((__overloadable__,__always_inline__,const)) native_sin( double2 x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__,__always_inline__,const)) native_sin( double3 x );
#endif
double4 __attribute__((__overloadable__,__always_inline__,const)) native_sin( double4 x );
double8 __attribute__((__overloadable__,__always_inline__,const)) native_sin( double8 x );
double16 __attribute__((__overloadable__,__always_inline__,const)) native_sin( double16 x );
float __attribute__((__overloadable__,__always_inline__,const)) native_sqrt( float x );
float2 __attribute__((__overloadable__,__always_inline__,const)) native_sqrt( float2 x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__,__always_inline__,const)) native_sqrt( float3 x );
#endif
float4 __attribute__((__overloadable__,__always_inline__,const)) native_sqrt( float4 x );
float8 __attribute__((__overloadable__,__always_inline__,const)) native_sqrt( float8 x );
float16 __attribute__((__overloadable__,__always_inline__,const)) native_sqrt( float16 x );
double __attribute__((__overloadable__,__always_inline__,const)) native_sqrt( double x );
double2 __attribute__((__overloadable__,__always_inline__,const)) native_sqrt( double2 x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__,__always_inline__,const)) native_sqrt( double3 x );
#endif
double4 __attribute__((__overloadable__,__always_inline__,const)) native_sqrt( double4 x );
double8 __attribute__((__overloadable__,__always_inline__,const)) native_sqrt( double8 x );
double16 __attribute__((__overloadable__,__always_inline__,const)) native_sqrt( double16 x );
float __attribute__((__overloadable__,__always_inline__,const)) native_tan( float x );
float2 __attribute__((__overloadable__,__always_inline__,const)) native_tan( float2 x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__,__always_inline__,const)) native_tan( float3 x );
#endif
float4 __attribute__((__overloadable__,__always_inline__,const)) native_tan( float4 x );
float8 __attribute__((__overloadable__,__always_inline__,const)) native_tan( float8 x );
float16 __attribute__((__overloadable__,__always_inline__,const)) native_tan( float16 x );
double __attribute__((__overloadable__,__always_inline__,const)) native_tan( double x );
double2 __attribute__((__overloadable__,__always_inline__,const)) native_tan( double2 x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__,__always_inline__,const)) native_tan( double3 x );
#endif
double4 __attribute__((__overloadable__,__always_inline__,const)) native_tan( double4 x );
double8 __attribute__((__overloadable__,__always_inline__,const)) native_tan( double8 x );
double16 __attribute__((__overloadable__,__always_inline__,const)) native_tan( double16 x );
#define __CVAttrs __attribute__((__overloadable__,__always_inline__,const))
char __CVAttrs convert_char(char);
char __CVAttrs convert_char(uchar);
char __CVAttrs convert_char(short);
char __CVAttrs convert_char(ushort);
char __CVAttrs convert_char(int);
char __CVAttrs convert_char(uint);
char __CVAttrs convert_char(long);
char __CVAttrs convert_char(ulong);
char __CVAttrs convert_char(float);
char __CVAttrs convert_char(double);
char __CVAttrs convert_char_rte(char);
char __CVAttrs convert_char_rte(uchar);
char __CVAttrs convert_char_rte(short);
char __CVAttrs convert_char_rte(ushort);
char __CVAttrs convert_char_rte(int);
char __CVAttrs convert_char_rte(uint);
char __CVAttrs convert_char_rte(long);
char __CVAttrs convert_char_rte(ulong);
char __CVAttrs convert_char_rte(float);
char __CVAttrs convert_char_rte(double);
char __CVAttrs convert_char_rtz(char);
char __CVAttrs convert_char_rtz(uchar);
char __CVAttrs convert_char_rtz(short);
char __CVAttrs convert_char_rtz(ushort);
char __CVAttrs convert_char_rtz(int);
char __CVAttrs convert_char_rtz(uint);
char __CVAttrs convert_char_rtz(long);
char __CVAttrs convert_char_rtz(ulong);
char __CVAttrs convert_char_rtz(float);
char __CVAttrs convert_char_rtz(double);
char __CVAttrs convert_char_rtp(char);
char __CVAttrs convert_char_rtp(uchar);
char __CVAttrs convert_char_rtp(short);
char __CVAttrs convert_char_rtp(ushort);
char __CVAttrs convert_char_rtp(int);
char __CVAttrs convert_char_rtp(uint);
char __CVAttrs convert_char_rtp(long);
char __CVAttrs convert_char_rtp(ulong);
char __CVAttrs convert_char_rtp(float);
char __CVAttrs convert_char_rtp(double);
char __CVAttrs convert_char_rtn(char);
char __CVAttrs convert_char_rtn(uchar);
char __CVAttrs convert_char_rtn(short);
char __CVAttrs convert_char_rtn(ushort);
char __CVAttrs convert_char_rtn(int);
char __CVAttrs convert_char_rtn(uint);
char __CVAttrs convert_char_rtn(long);
char __CVAttrs convert_char_rtn(ulong);
char __CVAttrs convert_char_rtn(float);
char __CVAttrs convert_char_rtn(double);
char __CVAttrs convert_char_sat(char);
char __CVAttrs convert_char_sat(uchar);
char __CVAttrs convert_char_sat(short);
char __CVAttrs convert_char_sat(ushort);
char __CVAttrs convert_char_sat(int);
char __CVAttrs convert_char_sat(uint);
char __CVAttrs convert_char_sat(long);
char __CVAttrs convert_char_sat(ulong);
char __CVAttrs convert_char_sat(float);
char __CVAttrs convert_char_sat(double);
char __CVAttrs convert_char_sat_rte(char);
char __CVAttrs convert_char_sat_rte(uchar);
char __CVAttrs convert_char_sat_rte(short);
char __CVAttrs convert_char_sat_rte(ushort);
char __CVAttrs convert_char_sat_rte(int);
char __CVAttrs convert_char_sat_rte(uint);
char __CVAttrs convert_char_sat_rte(long);
char __CVAttrs convert_char_sat_rte(ulong);
char __CVAttrs convert_char_sat_rte(float);
char __CVAttrs convert_char_sat_rte(double);
char __CVAttrs convert_char_sat_rtz(char);
char __CVAttrs convert_char_sat_rtz(uchar);
char __CVAttrs convert_char_sat_rtz(short);
char __CVAttrs convert_char_sat_rtz(ushort);
char __CVAttrs convert_char_sat_rtz(int);
char __CVAttrs convert_char_sat_rtz(uint);
char __CVAttrs convert_char_sat_rtz(long);
char __CVAttrs convert_char_sat_rtz(ulong);
char __CVAttrs convert_char_sat_rtz(float);
char __CVAttrs convert_char_sat_rtz(double);
char __CVAttrs convert_char_sat_rtp(char);
char __CVAttrs convert_char_sat_rtp(uchar);
char __CVAttrs convert_char_sat_rtp(short);
char __CVAttrs convert_char_sat_rtp(ushort);
char __CVAttrs convert_char_sat_rtp(int);
char __CVAttrs convert_char_sat_rtp(uint);
char __CVAttrs convert_char_sat_rtp(long);
char __CVAttrs convert_char_sat_rtp(ulong);
char __CVAttrs convert_char_sat_rtp(float);
char __CVAttrs convert_char_sat_rtp(double);
char __CVAttrs convert_char_sat_rtn(char);
char __CVAttrs convert_char_sat_rtn(uchar);
char __CVAttrs convert_char_sat_rtn(short);
char __CVAttrs convert_char_sat_rtn(ushort);
char __CVAttrs convert_char_sat_rtn(int);
char __CVAttrs convert_char_sat_rtn(uint);
char __CVAttrs convert_char_sat_rtn(long);
char __CVAttrs convert_char_sat_rtn(ulong);
char __CVAttrs convert_char_sat_rtn(float);
char __CVAttrs convert_char_sat_rtn(double);
char2 __CVAttrs convert_char2(char2);
char2 __CVAttrs convert_char2(uchar2);
char2 __CVAttrs convert_char2(short2);
char2 __CVAttrs convert_char2(ushort2);
char2 __CVAttrs convert_char2(int2);
char2 __CVAttrs convert_char2(uint2);
char2 __CVAttrs convert_char2(long2);
char2 __CVAttrs convert_char2(ulong2);
char2 __CVAttrs convert_char2(float2);
char2 __CVAttrs convert_char2(double2);
char2 __CVAttrs convert_char2_rte(char2);
char2 __CVAttrs convert_char2_rte(uchar2);
char2 __CVAttrs convert_char2_rte(short2);
char2 __CVAttrs convert_char2_rte(ushort2);
char2 __CVAttrs convert_char2_rte(int2);
char2 __CVAttrs convert_char2_rte(uint2);
char2 __CVAttrs convert_char2_rte(long2);
char2 __CVAttrs convert_char2_rte(ulong2);
char2 __CVAttrs convert_char2_rte(float2);
char2 __CVAttrs convert_char2_rte(double2);
char2 __CVAttrs convert_char2_rtz(char2);
char2 __CVAttrs convert_char2_rtz(uchar2);
char2 __CVAttrs convert_char2_rtz(short2);
char2 __CVAttrs convert_char2_rtz(ushort2);
char2 __CVAttrs convert_char2_rtz(int2);
char2 __CVAttrs convert_char2_rtz(uint2);
char2 __CVAttrs convert_char2_rtz(long2);
char2 __CVAttrs convert_char2_rtz(ulong2);
char2 __CVAttrs convert_char2_rtz(float2);
char2 __CVAttrs convert_char2_rtz(double2);
char2 __CVAttrs convert_char2_rtp(char2);
char2 __CVAttrs convert_char2_rtp(uchar2);
char2 __CVAttrs convert_char2_rtp(short2);
char2 __CVAttrs convert_char2_rtp(ushort2);
char2 __CVAttrs convert_char2_rtp(int2);
char2 __CVAttrs convert_char2_rtp(uint2);
char2 __CVAttrs convert_char2_rtp(long2);
char2 __CVAttrs convert_char2_rtp(ulong2);
char2 __CVAttrs convert_char2_rtp(float2);
char2 __CVAttrs convert_char2_rtp(double2);
char2 __CVAttrs convert_char2_rtn(char2);
char2 __CVAttrs convert_char2_rtn(uchar2);
char2 __CVAttrs convert_char2_rtn(short2);
char2 __CVAttrs convert_char2_rtn(ushort2);
char2 __CVAttrs convert_char2_rtn(int2);
char2 __CVAttrs convert_char2_rtn(uint2);
char2 __CVAttrs convert_char2_rtn(long2);
char2 __CVAttrs convert_char2_rtn(ulong2);
char2 __CVAttrs convert_char2_rtn(float2);
char2 __CVAttrs convert_char2_rtn(double2);
char2 __CVAttrs convert_char2_sat(char2);
char2 __CVAttrs convert_char2_sat(uchar2);
char2 __CVAttrs convert_char2_sat(short2);
char2 __CVAttrs convert_char2_sat(ushort2);
char2 __CVAttrs convert_char2_sat(int2);
char2 __CVAttrs convert_char2_sat(uint2);
char2 __CVAttrs convert_char2_sat(long2);
char2 __CVAttrs convert_char2_sat(ulong2);
char2 __CVAttrs convert_char2_sat(float2);
char2 __CVAttrs convert_char2_sat(double2);
char2 __CVAttrs convert_char2_sat_rte(char2);
char2 __CVAttrs convert_char2_sat_rte(uchar2);
char2 __CVAttrs convert_char2_sat_rte(short2);
char2 __CVAttrs convert_char2_sat_rte(ushort2);
char2 __CVAttrs convert_char2_sat_rte(int2);
char2 __CVAttrs convert_char2_sat_rte(uint2);
char2 __CVAttrs convert_char2_sat_rte(long2);
char2 __CVAttrs convert_char2_sat_rte(ulong2);
char2 __CVAttrs convert_char2_sat_rte(float2);
char2 __CVAttrs convert_char2_sat_rte(double2);
char2 __CVAttrs convert_char2_sat_rtz(char2);
char2 __CVAttrs convert_char2_sat_rtz(uchar2);
char2 __CVAttrs convert_char2_sat_rtz(short2);
char2 __CVAttrs convert_char2_sat_rtz(ushort2);
char2 __CVAttrs convert_char2_sat_rtz(int2);
char2 __CVAttrs convert_char2_sat_rtz(uint2);
char2 __CVAttrs convert_char2_sat_rtz(long2);
char2 __CVAttrs convert_char2_sat_rtz(ulong2);
char2 __CVAttrs convert_char2_sat_rtz(float2);
char2 __CVAttrs convert_char2_sat_rtz(double2);
char2 __CVAttrs convert_char2_sat_rtp(char2);
char2 __CVAttrs convert_char2_sat_rtp(uchar2);
char2 __CVAttrs convert_char2_sat_rtp(short2);
char2 __CVAttrs convert_char2_sat_rtp(ushort2);
char2 __CVAttrs convert_char2_sat_rtp(int2);
char2 __CVAttrs convert_char2_sat_rtp(uint2);
char2 __CVAttrs convert_char2_sat_rtp(long2);
char2 __CVAttrs convert_char2_sat_rtp(ulong2);
char2 __CVAttrs convert_char2_sat_rtp(float2);
char2 __CVAttrs convert_char2_sat_rtp(double2);
char2 __CVAttrs convert_char2_sat_rtn(char2);
char2 __CVAttrs convert_char2_sat_rtn(uchar2);
char2 __CVAttrs convert_char2_sat_rtn(short2);
char2 __CVAttrs convert_char2_sat_rtn(ushort2);
char2 __CVAttrs convert_char2_sat_rtn(int2);
char2 __CVAttrs convert_char2_sat_rtn(uint2);
char2 __CVAttrs convert_char2_sat_rtn(long2);
char2 __CVAttrs convert_char2_sat_rtn(ulong2);
char2 __CVAttrs convert_char2_sat_rtn(float2);
char2 __CVAttrs convert_char2_sat_rtn(double2);
#if __OPENCL_C_VERSION__ >= 110
char3 __CVAttrs convert_char3(char3);
char3 __CVAttrs convert_char3(uchar3);
char3 __CVAttrs convert_char3(short3);
char3 __CVAttrs convert_char3(ushort3);
char3 __CVAttrs convert_char3(int3);
char3 __CVAttrs convert_char3(uint3);
char3 __CVAttrs convert_char3(long3);
char3 __CVAttrs convert_char3(ulong3);
char3 __CVAttrs convert_char3(float3);
char3 __CVAttrs convert_char3(double3);
char3 __CVAttrs convert_char3_rte(char3);
char3 __CVAttrs convert_char3_rte(uchar3);
char3 __CVAttrs convert_char3_rte(short3);
char3 __CVAttrs convert_char3_rte(ushort3);
char3 __CVAttrs convert_char3_rte(int3);
char3 __CVAttrs convert_char3_rte(uint3);
char3 __CVAttrs convert_char3_rte(long3);
char3 __CVAttrs convert_char3_rte(ulong3);
char3 __CVAttrs convert_char3_rte(float3);
char3 __CVAttrs convert_char3_rte(double3);
char3 __CVAttrs convert_char3_rtz(char3);
char3 __CVAttrs convert_char3_rtz(uchar3);
char3 __CVAttrs convert_char3_rtz(short3);
char3 __CVAttrs convert_char3_rtz(ushort3);
char3 __CVAttrs convert_char3_rtz(int3);
char3 __CVAttrs convert_char3_rtz(uint3);
char3 __CVAttrs convert_char3_rtz(long3);
char3 __CVAttrs convert_char3_rtz(ulong3);
char3 __CVAttrs convert_char3_rtz(float3);
char3 __CVAttrs convert_char3_rtz(double3);
char3 __CVAttrs convert_char3_rtp(char3);
char3 __CVAttrs convert_char3_rtp(uchar3);
char3 __CVAttrs convert_char3_rtp(short3);
char3 __CVAttrs convert_char3_rtp(ushort3);
char3 __CVAttrs convert_char3_rtp(int3);
char3 __CVAttrs convert_char3_rtp(uint3);
char3 __CVAttrs convert_char3_rtp(long3);
char3 __CVAttrs convert_char3_rtp(ulong3);
char3 __CVAttrs convert_char3_rtp(float3);
char3 __CVAttrs convert_char3_rtp(double3);
char3 __CVAttrs convert_char3_rtn(char3);
char3 __CVAttrs convert_char3_rtn(uchar3);
char3 __CVAttrs convert_char3_rtn(short3);
char3 __CVAttrs convert_char3_rtn(ushort3);
char3 __CVAttrs convert_char3_rtn(int3);
char3 __CVAttrs convert_char3_rtn(uint3);
char3 __CVAttrs convert_char3_rtn(long3);
char3 __CVAttrs convert_char3_rtn(ulong3);
char3 __CVAttrs convert_char3_rtn(float3);
char3 __CVAttrs convert_char3_rtn(double3);
char3 __CVAttrs convert_char3_sat(char3);
char3 __CVAttrs convert_char3_sat(uchar3);
char3 __CVAttrs convert_char3_sat(short3);
char3 __CVAttrs convert_char3_sat(ushort3);
char3 __CVAttrs convert_char3_sat(int3);
char3 __CVAttrs convert_char3_sat(uint3);
char3 __CVAttrs convert_char3_sat(long3);
char3 __CVAttrs convert_char3_sat(ulong3);
char3 __CVAttrs convert_char3_sat(float3);
char3 __CVAttrs convert_char3_sat(double3);
char3 __CVAttrs convert_char3_sat_rte(char3);
char3 __CVAttrs convert_char3_sat_rte(uchar3);
char3 __CVAttrs convert_char3_sat_rte(short3);
char3 __CVAttrs convert_char3_sat_rte(ushort3);
char3 __CVAttrs convert_char3_sat_rte(int3);
char3 __CVAttrs convert_char3_sat_rte(uint3);
char3 __CVAttrs convert_char3_sat_rte(long3);
char3 __CVAttrs convert_char3_sat_rte(ulong3);
char3 __CVAttrs convert_char3_sat_rte(float3);
char3 __CVAttrs convert_char3_sat_rte(double3);
char3 __CVAttrs convert_char3_sat_rtz(char3);
char3 __CVAttrs convert_char3_sat_rtz(uchar3);
char3 __CVAttrs convert_char3_sat_rtz(short3);
char3 __CVAttrs convert_char3_sat_rtz(ushort3);
char3 __CVAttrs convert_char3_sat_rtz(int3);
char3 __CVAttrs convert_char3_sat_rtz(uint3);
char3 __CVAttrs convert_char3_sat_rtz(long3);
char3 __CVAttrs convert_char3_sat_rtz(ulong3);
char3 __CVAttrs convert_char3_sat_rtz(float3);
char3 __CVAttrs convert_char3_sat_rtz(double3);
char3 __CVAttrs convert_char3_sat_rtp(char3);
char3 __CVAttrs convert_char3_sat_rtp(uchar3);
char3 __CVAttrs convert_char3_sat_rtp(short3);
char3 __CVAttrs convert_char3_sat_rtp(ushort3);
char3 __CVAttrs convert_char3_sat_rtp(int3);
char3 __CVAttrs convert_char3_sat_rtp(uint3);
char3 __CVAttrs convert_char3_sat_rtp(long3);
char3 __CVAttrs convert_char3_sat_rtp(ulong3);
char3 __CVAttrs convert_char3_sat_rtp(float3);
char3 __CVAttrs convert_char3_sat_rtp(double3);
char3 __CVAttrs convert_char3_sat_rtn(char3);
char3 __CVAttrs convert_char3_sat_rtn(uchar3);
char3 __CVAttrs convert_char3_sat_rtn(short3);
char3 __CVAttrs convert_char3_sat_rtn(ushort3);
char3 __CVAttrs convert_char3_sat_rtn(int3);
char3 __CVAttrs convert_char3_sat_rtn(uint3);
char3 __CVAttrs convert_char3_sat_rtn(long3);
char3 __CVAttrs convert_char3_sat_rtn(ulong3);
char3 __CVAttrs convert_char3_sat_rtn(float3);
char3 __CVAttrs convert_char3_sat_rtn(double3);
#endif
char4 __CVAttrs convert_char4(char4);
char4 __CVAttrs convert_char4(uchar4);
char4 __CVAttrs convert_char4(short4);
char4 __CVAttrs convert_char4(ushort4);
char4 __CVAttrs convert_char4(int4);
char4 __CVAttrs convert_char4(uint4);
char4 __CVAttrs convert_char4(long4);
char4 __CVAttrs convert_char4(ulong4);
char4 __CVAttrs convert_char4(float4);
char4 __CVAttrs convert_char4(double4);
char4 __CVAttrs convert_char4_rte(char4);
char4 __CVAttrs convert_char4_rte(uchar4);
char4 __CVAttrs convert_char4_rte(short4);
char4 __CVAttrs convert_char4_rte(ushort4);
char4 __CVAttrs convert_char4_rte(int4);
char4 __CVAttrs convert_char4_rte(uint4);
char4 __CVAttrs convert_char4_rte(long4);
char4 __CVAttrs convert_char4_rte(ulong4);
char4 __CVAttrs convert_char4_rte(float4);
char4 __CVAttrs convert_char4_rte(double4);
char4 __CVAttrs convert_char4_rtz(char4);
char4 __CVAttrs convert_char4_rtz(uchar4);
char4 __CVAttrs convert_char4_rtz(short4);
char4 __CVAttrs convert_char4_rtz(ushort4);
char4 __CVAttrs convert_char4_rtz(int4);
char4 __CVAttrs convert_char4_rtz(uint4);
char4 __CVAttrs convert_char4_rtz(long4);
char4 __CVAttrs convert_char4_rtz(ulong4);
char4 __CVAttrs convert_char4_rtz(float4);
char4 __CVAttrs convert_char4_rtz(double4);
char4 __CVAttrs convert_char4_rtp(char4);
char4 __CVAttrs convert_char4_rtp(uchar4);
char4 __CVAttrs convert_char4_rtp(short4);
char4 __CVAttrs convert_char4_rtp(ushort4);
char4 __CVAttrs convert_char4_rtp(int4);
char4 __CVAttrs convert_char4_rtp(uint4);
char4 __CVAttrs convert_char4_rtp(long4);
char4 __CVAttrs convert_char4_rtp(ulong4);
char4 __CVAttrs convert_char4_rtp(float4);
char4 __CVAttrs convert_char4_rtp(double4);
char4 __CVAttrs convert_char4_rtn(char4);
char4 __CVAttrs convert_char4_rtn(uchar4);
char4 __CVAttrs convert_char4_rtn(short4);
char4 __CVAttrs convert_char4_rtn(ushort4);
char4 __CVAttrs convert_char4_rtn(int4);
char4 __CVAttrs convert_char4_rtn(uint4);
char4 __CVAttrs convert_char4_rtn(long4);
char4 __CVAttrs convert_char4_rtn(ulong4);
char4 __CVAttrs convert_char4_rtn(float4);
char4 __CVAttrs convert_char4_rtn(double4);
char4 __CVAttrs convert_char4_sat(char4);
char4 __CVAttrs convert_char4_sat(uchar4);
char4 __CVAttrs convert_char4_sat(short4);
char4 __CVAttrs convert_char4_sat(ushort4);
char4 __CVAttrs convert_char4_sat(int4);
char4 __CVAttrs convert_char4_sat(uint4);
char4 __CVAttrs convert_char4_sat(long4);
char4 __CVAttrs convert_char4_sat(ulong4);
char4 __CVAttrs convert_char4_sat(float4);
char4 __CVAttrs convert_char4_sat(double4);
char4 __CVAttrs convert_char4_sat_rte(char4);
char4 __CVAttrs convert_char4_sat_rte(uchar4);
char4 __CVAttrs convert_char4_sat_rte(short4);
char4 __CVAttrs convert_char4_sat_rte(ushort4);
char4 __CVAttrs convert_char4_sat_rte(int4);
char4 __CVAttrs convert_char4_sat_rte(uint4);
char4 __CVAttrs convert_char4_sat_rte(long4);
char4 __CVAttrs convert_char4_sat_rte(ulong4);
char4 __CVAttrs convert_char4_sat_rte(float4);
char4 __CVAttrs convert_char4_sat_rte(double4);
char4 __CVAttrs convert_char4_sat_rtz(char4);
char4 __CVAttrs convert_char4_sat_rtz(uchar4);
char4 __CVAttrs convert_char4_sat_rtz(short4);
char4 __CVAttrs convert_char4_sat_rtz(ushort4);
char4 __CVAttrs convert_char4_sat_rtz(int4);
char4 __CVAttrs convert_char4_sat_rtz(uint4);
char4 __CVAttrs convert_char4_sat_rtz(long4);
char4 __CVAttrs convert_char4_sat_rtz(ulong4);
char4 __CVAttrs convert_char4_sat_rtz(float4);
char4 __CVAttrs convert_char4_sat_rtz(double4);
char4 __CVAttrs convert_char4_sat_rtp(char4);
char4 __CVAttrs convert_char4_sat_rtp(uchar4);
char4 __CVAttrs convert_char4_sat_rtp(short4);
char4 __CVAttrs convert_char4_sat_rtp(ushort4);
char4 __CVAttrs convert_char4_sat_rtp(int4);
char4 __CVAttrs convert_char4_sat_rtp(uint4);
char4 __CVAttrs convert_char4_sat_rtp(long4);
char4 __CVAttrs convert_char4_sat_rtp(ulong4);
char4 __CVAttrs convert_char4_sat_rtp(float4);
char4 __CVAttrs convert_char4_sat_rtp(double4);
char4 __CVAttrs convert_char4_sat_rtn(char4);
char4 __CVAttrs convert_char4_sat_rtn(uchar4);
char4 __CVAttrs convert_char4_sat_rtn(short4);
char4 __CVAttrs convert_char4_sat_rtn(ushort4);
char4 __CVAttrs convert_char4_sat_rtn(int4);
char4 __CVAttrs convert_char4_sat_rtn(uint4);
char4 __CVAttrs convert_char4_sat_rtn(long4);
char4 __CVAttrs convert_char4_sat_rtn(ulong4);
char4 __CVAttrs convert_char4_sat_rtn(float4);
char4 __CVAttrs convert_char4_sat_rtn(double4);
char8 __CVAttrs convert_char8(char8);
char8 __CVAttrs convert_char8(uchar8);
char8 __CVAttrs convert_char8(short8);
char8 __CVAttrs convert_char8(ushort8);
char8 __CVAttrs convert_char8(int8);
char8 __CVAttrs convert_char8(uint8);
char8 __CVAttrs convert_char8(long8);
char8 __CVAttrs convert_char8(ulong8);
char8 __CVAttrs convert_char8(float8);
char8 __CVAttrs convert_char8(double8);
char8 __CVAttrs convert_char8_rte(char8);
char8 __CVAttrs convert_char8_rte(uchar8);
char8 __CVAttrs convert_char8_rte(short8);
char8 __CVAttrs convert_char8_rte(ushort8);
char8 __CVAttrs convert_char8_rte(int8);
char8 __CVAttrs convert_char8_rte(uint8);
char8 __CVAttrs convert_char8_rte(long8);
char8 __CVAttrs convert_char8_rte(ulong8);
char8 __CVAttrs convert_char8_rte(float8);
char8 __CVAttrs convert_char8_rte(double8);
char8 __CVAttrs convert_char8_rtz(char8);
char8 __CVAttrs convert_char8_rtz(uchar8);
char8 __CVAttrs convert_char8_rtz(short8);
char8 __CVAttrs convert_char8_rtz(ushort8);
char8 __CVAttrs convert_char8_rtz(int8);
char8 __CVAttrs convert_char8_rtz(uint8);
char8 __CVAttrs convert_char8_rtz(long8);
char8 __CVAttrs convert_char8_rtz(ulong8);
char8 __CVAttrs convert_char8_rtz(float8);
char8 __CVAttrs convert_char8_rtz(double8);
char8 __CVAttrs convert_char8_rtp(char8);
char8 __CVAttrs convert_char8_rtp(uchar8);
char8 __CVAttrs convert_char8_rtp(short8);
char8 __CVAttrs convert_char8_rtp(ushort8);
char8 __CVAttrs convert_char8_rtp(int8);
char8 __CVAttrs convert_char8_rtp(uint8);
char8 __CVAttrs convert_char8_rtp(long8);
char8 __CVAttrs convert_char8_rtp(ulong8);
char8 __CVAttrs convert_char8_rtp(float8);
char8 __CVAttrs convert_char8_rtp(double8);
char8 __CVAttrs convert_char8_rtn(char8);
char8 __CVAttrs convert_char8_rtn(uchar8);
char8 __CVAttrs convert_char8_rtn(short8);
char8 __CVAttrs convert_char8_rtn(ushort8);
char8 __CVAttrs convert_char8_rtn(int8);
char8 __CVAttrs convert_char8_rtn(uint8);
char8 __CVAttrs convert_char8_rtn(long8);
char8 __CVAttrs convert_char8_rtn(ulong8);
char8 __CVAttrs convert_char8_rtn(float8);
char8 __CVAttrs convert_char8_rtn(double8);
char8 __CVAttrs convert_char8_sat(char8);
char8 __CVAttrs convert_char8_sat(uchar8);
char8 __CVAttrs convert_char8_sat(short8);
char8 __CVAttrs convert_char8_sat(ushort8);
char8 __CVAttrs convert_char8_sat(int8);
char8 __CVAttrs convert_char8_sat(uint8);
char8 __CVAttrs convert_char8_sat(long8);
char8 __CVAttrs convert_char8_sat(ulong8);
char8 __CVAttrs convert_char8_sat(float8);
char8 __CVAttrs convert_char8_sat(double8);
char8 __CVAttrs convert_char8_sat_rte(char8);
char8 __CVAttrs convert_char8_sat_rte(uchar8);
char8 __CVAttrs convert_char8_sat_rte(short8);
char8 __CVAttrs convert_char8_sat_rte(ushort8);
char8 __CVAttrs convert_char8_sat_rte(int8);
char8 __CVAttrs convert_char8_sat_rte(uint8);
char8 __CVAttrs convert_char8_sat_rte(long8);
char8 __CVAttrs convert_char8_sat_rte(ulong8);
char8 __CVAttrs convert_char8_sat_rte(float8);
char8 __CVAttrs convert_char8_sat_rte(double8);
char8 __CVAttrs convert_char8_sat_rtz(char8);
char8 __CVAttrs convert_char8_sat_rtz(uchar8);
char8 __CVAttrs convert_char8_sat_rtz(short8);
char8 __CVAttrs convert_char8_sat_rtz(ushort8);
char8 __CVAttrs convert_char8_sat_rtz(int8);
char8 __CVAttrs convert_char8_sat_rtz(uint8);
char8 __CVAttrs convert_char8_sat_rtz(long8);
char8 __CVAttrs convert_char8_sat_rtz(ulong8);
char8 __CVAttrs convert_char8_sat_rtz(float8);
char8 __CVAttrs convert_char8_sat_rtz(double8);
char8 __CVAttrs convert_char8_sat_rtp(char8);
char8 __CVAttrs convert_char8_sat_rtp(uchar8);
char8 __CVAttrs convert_char8_sat_rtp(short8);
char8 __CVAttrs convert_char8_sat_rtp(ushort8);
char8 __CVAttrs convert_char8_sat_rtp(int8);
char8 __CVAttrs convert_char8_sat_rtp(uint8);
char8 __CVAttrs convert_char8_sat_rtp(long8);
char8 __CVAttrs convert_char8_sat_rtp(ulong8);
char8 __CVAttrs convert_char8_sat_rtp(float8);
char8 __CVAttrs convert_char8_sat_rtp(double8);
char8 __CVAttrs convert_char8_sat_rtn(char8);
char8 __CVAttrs convert_char8_sat_rtn(uchar8);
char8 __CVAttrs convert_char8_sat_rtn(short8);
char8 __CVAttrs convert_char8_sat_rtn(ushort8);
char8 __CVAttrs convert_char8_sat_rtn(int8);
char8 __CVAttrs convert_char8_sat_rtn(uint8);
char8 __CVAttrs convert_char8_sat_rtn(long8);
char8 __CVAttrs convert_char8_sat_rtn(ulong8);
char8 __CVAttrs convert_char8_sat_rtn(float8);
char8 __CVAttrs convert_char8_sat_rtn(double8);
char16 __CVAttrs convert_char16(char16);
char16 __CVAttrs convert_char16(uchar16);
char16 __CVAttrs convert_char16(short16);
char16 __CVAttrs convert_char16(ushort16);
char16 __CVAttrs convert_char16(int16);
char16 __CVAttrs convert_char16(uint16);
char16 __CVAttrs convert_char16(long16);
char16 __CVAttrs convert_char16(ulong16);
char16 __CVAttrs convert_char16(float16);
char16 __CVAttrs convert_char16(double16);
char16 __CVAttrs convert_char16_rte(char16);
char16 __CVAttrs convert_char16_rte(uchar16);
char16 __CVAttrs convert_char16_rte(short16);
char16 __CVAttrs convert_char16_rte(ushort16);
char16 __CVAttrs convert_char16_rte(int16);
char16 __CVAttrs convert_char16_rte(uint16);
char16 __CVAttrs convert_char16_rte(long16);
char16 __CVAttrs convert_char16_rte(ulong16);
char16 __CVAttrs convert_char16_rte(float16);
char16 __CVAttrs convert_char16_rte(double16);
char16 __CVAttrs convert_char16_rtz(char16);
char16 __CVAttrs convert_char16_rtz(uchar16);
char16 __CVAttrs convert_char16_rtz(short16);
char16 __CVAttrs convert_char16_rtz(ushort16);
char16 __CVAttrs convert_char16_rtz(int16);
char16 __CVAttrs convert_char16_rtz(uint16);
char16 __CVAttrs convert_char16_rtz(long16);
char16 __CVAttrs convert_char16_rtz(ulong16);
char16 __CVAttrs convert_char16_rtz(float16);
char16 __CVAttrs convert_char16_rtz(double16);
char16 __CVAttrs convert_char16_rtp(char16);
char16 __CVAttrs convert_char16_rtp(uchar16);
char16 __CVAttrs convert_char16_rtp(short16);
char16 __CVAttrs convert_char16_rtp(ushort16);
char16 __CVAttrs convert_char16_rtp(int16);
char16 __CVAttrs convert_char16_rtp(uint16);
char16 __CVAttrs convert_char16_rtp(long16);
char16 __CVAttrs convert_char16_rtp(ulong16);
char16 __CVAttrs convert_char16_rtp(float16);
char16 __CVAttrs convert_char16_rtp(double16);
char16 __CVAttrs convert_char16_rtn(char16);
char16 __CVAttrs convert_char16_rtn(uchar16);
char16 __CVAttrs convert_char16_rtn(short16);
char16 __CVAttrs convert_char16_rtn(ushort16);
char16 __CVAttrs convert_char16_rtn(int16);
char16 __CVAttrs convert_char16_rtn(uint16);
char16 __CVAttrs convert_char16_rtn(long16);
char16 __CVAttrs convert_char16_rtn(ulong16);
char16 __CVAttrs convert_char16_rtn(float16);
char16 __CVAttrs convert_char16_rtn(double16);
char16 __CVAttrs convert_char16_sat(char16);
char16 __CVAttrs convert_char16_sat(uchar16);
char16 __CVAttrs convert_char16_sat(short16);
char16 __CVAttrs convert_char16_sat(ushort16);
char16 __CVAttrs convert_char16_sat(int16);
char16 __CVAttrs convert_char16_sat(uint16);
char16 __CVAttrs convert_char16_sat(long16);
char16 __CVAttrs convert_char16_sat(ulong16);
char16 __CVAttrs convert_char16_sat(float16);
char16 __CVAttrs convert_char16_sat(double16);
char16 __CVAttrs convert_char16_sat_rte(char16);
char16 __CVAttrs convert_char16_sat_rte(uchar16);
char16 __CVAttrs convert_char16_sat_rte(short16);
char16 __CVAttrs convert_char16_sat_rte(ushort16);
char16 __CVAttrs convert_char16_sat_rte(int16);
char16 __CVAttrs convert_char16_sat_rte(uint16);
char16 __CVAttrs convert_char16_sat_rte(long16);
char16 __CVAttrs convert_char16_sat_rte(ulong16);
char16 __CVAttrs convert_char16_sat_rte(float16);
char16 __CVAttrs convert_char16_sat_rte(double16);
char16 __CVAttrs convert_char16_sat_rtz(char16);
char16 __CVAttrs convert_char16_sat_rtz(uchar16);
char16 __CVAttrs convert_char16_sat_rtz(short16);
char16 __CVAttrs convert_char16_sat_rtz(ushort16);
char16 __CVAttrs convert_char16_sat_rtz(int16);
char16 __CVAttrs convert_char16_sat_rtz(uint16);
char16 __CVAttrs convert_char16_sat_rtz(long16);
char16 __CVAttrs convert_char16_sat_rtz(ulong16);
char16 __CVAttrs convert_char16_sat_rtz(float16);
char16 __CVAttrs convert_char16_sat_rtz(double16);
char16 __CVAttrs convert_char16_sat_rtp(char16);
char16 __CVAttrs convert_char16_sat_rtp(uchar16);
char16 __CVAttrs convert_char16_sat_rtp(short16);
char16 __CVAttrs convert_char16_sat_rtp(ushort16);
char16 __CVAttrs convert_char16_sat_rtp(int16);
char16 __CVAttrs convert_char16_sat_rtp(uint16);
char16 __CVAttrs convert_char16_sat_rtp(long16);
char16 __CVAttrs convert_char16_sat_rtp(ulong16);
char16 __CVAttrs convert_char16_sat_rtp(float16);
char16 __CVAttrs convert_char16_sat_rtp(double16);
char16 __CVAttrs convert_char16_sat_rtn(char16);
char16 __CVAttrs convert_char16_sat_rtn(uchar16);
char16 __CVAttrs convert_char16_sat_rtn(short16);
char16 __CVAttrs convert_char16_sat_rtn(ushort16);
char16 __CVAttrs convert_char16_sat_rtn(int16);
char16 __CVAttrs convert_char16_sat_rtn(uint16);
char16 __CVAttrs convert_char16_sat_rtn(long16);
char16 __CVAttrs convert_char16_sat_rtn(ulong16);
char16 __CVAttrs convert_char16_sat_rtn(float16);
char16 __CVAttrs convert_char16_sat_rtn(double16);
uchar __CVAttrs convert_uchar(char);
uchar __CVAttrs convert_uchar(uchar);
uchar __CVAttrs convert_uchar(short);
uchar __CVAttrs convert_uchar(ushort);
uchar __CVAttrs convert_uchar(int);
uchar __CVAttrs convert_uchar(uint);
uchar __CVAttrs convert_uchar(long);
uchar __CVAttrs convert_uchar(ulong);
uchar __CVAttrs convert_uchar(float);
uchar __CVAttrs convert_uchar(double);
uchar __CVAttrs convert_uchar_rte(char);
uchar __CVAttrs convert_uchar_rte(uchar);
uchar __CVAttrs convert_uchar_rte(short);
uchar __CVAttrs convert_uchar_rte(ushort);
uchar __CVAttrs convert_uchar_rte(int);
uchar __CVAttrs convert_uchar_rte(uint);
uchar __CVAttrs convert_uchar_rte(long);
uchar __CVAttrs convert_uchar_rte(ulong);
uchar __CVAttrs convert_uchar_rte(float);
uchar __CVAttrs convert_uchar_rte(double);
uchar __CVAttrs convert_uchar_rtz(char);
uchar __CVAttrs convert_uchar_rtz(uchar);
uchar __CVAttrs convert_uchar_rtz(short);
uchar __CVAttrs convert_uchar_rtz(ushort);
uchar __CVAttrs convert_uchar_rtz(int);
uchar __CVAttrs convert_uchar_rtz(uint);
uchar __CVAttrs convert_uchar_rtz(long);
uchar __CVAttrs convert_uchar_rtz(ulong);
uchar __CVAttrs convert_uchar_rtz(float);
uchar __CVAttrs convert_uchar_rtz(double);
uchar __CVAttrs convert_uchar_rtp(char);
uchar __CVAttrs convert_uchar_rtp(uchar);
uchar __CVAttrs convert_uchar_rtp(short);
uchar __CVAttrs convert_uchar_rtp(ushort);
uchar __CVAttrs convert_uchar_rtp(int);
uchar __CVAttrs convert_uchar_rtp(uint);
uchar __CVAttrs convert_uchar_rtp(long);
uchar __CVAttrs convert_uchar_rtp(ulong);
uchar __CVAttrs convert_uchar_rtp(float);
uchar __CVAttrs convert_uchar_rtp(double);
uchar __CVAttrs convert_uchar_rtn(char);
uchar __CVAttrs convert_uchar_rtn(uchar);
uchar __CVAttrs convert_uchar_rtn(short);
uchar __CVAttrs convert_uchar_rtn(ushort);
uchar __CVAttrs convert_uchar_rtn(int);
uchar __CVAttrs convert_uchar_rtn(uint);
uchar __CVAttrs convert_uchar_rtn(long);
uchar __CVAttrs convert_uchar_rtn(ulong);
uchar __CVAttrs convert_uchar_rtn(float);
uchar __CVAttrs convert_uchar_rtn(double);
uchar __CVAttrs convert_uchar_sat(char);
uchar __CVAttrs convert_uchar_sat(uchar);
uchar __CVAttrs convert_uchar_sat(short);
uchar __CVAttrs convert_uchar_sat(ushort);
uchar __CVAttrs convert_uchar_sat(int);
uchar __CVAttrs convert_uchar_sat(uint);
uchar __CVAttrs convert_uchar_sat(long);
uchar __CVAttrs convert_uchar_sat(ulong);
uchar __CVAttrs convert_uchar_sat(float);
uchar __CVAttrs convert_uchar_sat(double);
uchar __CVAttrs convert_uchar_sat_rte(char);
uchar __CVAttrs convert_uchar_sat_rte(uchar);
uchar __CVAttrs convert_uchar_sat_rte(short);
uchar __CVAttrs convert_uchar_sat_rte(ushort);
uchar __CVAttrs convert_uchar_sat_rte(int);
uchar __CVAttrs convert_uchar_sat_rte(uint);
uchar __CVAttrs convert_uchar_sat_rte(long);
uchar __CVAttrs convert_uchar_sat_rte(ulong);
uchar __CVAttrs convert_uchar_sat_rte(float);
uchar __CVAttrs convert_uchar_sat_rte(double);
uchar __CVAttrs convert_uchar_sat_rtz(char);
uchar __CVAttrs convert_uchar_sat_rtz(uchar);
uchar __CVAttrs convert_uchar_sat_rtz(short);
uchar __CVAttrs convert_uchar_sat_rtz(ushort);
uchar __CVAttrs convert_uchar_sat_rtz(int);
uchar __CVAttrs convert_uchar_sat_rtz(uint);
uchar __CVAttrs convert_uchar_sat_rtz(long);
uchar __CVAttrs convert_uchar_sat_rtz(ulong);
uchar __CVAttrs convert_uchar_sat_rtz(float);
uchar __CVAttrs convert_uchar_sat_rtz(double);
uchar __CVAttrs convert_uchar_sat_rtp(char);
uchar __CVAttrs convert_uchar_sat_rtp(uchar);
uchar __CVAttrs convert_uchar_sat_rtp(short);
uchar __CVAttrs convert_uchar_sat_rtp(ushort);
uchar __CVAttrs convert_uchar_sat_rtp(int);
uchar __CVAttrs convert_uchar_sat_rtp(uint);
uchar __CVAttrs convert_uchar_sat_rtp(long);
uchar __CVAttrs convert_uchar_sat_rtp(ulong);
uchar __CVAttrs convert_uchar_sat_rtp(float);
uchar __CVAttrs convert_uchar_sat_rtp(double);
uchar __CVAttrs convert_uchar_sat_rtn(char);
uchar __CVAttrs convert_uchar_sat_rtn(uchar);
uchar __CVAttrs convert_uchar_sat_rtn(short);
uchar __CVAttrs convert_uchar_sat_rtn(ushort);
uchar __CVAttrs convert_uchar_sat_rtn(int);
uchar __CVAttrs convert_uchar_sat_rtn(uint);
uchar __CVAttrs convert_uchar_sat_rtn(long);
uchar __CVAttrs convert_uchar_sat_rtn(ulong);
uchar __CVAttrs convert_uchar_sat_rtn(float);
uchar __CVAttrs convert_uchar_sat_rtn(double);
uchar2 __CVAttrs convert_uchar2(char2);
uchar2 __CVAttrs convert_uchar2(uchar2);
uchar2 __CVAttrs convert_uchar2(short2);
uchar2 __CVAttrs convert_uchar2(ushort2);
uchar2 __CVAttrs convert_uchar2(int2);
uchar2 __CVAttrs convert_uchar2(uint2);
uchar2 __CVAttrs convert_uchar2(long2);
uchar2 __CVAttrs convert_uchar2(ulong2);
uchar2 __CVAttrs convert_uchar2(float2);
uchar2 __CVAttrs convert_uchar2(double2);
uchar2 __CVAttrs convert_uchar2_rte(char2);
uchar2 __CVAttrs convert_uchar2_rte(uchar2);
uchar2 __CVAttrs convert_uchar2_rte(short2);
uchar2 __CVAttrs convert_uchar2_rte(ushort2);
uchar2 __CVAttrs convert_uchar2_rte(int2);
uchar2 __CVAttrs convert_uchar2_rte(uint2);
uchar2 __CVAttrs convert_uchar2_rte(long2);
uchar2 __CVAttrs convert_uchar2_rte(ulong2);
uchar2 __CVAttrs convert_uchar2_rte(float2);
uchar2 __CVAttrs convert_uchar2_rte(double2);
uchar2 __CVAttrs convert_uchar2_rtz(char2);
uchar2 __CVAttrs convert_uchar2_rtz(uchar2);
uchar2 __CVAttrs convert_uchar2_rtz(short2);
uchar2 __CVAttrs convert_uchar2_rtz(ushort2);
uchar2 __CVAttrs convert_uchar2_rtz(int2);
uchar2 __CVAttrs convert_uchar2_rtz(uint2);
uchar2 __CVAttrs convert_uchar2_rtz(long2);
uchar2 __CVAttrs convert_uchar2_rtz(ulong2);
uchar2 __CVAttrs convert_uchar2_rtz(float2);
uchar2 __CVAttrs convert_uchar2_rtz(double2);
uchar2 __CVAttrs convert_uchar2_rtp(char2);
uchar2 __CVAttrs convert_uchar2_rtp(uchar2);
uchar2 __CVAttrs convert_uchar2_rtp(short2);
uchar2 __CVAttrs convert_uchar2_rtp(ushort2);
uchar2 __CVAttrs convert_uchar2_rtp(int2);
uchar2 __CVAttrs convert_uchar2_rtp(uint2);
uchar2 __CVAttrs convert_uchar2_rtp(long2);
uchar2 __CVAttrs convert_uchar2_rtp(ulong2);
uchar2 __CVAttrs convert_uchar2_rtp(float2);
uchar2 __CVAttrs convert_uchar2_rtp(double2);
uchar2 __CVAttrs convert_uchar2_rtn(char2);
uchar2 __CVAttrs convert_uchar2_rtn(uchar2);
uchar2 __CVAttrs convert_uchar2_rtn(short2);
uchar2 __CVAttrs convert_uchar2_rtn(ushort2);
uchar2 __CVAttrs convert_uchar2_rtn(int2);
uchar2 __CVAttrs convert_uchar2_rtn(uint2);
uchar2 __CVAttrs convert_uchar2_rtn(long2);
uchar2 __CVAttrs convert_uchar2_rtn(ulong2);
uchar2 __CVAttrs convert_uchar2_rtn(float2);
uchar2 __CVAttrs convert_uchar2_rtn(double2);
uchar2 __CVAttrs convert_uchar2_sat(char2);
uchar2 __CVAttrs convert_uchar2_sat(uchar2);
uchar2 __CVAttrs convert_uchar2_sat(short2);
uchar2 __CVAttrs convert_uchar2_sat(ushort2);
uchar2 __CVAttrs convert_uchar2_sat(int2);
uchar2 __CVAttrs convert_uchar2_sat(uint2);
uchar2 __CVAttrs convert_uchar2_sat(long2);
uchar2 __CVAttrs convert_uchar2_sat(ulong2);
uchar2 __CVAttrs convert_uchar2_sat(float2);
uchar2 __CVAttrs convert_uchar2_sat(double2);
uchar2 __CVAttrs convert_uchar2_sat_rte(char2);
uchar2 __CVAttrs convert_uchar2_sat_rte(uchar2);
uchar2 __CVAttrs convert_uchar2_sat_rte(short2);
uchar2 __CVAttrs convert_uchar2_sat_rte(ushort2);
uchar2 __CVAttrs convert_uchar2_sat_rte(int2);
uchar2 __CVAttrs convert_uchar2_sat_rte(uint2);
uchar2 __CVAttrs convert_uchar2_sat_rte(long2);
uchar2 __CVAttrs convert_uchar2_sat_rte(ulong2);
uchar2 __CVAttrs convert_uchar2_sat_rte(float2);
uchar2 __CVAttrs convert_uchar2_sat_rte(double2);
uchar2 __CVAttrs convert_uchar2_sat_rtz(char2);
uchar2 __CVAttrs convert_uchar2_sat_rtz(uchar2);
uchar2 __CVAttrs convert_uchar2_sat_rtz(short2);
uchar2 __CVAttrs convert_uchar2_sat_rtz(ushort2);
uchar2 __CVAttrs convert_uchar2_sat_rtz(int2);
uchar2 __CVAttrs convert_uchar2_sat_rtz(uint2);
uchar2 __CVAttrs convert_uchar2_sat_rtz(long2);
uchar2 __CVAttrs convert_uchar2_sat_rtz(ulong2);
uchar2 __CVAttrs convert_uchar2_sat_rtz(float2);
uchar2 __CVAttrs convert_uchar2_sat_rtz(double2);
uchar2 __CVAttrs convert_uchar2_sat_rtp(char2);
uchar2 __CVAttrs convert_uchar2_sat_rtp(uchar2);
uchar2 __CVAttrs convert_uchar2_sat_rtp(short2);
uchar2 __CVAttrs convert_uchar2_sat_rtp(ushort2);
uchar2 __CVAttrs convert_uchar2_sat_rtp(int2);
uchar2 __CVAttrs convert_uchar2_sat_rtp(uint2);
uchar2 __CVAttrs convert_uchar2_sat_rtp(long2);
uchar2 __CVAttrs convert_uchar2_sat_rtp(ulong2);
uchar2 __CVAttrs convert_uchar2_sat_rtp(float2);
uchar2 __CVAttrs convert_uchar2_sat_rtp(double2);
uchar2 __CVAttrs convert_uchar2_sat_rtn(char2);
uchar2 __CVAttrs convert_uchar2_sat_rtn(uchar2);
uchar2 __CVAttrs convert_uchar2_sat_rtn(short2);
uchar2 __CVAttrs convert_uchar2_sat_rtn(ushort2);
uchar2 __CVAttrs convert_uchar2_sat_rtn(int2);
uchar2 __CVAttrs convert_uchar2_sat_rtn(uint2);
uchar2 __CVAttrs convert_uchar2_sat_rtn(long2);
uchar2 __CVAttrs convert_uchar2_sat_rtn(ulong2);
uchar2 __CVAttrs convert_uchar2_sat_rtn(float2);
uchar2 __CVAttrs convert_uchar2_sat_rtn(double2);
#if __OPENCL_C_VERSION__ >= 110
uchar3 __CVAttrs convert_uchar3(char3);
uchar3 __CVAttrs convert_uchar3(uchar3);
uchar3 __CVAttrs convert_uchar3(short3);
uchar3 __CVAttrs convert_uchar3(ushort3);
uchar3 __CVAttrs convert_uchar3(int3);
uchar3 __CVAttrs convert_uchar3(uint3);
uchar3 __CVAttrs convert_uchar3(long3);
uchar3 __CVAttrs convert_uchar3(ulong3);
uchar3 __CVAttrs convert_uchar3(float3);
uchar3 __CVAttrs convert_uchar3(double3);
uchar3 __CVAttrs convert_uchar3_rte(char3);
uchar3 __CVAttrs convert_uchar3_rte(uchar3);
uchar3 __CVAttrs convert_uchar3_rte(short3);
uchar3 __CVAttrs convert_uchar3_rte(ushort3);
uchar3 __CVAttrs convert_uchar3_rte(int3);
uchar3 __CVAttrs convert_uchar3_rte(uint3);
uchar3 __CVAttrs convert_uchar3_rte(long3);
uchar3 __CVAttrs convert_uchar3_rte(ulong3);
uchar3 __CVAttrs convert_uchar3_rte(float3);
uchar3 __CVAttrs convert_uchar3_rte(double3);
uchar3 __CVAttrs convert_uchar3_rtz(char3);
uchar3 __CVAttrs convert_uchar3_rtz(uchar3);
uchar3 __CVAttrs convert_uchar3_rtz(short3);
uchar3 __CVAttrs convert_uchar3_rtz(ushort3);
uchar3 __CVAttrs convert_uchar3_rtz(int3);
uchar3 __CVAttrs convert_uchar3_rtz(uint3);
uchar3 __CVAttrs convert_uchar3_rtz(long3);
uchar3 __CVAttrs convert_uchar3_rtz(ulong3);
uchar3 __CVAttrs convert_uchar3_rtz(float3);
uchar3 __CVAttrs convert_uchar3_rtz(double3);
uchar3 __CVAttrs convert_uchar3_rtp(char3);
uchar3 __CVAttrs convert_uchar3_rtp(uchar3);
uchar3 __CVAttrs convert_uchar3_rtp(short3);
uchar3 __CVAttrs convert_uchar3_rtp(ushort3);
uchar3 __CVAttrs convert_uchar3_rtp(int3);
uchar3 __CVAttrs convert_uchar3_rtp(uint3);
uchar3 __CVAttrs convert_uchar3_rtp(long3);
uchar3 __CVAttrs convert_uchar3_rtp(ulong3);
uchar3 __CVAttrs convert_uchar3_rtp(float3);
uchar3 __CVAttrs convert_uchar3_rtp(double3);
uchar3 __CVAttrs convert_uchar3_rtn(char3);
uchar3 __CVAttrs convert_uchar3_rtn(uchar3);
uchar3 __CVAttrs convert_uchar3_rtn(short3);
uchar3 __CVAttrs convert_uchar3_rtn(ushort3);
uchar3 __CVAttrs convert_uchar3_rtn(int3);
uchar3 __CVAttrs convert_uchar3_rtn(uint3);
uchar3 __CVAttrs convert_uchar3_rtn(long3);
uchar3 __CVAttrs convert_uchar3_rtn(ulong3);
uchar3 __CVAttrs convert_uchar3_rtn(float3);
uchar3 __CVAttrs convert_uchar3_rtn(double3);
uchar3 __CVAttrs convert_uchar3_sat(char3);
uchar3 __CVAttrs convert_uchar3_sat(uchar3);
uchar3 __CVAttrs convert_uchar3_sat(short3);
uchar3 __CVAttrs convert_uchar3_sat(ushort3);
uchar3 __CVAttrs convert_uchar3_sat(int3);
uchar3 __CVAttrs convert_uchar3_sat(uint3);
uchar3 __CVAttrs convert_uchar3_sat(long3);
uchar3 __CVAttrs convert_uchar3_sat(ulong3);
uchar3 __CVAttrs convert_uchar3_sat(float3);
uchar3 __CVAttrs convert_uchar3_sat(double3);
uchar3 __CVAttrs convert_uchar3_sat_rte(char3);
uchar3 __CVAttrs convert_uchar3_sat_rte(uchar3);
uchar3 __CVAttrs convert_uchar3_sat_rte(short3);
uchar3 __CVAttrs convert_uchar3_sat_rte(ushort3);
uchar3 __CVAttrs convert_uchar3_sat_rte(int3);
uchar3 __CVAttrs convert_uchar3_sat_rte(uint3);
uchar3 __CVAttrs convert_uchar3_sat_rte(long3);
uchar3 __CVAttrs convert_uchar3_sat_rte(ulong3);
uchar3 __CVAttrs convert_uchar3_sat_rte(float3);
uchar3 __CVAttrs convert_uchar3_sat_rte(double3);
uchar3 __CVAttrs convert_uchar3_sat_rtz(char3);
uchar3 __CVAttrs convert_uchar3_sat_rtz(uchar3);
uchar3 __CVAttrs convert_uchar3_sat_rtz(short3);
uchar3 __CVAttrs convert_uchar3_sat_rtz(ushort3);
uchar3 __CVAttrs convert_uchar3_sat_rtz(int3);
uchar3 __CVAttrs convert_uchar3_sat_rtz(uint3);
uchar3 __CVAttrs convert_uchar3_sat_rtz(long3);
uchar3 __CVAttrs convert_uchar3_sat_rtz(ulong3);
uchar3 __CVAttrs convert_uchar3_sat_rtz(float3);
uchar3 __CVAttrs convert_uchar3_sat_rtz(double3);
uchar3 __CVAttrs convert_uchar3_sat_rtp(char3);
uchar3 __CVAttrs convert_uchar3_sat_rtp(uchar3);
uchar3 __CVAttrs convert_uchar3_sat_rtp(short3);
uchar3 __CVAttrs convert_uchar3_sat_rtp(ushort3);
uchar3 __CVAttrs convert_uchar3_sat_rtp(int3);
uchar3 __CVAttrs convert_uchar3_sat_rtp(uint3);
uchar3 __CVAttrs convert_uchar3_sat_rtp(long3);
uchar3 __CVAttrs convert_uchar3_sat_rtp(ulong3);
uchar3 __CVAttrs convert_uchar3_sat_rtp(float3);
uchar3 __CVAttrs convert_uchar3_sat_rtp(double3);
uchar3 __CVAttrs convert_uchar3_sat_rtn(char3);
uchar3 __CVAttrs convert_uchar3_sat_rtn(uchar3);
uchar3 __CVAttrs convert_uchar3_sat_rtn(short3);
uchar3 __CVAttrs convert_uchar3_sat_rtn(ushort3);
uchar3 __CVAttrs convert_uchar3_sat_rtn(int3);
uchar3 __CVAttrs convert_uchar3_sat_rtn(uint3);
uchar3 __CVAttrs convert_uchar3_sat_rtn(long3);
uchar3 __CVAttrs convert_uchar3_sat_rtn(ulong3);
uchar3 __CVAttrs convert_uchar3_sat_rtn(float3);
uchar3 __CVAttrs convert_uchar3_sat_rtn(double3);
#endif
uchar4 __CVAttrs convert_uchar4(char4);
uchar4 __CVAttrs convert_uchar4(uchar4);
uchar4 __CVAttrs convert_uchar4(short4);
uchar4 __CVAttrs convert_uchar4(ushort4);
uchar4 __CVAttrs convert_uchar4(int4);
uchar4 __CVAttrs convert_uchar4(uint4);
uchar4 __CVAttrs convert_uchar4(long4);
uchar4 __CVAttrs convert_uchar4(ulong4);
uchar4 __CVAttrs convert_uchar4(float4);
uchar4 __CVAttrs convert_uchar4(double4);
uchar4 __CVAttrs convert_uchar4_rte(char4);
uchar4 __CVAttrs convert_uchar4_rte(uchar4);
uchar4 __CVAttrs convert_uchar4_rte(short4);
uchar4 __CVAttrs convert_uchar4_rte(ushort4);
uchar4 __CVAttrs convert_uchar4_rte(int4);
uchar4 __CVAttrs convert_uchar4_rte(uint4);
uchar4 __CVAttrs convert_uchar4_rte(long4);
uchar4 __CVAttrs convert_uchar4_rte(ulong4);
uchar4 __CVAttrs convert_uchar4_rte(float4);
uchar4 __CVAttrs convert_uchar4_rte(double4);
uchar4 __CVAttrs convert_uchar4_rtz(char4);
uchar4 __CVAttrs convert_uchar4_rtz(uchar4);
uchar4 __CVAttrs convert_uchar4_rtz(short4);
uchar4 __CVAttrs convert_uchar4_rtz(ushort4);
uchar4 __CVAttrs convert_uchar4_rtz(int4);
uchar4 __CVAttrs convert_uchar4_rtz(uint4);
uchar4 __CVAttrs convert_uchar4_rtz(long4);
uchar4 __CVAttrs convert_uchar4_rtz(ulong4);
uchar4 __CVAttrs convert_uchar4_rtz(float4);
uchar4 __CVAttrs convert_uchar4_rtz(double4);
uchar4 __CVAttrs convert_uchar4_rtp(char4);
uchar4 __CVAttrs convert_uchar4_rtp(uchar4);
uchar4 __CVAttrs convert_uchar4_rtp(short4);
uchar4 __CVAttrs convert_uchar4_rtp(ushort4);
uchar4 __CVAttrs convert_uchar4_rtp(int4);
uchar4 __CVAttrs convert_uchar4_rtp(uint4);
uchar4 __CVAttrs convert_uchar4_rtp(long4);
uchar4 __CVAttrs convert_uchar4_rtp(ulong4);
uchar4 __CVAttrs convert_uchar4_rtp(float4);
uchar4 __CVAttrs convert_uchar4_rtp(double4);
uchar4 __CVAttrs convert_uchar4_rtn(char4);
uchar4 __CVAttrs convert_uchar4_rtn(uchar4);
uchar4 __CVAttrs convert_uchar4_rtn(short4);
uchar4 __CVAttrs convert_uchar4_rtn(ushort4);
uchar4 __CVAttrs convert_uchar4_rtn(int4);
uchar4 __CVAttrs convert_uchar4_rtn(uint4);
uchar4 __CVAttrs convert_uchar4_rtn(long4);
uchar4 __CVAttrs convert_uchar4_rtn(ulong4);
uchar4 __CVAttrs convert_uchar4_rtn(float4);
uchar4 __CVAttrs convert_uchar4_rtn(double4);
uchar4 __CVAttrs convert_uchar4_sat(char4);
uchar4 __CVAttrs convert_uchar4_sat(uchar4);
uchar4 __CVAttrs convert_uchar4_sat(short4);
uchar4 __CVAttrs convert_uchar4_sat(ushort4);
uchar4 __CVAttrs convert_uchar4_sat(int4);
uchar4 __CVAttrs convert_uchar4_sat(uint4);
uchar4 __CVAttrs convert_uchar4_sat(long4);
uchar4 __CVAttrs convert_uchar4_sat(ulong4);
uchar4 __CVAttrs convert_uchar4_sat(float4);
uchar4 __CVAttrs convert_uchar4_sat(double4);
uchar4 __CVAttrs convert_uchar4_sat_rte(char4);
uchar4 __CVAttrs convert_uchar4_sat_rte(uchar4);
uchar4 __CVAttrs convert_uchar4_sat_rte(short4);
uchar4 __CVAttrs convert_uchar4_sat_rte(ushort4);
uchar4 __CVAttrs convert_uchar4_sat_rte(int4);
uchar4 __CVAttrs convert_uchar4_sat_rte(uint4);
uchar4 __CVAttrs convert_uchar4_sat_rte(long4);
uchar4 __CVAttrs convert_uchar4_sat_rte(ulong4);
uchar4 __CVAttrs convert_uchar4_sat_rte(float4);
uchar4 __CVAttrs convert_uchar4_sat_rte(double4);
uchar4 __CVAttrs convert_uchar4_sat_rtz(char4);
uchar4 __CVAttrs convert_uchar4_sat_rtz(uchar4);
uchar4 __CVAttrs convert_uchar4_sat_rtz(short4);
uchar4 __CVAttrs convert_uchar4_sat_rtz(ushort4);
uchar4 __CVAttrs convert_uchar4_sat_rtz(int4);
uchar4 __CVAttrs convert_uchar4_sat_rtz(uint4);
uchar4 __CVAttrs convert_uchar4_sat_rtz(long4);
uchar4 __CVAttrs convert_uchar4_sat_rtz(ulong4);
uchar4 __CVAttrs convert_uchar4_sat_rtz(float4);
uchar4 __CVAttrs convert_uchar4_sat_rtz(double4);
uchar4 __CVAttrs convert_uchar4_sat_rtp(char4);
uchar4 __CVAttrs convert_uchar4_sat_rtp(uchar4);
uchar4 __CVAttrs convert_uchar4_sat_rtp(short4);
uchar4 __CVAttrs convert_uchar4_sat_rtp(ushort4);
uchar4 __CVAttrs convert_uchar4_sat_rtp(int4);
uchar4 __CVAttrs convert_uchar4_sat_rtp(uint4);
uchar4 __CVAttrs convert_uchar4_sat_rtp(long4);
uchar4 __CVAttrs convert_uchar4_sat_rtp(ulong4);
uchar4 __CVAttrs convert_uchar4_sat_rtp(float4);
uchar4 __CVAttrs convert_uchar4_sat_rtp(double4);
uchar4 __CVAttrs convert_uchar4_sat_rtn(char4);
uchar4 __CVAttrs convert_uchar4_sat_rtn(uchar4);
uchar4 __CVAttrs convert_uchar4_sat_rtn(short4);
uchar4 __CVAttrs convert_uchar4_sat_rtn(ushort4);
uchar4 __CVAttrs convert_uchar4_sat_rtn(int4);
uchar4 __CVAttrs convert_uchar4_sat_rtn(uint4);
uchar4 __CVAttrs convert_uchar4_sat_rtn(long4);
uchar4 __CVAttrs convert_uchar4_sat_rtn(ulong4);
uchar4 __CVAttrs convert_uchar4_sat_rtn(float4);
uchar4 __CVAttrs convert_uchar4_sat_rtn(double4);
uchar8 __CVAttrs convert_uchar8(char8);
uchar8 __CVAttrs convert_uchar8(uchar8);
uchar8 __CVAttrs convert_uchar8(short8);
uchar8 __CVAttrs convert_uchar8(ushort8);
uchar8 __CVAttrs convert_uchar8(int8);
uchar8 __CVAttrs convert_uchar8(uint8);
uchar8 __CVAttrs convert_uchar8(long8);
uchar8 __CVAttrs convert_uchar8(ulong8);
uchar8 __CVAttrs convert_uchar8(float8);
uchar8 __CVAttrs convert_uchar8(double8);
uchar8 __CVAttrs convert_uchar8_rte(char8);
uchar8 __CVAttrs convert_uchar8_rte(uchar8);
uchar8 __CVAttrs convert_uchar8_rte(short8);
uchar8 __CVAttrs convert_uchar8_rte(ushort8);
uchar8 __CVAttrs convert_uchar8_rte(int8);
uchar8 __CVAttrs convert_uchar8_rte(uint8);
uchar8 __CVAttrs convert_uchar8_rte(long8);
uchar8 __CVAttrs convert_uchar8_rte(ulong8);
uchar8 __CVAttrs convert_uchar8_rte(float8);
uchar8 __CVAttrs convert_uchar8_rte(double8);
uchar8 __CVAttrs convert_uchar8_rtz(char8);
uchar8 __CVAttrs convert_uchar8_rtz(uchar8);
uchar8 __CVAttrs convert_uchar8_rtz(short8);
uchar8 __CVAttrs convert_uchar8_rtz(ushort8);
uchar8 __CVAttrs convert_uchar8_rtz(int8);
uchar8 __CVAttrs convert_uchar8_rtz(uint8);
uchar8 __CVAttrs convert_uchar8_rtz(long8);
uchar8 __CVAttrs convert_uchar8_rtz(ulong8);
uchar8 __CVAttrs convert_uchar8_rtz(float8);
uchar8 __CVAttrs convert_uchar8_rtz(double8);
uchar8 __CVAttrs convert_uchar8_rtp(char8);
uchar8 __CVAttrs convert_uchar8_rtp(uchar8);
uchar8 __CVAttrs convert_uchar8_rtp(short8);
uchar8 __CVAttrs convert_uchar8_rtp(ushort8);
uchar8 __CVAttrs convert_uchar8_rtp(int8);
uchar8 __CVAttrs convert_uchar8_rtp(uint8);
uchar8 __CVAttrs convert_uchar8_rtp(long8);
uchar8 __CVAttrs convert_uchar8_rtp(ulong8);
uchar8 __CVAttrs convert_uchar8_rtp(float8);
uchar8 __CVAttrs convert_uchar8_rtp(double8);
uchar8 __CVAttrs convert_uchar8_rtn(char8);
uchar8 __CVAttrs convert_uchar8_rtn(uchar8);
uchar8 __CVAttrs convert_uchar8_rtn(short8);
uchar8 __CVAttrs convert_uchar8_rtn(ushort8);
uchar8 __CVAttrs convert_uchar8_rtn(int8);
uchar8 __CVAttrs convert_uchar8_rtn(uint8);
uchar8 __CVAttrs convert_uchar8_rtn(long8);
uchar8 __CVAttrs convert_uchar8_rtn(ulong8);
uchar8 __CVAttrs convert_uchar8_rtn(float8);
uchar8 __CVAttrs convert_uchar8_rtn(double8);
uchar8 __CVAttrs convert_uchar8_sat(char8);
uchar8 __CVAttrs convert_uchar8_sat(uchar8);
uchar8 __CVAttrs convert_uchar8_sat(short8);
uchar8 __CVAttrs convert_uchar8_sat(ushort8);
uchar8 __CVAttrs convert_uchar8_sat(int8);
uchar8 __CVAttrs convert_uchar8_sat(uint8);
uchar8 __CVAttrs convert_uchar8_sat(long8);
uchar8 __CVAttrs convert_uchar8_sat(ulong8);
uchar8 __CVAttrs convert_uchar8_sat(float8);
uchar8 __CVAttrs convert_uchar8_sat(double8);
uchar8 __CVAttrs convert_uchar8_sat_rte(char8);
uchar8 __CVAttrs convert_uchar8_sat_rte(uchar8);
uchar8 __CVAttrs convert_uchar8_sat_rte(short8);
uchar8 __CVAttrs convert_uchar8_sat_rte(ushort8);
uchar8 __CVAttrs convert_uchar8_sat_rte(int8);
uchar8 __CVAttrs convert_uchar8_sat_rte(uint8);
uchar8 __CVAttrs convert_uchar8_sat_rte(long8);
uchar8 __CVAttrs convert_uchar8_sat_rte(ulong8);
uchar8 __CVAttrs convert_uchar8_sat_rte(float8);
uchar8 __CVAttrs convert_uchar8_sat_rte(double8);
uchar8 __CVAttrs convert_uchar8_sat_rtz(char8);
uchar8 __CVAttrs convert_uchar8_sat_rtz(uchar8);
uchar8 __CVAttrs convert_uchar8_sat_rtz(short8);
uchar8 __CVAttrs convert_uchar8_sat_rtz(ushort8);
uchar8 __CVAttrs convert_uchar8_sat_rtz(int8);
uchar8 __CVAttrs convert_uchar8_sat_rtz(uint8);
uchar8 __CVAttrs convert_uchar8_sat_rtz(long8);
uchar8 __CVAttrs convert_uchar8_sat_rtz(ulong8);
uchar8 __CVAttrs convert_uchar8_sat_rtz(float8);
uchar8 __CVAttrs convert_uchar8_sat_rtz(double8);
uchar8 __CVAttrs convert_uchar8_sat_rtp(char8);
uchar8 __CVAttrs convert_uchar8_sat_rtp(uchar8);
uchar8 __CVAttrs convert_uchar8_sat_rtp(short8);
uchar8 __CVAttrs convert_uchar8_sat_rtp(ushort8);
uchar8 __CVAttrs convert_uchar8_sat_rtp(int8);
uchar8 __CVAttrs convert_uchar8_sat_rtp(uint8);
uchar8 __CVAttrs convert_uchar8_sat_rtp(long8);
uchar8 __CVAttrs convert_uchar8_sat_rtp(ulong8);
uchar8 __CVAttrs convert_uchar8_sat_rtp(float8);
uchar8 __CVAttrs convert_uchar8_sat_rtp(double8);
uchar8 __CVAttrs convert_uchar8_sat_rtn(char8);
uchar8 __CVAttrs convert_uchar8_sat_rtn(uchar8);
uchar8 __CVAttrs convert_uchar8_sat_rtn(short8);
uchar8 __CVAttrs convert_uchar8_sat_rtn(ushort8);
uchar8 __CVAttrs convert_uchar8_sat_rtn(int8);
uchar8 __CVAttrs convert_uchar8_sat_rtn(uint8);
uchar8 __CVAttrs convert_uchar8_sat_rtn(long8);
uchar8 __CVAttrs convert_uchar8_sat_rtn(ulong8);
uchar8 __CVAttrs convert_uchar8_sat_rtn(float8);
uchar8 __CVAttrs convert_uchar8_sat_rtn(double8);
uchar16 __CVAttrs convert_uchar16(char16);
uchar16 __CVAttrs convert_uchar16(uchar16);
uchar16 __CVAttrs convert_uchar16(short16);
uchar16 __CVAttrs convert_uchar16(ushort16);
uchar16 __CVAttrs convert_uchar16(int16);
uchar16 __CVAttrs convert_uchar16(uint16);
uchar16 __CVAttrs convert_uchar16(long16);
uchar16 __CVAttrs convert_uchar16(ulong16);
uchar16 __CVAttrs convert_uchar16(float16);
uchar16 __CVAttrs convert_uchar16(double16);
uchar16 __CVAttrs convert_uchar16_rte(char16);
uchar16 __CVAttrs convert_uchar16_rte(uchar16);
uchar16 __CVAttrs convert_uchar16_rte(short16);
uchar16 __CVAttrs convert_uchar16_rte(ushort16);
uchar16 __CVAttrs convert_uchar16_rte(int16);
uchar16 __CVAttrs convert_uchar16_rte(uint16);
uchar16 __CVAttrs convert_uchar16_rte(long16);
uchar16 __CVAttrs convert_uchar16_rte(ulong16);
uchar16 __CVAttrs convert_uchar16_rte(float16);
uchar16 __CVAttrs convert_uchar16_rte(double16);
uchar16 __CVAttrs convert_uchar16_rtz(char16);
uchar16 __CVAttrs convert_uchar16_rtz(uchar16);
uchar16 __CVAttrs convert_uchar16_rtz(short16);
uchar16 __CVAttrs convert_uchar16_rtz(ushort16);
uchar16 __CVAttrs convert_uchar16_rtz(int16);
uchar16 __CVAttrs convert_uchar16_rtz(uint16);
uchar16 __CVAttrs convert_uchar16_rtz(long16);
uchar16 __CVAttrs convert_uchar16_rtz(ulong16);
uchar16 __CVAttrs convert_uchar16_rtz(float16);
uchar16 __CVAttrs convert_uchar16_rtz(double16);
uchar16 __CVAttrs convert_uchar16_rtp(char16);
uchar16 __CVAttrs convert_uchar16_rtp(uchar16);
uchar16 __CVAttrs convert_uchar16_rtp(short16);
uchar16 __CVAttrs convert_uchar16_rtp(ushort16);
uchar16 __CVAttrs convert_uchar16_rtp(int16);
uchar16 __CVAttrs convert_uchar16_rtp(uint16);
uchar16 __CVAttrs convert_uchar16_rtp(long16);
uchar16 __CVAttrs convert_uchar16_rtp(ulong16);
uchar16 __CVAttrs convert_uchar16_rtp(float16);
uchar16 __CVAttrs convert_uchar16_rtp(double16);
uchar16 __CVAttrs convert_uchar16_rtn(char16);
uchar16 __CVAttrs convert_uchar16_rtn(uchar16);
uchar16 __CVAttrs convert_uchar16_rtn(short16);
uchar16 __CVAttrs convert_uchar16_rtn(ushort16);
uchar16 __CVAttrs convert_uchar16_rtn(int16);
uchar16 __CVAttrs convert_uchar16_rtn(uint16);
uchar16 __CVAttrs convert_uchar16_rtn(long16);
uchar16 __CVAttrs convert_uchar16_rtn(ulong16);
uchar16 __CVAttrs convert_uchar16_rtn(float16);
uchar16 __CVAttrs convert_uchar16_rtn(double16);
uchar16 __CVAttrs convert_uchar16_sat(char16);
uchar16 __CVAttrs convert_uchar16_sat(uchar16);
uchar16 __CVAttrs convert_uchar16_sat(short16);
uchar16 __CVAttrs convert_uchar16_sat(ushort16);
uchar16 __CVAttrs convert_uchar16_sat(int16);
uchar16 __CVAttrs convert_uchar16_sat(uint16);
uchar16 __CVAttrs convert_uchar16_sat(long16);
uchar16 __CVAttrs convert_uchar16_sat(ulong16);
uchar16 __CVAttrs convert_uchar16_sat(float16);
uchar16 __CVAttrs convert_uchar16_sat(double16);
uchar16 __CVAttrs convert_uchar16_sat_rte(char16);
uchar16 __CVAttrs convert_uchar16_sat_rte(uchar16);
uchar16 __CVAttrs convert_uchar16_sat_rte(short16);
uchar16 __CVAttrs convert_uchar16_sat_rte(ushort16);
uchar16 __CVAttrs convert_uchar16_sat_rte(int16);
uchar16 __CVAttrs convert_uchar16_sat_rte(uint16);
uchar16 __CVAttrs convert_uchar16_sat_rte(long16);
uchar16 __CVAttrs convert_uchar16_sat_rte(ulong16);
uchar16 __CVAttrs convert_uchar16_sat_rte(float16);
uchar16 __CVAttrs convert_uchar16_sat_rte(double16);
uchar16 __CVAttrs convert_uchar16_sat_rtz(char16);
uchar16 __CVAttrs convert_uchar16_sat_rtz(uchar16);
uchar16 __CVAttrs convert_uchar16_sat_rtz(short16);
uchar16 __CVAttrs convert_uchar16_sat_rtz(ushort16);
uchar16 __CVAttrs convert_uchar16_sat_rtz(int16);
uchar16 __CVAttrs convert_uchar16_sat_rtz(uint16);
uchar16 __CVAttrs convert_uchar16_sat_rtz(long16);
uchar16 __CVAttrs convert_uchar16_sat_rtz(ulong16);
uchar16 __CVAttrs convert_uchar16_sat_rtz(float16);
uchar16 __CVAttrs convert_uchar16_sat_rtz(double16);
uchar16 __CVAttrs convert_uchar16_sat_rtp(char16);
uchar16 __CVAttrs convert_uchar16_sat_rtp(uchar16);
uchar16 __CVAttrs convert_uchar16_sat_rtp(short16);
uchar16 __CVAttrs convert_uchar16_sat_rtp(ushort16);
uchar16 __CVAttrs convert_uchar16_sat_rtp(int16);
uchar16 __CVAttrs convert_uchar16_sat_rtp(uint16);
uchar16 __CVAttrs convert_uchar16_sat_rtp(long16);
uchar16 __CVAttrs convert_uchar16_sat_rtp(ulong16);
uchar16 __CVAttrs convert_uchar16_sat_rtp(float16);
uchar16 __CVAttrs convert_uchar16_sat_rtp(double16);
uchar16 __CVAttrs convert_uchar16_sat_rtn(char16);
uchar16 __CVAttrs convert_uchar16_sat_rtn(uchar16);
uchar16 __CVAttrs convert_uchar16_sat_rtn(short16);
uchar16 __CVAttrs convert_uchar16_sat_rtn(ushort16);
uchar16 __CVAttrs convert_uchar16_sat_rtn(int16);
uchar16 __CVAttrs convert_uchar16_sat_rtn(uint16);
uchar16 __CVAttrs convert_uchar16_sat_rtn(long16);
uchar16 __CVAttrs convert_uchar16_sat_rtn(ulong16);
uchar16 __CVAttrs convert_uchar16_sat_rtn(float16);
uchar16 __CVAttrs convert_uchar16_sat_rtn(double16);
short __CVAttrs convert_short(char);
short __CVAttrs convert_short(uchar);
short __CVAttrs convert_short(short);
short __CVAttrs convert_short(ushort);
short __CVAttrs convert_short(int);
short __CVAttrs convert_short(uint);
short __CVAttrs convert_short(long);
short __CVAttrs convert_short(ulong);
short __CVAttrs convert_short(float);
short __CVAttrs convert_short(double);
short __CVAttrs convert_short_rte(char);
short __CVAttrs convert_short_rte(uchar);
short __CVAttrs convert_short_rte(short);
short __CVAttrs convert_short_rte(ushort);
short __CVAttrs convert_short_rte(int);
short __CVAttrs convert_short_rte(uint);
short __CVAttrs convert_short_rte(long);
short __CVAttrs convert_short_rte(ulong);
short __CVAttrs convert_short_rte(float);
short __CVAttrs convert_short_rte(double);
short __CVAttrs convert_short_rtz(char);
short __CVAttrs convert_short_rtz(uchar);
short __CVAttrs convert_short_rtz(short);
short __CVAttrs convert_short_rtz(ushort);
short __CVAttrs convert_short_rtz(int);
short __CVAttrs convert_short_rtz(uint);
short __CVAttrs convert_short_rtz(long);
short __CVAttrs convert_short_rtz(ulong);
short __CVAttrs convert_short_rtz(float);
short __CVAttrs convert_short_rtz(double);
short __CVAttrs convert_short_rtp(char);
short __CVAttrs convert_short_rtp(uchar);
short __CVAttrs convert_short_rtp(short);
short __CVAttrs convert_short_rtp(ushort);
short __CVAttrs convert_short_rtp(int);
short __CVAttrs convert_short_rtp(uint);
short __CVAttrs convert_short_rtp(long);
short __CVAttrs convert_short_rtp(ulong);
short __CVAttrs convert_short_rtp(float);
short __CVAttrs convert_short_rtp(double);
short __CVAttrs convert_short_rtn(char);
short __CVAttrs convert_short_rtn(uchar);
short __CVAttrs convert_short_rtn(short);
short __CVAttrs convert_short_rtn(ushort);
short __CVAttrs convert_short_rtn(int);
short __CVAttrs convert_short_rtn(uint);
short __CVAttrs convert_short_rtn(long);
short __CVAttrs convert_short_rtn(ulong);
short __CVAttrs convert_short_rtn(float);
short __CVAttrs convert_short_rtn(double);
short __CVAttrs convert_short_sat(char);
short __CVAttrs convert_short_sat(uchar);
short __CVAttrs convert_short_sat(short);
short __CVAttrs convert_short_sat(ushort);
short __CVAttrs convert_short_sat(int);
short __CVAttrs convert_short_sat(uint);
short __CVAttrs convert_short_sat(long);
short __CVAttrs convert_short_sat(ulong);
short __CVAttrs convert_short_sat(float);
short __CVAttrs convert_short_sat(double);
short __CVAttrs convert_short_sat_rte(char);
short __CVAttrs convert_short_sat_rte(uchar);
short __CVAttrs convert_short_sat_rte(short);
short __CVAttrs convert_short_sat_rte(ushort);
short __CVAttrs convert_short_sat_rte(int);
short __CVAttrs convert_short_sat_rte(uint);
short __CVAttrs convert_short_sat_rte(long);
short __CVAttrs convert_short_sat_rte(ulong);
short __CVAttrs convert_short_sat_rte(float);
short __CVAttrs convert_short_sat_rte(double);
short __CVAttrs convert_short_sat_rtz(char);
short __CVAttrs convert_short_sat_rtz(uchar);
short __CVAttrs convert_short_sat_rtz(short);
short __CVAttrs convert_short_sat_rtz(ushort);
short __CVAttrs convert_short_sat_rtz(int);
short __CVAttrs convert_short_sat_rtz(uint);
short __CVAttrs convert_short_sat_rtz(long);
short __CVAttrs convert_short_sat_rtz(ulong);
short __CVAttrs convert_short_sat_rtz(float);
short __CVAttrs convert_short_sat_rtz(double);
short __CVAttrs convert_short_sat_rtp(char);
short __CVAttrs convert_short_sat_rtp(uchar);
short __CVAttrs convert_short_sat_rtp(short);
short __CVAttrs convert_short_sat_rtp(ushort);
short __CVAttrs convert_short_sat_rtp(int);
short __CVAttrs convert_short_sat_rtp(uint);
short __CVAttrs convert_short_sat_rtp(long);
short __CVAttrs convert_short_sat_rtp(ulong);
short __CVAttrs convert_short_sat_rtp(float);
short __CVAttrs convert_short_sat_rtp(double);
short __CVAttrs convert_short_sat_rtn(char);
short __CVAttrs convert_short_sat_rtn(uchar);
short __CVAttrs convert_short_sat_rtn(short);
short __CVAttrs convert_short_sat_rtn(ushort);
short __CVAttrs convert_short_sat_rtn(int);
short __CVAttrs convert_short_sat_rtn(uint);
short __CVAttrs convert_short_sat_rtn(long);
short __CVAttrs convert_short_sat_rtn(ulong);
short __CVAttrs convert_short_sat_rtn(float);
short __CVAttrs convert_short_sat_rtn(double);
short2 __CVAttrs convert_short2(char2);
short2 __CVAttrs convert_short2(uchar2);
short2 __CVAttrs convert_short2(short2);
short2 __CVAttrs convert_short2(ushort2);
short2 __CVAttrs convert_short2(int2);
short2 __CVAttrs convert_short2(uint2);
short2 __CVAttrs convert_short2(long2);
short2 __CVAttrs convert_short2(ulong2);
short2 __CVAttrs convert_short2(float2);
short2 __CVAttrs convert_short2(double2);
short2 __CVAttrs convert_short2_rte(char2);
short2 __CVAttrs convert_short2_rte(uchar2);
short2 __CVAttrs convert_short2_rte(short2);
short2 __CVAttrs convert_short2_rte(ushort2);
short2 __CVAttrs convert_short2_rte(int2);
short2 __CVAttrs convert_short2_rte(uint2);
short2 __CVAttrs convert_short2_rte(long2);
short2 __CVAttrs convert_short2_rte(ulong2);
short2 __CVAttrs convert_short2_rte(float2);
short2 __CVAttrs convert_short2_rte(double2);
short2 __CVAttrs convert_short2_rtz(char2);
short2 __CVAttrs convert_short2_rtz(uchar2);
short2 __CVAttrs convert_short2_rtz(short2);
short2 __CVAttrs convert_short2_rtz(ushort2);
short2 __CVAttrs convert_short2_rtz(int2);
short2 __CVAttrs convert_short2_rtz(uint2);
short2 __CVAttrs convert_short2_rtz(long2);
short2 __CVAttrs convert_short2_rtz(ulong2);
short2 __CVAttrs convert_short2_rtz(float2);
short2 __CVAttrs convert_short2_rtz(double2);
short2 __CVAttrs convert_short2_rtp(char2);
short2 __CVAttrs convert_short2_rtp(uchar2);
short2 __CVAttrs convert_short2_rtp(short2);
short2 __CVAttrs convert_short2_rtp(ushort2);
short2 __CVAttrs convert_short2_rtp(int2);
short2 __CVAttrs convert_short2_rtp(uint2);
short2 __CVAttrs convert_short2_rtp(long2);
short2 __CVAttrs convert_short2_rtp(ulong2);
short2 __CVAttrs convert_short2_rtp(float2);
short2 __CVAttrs convert_short2_rtp(double2);
short2 __CVAttrs convert_short2_rtn(char2);
short2 __CVAttrs convert_short2_rtn(uchar2);
short2 __CVAttrs convert_short2_rtn(short2);
short2 __CVAttrs convert_short2_rtn(ushort2);
short2 __CVAttrs convert_short2_rtn(int2);
short2 __CVAttrs convert_short2_rtn(uint2);
short2 __CVAttrs convert_short2_rtn(long2);
short2 __CVAttrs convert_short2_rtn(ulong2);
short2 __CVAttrs convert_short2_rtn(float2);
short2 __CVAttrs convert_short2_rtn(double2);
short2 __CVAttrs convert_short2_sat(char2);
short2 __CVAttrs convert_short2_sat(uchar2);
short2 __CVAttrs convert_short2_sat(short2);
short2 __CVAttrs convert_short2_sat(ushort2);
short2 __CVAttrs convert_short2_sat(int2);
short2 __CVAttrs convert_short2_sat(uint2);
short2 __CVAttrs convert_short2_sat(long2);
short2 __CVAttrs convert_short2_sat(ulong2);
short2 __CVAttrs convert_short2_sat(float2);
short2 __CVAttrs convert_short2_sat(double2);
short2 __CVAttrs convert_short2_sat_rte(char2);
short2 __CVAttrs convert_short2_sat_rte(uchar2);
short2 __CVAttrs convert_short2_sat_rte(short2);
short2 __CVAttrs convert_short2_sat_rte(ushort2);
short2 __CVAttrs convert_short2_sat_rte(int2);
short2 __CVAttrs convert_short2_sat_rte(uint2);
short2 __CVAttrs convert_short2_sat_rte(long2);
short2 __CVAttrs convert_short2_sat_rte(ulong2);
short2 __CVAttrs convert_short2_sat_rte(float2);
short2 __CVAttrs convert_short2_sat_rte(double2);
short2 __CVAttrs convert_short2_sat_rtz(char2);
short2 __CVAttrs convert_short2_sat_rtz(uchar2);
short2 __CVAttrs convert_short2_sat_rtz(short2);
short2 __CVAttrs convert_short2_sat_rtz(ushort2);
short2 __CVAttrs convert_short2_sat_rtz(int2);
short2 __CVAttrs convert_short2_sat_rtz(uint2);
short2 __CVAttrs convert_short2_sat_rtz(long2);
short2 __CVAttrs convert_short2_sat_rtz(ulong2);
short2 __CVAttrs convert_short2_sat_rtz(float2);
short2 __CVAttrs convert_short2_sat_rtz(double2);
short2 __CVAttrs convert_short2_sat_rtp(char2);
short2 __CVAttrs convert_short2_sat_rtp(uchar2);
short2 __CVAttrs convert_short2_sat_rtp(short2);
short2 __CVAttrs convert_short2_sat_rtp(ushort2);
short2 __CVAttrs convert_short2_sat_rtp(int2);
short2 __CVAttrs convert_short2_sat_rtp(uint2);
short2 __CVAttrs convert_short2_sat_rtp(long2);
short2 __CVAttrs convert_short2_sat_rtp(ulong2);
short2 __CVAttrs convert_short2_sat_rtp(float2);
short2 __CVAttrs convert_short2_sat_rtp(double2);
short2 __CVAttrs convert_short2_sat_rtn(char2);
short2 __CVAttrs convert_short2_sat_rtn(uchar2);
short2 __CVAttrs convert_short2_sat_rtn(short2);
short2 __CVAttrs convert_short2_sat_rtn(ushort2);
short2 __CVAttrs convert_short2_sat_rtn(int2);
short2 __CVAttrs convert_short2_sat_rtn(uint2);
short2 __CVAttrs convert_short2_sat_rtn(long2);
short2 __CVAttrs convert_short2_sat_rtn(ulong2);
short2 __CVAttrs convert_short2_sat_rtn(float2);
short2 __CVAttrs convert_short2_sat_rtn(double2);
#if __OPENCL_C_VERSION__ >= 110
short3 __CVAttrs convert_short3(char3);
short3 __CVAttrs convert_short3(uchar3);
short3 __CVAttrs convert_short3(short3);
short3 __CVAttrs convert_short3(ushort3);
short3 __CVAttrs convert_short3(int3);
short3 __CVAttrs convert_short3(uint3);
short3 __CVAttrs convert_short3(long3);
short3 __CVAttrs convert_short3(ulong3);
short3 __CVAttrs convert_short3(float3);
short3 __CVAttrs convert_short3(double3);
short3 __CVAttrs convert_short3_rte(char3);
short3 __CVAttrs convert_short3_rte(uchar3);
short3 __CVAttrs convert_short3_rte(short3);
short3 __CVAttrs convert_short3_rte(ushort3);
short3 __CVAttrs convert_short3_rte(int3);
short3 __CVAttrs convert_short3_rte(uint3);
short3 __CVAttrs convert_short3_rte(long3);
short3 __CVAttrs convert_short3_rte(ulong3);
short3 __CVAttrs convert_short3_rte(float3);
short3 __CVAttrs convert_short3_rte(double3);
short3 __CVAttrs convert_short3_rtz(char3);
short3 __CVAttrs convert_short3_rtz(uchar3);
short3 __CVAttrs convert_short3_rtz(short3);
short3 __CVAttrs convert_short3_rtz(ushort3);
short3 __CVAttrs convert_short3_rtz(int3);
short3 __CVAttrs convert_short3_rtz(uint3);
short3 __CVAttrs convert_short3_rtz(long3);
short3 __CVAttrs convert_short3_rtz(ulong3);
short3 __CVAttrs convert_short3_rtz(float3);
short3 __CVAttrs convert_short3_rtz(double3);
short3 __CVAttrs convert_short3_rtp(char3);
short3 __CVAttrs convert_short3_rtp(uchar3);
short3 __CVAttrs convert_short3_rtp(short3);
short3 __CVAttrs convert_short3_rtp(ushort3);
short3 __CVAttrs convert_short3_rtp(int3);
short3 __CVAttrs convert_short3_rtp(uint3);
short3 __CVAttrs convert_short3_rtp(long3);
short3 __CVAttrs convert_short3_rtp(ulong3);
short3 __CVAttrs convert_short3_rtp(float3);
short3 __CVAttrs convert_short3_rtp(double3);
short3 __CVAttrs convert_short3_rtn(char3);
short3 __CVAttrs convert_short3_rtn(uchar3);
short3 __CVAttrs convert_short3_rtn(short3);
short3 __CVAttrs convert_short3_rtn(ushort3);
short3 __CVAttrs convert_short3_rtn(int3);
short3 __CVAttrs convert_short3_rtn(uint3);
short3 __CVAttrs convert_short3_rtn(long3);
short3 __CVAttrs convert_short3_rtn(ulong3);
short3 __CVAttrs convert_short3_rtn(float3);
short3 __CVAttrs convert_short3_rtn(double3);
short3 __CVAttrs convert_short3_sat(char3);
short3 __CVAttrs convert_short3_sat(uchar3);
short3 __CVAttrs convert_short3_sat(short3);
short3 __CVAttrs convert_short3_sat(ushort3);
short3 __CVAttrs convert_short3_sat(int3);
short3 __CVAttrs convert_short3_sat(uint3);
short3 __CVAttrs convert_short3_sat(long3);
short3 __CVAttrs convert_short3_sat(ulong3);
short3 __CVAttrs convert_short3_sat(float3);
short3 __CVAttrs convert_short3_sat(double3);
short3 __CVAttrs convert_short3_sat_rte(char3);
short3 __CVAttrs convert_short3_sat_rte(uchar3);
short3 __CVAttrs convert_short3_sat_rte(short3);
short3 __CVAttrs convert_short3_sat_rte(ushort3);
short3 __CVAttrs convert_short3_sat_rte(int3);
short3 __CVAttrs convert_short3_sat_rte(uint3);
short3 __CVAttrs convert_short3_sat_rte(long3);
short3 __CVAttrs convert_short3_sat_rte(ulong3);
short3 __CVAttrs convert_short3_sat_rte(float3);
short3 __CVAttrs convert_short3_sat_rte(double3);
short3 __CVAttrs convert_short3_sat_rtz(char3);
short3 __CVAttrs convert_short3_sat_rtz(uchar3);
short3 __CVAttrs convert_short3_sat_rtz(short3);
short3 __CVAttrs convert_short3_sat_rtz(ushort3);
short3 __CVAttrs convert_short3_sat_rtz(int3);
short3 __CVAttrs convert_short3_sat_rtz(uint3);
short3 __CVAttrs convert_short3_sat_rtz(long3);
short3 __CVAttrs convert_short3_sat_rtz(ulong3);
short3 __CVAttrs convert_short3_sat_rtz(float3);
short3 __CVAttrs convert_short3_sat_rtz(double3);
short3 __CVAttrs convert_short3_sat_rtp(char3);
short3 __CVAttrs convert_short3_sat_rtp(uchar3);
short3 __CVAttrs convert_short3_sat_rtp(short3);
short3 __CVAttrs convert_short3_sat_rtp(ushort3);
short3 __CVAttrs convert_short3_sat_rtp(int3);
short3 __CVAttrs convert_short3_sat_rtp(uint3);
short3 __CVAttrs convert_short3_sat_rtp(long3);
short3 __CVAttrs convert_short3_sat_rtp(ulong3);
short3 __CVAttrs convert_short3_sat_rtp(float3);
short3 __CVAttrs convert_short3_sat_rtp(double3);
short3 __CVAttrs convert_short3_sat_rtn(char3);
short3 __CVAttrs convert_short3_sat_rtn(uchar3);
short3 __CVAttrs convert_short3_sat_rtn(short3);
short3 __CVAttrs convert_short3_sat_rtn(ushort3);
short3 __CVAttrs convert_short3_sat_rtn(int3);
short3 __CVAttrs convert_short3_sat_rtn(uint3);
short3 __CVAttrs convert_short3_sat_rtn(long3);
short3 __CVAttrs convert_short3_sat_rtn(ulong3);
short3 __CVAttrs convert_short3_sat_rtn(float3);
short3 __CVAttrs convert_short3_sat_rtn(double3);
#endif
short4 __CVAttrs convert_short4(char4);
short4 __CVAttrs convert_short4(uchar4);
short4 __CVAttrs convert_short4(short4);
short4 __CVAttrs convert_short4(ushort4);
short4 __CVAttrs convert_short4(int4);
short4 __CVAttrs convert_short4(uint4);
short4 __CVAttrs convert_short4(long4);
short4 __CVAttrs convert_short4(ulong4);
short4 __CVAttrs convert_short4(float4);
short4 __CVAttrs convert_short4(double4);
short4 __CVAttrs convert_short4_rte(char4);
short4 __CVAttrs convert_short4_rte(uchar4);
short4 __CVAttrs convert_short4_rte(short4);
short4 __CVAttrs convert_short4_rte(ushort4);
short4 __CVAttrs convert_short4_rte(int4);
short4 __CVAttrs convert_short4_rte(uint4);
short4 __CVAttrs convert_short4_rte(long4);
short4 __CVAttrs convert_short4_rte(ulong4);
short4 __CVAttrs convert_short4_rte(float4);
short4 __CVAttrs convert_short4_rte(double4);
short4 __CVAttrs convert_short4_rtz(char4);
short4 __CVAttrs convert_short4_rtz(uchar4);
short4 __CVAttrs convert_short4_rtz(short4);
short4 __CVAttrs convert_short4_rtz(ushort4);
short4 __CVAttrs convert_short4_rtz(int4);
short4 __CVAttrs convert_short4_rtz(uint4);
short4 __CVAttrs convert_short4_rtz(long4);
short4 __CVAttrs convert_short4_rtz(ulong4);
short4 __CVAttrs convert_short4_rtz(float4);
short4 __CVAttrs convert_short4_rtz(double4);
short4 __CVAttrs convert_short4_rtp(char4);
short4 __CVAttrs convert_short4_rtp(uchar4);
short4 __CVAttrs convert_short4_rtp(short4);
short4 __CVAttrs convert_short4_rtp(ushort4);
short4 __CVAttrs convert_short4_rtp(int4);
short4 __CVAttrs convert_short4_rtp(uint4);
short4 __CVAttrs convert_short4_rtp(long4);
short4 __CVAttrs convert_short4_rtp(ulong4);
short4 __CVAttrs convert_short4_rtp(float4);
short4 __CVAttrs convert_short4_rtp(double4);
short4 __CVAttrs convert_short4_rtn(char4);
short4 __CVAttrs convert_short4_rtn(uchar4);
short4 __CVAttrs convert_short4_rtn(short4);
short4 __CVAttrs convert_short4_rtn(ushort4);
short4 __CVAttrs convert_short4_rtn(int4);
short4 __CVAttrs convert_short4_rtn(uint4);
short4 __CVAttrs convert_short4_rtn(long4);
short4 __CVAttrs convert_short4_rtn(ulong4);
short4 __CVAttrs convert_short4_rtn(float4);
short4 __CVAttrs convert_short4_rtn(double4);
short4 __CVAttrs convert_short4_sat(char4);
short4 __CVAttrs convert_short4_sat(uchar4);
short4 __CVAttrs convert_short4_sat(short4);
short4 __CVAttrs convert_short4_sat(ushort4);
short4 __CVAttrs convert_short4_sat(int4);
short4 __CVAttrs convert_short4_sat(uint4);
short4 __CVAttrs convert_short4_sat(long4);
short4 __CVAttrs convert_short4_sat(ulong4);
short4 __CVAttrs convert_short4_sat(float4);
short4 __CVAttrs convert_short4_sat(double4);
short4 __CVAttrs convert_short4_sat_rte(char4);
short4 __CVAttrs convert_short4_sat_rte(uchar4);
short4 __CVAttrs convert_short4_sat_rte(short4);
short4 __CVAttrs convert_short4_sat_rte(ushort4);
short4 __CVAttrs convert_short4_sat_rte(int4);
short4 __CVAttrs convert_short4_sat_rte(uint4);
short4 __CVAttrs convert_short4_sat_rte(long4);
short4 __CVAttrs convert_short4_sat_rte(ulong4);
short4 __CVAttrs convert_short4_sat_rte(float4);
short4 __CVAttrs convert_short4_sat_rte(double4);
short4 __CVAttrs convert_short4_sat_rtz(char4);
short4 __CVAttrs convert_short4_sat_rtz(uchar4);
short4 __CVAttrs convert_short4_sat_rtz(short4);
short4 __CVAttrs convert_short4_sat_rtz(ushort4);
short4 __CVAttrs convert_short4_sat_rtz(int4);
short4 __CVAttrs convert_short4_sat_rtz(uint4);
short4 __CVAttrs convert_short4_sat_rtz(long4);
short4 __CVAttrs convert_short4_sat_rtz(ulong4);
short4 __CVAttrs convert_short4_sat_rtz(float4);
short4 __CVAttrs convert_short4_sat_rtz(double4);
short4 __CVAttrs convert_short4_sat_rtp(char4);
short4 __CVAttrs convert_short4_sat_rtp(uchar4);
short4 __CVAttrs convert_short4_sat_rtp(short4);
short4 __CVAttrs convert_short4_sat_rtp(ushort4);
short4 __CVAttrs convert_short4_sat_rtp(int4);
short4 __CVAttrs convert_short4_sat_rtp(uint4);
short4 __CVAttrs convert_short4_sat_rtp(long4);
short4 __CVAttrs convert_short4_sat_rtp(ulong4);
short4 __CVAttrs convert_short4_sat_rtp(float4);
short4 __CVAttrs convert_short4_sat_rtp(double4);
short4 __CVAttrs convert_short4_sat_rtn(char4);
short4 __CVAttrs convert_short4_sat_rtn(uchar4);
short4 __CVAttrs convert_short4_sat_rtn(short4);
short4 __CVAttrs convert_short4_sat_rtn(ushort4);
short4 __CVAttrs convert_short4_sat_rtn(int4);
short4 __CVAttrs convert_short4_sat_rtn(uint4);
short4 __CVAttrs convert_short4_sat_rtn(long4);
short4 __CVAttrs convert_short4_sat_rtn(ulong4);
short4 __CVAttrs convert_short4_sat_rtn(float4);
short4 __CVAttrs convert_short4_sat_rtn(double4);
short8 __CVAttrs convert_short8(char8);
short8 __CVAttrs convert_short8(uchar8);
short8 __CVAttrs convert_short8(short8);
short8 __CVAttrs convert_short8(ushort8);
short8 __CVAttrs convert_short8(int8);
short8 __CVAttrs convert_short8(uint8);
short8 __CVAttrs convert_short8(long8);
short8 __CVAttrs convert_short8(ulong8);
short8 __CVAttrs convert_short8(float8);
short8 __CVAttrs convert_short8(double8);
short8 __CVAttrs convert_short8_rte(char8);
short8 __CVAttrs convert_short8_rte(uchar8);
short8 __CVAttrs convert_short8_rte(short8);
short8 __CVAttrs convert_short8_rte(ushort8);
short8 __CVAttrs convert_short8_rte(int8);
short8 __CVAttrs convert_short8_rte(uint8);
short8 __CVAttrs convert_short8_rte(long8);
short8 __CVAttrs convert_short8_rte(ulong8);
short8 __CVAttrs convert_short8_rte(float8);
short8 __CVAttrs convert_short8_rte(double8);
short8 __CVAttrs convert_short8_rtz(char8);
short8 __CVAttrs convert_short8_rtz(uchar8);
short8 __CVAttrs convert_short8_rtz(short8);
short8 __CVAttrs convert_short8_rtz(ushort8);
short8 __CVAttrs convert_short8_rtz(int8);
short8 __CVAttrs convert_short8_rtz(uint8);
short8 __CVAttrs convert_short8_rtz(long8);
short8 __CVAttrs convert_short8_rtz(ulong8);
short8 __CVAttrs convert_short8_rtz(float8);
short8 __CVAttrs convert_short8_rtz(double8);
short8 __CVAttrs convert_short8_rtp(char8);
short8 __CVAttrs convert_short8_rtp(uchar8);
short8 __CVAttrs convert_short8_rtp(short8);
short8 __CVAttrs convert_short8_rtp(ushort8);
short8 __CVAttrs convert_short8_rtp(int8);
short8 __CVAttrs convert_short8_rtp(uint8);
short8 __CVAttrs convert_short8_rtp(long8);
short8 __CVAttrs convert_short8_rtp(ulong8);
short8 __CVAttrs convert_short8_rtp(float8);
short8 __CVAttrs convert_short8_rtp(double8);
short8 __CVAttrs convert_short8_rtn(char8);
short8 __CVAttrs convert_short8_rtn(uchar8);
short8 __CVAttrs convert_short8_rtn(short8);
short8 __CVAttrs convert_short8_rtn(ushort8);
short8 __CVAttrs convert_short8_rtn(int8);
short8 __CVAttrs convert_short8_rtn(uint8);
short8 __CVAttrs convert_short8_rtn(long8);
short8 __CVAttrs convert_short8_rtn(ulong8);
short8 __CVAttrs convert_short8_rtn(float8);
short8 __CVAttrs convert_short8_rtn(double8);
short8 __CVAttrs convert_short8_sat(char8);
short8 __CVAttrs convert_short8_sat(uchar8);
short8 __CVAttrs convert_short8_sat(short8);
short8 __CVAttrs convert_short8_sat(ushort8);
short8 __CVAttrs convert_short8_sat(int8);
short8 __CVAttrs convert_short8_sat(uint8);
short8 __CVAttrs convert_short8_sat(long8);
short8 __CVAttrs convert_short8_sat(ulong8);
short8 __CVAttrs convert_short8_sat(float8);
short8 __CVAttrs convert_short8_sat(double8);
short8 __CVAttrs convert_short8_sat_rte(char8);
short8 __CVAttrs convert_short8_sat_rte(uchar8);
short8 __CVAttrs convert_short8_sat_rte(short8);
short8 __CVAttrs convert_short8_sat_rte(ushort8);
short8 __CVAttrs convert_short8_sat_rte(int8);
short8 __CVAttrs convert_short8_sat_rte(uint8);
short8 __CVAttrs convert_short8_sat_rte(long8);
short8 __CVAttrs convert_short8_sat_rte(ulong8);
short8 __CVAttrs convert_short8_sat_rte(float8);
short8 __CVAttrs convert_short8_sat_rte(double8);
short8 __CVAttrs convert_short8_sat_rtz(char8);
short8 __CVAttrs convert_short8_sat_rtz(uchar8);
short8 __CVAttrs convert_short8_sat_rtz(short8);
short8 __CVAttrs convert_short8_sat_rtz(ushort8);
short8 __CVAttrs convert_short8_sat_rtz(int8);
short8 __CVAttrs convert_short8_sat_rtz(uint8);
short8 __CVAttrs convert_short8_sat_rtz(long8);
short8 __CVAttrs convert_short8_sat_rtz(ulong8);
short8 __CVAttrs convert_short8_sat_rtz(float8);
short8 __CVAttrs convert_short8_sat_rtz(double8);
short8 __CVAttrs convert_short8_sat_rtp(char8);
short8 __CVAttrs convert_short8_sat_rtp(uchar8);
short8 __CVAttrs convert_short8_sat_rtp(short8);
short8 __CVAttrs convert_short8_sat_rtp(ushort8);
short8 __CVAttrs convert_short8_sat_rtp(int8);
short8 __CVAttrs convert_short8_sat_rtp(uint8);
short8 __CVAttrs convert_short8_sat_rtp(long8);
short8 __CVAttrs convert_short8_sat_rtp(ulong8);
short8 __CVAttrs convert_short8_sat_rtp(float8);
short8 __CVAttrs convert_short8_sat_rtp(double8);
short8 __CVAttrs convert_short8_sat_rtn(char8);
short8 __CVAttrs convert_short8_sat_rtn(uchar8);
short8 __CVAttrs convert_short8_sat_rtn(short8);
short8 __CVAttrs convert_short8_sat_rtn(ushort8);
short8 __CVAttrs convert_short8_sat_rtn(int8);
short8 __CVAttrs convert_short8_sat_rtn(uint8);
short8 __CVAttrs convert_short8_sat_rtn(long8);
short8 __CVAttrs convert_short8_sat_rtn(ulong8);
short8 __CVAttrs convert_short8_sat_rtn(float8);
short8 __CVAttrs convert_short8_sat_rtn(double8);
short16 __CVAttrs convert_short16(char16);
short16 __CVAttrs convert_short16(uchar16);
short16 __CVAttrs convert_short16(short16);
short16 __CVAttrs convert_short16(ushort16);
short16 __CVAttrs convert_short16(int16);
short16 __CVAttrs convert_short16(uint16);
short16 __CVAttrs convert_short16(long16);
short16 __CVAttrs convert_short16(ulong16);
short16 __CVAttrs convert_short16(float16);
short16 __CVAttrs convert_short16(double16);
short16 __CVAttrs convert_short16_rte(char16);
short16 __CVAttrs convert_short16_rte(uchar16);
short16 __CVAttrs convert_short16_rte(short16);
short16 __CVAttrs convert_short16_rte(ushort16);
short16 __CVAttrs convert_short16_rte(int16);
short16 __CVAttrs convert_short16_rte(uint16);
short16 __CVAttrs convert_short16_rte(long16);
short16 __CVAttrs convert_short16_rte(ulong16);
short16 __CVAttrs convert_short16_rte(float16);
short16 __CVAttrs convert_short16_rte(double16);
short16 __CVAttrs convert_short16_rtz(char16);
short16 __CVAttrs convert_short16_rtz(uchar16);
short16 __CVAttrs convert_short16_rtz(short16);
short16 __CVAttrs convert_short16_rtz(ushort16);
short16 __CVAttrs convert_short16_rtz(int16);
short16 __CVAttrs convert_short16_rtz(uint16);
short16 __CVAttrs convert_short16_rtz(long16);
short16 __CVAttrs convert_short16_rtz(ulong16);
short16 __CVAttrs convert_short16_rtz(float16);
short16 __CVAttrs convert_short16_rtz(double16);
short16 __CVAttrs convert_short16_rtp(char16);
short16 __CVAttrs convert_short16_rtp(uchar16);
short16 __CVAttrs convert_short16_rtp(short16);
short16 __CVAttrs convert_short16_rtp(ushort16);
short16 __CVAttrs convert_short16_rtp(int16);
short16 __CVAttrs convert_short16_rtp(uint16);
short16 __CVAttrs convert_short16_rtp(long16);
short16 __CVAttrs convert_short16_rtp(ulong16);
short16 __CVAttrs convert_short16_rtp(float16);
short16 __CVAttrs convert_short16_rtp(double16);
short16 __CVAttrs convert_short16_rtn(char16);
short16 __CVAttrs convert_short16_rtn(uchar16);
short16 __CVAttrs convert_short16_rtn(short16);
short16 __CVAttrs convert_short16_rtn(ushort16);
short16 __CVAttrs convert_short16_rtn(int16);
short16 __CVAttrs convert_short16_rtn(uint16);
short16 __CVAttrs convert_short16_rtn(long16);
short16 __CVAttrs convert_short16_rtn(ulong16);
short16 __CVAttrs convert_short16_rtn(float16);
short16 __CVAttrs convert_short16_rtn(double16);
short16 __CVAttrs convert_short16_sat(char16);
short16 __CVAttrs convert_short16_sat(uchar16);
short16 __CVAttrs convert_short16_sat(short16);
short16 __CVAttrs convert_short16_sat(ushort16);
short16 __CVAttrs convert_short16_sat(int16);
short16 __CVAttrs convert_short16_sat(uint16);
short16 __CVAttrs convert_short16_sat(long16);
short16 __CVAttrs convert_short16_sat(ulong16);
short16 __CVAttrs convert_short16_sat(float16);
short16 __CVAttrs convert_short16_sat(double16);
short16 __CVAttrs convert_short16_sat_rte(char16);
short16 __CVAttrs convert_short16_sat_rte(uchar16);
short16 __CVAttrs convert_short16_sat_rte(short16);
short16 __CVAttrs convert_short16_sat_rte(ushort16);
short16 __CVAttrs convert_short16_sat_rte(int16);
short16 __CVAttrs convert_short16_sat_rte(uint16);
short16 __CVAttrs convert_short16_sat_rte(long16);
short16 __CVAttrs convert_short16_sat_rte(ulong16);
short16 __CVAttrs convert_short16_sat_rte(float16);
short16 __CVAttrs convert_short16_sat_rte(double16);
short16 __CVAttrs convert_short16_sat_rtz(char16);
short16 __CVAttrs convert_short16_sat_rtz(uchar16);
short16 __CVAttrs convert_short16_sat_rtz(short16);
short16 __CVAttrs convert_short16_sat_rtz(ushort16);
short16 __CVAttrs convert_short16_sat_rtz(int16);
short16 __CVAttrs convert_short16_sat_rtz(uint16);
short16 __CVAttrs convert_short16_sat_rtz(long16);
short16 __CVAttrs convert_short16_sat_rtz(ulong16);
short16 __CVAttrs convert_short16_sat_rtz(float16);
short16 __CVAttrs convert_short16_sat_rtz(double16);
short16 __CVAttrs convert_short16_sat_rtp(char16);
short16 __CVAttrs convert_short16_sat_rtp(uchar16);
short16 __CVAttrs convert_short16_sat_rtp(short16);
short16 __CVAttrs convert_short16_sat_rtp(ushort16);
short16 __CVAttrs convert_short16_sat_rtp(int16);
short16 __CVAttrs convert_short16_sat_rtp(uint16);
short16 __CVAttrs convert_short16_sat_rtp(long16);
short16 __CVAttrs convert_short16_sat_rtp(ulong16);
short16 __CVAttrs convert_short16_sat_rtp(float16);
short16 __CVAttrs convert_short16_sat_rtp(double16);
short16 __CVAttrs convert_short16_sat_rtn(char16);
short16 __CVAttrs convert_short16_sat_rtn(uchar16);
short16 __CVAttrs convert_short16_sat_rtn(short16);
short16 __CVAttrs convert_short16_sat_rtn(ushort16);
short16 __CVAttrs convert_short16_sat_rtn(int16);
short16 __CVAttrs convert_short16_sat_rtn(uint16);
short16 __CVAttrs convert_short16_sat_rtn(long16);
short16 __CVAttrs convert_short16_sat_rtn(ulong16);
short16 __CVAttrs convert_short16_sat_rtn(float16);
short16 __CVAttrs convert_short16_sat_rtn(double16);
ushort __CVAttrs convert_ushort(char);
ushort __CVAttrs convert_ushort(uchar);
ushort __CVAttrs convert_ushort(short);
ushort __CVAttrs convert_ushort(ushort);
ushort __CVAttrs convert_ushort(int);
ushort __CVAttrs convert_ushort(uint);
ushort __CVAttrs convert_ushort(long);
ushort __CVAttrs convert_ushort(ulong);
ushort __CVAttrs convert_ushort(float);
ushort __CVAttrs convert_ushort(double);
ushort __CVAttrs convert_ushort_rte(char);
ushort __CVAttrs convert_ushort_rte(uchar);
ushort __CVAttrs convert_ushort_rte(short);
ushort __CVAttrs convert_ushort_rte(ushort);
ushort __CVAttrs convert_ushort_rte(int);
ushort __CVAttrs convert_ushort_rte(uint);
ushort __CVAttrs convert_ushort_rte(long);
ushort __CVAttrs convert_ushort_rte(ulong);
ushort __CVAttrs convert_ushort_rte(float);
ushort __CVAttrs convert_ushort_rte(double);
ushort __CVAttrs convert_ushort_rtz(char);
ushort __CVAttrs convert_ushort_rtz(uchar);
ushort __CVAttrs convert_ushort_rtz(short);
ushort __CVAttrs convert_ushort_rtz(ushort);
ushort __CVAttrs convert_ushort_rtz(int);
ushort __CVAttrs convert_ushort_rtz(uint);
ushort __CVAttrs convert_ushort_rtz(long);
ushort __CVAttrs convert_ushort_rtz(ulong);
ushort __CVAttrs convert_ushort_rtz(float);
ushort __CVAttrs convert_ushort_rtz(double);
ushort __CVAttrs convert_ushort_rtp(char);
ushort __CVAttrs convert_ushort_rtp(uchar);
ushort __CVAttrs convert_ushort_rtp(short);
ushort __CVAttrs convert_ushort_rtp(ushort);
ushort __CVAttrs convert_ushort_rtp(int);
ushort __CVAttrs convert_ushort_rtp(uint);
ushort __CVAttrs convert_ushort_rtp(long);
ushort __CVAttrs convert_ushort_rtp(ulong);
ushort __CVAttrs convert_ushort_rtp(float);
ushort __CVAttrs convert_ushort_rtp(double);
ushort __CVAttrs convert_ushort_rtn(char);
ushort __CVAttrs convert_ushort_rtn(uchar);
ushort __CVAttrs convert_ushort_rtn(short);
ushort __CVAttrs convert_ushort_rtn(ushort);
ushort __CVAttrs convert_ushort_rtn(int);
ushort __CVAttrs convert_ushort_rtn(uint);
ushort __CVAttrs convert_ushort_rtn(long);
ushort __CVAttrs convert_ushort_rtn(ulong);
ushort __CVAttrs convert_ushort_rtn(float);
ushort __CVAttrs convert_ushort_rtn(double);
ushort __CVAttrs convert_ushort_sat(char);
ushort __CVAttrs convert_ushort_sat(uchar);
ushort __CVAttrs convert_ushort_sat(short);
ushort __CVAttrs convert_ushort_sat(ushort);
ushort __CVAttrs convert_ushort_sat(int);
ushort __CVAttrs convert_ushort_sat(uint);
ushort __CVAttrs convert_ushort_sat(long);
ushort __CVAttrs convert_ushort_sat(ulong);
ushort __CVAttrs convert_ushort_sat(float);
ushort __CVAttrs convert_ushort_sat(double);
ushort __CVAttrs convert_ushort_sat_rte(char);
ushort __CVAttrs convert_ushort_sat_rte(uchar);
ushort __CVAttrs convert_ushort_sat_rte(short);
ushort __CVAttrs convert_ushort_sat_rte(ushort);
ushort __CVAttrs convert_ushort_sat_rte(int);
ushort __CVAttrs convert_ushort_sat_rte(uint);
ushort __CVAttrs convert_ushort_sat_rte(long);
ushort __CVAttrs convert_ushort_sat_rte(ulong);
ushort __CVAttrs convert_ushort_sat_rte(float);
ushort __CVAttrs convert_ushort_sat_rte(double);
ushort __CVAttrs convert_ushort_sat_rtz(char);
ushort __CVAttrs convert_ushort_sat_rtz(uchar);
ushort __CVAttrs convert_ushort_sat_rtz(short);
ushort __CVAttrs convert_ushort_sat_rtz(ushort);
ushort __CVAttrs convert_ushort_sat_rtz(int);
ushort __CVAttrs convert_ushort_sat_rtz(uint);
ushort __CVAttrs convert_ushort_sat_rtz(long);
ushort __CVAttrs convert_ushort_sat_rtz(ulong);
ushort __CVAttrs convert_ushort_sat_rtz(float);
ushort __CVAttrs convert_ushort_sat_rtz(double);
ushort __CVAttrs convert_ushort_sat_rtp(char);
ushort __CVAttrs convert_ushort_sat_rtp(uchar);
ushort __CVAttrs convert_ushort_sat_rtp(short);
ushort __CVAttrs convert_ushort_sat_rtp(ushort);
ushort __CVAttrs convert_ushort_sat_rtp(int);
ushort __CVAttrs convert_ushort_sat_rtp(uint);
ushort __CVAttrs convert_ushort_sat_rtp(long);
ushort __CVAttrs convert_ushort_sat_rtp(ulong);
ushort __CVAttrs convert_ushort_sat_rtp(float);
ushort __CVAttrs convert_ushort_sat_rtp(double);
ushort __CVAttrs convert_ushort_sat_rtn(char);
ushort __CVAttrs convert_ushort_sat_rtn(uchar);
ushort __CVAttrs convert_ushort_sat_rtn(short);
ushort __CVAttrs convert_ushort_sat_rtn(ushort);
ushort __CVAttrs convert_ushort_sat_rtn(int);
ushort __CVAttrs convert_ushort_sat_rtn(uint);
ushort __CVAttrs convert_ushort_sat_rtn(long);
ushort __CVAttrs convert_ushort_sat_rtn(ulong);
ushort __CVAttrs convert_ushort_sat_rtn(float);
ushort __CVAttrs convert_ushort_sat_rtn(double);
ushort2 __CVAttrs convert_ushort2(char2);
ushort2 __CVAttrs convert_ushort2(uchar2);
ushort2 __CVAttrs convert_ushort2(short2);
ushort2 __CVAttrs convert_ushort2(ushort2);
ushort2 __CVAttrs convert_ushort2(int2);
ushort2 __CVAttrs convert_ushort2(uint2);
ushort2 __CVAttrs convert_ushort2(long2);
ushort2 __CVAttrs convert_ushort2(ulong2);
ushort2 __CVAttrs convert_ushort2(float2);
ushort2 __CVAttrs convert_ushort2(double2);
ushort2 __CVAttrs convert_ushort2_rte(char2);
ushort2 __CVAttrs convert_ushort2_rte(uchar2);
ushort2 __CVAttrs convert_ushort2_rte(short2);
ushort2 __CVAttrs convert_ushort2_rte(ushort2);
ushort2 __CVAttrs convert_ushort2_rte(int2);
ushort2 __CVAttrs convert_ushort2_rte(uint2);
ushort2 __CVAttrs convert_ushort2_rte(long2);
ushort2 __CVAttrs convert_ushort2_rte(ulong2);
ushort2 __CVAttrs convert_ushort2_rte(float2);
ushort2 __CVAttrs convert_ushort2_rte(double2);
ushort2 __CVAttrs convert_ushort2_rtz(char2);
ushort2 __CVAttrs convert_ushort2_rtz(uchar2);
ushort2 __CVAttrs convert_ushort2_rtz(short2);
ushort2 __CVAttrs convert_ushort2_rtz(ushort2);
ushort2 __CVAttrs convert_ushort2_rtz(int2);
ushort2 __CVAttrs convert_ushort2_rtz(uint2);
ushort2 __CVAttrs convert_ushort2_rtz(long2);
ushort2 __CVAttrs convert_ushort2_rtz(ulong2);
ushort2 __CVAttrs convert_ushort2_rtz(float2);
ushort2 __CVAttrs convert_ushort2_rtz(double2);
ushort2 __CVAttrs convert_ushort2_rtp(char2);
ushort2 __CVAttrs convert_ushort2_rtp(uchar2);
ushort2 __CVAttrs convert_ushort2_rtp(short2);
ushort2 __CVAttrs convert_ushort2_rtp(ushort2);
ushort2 __CVAttrs convert_ushort2_rtp(int2);
ushort2 __CVAttrs convert_ushort2_rtp(uint2);
ushort2 __CVAttrs convert_ushort2_rtp(long2);
ushort2 __CVAttrs convert_ushort2_rtp(ulong2);
ushort2 __CVAttrs convert_ushort2_rtp(float2);
ushort2 __CVAttrs convert_ushort2_rtp(double2);
ushort2 __CVAttrs convert_ushort2_rtn(char2);
ushort2 __CVAttrs convert_ushort2_rtn(uchar2);
ushort2 __CVAttrs convert_ushort2_rtn(short2);
ushort2 __CVAttrs convert_ushort2_rtn(ushort2);
ushort2 __CVAttrs convert_ushort2_rtn(int2);
ushort2 __CVAttrs convert_ushort2_rtn(uint2);
ushort2 __CVAttrs convert_ushort2_rtn(long2);
ushort2 __CVAttrs convert_ushort2_rtn(ulong2);
ushort2 __CVAttrs convert_ushort2_rtn(float2);
ushort2 __CVAttrs convert_ushort2_rtn(double2);
ushort2 __CVAttrs convert_ushort2_sat(char2);
ushort2 __CVAttrs convert_ushort2_sat(uchar2);
ushort2 __CVAttrs convert_ushort2_sat(short2);
ushort2 __CVAttrs convert_ushort2_sat(ushort2);
ushort2 __CVAttrs convert_ushort2_sat(int2);
ushort2 __CVAttrs convert_ushort2_sat(uint2);
ushort2 __CVAttrs convert_ushort2_sat(long2);
ushort2 __CVAttrs convert_ushort2_sat(ulong2);
ushort2 __CVAttrs convert_ushort2_sat(float2);
ushort2 __CVAttrs convert_ushort2_sat(double2);
ushort2 __CVAttrs convert_ushort2_sat_rte(char2);
ushort2 __CVAttrs convert_ushort2_sat_rte(uchar2);
ushort2 __CVAttrs convert_ushort2_sat_rte(short2);
ushort2 __CVAttrs convert_ushort2_sat_rte(ushort2);
ushort2 __CVAttrs convert_ushort2_sat_rte(int2);
ushort2 __CVAttrs convert_ushort2_sat_rte(uint2);
ushort2 __CVAttrs convert_ushort2_sat_rte(long2);
ushort2 __CVAttrs convert_ushort2_sat_rte(ulong2);
ushort2 __CVAttrs convert_ushort2_sat_rte(float2);
ushort2 __CVAttrs convert_ushort2_sat_rte(double2);
ushort2 __CVAttrs convert_ushort2_sat_rtz(char2);
ushort2 __CVAttrs convert_ushort2_sat_rtz(uchar2);
ushort2 __CVAttrs convert_ushort2_sat_rtz(short2);
ushort2 __CVAttrs convert_ushort2_sat_rtz(ushort2);
ushort2 __CVAttrs convert_ushort2_sat_rtz(int2);
ushort2 __CVAttrs convert_ushort2_sat_rtz(uint2);
ushort2 __CVAttrs convert_ushort2_sat_rtz(long2);
ushort2 __CVAttrs convert_ushort2_sat_rtz(ulong2);
ushort2 __CVAttrs convert_ushort2_sat_rtz(float2);
ushort2 __CVAttrs convert_ushort2_sat_rtz(double2);
ushort2 __CVAttrs convert_ushort2_sat_rtp(char2);
ushort2 __CVAttrs convert_ushort2_sat_rtp(uchar2);
ushort2 __CVAttrs convert_ushort2_sat_rtp(short2);
ushort2 __CVAttrs convert_ushort2_sat_rtp(ushort2);
ushort2 __CVAttrs convert_ushort2_sat_rtp(int2);
ushort2 __CVAttrs convert_ushort2_sat_rtp(uint2);
ushort2 __CVAttrs convert_ushort2_sat_rtp(long2);
ushort2 __CVAttrs convert_ushort2_sat_rtp(ulong2);
ushort2 __CVAttrs convert_ushort2_sat_rtp(float2);
ushort2 __CVAttrs convert_ushort2_sat_rtp(double2);
ushort2 __CVAttrs convert_ushort2_sat_rtn(char2);
ushort2 __CVAttrs convert_ushort2_sat_rtn(uchar2);
ushort2 __CVAttrs convert_ushort2_sat_rtn(short2);
ushort2 __CVAttrs convert_ushort2_sat_rtn(ushort2);
ushort2 __CVAttrs convert_ushort2_sat_rtn(int2);
ushort2 __CVAttrs convert_ushort2_sat_rtn(uint2);
ushort2 __CVAttrs convert_ushort2_sat_rtn(long2);
ushort2 __CVAttrs convert_ushort2_sat_rtn(ulong2);
ushort2 __CVAttrs convert_ushort2_sat_rtn(float2);
ushort2 __CVAttrs convert_ushort2_sat_rtn(double2);
#if __OPENCL_C_VERSION__ >= 110
ushort3 __CVAttrs convert_ushort3(char3);
ushort3 __CVAttrs convert_ushort3(uchar3);
ushort3 __CVAttrs convert_ushort3(short3);
ushort3 __CVAttrs convert_ushort3(ushort3);
ushort3 __CVAttrs convert_ushort3(int3);
ushort3 __CVAttrs convert_ushort3(uint3);
ushort3 __CVAttrs convert_ushort3(long3);
ushort3 __CVAttrs convert_ushort3(ulong3);
ushort3 __CVAttrs convert_ushort3(float3);
ushort3 __CVAttrs convert_ushort3(double3);
ushort3 __CVAttrs convert_ushort3_rte(char3);
ushort3 __CVAttrs convert_ushort3_rte(uchar3);
ushort3 __CVAttrs convert_ushort3_rte(short3);
ushort3 __CVAttrs convert_ushort3_rte(ushort3);
ushort3 __CVAttrs convert_ushort3_rte(int3);
ushort3 __CVAttrs convert_ushort3_rte(uint3);
ushort3 __CVAttrs convert_ushort3_rte(long3);
ushort3 __CVAttrs convert_ushort3_rte(ulong3);
ushort3 __CVAttrs convert_ushort3_rte(float3);
ushort3 __CVAttrs convert_ushort3_rte(double3);
ushort3 __CVAttrs convert_ushort3_rtz(char3);
ushort3 __CVAttrs convert_ushort3_rtz(uchar3);
ushort3 __CVAttrs convert_ushort3_rtz(short3);
ushort3 __CVAttrs convert_ushort3_rtz(ushort3);
ushort3 __CVAttrs convert_ushort3_rtz(int3);
ushort3 __CVAttrs convert_ushort3_rtz(uint3);
ushort3 __CVAttrs convert_ushort3_rtz(long3);
ushort3 __CVAttrs convert_ushort3_rtz(ulong3);
ushort3 __CVAttrs convert_ushort3_rtz(float3);
ushort3 __CVAttrs convert_ushort3_rtz(double3);
ushort3 __CVAttrs convert_ushort3_rtp(char3);
ushort3 __CVAttrs convert_ushort3_rtp(uchar3);
ushort3 __CVAttrs convert_ushort3_rtp(short3);
ushort3 __CVAttrs convert_ushort3_rtp(ushort3);
ushort3 __CVAttrs convert_ushort3_rtp(int3);
ushort3 __CVAttrs convert_ushort3_rtp(uint3);
ushort3 __CVAttrs convert_ushort3_rtp(long3);
ushort3 __CVAttrs convert_ushort3_rtp(ulong3);
ushort3 __CVAttrs convert_ushort3_rtp(float3);
ushort3 __CVAttrs convert_ushort3_rtp(double3);
ushort3 __CVAttrs convert_ushort3_rtn(char3);
ushort3 __CVAttrs convert_ushort3_rtn(uchar3);
ushort3 __CVAttrs convert_ushort3_rtn(short3);
ushort3 __CVAttrs convert_ushort3_rtn(ushort3);
ushort3 __CVAttrs convert_ushort3_rtn(int3);
ushort3 __CVAttrs convert_ushort3_rtn(uint3);
ushort3 __CVAttrs convert_ushort3_rtn(long3);
ushort3 __CVAttrs convert_ushort3_rtn(ulong3);
ushort3 __CVAttrs convert_ushort3_rtn(float3);
ushort3 __CVAttrs convert_ushort3_rtn(double3);
ushort3 __CVAttrs convert_ushort3_sat(char3);
ushort3 __CVAttrs convert_ushort3_sat(uchar3);
ushort3 __CVAttrs convert_ushort3_sat(short3);
ushort3 __CVAttrs convert_ushort3_sat(ushort3);
ushort3 __CVAttrs convert_ushort3_sat(int3);
ushort3 __CVAttrs convert_ushort3_sat(uint3);
ushort3 __CVAttrs convert_ushort3_sat(long3);
ushort3 __CVAttrs convert_ushort3_sat(ulong3);
ushort3 __CVAttrs convert_ushort3_sat(float3);
ushort3 __CVAttrs convert_ushort3_sat(double3);
ushort3 __CVAttrs convert_ushort3_sat_rte(char3);
ushort3 __CVAttrs convert_ushort3_sat_rte(uchar3);
ushort3 __CVAttrs convert_ushort3_sat_rte(short3);
ushort3 __CVAttrs convert_ushort3_sat_rte(ushort3);
ushort3 __CVAttrs convert_ushort3_sat_rte(int3);
ushort3 __CVAttrs convert_ushort3_sat_rte(uint3);
ushort3 __CVAttrs convert_ushort3_sat_rte(long3);
ushort3 __CVAttrs convert_ushort3_sat_rte(ulong3);
ushort3 __CVAttrs convert_ushort3_sat_rte(float3);
ushort3 __CVAttrs convert_ushort3_sat_rte(double3);
ushort3 __CVAttrs convert_ushort3_sat_rtz(char3);
ushort3 __CVAttrs convert_ushort3_sat_rtz(uchar3);
ushort3 __CVAttrs convert_ushort3_sat_rtz(short3);
ushort3 __CVAttrs convert_ushort3_sat_rtz(ushort3);
ushort3 __CVAttrs convert_ushort3_sat_rtz(int3);
ushort3 __CVAttrs convert_ushort3_sat_rtz(uint3);
ushort3 __CVAttrs convert_ushort3_sat_rtz(long3);
ushort3 __CVAttrs convert_ushort3_sat_rtz(ulong3);
ushort3 __CVAttrs convert_ushort3_sat_rtz(float3);
ushort3 __CVAttrs convert_ushort3_sat_rtz(double3);
ushort3 __CVAttrs convert_ushort3_sat_rtp(char3);
ushort3 __CVAttrs convert_ushort3_sat_rtp(uchar3);
ushort3 __CVAttrs convert_ushort3_sat_rtp(short3);
ushort3 __CVAttrs convert_ushort3_sat_rtp(ushort3);
ushort3 __CVAttrs convert_ushort3_sat_rtp(int3);
ushort3 __CVAttrs convert_ushort3_sat_rtp(uint3);
ushort3 __CVAttrs convert_ushort3_sat_rtp(long3);
ushort3 __CVAttrs convert_ushort3_sat_rtp(ulong3);
ushort3 __CVAttrs convert_ushort3_sat_rtp(float3);
ushort3 __CVAttrs convert_ushort3_sat_rtp(double3);
ushort3 __CVAttrs convert_ushort3_sat_rtn(char3);
ushort3 __CVAttrs convert_ushort3_sat_rtn(uchar3);
ushort3 __CVAttrs convert_ushort3_sat_rtn(short3);
ushort3 __CVAttrs convert_ushort3_sat_rtn(ushort3);
ushort3 __CVAttrs convert_ushort3_sat_rtn(int3);
ushort3 __CVAttrs convert_ushort3_sat_rtn(uint3);
ushort3 __CVAttrs convert_ushort3_sat_rtn(long3);
ushort3 __CVAttrs convert_ushort3_sat_rtn(ulong3);
ushort3 __CVAttrs convert_ushort3_sat_rtn(float3);
ushort3 __CVAttrs convert_ushort3_sat_rtn(double3);
#endif
ushort4 __CVAttrs convert_ushort4(char4);
ushort4 __CVAttrs convert_ushort4(uchar4);
ushort4 __CVAttrs convert_ushort4(short4);
ushort4 __CVAttrs convert_ushort4(ushort4);
ushort4 __CVAttrs convert_ushort4(int4);
ushort4 __CVAttrs convert_ushort4(uint4);
ushort4 __CVAttrs convert_ushort4(long4);
ushort4 __CVAttrs convert_ushort4(ulong4);
ushort4 __CVAttrs convert_ushort4(float4);
ushort4 __CVAttrs convert_ushort4(double4);
ushort4 __CVAttrs convert_ushort4_rte(char4);
ushort4 __CVAttrs convert_ushort4_rte(uchar4);
ushort4 __CVAttrs convert_ushort4_rte(short4);
ushort4 __CVAttrs convert_ushort4_rte(ushort4);
ushort4 __CVAttrs convert_ushort4_rte(int4);
ushort4 __CVAttrs convert_ushort4_rte(uint4);
ushort4 __CVAttrs convert_ushort4_rte(long4);
ushort4 __CVAttrs convert_ushort4_rte(ulong4);
ushort4 __CVAttrs convert_ushort4_rte(float4);
ushort4 __CVAttrs convert_ushort4_rte(double4);
ushort4 __CVAttrs convert_ushort4_rtz(char4);
ushort4 __CVAttrs convert_ushort4_rtz(uchar4);
ushort4 __CVAttrs convert_ushort4_rtz(short4);
ushort4 __CVAttrs convert_ushort4_rtz(ushort4);
ushort4 __CVAttrs convert_ushort4_rtz(int4);
ushort4 __CVAttrs convert_ushort4_rtz(uint4);
ushort4 __CVAttrs convert_ushort4_rtz(long4);
ushort4 __CVAttrs convert_ushort4_rtz(ulong4);
ushort4 __CVAttrs convert_ushort4_rtz(float4);
ushort4 __CVAttrs convert_ushort4_rtz(double4);
ushort4 __CVAttrs convert_ushort4_rtp(char4);
ushort4 __CVAttrs convert_ushort4_rtp(uchar4);
ushort4 __CVAttrs convert_ushort4_rtp(short4);
ushort4 __CVAttrs convert_ushort4_rtp(ushort4);
ushort4 __CVAttrs convert_ushort4_rtp(int4);
ushort4 __CVAttrs convert_ushort4_rtp(uint4);
ushort4 __CVAttrs convert_ushort4_rtp(long4);
ushort4 __CVAttrs convert_ushort4_rtp(ulong4);
ushort4 __CVAttrs convert_ushort4_rtp(float4);
ushort4 __CVAttrs convert_ushort4_rtp(double4);
ushort4 __CVAttrs convert_ushort4_rtn(char4);
ushort4 __CVAttrs convert_ushort4_rtn(uchar4);
ushort4 __CVAttrs convert_ushort4_rtn(short4);
ushort4 __CVAttrs convert_ushort4_rtn(ushort4);
ushort4 __CVAttrs convert_ushort4_rtn(int4);
ushort4 __CVAttrs convert_ushort4_rtn(uint4);
ushort4 __CVAttrs convert_ushort4_rtn(long4);
ushort4 __CVAttrs convert_ushort4_rtn(ulong4);
ushort4 __CVAttrs convert_ushort4_rtn(float4);
ushort4 __CVAttrs convert_ushort4_rtn(double4);
ushort4 __CVAttrs convert_ushort4_sat(char4);
ushort4 __CVAttrs convert_ushort4_sat(uchar4);
ushort4 __CVAttrs convert_ushort4_sat(short4);
ushort4 __CVAttrs convert_ushort4_sat(ushort4);
ushort4 __CVAttrs convert_ushort4_sat(int4);
ushort4 __CVAttrs convert_ushort4_sat(uint4);
ushort4 __CVAttrs convert_ushort4_sat(long4);
ushort4 __CVAttrs convert_ushort4_sat(ulong4);
ushort4 __CVAttrs convert_ushort4_sat(float4);
ushort4 __CVAttrs convert_ushort4_sat(double4);
ushort4 __CVAttrs convert_ushort4_sat_rte(char4);
ushort4 __CVAttrs convert_ushort4_sat_rte(uchar4);
ushort4 __CVAttrs convert_ushort4_sat_rte(short4);
ushort4 __CVAttrs convert_ushort4_sat_rte(ushort4);
ushort4 __CVAttrs convert_ushort4_sat_rte(int4);
ushort4 __CVAttrs convert_ushort4_sat_rte(uint4);
ushort4 __CVAttrs convert_ushort4_sat_rte(long4);
ushort4 __CVAttrs convert_ushort4_sat_rte(ulong4);
ushort4 __CVAttrs convert_ushort4_sat_rte(float4);
ushort4 __CVAttrs convert_ushort4_sat_rte(double4);
ushort4 __CVAttrs convert_ushort4_sat_rtz(char4);
ushort4 __CVAttrs convert_ushort4_sat_rtz(uchar4);
ushort4 __CVAttrs convert_ushort4_sat_rtz(short4);
ushort4 __CVAttrs convert_ushort4_sat_rtz(ushort4);
ushort4 __CVAttrs convert_ushort4_sat_rtz(int4);
ushort4 __CVAttrs convert_ushort4_sat_rtz(uint4);
ushort4 __CVAttrs convert_ushort4_sat_rtz(long4);
ushort4 __CVAttrs convert_ushort4_sat_rtz(ulong4);
ushort4 __CVAttrs convert_ushort4_sat_rtz(float4);
ushort4 __CVAttrs convert_ushort4_sat_rtz(double4);
ushort4 __CVAttrs convert_ushort4_sat_rtp(char4);
ushort4 __CVAttrs convert_ushort4_sat_rtp(uchar4);
ushort4 __CVAttrs convert_ushort4_sat_rtp(short4);
ushort4 __CVAttrs convert_ushort4_sat_rtp(ushort4);
ushort4 __CVAttrs convert_ushort4_sat_rtp(int4);
ushort4 __CVAttrs convert_ushort4_sat_rtp(uint4);
ushort4 __CVAttrs convert_ushort4_sat_rtp(long4);
ushort4 __CVAttrs convert_ushort4_sat_rtp(ulong4);
ushort4 __CVAttrs convert_ushort4_sat_rtp(float4);
ushort4 __CVAttrs convert_ushort4_sat_rtp(double4);
ushort4 __CVAttrs convert_ushort4_sat_rtn(char4);
ushort4 __CVAttrs convert_ushort4_sat_rtn(uchar4);
ushort4 __CVAttrs convert_ushort4_sat_rtn(short4);
ushort4 __CVAttrs convert_ushort4_sat_rtn(ushort4);
ushort4 __CVAttrs convert_ushort4_sat_rtn(int4);
ushort4 __CVAttrs convert_ushort4_sat_rtn(uint4);
ushort4 __CVAttrs convert_ushort4_sat_rtn(long4);
ushort4 __CVAttrs convert_ushort4_sat_rtn(ulong4);
ushort4 __CVAttrs convert_ushort4_sat_rtn(float4);
ushort4 __CVAttrs convert_ushort4_sat_rtn(double4);
ushort8 __CVAttrs convert_ushort8(char8);
ushort8 __CVAttrs convert_ushort8(uchar8);
ushort8 __CVAttrs convert_ushort8(short8);
ushort8 __CVAttrs convert_ushort8(ushort8);
ushort8 __CVAttrs convert_ushort8(int8);
ushort8 __CVAttrs convert_ushort8(uint8);
ushort8 __CVAttrs convert_ushort8(long8);
ushort8 __CVAttrs convert_ushort8(ulong8);
ushort8 __CVAttrs convert_ushort8(float8);
ushort8 __CVAttrs convert_ushort8(double8);
ushort8 __CVAttrs convert_ushort8_rte(char8);
ushort8 __CVAttrs convert_ushort8_rte(uchar8);
ushort8 __CVAttrs convert_ushort8_rte(short8);
ushort8 __CVAttrs convert_ushort8_rte(ushort8);
ushort8 __CVAttrs convert_ushort8_rte(int8);
ushort8 __CVAttrs convert_ushort8_rte(uint8);
ushort8 __CVAttrs convert_ushort8_rte(long8);
ushort8 __CVAttrs convert_ushort8_rte(ulong8);
ushort8 __CVAttrs convert_ushort8_rte(float8);
ushort8 __CVAttrs convert_ushort8_rte(double8);
ushort8 __CVAttrs convert_ushort8_rtz(char8);
ushort8 __CVAttrs convert_ushort8_rtz(uchar8);
ushort8 __CVAttrs convert_ushort8_rtz(short8);
ushort8 __CVAttrs convert_ushort8_rtz(ushort8);
ushort8 __CVAttrs convert_ushort8_rtz(int8);
ushort8 __CVAttrs convert_ushort8_rtz(uint8);
ushort8 __CVAttrs convert_ushort8_rtz(long8);
ushort8 __CVAttrs convert_ushort8_rtz(ulong8);
ushort8 __CVAttrs convert_ushort8_rtz(float8);
ushort8 __CVAttrs convert_ushort8_rtz(double8);
ushort8 __CVAttrs convert_ushort8_rtp(char8);
ushort8 __CVAttrs convert_ushort8_rtp(uchar8);
ushort8 __CVAttrs convert_ushort8_rtp(short8);
ushort8 __CVAttrs convert_ushort8_rtp(ushort8);
ushort8 __CVAttrs convert_ushort8_rtp(int8);
ushort8 __CVAttrs convert_ushort8_rtp(uint8);
ushort8 __CVAttrs convert_ushort8_rtp(long8);
ushort8 __CVAttrs convert_ushort8_rtp(ulong8);
ushort8 __CVAttrs convert_ushort8_rtp(float8);
ushort8 __CVAttrs convert_ushort8_rtp(double8);
ushort8 __CVAttrs convert_ushort8_rtn(char8);
ushort8 __CVAttrs convert_ushort8_rtn(uchar8);
ushort8 __CVAttrs convert_ushort8_rtn(short8);
ushort8 __CVAttrs convert_ushort8_rtn(ushort8);
ushort8 __CVAttrs convert_ushort8_rtn(int8);
ushort8 __CVAttrs convert_ushort8_rtn(uint8);
ushort8 __CVAttrs convert_ushort8_rtn(long8);
ushort8 __CVAttrs convert_ushort8_rtn(ulong8);
ushort8 __CVAttrs convert_ushort8_rtn(float8);
ushort8 __CVAttrs convert_ushort8_rtn(double8);
ushort8 __CVAttrs convert_ushort8_sat(char8);
ushort8 __CVAttrs convert_ushort8_sat(uchar8);
ushort8 __CVAttrs convert_ushort8_sat(short8);
ushort8 __CVAttrs convert_ushort8_sat(ushort8);
ushort8 __CVAttrs convert_ushort8_sat(int8);
ushort8 __CVAttrs convert_ushort8_sat(uint8);
ushort8 __CVAttrs convert_ushort8_sat(long8);
ushort8 __CVAttrs convert_ushort8_sat(ulong8);
ushort8 __CVAttrs convert_ushort8_sat(float8);
ushort8 __CVAttrs convert_ushort8_sat(double8);
ushort8 __CVAttrs convert_ushort8_sat_rte(char8);
ushort8 __CVAttrs convert_ushort8_sat_rte(uchar8);
ushort8 __CVAttrs convert_ushort8_sat_rte(short8);
ushort8 __CVAttrs convert_ushort8_sat_rte(ushort8);
ushort8 __CVAttrs convert_ushort8_sat_rte(int8);
ushort8 __CVAttrs convert_ushort8_sat_rte(uint8);
ushort8 __CVAttrs convert_ushort8_sat_rte(long8);
ushort8 __CVAttrs convert_ushort8_sat_rte(ulong8);
ushort8 __CVAttrs convert_ushort8_sat_rte(float8);
ushort8 __CVAttrs convert_ushort8_sat_rte(double8);
ushort8 __CVAttrs convert_ushort8_sat_rtz(char8);
ushort8 __CVAttrs convert_ushort8_sat_rtz(uchar8);
ushort8 __CVAttrs convert_ushort8_sat_rtz(short8);
ushort8 __CVAttrs convert_ushort8_sat_rtz(ushort8);
ushort8 __CVAttrs convert_ushort8_sat_rtz(int8);
ushort8 __CVAttrs convert_ushort8_sat_rtz(uint8);
ushort8 __CVAttrs convert_ushort8_sat_rtz(long8);
ushort8 __CVAttrs convert_ushort8_sat_rtz(ulong8);
ushort8 __CVAttrs convert_ushort8_sat_rtz(float8);
ushort8 __CVAttrs convert_ushort8_sat_rtz(double8);
ushort8 __CVAttrs convert_ushort8_sat_rtp(char8);
ushort8 __CVAttrs convert_ushort8_sat_rtp(uchar8);
ushort8 __CVAttrs convert_ushort8_sat_rtp(short8);
ushort8 __CVAttrs convert_ushort8_sat_rtp(ushort8);
ushort8 __CVAttrs convert_ushort8_sat_rtp(int8);
ushort8 __CVAttrs convert_ushort8_sat_rtp(uint8);
ushort8 __CVAttrs convert_ushort8_sat_rtp(long8);
ushort8 __CVAttrs convert_ushort8_sat_rtp(ulong8);
ushort8 __CVAttrs convert_ushort8_sat_rtp(float8);
ushort8 __CVAttrs convert_ushort8_sat_rtp(double8);
ushort8 __CVAttrs convert_ushort8_sat_rtn(char8);
ushort8 __CVAttrs convert_ushort8_sat_rtn(uchar8);
ushort8 __CVAttrs convert_ushort8_sat_rtn(short8);
ushort8 __CVAttrs convert_ushort8_sat_rtn(ushort8);
ushort8 __CVAttrs convert_ushort8_sat_rtn(int8);
ushort8 __CVAttrs convert_ushort8_sat_rtn(uint8);
ushort8 __CVAttrs convert_ushort8_sat_rtn(long8);
ushort8 __CVAttrs convert_ushort8_sat_rtn(ulong8);
ushort8 __CVAttrs convert_ushort8_sat_rtn(float8);
ushort8 __CVAttrs convert_ushort8_sat_rtn(double8);
ushort16 __CVAttrs convert_ushort16(char16);
ushort16 __CVAttrs convert_ushort16(uchar16);
ushort16 __CVAttrs convert_ushort16(short16);
ushort16 __CVAttrs convert_ushort16(ushort16);
ushort16 __CVAttrs convert_ushort16(int16);
ushort16 __CVAttrs convert_ushort16(uint16);
ushort16 __CVAttrs convert_ushort16(long16);
ushort16 __CVAttrs convert_ushort16(ulong16);
ushort16 __CVAttrs convert_ushort16(float16);
ushort16 __CVAttrs convert_ushort16(double16);
ushort16 __CVAttrs convert_ushort16_rte(char16);
ushort16 __CVAttrs convert_ushort16_rte(uchar16);
ushort16 __CVAttrs convert_ushort16_rte(short16);
ushort16 __CVAttrs convert_ushort16_rte(ushort16);
ushort16 __CVAttrs convert_ushort16_rte(int16);
ushort16 __CVAttrs convert_ushort16_rte(uint16);
ushort16 __CVAttrs convert_ushort16_rte(long16);
ushort16 __CVAttrs convert_ushort16_rte(ulong16);
ushort16 __CVAttrs convert_ushort16_rte(float16);
ushort16 __CVAttrs convert_ushort16_rte(double16);
ushort16 __CVAttrs convert_ushort16_rtz(char16);
ushort16 __CVAttrs convert_ushort16_rtz(uchar16);
ushort16 __CVAttrs convert_ushort16_rtz(short16);
ushort16 __CVAttrs convert_ushort16_rtz(ushort16);
ushort16 __CVAttrs convert_ushort16_rtz(int16);
ushort16 __CVAttrs convert_ushort16_rtz(uint16);
ushort16 __CVAttrs convert_ushort16_rtz(long16);
ushort16 __CVAttrs convert_ushort16_rtz(ulong16);
ushort16 __CVAttrs convert_ushort16_rtz(float16);
ushort16 __CVAttrs convert_ushort16_rtz(double16);
ushort16 __CVAttrs convert_ushort16_rtp(char16);
ushort16 __CVAttrs convert_ushort16_rtp(uchar16);
ushort16 __CVAttrs convert_ushort16_rtp(short16);
ushort16 __CVAttrs convert_ushort16_rtp(ushort16);
ushort16 __CVAttrs convert_ushort16_rtp(int16);
ushort16 __CVAttrs convert_ushort16_rtp(uint16);
ushort16 __CVAttrs convert_ushort16_rtp(long16);
ushort16 __CVAttrs convert_ushort16_rtp(ulong16);
ushort16 __CVAttrs convert_ushort16_rtp(float16);
ushort16 __CVAttrs convert_ushort16_rtp(double16);
ushort16 __CVAttrs convert_ushort16_rtn(char16);
ushort16 __CVAttrs convert_ushort16_rtn(uchar16);
ushort16 __CVAttrs convert_ushort16_rtn(short16);
ushort16 __CVAttrs convert_ushort16_rtn(ushort16);
ushort16 __CVAttrs convert_ushort16_rtn(int16);
ushort16 __CVAttrs convert_ushort16_rtn(uint16);
ushort16 __CVAttrs convert_ushort16_rtn(long16);
ushort16 __CVAttrs convert_ushort16_rtn(ulong16);
ushort16 __CVAttrs convert_ushort16_rtn(float16);
ushort16 __CVAttrs convert_ushort16_rtn(double16);
ushort16 __CVAttrs convert_ushort16_sat(char16);
ushort16 __CVAttrs convert_ushort16_sat(uchar16);
ushort16 __CVAttrs convert_ushort16_sat(short16);
ushort16 __CVAttrs convert_ushort16_sat(ushort16);
ushort16 __CVAttrs convert_ushort16_sat(int16);
ushort16 __CVAttrs convert_ushort16_sat(uint16);
ushort16 __CVAttrs convert_ushort16_sat(long16);
ushort16 __CVAttrs convert_ushort16_sat(ulong16);
ushort16 __CVAttrs convert_ushort16_sat(float16);
ushort16 __CVAttrs convert_ushort16_sat(double16);
ushort16 __CVAttrs convert_ushort16_sat_rte(char16);
ushort16 __CVAttrs convert_ushort16_sat_rte(uchar16);
ushort16 __CVAttrs convert_ushort16_sat_rte(short16);
ushort16 __CVAttrs convert_ushort16_sat_rte(ushort16);
ushort16 __CVAttrs convert_ushort16_sat_rte(int16);
ushort16 __CVAttrs convert_ushort16_sat_rte(uint16);
ushort16 __CVAttrs convert_ushort16_sat_rte(long16);
ushort16 __CVAttrs convert_ushort16_sat_rte(ulong16);
ushort16 __CVAttrs convert_ushort16_sat_rte(float16);
ushort16 __CVAttrs convert_ushort16_sat_rte(double16);
ushort16 __CVAttrs convert_ushort16_sat_rtz(char16);
ushort16 __CVAttrs convert_ushort16_sat_rtz(uchar16);
ushort16 __CVAttrs convert_ushort16_sat_rtz(short16);
ushort16 __CVAttrs convert_ushort16_sat_rtz(ushort16);
ushort16 __CVAttrs convert_ushort16_sat_rtz(int16);
ushort16 __CVAttrs convert_ushort16_sat_rtz(uint16);
ushort16 __CVAttrs convert_ushort16_sat_rtz(long16);
ushort16 __CVAttrs convert_ushort16_sat_rtz(ulong16);
ushort16 __CVAttrs convert_ushort16_sat_rtz(float16);
ushort16 __CVAttrs convert_ushort16_sat_rtz(double16);
ushort16 __CVAttrs convert_ushort16_sat_rtp(char16);
ushort16 __CVAttrs convert_ushort16_sat_rtp(uchar16);
ushort16 __CVAttrs convert_ushort16_sat_rtp(short16);
ushort16 __CVAttrs convert_ushort16_sat_rtp(ushort16);
ushort16 __CVAttrs convert_ushort16_sat_rtp(int16);
ushort16 __CVAttrs convert_ushort16_sat_rtp(uint16);
ushort16 __CVAttrs convert_ushort16_sat_rtp(long16);
ushort16 __CVAttrs convert_ushort16_sat_rtp(ulong16);
ushort16 __CVAttrs convert_ushort16_sat_rtp(float16);
ushort16 __CVAttrs convert_ushort16_sat_rtp(double16);
ushort16 __CVAttrs convert_ushort16_sat_rtn(char16);
ushort16 __CVAttrs convert_ushort16_sat_rtn(uchar16);
ushort16 __CVAttrs convert_ushort16_sat_rtn(short16);
ushort16 __CVAttrs convert_ushort16_sat_rtn(ushort16);
ushort16 __CVAttrs convert_ushort16_sat_rtn(int16);
ushort16 __CVAttrs convert_ushort16_sat_rtn(uint16);
ushort16 __CVAttrs convert_ushort16_sat_rtn(long16);
ushort16 __CVAttrs convert_ushort16_sat_rtn(ulong16);
ushort16 __CVAttrs convert_ushort16_sat_rtn(float16);
ushort16 __CVAttrs convert_ushort16_sat_rtn(double16);
int __CVAttrs convert_int(char);
int __CVAttrs convert_int(uchar);
int __CVAttrs convert_int(short);
int __CVAttrs convert_int(ushort);
int __CVAttrs convert_int(int);
int __CVAttrs convert_int(uint);
int __CVAttrs convert_int(long);
int __CVAttrs convert_int(ulong);
int __CVAttrs convert_int(float);
int __CVAttrs convert_int(double);
int __CVAttrs convert_int_rte(char);
int __CVAttrs convert_int_rte(uchar);
int __CVAttrs convert_int_rte(short);
int __CVAttrs convert_int_rte(ushort);
int __CVAttrs convert_int_rte(int);
int __CVAttrs convert_int_rte(uint);
int __CVAttrs convert_int_rte(long);
int __CVAttrs convert_int_rte(ulong);
int __CVAttrs convert_int_rte(float);
int __CVAttrs convert_int_rte(double);
int __CVAttrs convert_int_rtz(char);
int __CVAttrs convert_int_rtz(uchar);
int __CVAttrs convert_int_rtz(short);
int __CVAttrs convert_int_rtz(ushort);
int __CVAttrs convert_int_rtz(int);
int __CVAttrs convert_int_rtz(uint);
int __CVAttrs convert_int_rtz(long);
int __CVAttrs convert_int_rtz(ulong);
int __CVAttrs convert_int_rtz(float);
int __CVAttrs convert_int_rtz(double);
int __CVAttrs convert_int_rtp(char);
int __CVAttrs convert_int_rtp(uchar);
int __CVAttrs convert_int_rtp(short);
int __CVAttrs convert_int_rtp(ushort);
int __CVAttrs convert_int_rtp(int);
int __CVAttrs convert_int_rtp(uint);
int __CVAttrs convert_int_rtp(long);
int __CVAttrs convert_int_rtp(ulong);
int __CVAttrs convert_int_rtp(float);
int __CVAttrs convert_int_rtp(double);
int __CVAttrs convert_int_rtn(char);
int __CVAttrs convert_int_rtn(uchar);
int __CVAttrs convert_int_rtn(short);
int __CVAttrs convert_int_rtn(ushort);
int __CVAttrs convert_int_rtn(int);
int __CVAttrs convert_int_rtn(uint);
int __CVAttrs convert_int_rtn(long);
int __CVAttrs convert_int_rtn(ulong);
int __CVAttrs convert_int_rtn(float);
int __CVAttrs convert_int_rtn(double);
int __CVAttrs convert_int_sat(char);
int __CVAttrs convert_int_sat(uchar);
int __CVAttrs convert_int_sat(short);
int __CVAttrs convert_int_sat(ushort);
int __CVAttrs convert_int_sat(int);
int __CVAttrs convert_int_sat(uint);
int __CVAttrs convert_int_sat(long);
int __CVAttrs convert_int_sat(ulong);
int __CVAttrs convert_int_sat(float);
int __CVAttrs convert_int_sat(double);
int __CVAttrs convert_int_sat_rte(char);
int __CVAttrs convert_int_sat_rte(uchar);
int __CVAttrs convert_int_sat_rte(short);
int __CVAttrs convert_int_sat_rte(ushort);
int __CVAttrs convert_int_sat_rte(int);
int __CVAttrs convert_int_sat_rte(uint);
int __CVAttrs convert_int_sat_rte(long);
int __CVAttrs convert_int_sat_rte(ulong);
int __CVAttrs convert_int_sat_rte(float);
int __CVAttrs convert_int_sat_rte(double);
int __CVAttrs convert_int_sat_rtz(char);
int __CVAttrs convert_int_sat_rtz(uchar);
int __CVAttrs convert_int_sat_rtz(short);
int __CVAttrs convert_int_sat_rtz(ushort);
int __CVAttrs convert_int_sat_rtz(int);
int __CVAttrs convert_int_sat_rtz(uint);
int __CVAttrs convert_int_sat_rtz(long);
int __CVAttrs convert_int_sat_rtz(ulong);
int __CVAttrs convert_int_sat_rtz(float);
int __CVAttrs convert_int_sat_rtz(double);
int __CVAttrs convert_int_sat_rtp(char);
int __CVAttrs convert_int_sat_rtp(uchar);
int __CVAttrs convert_int_sat_rtp(short);
int __CVAttrs convert_int_sat_rtp(ushort);
int __CVAttrs convert_int_sat_rtp(int);
int __CVAttrs convert_int_sat_rtp(uint);
int __CVAttrs convert_int_sat_rtp(long);
int __CVAttrs convert_int_sat_rtp(ulong);
int __CVAttrs convert_int_sat_rtp(float);
int __CVAttrs convert_int_sat_rtp(double);
int __CVAttrs convert_int_sat_rtn(char);
int __CVAttrs convert_int_sat_rtn(uchar);
int __CVAttrs convert_int_sat_rtn(short);
int __CVAttrs convert_int_sat_rtn(ushort);
int __CVAttrs convert_int_sat_rtn(int);
int __CVAttrs convert_int_sat_rtn(uint);
int __CVAttrs convert_int_sat_rtn(long);
int __CVAttrs convert_int_sat_rtn(ulong);
int __CVAttrs convert_int_sat_rtn(float);
int __CVAttrs convert_int_sat_rtn(double);
int2 __CVAttrs convert_int2(char2);
int2 __CVAttrs convert_int2(uchar2);
int2 __CVAttrs convert_int2(short2);
int2 __CVAttrs convert_int2(ushort2);
int2 __CVAttrs convert_int2(int2);
int2 __CVAttrs convert_int2(uint2);
int2 __CVAttrs convert_int2(long2);
int2 __CVAttrs convert_int2(ulong2);
int2 __CVAttrs convert_int2(float2);
int2 __CVAttrs convert_int2(double2);
int2 __CVAttrs convert_int2_rte(char2);
int2 __CVAttrs convert_int2_rte(uchar2);
int2 __CVAttrs convert_int2_rte(short2);
int2 __CVAttrs convert_int2_rte(ushort2);
int2 __CVAttrs convert_int2_rte(int2);
int2 __CVAttrs convert_int2_rte(uint2);
int2 __CVAttrs convert_int2_rte(long2);
int2 __CVAttrs convert_int2_rte(ulong2);
int2 __CVAttrs convert_int2_rte(float2);
int2 __CVAttrs convert_int2_rte(double2);
int2 __CVAttrs convert_int2_rtz(char2);
int2 __CVAttrs convert_int2_rtz(uchar2);
int2 __CVAttrs convert_int2_rtz(short2);
int2 __CVAttrs convert_int2_rtz(ushort2);
int2 __CVAttrs convert_int2_rtz(int2);
int2 __CVAttrs convert_int2_rtz(uint2);
int2 __CVAttrs convert_int2_rtz(long2);
int2 __CVAttrs convert_int2_rtz(ulong2);
int2 __CVAttrs convert_int2_rtz(float2);
int2 __CVAttrs convert_int2_rtz(double2);
int2 __CVAttrs convert_int2_rtp(char2);
int2 __CVAttrs convert_int2_rtp(uchar2);
int2 __CVAttrs convert_int2_rtp(short2);
int2 __CVAttrs convert_int2_rtp(ushort2);
int2 __CVAttrs convert_int2_rtp(int2);
int2 __CVAttrs convert_int2_rtp(uint2);
int2 __CVAttrs convert_int2_rtp(long2);
int2 __CVAttrs convert_int2_rtp(ulong2);
int2 __CVAttrs convert_int2_rtp(float2);
int2 __CVAttrs convert_int2_rtp(double2);
int2 __CVAttrs convert_int2_rtn(char2);
int2 __CVAttrs convert_int2_rtn(uchar2);
int2 __CVAttrs convert_int2_rtn(short2);
int2 __CVAttrs convert_int2_rtn(ushort2);
int2 __CVAttrs convert_int2_rtn(int2);
int2 __CVAttrs convert_int2_rtn(uint2);
int2 __CVAttrs convert_int2_rtn(long2);
int2 __CVAttrs convert_int2_rtn(ulong2);
int2 __CVAttrs convert_int2_rtn(float2);
int2 __CVAttrs convert_int2_rtn(double2);
int2 __CVAttrs convert_int2_sat(char2);
int2 __CVAttrs convert_int2_sat(uchar2);
int2 __CVAttrs convert_int2_sat(short2);
int2 __CVAttrs convert_int2_sat(ushort2);
int2 __CVAttrs convert_int2_sat(int2);
int2 __CVAttrs convert_int2_sat(uint2);
int2 __CVAttrs convert_int2_sat(long2);
int2 __CVAttrs convert_int2_sat(ulong2);
int2 __CVAttrs convert_int2_sat(float2);
int2 __CVAttrs convert_int2_sat(double2);
int2 __CVAttrs convert_int2_sat_rte(char2);
int2 __CVAttrs convert_int2_sat_rte(uchar2);
int2 __CVAttrs convert_int2_sat_rte(short2);
int2 __CVAttrs convert_int2_sat_rte(ushort2);
int2 __CVAttrs convert_int2_sat_rte(int2);
int2 __CVAttrs convert_int2_sat_rte(uint2);
int2 __CVAttrs convert_int2_sat_rte(long2);
int2 __CVAttrs convert_int2_sat_rte(ulong2);
int2 __CVAttrs convert_int2_sat_rte(float2);
int2 __CVAttrs convert_int2_sat_rte(double2);
int2 __CVAttrs convert_int2_sat_rtz(char2);
int2 __CVAttrs convert_int2_sat_rtz(uchar2);
int2 __CVAttrs convert_int2_sat_rtz(short2);
int2 __CVAttrs convert_int2_sat_rtz(ushort2);
int2 __CVAttrs convert_int2_sat_rtz(int2);
int2 __CVAttrs convert_int2_sat_rtz(uint2);
int2 __CVAttrs convert_int2_sat_rtz(long2);
int2 __CVAttrs convert_int2_sat_rtz(ulong2);
int2 __CVAttrs convert_int2_sat_rtz(float2);
int2 __CVAttrs convert_int2_sat_rtz(double2);
int2 __CVAttrs convert_int2_sat_rtp(char2);
int2 __CVAttrs convert_int2_sat_rtp(uchar2);
int2 __CVAttrs convert_int2_sat_rtp(short2);
int2 __CVAttrs convert_int2_sat_rtp(ushort2);
int2 __CVAttrs convert_int2_sat_rtp(int2);
int2 __CVAttrs convert_int2_sat_rtp(uint2);
int2 __CVAttrs convert_int2_sat_rtp(long2);
int2 __CVAttrs convert_int2_sat_rtp(ulong2);
int2 __CVAttrs convert_int2_sat_rtp(float2);
int2 __CVAttrs convert_int2_sat_rtp(double2);
int2 __CVAttrs convert_int2_sat_rtn(char2);
int2 __CVAttrs convert_int2_sat_rtn(uchar2);
int2 __CVAttrs convert_int2_sat_rtn(short2);
int2 __CVAttrs convert_int2_sat_rtn(ushort2);
int2 __CVAttrs convert_int2_sat_rtn(int2);
int2 __CVAttrs convert_int2_sat_rtn(uint2);
int2 __CVAttrs convert_int2_sat_rtn(long2);
int2 __CVAttrs convert_int2_sat_rtn(ulong2);
int2 __CVAttrs convert_int2_sat_rtn(float2);
int2 __CVAttrs convert_int2_sat_rtn(double2);
#if __OPENCL_C_VERSION__ >= 110
int3 __CVAttrs convert_int3(char3);
int3 __CVAttrs convert_int3(uchar3);
int3 __CVAttrs convert_int3(short3);
int3 __CVAttrs convert_int3(ushort3);
int3 __CVAttrs convert_int3(int3);
int3 __CVAttrs convert_int3(uint3);
int3 __CVAttrs convert_int3(long3);
int3 __CVAttrs convert_int3(ulong3);
int3 __CVAttrs convert_int3(float3);
int3 __CVAttrs convert_int3(double3);
int3 __CVAttrs convert_int3_rte(char3);
int3 __CVAttrs convert_int3_rte(uchar3);
int3 __CVAttrs convert_int3_rte(short3);
int3 __CVAttrs convert_int3_rte(ushort3);
int3 __CVAttrs convert_int3_rte(int3);
int3 __CVAttrs convert_int3_rte(uint3);
int3 __CVAttrs convert_int3_rte(long3);
int3 __CVAttrs convert_int3_rte(ulong3);
int3 __CVAttrs convert_int3_rte(float3);
int3 __CVAttrs convert_int3_rte(double3);
int3 __CVAttrs convert_int3_rtz(char3);
int3 __CVAttrs convert_int3_rtz(uchar3);
int3 __CVAttrs convert_int3_rtz(short3);
int3 __CVAttrs convert_int3_rtz(ushort3);
int3 __CVAttrs convert_int3_rtz(int3);
int3 __CVAttrs convert_int3_rtz(uint3);
int3 __CVAttrs convert_int3_rtz(long3);
int3 __CVAttrs convert_int3_rtz(ulong3);
int3 __CVAttrs convert_int3_rtz(float3);
int3 __CVAttrs convert_int3_rtz(double3);
int3 __CVAttrs convert_int3_rtp(char3);
int3 __CVAttrs convert_int3_rtp(uchar3);
int3 __CVAttrs convert_int3_rtp(short3);
int3 __CVAttrs convert_int3_rtp(ushort3);
int3 __CVAttrs convert_int3_rtp(int3);
int3 __CVAttrs convert_int3_rtp(uint3);
int3 __CVAttrs convert_int3_rtp(long3);
int3 __CVAttrs convert_int3_rtp(ulong3);
int3 __CVAttrs convert_int3_rtp(float3);
int3 __CVAttrs convert_int3_rtp(double3);
int3 __CVAttrs convert_int3_rtn(char3);
int3 __CVAttrs convert_int3_rtn(uchar3);
int3 __CVAttrs convert_int3_rtn(short3);
int3 __CVAttrs convert_int3_rtn(ushort3);
int3 __CVAttrs convert_int3_rtn(int3);
int3 __CVAttrs convert_int3_rtn(uint3);
int3 __CVAttrs convert_int3_rtn(long3);
int3 __CVAttrs convert_int3_rtn(ulong3);
int3 __CVAttrs convert_int3_rtn(float3);
int3 __CVAttrs convert_int3_rtn(double3);
int3 __CVAttrs convert_int3_sat(char3);
int3 __CVAttrs convert_int3_sat(uchar3);
int3 __CVAttrs convert_int3_sat(short3);
int3 __CVAttrs convert_int3_sat(ushort3);
int3 __CVAttrs convert_int3_sat(int3);
int3 __CVAttrs convert_int3_sat(uint3);
int3 __CVAttrs convert_int3_sat(long3);
int3 __CVAttrs convert_int3_sat(ulong3);
int3 __CVAttrs convert_int3_sat(float3);
int3 __CVAttrs convert_int3_sat(double3);
int3 __CVAttrs convert_int3_sat_rte(char3);
int3 __CVAttrs convert_int3_sat_rte(uchar3);
int3 __CVAttrs convert_int3_sat_rte(short3);
int3 __CVAttrs convert_int3_sat_rte(ushort3);
int3 __CVAttrs convert_int3_sat_rte(int3);
int3 __CVAttrs convert_int3_sat_rte(uint3);
int3 __CVAttrs convert_int3_sat_rte(long3);
int3 __CVAttrs convert_int3_sat_rte(ulong3);
int3 __CVAttrs convert_int3_sat_rte(float3);
int3 __CVAttrs convert_int3_sat_rte(double3);
int3 __CVAttrs convert_int3_sat_rtz(char3);
int3 __CVAttrs convert_int3_sat_rtz(uchar3);
int3 __CVAttrs convert_int3_sat_rtz(short3);
int3 __CVAttrs convert_int3_sat_rtz(ushort3);
int3 __CVAttrs convert_int3_sat_rtz(int3);
int3 __CVAttrs convert_int3_sat_rtz(uint3);
int3 __CVAttrs convert_int3_sat_rtz(long3);
int3 __CVAttrs convert_int3_sat_rtz(ulong3);
int3 __CVAttrs convert_int3_sat_rtz(float3);
int3 __CVAttrs convert_int3_sat_rtz(double3);
int3 __CVAttrs convert_int3_sat_rtp(char3);
int3 __CVAttrs convert_int3_sat_rtp(uchar3);
int3 __CVAttrs convert_int3_sat_rtp(short3);
int3 __CVAttrs convert_int3_sat_rtp(ushort3);
int3 __CVAttrs convert_int3_sat_rtp(int3);
int3 __CVAttrs convert_int3_sat_rtp(uint3);
int3 __CVAttrs convert_int3_sat_rtp(long3);
int3 __CVAttrs convert_int3_sat_rtp(ulong3);
int3 __CVAttrs convert_int3_sat_rtp(float3);
int3 __CVAttrs convert_int3_sat_rtp(double3);
int3 __CVAttrs convert_int3_sat_rtn(char3);
int3 __CVAttrs convert_int3_sat_rtn(uchar3);
int3 __CVAttrs convert_int3_sat_rtn(short3);
int3 __CVAttrs convert_int3_sat_rtn(ushort3);
int3 __CVAttrs convert_int3_sat_rtn(int3);
int3 __CVAttrs convert_int3_sat_rtn(uint3);
int3 __CVAttrs convert_int3_sat_rtn(long3);
int3 __CVAttrs convert_int3_sat_rtn(ulong3);
int3 __CVAttrs convert_int3_sat_rtn(float3);
int3 __CVAttrs convert_int3_sat_rtn(double3);
#endif
int4 __CVAttrs convert_int4(char4);
int4 __CVAttrs convert_int4(uchar4);
int4 __CVAttrs convert_int4(short4);
int4 __CVAttrs convert_int4(ushort4);
int4 __CVAttrs convert_int4(int4);
int4 __CVAttrs convert_int4(uint4);
int4 __CVAttrs convert_int4(long4);
int4 __CVAttrs convert_int4(ulong4);
int4 __CVAttrs convert_int4(float4);
int4 __CVAttrs convert_int4(double4);
int4 __CVAttrs convert_int4_rte(char4);
int4 __CVAttrs convert_int4_rte(uchar4);
int4 __CVAttrs convert_int4_rte(short4);
int4 __CVAttrs convert_int4_rte(ushort4);
int4 __CVAttrs convert_int4_rte(int4);
int4 __CVAttrs convert_int4_rte(uint4);
int4 __CVAttrs convert_int4_rte(long4);
int4 __CVAttrs convert_int4_rte(ulong4);
int4 __CVAttrs convert_int4_rte(float4);
int4 __CVAttrs convert_int4_rte(double4);
int4 __CVAttrs convert_int4_rtz(char4);
int4 __CVAttrs convert_int4_rtz(uchar4);
int4 __CVAttrs convert_int4_rtz(short4);
int4 __CVAttrs convert_int4_rtz(ushort4);
int4 __CVAttrs convert_int4_rtz(int4);
int4 __CVAttrs convert_int4_rtz(uint4);
int4 __CVAttrs convert_int4_rtz(long4);
int4 __CVAttrs convert_int4_rtz(ulong4);
int4 __CVAttrs convert_int4_rtz(float4);
int4 __CVAttrs convert_int4_rtz(double4);
int4 __CVAttrs convert_int4_rtp(char4);
int4 __CVAttrs convert_int4_rtp(uchar4);
int4 __CVAttrs convert_int4_rtp(short4);
int4 __CVAttrs convert_int4_rtp(ushort4);
int4 __CVAttrs convert_int4_rtp(int4);
int4 __CVAttrs convert_int4_rtp(uint4);
int4 __CVAttrs convert_int4_rtp(long4);
int4 __CVAttrs convert_int4_rtp(ulong4);
int4 __CVAttrs convert_int4_rtp(float4);
int4 __CVAttrs convert_int4_rtp(double4);
int4 __CVAttrs convert_int4_rtn(char4);
int4 __CVAttrs convert_int4_rtn(uchar4);
int4 __CVAttrs convert_int4_rtn(short4);
int4 __CVAttrs convert_int4_rtn(ushort4);
int4 __CVAttrs convert_int4_rtn(int4);
int4 __CVAttrs convert_int4_rtn(uint4);
int4 __CVAttrs convert_int4_rtn(long4);
int4 __CVAttrs convert_int4_rtn(ulong4);
int4 __CVAttrs convert_int4_rtn(float4);
int4 __CVAttrs convert_int4_rtn(double4);
int4 __CVAttrs convert_int4_sat(char4);
int4 __CVAttrs convert_int4_sat(uchar4);
int4 __CVAttrs convert_int4_sat(short4);
int4 __CVAttrs convert_int4_sat(ushort4);
int4 __CVAttrs convert_int4_sat(int4);
int4 __CVAttrs convert_int4_sat(uint4);
int4 __CVAttrs convert_int4_sat(long4);
int4 __CVAttrs convert_int4_sat(ulong4);
int4 __CVAttrs convert_int4_sat(float4);
int4 __CVAttrs convert_int4_sat(double4);
int4 __CVAttrs convert_int4_sat_rte(char4);
int4 __CVAttrs convert_int4_sat_rte(uchar4);
int4 __CVAttrs convert_int4_sat_rte(short4);
int4 __CVAttrs convert_int4_sat_rte(ushort4);
int4 __CVAttrs convert_int4_sat_rte(int4);
int4 __CVAttrs convert_int4_sat_rte(uint4);
int4 __CVAttrs convert_int4_sat_rte(long4);
int4 __CVAttrs convert_int4_sat_rte(ulong4);
int4 __CVAttrs convert_int4_sat_rte(float4);
int4 __CVAttrs convert_int4_sat_rte(double4);
int4 __CVAttrs convert_int4_sat_rtz(char4);
int4 __CVAttrs convert_int4_sat_rtz(uchar4);
int4 __CVAttrs convert_int4_sat_rtz(short4);
int4 __CVAttrs convert_int4_sat_rtz(ushort4);
int4 __CVAttrs convert_int4_sat_rtz(int4);
int4 __CVAttrs convert_int4_sat_rtz(uint4);
int4 __CVAttrs convert_int4_sat_rtz(long4);
int4 __CVAttrs convert_int4_sat_rtz(ulong4);
int4 __CVAttrs convert_int4_sat_rtz(float4);
int4 __CVAttrs convert_int4_sat_rtz(double4);
int4 __CVAttrs convert_int4_sat_rtp(char4);
int4 __CVAttrs convert_int4_sat_rtp(uchar4);
int4 __CVAttrs convert_int4_sat_rtp(short4);
int4 __CVAttrs convert_int4_sat_rtp(ushort4);
int4 __CVAttrs convert_int4_sat_rtp(int4);
int4 __CVAttrs convert_int4_sat_rtp(uint4);
int4 __CVAttrs convert_int4_sat_rtp(long4);
int4 __CVAttrs convert_int4_sat_rtp(ulong4);
int4 __CVAttrs convert_int4_sat_rtp(float4);
int4 __CVAttrs convert_int4_sat_rtp(double4);
int4 __CVAttrs convert_int4_sat_rtn(char4);
int4 __CVAttrs convert_int4_sat_rtn(uchar4);
int4 __CVAttrs convert_int4_sat_rtn(short4);
int4 __CVAttrs convert_int4_sat_rtn(ushort4);
int4 __CVAttrs convert_int4_sat_rtn(int4);
int4 __CVAttrs convert_int4_sat_rtn(uint4);
int4 __CVAttrs convert_int4_sat_rtn(long4);
int4 __CVAttrs convert_int4_sat_rtn(ulong4);
int4 __CVAttrs convert_int4_sat_rtn(float4);
int4 __CVAttrs convert_int4_sat_rtn(double4);
int8 __CVAttrs convert_int8(char8);
int8 __CVAttrs convert_int8(uchar8);
int8 __CVAttrs convert_int8(short8);
int8 __CVAttrs convert_int8(ushort8);
int8 __CVAttrs convert_int8(int8);
int8 __CVAttrs convert_int8(uint8);
int8 __CVAttrs convert_int8(long8);
int8 __CVAttrs convert_int8(ulong8);
int8 __CVAttrs convert_int8(float8);
int8 __CVAttrs convert_int8(double8);
int8 __CVAttrs convert_int8_rte(char8);
int8 __CVAttrs convert_int8_rte(uchar8);
int8 __CVAttrs convert_int8_rte(short8);
int8 __CVAttrs convert_int8_rte(ushort8);
int8 __CVAttrs convert_int8_rte(int8);
int8 __CVAttrs convert_int8_rte(uint8);
int8 __CVAttrs convert_int8_rte(long8);
int8 __CVAttrs convert_int8_rte(ulong8);
int8 __CVAttrs convert_int8_rte(float8);
int8 __CVAttrs convert_int8_rte(double8);
int8 __CVAttrs convert_int8_rtz(char8);
int8 __CVAttrs convert_int8_rtz(uchar8);
int8 __CVAttrs convert_int8_rtz(short8);
int8 __CVAttrs convert_int8_rtz(ushort8);
int8 __CVAttrs convert_int8_rtz(int8);
int8 __CVAttrs convert_int8_rtz(uint8);
int8 __CVAttrs convert_int8_rtz(long8);
int8 __CVAttrs convert_int8_rtz(ulong8);
int8 __CVAttrs convert_int8_rtz(float8);
int8 __CVAttrs convert_int8_rtz(double8);
int8 __CVAttrs convert_int8_rtp(char8);
int8 __CVAttrs convert_int8_rtp(uchar8);
int8 __CVAttrs convert_int8_rtp(short8);
int8 __CVAttrs convert_int8_rtp(ushort8);
int8 __CVAttrs convert_int8_rtp(int8);
int8 __CVAttrs convert_int8_rtp(uint8);
int8 __CVAttrs convert_int8_rtp(long8);
int8 __CVAttrs convert_int8_rtp(ulong8);
int8 __CVAttrs convert_int8_rtp(float8);
int8 __CVAttrs convert_int8_rtp(double8);
int8 __CVAttrs convert_int8_rtn(char8);
int8 __CVAttrs convert_int8_rtn(uchar8);
int8 __CVAttrs convert_int8_rtn(short8);
int8 __CVAttrs convert_int8_rtn(ushort8);
int8 __CVAttrs convert_int8_rtn(int8);
int8 __CVAttrs convert_int8_rtn(uint8);
int8 __CVAttrs convert_int8_rtn(long8);
int8 __CVAttrs convert_int8_rtn(ulong8);
int8 __CVAttrs convert_int8_rtn(float8);
int8 __CVAttrs convert_int8_rtn(double8);
int8 __CVAttrs convert_int8_sat(char8);
int8 __CVAttrs convert_int8_sat(uchar8);
int8 __CVAttrs convert_int8_sat(short8);
int8 __CVAttrs convert_int8_sat(ushort8);
int8 __CVAttrs convert_int8_sat(int8);
int8 __CVAttrs convert_int8_sat(uint8);
int8 __CVAttrs convert_int8_sat(long8);
int8 __CVAttrs convert_int8_sat(ulong8);
int8 __CVAttrs convert_int8_sat(float8);
int8 __CVAttrs convert_int8_sat(double8);
int8 __CVAttrs convert_int8_sat_rte(char8);
int8 __CVAttrs convert_int8_sat_rte(uchar8);
int8 __CVAttrs convert_int8_sat_rte(short8);
int8 __CVAttrs convert_int8_sat_rte(ushort8);
int8 __CVAttrs convert_int8_sat_rte(int8);
int8 __CVAttrs convert_int8_sat_rte(uint8);
int8 __CVAttrs convert_int8_sat_rte(long8);
int8 __CVAttrs convert_int8_sat_rte(ulong8);
int8 __CVAttrs convert_int8_sat_rte(float8);
int8 __CVAttrs convert_int8_sat_rte(double8);
int8 __CVAttrs convert_int8_sat_rtz(char8);
int8 __CVAttrs convert_int8_sat_rtz(uchar8);
int8 __CVAttrs convert_int8_sat_rtz(short8);
int8 __CVAttrs convert_int8_sat_rtz(ushort8);
int8 __CVAttrs convert_int8_sat_rtz(int8);
int8 __CVAttrs convert_int8_sat_rtz(uint8);
int8 __CVAttrs convert_int8_sat_rtz(long8);
int8 __CVAttrs convert_int8_sat_rtz(ulong8);
int8 __CVAttrs convert_int8_sat_rtz(float8);
int8 __CVAttrs convert_int8_sat_rtz(double8);
int8 __CVAttrs convert_int8_sat_rtp(char8);
int8 __CVAttrs convert_int8_sat_rtp(uchar8);
int8 __CVAttrs convert_int8_sat_rtp(short8);
int8 __CVAttrs convert_int8_sat_rtp(ushort8);
int8 __CVAttrs convert_int8_sat_rtp(int8);
int8 __CVAttrs convert_int8_sat_rtp(uint8);
int8 __CVAttrs convert_int8_sat_rtp(long8);
int8 __CVAttrs convert_int8_sat_rtp(ulong8);
int8 __CVAttrs convert_int8_sat_rtp(float8);
int8 __CVAttrs convert_int8_sat_rtp(double8);
int8 __CVAttrs convert_int8_sat_rtn(char8);
int8 __CVAttrs convert_int8_sat_rtn(uchar8);
int8 __CVAttrs convert_int8_sat_rtn(short8);
int8 __CVAttrs convert_int8_sat_rtn(ushort8);
int8 __CVAttrs convert_int8_sat_rtn(int8);
int8 __CVAttrs convert_int8_sat_rtn(uint8);
int8 __CVAttrs convert_int8_sat_rtn(long8);
int8 __CVAttrs convert_int8_sat_rtn(ulong8);
int8 __CVAttrs convert_int8_sat_rtn(float8);
int8 __CVAttrs convert_int8_sat_rtn(double8);
int16 __CVAttrs convert_int16(char16);
int16 __CVAttrs convert_int16(uchar16);
int16 __CVAttrs convert_int16(short16);
int16 __CVAttrs convert_int16(ushort16);
int16 __CVAttrs convert_int16(int16);
int16 __CVAttrs convert_int16(uint16);
int16 __CVAttrs convert_int16(long16);
int16 __CVAttrs convert_int16(ulong16);
int16 __CVAttrs convert_int16(float16);
int16 __CVAttrs convert_int16(double16);
int16 __CVAttrs convert_int16_rte(char16);
int16 __CVAttrs convert_int16_rte(uchar16);
int16 __CVAttrs convert_int16_rte(short16);
int16 __CVAttrs convert_int16_rte(ushort16);
int16 __CVAttrs convert_int16_rte(int16);
int16 __CVAttrs convert_int16_rte(uint16);
int16 __CVAttrs convert_int16_rte(long16);
int16 __CVAttrs convert_int16_rte(ulong16);
int16 __CVAttrs convert_int16_rte(float16);
int16 __CVAttrs convert_int16_rte(double16);
int16 __CVAttrs convert_int16_rtz(char16);
int16 __CVAttrs convert_int16_rtz(uchar16);
int16 __CVAttrs convert_int16_rtz(short16);
int16 __CVAttrs convert_int16_rtz(ushort16);
int16 __CVAttrs convert_int16_rtz(int16);
int16 __CVAttrs convert_int16_rtz(uint16);
int16 __CVAttrs convert_int16_rtz(long16);
int16 __CVAttrs convert_int16_rtz(ulong16);
int16 __CVAttrs convert_int16_rtz(float16);
int16 __CVAttrs convert_int16_rtz(double16);
int16 __CVAttrs convert_int16_rtp(char16);
int16 __CVAttrs convert_int16_rtp(uchar16);
int16 __CVAttrs convert_int16_rtp(short16);
int16 __CVAttrs convert_int16_rtp(ushort16);
int16 __CVAttrs convert_int16_rtp(int16);
int16 __CVAttrs convert_int16_rtp(uint16);
int16 __CVAttrs convert_int16_rtp(long16);
int16 __CVAttrs convert_int16_rtp(ulong16);
int16 __CVAttrs convert_int16_rtp(float16);
int16 __CVAttrs convert_int16_rtp(double16);
int16 __CVAttrs convert_int16_rtn(char16);
int16 __CVAttrs convert_int16_rtn(uchar16);
int16 __CVAttrs convert_int16_rtn(short16);
int16 __CVAttrs convert_int16_rtn(ushort16);
int16 __CVAttrs convert_int16_rtn(int16);
int16 __CVAttrs convert_int16_rtn(uint16);
int16 __CVAttrs convert_int16_rtn(long16);
int16 __CVAttrs convert_int16_rtn(ulong16);
int16 __CVAttrs convert_int16_rtn(float16);
int16 __CVAttrs convert_int16_rtn(double16);
int16 __CVAttrs convert_int16_sat(char16);
int16 __CVAttrs convert_int16_sat(uchar16);
int16 __CVAttrs convert_int16_sat(short16);
int16 __CVAttrs convert_int16_sat(ushort16);
int16 __CVAttrs convert_int16_sat(int16);
int16 __CVAttrs convert_int16_sat(uint16);
int16 __CVAttrs convert_int16_sat(long16);
int16 __CVAttrs convert_int16_sat(ulong16);
int16 __CVAttrs convert_int16_sat(float16);
int16 __CVAttrs convert_int16_sat(double16);
int16 __CVAttrs convert_int16_sat_rte(char16);
int16 __CVAttrs convert_int16_sat_rte(uchar16);
int16 __CVAttrs convert_int16_sat_rte(short16);
int16 __CVAttrs convert_int16_sat_rte(ushort16);
int16 __CVAttrs convert_int16_sat_rte(int16);
int16 __CVAttrs convert_int16_sat_rte(uint16);
int16 __CVAttrs convert_int16_sat_rte(long16);
int16 __CVAttrs convert_int16_sat_rte(ulong16);
int16 __CVAttrs convert_int16_sat_rte(float16);
int16 __CVAttrs convert_int16_sat_rte(double16);
int16 __CVAttrs convert_int16_sat_rtz(char16);
int16 __CVAttrs convert_int16_sat_rtz(uchar16);
int16 __CVAttrs convert_int16_sat_rtz(short16);
int16 __CVAttrs convert_int16_sat_rtz(ushort16);
int16 __CVAttrs convert_int16_sat_rtz(int16);
int16 __CVAttrs convert_int16_sat_rtz(uint16);
int16 __CVAttrs convert_int16_sat_rtz(long16);
int16 __CVAttrs convert_int16_sat_rtz(ulong16);
int16 __CVAttrs convert_int16_sat_rtz(float16);
int16 __CVAttrs convert_int16_sat_rtz(double16);
int16 __CVAttrs convert_int16_sat_rtp(char16);
int16 __CVAttrs convert_int16_sat_rtp(uchar16);
int16 __CVAttrs convert_int16_sat_rtp(short16);
int16 __CVAttrs convert_int16_sat_rtp(ushort16);
int16 __CVAttrs convert_int16_sat_rtp(int16);
int16 __CVAttrs convert_int16_sat_rtp(uint16);
int16 __CVAttrs convert_int16_sat_rtp(long16);
int16 __CVAttrs convert_int16_sat_rtp(ulong16);
int16 __CVAttrs convert_int16_sat_rtp(float16);
int16 __CVAttrs convert_int16_sat_rtp(double16);
int16 __CVAttrs convert_int16_sat_rtn(char16);
int16 __CVAttrs convert_int16_sat_rtn(uchar16);
int16 __CVAttrs convert_int16_sat_rtn(short16);
int16 __CVAttrs convert_int16_sat_rtn(ushort16);
int16 __CVAttrs convert_int16_sat_rtn(int16);
int16 __CVAttrs convert_int16_sat_rtn(uint16);
int16 __CVAttrs convert_int16_sat_rtn(long16);
int16 __CVAttrs convert_int16_sat_rtn(ulong16);
int16 __CVAttrs convert_int16_sat_rtn(float16);
int16 __CVAttrs convert_int16_sat_rtn(double16);
uint __CVAttrs convert_uint(char);
uint __CVAttrs convert_uint(uchar);
uint __CVAttrs convert_uint(short);
uint __CVAttrs convert_uint(ushort);
uint __CVAttrs convert_uint(int);
uint __CVAttrs convert_uint(uint);
uint __CVAttrs convert_uint(long);
uint __CVAttrs convert_uint(ulong);
uint __CVAttrs convert_uint(float);
uint __CVAttrs convert_uint(double);
uint __CVAttrs convert_uint_rte(char);
uint __CVAttrs convert_uint_rte(uchar);
uint __CVAttrs convert_uint_rte(short);
uint __CVAttrs convert_uint_rte(ushort);
uint __CVAttrs convert_uint_rte(int);
uint __CVAttrs convert_uint_rte(uint);
uint __CVAttrs convert_uint_rte(long);
uint __CVAttrs convert_uint_rte(ulong);
uint __CVAttrs convert_uint_rte(float);
uint __CVAttrs convert_uint_rte(double);
uint __CVAttrs convert_uint_rtz(char);
uint __CVAttrs convert_uint_rtz(uchar);
uint __CVAttrs convert_uint_rtz(short);
uint __CVAttrs convert_uint_rtz(ushort);
uint __CVAttrs convert_uint_rtz(int);
uint __CVAttrs convert_uint_rtz(uint);
uint __CVAttrs convert_uint_rtz(long);
uint __CVAttrs convert_uint_rtz(ulong);
uint __CVAttrs convert_uint_rtz(float);
uint __CVAttrs convert_uint_rtz(double);
uint __CVAttrs convert_uint_rtp(char);
uint __CVAttrs convert_uint_rtp(uchar);
uint __CVAttrs convert_uint_rtp(short);
uint __CVAttrs convert_uint_rtp(ushort);
uint __CVAttrs convert_uint_rtp(int);
uint __CVAttrs convert_uint_rtp(uint);
uint __CVAttrs convert_uint_rtp(long);
uint __CVAttrs convert_uint_rtp(ulong);
uint __CVAttrs convert_uint_rtp(float);
uint __CVAttrs convert_uint_rtp(double);
uint __CVAttrs convert_uint_rtn(char);
uint __CVAttrs convert_uint_rtn(uchar);
uint __CVAttrs convert_uint_rtn(short);
uint __CVAttrs convert_uint_rtn(ushort);
uint __CVAttrs convert_uint_rtn(int);
uint __CVAttrs convert_uint_rtn(uint);
uint __CVAttrs convert_uint_rtn(long);
uint __CVAttrs convert_uint_rtn(ulong);
uint __CVAttrs convert_uint_rtn(float);
uint __CVAttrs convert_uint_rtn(double);
uint __CVAttrs convert_uint_sat(char);
uint __CVAttrs convert_uint_sat(uchar);
uint __CVAttrs convert_uint_sat(short);
uint __CVAttrs convert_uint_sat(ushort);
uint __CVAttrs convert_uint_sat(int);
uint __CVAttrs convert_uint_sat(uint);
uint __CVAttrs convert_uint_sat(long);
uint __CVAttrs convert_uint_sat(ulong);
uint __CVAttrs convert_uint_sat(float);
uint __CVAttrs convert_uint_sat(double);
uint __CVAttrs convert_uint_sat_rte(char);
uint __CVAttrs convert_uint_sat_rte(uchar);
uint __CVAttrs convert_uint_sat_rte(short);
uint __CVAttrs convert_uint_sat_rte(ushort);
uint __CVAttrs convert_uint_sat_rte(int);
uint __CVAttrs convert_uint_sat_rte(uint);
uint __CVAttrs convert_uint_sat_rte(long);
uint __CVAttrs convert_uint_sat_rte(ulong);
uint __CVAttrs convert_uint_sat_rte(float);
uint __CVAttrs convert_uint_sat_rte(double);
uint __CVAttrs convert_uint_sat_rtz(char);
uint __CVAttrs convert_uint_sat_rtz(uchar);
uint __CVAttrs convert_uint_sat_rtz(short);
uint __CVAttrs convert_uint_sat_rtz(ushort);
uint __CVAttrs convert_uint_sat_rtz(int);
uint __CVAttrs convert_uint_sat_rtz(uint);
uint __CVAttrs convert_uint_sat_rtz(long);
uint __CVAttrs convert_uint_sat_rtz(ulong);
uint __CVAttrs convert_uint_sat_rtz(float);
uint __CVAttrs convert_uint_sat_rtz(double);
uint __CVAttrs convert_uint_sat_rtp(char);
uint __CVAttrs convert_uint_sat_rtp(uchar);
uint __CVAttrs convert_uint_sat_rtp(short);
uint __CVAttrs convert_uint_sat_rtp(ushort);
uint __CVAttrs convert_uint_sat_rtp(int);
uint __CVAttrs convert_uint_sat_rtp(uint);
uint __CVAttrs convert_uint_sat_rtp(long);
uint __CVAttrs convert_uint_sat_rtp(ulong);
uint __CVAttrs convert_uint_sat_rtp(float);
uint __CVAttrs convert_uint_sat_rtp(double);
uint __CVAttrs convert_uint_sat_rtn(char);
uint __CVAttrs convert_uint_sat_rtn(uchar);
uint __CVAttrs convert_uint_sat_rtn(short);
uint __CVAttrs convert_uint_sat_rtn(ushort);
uint __CVAttrs convert_uint_sat_rtn(int);
uint __CVAttrs convert_uint_sat_rtn(uint);
uint __CVAttrs convert_uint_sat_rtn(long);
uint __CVAttrs convert_uint_sat_rtn(ulong);
uint __CVAttrs convert_uint_sat_rtn(float);
uint __CVAttrs convert_uint_sat_rtn(double);
uint2 __CVAttrs convert_uint2(char2);
uint2 __CVAttrs convert_uint2(uchar2);
uint2 __CVAttrs convert_uint2(short2);
uint2 __CVAttrs convert_uint2(ushort2);
uint2 __CVAttrs convert_uint2(int2);
uint2 __CVAttrs convert_uint2(uint2);
uint2 __CVAttrs convert_uint2(long2);
uint2 __CVAttrs convert_uint2(ulong2);
uint2 __CVAttrs convert_uint2(float2);
uint2 __CVAttrs convert_uint2(double2);
uint2 __CVAttrs convert_uint2_rte(char2);
uint2 __CVAttrs convert_uint2_rte(uchar2);
uint2 __CVAttrs convert_uint2_rte(short2);
uint2 __CVAttrs convert_uint2_rte(ushort2);
uint2 __CVAttrs convert_uint2_rte(int2);
uint2 __CVAttrs convert_uint2_rte(uint2);
uint2 __CVAttrs convert_uint2_rte(long2);
uint2 __CVAttrs convert_uint2_rte(ulong2);
uint2 __CVAttrs convert_uint2_rte(float2);
uint2 __CVAttrs convert_uint2_rte(double2);
uint2 __CVAttrs convert_uint2_rtz(char2);
uint2 __CVAttrs convert_uint2_rtz(uchar2);
uint2 __CVAttrs convert_uint2_rtz(short2);
uint2 __CVAttrs convert_uint2_rtz(ushort2);
uint2 __CVAttrs convert_uint2_rtz(int2);
uint2 __CVAttrs convert_uint2_rtz(uint2);
uint2 __CVAttrs convert_uint2_rtz(long2);
uint2 __CVAttrs convert_uint2_rtz(ulong2);
uint2 __CVAttrs convert_uint2_rtz(float2);
uint2 __CVAttrs convert_uint2_rtz(double2);
uint2 __CVAttrs convert_uint2_rtp(char2);
uint2 __CVAttrs convert_uint2_rtp(uchar2);
uint2 __CVAttrs convert_uint2_rtp(short2);
uint2 __CVAttrs convert_uint2_rtp(ushort2);
uint2 __CVAttrs convert_uint2_rtp(int2);
uint2 __CVAttrs convert_uint2_rtp(uint2);
uint2 __CVAttrs convert_uint2_rtp(long2);
uint2 __CVAttrs convert_uint2_rtp(ulong2);
uint2 __CVAttrs convert_uint2_rtp(float2);
uint2 __CVAttrs convert_uint2_rtp(double2);
uint2 __CVAttrs convert_uint2_rtn(char2);
uint2 __CVAttrs convert_uint2_rtn(uchar2);
uint2 __CVAttrs convert_uint2_rtn(short2);
uint2 __CVAttrs convert_uint2_rtn(ushort2);
uint2 __CVAttrs convert_uint2_rtn(int2);
uint2 __CVAttrs convert_uint2_rtn(uint2);
uint2 __CVAttrs convert_uint2_rtn(long2);
uint2 __CVAttrs convert_uint2_rtn(ulong2);
uint2 __CVAttrs convert_uint2_rtn(float2);
uint2 __CVAttrs convert_uint2_rtn(double2);
uint2 __CVAttrs convert_uint2_sat(char2);
uint2 __CVAttrs convert_uint2_sat(uchar2);
uint2 __CVAttrs convert_uint2_sat(short2);
uint2 __CVAttrs convert_uint2_sat(ushort2);
uint2 __CVAttrs convert_uint2_sat(int2);
uint2 __CVAttrs convert_uint2_sat(uint2);
uint2 __CVAttrs convert_uint2_sat(long2);
uint2 __CVAttrs convert_uint2_sat(ulong2);
uint2 __CVAttrs convert_uint2_sat(float2);
uint2 __CVAttrs convert_uint2_sat(double2);
uint2 __CVAttrs convert_uint2_sat_rte(char2);
uint2 __CVAttrs convert_uint2_sat_rte(uchar2);
uint2 __CVAttrs convert_uint2_sat_rte(short2);
uint2 __CVAttrs convert_uint2_sat_rte(ushort2);
uint2 __CVAttrs convert_uint2_sat_rte(int2);
uint2 __CVAttrs convert_uint2_sat_rte(uint2);
uint2 __CVAttrs convert_uint2_sat_rte(long2);
uint2 __CVAttrs convert_uint2_sat_rte(ulong2);
uint2 __CVAttrs convert_uint2_sat_rte(float2);
uint2 __CVAttrs convert_uint2_sat_rte(double2);
uint2 __CVAttrs convert_uint2_sat_rtz(char2);
uint2 __CVAttrs convert_uint2_sat_rtz(uchar2);
uint2 __CVAttrs convert_uint2_sat_rtz(short2);
uint2 __CVAttrs convert_uint2_sat_rtz(ushort2);
uint2 __CVAttrs convert_uint2_sat_rtz(int2);
uint2 __CVAttrs convert_uint2_sat_rtz(uint2);
uint2 __CVAttrs convert_uint2_sat_rtz(long2);
uint2 __CVAttrs convert_uint2_sat_rtz(ulong2);
uint2 __CVAttrs convert_uint2_sat_rtz(float2);
uint2 __CVAttrs convert_uint2_sat_rtz(double2);
uint2 __CVAttrs convert_uint2_sat_rtp(char2);
uint2 __CVAttrs convert_uint2_sat_rtp(uchar2);
uint2 __CVAttrs convert_uint2_sat_rtp(short2);
uint2 __CVAttrs convert_uint2_sat_rtp(ushort2);
uint2 __CVAttrs convert_uint2_sat_rtp(int2);
uint2 __CVAttrs convert_uint2_sat_rtp(uint2);
uint2 __CVAttrs convert_uint2_sat_rtp(long2);
uint2 __CVAttrs convert_uint2_sat_rtp(ulong2);
uint2 __CVAttrs convert_uint2_sat_rtp(float2);
uint2 __CVAttrs convert_uint2_sat_rtp(double2);
uint2 __CVAttrs convert_uint2_sat_rtn(char2);
uint2 __CVAttrs convert_uint2_sat_rtn(uchar2);
uint2 __CVAttrs convert_uint2_sat_rtn(short2);
uint2 __CVAttrs convert_uint2_sat_rtn(ushort2);
uint2 __CVAttrs convert_uint2_sat_rtn(int2);
uint2 __CVAttrs convert_uint2_sat_rtn(uint2);
uint2 __CVAttrs convert_uint2_sat_rtn(long2);
uint2 __CVAttrs convert_uint2_sat_rtn(ulong2);
uint2 __CVAttrs convert_uint2_sat_rtn(float2);
uint2 __CVAttrs convert_uint2_sat_rtn(double2);
#if __OPENCL_C_VERSION__ >= 110
uint3 __CVAttrs convert_uint3(char3);
uint3 __CVAttrs convert_uint3(uchar3);
uint3 __CVAttrs convert_uint3(short3);
uint3 __CVAttrs convert_uint3(ushort3);
uint3 __CVAttrs convert_uint3(int3);
uint3 __CVAttrs convert_uint3(uint3);
uint3 __CVAttrs convert_uint3(long3);
uint3 __CVAttrs convert_uint3(ulong3);
uint3 __CVAttrs convert_uint3(float3);
uint3 __CVAttrs convert_uint3(double3);
uint3 __CVAttrs convert_uint3_rte(char3);
uint3 __CVAttrs convert_uint3_rte(uchar3);
uint3 __CVAttrs convert_uint3_rte(short3);
uint3 __CVAttrs convert_uint3_rte(ushort3);
uint3 __CVAttrs convert_uint3_rte(int3);
uint3 __CVAttrs convert_uint3_rte(uint3);
uint3 __CVAttrs convert_uint3_rte(long3);
uint3 __CVAttrs convert_uint3_rte(ulong3);
uint3 __CVAttrs convert_uint3_rte(float3);
uint3 __CVAttrs convert_uint3_rte(double3);
uint3 __CVAttrs convert_uint3_rtz(char3);
uint3 __CVAttrs convert_uint3_rtz(uchar3);
uint3 __CVAttrs convert_uint3_rtz(short3);
uint3 __CVAttrs convert_uint3_rtz(ushort3);
uint3 __CVAttrs convert_uint3_rtz(int3);
uint3 __CVAttrs convert_uint3_rtz(uint3);
uint3 __CVAttrs convert_uint3_rtz(long3);
uint3 __CVAttrs convert_uint3_rtz(ulong3);
uint3 __CVAttrs convert_uint3_rtz(float3);
uint3 __CVAttrs convert_uint3_rtz(double3);
uint3 __CVAttrs convert_uint3_rtp(char3);
uint3 __CVAttrs convert_uint3_rtp(uchar3);
uint3 __CVAttrs convert_uint3_rtp(short3);
uint3 __CVAttrs convert_uint3_rtp(ushort3);
uint3 __CVAttrs convert_uint3_rtp(int3);
uint3 __CVAttrs convert_uint3_rtp(uint3);
uint3 __CVAttrs convert_uint3_rtp(long3);
uint3 __CVAttrs convert_uint3_rtp(ulong3);
uint3 __CVAttrs convert_uint3_rtp(float3);
uint3 __CVAttrs convert_uint3_rtp(double3);
uint3 __CVAttrs convert_uint3_rtn(char3);
uint3 __CVAttrs convert_uint3_rtn(uchar3);
uint3 __CVAttrs convert_uint3_rtn(short3);
uint3 __CVAttrs convert_uint3_rtn(ushort3);
uint3 __CVAttrs convert_uint3_rtn(int3);
uint3 __CVAttrs convert_uint3_rtn(uint3);
uint3 __CVAttrs convert_uint3_rtn(long3);
uint3 __CVAttrs convert_uint3_rtn(ulong3);
uint3 __CVAttrs convert_uint3_rtn(float3);
uint3 __CVAttrs convert_uint3_rtn(double3);
uint3 __CVAttrs convert_uint3_sat(char3);
uint3 __CVAttrs convert_uint3_sat(uchar3);
uint3 __CVAttrs convert_uint3_sat(short3);
uint3 __CVAttrs convert_uint3_sat(ushort3);
uint3 __CVAttrs convert_uint3_sat(int3);
uint3 __CVAttrs convert_uint3_sat(uint3);
uint3 __CVAttrs convert_uint3_sat(long3);
uint3 __CVAttrs convert_uint3_sat(ulong3);
uint3 __CVAttrs convert_uint3_sat(float3);
uint3 __CVAttrs convert_uint3_sat(double3);
uint3 __CVAttrs convert_uint3_sat_rte(char3);
uint3 __CVAttrs convert_uint3_sat_rte(uchar3);
uint3 __CVAttrs convert_uint3_sat_rte(short3);
uint3 __CVAttrs convert_uint3_sat_rte(ushort3);
uint3 __CVAttrs convert_uint3_sat_rte(int3);
uint3 __CVAttrs convert_uint3_sat_rte(uint3);
uint3 __CVAttrs convert_uint3_sat_rte(long3);
uint3 __CVAttrs convert_uint3_sat_rte(ulong3);
uint3 __CVAttrs convert_uint3_sat_rte(float3);
uint3 __CVAttrs convert_uint3_sat_rte(double3);
uint3 __CVAttrs convert_uint3_sat_rtz(char3);
uint3 __CVAttrs convert_uint3_sat_rtz(uchar3);
uint3 __CVAttrs convert_uint3_sat_rtz(short3);
uint3 __CVAttrs convert_uint3_sat_rtz(ushort3);
uint3 __CVAttrs convert_uint3_sat_rtz(int3);
uint3 __CVAttrs convert_uint3_sat_rtz(uint3);
uint3 __CVAttrs convert_uint3_sat_rtz(long3);
uint3 __CVAttrs convert_uint3_sat_rtz(ulong3);
uint3 __CVAttrs convert_uint3_sat_rtz(float3);
uint3 __CVAttrs convert_uint3_sat_rtz(double3);
uint3 __CVAttrs convert_uint3_sat_rtp(char3);
uint3 __CVAttrs convert_uint3_sat_rtp(uchar3);
uint3 __CVAttrs convert_uint3_sat_rtp(short3);
uint3 __CVAttrs convert_uint3_sat_rtp(ushort3);
uint3 __CVAttrs convert_uint3_sat_rtp(int3);
uint3 __CVAttrs convert_uint3_sat_rtp(uint3);
uint3 __CVAttrs convert_uint3_sat_rtp(long3);
uint3 __CVAttrs convert_uint3_sat_rtp(ulong3);
uint3 __CVAttrs convert_uint3_sat_rtp(float3);
uint3 __CVAttrs convert_uint3_sat_rtp(double3);
uint3 __CVAttrs convert_uint3_sat_rtn(char3);
uint3 __CVAttrs convert_uint3_sat_rtn(uchar3);
uint3 __CVAttrs convert_uint3_sat_rtn(short3);
uint3 __CVAttrs convert_uint3_sat_rtn(ushort3);
uint3 __CVAttrs convert_uint3_sat_rtn(int3);
uint3 __CVAttrs convert_uint3_sat_rtn(uint3);
uint3 __CVAttrs convert_uint3_sat_rtn(long3);
uint3 __CVAttrs convert_uint3_sat_rtn(ulong3);
uint3 __CVAttrs convert_uint3_sat_rtn(float3);
uint3 __CVAttrs convert_uint3_sat_rtn(double3);
#endif
uint4 __CVAttrs convert_uint4(char4);
uint4 __CVAttrs convert_uint4(uchar4);
uint4 __CVAttrs convert_uint4(short4);
uint4 __CVAttrs convert_uint4(ushort4);
uint4 __CVAttrs convert_uint4(int4);
uint4 __CVAttrs convert_uint4(uint4);
uint4 __CVAttrs convert_uint4(long4);
uint4 __CVAttrs convert_uint4(ulong4);
uint4 __CVAttrs convert_uint4(float4);
uint4 __CVAttrs convert_uint4(double4);
uint4 __CVAttrs convert_uint4_rte(char4);
uint4 __CVAttrs convert_uint4_rte(uchar4);
uint4 __CVAttrs convert_uint4_rte(short4);
uint4 __CVAttrs convert_uint4_rte(ushort4);
uint4 __CVAttrs convert_uint4_rte(int4);
uint4 __CVAttrs convert_uint4_rte(uint4);
uint4 __CVAttrs convert_uint4_rte(long4);
uint4 __CVAttrs convert_uint4_rte(ulong4);
uint4 __CVAttrs convert_uint4_rte(float4);
uint4 __CVAttrs convert_uint4_rte(double4);
uint4 __CVAttrs convert_uint4_rtz(char4);
uint4 __CVAttrs convert_uint4_rtz(uchar4);
uint4 __CVAttrs convert_uint4_rtz(short4);
uint4 __CVAttrs convert_uint4_rtz(ushort4);
uint4 __CVAttrs convert_uint4_rtz(int4);
uint4 __CVAttrs convert_uint4_rtz(uint4);
uint4 __CVAttrs convert_uint4_rtz(long4);
uint4 __CVAttrs convert_uint4_rtz(ulong4);
uint4 __CVAttrs convert_uint4_rtz(float4);
uint4 __CVAttrs convert_uint4_rtz(double4);
uint4 __CVAttrs convert_uint4_rtp(char4);
uint4 __CVAttrs convert_uint4_rtp(uchar4);
uint4 __CVAttrs convert_uint4_rtp(short4);
uint4 __CVAttrs convert_uint4_rtp(ushort4);
uint4 __CVAttrs convert_uint4_rtp(int4);
uint4 __CVAttrs convert_uint4_rtp(uint4);
uint4 __CVAttrs convert_uint4_rtp(long4);
uint4 __CVAttrs convert_uint4_rtp(ulong4);
uint4 __CVAttrs convert_uint4_rtp(float4);
uint4 __CVAttrs convert_uint4_rtp(double4);
uint4 __CVAttrs convert_uint4_rtn(char4);
uint4 __CVAttrs convert_uint4_rtn(uchar4);
uint4 __CVAttrs convert_uint4_rtn(short4);
uint4 __CVAttrs convert_uint4_rtn(ushort4);
uint4 __CVAttrs convert_uint4_rtn(int4);
uint4 __CVAttrs convert_uint4_rtn(uint4);
uint4 __CVAttrs convert_uint4_rtn(long4);
uint4 __CVAttrs convert_uint4_rtn(ulong4);
uint4 __CVAttrs convert_uint4_rtn(float4);
uint4 __CVAttrs convert_uint4_rtn(double4);
uint4 __CVAttrs convert_uint4_sat(char4);
uint4 __CVAttrs convert_uint4_sat(uchar4);
uint4 __CVAttrs convert_uint4_sat(short4);
uint4 __CVAttrs convert_uint4_sat(ushort4);
uint4 __CVAttrs convert_uint4_sat(int4);
uint4 __CVAttrs convert_uint4_sat(uint4);
uint4 __CVAttrs convert_uint4_sat(long4);
uint4 __CVAttrs convert_uint4_sat(ulong4);
uint4 __CVAttrs convert_uint4_sat(float4);
uint4 __CVAttrs convert_uint4_sat(double4);
uint4 __CVAttrs convert_uint4_sat_rte(char4);
uint4 __CVAttrs convert_uint4_sat_rte(uchar4);
uint4 __CVAttrs convert_uint4_sat_rte(short4);
uint4 __CVAttrs convert_uint4_sat_rte(ushort4);
uint4 __CVAttrs convert_uint4_sat_rte(int4);
uint4 __CVAttrs convert_uint4_sat_rte(uint4);
uint4 __CVAttrs convert_uint4_sat_rte(long4);
uint4 __CVAttrs convert_uint4_sat_rte(ulong4);
uint4 __CVAttrs convert_uint4_sat_rte(float4);
uint4 __CVAttrs convert_uint4_sat_rte(double4);
uint4 __CVAttrs convert_uint4_sat_rtz(char4);
uint4 __CVAttrs convert_uint4_sat_rtz(uchar4);
uint4 __CVAttrs convert_uint4_sat_rtz(short4);
uint4 __CVAttrs convert_uint4_sat_rtz(ushort4);
uint4 __CVAttrs convert_uint4_sat_rtz(int4);
uint4 __CVAttrs convert_uint4_sat_rtz(uint4);
uint4 __CVAttrs convert_uint4_sat_rtz(long4);
uint4 __CVAttrs convert_uint4_sat_rtz(ulong4);
uint4 __CVAttrs convert_uint4_sat_rtz(float4);
uint4 __CVAttrs convert_uint4_sat_rtz(double4);
uint4 __CVAttrs convert_uint4_sat_rtp(char4);
uint4 __CVAttrs convert_uint4_sat_rtp(uchar4);
uint4 __CVAttrs convert_uint4_sat_rtp(short4);
uint4 __CVAttrs convert_uint4_sat_rtp(ushort4);
uint4 __CVAttrs convert_uint4_sat_rtp(int4);
uint4 __CVAttrs convert_uint4_sat_rtp(uint4);
uint4 __CVAttrs convert_uint4_sat_rtp(long4);
uint4 __CVAttrs convert_uint4_sat_rtp(ulong4);
uint4 __CVAttrs convert_uint4_sat_rtp(float4);
uint4 __CVAttrs convert_uint4_sat_rtp(double4);
uint4 __CVAttrs convert_uint4_sat_rtn(char4);
uint4 __CVAttrs convert_uint4_sat_rtn(uchar4);
uint4 __CVAttrs convert_uint4_sat_rtn(short4);
uint4 __CVAttrs convert_uint4_sat_rtn(ushort4);
uint4 __CVAttrs convert_uint4_sat_rtn(int4);
uint4 __CVAttrs convert_uint4_sat_rtn(uint4);
uint4 __CVAttrs convert_uint4_sat_rtn(long4);
uint4 __CVAttrs convert_uint4_sat_rtn(ulong4);
uint4 __CVAttrs convert_uint4_sat_rtn(float4);
uint4 __CVAttrs convert_uint4_sat_rtn(double4);
uint8 __CVAttrs convert_uint8(char8);
uint8 __CVAttrs convert_uint8(uchar8);
uint8 __CVAttrs convert_uint8(short8);
uint8 __CVAttrs convert_uint8(ushort8);
uint8 __CVAttrs convert_uint8(int8);
uint8 __CVAttrs convert_uint8(uint8);
uint8 __CVAttrs convert_uint8(long8);
uint8 __CVAttrs convert_uint8(ulong8);
uint8 __CVAttrs convert_uint8(float8);
uint8 __CVAttrs convert_uint8(double8);
uint8 __CVAttrs convert_uint8_rte(char8);
uint8 __CVAttrs convert_uint8_rte(uchar8);
uint8 __CVAttrs convert_uint8_rte(short8);
uint8 __CVAttrs convert_uint8_rte(ushort8);
uint8 __CVAttrs convert_uint8_rte(int8);
uint8 __CVAttrs convert_uint8_rte(uint8);
uint8 __CVAttrs convert_uint8_rte(long8);
uint8 __CVAttrs convert_uint8_rte(ulong8);
uint8 __CVAttrs convert_uint8_rte(float8);
uint8 __CVAttrs convert_uint8_rte(double8);
uint8 __CVAttrs convert_uint8_rtz(char8);
uint8 __CVAttrs convert_uint8_rtz(uchar8);
uint8 __CVAttrs convert_uint8_rtz(short8);
uint8 __CVAttrs convert_uint8_rtz(ushort8);
uint8 __CVAttrs convert_uint8_rtz(int8);
uint8 __CVAttrs convert_uint8_rtz(uint8);
uint8 __CVAttrs convert_uint8_rtz(long8);
uint8 __CVAttrs convert_uint8_rtz(ulong8);
uint8 __CVAttrs convert_uint8_rtz(float8);
uint8 __CVAttrs convert_uint8_rtz(double8);
uint8 __CVAttrs convert_uint8_rtp(char8);
uint8 __CVAttrs convert_uint8_rtp(uchar8);
uint8 __CVAttrs convert_uint8_rtp(short8);
uint8 __CVAttrs convert_uint8_rtp(ushort8);
uint8 __CVAttrs convert_uint8_rtp(int8);
uint8 __CVAttrs convert_uint8_rtp(uint8);
uint8 __CVAttrs convert_uint8_rtp(long8);
uint8 __CVAttrs convert_uint8_rtp(ulong8);
uint8 __CVAttrs convert_uint8_rtp(float8);
uint8 __CVAttrs convert_uint8_rtp(double8);
uint8 __CVAttrs convert_uint8_rtn(char8);
uint8 __CVAttrs convert_uint8_rtn(uchar8);
uint8 __CVAttrs convert_uint8_rtn(short8);
uint8 __CVAttrs convert_uint8_rtn(ushort8);
uint8 __CVAttrs convert_uint8_rtn(int8);
uint8 __CVAttrs convert_uint8_rtn(uint8);
uint8 __CVAttrs convert_uint8_rtn(long8);
uint8 __CVAttrs convert_uint8_rtn(ulong8);
uint8 __CVAttrs convert_uint8_rtn(float8);
uint8 __CVAttrs convert_uint8_rtn(double8);
uint8 __CVAttrs convert_uint8_sat(char8);
uint8 __CVAttrs convert_uint8_sat(uchar8);
uint8 __CVAttrs convert_uint8_sat(short8);
uint8 __CVAttrs convert_uint8_sat(ushort8);
uint8 __CVAttrs convert_uint8_sat(int8);
uint8 __CVAttrs convert_uint8_sat(uint8);
uint8 __CVAttrs convert_uint8_sat(long8);
uint8 __CVAttrs convert_uint8_sat(ulong8);
uint8 __CVAttrs convert_uint8_sat(float8);
uint8 __CVAttrs convert_uint8_sat(double8);
uint8 __CVAttrs convert_uint8_sat_rte(char8);
uint8 __CVAttrs convert_uint8_sat_rte(uchar8);
uint8 __CVAttrs convert_uint8_sat_rte(short8);
uint8 __CVAttrs convert_uint8_sat_rte(ushort8);
uint8 __CVAttrs convert_uint8_sat_rte(int8);
uint8 __CVAttrs convert_uint8_sat_rte(uint8);
uint8 __CVAttrs convert_uint8_sat_rte(long8);
uint8 __CVAttrs convert_uint8_sat_rte(ulong8);
uint8 __CVAttrs convert_uint8_sat_rte(float8);
uint8 __CVAttrs convert_uint8_sat_rte(double8);
uint8 __CVAttrs convert_uint8_sat_rtz(char8);
uint8 __CVAttrs convert_uint8_sat_rtz(uchar8);
uint8 __CVAttrs convert_uint8_sat_rtz(short8);
uint8 __CVAttrs convert_uint8_sat_rtz(ushort8);
uint8 __CVAttrs convert_uint8_sat_rtz(int8);
uint8 __CVAttrs convert_uint8_sat_rtz(uint8);
uint8 __CVAttrs convert_uint8_sat_rtz(long8);
uint8 __CVAttrs convert_uint8_sat_rtz(ulong8);
uint8 __CVAttrs convert_uint8_sat_rtz(float8);
uint8 __CVAttrs convert_uint8_sat_rtz(double8);
uint8 __CVAttrs convert_uint8_sat_rtp(char8);
uint8 __CVAttrs convert_uint8_sat_rtp(uchar8);
uint8 __CVAttrs convert_uint8_sat_rtp(short8);
uint8 __CVAttrs convert_uint8_sat_rtp(ushort8);
uint8 __CVAttrs convert_uint8_sat_rtp(int8);
uint8 __CVAttrs convert_uint8_sat_rtp(uint8);
uint8 __CVAttrs convert_uint8_sat_rtp(long8);
uint8 __CVAttrs convert_uint8_sat_rtp(ulong8);
uint8 __CVAttrs convert_uint8_sat_rtp(float8);
uint8 __CVAttrs convert_uint8_sat_rtp(double8);
uint8 __CVAttrs convert_uint8_sat_rtn(char8);
uint8 __CVAttrs convert_uint8_sat_rtn(uchar8);
uint8 __CVAttrs convert_uint8_sat_rtn(short8);
uint8 __CVAttrs convert_uint8_sat_rtn(ushort8);
uint8 __CVAttrs convert_uint8_sat_rtn(int8);
uint8 __CVAttrs convert_uint8_sat_rtn(uint8);
uint8 __CVAttrs convert_uint8_sat_rtn(long8);
uint8 __CVAttrs convert_uint8_sat_rtn(ulong8);
uint8 __CVAttrs convert_uint8_sat_rtn(float8);
uint8 __CVAttrs convert_uint8_sat_rtn(double8);
uint16 __CVAttrs convert_uint16(char16);
uint16 __CVAttrs convert_uint16(uchar16);
uint16 __CVAttrs convert_uint16(short16);
uint16 __CVAttrs convert_uint16(ushort16);
uint16 __CVAttrs convert_uint16(int16);
uint16 __CVAttrs convert_uint16(uint16);
uint16 __CVAttrs convert_uint16(long16);
uint16 __CVAttrs convert_uint16(ulong16);
uint16 __CVAttrs convert_uint16(float16);
uint16 __CVAttrs convert_uint16(double16);
uint16 __CVAttrs convert_uint16_rte(char16);
uint16 __CVAttrs convert_uint16_rte(uchar16);
uint16 __CVAttrs convert_uint16_rte(short16);
uint16 __CVAttrs convert_uint16_rte(ushort16);
uint16 __CVAttrs convert_uint16_rte(int16);
uint16 __CVAttrs convert_uint16_rte(uint16);
uint16 __CVAttrs convert_uint16_rte(long16);
uint16 __CVAttrs convert_uint16_rte(ulong16);
uint16 __CVAttrs convert_uint16_rte(float16);
uint16 __CVAttrs convert_uint16_rte(double16);
uint16 __CVAttrs convert_uint16_rtz(char16);
uint16 __CVAttrs convert_uint16_rtz(uchar16);
uint16 __CVAttrs convert_uint16_rtz(short16);
uint16 __CVAttrs convert_uint16_rtz(ushort16);
uint16 __CVAttrs convert_uint16_rtz(int16);
uint16 __CVAttrs convert_uint16_rtz(uint16);
uint16 __CVAttrs convert_uint16_rtz(long16);
uint16 __CVAttrs convert_uint16_rtz(ulong16);
uint16 __CVAttrs convert_uint16_rtz(float16);
uint16 __CVAttrs convert_uint16_rtz(double16);
uint16 __CVAttrs convert_uint16_rtp(char16);
uint16 __CVAttrs convert_uint16_rtp(uchar16);
uint16 __CVAttrs convert_uint16_rtp(short16);
uint16 __CVAttrs convert_uint16_rtp(ushort16);
uint16 __CVAttrs convert_uint16_rtp(int16);
uint16 __CVAttrs convert_uint16_rtp(uint16);
uint16 __CVAttrs convert_uint16_rtp(long16);
uint16 __CVAttrs convert_uint16_rtp(ulong16);
uint16 __CVAttrs convert_uint16_rtp(float16);
uint16 __CVAttrs convert_uint16_rtp(double16);
uint16 __CVAttrs convert_uint16_rtn(char16);
uint16 __CVAttrs convert_uint16_rtn(uchar16);
uint16 __CVAttrs convert_uint16_rtn(short16);
uint16 __CVAttrs convert_uint16_rtn(ushort16);
uint16 __CVAttrs convert_uint16_rtn(int16);
uint16 __CVAttrs convert_uint16_rtn(uint16);
uint16 __CVAttrs convert_uint16_rtn(long16);
uint16 __CVAttrs convert_uint16_rtn(ulong16);
uint16 __CVAttrs convert_uint16_rtn(float16);
uint16 __CVAttrs convert_uint16_rtn(double16);
uint16 __CVAttrs convert_uint16_sat(char16);
uint16 __CVAttrs convert_uint16_sat(uchar16);
uint16 __CVAttrs convert_uint16_sat(short16);
uint16 __CVAttrs convert_uint16_sat(ushort16);
uint16 __CVAttrs convert_uint16_sat(int16);
uint16 __CVAttrs convert_uint16_sat(uint16);
uint16 __CVAttrs convert_uint16_sat(long16);
uint16 __CVAttrs convert_uint16_sat(ulong16);
uint16 __CVAttrs convert_uint16_sat(float16);
uint16 __CVAttrs convert_uint16_sat(double16);
uint16 __CVAttrs convert_uint16_sat_rte(char16);
uint16 __CVAttrs convert_uint16_sat_rte(uchar16);
uint16 __CVAttrs convert_uint16_sat_rte(short16);
uint16 __CVAttrs convert_uint16_sat_rte(ushort16);
uint16 __CVAttrs convert_uint16_sat_rte(int16);
uint16 __CVAttrs convert_uint16_sat_rte(uint16);
uint16 __CVAttrs convert_uint16_sat_rte(long16);
uint16 __CVAttrs convert_uint16_sat_rte(ulong16);
uint16 __CVAttrs convert_uint16_sat_rte(float16);
uint16 __CVAttrs convert_uint16_sat_rte(double16);
uint16 __CVAttrs convert_uint16_sat_rtz(char16);
uint16 __CVAttrs convert_uint16_sat_rtz(uchar16);
uint16 __CVAttrs convert_uint16_sat_rtz(short16);
uint16 __CVAttrs convert_uint16_sat_rtz(ushort16);
uint16 __CVAttrs convert_uint16_sat_rtz(int16);
uint16 __CVAttrs convert_uint16_sat_rtz(uint16);
uint16 __CVAttrs convert_uint16_sat_rtz(long16);
uint16 __CVAttrs convert_uint16_sat_rtz(ulong16);
uint16 __CVAttrs convert_uint16_sat_rtz(float16);
uint16 __CVAttrs convert_uint16_sat_rtz(double16);
uint16 __CVAttrs convert_uint16_sat_rtp(char16);
uint16 __CVAttrs convert_uint16_sat_rtp(uchar16);
uint16 __CVAttrs convert_uint16_sat_rtp(short16);
uint16 __CVAttrs convert_uint16_sat_rtp(ushort16);
uint16 __CVAttrs convert_uint16_sat_rtp(int16);
uint16 __CVAttrs convert_uint16_sat_rtp(uint16);
uint16 __CVAttrs convert_uint16_sat_rtp(long16);
uint16 __CVAttrs convert_uint16_sat_rtp(ulong16);
uint16 __CVAttrs convert_uint16_sat_rtp(float16);
uint16 __CVAttrs convert_uint16_sat_rtp(double16);
uint16 __CVAttrs convert_uint16_sat_rtn(char16);
uint16 __CVAttrs convert_uint16_sat_rtn(uchar16);
uint16 __CVAttrs convert_uint16_sat_rtn(short16);
uint16 __CVAttrs convert_uint16_sat_rtn(ushort16);
uint16 __CVAttrs convert_uint16_sat_rtn(int16);
uint16 __CVAttrs convert_uint16_sat_rtn(uint16);
uint16 __CVAttrs convert_uint16_sat_rtn(long16);
uint16 __CVAttrs convert_uint16_sat_rtn(ulong16);
uint16 __CVAttrs convert_uint16_sat_rtn(float16);
uint16 __CVAttrs convert_uint16_sat_rtn(double16);
long __CVAttrs convert_long(char);
long __CVAttrs convert_long(uchar);
long __CVAttrs convert_long(short);
long __CVAttrs convert_long(ushort);
long __CVAttrs convert_long(int);
long __CVAttrs convert_long(uint);
long __CVAttrs convert_long(long);
long __CVAttrs convert_long(ulong);
long __CVAttrs convert_long(float);
long __CVAttrs convert_long(double);
long __CVAttrs convert_long_rte(char);
long __CVAttrs convert_long_rte(uchar);
long __CVAttrs convert_long_rte(short);
long __CVAttrs convert_long_rte(ushort);
long __CVAttrs convert_long_rte(int);
long __CVAttrs convert_long_rte(uint);
long __CVAttrs convert_long_rte(long);
long __CVAttrs convert_long_rte(ulong);
long __CVAttrs convert_long_rte(float);
long __CVAttrs convert_long_rte(double);
long __CVAttrs convert_long_rtz(char);
long __CVAttrs convert_long_rtz(uchar);
long __CVAttrs convert_long_rtz(short);
long __CVAttrs convert_long_rtz(ushort);
long __CVAttrs convert_long_rtz(int);
long __CVAttrs convert_long_rtz(uint);
long __CVAttrs convert_long_rtz(long);
long __CVAttrs convert_long_rtz(ulong);
long __CVAttrs convert_long_rtz(float);
long __CVAttrs convert_long_rtz(double);
long __CVAttrs convert_long_rtp(char);
long __CVAttrs convert_long_rtp(uchar);
long __CVAttrs convert_long_rtp(short);
long __CVAttrs convert_long_rtp(ushort);
long __CVAttrs convert_long_rtp(int);
long __CVAttrs convert_long_rtp(uint);
long __CVAttrs convert_long_rtp(long);
long __CVAttrs convert_long_rtp(ulong);
long __CVAttrs convert_long_rtp(float);
long __CVAttrs convert_long_rtp(double);
long __CVAttrs convert_long_rtn(char);
long __CVAttrs convert_long_rtn(uchar);
long __CVAttrs convert_long_rtn(short);
long __CVAttrs convert_long_rtn(ushort);
long __CVAttrs convert_long_rtn(int);
long __CVAttrs convert_long_rtn(uint);
long __CVAttrs convert_long_rtn(long);
long __CVAttrs convert_long_rtn(ulong);
long __CVAttrs convert_long_rtn(float);
long __CVAttrs convert_long_rtn(double);
long __CVAttrs convert_long_sat(char);
long __CVAttrs convert_long_sat(uchar);
long __CVAttrs convert_long_sat(short);
long __CVAttrs convert_long_sat(ushort);
long __CVAttrs convert_long_sat(int);
long __CVAttrs convert_long_sat(uint);
long __CVAttrs convert_long_sat(long);
long __CVAttrs convert_long_sat(ulong);
long __CVAttrs convert_long_sat(float);
long __CVAttrs convert_long_sat(double);
long __CVAttrs convert_long_sat_rte(char);
long __CVAttrs convert_long_sat_rte(uchar);
long __CVAttrs convert_long_sat_rte(short);
long __CVAttrs convert_long_sat_rte(ushort);
long __CVAttrs convert_long_sat_rte(int);
long __CVAttrs convert_long_sat_rte(uint);
long __CVAttrs convert_long_sat_rte(long);
long __CVAttrs convert_long_sat_rte(ulong);
long __CVAttrs convert_long_sat_rte(float);
long __CVAttrs convert_long_sat_rte(double);
long __CVAttrs convert_long_sat_rtz(char);
long __CVAttrs convert_long_sat_rtz(uchar);
long __CVAttrs convert_long_sat_rtz(short);
long __CVAttrs convert_long_sat_rtz(ushort);
long __CVAttrs convert_long_sat_rtz(int);
long __CVAttrs convert_long_sat_rtz(uint);
long __CVAttrs convert_long_sat_rtz(long);
long __CVAttrs convert_long_sat_rtz(ulong);
long __CVAttrs convert_long_sat_rtz(float);
long __CVAttrs convert_long_sat_rtz(double);
long __CVAttrs convert_long_sat_rtp(char);
long __CVAttrs convert_long_sat_rtp(uchar);
long __CVAttrs convert_long_sat_rtp(short);
long __CVAttrs convert_long_sat_rtp(ushort);
long __CVAttrs convert_long_sat_rtp(int);
long __CVAttrs convert_long_sat_rtp(uint);
long __CVAttrs convert_long_sat_rtp(long);
long __CVAttrs convert_long_sat_rtp(ulong);
long __CVAttrs convert_long_sat_rtp(float);
long __CVAttrs convert_long_sat_rtp(double);
long __CVAttrs convert_long_sat_rtn(char);
long __CVAttrs convert_long_sat_rtn(uchar);
long __CVAttrs convert_long_sat_rtn(short);
long __CVAttrs convert_long_sat_rtn(ushort);
long __CVAttrs convert_long_sat_rtn(int);
long __CVAttrs convert_long_sat_rtn(uint);
long __CVAttrs convert_long_sat_rtn(long);
long __CVAttrs convert_long_sat_rtn(ulong);
long __CVAttrs convert_long_sat_rtn(float);
long __CVAttrs convert_long_sat_rtn(double);
long2 __CVAttrs convert_long2(char2);
long2 __CVAttrs convert_long2(uchar2);
long2 __CVAttrs convert_long2(short2);
long2 __CVAttrs convert_long2(ushort2);
long2 __CVAttrs convert_long2(int2);
long2 __CVAttrs convert_long2(uint2);
long2 __CVAttrs convert_long2(long2);
long2 __CVAttrs convert_long2(ulong2);
long2 __CVAttrs convert_long2(float2);
long2 __CVAttrs convert_long2(double2);
long2 __CVAttrs convert_long2_rte(char2);
long2 __CVAttrs convert_long2_rte(uchar2);
long2 __CVAttrs convert_long2_rte(short2);
long2 __CVAttrs convert_long2_rte(ushort2);
long2 __CVAttrs convert_long2_rte(int2);
long2 __CVAttrs convert_long2_rte(uint2);
long2 __CVAttrs convert_long2_rte(long2);
long2 __CVAttrs convert_long2_rte(ulong2);
long2 __CVAttrs convert_long2_rte(float2);
long2 __CVAttrs convert_long2_rte(double2);
long2 __CVAttrs convert_long2_rtz(char2);
long2 __CVAttrs convert_long2_rtz(uchar2);
long2 __CVAttrs convert_long2_rtz(short2);
long2 __CVAttrs convert_long2_rtz(ushort2);
long2 __CVAttrs convert_long2_rtz(int2);
long2 __CVAttrs convert_long2_rtz(uint2);
long2 __CVAttrs convert_long2_rtz(long2);
long2 __CVAttrs convert_long2_rtz(ulong2);
long2 __CVAttrs convert_long2_rtz(float2);
long2 __CVAttrs convert_long2_rtz(double2);
long2 __CVAttrs convert_long2_rtp(char2);
long2 __CVAttrs convert_long2_rtp(uchar2);
long2 __CVAttrs convert_long2_rtp(short2);
long2 __CVAttrs convert_long2_rtp(ushort2);
long2 __CVAttrs convert_long2_rtp(int2);
long2 __CVAttrs convert_long2_rtp(uint2);
long2 __CVAttrs convert_long2_rtp(long2);
long2 __CVAttrs convert_long2_rtp(ulong2);
long2 __CVAttrs convert_long2_rtp(float2);
long2 __CVAttrs convert_long2_rtp(double2);
long2 __CVAttrs convert_long2_rtn(char2);
long2 __CVAttrs convert_long2_rtn(uchar2);
long2 __CVAttrs convert_long2_rtn(short2);
long2 __CVAttrs convert_long2_rtn(ushort2);
long2 __CVAttrs convert_long2_rtn(int2);
long2 __CVAttrs convert_long2_rtn(uint2);
long2 __CVAttrs convert_long2_rtn(long2);
long2 __CVAttrs convert_long2_rtn(ulong2);
long2 __CVAttrs convert_long2_rtn(float2);
long2 __CVAttrs convert_long2_rtn(double2);
long2 __CVAttrs convert_long2_sat(char2);
long2 __CVAttrs convert_long2_sat(uchar2);
long2 __CVAttrs convert_long2_sat(short2);
long2 __CVAttrs convert_long2_sat(ushort2);
long2 __CVAttrs convert_long2_sat(int2);
long2 __CVAttrs convert_long2_sat(uint2);
long2 __CVAttrs convert_long2_sat(long2);
long2 __CVAttrs convert_long2_sat(ulong2);
long2 __CVAttrs convert_long2_sat(float2);
long2 __CVAttrs convert_long2_sat(double2);
long2 __CVAttrs convert_long2_sat_rte(char2);
long2 __CVAttrs convert_long2_sat_rte(uchar2);
long2 __CVAttrs convert_long2_sat_rte(short2);
long2 __CVAttrs convert_long2_sat_rte(ushort2);
long2 __CVAttrs convert_long2_sat_rte(int2);
long2 __CVAttrs convert_long2_sat_rte(uint2);
long2 __CVAttrs convert_long2_sat_rte(long2);
long2 __CVAttrs convert_long2_sat_rte(ulong2);
long2 __CVAttrs convert_long2_sat_rte(float2);
long2 __CVAttrs convert_long2_sat_rte(double2);
long2 __CVAttrs convert_long2_sat_rtz(char2);
long2 __CVAttrs convert_long2_sat_rtz(uchar2);
long2 __CVAttrs convert_long2_sat_rtz(short2);
long2 __CVAttrs convert_long2_sat_rtz(ushort2);
long2 __CVAttrs convert_long2_sat_rtz(int2);
long2 __CVAttrs convert_long2_sat_rtz(uint2);
long2 __CVAttrs convert_long2_sat_rtz(long2);
long2 __CVAttrs convert_long2_sat_rtz(ulong2);
long2 __CVAttrs convert_long2_sat_rtz(float2);
long2 __CVAttrs convert_long2_sat_rtz(double2);
long2 __CVAttrs convert_long2_sat_rtp(char2);
long2 __CVAttrs convert_long2_sat_rtp(uchar2);
long2 __CVAttrs convert_long2_sat_rtp(short2);
long2 __CVAttrs convert_long2_sat_rtp(ushort2);
long2 __CVAttrs convert_long2_sat_rtp(int2);
long2 __CVAttrs convert_long2_sat_rtp(uint2);
long2 __CVAttrs convert_long2_sat_rtp(long2);
long2 __CVAttrs convert_long2_sat_rtp(ulong2);
long2 __CVAttrs convert_long2_sat_rtp(float2);
long2 __CVAttrs convert_long2_sat_rtp(double2);
long2 __CVAttrs convert_long2_sat_rtn(char2);
long2 __CVAttrs convert_long2_sat_rtn(uchar2);
long2 __CVAttrs convert_long2_sat_rtn(short2);
long2 __CVAttrs convert_long2_sat_rtn(ushort2);
long2 __CVAttrs convert_long2_sat_rtn(int2);
long2 __CVAttrs convert_long2_sat_rtn(uint2);
long2 __CVAttrs convert_long2_sat_rtn(long2);
long2 __CVAttrs convert_long2_sat_rtn(ulong2);
long2 __CVAttrs convert_long2_sat_rtn(float2);
long2 __CVAttrs convert_long2_sat_rtn(double2);
#if __OPENCL_C_VERSION__ >= 110
long3 __CVAttrs convert_long3(char3);
long3 __CVAttrs convert_long3(uchar3);
long3 __CVAttrs convert_long3(short3);
long3 __CVAttrs convert_long3(ushort3);
long3 __CVAttrs convert_long3(int3);
long3 __CVAttrs convert_long3(uint3);
long3 __CVAttrs convert_long3(long3);
long3 __CVAttrs convert_long3(ulong3);
long3 __CVAttrs convert_long3(float3);
long3 __CVAttrs convert_long3(double3);
long3 __CVAttrs convert_long3_rte(char3);
long3 __CVAttrs convert_long3_rte(uchar3);
long3 __CVAttrs convert_long3_rte(short3);
long3 __CVAttrs convert_long3_rte(ushort3);
long3 __CVAttrs convert_long3_rte(int3);
long3 __CVAttrs convert_long3_rte(uint3);
long3 __CVAttrs convert_long3_rte(long3);
long3 __CVAttrs convert_long3_rte(ulong3);
long3 __CVAttrs convert_long3_rte(float3);
long3 __CVAttrs convert_long3_rte(double3);
long3 __CVAttrs convert_long3_rtz(char3);
long3 __CVAttrs convert_long3_rtz(uchar3);
long3 __CVAttrs convert_long3_rtz(short3);
long3 __CVAttrs convert_long3_rtz(ushort3);
long3 __CVAttrs convert_long3_rtz(int3);
long3 __CVAttrs convert_long3_rtz(uint3);
long3 __CVAttrs convert_long3_rtz(long3);
long3 __CVAttrs convert_long3_rtz(ulong3);
long3 __CVAttrs convert_long3_rtz(float3);
long3 __CVAttrs convert_long3_rtz(double3);
long3 __CVAttrs convert_long3_rtp(char3);
long3 __CVAttrs convert_long3_rtp(uchar3);
long3 __CVAttrs convert_long3_rtp(short3);
long3 __CVAttrs convert_long3_rtp(ushort3);
long3 __CVAttrs convert_long3_rtp(int3);
long3 __CVAttrs convert_long3_rtp(uint3);
long3 __CVAttrs convert_long3_rtp(long3);
long3 __CVAttrs convert_long3_rtp(ulong3);
long3 __CVAttrs convert_long3_rtp(float3);
long3 __CVAttrs convert_long3_rtp(double3);
long3 __CVAttrs convert_long3_rtn(char3);
long3 __CVAttrs convert_long3_rtn(uchar3);
long3 __CVAttrs convert_long3_rtn(short3);
long3 __CVAttrs convert_long3_rtn(ushort3);
long3 __CVAttrs convert_long3_rtn(int3);
long3 __CVAttrs convert_long3_rtn(uint3);
long3 __CVAttrs convert_long3_rtn(long3);
long3 __CVAttrs convert_long3_rtn(ulong3);
long3 __CVAttrs convert_long3_rtn(float3);
long3 __CVAttrs convert_long3_rtn(double3);
long3 __CVAttrs convert_long3_sat(char3);
long3 __CVAttrs convert_long3_sat(uchar3);
long3 __CVAttrs convert_long3_sat(short3);
long3 __CVAttrs convert_long3_sat(ushort3);
long3 __CVAttrs convert_long3_sat(int3);
long3 __CVAttrs convert_long3_sat(uint3);
long3 __CVAttrs convert_long3_sat(long3);
long3 __CVAttrs convert_long3_sat(ulong3);
long3 __CVAttrs convert_long3_sat(float3);
long3 __CVAttrs convert_long3_sat(double3);
long3 __CVAttrs convert_long3_sat_rte(char3);
long3 __CVAttrs convert_long3_sat_rte(uchar3);
long3 __CVAttrs convert_long3_sat_rte(short3);
long3 __CVAttrs convert_long3_sat_rte(ushort3);
long3 __CVAttrs convert_long3_sat_rte(int3);
long3 __CVAttrs convert_long3_sat_rte(uint3);
long3 __CVAttrs convert_long3_sat_rte(long3);
long3 __CVAttrs convert_long3_sat_rte(ulong3);
long3 __CVAttrs convert_long3_sat_rte(float3);
long3 __CVAttrs convert_long3_sat_rte(double3);
long3 __CVAttrs convert_long3_sat_rtz(char3);
long3 __CVAttrs convert_long3_sat_rtz(uchar3);
long3 __CVAttrs convert_long3_sat_rtz(short3);
long3 __CVAttrs convert_long3_sat_rtz(ushort3);
long3 __CVAttrs convert_long3_sat_rtz(int3);
long3 __CVAttrs convert_long3_sat_rtz(uint3);
long3 __CVAttrs convert_long3_sat_rtz(long3);
long3 __CVAttrs convert_long3_sat_rtz(ulong3);
long3 __CVAttrs convert_long3_sat_rtz(float3);
long3 __CVAttrs convert_long3_sat_rtz(double3);
long3 __CVAttrs convert_long3_sat_rtp(char3);
long3 __CVAttrs convert_long3_sat_rtp(uchar3);
long3 __CVAttrs convert_long3_sat_rtp(short3);
long3 __CVAttrs convert_long3_sat_rtp(ushort3);
long3 __CVAttrs convert_long3_sat_rtp(int3);
long3 __CVAttrs convert_long3_sat_rtp(uint3);
long3 __CVAttrs convert_long3_sat_rtp(long3);
long3 __CVAttrs convert_long3_sat_rtp(ulong3);
long3 __CVAttrs convert_long3_sat_rtp(float3);
long3 __CVAttrs convert_long3_sat_rtp(double3);
long3 __CVAttrs convert_long3_sat_rtn(char3);
long3 __CVAttrs convert_long3_sat_rtn(uchar3);
long3 __CVAttrs convert_long3_sat_rtn(short3);
long3 __CVAttrs convert_long3_sat_rtn(ushort3);
long3 __CVAttrs convert_long3_sat_rtn(int3);
long3 __CVAttrs convert_long3_sat_rtn(uint3);
long3 __CVAttrs convert_long3_sat_rtn(long3);
long3 __CVAttrs convert_long3_sat_rtn(ulong3);
long3 __CVAttrs convert_long3_sat_rtn(float3);
long3 __CVAttrs convert_long3_sat_rtn(double3);
#endif
long4 __CVAttrs convert_long4(char4);
long4 __CVAttrs convert_long4(uchar4);
long4 __CVAttrs convert_long4(short4);
long4 __CVAttrs convert_long4(ushort4);
long4 __CVAttrs convert_long4(int4);
long4 __CVAttrs convert_long4(uint4);
long4 __CVAttrs convert_long4(long4);
long4 __CVAttrs convert_long4(ulong4);
long4 __CVAttrs convert_long4(float4);
long4 __CVAttrs convert_long4(double4);
long4 __CVAttrs convert_long4_rte(char4);
long4 __CVAttrs convert_long4_rte(uchar4);
long4 __CVAttrs convert_long4_rte(short4);
long4 __CVAttrs convert_long4_rte(ushort4);
long4 __CVAttrs convert_long4_rte(int4);
long4 __CVAttrs convert_long4_rte(uint4);
long4 __CVAttrs convert_long4_rte(long4);
long4 __CVAttrs convert_long4_rte(ulong4);
long4 __CVAttrs convert_long4_rte(float4);
long4 __CVAttrs convert_long4_rte(double4);
long4 __CVAttrs convert_long4_rtz(char4);
long4 __CVAttrs convert_long4_rtz(uchar4);
long4 __CVAttrs convert_long4_rtz(short4);
long4 __CVAttrs convert_long4_rtz(ushort4);
long4 __CVAttrs convert_long4_rtz(int4);
long4 __CVAttrs convert_long4_rtz(uint4);
long4 __CVAttrs convert_long4_rtz(long4);
long4 __CVAttrs convert_long4_rtz(ulong4);
long4 __CVAttrs convert_long4_rtz(float4);
long4 __CVAttrs convert_long4_rtz(double4);
long4 __CVAttrs convert_long4_rtp(char4);
long4 __CVAttrs convert_long4_rtp(uchar4);
long4 __CVAttrs convert_long4_rtp(short4);
long4 __CVAttrs convert_long4_rtp(ushort4);
long4 __CVAttrs convert_long4_rtp(int4);
long4 __CVAttrs convert_long4_rtp(uint4);
long4 __CVAttrs convert_long4_rtp(long4);
long4 __CVAttrs convert_long4_rtp(ulong4);
long4 __CVAttrs convert_long4_rtp(float4);
long4 __CVAttrs convert_long4_rtp(double4);
long4 __CVAttrs convert_long4_rtn(char4);
long4 __CVAttrs convert_long4_rtn(uchar4);
long4 __CVAttrs convert_long4_rtn(short4);
long4 __CVAttrs convert_long4_rtn(ushort4);
long4 __CVAttrs convert_long4_rtn(int4);
long4 __CVAttrs convert_long4_rtn(uint4);
long4 __CVAttrs convert_long4_rtn(long4);
long4 __CVAttrs convert_long4_rtn(ulong4);
long4 __CVAttrs convert_long4_rtn(float4);
long4 __CVAttrs convert_long4_rtn(double4);
long4 __CVAttrs convert_long4_sat(char4);
long4 __CVAttrs convert_long4_sat(uchar4);
long4 __CVAttrs convert_long4_sat(short4);
long4 __CVAttrs convert_long4_sat(ushort4);
long4 __CVAttrs convert_long4_sat(int4);
long4 __CVAttrs convert_long4_sat(uint4);
long4 __CVAttrs convert_long4_sat(long4);
long4 __CVAttrs convert_long4_sat(ulong4);
long4 __CVAttrs convert_long4_sat(float4);
long4 __CVAttrs convert_long4_sat(double4);
long4 __CVAttrs convert_long4_sat_rte(char4);
long4 __CVAttrs convert_long4_sat_rte(uchar4);
long4 __CVAttrs convert_long4_sat_rte(short4);
long4 __CVAttrs convert_long4_sat_rte(ushort4);
long4 __CVAttrs convert_long4_sat_rte(int4);
long4 __CVAttrs convert_long4_sat_rte(uint4);
long4 __CVAttrs convert_long4_sat_rte(long4);
long4 __CVAttrs convert_long4_sat_rte(ulong4);
long4 __CVAttrs convert_long4_sat_rte(float4);
long4 __CVAttrs convert_long4_sat_rte(double4);
long4 __CVAttrs convert_long4_sat_rtz(char4);
long4 __CVAttrs convert_long4_sat_rtz(uchar4);
long4 __CVAttrs convert_long4_sat_rtz(short4);
long4 __CVAttrs convert_long4_sat_rtz(ushort4);
long4 __CVAttrs convert_long4_sat_rtz(int4);
long4 __CVAttrs convert_long4_sat_rtz(uint4);
long4 __CVAttrs convert_long4_sat_rtz(long4);
long4 __CVAttrs convert_long4_sat_rtz(ulong4);
long4 __CVAttrs convert_long4_sat_rtz(float4);
long4 __CVAttrs convert_long4_sat_rtz(double4);
long4 __CVAttrs convert_long4_sat_rtp(char4);
long4 __CVAttrs convert_long4_sat_rtp(uchar4);
long4 __CVAttrs convert_long4_sat_rtp(short4);
long4 __CVAttrs convert_long4_sat_rtp(ushort4);
long4 __CVAttrs convert_long4_sat_rtp(int4);
long4 __CVAttrs convert_long4_sat_rtp(uint4);
long4 __CVAttrs convert_long4_sat_rtp(long4);
long4 __CVAttrs convert_long4_sat_rtp(ulong4);
long4 __CVAttrs convert_long4_sat_rtp(float4);
long4 __CVAttrs convert_long4_sat_rtp(double4);
long4 __CVAttrs convert_long4_sat_rtn(char4);
long4 __CVAttrs convert_long4_sat_rtn(uchar4);
long4 __CVAttrs convert_long4_sat_rtn(short4);
long4 __CVAttrs convert_long4_sat_rtn(ushort4);
long4 __CVAttrs convert_long4_sat_rtn(int4);
long4 __CVAttrs convert_long4_sat_rtn(uint4);
long4 __CVAttrs convert_long4_sat_rtn(long4);
long4 __CVAttrs convert_long4_sat_rtn(ulong4);
long4 __CVAttrs convert_long4_sat_rtn(float4);
long4 __CVAttrs convert_long4_sat_rtn(double4);
long8 __CVAttrs convert_long8(char8);
long8 __CVAttrs convert_long8(uchar8);
long8 __CVAttrs convert_long8(short8);
long8 __CVAttrs convert_long8(ushort8);
long8 __CVAttrs convert_long8(int8);
long8 __CVAttrs convert_long8(uint8);
long8 __CVAttrs convert_long8(long8);
long8 __CVAttrs convert_long8(ulong8);
long8 __CVAttrs convert_long8(float8);
long8 __CVAttrs convert_long8(double8);
long8 __CVAttrs convert_long8_rte(char8);
long8 __CVAttrs convert_long8_rte(uchar8);
long8 __CVAttrs convert_long8_rte(short8);
long8 __CVAttrs convert_long8_rte(ushort8);
long8 __CVAttrs convert_long8_rte(int8);
long8 __CVAttrs convert_long8_rte(uint8);
long8 __CVAttrs convert_long8_rte(long8);
long8 __CVAttrs convert_long8_rte(ulong8);
long8 __CVAttrs convert_long8_rte(float8);
long8 __CVAttrs convert_long8_rte(double8);
long8 __CVAttrs convert_long8_rtz(char8);
long8 __CVAttrs convert_long8_rtz(uchar8);
long8 __CVAttrs convert_long8_rtz(short8);
long8 __CVAttrs convert_long8_rtz(ushort8);
long8 __CVAttrs convert_long8_rtz(int8);
long8 __CVAttrs convert_long8_rtz(uint8);
long8 __CVAttrs convert_long8_rtz(long8);
long8 __CVAttrs convert_long8_rtz(ulong8);
long8 __CVAttrs convert_long8_rtz(float8);
long8 __CVAttrs convert_long8_rtz(double8);
long8 __CVAttrs convert_long8_rtp(char8);
long8 __CVAttrs convert_long8_rtp(uchar8);
long8 __CVAttrs convert_long8_rtp(short8);
long8 __CVAttrs convert_long8_rtp(ushort8);
long8 __CVAttrs convert_long8_rtp(int8);
long8 __CVAttrs convert_long8_rtp(uint8);
long8 __CVAttrs convert_long8_rtp(long8);
long8 __CVAttrs convert_long8_rtp(ulong8);
long8 __CVAttrs convert_long8_rtp(float8);
long8 __CVAttrs convert_long8_rtp(double8);
long8 __CVAttrs convert_long8_rtn(char8);
long8 __CVAttrs convert_long8_rtn(uchar8);
long8 __CVAttrs convert_long8_rtn(short8);
long8 __CVAttrs convert_long8_rtn(ushort8);
long8 __CVAttrs convert_long8_rtn(int8);
long8 __CVAttrs convert_long8_rtn(uint8);
long8 __CVAttrs convert_long8_rtn(long8);
long8 __CVAttrs convert_long8_rtn(ulong8);
long8 __CVAttrs convert_long8_rtn(float8);
long8 __CVAttrs convert_long8_rtn(double8);
long8 __CVAttrs convert_long8_sat(char8);
long8 __CVAttrs convert_long8_sat(uchar8);
long8 __CVAttrs convert_long8_sat(short8);
long8 __CVAttrs convert_long8_sat(ushort8);
long8 __CVAttrs convert_long8_sat(int8);
long8 __CVAttrs convert_long8_sat(uint8);
long8 __CVAttrs convert_long8_sat(long8);
long8 __CVAttrs convert_long8_sat(ulong8);
long8 __CVAttrs convert_long8_sat(float8);
long8 __CVAttrs convert_long8_sat(double8);
long8 __CVAttrs convert_long8_sat_rte(char8);
long8 __CVAttrs convert_long8_sat_rte(uchar8);
long8 __CVAttrs convert_long8_sat_rte(short8);
long8 __CVAttrs convert_long8_sat_rte(ushort8);
long8 __CVAttrs convert_long8_sat_rte(int8);
long8 __CVAttrs convert_long8_sat_rte(uint8);
long8 __CVAttrs convert_long8_sat_rte(long8);
long8 __CVAttrs convert_long8_sat_rte(ulong8);
long8 __CVAttrs convert_long8_sat_rte(float8);
long8 __CVAttrs convert_long8_sat_rte(double8);
long8 __CVAttrs convert_long8_sat_rtz(char8);
long8 __CVAttrs convert_long8_sat_rtz(uchar8);
long8 __CVAttrs convert_long8_sat_rtz(short8);
long8 __CVAttrs convert_long8_sat_rtz(ushort8);
long8 __CVAttrs convert_long8_sat_rtz(int8);
long8 __CVAttrs convert_long8_sat_rtz(uint8);
long8 __CVAttrs convert_long8_sat_rtz(long8);
long8 __CVAttrs convert_long8_sat_rtz(ulong8);
long8 __CVAttrs convert_long8_sat_rtz(float8);
long8 __CVAttrs convert_long8_sat_rtz(double8);
long8 __CVAttrs convert_long8_sat_rtp(char8);
long8 __CVAttrs convert_long8_sat_rtp(uchar8);
long8 __CVAttrs convert_long8_sat_rtp(short8);
long8 __CVAttrs convert_long8_sat_rtp(ushort8);
long8 __CVAttrs convert_long8_sat_rtp(int8);
long8 __CVAttrs convert_long8_sat_rtp(uint8);
long8 __CVAttrs convert_long8_sat_rtp(long8);
long8 __CVAttrs convert_long8_sat_rtp(ulong8);
long8 __CVAttrs convert_long8_sat_rtp(float8);
long8 __CVAttrs convert_long8_sat_rtp(double8);
long8 __CVAttrs convert_long8_sat_rtn(char8);
long8 __CVAttrs convert_long8_sat_rtn(uchar8);
long8 __CVAttrs convert_long8_sat_rtn(short8);
long8 __CVAttrs convert_long8_sat_rtn(ushort8);
long8 __CVAttrs convert_long8_sat_rtn(int8);
long8 __CVAttrs convert_long8_sat_rtn(uint8);
long8 __CVAttrs convert_long8_sat_rtn(long8);
long8 __CVAttrs convert_long8_sat_rtn(ulong8);
long8 __CVAttrs convert_long8_sat_rtn(float8);
long8 __CVAttrs convert_long8_sat_rtn(double8);
long16 __CVAttrs convert_long16(char16);
long16 __CVAttrs convert_long16(uchar16);
long16 __CVAttrs convert_long16(short16);
long16 __CVAttrs convert_long16(ushort16);
long16 __CVAttrs convert_long16(int16);
long16 __CVAttrs convert_long16(uint16);
long16 __CVAttrs convert_long16(long16);
long16 __CVAttrs convert_long16(ulong16);
long16 __CVAttrs convert_long16(float16);
long16 __CVAttrs convert_long16(double16);
long16 __CVAttrs convert_long16_rte(char16);
long16 __CVAttrs convert_long16_rte(uchar16);
long16 __CVAttrs convert_long16_rte(short16);
long16 __CVAttrs convert_long16_rte(ushort16);
long16 __CVAttrs convert_long16_rte(int16);
long16 __CVAttrs convert_long16_rte(uint16);
long16 __CVAttrs convert_long16_rte(long16);
long16 __CVAttrs convert_long16_rte(ulong16);
long16 __CVAttrs convert_long16_rte(float16);
long16 __CVAttrs convert_long16_rte(double16);
long16 __CVAttrs convert_long16_rtz(char16);
long16 __CVAttrs convert_long16_rtz(uchar16);
long16 __CVAttrs convert_long16_rtz(short16);
long16 __CVAttrs convert_long16_rtz(ushort16);
long16 __CVAttrs convert_long16_rtz(int16);
long16 __CVAttrs convert_long16_rtz(uint16);
long16 __CVAttrs convert_long16_rtz(long16);
long16 __CVAttrs convert_long16_rtz(ulong16);
long16 __CVAttrs convert_long16_rtz(float16);
long16 __CVAttrs convert_long16_rtz(double16);
long16 __CVAttrs convert_long16_rtp(char16);
long16 __CVAttrs convert_long16_rtp(uchar16);
long16 __CVAttrs convert_long16_rtp(short16);
long16 __CVAttrs convert_long16_rtp(ushort16);
long16 __CVAttrs convert_long16_rtp(int16);
long16 __CVAttrs convert_long16_rtp(uint16);
long16 __CVAttrs convert_long16_rtp(long16);
long16 __CVAttrs convert_long16_rtp(ulong16);
long16 __CVAttrs convert_long16_rtp(float16);
long16 __CVAttrs convert_long16_rtp(double16);
long16 __CVAttrs convert_long16_rtn(char16);
long16 __CVAttrs convert_long16_rtn(uchar16);
long16 __CVAttrs convert_long16_rtn(short16);
long16 __CVAttrs convert_long16_rtn(ushort16);
long16 __CVAttrs convert_long16_rtn(int16);
long16 __CVAttrs convert_long16_rtn(uint16);
long16 __CVAttrs convert_long16_rtn(long16);
long16 __CVAttrs convert_long16_rtn(ulong16);
long16 __CVAttrs convert_long16_rtn(float16);
long16 __CVAttrs convert_long16_rtn(double16);
long16 __CVAttrs convert_long16_sat(char16);
long16 __CVAttrs convert_long16_sat(uchar16);
long16 __CVAttrs convert_long16_sat(short16);
long16 __CVAttrs convert_long16_sat(ushort16);
long16 __CVAttrs convert_long16_sat(int16);
long16 __CVAttrs convert_long16_sat(uint16);
long16 __CVAttrs convert_long16_sat(long16);
long16 __CVAttrs convert_long16_sat(ulong16);
long16 __CVAttrs convert_long16_sat(float16);
long16 __CVAttrs convert_long16_sat(double16);
long16 __CVAttrs convert_long16_sat_rte(char16);
long16 __CVAttrs convert_long16_sat_rte(uchar16);
long16 __CVAttrs convert_long16_sat_rte(short16);
long16 __CVAttrs convert_long16_sat_rte(ushort16);
long16 __CVAttrs convert_long16_sat_rte(int16);
long16 __CVAttrs convert_long16_sat_rte(uint16);
long16 __CVAttrs convert_long16_sat_rte(long16);
long16 __CVAttrs convert_long16_sat_rte(ulong16);
long16 __CVAttrs convert_long16_sat_rte(float16);
long16 __CVAttrs convert_long16_sat_rte(double16);
long16 __CVAttrs convert_long16_sat_rtz(char16);
long16 __CVAttrs convert_long16_sat_rtz(uchar16);
long16 __CVAttrs convert_long16_sat_rtz(short16);
long16 __CVAttrs convert_long16_sat_rtz(ushort16);
long16 __CVAttrs convert_long16_sat_rtz(int16);
long16 __CVAttrs convert_long16_sat_rtz(uint16);
long16 __CVAttrs convert_long16_sat_rtz(long16);
long16 __CVAttrs convert_long16_sat_rtz(ulong16);
long16 __CVAttrs convert_long16_sat_rtz(float16);
long16 __CVAttrs convert_long16_sat_rtz(double16);
long16 __CVAttrs convert_long16_sat_rtp(char16);
long16 __CVAttrs convert_long16_sat_rtp(uchar16);
long16 __CVAttrs convert_long16_sat_rtp(short16);
long16 __CVAttrs convert_long16_sat_rtp(ushort16);
long16 __CVAttrs convert_long16_sat_rtp(int16);
long16 __CVAttrs convert_long16_sat_rtp(uint16);
long16 __CVAttrs convert_long16_sat_rtp(long16);
long16 __CVAttrs convert_long16_sat_rtp(ulong16);
long16 __CVAttrs convert_long16_sat_rtp(float16);
long16 __CVAttrs convert_long16_sat_rtp(double16);
long16 __CVAttrs convert_long16_sat_rtn(char16);
long16 __CVAttrs convert_long16_sat_rtn(uchar16);
long16 __CVAttrs convert_long16_sat_rtn(short16);
long16 __CVAttrs convert_long16_sat_rtn(ushort16);
long16 __CVAttrs convert_long16_sat_rtn(int16);
long16 __CVAttrs convert_long16_sat_rtn(uint16);
long16 __CVAttrs convert_long16_sat_rtn(long16);
long16 __CVAttrs convert_long16_sat_rtn(ulong16);
long16 __CVAttrs convert_long16_sat_rtn(float16);
long16 __CVAttrs convert_long16_sat_rtn(double16);
ulong __CVAttrs convert_ulong(char);
ulong __CVAttrs convert_ulong(uchar);
ulong __CVAttrs convert_ulong(short);
ulong __CVAttrs convert_ulong(ushort);
ulong __CVAttrs convert_ulong(int);
ulong __CVAttrs convert_ulong(uint);
ulong __CVAttrs convert_ulong(long);
ulong __CVAttrs convert_ulong(ulong);
ulong __CVAttrs convert_ulong(float);
ulong __CVAttrs convert_ulong(double);
ulong __CVAttrs convert_ulong_rte(char);
ulong __CVAttrs convert_ulong_rte(uchar);
ulong __CVAttrs convert_ulong_rte(short);
ulong __CVAttrs convert_ulong_rte(ushort);
ulong __CVAttrs convert_ulong_rte(int);
ulong __CVAttrs convert_ulong_rte(uint);
ulong __CVAttrs convert_ulong_rte(long);
ulong __CVAttrs convert_ulong_rte(ulong);
ulong __CVAttrs convert_ulong_rte(float);
ulong __CVAttrs convert_ulong_rte(double);
ulong __CVAttrs convert_ulong_rtz(char);
ulong __CVAttrs convert_ulong_rtz(uchar);
ulong __CVAttrs convert_ulong_rtz(short);
ulong __CVAttrs convert_ulong_rtz(ushort);
ulong __CVAttrs convert_ulong_rtz(int);
ulong __CVAttrs convert_ulong_rtz(uint);
ulong __CVAttrs convert_ulong_rtz(long);
ulong __CVAttrs convert_ulong_rtz(ulong);
ulong __CVAttrs convert_ulong_rtz(float);
ulong __CVAttrs convert_ulong_rtz(double);
ulong __CVAttrs convert_ulong_rtp(char);
ulong __CVAttrs convert_ulong_rtp(uchar);
ulong __CVAttrs convert_ulong_rtp(short);
ulong __CVAttrs convert_ulong_rtp(ushort);
ulong __CVAttrs convert_ulong_rtp(int);
ulong __CVAttrs convert_ulong_rtp(uint);
ulong __CVAttrs convert_ulong_rtp(long);
ulong __CVAttrs convert_ulong_rtp(ulong);
ulong __CVAttrs convert_ulong_rtp(float);
ulong __CVAttrs convert_ulong_rtp(double);
ulong __CVAttrs convert_ulong_rtn(char);
ulong __CVAttrs convert_ulong_rtn(uchar);
ulong __CVAttrs convert_ulong_rtn(short);
ulong __CVAttrs convert_ulong_rtn(ushort);
ulong __CVAttrs convert_ulong_rtn(int);
ulong __CVAttrs convert_ulong_rtn(uint);
ulong __CVAttrs convert_ulong_rtn(long);
ulong __CVAttrs convert_ulong_rtn(ulong);
ulong __CVAttrs convert_ulong_rtn(float);
ulong __CVAttrs convert_ulong_rtn(double);
ulong __CVAttrs convert_ulong_sat(char);
ulong __CVAttrs convert_ulong_sat(uchar);
ulong __CVAttrs convert_ulong_sat(short);
ulong __CVAttrs convert_ulong_sat(ushort);
ulong __CVAttrs convert_ulong_sat(int);
ulong __CVAttrs convert_ulong_sat(uint);
ulong __CVAttrs convert_ulong_sat(long);
ulong __CVAttrs convert_ulong_sat(ulong);
ulong __CVAttrs convert_ulong_sat(float);
ulong __CVAttrs convert_ulong_sat(double);
ulong __CVAttrs convert_ulong_sat_rte(char);
ulong __CVAttrs convert_ulong_sat_rte(uchar);
ulong __CVAttrs convert_ulong_sat_rte(short);
ulong __CVAttrs convert_ulong_sat_rte(ushort);
ulong __CVAttrs convert_ulong_sat_rte(int);
ulong __CVAttrs convert_ulong_sat_rte(uint);
ulong __CVAttrs convert_ulong_sat_rte(long);
ulong __CVAttrs convert_ulong_sat_rte(ulong);
ulong __CVAttrs convert_ulong_sat_rte(float);
ulong __CVAttrs convert_ulong_sat_rte(double);
ulong __CVAttrs convert_ulong_sat_rtz(char);
ulong __CVAttrs convert_ulong_sat_rtz(uchar);
ulong __CVAttrs convert_ulong_sat_rtz(short);
ulong __CVAttrs convert_ulong_sat_rtz(ushort);
ulong __CVAttrs convert_ulong_sat_rtz(int);
ulong __CVAttrs convert_ulong_sat_rtz(uint);
ulong __CVAttrs convert_ulong_sat_rtz(long);
ulong __CVAttrs convert_ulong_sat_rtz(ulong);
ulong __CVAttrs convert_ulong_sat_rtz(float);
ulong __CVAttrs convert_ulong_sat_rtz(double);
ulong __CVAttrs convert_ulong_sat_rtp(char);
ulong __CVAttrs convert_ulong_sat_rtp(uchar);
ulong __CVAttrs convert_ulong_sat_rtp(short);
ulong __CVAttrs convert_ulong_sat_rtp(ushort);
ulong __CVAttrs convert_ulong_sat_rtp(int);
ulong __CVAttrs convert_ulong_sat_rtp(uint);
ulong __CVAttrs convert_ulong_sat_rtp(long);
ulong __CVAttrs convert_ulong_sat_rtp(ulong);
ulong __CVAttrs convert_ulong_sat_rtp(float);
ulong __CVAttrs convert_ulong_sat_rtp(double);
ulong __CVAttrs convert_ulong_sat_rtn(char);
ulong __CVAttrs convert_ulong_sat_rtn(uchar);
ulong __CVAttrs convert_ulong_sat_rtn(short);
ulong __CVAttrs convert_ulong_sat_rtn(ushort);
ulong __CVAttrs convert_ulong_sat_rtn(int);
ulong __CVAttrs convert_ulong_sat_rtn(uint);
ulong __CVAttrs convert_ulong_sat_rtn(long);
ulong __CVAttrs convert_ulong_sat_rtn(ulong);
ulong __CVAttrs convert_ulong_sat_rtn(float);
ulong __CVAttrs convert_ulong_sat_rtn(double);
ulong2 __CVAttrs convert_ulong2(char2);
ulong2 __CVAttrs convert_ulong2(uchar2);
ulong2 __CVAttrs convert_ulong2(short2);
ulong2 __CVAttrs convert_ulong2(ushort2);
ulong2 __CVAttrs convert_ulong2(int2);
ulong2 __CVAttrs convert_ulong2(uint2);
ulong2 __CVAttrs convert_ulong2(long2);
ulong2 __CVAttrs convert_ulong2(ulong2);
ulong2 __CVAttrs convert_ulong2(float2);
ulong2 __CVAttrs convert_ulong2(double2);
ulong2 __CVAttrs convert_ulong2_rte(char2);
ulong2 __CVAttrs convert_ulong2_rte(uchar2);
ulong2 __CVAttrs convert_ulong2_rte(short2);
ulong2 __CVAttrs convert_ulong2_rte(ushort2);
ulong2 __CVAttrs convert_ulong2_rte(int2);
ulong2 __CVAttrs convert_ulong2_rte(uint2);
ulong2 __CVAttrs convert_ulong2_rte(long2);
ulong2 __CVAttrs convert_ulong2_rte(ulong2);
ulong2 __CVAttrs convert_ulong2_rte(float2);
ulong2 __CVAttrs convert_ulong2_rte(double2);
ulong2 __CVAttrs convert_ulong2_rtz(char2);
ulong2 __CVAttrs convert_ulong2_rtz(uchar2);
ulong2 __CVAttrs convert_ulong2_rtz(short2);
ulong2 __CVAttrs convert_ulong2_rtz(ushort2);
ulong2 __CVAttrs convert_ulong2_rtz(int2);
ulong2 __CVAttrs convert_ulong2_rtz(uint2);
ulong2 __CVAttrs convert_ulong2_rtz(long2);
ulong2 __CVAttrs convert_ulong2_rtz(ulong2);
ulong2 __CVAttrs convert_ulong2_rtz(float2);
ulong2 __CVAttrs convert_ulong2_rtz(double2);
ulong2 __CVAttrs convert_ulong2_rtp(char2);
ulong2 __CVAttrs convert_ulong2_rtp(uchar2);
ulong2 __CVAttrs convert_ulong2_rtp(short2);
ulong2 __CVAttrs convert_ulong2_rtp(ushort2);
ulong2 __CVAttrs convert_ulong2_rtp(int2);
ulong2 __CVAttrs convert_ulong2_rtp(uint2);
ulong2 __CVAttrs convert_ulong2_rtp(long2);
ulong2 __CVAttrs convert_ulong2_rtp(ulong2);
ulong2 __CVAttrs convert_ulong2_rtp(float2);
ulong2 __CVAttrs convert_ulong2_rtp(double2);
ulong2 __CVAttrs convert_ulong2_rtn(char2);
ulong2 __CVAttrs convert_ulong2_rtn(uchar2);
ulong2 __CVAttrs convert_ulong2_rtn(short2);
ulong2 __CVAttrs convert_ulong2_rtn(ushort2);
ulong2 __CVAttrs convert_ulong2_rtn(int2);
ulong2 __CVAttrs convert_ulong2_rtn(uint2);
ulong2 __CVAttrs convert_ulong2_rtn(long2);
ulong2 __CVAttrs convert_ulong2_rtn(ulong2);
ulong2 __CVAttrs convert_ulong2_rtn(float2);
ulong2 __CVAttrs convert_ulong2_rtn(double2);
ulong2 __CVAttrs convert_ulong2_sat(char2);
ulong2 __CVAttrs convert_ulong2_sat(uchar2);
ulong2 __CVAttrs convert_ulong2_sat(short2);
ulong2 __CVAttrs convert_ulong2_sat(ushort2);
ulong2 __CVAttrs convert_ulong2_sat(int2);
ulong2 __CVAttrs convert_ulong2_sat(uint2);
ulong2 __CVAttrs convert_ulong2_sat(long2);
ulong2 __CVAttrs convert_ulong2_sat(ulong2);
ulong2 __CVAttrs convert_ulong2_sat(float2);
ulong2 __CVAttrs convert_ulong2_sat(double2);
ulong2 __CVAttrs convert_ulong2_sat_rte(char2);
ulong2 __CVAttrs convert_ulong2_sat_rte(uchar2);
ulong2 __CVAttrs convert_ulong2_sat_rte(short2);
ulong2 __CVAttrs convert_ulong2_sat_rte(ushort2);
ulong2 __CVAttrs convert_ulong2_sat_rte(int2);
ulong2 __CVAttrs convert_ulong2_sat_rte(uint2);
ulong2 __CVAttrs convert_ulong2_sat_rte(long2);
ulong2 __CVAttrs convert_ulong2_sat_rte(ulong2);
ulong2 __CVAttrs convert_ulong2_sat_rte(float2);
ulong2 __CVAttrs convert_ulong2_sat_rte(double2);
ulong2 __CVAttrs convert_ulong2_sat_rtz(char2);
ulong2 __CVAttrs convert_ulong2_sat_rtz(uchar2);
ulong2 __CVAttrs convert_ulong2_sat_rtz(short2);
ulong2 __CVAttrs convert_ulong2_sat_rtz(ushort2);
ulong2 __CVAttrs convert_ulong2_sat_rtz(int2);
ulong2 __CVAttrs convert_ulong2_sat_rtz(uint2);
ulong2 __CVAttrs convert_ulong2_sat_rtz(long2);
ulong2 __CVAttrs convert_ulong2_sat_rtz(ulong2);
ulong2 __CVAttrs convert_ulong2_sat_rtz(float2);
ulong2 __CVAttrs convert_ulong2_sat_rtz(double2);
ulong2 __CVAttrs convert_ulong2_sat_rtp(char2);
ulong2 __CVAttrs convert_ulong2_sat_rtp(uchar2);
ulong2 __CVAttrs convert_ulong2_sat_rtp(short2);
ulong2 __CVAttrs convert_ulong2_sat_rtp(ushort2);
ulong2 __CVAttrs convert_ulong2_sat_rtp(int2);
ulong2 __CVAttrs convert_ulong2_sat_rtp(uint2);
ulong2 __CVAttrs convert_ulong2_sat_rtp(long2);
ulong2 __CVAttrs convert_ulong2_sat_rtp(ulong2);
ulong2 __CVAttrs convert_ulong2_sat_rtp(float2);
ulong2 __CVAttrs convert_ulong2_sat_rtp(double2);
ulong2 __CVAttrs convert_ulong2_sat_rtn(char2);
ulong2 __CVAttrs convert_ulong2_sat_rtn(uchar2);
ulong2 __CVAttrs convert_ulong2_sat_rtn(short2);
ulong2 __CVAttrs convert_ulong2_sat_rtn(ushort2);
ulong2 __CVAttrs convert_ulong2_sat_rtn(int2);
ulong2 __CVAttrs convert_ulong2_sat_rtn(uint2);
ulong2 __CVAttrs convert_ulong2_sat_rtn(long2);
ulong2 __CVAttrs convert_ulong2_sat_rtn(ulong2);
ulong2 __CVAttrs convert_ulong2_sat_rtn(float2);
ulong2 __CVAttrs convert_ulong2_sat_rtn(double2);
#if __OPENCL_C_VERSION__ >= 110
ulong3 __CVAttrs convert_ulong3(char3);
ulong3 __CVAttrs convert_ulong3(uchar3);
ulong3 __CVAttrs convert_ulong3(short3);
ulong3 __CVAttrs convert_ulong3(ushort3);
ulong3 __CVAttrs convert_ulong3(int3);
ulong3 __CVAttrs convert_ulong3(uint3);
ulong3 __CVAttrs convert_ulong3(long3);
ulong3 __CVAttrs convert_ulong3(ulong3);
ulong3 __CVAttrs convert_ulong3(float3);
ulong3 __CVAttrs convert_ulong3(double3);
ulong3 __CVAttrs convert_ulong3_rte(char3);
ulong3 __CVAttrs convert_ulong3_rte(uchar3);
ulong3 __CVAttrs convert_ulong3_rte(short3);
ulong3 __CVAttrs convert_ulong3_rte(ushort3);
ulong3 __CVAttrs convert_ulong3_rte(int3);
ulong3 __CVAttrs convert_ulong3_rte(uint3);
ulong3 __CVAttrs convert_ulong3_rte(long3);
ulong3 __CVAttrs convert_ulong3_rte(ulong3);
ulong3 __CVAttrs convert_ulong3_rte(float3);
ulong3 __CVAttrs convert_ulong3_rte(double3);
ulong3 __CVAttrs convert_ulong3_rtz(char3);
ulong3 __CVAttrs convert_ulong3_rtz(uchar3);
ulong3 __CVAttrs convert_ulong3_rtz(short3);
ulong3 __CVAttrs convert_ulong3_rtz(ushort3);
ulong3 __CVAttrs convert_ulong3_rtz(int3);
ulong3 __CVAttrs convert_ulong3_rtz(uint3);
ulong3 __CVAttrs convert_ulong3_rtz(long3);
ulong3 __CVAttrs convert_ulong3_rtz(ulong3);
ulong3 __CVAttrs convert_ulong3_rtz(float3);
ulong3 __CVAttrs convert_ulong3_rtz(double3);
ulong3 __CVAttrs convert_ulong3_rtp(char3);
ulong3 __CVAttrs convert_ulong3_rtp(uchar3);
ulong3 __CVAttrs convert_ulong3_rtp(short3);
ulong3 __CVAttrs convert_ulong3_rtp(ushort3);
ulong3 __CVAttrs convert_ulong3_rtp(int3);
ulong3 __CVAttrs convert_ulong3_rtp(uint3);
ulong3 __CVAttrs convert_ulong3_rtp(long3);
ulong3 __CVAttrs convert_ulong3_rtp(ulong3);
ulong3 __CVAttrs convert_ulong3_rtp(float3);
ulong3 __CVAttrs convert_ulong3_rtp(double3);
ulong3 __CVAttrs convert_ulong3_rtn(char3);
ulong3 __CVAttrs convert_ulong3_rtn(uchar3);
ulong3 __CVAttrs convert_ulong3_rtn(short3);
ulong3 __CVAttrs convert_ulong3_rtn(ushort3);
ulong3 __CVAttrs convert_ulong3_rtn(int3);
ulong3 __CVAttrs convert_ulong3_rtn(uint3);
ulong3 __CVAttrs convert_ulong3_rtn(long3);
ulong3 __CVAttrs convert_ulong3_rtn(ulong3);
ulong3 __CVAttrs convert_ulong3_rtn(float3);
ulong3 __CVAttrs convert_ulong3_rtn(double3);
ulong3 __CVAttrs convert_ulong3_sat(char3);
ulong3 __CVAttrs convert_ulong3_sat(uchar3);
ulong3 __CVAttrs convert_ulong3_sat(short3);
ulong3 __CVAttrs convert_ulong3_sat(ushort3);
ulong3 __CVAttrs convert_ulong3_sat(int3);
ulong3 __CVAttrs convert_ulong3_sat(uint3);
ulong3 __CVAttrs convert_ulong3_sat(long3);
ulong3 __CVAttrs convert_ulong3_sat(ulong3);
ulong3 __CVAttrs convert_ulong3_sat(float3);
ulong3 __CVAttrs convert_ulong3_sat(double3);
ulong3 __CVAttrs convert_ulong3_sat_rte(char3);
ulong3 __CVAttrs convert_ulong3_sat_rte(uchar3);
ulong3 __CVAttrs convert_ulong3_sat_rte(short3);
ulong3 __CVAttrs convert_ulong3_sat_rte(ushort3);
ulong3 __CVAttrs convert_ulong3_sat_rte(int3);
ulong3 __CVAttrs convert_ulong3_sat_rte(uint3);
ulong3 __CVAttrs convert_ulong3_sat_rte(long3);
ulong3 __CVAttrs convert_ulong3_sat_rte(ulong3);
ulong3 __CVAttrs convert_ulong3_sat_rte(float3);
ulong3 __CVAttrs convert_ulong3_sat_rte(double3);
ulong3 __CVAttrs convert_ulong3_sat_rtz(char3);
ulong3 __CVAttrs convert_ulong3_sat_rtz(uchar3);
ulong3 __CVAttrs convert_ulong3_sat_rtz(short3);
ulong3 __CVAttrs convert_ulong3_sat_rtz(ushort3);
ulong3 __CVAttrs convert_ulong3_sat_rtz(int3);
ulong3 __CVAttrs convert_ulong3_sat_rtz(uint3);
ulong3 __CVAttrs convert_ulong3_sat_rtz(long3);
ulong3 __CVAttrs convert_ulong3_sat_rtz(ulong3);
ulong3 __CVAttrs convert_ulong3_sat_rtz(float3);
ulong3 __CVAttrs convert_ulong3_sat_rtz(double3);
ulong3 __CVAttrs convert_ulong3_sat_rtp(char3);
ulong3 __CVAttrs convert_ulong3_sat_rtp(uchar3);
ulong3 __CVAttrs convert_ulong3_sat_rtp(short3);
ulong3 __CVAttrs convert_ulong3_sat_rtp(ushort3);
ulong3 __CVAttrs convert_ulong3_sat_rtp(int3);
ulong3 __CVAttrs convert_ulong3_sat_rtp(uint3);
ulong3 __CVAttrs convert_ulong3_sat_rtp(long3);
ulong3 __CVAttrs convert_ulong3_sat_rtp(ulong3);
ulong3 __CVAttrs convert_ulong3_sat_rtp(float3);
ulong3 __CVAttrs convert_ulong3_sat_rtp(double3);
ulong3 __CVAttrs convert_ulong3_sat_rtn(char3);
ulong3 __CVAttrs convert_ulong3_sat_rtn(uchar3);
ulong3 __CVAttrs convert_ulong3_sat_rtn(short3);
ulong3 __CVAttrs convert_ulong3_sat_rtn(ushort3);
ulong3 __CVAttrs convert_ulong3_sat_rtn(int3);
ulong3 __CVAttrs convert_ulong3_sat_rtn(uint3);
ulong3 __CVAttrs convert_ulong3_sat_rtn(long3);
ulong3 __CVAttrs convert_ulong3_sat_rtn(ulong3);
ulong3 __CVAttrs convert_ulong3_sat_rtn(float3);
ulong3 __CVAttrs convert_ulong3_sat_rtn(double3);
#endif
ulong4 __CVAttrs convert_ulong4(char4);
ulong4 __CVAttrs convert_ulong4(uchar4);
ulong4 __CVAttrs convert_ulong4(short4);
ulong4 __CVAttrs convert_ulong4(ushort4);
ulong4 __CVAttrs convert_ulong4(int4);
ulong4 __CVAttrs convert_ulong4(uint4);
ulong4 __CVAttrs convert_ulong4(long4);
ulong4 __CVAttrs convert_ulong4(ulong4);
ulong4 __CVAttrs convert_ulong4(float4);
ulong4 __CVAttrs convert_ulong4(double4);
ulong4 __CVAttrs convert_ulong4_rte(char4);
ulong4 __CVAttrs convert_ulong4_rte(uchar4);
ulong4 __CVAttrs convert_ulong4_rte(short4);
ulong4 __CVAttrs convert_ulong4_rte(ushort4);
ulong4 __CVAttrs convert_ulong4_rte(int4);
ulong4 __CVAttrs convert_ulong4_rte(uint4);
ulong4 __CVAttrs convert_ulong4_rte(long4);
ulong4 __CVAttrs convert_ulong4_rte(ulong4);
ulong4 __CVAttrs convert_ulong4_rte(float4);
ulong4 __CVAttrs convert_ulong4_rte(double4);
ulong4 __CVAttrs convert_ulong4_rtz(char4);
ulong4 __CVAttrs convert_ulong4_rtz(uchar4);
ulong4 __CVAttrs convert_ulong4_rtz(short4);
ulong4 __CVAttrs convert_ulong4_rtz(ushort4);
ulong4 __CVAttrs convert_ulong4_rtz(int4);
ulong4 __CVAttrs convert_ulong4_rtz(uint4);
ulong4 __CVAttrs convert_ulong4_rtz(long4);
ulong4 __CVAttrs convert_ulong4_rtz(ulong4);
ulong4 __CVAttrs convert_ulong4_rtz(float4);
ulong4 __CVAttrs convert_ulong4_rtz(double4);
ulong4 __CVAttrs convert_ulong4_rtp(char4);
ulong4 __CVAttrs convert_ulong4_rtp(uchar4);
ulong4 __CVAttrs convert_ulong4_rtp(short4);
ulong4 __CVAttrs convert_ulong4_rtp(ushort4);
ulong4 __CVAttrs convert_ulong4_rtp(int4);
ulong4 __CVAttrs convert_ulong4_rtp(uint4);
ulong4 __CVAttrs convert_ulong4_rtp(long4);
ulong4 __CVAttrs convert_ulong4_rtp(ulong4);
ulong4 __CVAttrs convert_ulong4_rtp(float4);
ulong4 __CVAttrs convert_ulong4_rtp(double4);
ulong4 __CVAttrs convert_ulong4_rtn(char4);
ulong4 __CVAttrs convert_ulong4_rtn(uchar4);
ulong4 __CVAttrs convert_ulong4_rtn(short4);
ulong4 __CVAttrs convert_ulong4_rtn(ushort4);
ulong4 __CVAttrs convert_ulong4_rtn(int4);
ulong4 __CVAttrs convert_ulong4_rtn(uint4);
ulong4 __CVAttrs convert_ulong4_rtn(long4);
ulong4 __CVAttrs convert_ulong4_rtn(ulong4);
ulong4 __CVAttrs convert_ulong4_rtn(float4);
ulong4 __CVAttrs convert_ulong4_rtn(double4);
ulong4 __CVAttrs convert_ulong4_sat(char4);
ulong4 __CVAttrs convert_ulong4_sat(uchar4);
ulong4 __CVAttrs convert_ulong4_sat(short4);
ulong4 __CVAttrs convert_ulong4_sat(ushort4);
ulong4 __CVAttrs convert_ulong4_sat(int4);
ulong4 __CVAttrs convert_ulong4_sat(uint4);
ulong4 __CVAttrs convert_ulong4_sat(long4);
ulong4 __CVAttrs convert_ulong4_sat(ulong4);
ulong4 __CVAttrs convert_ulong4_sat(float4);
ulong4 __CVAttrs convert_ulong4_sat(double4);
ulong4 __CVAttrs convert_ulong4_sat_rte(char4);
ulong4 __CVAttrs convert_ulong4_sat_rte(uchar4);
ulong4 __CVAttrs convert_ulong4_sat_rte(short4);
ulong4 __CVAttrs convert_ulong4_sat_rte(ushort4);
ulong4 __CVAttrs convert_ulong4_sat_rte(int4);
ulong4 __CVAttrs convert_ulong4_sat_rte(uint4);
ulong4 __CVAttrs convert_ulong4_sat_rte(long4);
ulong4 __CVAttrs convert_ulong4_sat_rte(ulong4);
ulong4 __CVAttrs convert_ulong4_sat_rte(float4);
ulong4 __CVAttrs convert_ulong4_sat_rte(double4);
ulong4 __CVAttrs convert_ulong4_sat_rtz(char4);
ulong4 __CVAttrs convert_ulong4_sat_rtz(uchar4);
ulong4 __CVAttrs convert_ulong4_sat_rtz(short4);
ulong4 __CVAttrs convert_ulong4_sat_rtz(ushort4);
ulong4 __CVAttrs convert_ulong4_sat_rtz(int4);
ulong4 __CVAttrs convert_ulong4_sat_rtz(uint4);
ulong4 __CVAttrs convert_ulong4_sat_rtz(long4);
ulong4 __CVAttrs convert_ulong4_sat_rtz(ulong4);
ulong4 __CVAttrs convert_ulong4_sat_rtz(float4);
ulong4 __CVAttrs convert_ulong4_sat_rtz(double4);
ulong4 __CVAttrs convert_ulong4_sat_rtp(char4);
ulong4 __CVAttrs convert_ulong4_sat_rtp(uchar4);
ulong4 __CVAttrs convert_ulong4_sat_rtp(short4);
ulong4 __CVAttrs convert_ulong4_sat_rtp(ushort4);
ulong4 __CVAttrs convert_ulong4_sat_rtp(int4);
ulong4 __CVAttrs convert_ulong4_sat_rtp(uint4);
ulong4 __CVAttrs convert_ulong4_sat_rtp(long4);
ulong4 __CVAttrs convert_ulong4_sat_rtp(ulong4);
ulong4 __CVAttrs convert_ulong4_sat_rtp(float4);
ulong4 __CVAttrs convert_ulong4_sat_rtp(double4);
ulong4 __CVAttrs convert_ulong4_sat_rtn(char4);
ulong4 __CVAttrs convert_ulong4_sat_rtn(uchar4);
ulong4 __CVAttrs convert_ulong4_sat_rtn(short4);
ulong4 __CVAttrs convert_ulong4_sat_rtn(ushort4);
ulong4 __CVAttrs convert_ulong4_sat_rtn(int4);
ulong4 __CVAttrs convert_ulong4_sat_rtn(uint4);
ulong4 __CVAttrs convert_ulong4_sat_rtn(long4);
ulong4 __CVAttrs convert_ulong4_sat_rtn(ulong4);
ulong4 __CVAttrs convert_ulong4_sat_rtn(float4);
ulong4 __CVAttrs convert_ulong4_sat_rtn(double4);
ulong8 __CVAttrs convert_ulong8(char8);
ulong8 __CVAttrs convert_ulong8(uchar8);
ulong8 __CVAttrs convert_ulong8(short8);
ulong8 __CVAttrs convert_ulong8(ushort8);
ulong8 __CVAttrs convert_ulong8(int8);
ulong8 __CVAttrs convert_ulong8(uint8);
ulong8 __CVAttrs convert_ulong8(long8);
ulong8 __CVAttrs convert_ulong8(ulong8);
ulong8 __CVAttrs convert_ulong8(float8);
ulong8 __CVAttrs convert_ulong8(double8);
ulong8 __CVAttrs convert_ulong8_rte(char8);
ulong8 __CVAttrs convert_ulong8_rte(uchar8);
ulong8 __CVAttrs convert_ulong8_rte(short8);
ulong8 __CVAttrs convert_ulong8_rte(ushort8);
ulong8 __CVAttrs convert_ulong8_rte(int8);
ulong8 __CVAttrs convert_ulong8_rte(uint8);
ulong8 __CVAttrs convert_ulong8_rte(long8);
ulong8 __CVAttrs convert_ulong8_rte(ulong8);
ulong8 __CVAttrs convert_ulong8_rte(float8);
ulong8 __CVAttrs convert_ulong8_rte(double8);
ulong8 __CVAttrs convert_ulong8_rtz(char8);
ulong8 __CVAttrs convert_ulong8_rtz(uchar8);
ulong8 __CVAttrs convert_ulong8_rtz(short8);
ulong8 __CVAttrs convert_ulong8_rtz(ushort8);
ulong8 __CVAttrs convert_ulong8_rtz(int8);
ulong8 __CVAttrs convert_ulong8_rtz(uint8);
ulong8 __CVAttrs convert_ulong8_rtz(long8);
ulong8 __CVAttrs convert_ulong8_rtz(ulong8);
ulong8 __CVAttrs convert_ulong8_rtz(float8);
ulong8 __CVAttrs convert_ulong8_rtz(double8);
ulong8 __CVAttrs convert_ulong8_rtp(char8);
ulong8 __CVAttrs convert_ulong8_rtp(uchar8);
ulong8 __CVAttrs convert_ulong8_rtp(short8);
ulong8 __CVAttrs convert_ulong8_rtp(ushort8);
ulong8 __CVAttrs convert_ulong8_rtp(int8);
ulong8 __CVAttrs convert_ulong8_rtp(uint8);
ulong8 __CVAttrs convert_ulong8_rtp(long8);
ulong8 __CVAttrs convert_ulong8_rtp(ulong8);
ulong8 __CVAttrs convert_ulong8_rtp(float8);
ulong8 __CVAttrs convert_ulong8_rtp(double8);
ulong8 __CVAttrs convert_ulong8_rtn(char8);
ulong8 __CVAttrs convert_ulong8_rtn(uchar8);
ulong8 __CVAttrs convert_ulong8_rtn(short8);
ulong8 __CVAttrs convert_ulong8_rtn(ushort8);
ulong8 __CVAttrs convert_ulong8_rtn(int8);
ulong8 __CVAttrs convert_ulong8_rtn(uint8);
ulong8 __CVAttrs convert_ulong8_rtn(long8);
ulong8 __CVAttrs convert_ulong8_rtn(ulong8);
ulong8 __CVAttrs convert_ulong8_rtn(float8);
ulong8 __CVAttrs convert_ulong8_rtn(double8);
ulong8 __CVAttrs convert_ulong8_sat(char8);
ulong8 __CVAttrs convert_ulong8_sat(uchar8);
ulong8 __CVAttrs convert_ulong8_sat(short8);
ulong8 __CVAttrs convert_ulong8_sat(ushort8);
ulong8 __CVAttrs convert_ulong8_sat(int8);
ulong8 __CVAttrs convert_ulong8_sat(uint8);
ulong8 __CVAttrs convert_ulong8_sat(long8);
ulong8 __CVAttrs convert_ulong8_sat(ulong8);
ulong8 __CVAttrs convert_ulong8_sat(float8);
ulong8 __CVAttrs convert_ulong8_sat(double8);
ulong8 __CVAttrs convert_ulong8_sat_rte(char8);
ulong8 __CVAttrs convert_ulong8_sat_rte(uchar8);
ulong8 __CVAttrs convert_ulong8_sat_rte(short8);
ulong8 __CVAttrs convert_ulong8_sat_rte(ushort8);
ulong8 __CVAttrs convert_ulong8_sat_rte(int8);
ulong8 __CVAttrs convert_ulong8_sat_rte(uint8);
ulong8 __CVAttrs convert_ulong8_sat_rte(long8);
ulong8 __CVAttrs convert_ulong8_sat_rte(ulong8);
ulong8 __CVAttrs convert_ulong8_sat_rte(float8);
ulong8 __CVAttrs convert_ulong8_sat_rte(double8);
ulong8 __CVAttrs convert_ulong8_sat_rtz(char8);
ulong8 __CVAttrs convert_ulong8_sat_rtz(uchar8);
ulong8 __CVAttrs convert_ulong8_sat_rtz(short8);
ulong8 __CVAttrs convert_ulong8_sat_rtz(ushort8);
ulong8 __CVAttrs convert_ulong8_sat_rtz(int8);
ulong8 __CVAttrs convert_ulong8_sat_rtz(uint8);
ulong8 __CVAttrs convert_ulong8_sat_rtz(long8);
ulong8 __CVAttrs convert_ulong8_sat_rtz(ulong8);
ulong8 __CVAttrs convert_ulong8_sat_rtz(float8);
ulong8 __CVAttrs convert_ulong8_sat_rtz(double8);
ulong8 __CVAttrs convert_ulong8_sat_rtp(char8);
ulong8 __CVAttrs convert_ulong8_sat_rtp(uchar8);
ulong8 __CVAttrs convert_ulong8_sat_rtp(short8);
ulong8 __CVAttrs convert_ulong8_sat_rtp(ushort8);
ulong8 __CVAttrs convert_ulong8_sat_rtp(int8);
ulong8 __CVAttrs convert_ulong8_sat_rtp(uint8);
ulong8 __CVAttrs convert_ulong8_sat_rtp(long8);
ulong8 __CVAttrs convert_ulong8_sat_rtp(ulong8);
ulong8 __CVAttrs convert_ulong8_sat_rtp(float8);
ulong8 __CVAttrs convert_ulong8_sat_rtp(double8);
ulong8 __CVAttrs convert_ulong8_sat_rtn(char8);
ulong8 __CVAttrs convert_ulong8_sat_rtn(uchar8);
ulong8 __CVAttrs convert_ulong8_sat_rtn(short8);
ulong8 __CVAttrs convert_ulong8_sat_rtn(ushort8);
ulong8 __CVAttrs convert_ulong8_sat_rtn(int8);
ulong8 __CVAttrs convert_ulong8_sat_rtn(uint8);
ulong8 __CVAttrs convert_ulong8_sat_rtn(long8);
ulong8 __CVAttrs convert_ulong8_sat_rtn(ulong8);
ulong8 __CVAttrs convert_ulong8_sat_rtn(float8);
ulong8 __CVAttrs convert_ulong8_sat_rtn(double8);
ulong16 __CVAttrs convert_ulong16(char16);
ulong16 __CVAttrs convert_ulong16(uchar16);
ulong16 __CVAttrs convert_ulong16(short16);
ulong16 __CVAttrs convert_ulong16(ushort16);
ulong16 __CVAttrs convert_ulong16(int16);
ulong16 __CVAttrs convert_ulong16(uint16);
ulong16 __CVAttrs convert_ulong16(long16);
ulong16 __CVAttrs convert_ulong16(ulong16);
ulong16 __CVAttrs convert_ulong16(float16);
ulong16 __CVAttrs convert_ulong16(double16);
ulong16 __CVAttrs convert_ulong16_rte(char16);
ulong16 __CVAttrs convert_ulong16_rte(uchar16);
ulong16 __CVAttrs convert_ulong16_rte(short16);
ulong16 __CVAttrs convert_ulong16_rte(ushort16);
ulong16 __CVAttrs convert_ulong16_rte(int16);
ulong16 __CVAttrs convert_ulong16_rte(uint16);
ulong16 __CVAttrs convert_ulong16_rte(long16);
ulong16 __CVAttrs convert_ulong16_rte(ulong16);
ulong16 __CVAttrs convert_ulong16_rte(float16);
ulong16 __CVAttrs convert_ulong16_rte(double16);
ulong16 __CVAttrs convert_ulong16_rtz(char16);
ulong16 __CVAttrs convert_ulong16_rtz(uchar16);
ulong16 __CVAttrs convert_ulong16_rtz(short16);
ulong16 __CVAttrs convert_ulong16_rtz(ushort16);
ulong16 __CVAttrs convert_ulong16_rtz(int16);
ulong16 __CVAttrs convert_ulong16_rtz(uint16);
ulong16 __CVAttrs convert_ulong16_rtz(long16);
ulong16 __CVAttrs convert_ulong16_rtz(ulong16);
ulong16 __CVAttrs convert_ulong16_rtz(float16);
ulong16 __CVAttrs convert_ulong16_rtz(double16);
ulong16 __CVAttrs convert_ulong16_rtp(char16);
ulong16 __CVAttrs convert_ulong16_rtp(uchar16);
ulong16 __CVAttrs convert_ulong16_rtp(short16);
ulong16 __CVAttrs convert_ulong16_rtp(ushort16);
ulong16 __CVAttrs convert_ulong16_rtp(int16);
ulong16 __CVAttrs convert_ulong16_rtp(uint16);
ulong16 __CVAttrs convert_ulong16_rtp(long16);
ulong16 __CVAttrs convert_ulong16_rtp(ulong16);
ulong16 __CVAttrs convert_ulong16_rtp(float16);
ulong16 __CVAttrs convert_ulong16_rtp(double16);
ulong16 __CVAttrs convert_ulong16_rtn(char16);
ulong16 __CVAttrs convert_ulong16_rtn(uchar16);
ulong16 __CVAttrs convert_ulong16_rtn(short16);
ulong16 __CVAttrs convert_ulong16_rtn(ushort16);
ulong16 __CVAttrs convert_ulong16_rtn(int16);
ulong16 __CVAttrs convert_ulong16_rtn(uint16);
ulong16 __CVAttrs convert_ulong16_rtn(long16);
ulong16 __CVAttrs convert_ulong16_rtn(ulong16);
ulong16 __CVAttrs convert_ulong16_rtn(float16);
ulong16 __CVAttrs convert_ulong16_rtn(double16);
ulong16 __CVAttrs convert_ulong16_sat(char16);
ulong16 __CVAttrs convert_ulong16_sat(uchar16);
ulong16 __CVAttrs convert_ulong16_sat(short16);
ulong16 __CVAttrs convert_ulong16_sat(ushort16);
ulong16 __CVAttrs convert_ulong16_sat(int16);
ulong16 __CVAttrs convert_ulong16_sat(uint16);
ulong16 __CVAttrs convert_ulong16_sat(long16);
ulong16 __CVAttrs convert_ulong16_sat(ulong16);
ulong16 __CVAttrs convert_ulong16_sat(float16);
ulong16 __CVAttrs convert_ulong16_sat(double16);
ulong16 __CVAttrs convert_ulong16_sat_rte(char16);
ulong16 __CVAttrs convert_ulong16_sat_rte(uchar16);
ulong16 __CVAttrs convert_ulong16_sat_rte(short16);
ulong16 __CVAttrs convert_ulong16_sat_rte(ushort16);
ulong16 __CVAttrs convert_ulong16_sat_rte(int16);
ulong16 __CVAttrs convert_ulong16_sat_rte(uint16);
ulong16 __CVAttrs convert_ulong16_sat_rte(long16);
ulong16 __CVAttrs convert_ulong16_sat_rte(ulong16);
ulong16 __CVAttrs convert_ulong16_sat_rte(float16);
ulong16 __CVAttrs convert_ulong16_sat_rte(double16);
ulong16 __CVAttrs convert_ulong16_sat_rtz(char16);
ulong16 __CVAttrs convert_ulong16_sat_rtz(uchar16);
ulong16 __CVAttrs convert_ulong16_sat_rtz(short16);
ulong16 __CVAttrs convert_ulong16_sat_rtz(ushort16);
ulong16 __CVAttrs convert_ulong16_sat_rtz(int16);
ulong16 __CVAttrs convert_ulong16_sat_rtz(uint16);
ulong16 __CVAttrs convert_ulong16_sat_rtz(long16);
ulong16 __CVAttrs convert_ulong16_sat_rtz(ulong16);
ulong16 __CVAttrs convert_ulong16_sat_rtz(float16);
ulong16 __CVAttrs convert_ulong16_sat_rtz(double16);
ulong16 __CVAttrs convert_ulong16_sat_rtp(char16);
ulong16 __CVAttrs convert_ulong16_sat_rtp(uchar16);
ulong16 __CVAttrs convert_ulong16_sat_rtp(short16);
ulong16 __CVAttrs convert_ulong16_sat_rtp(ushort16);
ulong16 __CVAttrs convert_ulong16_sat_rtp(int16);
ulong16 __CVAttrs convert_ulong16_sat_rtp(uint16);
ulong16 __CVAttrs convert_ulong16_sat_rtp(long16);
ulong16 __CVAttrs convert_ulong16_sat_rtp(ulong16);
ulong16 __CVAttrs convert_ulong16_sat_rtp(float16);
ulong16 __CVAttrs convert_ulong16_sat_rtp(double16);
ulong16 __CVAttrs convert_ulong16_sat_rtn(char16);
ulong16 __CVAttrs convert_ulong16_sat_rtn(uchar16);
ulong16 __CVAttrs convert_ulong16_sat_rtn(short16);
ulong16 __CVAttrs convert_ulong16_sat_rtn(ushort16);
ulong16 __CVAttrs convert_ulong16_sat_rtn(int16);
ulong16 __CVAttrs convert_ulong16_sat_rtn(uint16);
ulong16 __CVAttrs convert_ulong16_sat_rtn(long16);
ulong16 __CVAttrs convert_ulong16_sat_rtn(ulong16);
ulong16 __CVAttrs convert_ulong16_sat_rtn(float16);
ulong16 __CVAttrs convert_ulong16_sat_rtn(double16);
float __CVAttrs convert_float(char);
float __CVAttrs convert_float(uchar);
float __CVAttrs convert_float(short);
float __CVAttrs convert_float(ushort);
float __CVAttrs convert_float(int);
float __CVAttrs convert_float(uint);
float __CVAttrs convert_float(long);
float __CVAttrs convert_float(ulong);
float __CVAttrs convert_float(float);
float __CVAttrs convert_float(double);
float __CVAttrs convert_float_rte(char);
float __CVAttrs convert_float_rte(uchar);
float __CVAttrs convert_float_rte(short);
float __CVAttrs convert_float_rte(ushort);
float __CVAttrs convert_float_rte(int);
float __CVAttrs convert_float_rte(uint);
float __CVAttrs convert_float_rte(long);
float __CVAttrs convert_float_rte(ulong);
float __CVAttrs convert_float_rte(float);
float __CVAttrs convert_float_rte(double);
float __CVAttrs convert_float_rtz(char);
float __CVAttrs convert_float_rtz(uchar);
float __CVAttrs convert_float_rtz(short);
float __CVAttrs convert_float_rtz(ushort);
float __CVAttrs convert_float_rtz(int);
float __CVAttrs convert_float_rtz(uint);
float __CVAttrs convert_float_rtz(long);
float __CVAttrs convert_float_rtz(ulong);
float __CVAttrs convert_float_rtz(float);
float __CVAttrs convert_float_rtz(double);
float __CVAttrs convert_float_rtp(char);
float __CVAttrs convert_float_rtp(uchar);
float __CVAttrs convert_float_rtp(short);
float __CVAttrs convert_float_rtp(ushort);
float __CVAttrs convert_float_rtp(int);
float __CVAttrs convert_float_rtp(uint);
float __CVAttrs convert_float_rtp(long);
float __CVAttrs convert_float_rtp(ulong);
float __CVAttrs convert_float_rtp(float);
float __CVAttrs convert_float_rtp(double);
float __CVAttrs convert_float_rtn(char);
float __CVAttrs convert_float_rtn(uchar);
float __CVAttrs convert_float_rtn(short);
float __CVAttrs convert_float_rtn(ushort);
float __CVAttrs convert_float_rtn(int);
float __CVAttrs convert_float_rtn(uint);
float __CVAttrs convert_float_rtn(long);
float __CVAttrs convert_float_rtn(ulong);
float __CVAttrs convert_float_rtn(float);
float __CVAttrs convert_float_rtn(double);
float2 __CVAttrs convert_float2(char2);
float2 __CVAttrs convert_float2(uchar2);
float2 __CVAttrs convert_float2(short2);
float2 __CVAttrs convert_float2(ushort2);
float2 __CVAttrs convert_float2(int2);
float2 __CVAttrs convert_float2(uint2);
float2 __CVAttrs convert_float2(long2);
float2 __CVAttrs convert_float2(ulong2);
float2 __CVAttrs convert_float2(float2);
float2 __CVAttrs convert_float2(double2);
float2 __CVAttrs convert_float2_rte(char2);
float2 __CVAttrs convert_float2_rte(uchar2);
float2 __CVAttrs convert_float2_rte(short2);
float2 __CVAttrs convert_float2_rte(ushort2);
float2 __CVAttrs convert_float2_rte(int2);
float2 __CVAttrs convert_float2_rte(uint2);
float2 __CVAttrs convert_float2_rte(long2);
float2 __CVAttrs convert_float2_rte(ulong2);
float2 __CVAttrs convert_float2_rte(float2);
float2 __CVAttrs convert_float2_rte(double2);
float2 __CVAttrs convert_float2_rtz(char2);
float2 __CVAttrs convert_float2_rtz(uchar2);
float2 __CVAttrs convert_float2_rtz(short2);
float2 __CVAttrs convert_float2_rtz(ushort2);
float2 __CVAttrs convert_float2_rtz(int2);
float2 __CVAttrs convert_float2_rtz(uint2);
float2 __CVAttrs convert_float2_rtz(long2);
float2 __CVAttrs convert_float2_rtz(ulong2);
float2 __CVAttrs convert_float2_rtz(float2);
float2 __CVAttrs convert_float2_rtz(double2);
float2 __CVAttrs convert_float2_rtp(char2);
float2 __CVAttrs convert_float2_rtp(uchar2);
float2 __CVAttrs convert_float2_rtp(short2);
float2 __CVAttrs convert_float2_rtp(ushort2);
float2 __CVAttrs convert_float2_rtp(int2);
float2 __CVAttrs convert_float2_rtp(uint2);
float2 __CVAttrs convert_float2_rtp(long2);
float2 __CVAttrs convert_float2_rtp(ulong2);
float2 __CVAttrs convert_float2_rtp(float2);
float2 __CVAttrs convert_float2_rtp(double2);
float2 __CVAttrs convert_float2_rtn(char2);
float2 __CVAttrs convert_float2_rtn(uchar2);
float2 __CVAttrs convert_float2_rtn(short2);
float2 __CVAttrs convert_float2_rtn(ushort2);
float2 __CVAttrs convert_float2_rtn(int2);
float2 __CVAttrs convert_float2_rtn(uint2);
float2 __CVAttrs convert_float2_rtn(long2);
float2 __CVAttrs convert_float2_rtn(ulong2);
float2 __CVAttrs convert_float2_rtn(float2);
float2 __CVAttrs convert_float2_rtn(double2);
#if __OPENCL_C_VERSION__ >= 110
float3 __CVAttrs convert_float3(char3);
float3 __CVAttrs convert_float3(uchar3);
float3 __CVAttrs convert_float3(short3);
float3 __CVAttrs convert_float3(ushort3);
float3 __CVAttrs convert_float3(int3);
float3 __CVAttrs convert_float3(uint3);
float3 __CVAttrs convert_float3(long3);
float3 __CVAttrs convert_float3(ulong3);
float3 __CVAttrs convert_float3(float3);
float3 __CVAttrs convert_float3(double3);
float3 __CVAttrs convert_float3_rte(char3);
float3 __CVAttrs convert_float3_rte(uchar3);
float3 __CVAttrs convert_float3_rte(short3);
float3 __CVAttrs convert_float3_rte(ushort3);
float3 __CVAttrs convert_float3_rte(int3);
float3 __CVAttrs convert_float3_rte(uint3);
float3 __CVAttrs convert_float3_rte(long3);
float3 __CVAttrs convert_float3_rte(ulong3);
float3 __CVAttrs convert_float3_rte(float3);
float3 __CVAttrs convert_float3_rte(double3);
float3 __CVAttrs convert_float3_rtz(char3);
float3 __CVAttrs convert_float3_rtz(uchar3);
float3 __CVAttrs convert_float3_rtz(short3);
float3 __CVAttrs convert_float3_rtz(ushort3);
float3 __CVAttrs convert_float3_rtz(int3);
float3 __CVAttrs convert_float3_rtz(uint3);
float3 __CVAttrs convert_float3_rtz(long3);
float3 __CVAttrs convert_float3_rtz(ulong3);
float3 __CVAttrs convert_float3_rtz(float3);
float3 __CVAttrs convert_float3_rtz(double3);
float3 __CVAttrs convert_float3_rtp(char3);
float3 __CVAttrs convert_float3_rtp(uchar3);
float3 __CVAttrs convert_float3_rtp(short3);
float3 __CVAttrs convert_float3_rtp(ushort3);
float3 __CVAttrs convert_float3_rtp(int3);
float3 __CVAttrs convert_float3_rtp(uint3);
float3 __CVAttrs convert_float3_rtp(long3);
float3 __CVAttrs convert_float3_rtp(ulong3);
float3 __CVAttrs convert_float3_rtp(float3);
float3 __CVAttrs convert_float3_rtp(double3);
float3 __CVAttrs convert_float3_rtn(char3);
float3 __CVAttrs convert_float3_rtn(uchar3);
float3 __CVAttrs convert_float3_rtn(short3);
float3 __CVAttrs convert_float3_rtn(ushort3);
float3 __CVAttrs convert_float3_rtn(int3);
float3 __CVAttrs convert_float3_rtn(uint3);
float3 __CVAttrs convert_float3_rtn(long3);
float3 __CVAttrs convert_float3_rtn(ulong3);
float3 __CVAttrs convert_float3_rtn(float3);
float3 __CVAttrs convert_float3_rtn(double3);
#endif
float4 __CVAttrs convert_float4(char4);
float4 __CVAttrs convert_float4(uchar4);
float4 __CVAttrs convert_float4(short4);
float4 __CVAttrs convert_float4(ushort4);
float4 __CVAttrs convert_float4(int4);
float4 __CVAttrs convert_float4(uint4);
float4 __CVAttrs convert_float4(long4);
float4 __CVAttrs convert_float4(ulong4);
float4 __CVAttrs convert_float4(float4);
float4 __CVAttrs convert_float4(double4);
float4 __CVAttrs convert_float4_rte(char4);
float4 __CVAttrs convert_float4_rte(uchar4);
float4 __CVAttrs convert_float4_rte(short4);
float4 __CVAttrs convert_float4_rte(ushort4);
float4 __CVAttrs convert_float4_rte(int4);
float4 __CVAttrs convert_float4_rte(uint4);
float4 __CVAttrs convert_float4_rte(long4);
float4 __CVAttrs convert_float4_rte(ulong4);
float4 __CVAttrs convert_float4_rte(float4);
float4 __CVAttrs convert_float4_rte(double4);
float4 __CVAttrs convert_float4_rtz(char4);
float4 __CVAttrs convert_float4_rtz(uchar4);
float4 __CVAttrs convert_float4_rtz(short4);
float4 __CVAttrs convert_float4_rtz(ushort4);
float4 __CVAttrs convert_float4_rtz(int4);
float4 __CVAttrs convert_float4_rtz(uint4);
float4 __CVAttrs convert_float4_rtz(long4);
float4 __CVAttrs convert_float4_rtz(ulong4);
float4 __CVAttrs convert_float4_rtz(float4);
float4 __CVAttrs convert_float4_rtz(double4);
float4 __CVAttrs convert_float4_rtp(char4);
float4 __CVAttrs convert_float4_rtp(uchar4);
float4 __CVAttrs convert_float4_rtp(short4);
float4 __CVAttrs convert_float4_rtp(ushort4);
float4 __CVAttrs convert_float4_rtp(int4);
float4 __CVAttrs convert_float4_rtp(uint4);
float4 __CVAttrs convert_float4_rtp(long4);
float4 __CVAttrs convert_float4_rtp(ulong4);
float4 __CVAttrs convert_float4_rtp(float4);
float4 __CVAttrs convert_float4_rtp(double4);
float4 __CVAttrs convert_float4_rtn(char4);
float4 __CVAttrs convert_float4_rtn(uchar4);
float4 __CVAttrs convert_float4_rtn(short4);
float4 __CVAttrs convert_float4_rtn(ushort4);
float4 __CVAttrs convert_float4_rtn(int4);
float4 __CVAttrs convert_float4_rtn(uint4);
float4 __CVAttrs convert_float4_rtn(long4);
float4 __CVAttrs convert_float4_rtn(ulong4);
float4 __CVAttrs convert_float4_rtn(float4);
float4 __CVAttrs convert_float4_rtn(double4);
float8 __CVAttrs convert_float8(char8);
float8 __CVAttrs convert_float8(uchar8);
float8 __CVAttrs convert_float8(short8);
float8 __CVAttrs convert_float8(ushort8);
float8 __CVAttrs convert_float8(int8);
float8 __CVAttrs convert_float8(uint8);
float8 __CVAttrs convert_float8(long8);
float8 __CVAttrs convert_float8(ulong8);
float8 __CVAttrs convert_float8(float8);
float8 __CVAttrs convert_float8(double8);
float8 __CVAttrs convert_float8_rte(char8);
float8 __CVAttrs convert_float8_rte(uchar8);
float8 __CVAttrs convert_float8_rte(short8);
float8 __CVAttrs convert_float8_rte(ushort8);
float8 __CVAttrs convert_float8_rte(int8);
float8 __CVAttrs convert_float8_rte(uint8);
float8 __CVAttrs convert_float8_rte(long8);
float8 __CVAttrs convert_float8_rte(ulong8);
float8 __CVAttrs convert_float8_rte(float8);
float8 __CVAttrs convert_float8_rte(double8);
float8 __CVAttrs convert_float8_rtz(char8);
float8 __CVAttrs convert_float8_rtz(uchar8);
float8 __CVAttrs convert_float8_rtz(short8);
float8 __CVAttrs convert_float8_rtz(ushort8);
float8 __CVAttrs convert_float8_rtz(int8);
float8 __CVAttrs convert_float8_rtz(uint8);
float8 __CVAttrs convert_float8_rtz(long8);
float8 __CVAttrs convert_float8_rtz(ulong8);
float8 __CVAttrs convert_float8_rtz(float8);
float8 __CVAttrs convert_float8_rtz(double8);
float8 __CVAttrs convert_float8_rtp(char8);
float8 __CVAttrs convert_float8_rtp(uchar8);
float8 __CVAttrs convert_float8_rtp(short8);
float8 __CVAttrs convert_float8_rtp(ushort8);
float8 __CVAttrs convert_float8_rtp(int8);
float8 __CVAttrs convert_float8_rtp(uint8);
float8 __CVAttrs convert_float8_rtp(long8);
float8 __CVAttrs convert_float8_rtp(ulong8);
float8 __CVAttrs convert_float8_rtp(float8);
float8 __CVAttrs convert_float8_rtp(double8);
float8 __CVAttrs convert_float8_rtn(char8);
float8 __CVAttrs convert_float8_rtn(uchar8);
float8 __CVAttrs convert_float8_rtn(short8);
float8 __CVAttrs convert_float8_rtn(ushort8);
float8 __CVAttrs convert_float8_rtn(int8);
float8 __CVAttrs convert_float8_rtn(uint8);
float8 __CVAttrs convert_float8_rtn(long8);
float8 __CVAttrs convert_float8_rtn(ulong8);
float8 __CVAttrs convert_float8_rtn(float8);
float8 __CVAttrs convert_float8_rtn(double8);
float16 __CVAttrs convert_float16(char16);
float16 __CVAttrs convert_float16(uchar16);
float16 __CVAttrs convert_float16(short16);
float16 __CVAttrs convert_float16(ushort16);
float16 __CVAttrs convert_float16(int16);
float16 __CVAttrs convert_float16(uint16);
float16 __CVAttrs convert_float16(long16);
float16 __CVAttrs convert_float16(ulong16);
float16 __CVAttrs convert_float16(float16);
float16 __CVAttrs convert_float16(double16);
float16 __CVAttrs convert_float16_rte(char16);
float16 __CVAttrs convert_float16_rte(uchar16);
float16 __CVAttrs convert_float16_rte(short16);
float16 __CVAttrs convert_float16_rte(ushort16);
float16 __CVAttrs convert_float16_rte(int16);
float16 __CVAttrs convert_float16_rte(uint16);
float16 __CVAttrs convert_float16_rte(long16);
float16 __CVAttrs convert_float16_rte(ulong16);
float16 __CVAttrs convert_float16_rte(float16);
float16 __CVAttrs convert_float16_rte(double16);
float16 __CVAttrs convert_float16_rtz(char16);
float16 __CVAttrs convert_float16_rtz(uchar16);
float16 __CVAttrs convert_float16_rtz(short16);
float16 __CVAttrs convert_float16_rtz(ushort16);
float16 __CVAttrs convert_float16_rtz(int16);
float16 __CVAttrs convert_float16_rtz(uint16);
float16 __CVAttrs convert_float16_rtz(long16);
float16 __CVAttrs convert_float16_rtz(ulong16);
float16 __CVAttrs convert_float16_rtz(float16);
float16 __CVAttrs convert_float16_rtz(double16);
float16 __CVAttrs convert_float16_rtp(char16);
float16 __CVAttrs convert_float16_rtp(uchar16);
float16 __CVAttrs convert_float16_rtp(short16);
float16 __CVAttrs convert_float16_rtp(ushort16);
float16 __CVAttrs convert_float16_rtp(int16);
float16 __CVAttrs convert_float16_rtp(uint16);
float16 __CVAttrs convert_float16_rtp(long16);
float16 __CVAttrs convert_float16_rtp(ulong16);
float16 __CVAttrs convert_float16_rtp(float16);
float16 __CVAttrs convert_float16_rtp(double16);
float16 __CVAttrs convert_float16_rtn(char16);
float16 __CVAttrs convert_float16_rtn(uchar16);
float16 __CVAttrs convert_float16_rtn(short16);
float16 __CVAttrs convert_float16_rtn(ushort16);
float16 __CVAttrs convert_float16_rtn(int16);
float16 __CVAttrs convert_float16_rtn(uint16);
float16 __CVAttrs convert_float16_rtn(long16);
float16 __CVAttrs convert_float16_rtn(ulong16);
float16 __CVAttrs convert_float16_rtn(float16);
float16 __CVAttrs convert_float16_rtn(double16);
double __CVAttrs convert_double(char);
double __CVAttrs convert_double(uchar);
double __CVAttrs convert_double(short);
double __CVAttrs convert_double(ushort);
double __CVAttrs convert_double(int);
double __CVAttrs convert_double(uint);
double __CVAttrs convert_double(long);
double __CVAttrs convert_double(ulong);
double __CVAttrs convert_double(float);
double __CVAttrs convert_double(double);
double __CVAttrs convert_double_rte(char);
double __CVAttrs convert_double_rte(uchar);
double __CVAttrs convert_double_rte(short);
double __CVAttrs convert_double_rte(ushort);
double __CVAttrs convert_double_rte(int);
double __CVAttrs convert_double_rte(uint);
double __CVAttrs convert_double_rte(long);
double __CVAttrs convert_double_rte(ulong);
double __CVAttrs convert_double_rte(float);
double __CVAttrs convert_double_rte(double);
double __CVAttrs convert_double_rtz(char);
double __CVAttrs convert_double_rtz(uchar);
double __CVAttrs convert_double_rtz(short);
double __CVAttrs convert_double_rtz(ushort);
double __CVAttrs convert_double_rtz(int);
double __CVAttrs convert_double_rtz(uint);
double __CVAttrs convert_double_rtz(long);
double __CVAttrs convert_double_rtz(ulong);
double __CVAttrs convert_double_rtz(float);
double __CVAttrs convert_double_rtz(double);
double __CVAttrs convert_double_rtp(char);
double __CVAttrs convert_double_rtp(uchar);
double __CVAttrs convert_double_rtp(short);
double __CVAttrs convert_double_rtp(ushort);
double __CVAttrs convert_double_rtp(int);
double __CVAttrs convert_double_rtp(uint);
double __CVAttrs convert_double_rtp(long);
double __CVAttrs convert_double_rtp(ulong);
double __CVAttrs convert_double_rtp(float);
double __CVAttrs convert_double_rtp(double);
double __CVAttrs convert_double_rtn(char);
double __CVAttrs convert_double_rtn(uchar);
double __CVAttrs convert_double_rtn(short);
double __CVAttrs convert_double_rtn(ushort);
double __CVAttrs convert_double_rtn(int);
double __CVAttrs convert_double_rtn(uint);
double __CVAttrs convert_double_rtn(long);
double __CVAttrs convert_double_rtn(ulong);
double __CVAttrs convert_double_rtn(float);
double __CVAttrs convert_double_rtn(double);
double2 __CVAttrs convert_double2(char2);
double2 __CVAttrs convert_double2(uchar2);
double2 __CVAttrs convert_double2(short2);
double2 __CVAttrs convert_double2(ushort2);
double2 __CVAttrs convert_double2(int2);
double2 __CVAttrs convert_double2(uint2);
double2 __CVAttrs convert_double2(long2);
double2 __CVAttrs convert_double2(ulong2);
double2 __CVAttrs convert_double2(float2);
double2 __CVAttrs convert_double2(double2);
double2 __CVAttrs convert_double2_rte(char2);
double2 __CVAttrs convert_double2_rte(uchar2);
double2 __CVAttrs convert_double2_rte(short2);
double2 __CVAttrs convert_double2_rte(ushort2);
double2 __CVAttrs convert_double2_rte(int2);
double2 __CVAttrs convert_double2_rte(uint2);
double2 __CVAttrs convert_double2_rte(long2);
double2 __CVAttrs convert_double2_rte(ulong2);
double2 __CVAttrs convert_double2_rte(float2);
double2 __CVAttrs convert_double2_rte(double2);
double2 __CVAttrs convert_double2_rtz(char2);
double2 __CVAttrs convert_double2_rtz(uchar2);
double2 __CVAttrs convert_double2_rtz(short2);
double2 __CVAttrs convert_double2_rtz(ushort2);
double2 __CVAttrs convert_double2_rtz(int2);
double2 __CVAttrs convert_double2_rtz(uint2);
double2 __CVAttrs convert_double2_rtz(long2);
double2 __CVAttrs convert_double2_rtz(ulong2);
double2 __CVAttrs convert_double2_rtz(float2);
double2 __CVAttrs convert_double2_rtz(double2);
double2 __CVAttrs convert_double2_rtp(char2);
double2 __CVAttrs convert_double2_rtp(uchar2);
double2 __CVAttrs convert_double2_rtp(short2);
double2 __CVAttrs convert_double2_rtp(ushort2);
double2 __CVAttrs convert_double2_rtp(int2);
double2 __CVAttrs convert_double2_rtp(uint2);
double2 __CVAttrs convert_double2_rtp(long2);
double2 __CVAttrs convert_double2_rtp(ulong2);
double2 __CVAttrs convert_double2_rtp(float2);
double2 __CVAttrs convert_double2_rtp(double2);
double2 __CVAttrs convert_double2_rtn(char2);
double2 __CVAttrs convert_double2_rtn(uchar2);
double2 __CVAttrs convert_double2_rtn(short2);
double2 __CVAttrs convert_double2_rtn(ushort2);
double2 __CVAttrs convert_double2_rtn(int2);
double2 __CVAttrs convert_double2_rtn(uint2);
double2 __CVAttrs convert_double2_rtn(long2);
double2 __CVAttrs convert_double2_rtn(ulong2);
double2 __CVAttrs convert_double2_rtn(float2);
double2 __CVAttrs convert_double2_rtn(double2);
#if __OPENCL_C_VERSION__ >= 110
double3 __CVAttrs convert_double3(char3);
double3 __CVAttrs convert_double3(uchar3);
double3 __CVAttrs convert_double3(short3);
double3 __CVAttrs convert_double3(ushort3);
double3 __CVAttrs convert_double3(int3);
double3 __CVAttrs convert_double3(uint3);
double3 __CVAttrs convert_double3(long3);
double3 __CVAttrs convert_double3(ulong3);
double3 __CVAttrs convert_double3(float3);
double3 __CVAttrs convert_double3(double3);
double3 __CVAttrs convert_double3_rte(char3);
double3 __CVAttrs convert_double3_rte(uchar3);
double3 __CVAttrs convert_double3_rte(short3);
double3 __CVAttrs convert_double3_rte(ushort3);
double3 __CVAttrs convert_double3_rte(int3);
double3 __CVAttrs convert_double3_rte(uint3);
double3 __CVAttrs convert_double3_rte(long3);
double3 __CVAttrs convert_double3_rte(ulong3);
double3 __CVAttrs convert_double3_rte(float3);
double3 __CVAttrs convert_double3_rte(double3);
double3 __CVAttrs convert_double3_rtz(char3);
double3 __CVAttrs convert_double3_rtz(uchar3);
double3 __CVAttrs convert_double3_rtz(short3);
double3 __CVAttrs convert_double3_rtz(ushort3);
double3 __CVAttrs convert_double3_rtz(int3);
double3 __CVAttrs convert_double3_rtz(uint3);
double3 __CVAttrs convert_double3_rtz(long3);
double3 __CVAttrs convert_double3_rtz(ulong3);
double3 __CVAttrs convert_double3_rtz(float3);
double3 __CVAttrs convert_double3_rtz(double3);
double3 __CVAttrs convert_double3_rtp(char3);
double3 __CVAttrs convert_double3_rtp(uchar3);
double3 __CVAttrs convert_double3_rtp(short3);
double3 __CVAttrs convert_double3_rtp(ushort3);
double3 __CVAttrs convert_double3_rtp(int3);
double3 __CVAttrs convert_double3_rtp(uint3);
double3 __CVAttrs convert_double3_rtp(long3);
double3 __CVAttrs convert_double3_rtp(ulong3);
double3 __CVAttrs convert_double3_rtp(float3);
double3 __CVAttrs convert_double3_rtp(double3);
double3 __CVAttrs convert_double3_rtn(char3);
double3 __CVAttrs convert_double3_rtn(uchar3);
double3 __CVAttrs convert_double3_rtn(short3);
double3 __CVAttrs convert_double3_rtn(ushort3);
double3 __CVAttrs convert_double3_rtn(int3);
double3 __CVAttrs convert_double3_rtn(uint3);
double3 __CVAttrs convert_double3_rtn(long3);
double3 __CVAttrs convert_double3_rtn(ulong3);
double3 __CVAttrs convert_double3_rtn(float3);
double3 __CVAttrs convert_double3_rtn(double3);
#endif
double4 __CVAttrs convert_double4(char4);
double4 __CVAttrs convert_double4(uchar4);
double4 __CVAttrs convert_double4(short4);
double4 __CVAttrs convert_double4(ushort4);
double4 __CVAttrs convert_double4(int4);
double4 __CVAttrs convert_double4(uint4);
double4 __CVAttrs convert_double4(long4);
double4 __CVAttrs convert_double4(ulong4);
double4 __CVAttrs convert_double4(float4);
double4 __CVAttrs convert_double4(double4);
double4 __CVAttrs convert_double4_rte(char4);
double4 __CVAttrs convert_double4_rte(uchar4);
double4 __CVAttrs convert_double4_rte(short4);
double4 __CVAttrs convert_double4_rte(ushort4);
double4 __CVAttrs convert_double4_rte(int4);
double4 __CVAttrs convert_double4_rte(uint4);
double4 __CVAttrs convert_double4_rte(long4);
double4 __CVAttrs convert_double4_rte(ulong4);
double4 __CVAttrs convert_double4_rte(float4);
double4 __CVAttrs convert_double4_rte(double4);
double4 __CVAttrs convert_double4_rtz(char4);
double4 __CVAttrs convert_double4_rtz(uchar4);
double4 __CVAttrs convert_double4_rtz(short4);
double4 __CVAttrs convert_double4_rtz(ushort4);
double4 __CVAttrs convert_double4_rtz(int4);
double4 __CVAttrs convert_double4_rtz(uint4);
double4 __CVAttrs convert_double4_rtz(long4);
double4 __CVAttrs convert_double4_rtz(ulong4);
double4 __CVAttrs convert_double4_rtz(float4);
double4 __CVAttrs convert_double4_rtz(double4);
double4 __CVAttrs convert_double4_rtp(char4);
double4 __CVAttrs convert_double4_rtp(uchar4);
double4 __CVAttrs convert_double4_rtp(short4);
double4 __CVAttrs convert_double4_rtp(ushort4);
double4 __CVAttrs convert_double4_rtp(int4);
double4 __CVAttrs convert_double4_rtp(uint4);
double4 __CVAttrs convert_double4_rtp(long4);
double4 __CVAttrs convert_double4_rtp(ulong4);
double4 __CVAttrs convert_double4_rtp(float4);
double4 __CVAttrs convert_double4_rtp(double4);
double4 __CVAttrs convert_double4_rtn(char4);
double4 __CVAttrs convert_double4_rtn(uchar4);
double4 __CVAttrs convert_double4_rtn(short4);
double4 __CVAttrs convert_double4_rtn(ushort4);
double4 __CVAttrs convert_double4_rtn(int4);
double4 __CVAttrs convert_double4_rtn(uint4);
double4 __CVAttrs convert_double4_rtn(long4);
double4 __CVAttrs convert_double4_rtn(ulong4);
double4 __CVAttrs convert_double4_rtn(float4);
double4 __CVAttrs convert_double4_rtn(double4);
double8 __CVAttrs convert_double8(char8);
double8 __CVAttrs convert_double8(uchar8);
double8 __CVAttrs convert_double8(short8);
double8 __CVAttrs convert_double8(ushort8);
double8 __CVAttrs convert_double8(int8);
double8 __CVAttrs convert_double8(uint8);
double8 __CVAttrs convert_double8(long8);
double8 __CVAttrs convert_double8(ulong8);
double8 __CVAttrs convert_double8(float8);
double8 __CVAttrs convert_double8(double8);
double8 __CVAttrs convert_double8_rte(char8);
double8 __CVAttrs convert_double8_rte(uchar8);
double8 __CVAttrs convert_double8_rte(short8);
double8 __CVAttrs convert_double8_rte(ushort8);
double8 __CVAttrs convert_double8_rte(int8);
double8 __CVAttrs convert_double8_rte(uint8);
double8 __CVAttrs convert_double8_rte(long8);
double8 __CVAttrs convert_double8_rte(ulong8);
double8 __CVAttrs convert_double8_rte(float8);
double8 __CVAttrs convert_double8_rte(double8);
double8 __CVAttrs convert_double8_rtz(char8);
double8 __CVAttrs convert_double8_rtz(uchar8);
double8 __CVAttrs convert_double8_rtz(short8);
double8 __CVAttrs convert_double8_rtz(ushort8);
double8 __CVAttrs convert_double8_rtz(int8);
double8 __CVAttrs convert_double8_rtz(uint8);
double8 __CVAttrs convert_double8_rtz(long8);
double8 __CVAttrs convert_double8_rtz(ulong8);
double8 __CVAttrs convert_double8_rtz(float8);
double8 __CVAttrs convert_double8_rtz(double8);
double8 __CVAttrs convert_double8_rtp(char8);
double8 __CVAttrs convert_double8_rtp(uchar8);
double8 __CVAttrs convert_double8_rtp(short8);
double8 __CVAttrs convert_double8_rtp(ushort8);
double8 __CVAttrs convert_double8_rtp(int8);
double8 __CVAttrs convert_double8_rtp(uint8);
double8 __CVAttrs convert_double8_rtp(long8);
double8 __CVAttrs convert_double8_rtp(ulong8);
double8 __CVAttrs convert_double8_rtp(float8);
double8 __CVAttrs convert_double8_rtp(double8);
double8 __CVAttrs convert_double8_rtn(char8);
double8 __CVAttrs convert_double8_rtn(uchar8);
double8 __CVAttrs convert_double8_rtn(short8);
double8 __CVAttrs convert_double8_rtn(ushort8);
double8 __CVAttrs convert_double8_rtn(int8);
double8 __CVAttrs convert_double8_rtn(uint8);
double8 __CVAttrs convert_double8_rtn(long8);
double8 __CVAttrs convert_double8_rtn(ulong8);
double8 __CVAttrs convert_double8_rtn(float8);
double8 __CVAttrs convert_double8_rtn(double8);
double16 __CVAttrs convert_double16(char16);
double16 __CVAttrs convert_double16(uchar16);
double16 __CVAttrs convert_double16(short16);
double16 __CVAttrs convert_double16(ushort16);
double16 __CVAttrs convert_double16(int16);
double16 __CVAttrs convert_double16(uint16);
double16 __CVAttrs convert_double16(long16);
double16 __CVAttrs convert_double16(ulong16);
double16 __CVAttrs convert_double16(float16);
double16 __CVAttrs convert_double16(double16);
double16 __CVAttrs convert_double16_rte(char16);
double16 __CVAttrs convert_double16_rte(uchar16);
double16 __CVAttrs convert_double16_rte(short16);
double16 __CVAttrs convert_double16_rte(ushort16);
double16 __CVAttrs convert_double16_rte(int16);
double16 __CVAttrs convert_double16_rte(uint16);
double16 __CVAttrs convert_double16_rte(long16);
double16 __CVAttrs convert_double16_rte(ulong16);
double16 __CVAttrs convert_double16_rte(float16);
double16 __CVAttrs convert_double16_rte(double16);
double16 __CVAttrs convert_double16_rtz(char16);
double16 __CVAttrs convert_double16_rtz(uchar16);
double16 __CVAttrs convert_double16_rtz(short16);
double16 __CVAttrs convert_double16_rtz(ushort16);
double16 __CVAttrs convert_double16_rtz(int16);
double16 __CVAttrs convert_double16_rtz(uint16);
double16 __CVAttrs convert_double16_rtz(long16);
double16 __CVAttrs convert_double16_rtz(ulong16);
double16 __CVAttrs convert_double16_rtz(float16);
double16 __CVAttrs convert_double16_rtz(double16);
double16 __CVAttrs convert_double16_rtp(char16);
double16 __CVAttrs convert_double16_rtp(uchar16);
double16 __CVAttrs convert_double16_rtp(short16);
double16 __CVAttrs convert_double16_rtp(ushort16);
double16 __CVAttrs convert_double16_rtp(int16);
double16 __CVAttrs convert_double16_rtp(uint16);
double16 __CVAttrs convert_double16_rtp(long16);
double16 __CVAttrs convert_double16_rtp(ulong16);
double16 __CVAttrs convert_double16_rtp(float16);
double16 __CVAttrs convert_double16_rtp(double16);
double16 __CVAttrs convert_double16_rtn(char16);
double16 __CVAttrs convert_double16_rtn(uchar16);
double16 __CVAttrs convert_double16_rtn(short16);
double16 __CVAttrs convert_double16_rtn(ushort16);
double16 __CVAttrs convert_double16_rtn(int16);
double16 __CVAttrs convert_double16_rtn(uint16);
double16 __CVAttrs convert_double16_rtn(long16);
double16 __CVAttrs convert_double16_rtn(ulong16);
double16 __CVAttrs convert_double16_rtn(float16);
double16 __CVAttrs convert_double16_rtn(double16);
#undef __CVAttrs

short __attribute__((__overloadable__,__always_inline__,const)) upsample( char hi, uchar lo );
ushort __attribute__((__overloadable__,__always_inline__,const)) upsample( uchar hi, uchar lo );
int __attribute__((__overloadable__,__always_inline__,const)) upsample( short hi, ushort lo );
uint __attribute__((__overloadable__,__always_inline__,const)) upsample( ushort hi, ushort lo );
long __attribute__((__overloadable__,__always_inline__,const)) upsample( int hi, uint lo );
ulong __attribute__((__overloadable__,__always_inline__,const)) upsample( uint hi, uint lo );
short2 __attribute__((__overloadable__,__always_inline__,const)) upsample( char2 hi, uchar2 lo );
#if __OPENCL_C_VERSION__ >= 110
short3 __attribute__((__overloadable__,__always_inline__,const)) upsample( char3 hi, uchar3 lo );
#endif
short4 __attribute__((__overloadable__,__always_inline__,const)) upsample( char4 hi, uchar4 lo );
short8 __attribute__((__overloadable__,__always_inline__,const)) upsample( char8 hi, uchar8 lo );
short16 __attribute__((__overloadable__,__always_inline__,const)) upsample( char16 hi, uchar16 lo );
ushort2 __attribute__((__overloadable__,__always_inline__,const)) upsample( uchar2 hi, uchar2 lo );
#if __OPENCL_C_VERSION__ >= 110
ushort3 __attribute__((__overloadable__,__always_inline__,const)) upsample( uchar3 hi, uchar3 lo );
#endif
ushort4 __attribute__((__overloadable__,__always_inline__,const)) upsample( uchar4 hi, uchar4 lo );
ushort8 __attribute__((__overloadable__,__always_inline__,const)) upsample( uchar8 hi, uchar8 lo );
ushort16 __attribute__((__overloadable__,__always_inline__,const)) upsample( uchar16 hi, uchar16 lo );
int2 __attribute__((__overloadable__,__always_inline__,const)) upsample( short2 hi, ushort2 lo );
#if __OPENCL_C_VERSION__ >= 110
int3 __attribute__((__overloadable__,__always_inline__,const)) upsample( short3 hi, ushort3 lo );
#endif
int4 __attribute__((__overloadable__,__always_inline__,const)) upsample( short4 hi, ushort4 lo );
int8 __attribute__((__overloadable__,__always_inline__,const)) upsample( short8 hi, ushort8 lo );
int16 __attribute__((__overloadable__,__always_inline__,const)) upsample( short16 hi, ushort16 lo );
uint2 __attribute__((__overloadable__,__always_inline__,const)) upsample( ushort2 hi, ushort2 lo );
#if __OPENCL_C_VERSION__ >= 110
uint3 __attribute__((__overloadable__,__always_inline__,const)) upsample( ushort3 hi, ushort3 lo );
#endif
uint4 __attribute__((__overloadable__,__always_inline__,const)) upsample( ushort4 hi, ushort4 lo );
uint8 __attribute__((__overloadable__,__always_inline__,const)) upsample( ushort8 hi, ushort8 lo );
uint16 __attribute__((__overloadable__,__always_inline__,const)) upsample( ushort16 hi, ushort16 lo );
long2 __attribute__((__overloadable__,__always_inline__,const)) upsample( int2 hi, uint2 lo );
#if __OPENCL_C_VERSION__ >= 110
long3 __attribute__((__overloadable__,__always_inline__,const)) upsample( int3 hi, uint3 lo );
#endif
long4 __attribute__((__overloadable__,__always_inline__,const)) upsample( int4 hi, uint4 lo );
long8 __attribute__((__overloadable__,__always_inline__,const)) upsample( int8 hi, uint8 lo );
long16 __attribute__((__overloadable__,__always_inline__,const)) upsample( int16 hi, uint16 lo );
ulong2 __attribute__((__overloadable__,__always_inline__,const)) upsample( uint2 hi, uint2 lo );
#if __OPENCL_C_VERSION__ >= 110
ulong3 __attribute__((__overloadable__,__always_inline__,const)) upsample( uint3 hi, uint3 lo );
#endif
ulong4 __attribute__((__overloadable__,__always_inline__,const)) upsample( uint4 hi, uint4 lo );
ulong8 __attribute__((__overloadable__,__always_inline__,const)) upsample( uint8 hi, uint8 lo );
ulong16 __attribute__((__overloadable__,__always_inline__,const)) upsample( uint16 hi, uint16 lo );
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global char *p, size_t n);
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global char2 *p, size_t n);
#if __OPENCL_C_VERSION__ >= 110
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global char3 *p, size_t n);
#endif
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global char4 *p, size_t n);
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global char8 *p, size_t n);
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global char16 *p, size_t n);
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global uchar *p, size_t n);
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global uchar2 *p, size_t n);
#if __OPENCL_C_VERSION__ >= 110
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global uchar3 *p, size_t n);
#endif
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global uchar4 *p, size_t n);
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global uchar8 *p, size_t n);
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global uchar16 *p, size_t n);
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global short *p, size_t n);
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global short2 *p, size_t n);
#if __OPENCL_C_VERSION__ >= 110
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global short3 *p, size_t n);
#endif
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global short4 *p, size_t n);
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global short8 *p, size_t n);
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global short16 *p, size_t n);
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global ushort *p, size_t n);
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global ushort2 *p, size_t n);
#if __OPENCL_C_VERSION__ >= 110
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global ushort3 *p, size_t n);
#endif
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global ushort4 *p, size_t n);
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global ushort8 *p, size_t n);
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global ushort16 *p, size_t n);
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global int *p, size_t n);
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global int2 *p, size_t n);
#if __OPENCL_C_VERSION__ >= 110
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global int3 *p, size_t n);
#endif
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global int4 *p, size_t n);
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global int8 *p, size_t n);
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global int16 *p, size_t n);
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global uint *p, size_t n);
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global uint2 *p, size_t n);
#if __OPENCL_C_VERSION__ >= 110
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global uint3 *p, size_t n);
#endif
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global uint4 *p, size_t n);
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global uint8 *p, size_t n);
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global uint16 *p, size_t n);
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global long *p, size_t n);
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global long2 *p, size_t n);
#if __OPENCL_C_VERSION__ >= 110
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global long3 *p, size_t n);
#endif
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global long4 *p, size_t n);
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global long8 *p, size_t n);
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global long16 *p, size_t n);
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global ulong *p, size_t n);
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global ulong2 *p, size_t n);
#if __OPENCL_C_VERSION__ >= 110
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global ulong3 *p, size_t n);
#endif
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global ulong4 *p, size_t n);
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global ulong8 *p, size_t n);
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global ulong16 *p, size_t n);
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global float *p, size_t n);
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global float2 *p, size_t n);
#if __OPENCL_C_VERSION__ >= 110
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global float3 *p, size_t n);
#endif
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global float4 *p, size_t n);
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global float8 *p, size_t n);
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global float16 *p, size_t n);
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global double *p, size_t n);
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global double2 *p, size_t n);
#if __OPENCL_C_VERSION__ >= 110
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global double3 *p, size_t n);
#endif
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global double4 *p, size_t n);
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global double8 *p, size_t n);
void  __attribute__((__overloadable__,__always_inline__,const)) prefetch( const global double16 *p, size_t n);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local char* dest, const global char* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local char2* dest, const global char2* src, size_t n, event_t e);
#if __OPENCL_C_VERSION__ >= 110
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local char3* dest, const global char3* src, size_t n, event_t e);
#endif
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local char4* dest, const global char4* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local char8* dest, const global char8* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local char16* dest, const global char16* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local uchar* dest, const global uchar* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local uchar2* dest, const global uchar2* src, size_t n, event_t e);
#if __OPENCL_C_VERSION__ >= 110
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local uchar3* dest, const global uchar3* src, size_t n, event_t e);
#endif
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local uchar4* dest, const global uchar4* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local uchar8* dest, const global uchar8* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local uchar16* dest, const global uchar16* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local short* dest, const global short* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local short2* dest, const global short2* src, size_t n, event_t e);
#if __OPENCL_C_VERSION__ >= 110
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local short3* dest, const global short3* src, size_t n, event_t e);
#endif
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local short4* dest, const global short4* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local short8* dest, const global short8* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local short16* dest, const global short16* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local ushort* dest, const global ushort* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local ushort2* dest, const global ushort2* src, size_t n, event_t e);
#if __OPENCL_C_VERSION__ >= 110
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local ushort3* dest, const global ushort3* src, size_t n, event_t e);
#endif
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local ushort4* dest, const global ushort4* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local ushort8* dest, const global ushort8* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local ushort16* dest, const global ushort16* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local int* dest, const global int* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local int2* dest, const global int2* src, size_t n, event_t e);
#if __OPENCL_C_VERSION__ >= 110
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local int3* dest, const global int3* src, size_t n, event_t e);
#endif
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local int4* dest, const global int4* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local int8* dest, const global int8* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local int16* dest, const global int16* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local uint* dest, const global uint* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local uint2* dest, const global uint2* src, size_t n, event_t e);
#if __OPENCL_C_VERSION__ >= 110
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local uint3* dest, const global uint3* src, size_t n, event_t e);
#endif
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local uint4* dest, const global uint4* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local uint8* dest, const global uint8* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local uint16* dest, const global uint16* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local long* dest, const global long* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local long2* dest, const global long2* src, size_t n, event_t e);
#if __OPENCL_C_VERSION__ >= 110
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local long3* dest, const global long3* src, size_t n, event_t e);
#endif
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local long4* dest, const global long4* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local long8* dest, const global long8* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local long16* dest, const global long16* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local ulong* dest, const global ulong* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local ulong2* dest, const global ulong2* src, size_t n, event_t e);
#if __OPENCL_C_VERSION__ >= 110
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local ulong3* dest, const global ulong3* src, size_t n, event_t e);
#endif
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local ulong4* dest, const global ulong4* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local ulong8* dest, const global ulong8* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local ulong16* dest, const global ulong16* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local float* dest, const global float* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local float2* dest, const global float2* src, size_t n, event_t e);
#if __OPENCL_C_VERSION__ >= 110
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local float3* dest, const global float3* src, size_t n, event_t e);
#endif
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local float4* dest, const global float4* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local float8* dest, const global float8* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local float16* dest, const global float16* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local double* dest, const global double* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local double2* dest, const global double2* src, size_t n, event_t e);
#if __OPENCL_C_VERSION__ >= 110
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local double3* dest, const global double3* src, size_t n, event_t e);
#endif
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local double4* dest, const global double4* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local double8* dest, const global double8* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(local double16* dest, const global double16* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global char* dest, const local char* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global char2* dest, const local char2* src, size_t n, event_t e);
#if __OPENCL_C_VERSION__ >= 110
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global char3* dest, const local char3* src, size_t n, event_t e);
#endif
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global char4* dest, const local char4* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global char8* dest, const local char8* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global char16* dest, const local char16* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global uchar* dest, const local uchar* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global uchar2* dest, const local uchar2* src, size_t n, event_t e);
#if __OPENCL_C_VERSION__ >= 110
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global uchar3* dest, const local uchar3* src, size_t n, event_t e);
#endif
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global uchar4* dest, const local uchar4* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global uchar8* dest, const local uchar8* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global uchar16* dest, const local uchar16* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global short* dest, const local short* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global short2* dest, const local short2* src, size_t n, event_t e);
#if __OPENCL_C_VERSION__ >= 110
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global short3* dest, const local short3* src, size_t n, event_t e);
#endif
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global short4* dest, const local short4* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global short8* dest, const local short8* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global short16* dest, const local short16* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global ushort* dest, const local ushort* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global ushort2* dest, const local ushort2* src, size_t n, event_t e);
#if __OPENCL_C_VERSION__ >= 110
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global ushort3* dest, const local ushort3* src, size_t n, event_t e);
#endif
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global ushort4* dest, const local ushort4* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global ushort8* dest, const local ushort8* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global ushort16* dest, const local ushort16* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global int* dest, const local int* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global int2* dest, const local int2* src, size_t n, event_t e);
#if __OPENCL_C_VERSION__ >= 110
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global int3* dest, const local int3* src, size_t n, event_t e);
#endif
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global int4* dest, const local int4* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global int8* dest, const local int8* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global int16* dest, const local int16* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global uint* dest, const local uint* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global uint2* dest, const local uint2* src, size_t n, event_t e);
#if __OPENCL_C_VERSION__ >= 110
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global uint3* dest, const local uint3* src, size_t n, event_t e);
#endif
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global uint4* dest, const local uint4* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global uint8* dest, const local uint8* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global uint16* dest, const local uint16* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global long* dest, const local long* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global long2* dest, const local long2* src, size_t n, event_t e);
#if __OPENCL_C_VERSION__ >= 110
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global long3* dest, const local long3* src, size_t n, event_t e);
#endif
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global long4* dest, const local long4* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global long8* dest, const local long8* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global long16* dest, const local long16* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global ulong* dest, const local ulong* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global ulong2* dest, const local ulong2* src, size_t n, event_t e);
#if __OPENCL_C_VERSION__ >= 110
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global ulong3* dest, const local ulong3* src, size_t n, event_t e);
#endif
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global ulong4* dest, const local ulong4* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global ulong8* dest, const local ulong8* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global ulong16* dest, const local ulong16* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global float* dest, const local float* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global float2* dest, const local float2* src, size_t n, event_t e);
#if __OPENCL_C_VERSION__ >= 110
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global float3* dest, const local float3* src, size_t n, event_t e);
#endif
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global float4* dest, const local float4* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global float8* dest, const local float8* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global float16* dest, const local float16* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global double* dest, const local double* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global double2* dest, const local double2* src, size_t n, event_t e);
#if __OPENCL_C_VERSION__ >= 110
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global double3* dest, const local double3* src, size_t n, event_t e);
#endif
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global double4* dest, const local double4* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global double8* dest, const local double8* src, size_t n, event_t e);
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_copy(global double16* dest, const local double16* src, size_t n, event_t e);
#if __OPENCL_C_VERSION__ >= 110
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local char* dest, const global char* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local char2* dest, const global char2* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local char3* dest, const global char3* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local char4* dest, const global char4* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local char8* dest, const global char8* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local char16* dest, const global char16* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local uchar* dest, const global uchar* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local uchar2* dest, const global uchar2* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local uchar3* dest, const global uchar3* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local uchar4* dest, const global uchar4* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local uchar8* dest, const global uchar8* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local uchar16* dest, const global uchar16* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local short* dest, const global short* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local short2* dest, const global short2* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local short3* dest, const global short3* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local short4* dest, const global short4* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local short8* dest, const global short8* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local short16* dest, const global short16* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local ushort* dest, const global ushort* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local ushort2* dest, const global ushort2* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local ushort3* dest, const global ushort3* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local ushort4* dest, const global ushort4* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local ushort8* dest, const global ushort8* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local ushort16* dest, const global ushort16* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local int* dest, const global int* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local int2* dest, const global int2* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local int3* dest, const global int3* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local int4* dest, const global int4* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local int8* dest, const global int8* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local int16* dest, const global int16* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local uint* dest, const global uint* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local uint2* dest, const global uint2* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local uint3* dest, const global uint3* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local uint4* dest, const global uint4* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local uint8* dest, const global uint8* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local uint16* dest, const global uint16* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local long* dest, const global long* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local long2* dest, const global long2* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local long3* dest, const global long3* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local long4* dest, const global long4* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local long8* dest, const global long8* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local long16* dest, const global long16* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local ulong* dest, const global ulong* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local ulong2* dest, const global ulong2* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local ulong3* dest, const global ulong3* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local ulong4* dest, const global ulong4* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local ulong8* dest, const global ulong8* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local ulong16* dest, const global ulong16* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local float* dest, const global float* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local float2* dest, const global float2* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local float3* dest, const global float3* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local float4* dest, const global float4* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local float8* dest, const global float8* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local float16* dest, const global float16* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local double* dest, const global double* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local double2* dest, const global double2* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local double3* dest, const global double3* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local double4* dest, const global double4* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local double8* dest, const global double8* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(local double16* dest, const global double16* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global char* dest, const local char* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global char2* dest, const local char2* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global char3* dest, const local char3* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global char4* dest, const local char4* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global char8* dest, const local char8* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global char16* dest, const local char16* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global uchar* dest, const local uchar* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global uchar2* dest, const local uchar2* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global uchar3* dest, const local uchar3* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global uchar4* dest, const local uchar4* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global uchar8* dest, const local uchar8* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global uchar16* dest, const local uchar16* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global short* dest, const local short* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global short2* dest, const local short2* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global short3* dest, const local short3* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global short4* dest, const local short4* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global short8* dest, const local short8* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global short16* dest, const local short16* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global ushort* dest, const local ushort* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global ushort2* dest, const local ushort2* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global ushort3* dest, const local ushort3* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global ushort4* dest, const local ushort4* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global ushort8* dest, const local ushort8* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global ushort16* dest, const local ushort16* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global int* dest, const local int* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global int2* dest, const local int2* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global int3* dest, const local int3* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global int4* dest, const local int4* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global int8* dest, const local int8* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global int16* dest, const local int16* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global uint* dest, const local uint* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global uint2* dest, const local uint2* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global uint3* dest, const local uint3* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global uint4* dest, const local uint4* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global uint8* dest, const local uint8* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global uint16* dest, const local uint16* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global long* dest, const local long* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global long2* dest, const local long2* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global long3* dest, const local long3* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global long4* dest, const local long4* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global long8* dest, const local long8* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global long16* dest, const local long16* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global ulong* dest, const local ulong* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global ulong2* dest, const local ulong2* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global ulong3* dest, const local ulong3* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global ulong4* dest, const local ulong4* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global ulong8* dest, const local ulong8* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global ulong16* dest, const local ulong16* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global float* dest, const local float* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global float2* dest, const local float2* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global float3* dest, const local float3* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global float4* dest, const local float4* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global float8* dest, const local float8* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global float16* dest, const local float16* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global double* dest, const local double* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global double2* dest, const local double2* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global double3* dest, const local double3* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global double4* dest, const local double4* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global double8* dest, const local double8* src, size_t n, size_t stride, event_t e );
event_t  __attribute__((__overloadable__,__always_inline__)) async_work_group_strided_copy(global double16* dest, const local double16* src, size_t n, size_t stride, event_t e );
#endif
void  __attribute__((__overloadable__,__always_inline__))  wait_group_events(int n, event_t *events);
void  __attribute__((__overloadable__,__always_inline__))  wait_group_events(int n, event_t __attribute__((address_space(4)))*events);
float __attribute__((__overloadable__,__always_inline__)) sincos(float x, global float* cosval );
float2 __attribute__((__overloadable__,__always_inline__)) sincos(float2 x, global float2* cosval );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__,__always_inline__)) sincos(float3 x, global float3* cosval );
#endif
float4 __attribute__((__overloadable__,__always_inline__)) sincos(float4 x, global float4* cosval );
float8 __attribute__((__overloadable__,__always_inline__)) sincos(float8 x, global float8* cosval );
float16 __attribute__((__overloadable__,__always_inline__)) sincos(float16 x, global float16* cosval );
float __attribute__((__overloadable__,__always_inline__)) sincos(float x, local float* cosval );
float2 __attribute__((__overloadable__,__always_inline__)) sincos(float2 x, local float2* cosval );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__,__always_inline__)) sincos(float3 x, local float3* cosval );
#endif
float4 __attribute__((__overloadable__,__always_inline__)) sincos(float4 x, local float4* cosval );
float8 __attribute__((__overloadable__,__always_inline__)) sincos(float8 x, local float8* cosval );
float16 __attribute__((__overloadable__,__always_inline__)) sincos(float16 x, local float16* cosval );
float __attribute__((__overloadable__,__always_inline__)) sincos(float x,  float* cosval );
float2 __attribute__((__overloadable__,__always_inline__)) sincos(float2 x,  float2* cosval );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__,__always_inline__)) sincos(float3 x,  float3* cosval );
#endif
float4 __attribute__((__overloadable__,__always_inline__)) sincos(float4 x,  float4* cosval );
float8 __attribute__((__overloadable__,__always_inline__)) sincos(float8 x,  float8* cosval );
float16 __attribute__((__overloadable__,__always_inline__)) sincos(float16 x,  float16* cosval );
double __attribute__((__overloadable__,__always_inline__)) sincos(double x, global double* cosval );
double2 __attribute__((__overloadable__,__always_inline__)) sincos(double2 x, global double2* cosval );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__,__always_inline__)) sincos(double3 x, global double3* cosval );
#endif
double4 __attribute__((__overloadable__,__always_inline__)) sincos(double4 x, global double4* cosval );
double8 __attribute__((__overloadable__,__always_inline__)) sincos(double8 x, global double8* cosval );
double16 __attribute__((__overloadable__,__always_inline__)) sincos(double16 x, global double16* cosval );
double __attribute__((__overloadable__,__always_inline__)) sincos(double x, local double* cosval );
double2 __attribute__((__overloadable__,__always_inline__)) sincos(double2 x, local double2* cosval );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__,__always_inline__)) sincos(double3 x, local double3* cosval );
#endif
double4 __attribute__((__overloadable__,__always_inline__)) sincos(double4 x, local double4* cosval );
double8 __attribute__((__overloadable__,__always_inline__)) sincos(double8 x, local double8* cosval );
double16 __attribute__((__overloadable__,__always_inline__)) sincos(double16 x, local double16* cosval );
double __attribute__((__overloadable__,__always_inline__)) sincos(double x,  double* cosval );
double2 __attribute__((__overloadable__,__always_inline__)) sincos(double2 x,  double2* cosval );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__,__always_inline__)) sincos(double3 x,  double3* cosval );
#endif
double4 __attribute__((__overloadable__,__always_inline__)) sincos(double4 x,  double4* cosval );
double8 __attribute__((__overloadable__,__always_inline__)) sincos(double8 x,  double8* cosval );
double16 __attribute__((__overloadable__,__always_inline__)) sincos(double16 x,  double16* cosval );
float __attribute__((__overloadable__,__always_inline__)) modf(float x,  float *intpart);
float2 __attribute__((__overloadable__,__always_inline__)) modf(float2 x,  float2 *iptr );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__,__always_inline__)) modf(float3 x,  float3 *iptr );
#endif
float4 __attribute__((__overloadable__,__always_inline__)) modf(float4 x,  float4 *iptr );
float8 __attribute__((__overloadable__,__always_inline__)) modf(float8 x,  float8 *iptr );
float16 __attribute__((__overloadable__,__always_inline__)) modf(float16 x,  float16 *iptr );
float __attribute__((__overloadable__,__always_inline__)) modf(float x, global float *intpart);
float2 __attribute__((__overloadable__,__always_inline__)) modf(float2 x, global float2 *iptr );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__,__always_inline__)) modf(float3 x, global float3 *iptr );
#endif
float4 __attribute__((__overloadable__,__always_inline__)) modf(float4 x, global float4 *iptr );
float8 __attribute__((__overloadable__,__always_inline__)) modf(float8 x, global float8 *iptr );
float16 __attribute__((__overloadable__,__always_inline__)) modf(float16 x, global float16 *iptr );
float __attribute__((__overloadable__,__always_inline__)) modf(float x, local float *intpart);
float2 __attribute__((__overloadable__,__always_inline__)) modf(float2 x, local float2 *iptr );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__,__always_inline__)) modf(float3 x, local float3 *iptr );
#endif
float4 __attribute__((__overloadable__,__always_inline__)) modf(float4 x, local float4 *iptr );
float8 __attribute__((__overloadable__,__always_inline__)) modf(float8 x, local float8 *iptr );
float16 __attribute__((__overloadable__,__always_inline__)) modf(float16 x, local float16 *iptr );
double __attribute__((__overloadable__,__always_inline__)) modf(double x,  double *intpart);
double2 __attribute__((__overloadable__,__always_inline__)) modf(double2 x,  double2 *iptr );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__,__always_inline__)) modf(double3 x,  double3 *iptr );
#endif
double4 __attribute__((__overloadable__,__always_inline__)) modf(double4 x,  double4 *iptr );
double8 __attribute__((__overloadable__,__always_inline__)) modf(double8 x,  double8 *iptr );
double16 __attribute__((__overloadable__,__always_inline__)) modf(double16 x,  double16 *iptr );
double __attribute__((__overloadable__,__always_inline__)) modf(double x, global double *intpart);
double2 __attribute__((__overloadable__,__always_inline__)) modf(double2 x, global double2 *iptr );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__,__always_inline__)) modf(double3 x, global double3 *iptr );
#endif
double4 __attribute__((__overloadable__,__always_inline__)) modf(double4 x, global double4 *iptr );
double8 __attribute__((__overloadable__,__always_inline__)) modf(double8 x, global double8 *iptr );
double16 __attribute__((__overloadable__,__always_inline__)) modf(double16 x, global double16 *iptr );
double __attribute__((__overloadable__,__always_inline__)) modf(double x, local double *intpart);
double2 __attribute__((__overloadable__,__always_inline__)) modf(double2 x, local double2 *iptr );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__,__always_inline__)) modf(double3 x, local double3 *iptr );
#endif
double4 __attribute__((__overloadable__,__always_inline__)) modf(double4 x, local double4 *iptr );
double8 __attribute__((__overloadable__,__always_inline__)) modf(double8 x, local double8 *iptr );
double16 __attribute__((__overloadable__,__always_inline__)) modf(double16 x, local double16 *iptr );
float __attribute__((__overloadable__,__always_inline__,const)) fract(float x,  float *iptr);
float2 __attribute__((__overloadable__,__always_inline__,const)) fract(float2 x,  float2 *iptr );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__,__always_inline__,const)) fract(float3 x,  float3 *iptr );
#endif
float4 __attribute__((__overloadable__,__always_inline__,const)) fract(float4 x,  float4 *iptr );
float8 __attribute__((__overloadable__,__always_inline__,const)) fract(float8 x,  float8 *iptr );
float16 __attribute__((__overloadable__,__always_inline__,const)) fract(float16 x,  float16 *iptr );
float __attribute__((__overloadable__,__always_inline__,const)) fract(float x, global float *iptr);
float2 __attribute__((__overloadable__,__always_inline__,const)) fract(float2 x, global float2 *iptr );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__,__always_inline__,const)) fract(float3 x, global float3 *iptr );
#endif
float4 __attribute__((__overloadable__,__always_inline__,const)) fract(float4 x, global float4 *iptr );
float8 __attribute__((__overloadable__,__always_inline__,const)) fract(float8 x, global float8 *iptr );
float16 __attribute__((__overloadable__,__always_inline__,const)) fract(float16 x, global float16 *iptr );
float __attribute__((__overloadable__,__always_inline__,const)) fract(float x, local float *iptr);
float2 __attribute__((__overloadable__,__always_inline__,const)) fract(float2 x, local float2 *iptr );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__,__always_inline__,const)) fract(float3 x, local float3 *iptr );
#endif
float4 __attribute__((__overloadable__,__always_inline__,const)) fract(float4 x, local float4 *iptr );
float8 __attribute__((__overloadable__,__always_inline__,const)) fract(float8 x, local float8 *iptr );
float16 __attribute__((__overloadable__,__always_inline__,const)) fract(float16 x, local float16 *iptr );
double __attribute__((__overloadable__,__always_inline__,const)) fract(double x,  double *iptr);
double2 __attribute__((__overloadable__,__always_inline__,const)) fract(double2 x,  double2 *iptr );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__,__always_inline__,const)) fract(double3 x,  double3 *iptr );
#endif
double4 __attribute__((__overloadable__,__always_inline__,const)) fract(double4 x,  double4 *iptr );
double8 __attribute__((__overloadable__,__always_inline__,const)) fract(double8 x,  double8 *iptr );
double16 __attribute__((__overloadable__,__always_inline__,const)) fract(double16 x,  double16 *iptr );
double __attribute__((__overloadable__,__always_inline__,const)) fract(double x, global double *iptr);
double2 __attribute__((__overloadable__,__always_inline__,const)) fract(double2 x, global double2 *iptr );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__,__always_inline__,const)) fract(double3 x, global double3 *iptr );
#endif
double4 __attribute__((__overloadable__,__always_inline__,const)) fract(double4 x, global double4 *iptr );
double8 __attribute__((__overloadable__,__always_inline__,const)) fract(double8 x, global double8 *iptr );
double16 __attribute__((__overloadable__,__always_inline__,const)) fract(double16 x, global double16 *iptr );
double __attribute__((__overloadable__,__always_inline__,const)) fract(double x, local double *iptr);
double2 __attribute__((__overloadable__,__always_inline__,const)) fract(double2 x, local double2 *iptr );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__,__always_inline__,const)) fract(double3 x, local double3 *iptr );
#endif
double4 __attribute__((__overloadable__,__always_inline__,const)) fract(double4 x, local double4 *iptr );
double8 __attribute__((__overloadable__,__always_inline__,const)) fract(double8 x, local double8 *iptr );
double16 __attribute__((__overloadable__,__always_inline__,const)) fract(double16 x, local double16 *iptr );
float __attribute__((__overloadable__,__always_inline__,const)) lgamma_r(float x, global int* signp );
float2 __attribute__((__overloadable__,__always_inline__,const)) lgamma_r(float2 x, global int2* signp);
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__,__always_inline__,const)) lgamma_r(float3 x, global int3* signp);
#endif
float4 __attribute__((__overloadable__,__always_inline__,const)) lgamma_r(float4 x, global int4* signp);
float8 __attribute__((__overloadable__,__always_inline__,const)) lgamma_r(float8 x, global int8* signp);
float16 __attribute__((__overloadable__,__always_inline__,const)) lgamma_r(float16 x, global int16* signp);
float __attribute__((__overloadable__,__always_inline__,const)) lgamma_r(float x, local int* signp );
float2 __attribute__((__overloadable__,__always_inline__,const)) lgamma_r(float2 x, local int2* signp);
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__,__always_inline__,const)) lgamma_r(float3 x, local int3* signp);
#endif
float4 __attribute__((__overloadable__,__always_inline__,const)) lgamma_r(float4 x, local int4* signp);
float8 __attribute__((__overloadable__,__always_inline__,const)) lgamma_r(float8 x, local int8* signp);
float16 __attribute__((__overloadable__,__always_inline__,const)) lgamma_r(float16 x, local int16* signp);
float __attribute__((__overloadable__,__always_inline__,const)) lgamma_r(float x,  int* signp );
float2 __attribute__((__overloadable__,__always_inline__,const)) lgamma_r(float2 x,  int2* signp);
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__,__always_inline__,const)) lgamma_r(float3 x,  int3* signp);
#endif
float4 __attribute__((__overloadable__,__always_inline__,const)) lgamma_r(float4 x,  int4* signp);
float8 __attribute__((__overloadable__,__always_inline__,const)) lgamma_r(float8 x,  int8* signp);
float16 __attribute__((__overloadable__,__always_inline__,const)) lgamma_r(float16 x,  int16* signp);
double __attribute__((__overloadable__,__always_inline__,const)) lgamma_r(double x, global int* signp );
double2 __attribute__((__overloadable__,__always_inline__,const)) lgamma_r(double2 x, global int2* signp);
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__,__always_inline__,const)) lgamma_r(double3 x, global int3* signp);
#endif
double4 __attribute__((__overloadable__,__always_inline__,const)) lgamma_r(double4 x, global int4* signp);
double8 __attribute__((__overloadable__,__always_inline__,const)) lgamma_r(double8 x, global int8* signp);
double16 __attribute__((__overloadable__,__always_inline__,const)) lgamma_r(double16 x, global int16* signp);
double __attribute__((__overloadable__,__always_inline__,const)) lgamma_r(double x, local int* signp );
double2 __attribute__((__overloadable__,__always_inline__,const)) lgamma_r(double2 x, local int2* signp);
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__,__always_inline__,const)) lgamma_r(double3 x, local int3* signp);
#endif
double4 __attribute__((__overloadable__,__always_inline__,const)) lgamma_r(double4 x, local int4* signp);
double8 __attribute__((__overloadable__,__always_inline__,const)) lgamma_r(double8 x, local int8* signp);
double16 __attribute__((__overloadable__,__always_inline__,const)) lgamma_r(double16 x, local int16* signp);
double __attribute__((__overloadable__,__always_inline__,const)) lgamma_r(double x,  int* signp );
double2 __attribute__((__overloadable__,__always_inline__,const)) lgamma_r(double2 x,  int2* signp);
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__,__always_inline__,const)) lgamma_r(double3 x,  int3* signp);
#endif
double4 __attribute__((__overloadable__,__always_inline__,const)) lgamma_r(double4 x,  int4* signp);
double8 __attribute__((__overloadable__,__always_inline__,const)) lgamma_r(double8 x,  int8* signp);
double16 __attribute__((__overloadable__,__always_inline__,const)) lgamma_r(double16 x,  int16* signp);
float __acl_frexpf(float x, private int *exponent);
double __acl_frexpd(double x, private int *exponent);
float __attribute__((__overloadable__,__always_inline__,const)) frexp(float x,  int *exponent);
float2 __attribute__((__overloadable__,__always_inline__,const)) frexp(float2 x,  int2 *exponent );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__,__always_inline__,const)) frexp(float3 x,  int3 *exponent );
#endif
float4 __attribute__((__overloadable__,__always_inline__,const)) frexp(float4 x,  int4 *exponent );
float8 __attribute__((__overloadable__,__always_inline__,const)) frexp(float8 x,  int8 *exponent );
float16 __attribute__((__overloadable__,__always_inline__,const)) frexp(float16 x,  int16 *exponent );
float __attribute__((__overloadable__,__always_inline__,const)) frexp(float x, global int *exponent);
float2 __attribute__((__overloadable__,__always_inline__,const)) frexp(float2 x, global int2 *exponent );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__,__always_inline__,const)) frexp(float3 x, global int3 *exponent );
#endif
float4 __attribute__((__overloadable__,__always_inline__,const)) frexp(float4 x, global int4 *exponent );
float8 __attribute__((__overloadable__,__always_inline__,const)) frexp(float8 x, global int8 *exponent );
float16 __attribute__((__overloadable__,__always_inline__,const)) frexp(float16 x, global int16 *exponent );
float __attribute__((__overloadable__,__always_inline__,const)) frexp(float x, local int *exponent);
float2 __attribute__((__overloadable__,__always_inline__,const)) frexp(float2 x, local int2 *exponent );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__,__always_inline__,const)) frexp(float3 x, local int3 *exponent );
#endif
float4 __attribute__((__overloadable__,__always_inline__,const)) frexp(float4 x, local int4 *exponent );
float8 __attribute__((__overloadable__,__always_inline__,const)) frexp(float8 x, local int8 *exponent );
float16 __attribute__((__overloadable__,__always_inline__,const)) frexp(float16 x, local int16 *exponent );
double __attribute__((__overloadable__,__always_inline__,const)) frexp(double x,  int *exponent);
double2 __attribute__((__overloadable__,__always_inline__,const)) frexp(double2 x,  int2 *exponent );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__,__always_inline__,const)) frexp(double3 x,  int3 *exponent );
#endif
double4 __attribute__((__overloadable__,__always_inline__,const)) frexp(double4 x,  int4 *exponent );
double8 __attribute__((__overloadable__,__always_inline__,const)) frexp(double8 x,  int8 *exponent );
double16 __attribute__((__overloadable__,__always_inline__,const)) frexp(double16 x,  int16 *exponent );
double __attribute__((__overloadable__,__always_inline__,const)) frexp(double x, global int *exponent);
double2 __attribute__((__overloadable__,__always_inline__,const)) frexp(double2 x, global int2 *exponent );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__,__always_inline__,const)) frexp(double3 x, global int3 *exponent );
#endif
double4 __attribute__((__overloadable__,__always_inline__,const)) frexp(double4 x, global int4 *exponent );
double8 __attribute__((__overloadable__,__always_inline__,const)) frexp(double8 x, global int8 *exponent );
double16 __attribute__((__overloadable__,__always_inline__,const)) frexp(double16 x, global int16 *exponent );
double __attribute__((__overloadable__,__always_inline__,const)) frexp(double x, local int *exponent);
double2 __attribute__((__overloadable__,__always_inline__,const)) frexp(double2 x, local int2 *exponent );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__,__always_inline__,const)) frexp(double3 x, local int3 *exponent );
#endif
double4 __attribute__((__overloadable__,__always_inline__,const)) frexp(double4 x, local int4 *exponent );
double8 __attribute__((__overloadable__,__always_inline__,const)) frexp(double8 x, local int8 *exponent );
double16 __attribute__((__overloadable__,__always_inline__,const)) frexp(double16 x, local int16 *exponent );
float __attribute__((const)) __acl__roundf(float x);
float __attribute__((__overloadable__,__always_inline__,const)) round(float x);
float2 __attribute__((__overloadable__,__always_inline__,const)) round(float2 x );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__,__always_inline__,const)) round(float3 x );
#endif
float4 __attribute__((__overloadable__,__always_inline__,const)) round(float4 x );
float8 __attribute__((__overloadable__,__always_inline__,const)) round(float8 x );
float16 __attribute__((__overloadable__,__always_inline__,const)) round(float16 x );
double __attribute__((const)) __acl__roundfd(double x);
double __attribute__((__overloadable__,__always_inline__,const)) round(double x);
double2 __attribute__((__overloadable__,__always_inline__,const)) round(double2 x );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__,__always_inline__,const)) round(double3 x );
#endif
double4 __attribute__((__overloadable__,__always_inline__,const)) round(double4 x );
double8 __attribute__((__overloadable__,__always_inline__,const)) round(double8 x );
double16 __attribute__((__overloadable__,__always_inline__,const)) round(double16 x );
float __attribute__((__overloadable__,__always_inline__,const)) nextafter(float x, float y);
float2 __attribute__((__overloadable__,__always_inline__,const)) nextafter(float2 x,
float2 y );
#if __OPENCL_C_VERSION__ >= 110
float3 __attribute__((__overloadable__,__always_inline__,const)) nextafter(float3 x,
float3 y );
#endif
float4 __attribute__((__overloadable__,__always_inline__,const)) nextafter(float4 x,
float4 y );
float8 __attribute__((__overloadable__,__always_inline__,const)) nextafter(float8 x,
float8 y );
float16 __attribute__((__overloadable__,__always_inline__,const)) nextafter(float16 x,
float16 y );
double __attribute__((__overloadable__,__always_inline__,const)) nextafter(double x, double y);
double2 __attribute__((__overloadable__,__always_inline__,const)) nextafter(double2 x,
double2 y );
#if __OPENCL_C_VERSION__ >= 110
double3 __attribute__((__overloadable__,__always_inline__,const)) nextafter(double3 x,
double3 y );
#endif
double4 __attribute__((__overloadable__,__always_inline__,const)) nextafter(double4 x,
double4 y );
double8 __attribute__((__overloadable__,__always_inline__,const)) nextafter(double8 x,
double8 y );
double16 __attribute__((__overloadable__,__always_inline__,const)) nextafter(double16 x,
double16 y );
int __attribute__((__overloadable__)) atom_add( volatile global int *p, int val);
#if __OPENCL_C_VERSION__ >= 110
int __attribute__((__overloadable__)) atomic_add( volatile global int *p, int val);
#endif
int __attribute__((__overloadable__)) atom_add( volatile local int *p, int val);
#if __OPENCL_C_VERSION__ >= 110
int __attribute__((__overloadable__)) atomic_add( volatile local int *p, int val);
#endif
unsigned int __attribute__((__overloadable__)) atom_add( volatile global unsigned int *p, unsigned int val);
#if __OPENCL_C_VERSION__ >= 110
unsigned int __attribute__((__overloadable__)) atomic_add( volatile global unsigned int *p, unsigned int val);
#endif
unsigned int __attribute__((__overloadable__)) atom_add( volatile local unsigned int *p, unsigned int val);
#if __OPENCL_C_VERSION__ >= 110
unsigned int __attribute__((__overloadable__)) atomic_add( volatile local unsigned int *p, unsigned int val);
#endif
int __attribute__((__overloadable__)) atom_sub( volatile global int *p, int val);
#if __OPENCL_C_VERSION__ >= 110
int __attribute__((__overloadable__)) atomic_sub( volatile global int *p, int val);
#endif
int __attribute__((__overloadable__)) atom_sub( volatile local int *p, int val);
#if __OPENCL_C_VERSION__ >= 110
int __attribute__((__overloadable__)) atomic_sub( volatile local int *p, int val);
#endif
unsigned int __attribute__((__overloadable__)) atom_sub( volatile global unsigned int *p, unsigned int val);
#if __OPENCL_C_VERSION__ >= 110
unsigned int __attribute__((__overloadable__)) atomic_sub( volatile global unsigned int *p, unsigned int val);
#endif
unsigned int __attribute__((__overloadable__)) atom_sub( volatile local unsigned int *p, unsigned int val);
#if __OPENCL_C_VERSION__ >= 110
unsigned int __attribute__((__overloadable__)) atomic_sub( volatile local unsigned int *p, unsigned int val);
#endif
int __attribute__((__overloadable__)) atom_xchg( volatile global int *p, int val);
#if __OPENCL_C_VERSION__ >= 110
int __attribute__((__overloadable__)) atomic_xchg( volatile global int *p, int val);
#endif
int __attribute__((__overloadable__)) atom_xchg( volatile local int *p, int val);
#if __OPENCL_C_VERSION__ >= 110
int __attribute__((__overloadable__)) atomic_xchg( volatile local int *p, int val);
#endif
unsigned int __attribute__((__overloadable__)) atom_xchg( volatile global unsigned int *p, unsigned int val);
#if __OPENCL_C_VERSION__ >= 110
unsigned int __attribute__((__overloadable__)) atomic_xchg( volatile global unsigned int *p, unsigned int val);
#endif
unsigned int __attribute__((__overloadable__)) atom_xchg( volatile local unsigned int *p, unsigned int val);
#if __OPENCL_C_VERSION__ >= 110
unsigned int __attribute__((__overloadable__)) atomic_xchg( volatile local unsigned int *p, unsigned int val);
#endif
int __attribute__((__overloadable__)) atom_min( volatile global int *p, int val);
#if __OPENCL_C_VERSION__ >= 110
int __attribute__((__overloadable__)) atomic_min( volatile global int *p, int val);
#endif
int __attribute__((__overloadable__)) atom_min( volatile local int *p, int val);
#if __OPENCL_C_VERSION__ >= 110
int __attribute__((__overloadable__)) atomic_min( volatile local int *p, int val);
#endif
unsigned int __attribute__((__overloadable__)) atom_min( volatile global unsigned int *p, unsigned int val);
#if __OPENCL_C_VERSION__ >= 110
unsigned int __attribute__((__overloadable__)) atomic_min( volatile global unsigned int *p, unsigned int val);
#endif
unsigned int __attribute__((__overloadable__)) atom_min( volatile local unsigned int *p, unsigned int val);
#if __OPENCL_C_VERSION__ >= 110
unsigned int __attribute__((__overloadable__)) atomic_min( volatile local unsigned int *p, unsigned int val);
#endif
int __attribute__((__overloadable__)) atom_max( volatile global int *p, int val);
#if __OPENCL_C_VERSION__ >= 110
int __attribute__((__overloadable__)) atomic_max( volatile global int *p, int val);
#endif
int __attribute__((__overloadable__)) atom_max( volatile local int *p, int val);
#if __OPENCL_C_VERSION__ >= 110
int __attribute__((__overloadable__)) atomic_max( volatile local int *p, int val);
#endif
unsigned int __attribute__((__overloadable__)) atom_max( volatile global unsigned int *p, unsigned int val);
#if __OPENCL_C_VERSION__ >= 110
unsigned int __attribute__((__overloadable__)) atomic_max( volatile global unsigned int *p, unsigned int val);
#endif
unsigned int __attribute__((__overloadable__)) atom_max( volatile local unsigned int *p, unsigned int val);
#if __OPENCL_C_VERSION__ >= 110
unsigned int __attribute__((__overloadable__)) atomic_max( volatile local unsigned int *p, unsigned int val);
#endif
int __attribute__((__overloadable__)) atom_and( volatile global int *p, int val);
#if __OPENCL_C_VERSION__ >= 110
int __attribute__((__overloadable__)) atomic_and( volatile global int *p, int val);
#endif
int __attribute__((__overloadable__)) atom_and( volatile local int *p, int val);
#if __OPENCL_C_VERSION__ >= 110
int __attribute__((__overloadable__)) atomic_and( volatile local int *p, int val);
#endif
unsigned int __attribute__((__overloadable__)) atom_and( volatile global unsigned int *p, unsigned int val);
#if __OPENCL_C_VERSION__ >= 110
unsigned int __attribute__((__overloadable__)) atomic_and( volatile global unsigned int *p, unsigned int val);
#endif
unsigned int __attribute__((__overloadable__)) atom_and( volatile local unsigned int *p, unsigned int val);
#if __OPENCL_C_VERSION__ >= 110
unsigned int __attribute__((__overloadable__)) atomic_and( volatile local unsigned int *p, unsigned int val);
#endif
int __attribute__((__overloadable__)) atom_or( volatile global int *p, int val);
#if __OPENCL_C_VERSION__ >= 110
int __attribute__((__overloadable__)) atomic_or( volatile global int *p, int val);
#endif
int __attribute__((__overloadable__)) atom_or( volatile local int *p, int val);
#if __OPENCL_C_VERSION__ >= 110
int __attribute__((__overloadable__)) atomic_or( volatile local int *p, int val);
#endif
unsigned int __attribute__((__overloadable__)) atom_or( volatile global unsigned int *p, unsigned int val);
#if __OPENCL_C_VERSION__ >= 110
unsigned int __attribute__((__overloadable__)) atomic_or( volatile global unsigned int *p, unsigned int val);
#endif
unsigned int __attribute__((__overloadable__)) atom_or( volatile local unsigned int *p, unsigned int val);
#if __OPENCL_C_VERSION__ >= 110
unsigned int __attribute__((__overloadable__)) atomic_or( volatile local unsigned int *p, unsigned int val);
#endif
int __attribute__((__overloadable__)) atom_xor( volatile global int *p, int val);
#if __OPENCL_C_VERSION__ >= 110
int __attribute__((__overloadable__)) atomic_xor( volatile global int *p, int val);
#endif
int __attribute__((__overloadable__)) atom_xor( volatile local int *p, int val);
#if __OPENCL_C_VERSION__ >= 110
int __attribute__((__overloadable__)) atomic_xor( volatile local int *p, int val);
#endif
unsigned int __attribute__((__overloadable__)) atom_xor( volatile global unsigned int *p, unsigned int val);
#if __OPENCL_C_VERSION__ >= 110
unsigned int __attribute__((__overloadable__)) atomic_xor( volatile global unsigned int *p, unsigned int val);
#endif
unsigned int __attribute__((__overloadable__)) atom_xor( volatile local unsigned int *p, unsigned int val);
#if __OPENCL_C_VERSION__ >= 110
unsigned int __attribute__((__overloadable__)) atomic_xor( volatile local unsigned int *p, unsigned int val);
#endif
float __attribute__((__overloadable__)) atom_xchg( volatile global float *p, float val );
#if __OPENCL_C_VERSION__ >= 110
float __attribute__((__overloadable__)) atomic_xchg( volatile global float *p, float val );
#endif
float __attribute__((__overloadable__)) atom_xchg( volatile local float *p, float val );
#if __OPENCL_C_VERSION__ >= 110
float __attribute__((__overloadable__)) atomic_xchg( volatile local float *p, float val );
#endif
int __attribute__((__overloadable__)) atom_inc( volatile global int *p );
#if __OPENCL_C_VERSION__ >= 110
int __attribute__((__overloadable__)) atomic_inc( volatile global int *p );
#endif
int __attribute__((__overloadable__)) atom_inc( volatile local int *p );
#if __OPENCL_C_VERSION__ >= 110
int __attribute__((__overloadable__)) atomic_inc( volatile local int *p );
#endif
unsigned int __attribute__((__overloadable__)) atom_inc( volatile global unsigned int *p );
#if __OPENCL_C_VERSION__ >= 110
unsigned int __attribute__((__overloadable__)) atomic_inc( volatile global unsigned int *p );
#endif
unsigned int __attribute__((__overloadable__)) atom_inc( volatile local unsigned int *p );
#if __OPENCL_C_VERSION__ >= 110
unsigned int __attribute__((__overloadable__)) atomic_inc( volatile local unsigned int *p );
#endif
int __attribute__((__overloadable__)) atom_dec( volatile global int *p );
#if __OPENCL_C_VERSION__ >= 110
int __attribute__((__overloadable__)) atomic_dec( volatile global int *p );
#endif
int __attribute__((__overloadable__)) atom_dec( volatile local int *p );
#if __OPENCL_C_VERSION__ >= 110
int __attribute__((__overloadable__)) atomic_dec( volatile local int *p );
#endif
unsigned int __attribute__((__overloadable__)) atom_dec( volatile global unsigned int *p );
#if __OPENCL_C_VERSION__ >= 110
unsigned int __attribute__((__overloadable__)) atomic_dec( volatile global unsigned int *p );
#endif
unsigned int __attribute__((__overloadable__)) atom_dec( volatile local unsigned int *p );
#if __OPENCL_C_VERSION__ >= 110
unsigned int __attribute__((__overloadable__)) atomic_dec( volatile local unsigned int *p );
#endif
int  __attribute__((__overloadable__)) atom_cmpxchg( volatile global int *p, int cmp, int val);
#if __OPENCL_C_VERSION__ >= 110
int  __attribute__((__overloadable__)) atomic_cmpxchg( volatile global int *p, int cmp, int val);
#endif
int  __attribute__((__overloadable__)) atom_cmpxchg( volatile local int *p, int cmp, int val);
#if __OPENCL_C_VERSION__ >= 110
int  __attribute__((__overloadable__)) atomic_cmpxchg( volatile local int *p, int cmp, int val);
#endif
unsigned int  __attribute__((__overloadable__)) atom_cmpxchg( volatile global unsigned int *p, unsigned int cmp, unsigned int val);
#if __OPENCL_C_VERSION__ >= 110
unsigned int  __attribute__((__overloadable__)) atomic_cmpxchg( volatile global unsigned int *p, unsigned int cmp, unsigned int val);
#endif
unsigned int  __attribute__((__overloadable__)) atom_cmpxchg( volatile local unsigned int *p, unsigned int cmp, unsigned int val);
#if __OPENCL_C_VERSION__ >= 110
unsigned int  __attribute__((__overloadable__)) atomic_cmpxchg( volatile local unsigned int *p, unsigned int cmp, unsigned int val);
#endif
char __attribute__((__overloadable__,__always_inline__,const)) clz(char a );
char2 __attribute__((__overloadable__,__always_inline__,const)) clz( char2 a );
#if __OPENCL_C_VERSION__ >= 110
char3 __attribute__((__overloadable__,__always_inline__,const)) clz( char3 a );
#endif
char4 __attribute__((__overloadable__,__always_inline__,const)) clz( char4 a );
char8 __attribute__((__overloadable__,__always_inline__,const)) clz( char8 a );
char16 __attribute__((__overloadable__,__always_inline__,const)) clz( char16 a );
uchar __attribute__((__overloadable__,__always_inline__,const)) clz(uchar a );
uchar2 __attribute__((__overloadable__,__always_inline__,const)) clz( uchar2 a );
#if __OPENCL_C_VERSION__ >= 110
uchar3 __attribute__((__overloadable__,__always_inline__,const)) clz( uchar3 a );
#endif
uchar4 __attribute__((__overloadable__,__always_inline__,const)) clz( uchar4 a );
uchar8 __attribute__((__overloadable__,__always_inline__,const)) clz( uchar8 a );
uchar16 __attribute__((__overloadable__,__always_inline__,const)) clz( uchar16 a );
short __attribute__((__overloadable__,__always_inline__,const)) clz(short a );
short2 __attribute__((__overloadable__,__always_inline__,const)) clz( short2 a );
#if __OPENCL_C_VERSION__ >= 110
short3 __attribute__((__overloadable__,__always_inline__,const)) clz( short3 a );
#endif
short4 __attribute__((__overloadable__,__always_inline__,const)) clz( short4 a );
short8 __attribute__((__overloadable__,__always_inline__,const)) clz( short8 a );
short16 __attribute__((__overloadable__,__always_inline__,const)) clz( short16 a );
ushort __attribute__((__overloadable__,__always_inline__,const)) clz(ushort a );
ushort2 __attribute__((__overloadable__,__always_inline__,const)) clz( ushort2 a );
#if __OPENCL_C_VERSION__ >= 110
ushort3 __attribute__((__overloadable__,__always_inline__,const)) clz( ushort3 a );
#endif
ushort4 __attribute__((__overloadable__,__always_inline__,const)) clz( ushort4 a );
ushort8 __attribute__((__overloadable__,__always_inline__,const)) clz( ushort8 a );
ushort16 __attribute__((__overloadable__,__always_inline__,const)) clz( ushort16 a );
int __attribute__((__overloadable__,__always_inline__,const)) clz(int a );
int2 __attribute__((__overloadable__,__always_inline__,const)) clz( int2 a );
#if __OPENCL_C_VERSION__ >= 110
int3 __attribute__((__overloadable__,__always_inline__,const)) clz( int3 a );
#endif
int4 __attribute__((__overloadable__,__always_inline__,const)) clz( int4 a );
int8 __attribute__((__overloadable__,__always_inline__,const)) clz( int8 a );
int16 __attribute__((__overloadable__,__always_inline__,const)) clz( int16 a );
uint __attribute__((__overloadable__,__always_inline__,const)) clz(uint a );
uint2 __attribute__((__overloadable__,__always_inline__,const)) clz( uint2 a );
#if __OPENCL_C_VERSION__ >= 110
uint3 __attribute__((__overloadable__,__always_inline__,const)) clz( uint3 a );
#endif
uint4 __attribute__((__overloadable__,__always_inline__,const)) clz( uint4 a );
uint8 __attribute__((__overloadable__,__always_inline__,const)) clz( uint8 a );
uint16 __attribute__((__overloadable__,__always_inline__,const)) clz( uint16 a );
long __attribute__((__overloadable__,__always_inline__,const)) clz(long a );
long2 __attribute__((__overloadable__,__always_inline__,const)) clz( long2 a );
#if __OPENCL_C_VERSION__ >= 110
long3 __attribute__((__overloadable__,__always_inline__,const)) clz( long3 a );
#endif
long4 __attribute__((__overloadable__,__always_inline__,const)) clz( long4 a );
long8 __attribute__((__overloadable__,__always_inline__,const)) clz( long8 a );
long16 __attribute__((__overloadable__,__always_inline__,const)) clz( long16 a );
ulong __attribute__((__overloadable__,__always_inline__,const)) clz(ulong a );
ulong2 __attribute__((__overloadable__,__always_inline__,const)) clz( ulong2 a );
#if __OPENCL_C_VERSION__ >= 110
ulong3 __attribute__((__overloadable__,__always_inline__,const)) clz( ulong3 a );
#endif
ulong4 __attribute__((__overloadable__,__always_inline__,const)) clz( ulong4 a );
ulong8 __attribute__((__overloadable__,__always_inline__,const)) clz( ulong8 a );
ulong16 __attribute__((__overloadable__,__always_inline__,const)) clz( ulong16 a );
#if __OPENCL_C_VERSION__ >= 120
char __attribute__((__overloadable__,__always_inline__,const)) popcount(char a );
char2 __attribute__((__overloadable__,__always_inline__,const)) popcount( char2 a );
#if __OPENCL_C_VERSION__ >= 110
char3 __attribute__((__overloadable__,__always_inline__,const)) popcount( char3 a );
#endif
char4 __attribute__((__overloadable__,__always_inline__,const)) popcount( char4 a );
char8 __attribute__((__overloadable__,__always_inline__,const)) popcount( char8 a );
char16 __attribute__((__overloadable__,__always_inline__,const)) popcount( char16 a );
uchar __attribute__((__overloadable__,__always_inline__,const)) popcount(uchar a );
uchar2 __attribute__((__overloadable__,__always_inline__,const)) popcount( uchar2 a );
#if __OPENCL_C_VERSION__ >= 110
uchar3 __attribute__((__overloadable__,__always_inline__,const)) popcount( uchar3 a );
#endif
uchar4 __attribute__((__overloadable__,__always_inline__,const)) popcount( uchar4 a );
uchar8 __attribute__((__overloadable__,__always_inline__,const)) popcount( uchar8 a );
uchar16 __attribute__((__overloadable__,__always_inline__,const)) popcount( uchar16 a );
short __attribute__((__overloadable__,__always_inline__,const)) popcount(short a );
short2 __attribute__((__overloadable__,__always_inline__,const)) popcount( short2 a );
#if __OPENCL_C_VERSION__ >= 110
short3 __attribute__((__overloadable__,__always_inline__,const)) popcount( short3 a );
#endif
short4 __attribute__((__overloadable__,__always_inline__,const)) popcount( short4 a );
short8 __attribute__((__overloadable__,__always_inline__,const)) popcount( short8 a );
short16 __attribute__((__overloadable__,__always_inline__,const)) popcount( short16 a );
ushort __attribute__((__overloadable__,__always_inline__,const)) popcount(ushort a );
ushort2 __attribute__((__overloadable__,__always_inline__,const)) popcount( ushort2 a );
#if __OPENCL_C_VERSION__ >= 110
ushort3 __attribute__((__overloadable__,__always_inline__,const)) popcount( ushort3 a );
#endif
ushort4 __attribute__((__overloadable__,__always_inline__,const)) popcount( ushort4 a );
ushort8 __attribute__((__overloadable__,__always_inline__,const)) popcount( ushort8 a );
ushort16 __attribute__((__overloadable__,__always_inline__,const)) popcount( ushort16 a );
int __attribute__((__overloadable__,__always_inline__,const)) popcount(int a );
int2 __attribute__((__overloadable__,__always_inline__,const)) popcount( int2 a );
#if __OPENCL_C_VERSION__ >= 110
int3 __attribute__((__overloadable__,__always_inline__,const)) popcount( int3 a );
#endif
int4 __attribute__((__overloadable__,__always_inline__,const)) popcount( int4 a );
int8 __attribute__((__overloadable__,__always_inline__,const)) popcount( int8 a );
int16 __attribute__((__overloadable__,__always_inline__,const)) popcount( int16 a );
uint __attribute__((__overloadable__,__always_inline__,const)) popcount(uint a );
uint2 __attribute__((__overloadable__,__always_inline__,const)) popcount( uint2 a );
#if __OPENCL_C_VERSION__ >= 110
uint3 __attribute__((__overloadable__,__always_inline__,const)) popcount( uint3 a );
#endif
uint4 __attribute__((__overloadable__,__always_inline__,const)) popcount( uint4 a );
uint8 __attribute__((__overloadable__,__always_inline__,const)) popcount( uint8 a );
uint16 __attribute__((__overloadable__,__always_inline__,const)) popcount( uint16 a );
long __attribute__((__overloadable__,__always_inline__,const)) popcount(long a );
long2 __attribute__((__overloadable__,__always_inline__,const)) popcount( long2 a );
#if __OPENCL_C_VERSION__ >= 110
long3 __attribute__((__overloadable__,__always_inline__,const)) popcount( long3 a );
#endif
long4 __attribute__((__overloadable__,__always_inline__,const)) popcount( long4 a );
long8 __attribute__((__overloadable__,__always_inline__,const)) popcount( long8 a );
long16 __attribute__((__overloadable__,__always_inline__,const)) popcount( long16 a );
ulong __attribute__((__overloadable__,__always_inline__,const)) popcount(ulong a );
ulong2 __attribute__((__overloadable__,__always_inline__,const)) popcount( ulong2 a );
#if __OPENCL_C_VERSION__ >= 110
ulong3 __attribute__((__overloadable__,__always_inline__,const)) popcount( ulong3 a );
#endif
ulong4 __attribute__((__overloadable__,__always_inline__,const)) popcount( ulong4 a );
ulong8 __attribute__((__overloadable__,__always_inline__,const)) popcount( ulong8 a );
ulong16 __attribute__((__overloadable__,__always_inline__,const)) popcount( ulong16 a );
#endif
char __attribute__((__overloadable__,__always_inline__,const)) add_sat(char a, char b );
char2 __attribute__((__overloadable__,__always_inline__,const)) add_sat( char2 a, char2 b );
#if __OPENCL_C_VERSION__ >= 110
char3 __attribute__((__overloadable__,__always_inline__,const)) add_sat( char3 a, char3 b );
#endif
char4 __attribute__((__overloadable__,__always_inline__,const)) add_sat( char4 a, char4 b );
char8 __attribute__((__overloadable__,__always_inline__,const)) add_sat( char8 a, char8 b );
char16 __attribute__((__overloadable__,__always_inline__,const)) add_sat( char16 a, char16 b );
uchar __attribute__((__overloadable__,__always_inline__,const)) add_sat(uchar a, uchar b );
uchar2 __attribute__((__overloadable__,__always_inline__,const)) add_sat( uchar2 a, uchar2 b );
#if __OPENCL_C_VERSION__ >= 110
uchar3 __attribute__((__overloadable__,__always_inline__,const)) add_sat( uchar3 a, uchar3 b );
#endif
uchar4 __attribute__((__overloadable__,__always_inline__,const)) add_sat( uchar4 a, uchar4 b );
uchar8 __attribute__((__overloadable__,__always_inline__,const)) add_sat( uchar8 a, uchar8 b );
uchar16 __attribute__((__overloadable__,__always_inline__,const)) add_sat( uchar16 a, uchar16 b );
short __attribute__((__overloadable__,__always_inline__,const)) add_sat(short a, short b );
short2 __attribute__((__overloadable__,__always_inline__,const)) add_sat( short2 a, short2 b );
#if __OPENCL_C_VERSION__ >= 110
short3 __attribute__((__overloadable__,__always_inline__,const)) add_sat( short3 a, short3 b );
#endif
short4 __attribute__((__overloadable__,__always_inline__,const)) add_sat( short4 a, short4 b );
short8 __attribute__((__overloadable__,__always_inline__,const)) add_sat( short8 a, short8 b );
short16 __attribute__((__overloadable__,__always_inline__,const)) add_sat( short16 a, short16 b );
ushort __attribute__((__overloadable__,__always_inline__,const)) add_sat(ushort a, ushort b );
ushort2 __attribute__((__overloadable__,__always_inline__,const)) add_sat( ushort2 a, ushort2 b );
#if __OPENCL_C_VERSION__ >= 110
ushort3 __attribute__((__overloadable__,__always_inline__,const)) add_sat( ushort3 a, ushort3 b );
#endif
ushort4 __attribute__((__overloadable__,__always_inline__,const)) add_sat( ushort4 a, ushort4 b );
ushort8 __attribute__((__overloadable__,__always_inline__,const)) add_sat( ushort8 a, ushort8 b );
ushort16 __attribute__((__overloadable__,__always_inline__,const)) add_sat( ushort16 a, ushort16 b );
int __attribute__((__overloadable__,__always_inline__,const)) add_sat(int a, int b );
int2 __attribute__((__overloadable__,__always_inline__,const)) add_sat( int2 a, int2 b );
#if __OPENCL_C_VERSION__ >= 110
int3 __attribute__((__overloadable__,__always_inline__,const)) add_sat( int3 a, int3 b );
#endif
int4 __attribute__((__overloadable__,__always_inline__,const)) add_sat( int4 a, int4 b );
int8 __attribute__((__overloadable__,__always_inline__,const)) add_sat( int8 a, int8 b );
int16 __attribute__((__overloadable__,__always_inline__,const)) add_sat( int16 a, int16 b );
uint __attribute__((__overloadable__,__always_inline__,const)) add_sat(uint a, uint b );
uint2 __attribute__((__overloadable__,__always_inline__,const)) add_sat( uint2 a, uint2 b );
#if __OPENCL_C_VERSION__ >= 110
uint3 __attribute__((__overloadable__,__always_inline__,const)) add_sat( uint3 a, uint3 b );
#endif
uint4 __attribute__((__overloadable__,__always_inline__,const)) add_sat( uint4 a, uint4 b );
uint8 __attribute__((__overloadable__,__always_inline__,const)) add_sat( uint8 a, uint8 b );
uint16 __attribute__((__overloadable__,__always_inline__,const)) add_sat( uint16 a, uint16 b );
long __attribute__((__overloadable__,__always_inline__,const)) add_sat(long a, long b );
long2 __attribute__((__overloadable__,__always_inline__,const)) add_sat( long2 a, long2 b );
#if __OPENCL_C_VERSION__ >= 110
long3 __attribute__((__overloadable__,__always_inline__,const)) add_sat( long3 a, long3 b );
#endif
long4 __attribute__((__overloadable__,__always_inline__,const)) add_sat( long4 a, long4 b );
long8 __attribute__((__overloadable__,__always_inline__,const)) add_sat( long8 a, long8 b );
long16 __attribute__((__overloadable__,__always_inline__,const)) add_sat( long16 a, long16 b );
ulong __attribute__((__overloadable__,__always_inline__,const)) add_sat(ulong a, ulong b );
ulong2 __attribute__((__overloadable__,__always_inline__,const)) add_sat( ulong2 a, ulong2 b );
#if __OPENCL_C_VERSION__ >= 110
ulong3 __attribute__((__overloadable__,__always_inline__,const)) add_sat( ulong3 a, ulong3 b );
#endif
ulong4 __attribute__((__overloadable__,__always_inline__,const)) add_sat( ulong4 a, ulong4 b );
ulong8 __attribute__((__overloadable__,__always_inline__,const)) add_sat( ulong8 a, ulong8 b );
ulong16 __attribute__((__overloadable__,__always_inline__,const)) add_sat( ulong16 a, ulong16 b );
char __attribute__((__overloadable__,__always_inline__,const)) hadd(char a, char b );
char2 __attribute__((__overloadable__,__always_inline__,const)) hadd( char2 a, char2 b );
#if __OPENCL_C_VERSION__ >= 110
char3 __attribute__((__overloadable__,__always_inline__,const)) hadd( char3 a, char3 b );
#endif
char4 __attribute__((__overloadable__,__always_inline__,const)) hadd( char4 a, char4 b );
char8 __attribute__((__overloadable__,__always_inline__,const)) hadd( char8 a, char8 b );
char16 __attribute__((__overloadable__,__always_inline__,const)) hadd( char16 a, char16 b );
uchar __attribute__((__overloadable__,__always_inline__,const)) hadd(uchar a, uchar b );
uchar2 __attribute__((__overloadable__,__always_inline__,const)) hadd( uchar2 a, uchar2 b );
#if __OPENCL_C_VERSION__ >= 110
uchar3 __attribute__((__overloadable__,__always_inline__,const)) hadd( uchar3 a, uchar3 b );
#endif
uchar4 __attribute__((__overloadable__,__always_inline__,const)) hadd( uchar4 a, uchar4 b );
uchar8 __attribute__((__overloadable__,__always_inline__,const)) hadd( uchar8 a, uchar8 b );
uchar16 __attribute__((__overloadable__,__always_inline__,const)) hadd( uchar16 a, uchar16 b );
short __attribute__((__overloadable__,__always_inline__,const)) hadd(short a, short b );
short2 __attribute__((__overloadable__,__always_inline__,const)) hadd( short2 a, short2 b );
#if __OPENCL_C_VERSION__ >= 110
short3 __attribute__((__overloadable__,__always_inline__,const)) hadd( short3 a, short3 b );
#endif
short4 __attribute__((__overloadable__,__always_inline__,const)) hadd( short4 a, short4 b );
short8 __attribute__((__overloadable__,__always_inline__,const)) hadd( short8 a, short8 b );
short16 __attribute__((__overloadable__,__always_inline__,const)) hadd( short16 a, short16 b );
ushort __attribute__((__overloadable__,__always_inline__,const)) hadd(ushort a, ushort b );
ushort2 __attribute__((__overloadable__,__always_inline__,const)) hadd( ushort2 a, ushort2 b );
#if __OPENCL_C_VERSION__ >= 110
ushort3 __attribute__((__overloadable__,__always_inline__,const)) hadd( ushort3 a, ushort3 b );
#endif
ushort4 __attribute__((__overloadable__,__always_inline__,const)) hadd( ushort4 a, ushort4 b );
ushort8 __attribute__((__overloadable__,__always_inline__,const)) hadd( ushort8 a, ushort8 b );
ushort16 __attribute__((__overloadable__,__always_inline__,const)) hadd( ushort16 a, ushort16 b );
int __attribute__((__overloadable__,__always_inline__,const)) hadd(int a, int b );
int2 __attribute__((__overloadable__,__always_inline__,const)) hadd( int2 a, int2 b );
#if __OPENCL_C_VERSION__ >= 110
int3 __attribute__((__overloadable__,__always_inline__,const)) hadd( int3 a, int3 b );
#endif
int4 __attribute__((__overloadable__,__always_inline__,const)) hadd( int4 a, int4 b );
int8 __attribute__((__overloadable__,__always_inline__,const)) hadd( int8 a, int8 b );
int16 __attribute__((__overloadable__,__always_inline__,const)) hadd( int16 a, int16 b );
uint __attribute__((__overloadable__,__always_inline__,const)) hadd(uint a, uint b );
uint2 __attribute__((__overloadable__,__always_inline__,const)) hadd( uint2 a, uint2 b );
#if __OPENCL_C_VERSION__ >= 110
uint3 __attribute__((__overloadable__,__always_inline__,const)) hadd( uint3 a, uint3 b );
#endif
uint4 __attribute__((__overloadable__,__always_inline__,const)) hadd( uint4 a, uint4 b );
uint8 __attribute__((__overloadable__,__always_inline__,const)) hadd( uint8 a, uint8 b );
uint16 __attribute__((__overloadable__,__always_inline__,const)) hadd( uint16 a, uint16 b );
long __attribute__((__overloadable__,__always_inline__,const)) hadd(long a, long b );
long2 __attribute__((__overloadable__,__always_inline__,const)) hadd( long2 a, long2 b );
#if __OPENCL_C_VERSION__ >= 110
long3 __attribute__((__overloadable__,__always_inline__,const)) hadd( long3 a, long3 b );
#endif
long4 __attribute__((__overloadable__,__always_inline__,const)) hadd( long4 a, long4 b );
long8 __attribute__((__overloadable__,__always_inline__,const)) hadd( long8 a, long8 b );
long16 __attribute__((__overloadable__,__always_inline__,const)) hadd( long16 a, long16 b );
ulong __attribute__((__overloadable__,__always_inline__,const)) hadd(ulong a, ulong b );
ulong2 __attribute__((__overloadable__,__always_inline__,const)) hadd( ulong2 a, ulong2 b );
#if __OPENCL_C_VERSION__ >= 110
ulong3 __attribute__((__overloadable__,__always_inline__,const)) hadd( ulong3 a, ulong3 b );
#endif
ulong4 __attribute__((__overloadable__,__always_inline__,const)) hadd( ulong4 a, ulong4 b );
ulong8 __attribute__((__overloadable__,__always_inline__,const)) hadd( ulong8 a, ulong8 b );
ulong16 __attribute__((__overloadable__,__always_inline__,const)) hadd( ulong16 a, ulong16 b );
char __attribute__((__overloadable__,__always_inline__,const)) rhadd(char a, char b );
char2 __attribute__((__overloadable__,__always_inline__,const)) rhadd( char2 a, char2 b );
#if __OPENCL_C_VERSION__ >= 110
char3 __attribute__((__overloadable__,__always_inline__,const)) rhadd( char3 a, char3 b );
#endif
char4 __attribute__((__overloadable__,__always_inline__,const)) rhadd( char4 a, char4 b );
char8 __attribute__((__overloadable__,__always_inline__,const)) rhadd( char8 a, char8 b );
char16 __attribute__((__overloadable__,__always_inline__,const)) rhadd( char16 a, char16 b );
uchar __attribute__((__overloadable__,__always_inline__,const)) rhadd(uchar a, uchar b );
uchar2 __attribute__((__overloadable__,__always_inline__,const)) rhadd( uchar2 a, uchar2 b );
#if __OPENCL_C_VERSION__ >= 110
uchar3 __attribute__((__overloadable__,__always_inline__,const)) rhadd( uchar3 a, uchar3 b );
#endif
uchar4 __attribute__((__overloadable__,__always_inline__,const)) rhadd( uchar4 a, uchar4 b );
uchar8 __attribute__((__overloadable__,__always_inline__,const)) rhadd( uchar8 a, uchar8 b );
uchar16 __attribute__((__overloadable__,__always_inline__,const)) rhadd( uchar16 a, uchar16 b );
short __attribute__((__overloadable__,__always_inline__,const)) rhadd(short a, short b );
short2 __attribute__((__overloadable__,__always_inline__,const)) rhadd( short2 a, short2 b );
#if __OPENCL_C_VERSION__ >= 110
short3 __attribute__((__overloadable__,__always_inline__,const)) rhadd( short3 a, short3 b );
#endif
short4 __attribute__((__overloadable__,__always_inline__,const)) rhadd( short4 a, short4 b );
short8 __attribute__((__overloadable__,__always_inline__,const)) rhadd( short8 a, short8 b );
short16 __attribute__((__overloadable__,__always_inline__,const)) rhadd( short16 a, short16 b );
ushort __attribute__((__overloadable__,__always_inline__,const)) rhadd(ushort a, ushort b );
ushort2 __attribute__((__overloadable__,__always_inline__,const)) rhadd( ushort2 a, ushort2 b );
#if __OPENCL_C_VERSION__ >= 110
ushort3 __attribute__((__overloadable__,__always_inline__,const)) rhadd( ushort3 a, ushort3 b );
#endif
ushort4 __attribute__((__overloadable__,__always_inline__,const)) rhadd( ushort4 a, ushort4 b );
ushort8 __attribute__((__overloadable__,__always_inline__,const)) rhadd( ushort8 a, ushort8 b );
ushort16 __attribute__((__overloadable__,__always_inline__,const)) rhadd( ushort16 a, ushort16 b );
int __attribute__((__overloadable__,__always_inline__,const)) rhadd(int a, int b );
int2 __attribute__((__overloadable__,__always_inline__,const)) rhadd( int2 a, int2 b );
#if __OPENCL_C_VERSION__ >= 110
int3 __attribute__((__overloadable__,__always_inline__,const)) rhadd( int3 a, int3 b );
#endif
int4 __attribute__((__overloadable__,__always_inline__,const)) rhadd( int4 a, int4 b );
int8 __attribute__((__overloadable__,__always_inline__,const)) rhadd( int8 a, int8 b );
int16 __attribute__((__overloadable__,__always_inline__,const)) rhadd( int16 a, int16 b );
uint __attribute__((__overloadable__,__always_inline__,const)) rhadd(uint a, uint b );
uint2 __attribute__((__overloadable__,__always_inline__,const)) rhadd( uint2 a, uint2 b );
#if __OPENCL_C_VERSION__ >= 110
uint3 __attribute__((__overloadable__,__always_inline__,const)) rhadd( uint3 a, uint3 b );
#endif
uint4 __attribute__((__overloadable__,__always_inline__,const)) rhadd( uint4 a, uint4 b );
uint8 __attribute__((__overloadable__,__always_inline__,const)) rhadd( uint8 a, uint8 b );
uint16 __attribute__((__overloadable__,__always_inline__,const)) rhadd( uint16 a, uint16 b );
long __attribute__((__overloadable__,__always_inline__,const)) rhadd(long a, long b );
long2 __attribute__((__overloadable__,__always_inline__,const)) rhadd( long2 a, long2 b );
#if __OPENCL_C_VERSION__ >= 110
long3 __attribute__((__overloadable__,__always_inline__,const)) rhadd( long3 a, long3 b );
#endif
long4 __attribute__((__overloadable__,__always_inline__,const)) rhadd( long4 a, long4 b );
long8 __attribute__((__overloadable__,__always_inline__,const)) rhadd( long8 a, long8 b );
long16 __attribute__((__overloadable__,__always_inline__,const)) rhadd( long16 a, long16 b );
ulong __attribute__((__overloadable__,__always_inline__,const)) rhadd(ulong a, ulong b );
ulong2 __attribute__((__overloadable__,__always_inline__,const)) rhadd( ulong2 a, ulong2 b );
#if __OPENCL_C_VERSION__ >= 110
ulong3 __attribute__((__overloadable__,__always_inline__,const)) rhadd( ulong3 a, ulong3 b );
#endif
ulong4 __attribute__((__overloadable__,__always_inline__,const)) rhadd( ulong4 a, ulong4 b );
ulong8 __attribute__((__overloadable__,__always_inline__,const)) rhadd( ulong8 a, ulong8 b );
ulong16 __attribute__((__overloadable__,__always_inline__,const)) rhadd( ulong16 a, ulong16 b );
char __attribute__((__overloadable__,__always_inline__,const)) mul_hi(char a, char b );
char2 __attribute__((__overloadable__,__always_inline__,const)) mul_hi( char2 a, char2 b );
#if __OPENCL_C_VERSION__ >= 110
char3 __attribute__((__overloadable__,__always_inline__,const)) mul_hi( char3 a, char3 b );
#endif
char4 __attribute__((__overloadable__,__always_inline__,const)) mul_hi( char4 a, char4 b );
char8 __attribute__((__overloadable__,__always_inline__,const)) mul_hi( char8 a, char8 b );
char16 __attribute__((__overloadable__,__always_inline__,const)) mul_hi( char16 a, char16 b );
uchar __attribute__((__overloadable__,__always_inline__,const)) mul_hi(uchar a, uchar b );
uchar2 __attribute__((__overloadable__,__always_inline__,const)) mul_hi( uchar2 a, uchar2 b );
#if __OPENCL_C_VERSION__ >= 110
uchar3 __attribute__((__overloadable__,__always_inline__,const)) mul_hi( uchar3 a, uchar3 b );
#endif
uchar4 __attribute__((__overloadable__,__always_inline__,const)) mul_hi( uchar4 a, uchar4 b );
uchar8 __attribute__((__overloadable__,__always_inline__,const)) mul_hi( uchar8 a, uchar8 b );
uchar16 __attribute__((__overloadable__,__always_inline__,const)) mul_hi( uchar16 a, uchar16 b );
short __attribute__((__overloadable__,__always_inline__,const)) mul_hi(short a, short b );
short2 __attribute__((__overloadable__,__always_inline__,const)) mul_hi( short2 a, short2 b );
#if __OPENCL_C_VERSION__ >= 110
short3 __attribute__((__overloadable__,__always_inline__,const)) mul_hi( short3 a, short3 b );
#endif
short4 __attribute__((__overloadable__,__always_inline__,const)) mul_hi( short4 a, short4 b );
short8 __attribute__((__overloadable__,__always_inline__,const)) mul_hi( short8 a, short8 b );
short16 __attribute__((__overloadable__,__always_inline__,const)) mul_hi( short16 a, short16 b );
ushort __attribute__((__overloadable__,__always_inline__,const)) mul_hi(ushort a, ushort b );
ushort2 __attribute__((__overloadable__,__always_inline__,const)) mul_hi( ushort2 a, ushort2 b );
#if __OPENCL_C_VERSION__ >= 110
ushort3 __attribute__((__overloadable__,__always_inline__,const)) mul_hi( ushort3 a, ushort3 b );
#endif
ushort4 __attribute__((__overloadable__,__always_inline__,const)) mul_hi( ushort4 a, ushort4 b );
ushort8 __attribute__((__overloadable__,__always_inline__,const)) mul_hi( ushort8 a, ushort8 b );
ushort16 __attribute__((__overloadable__,__always_inline__,const)) mul_hi( ushort16 a, ushort16 b );
int __attribute__((__overloadable__,__always_inline__,const)) mul_hi(int a, int b );
int2 __attribute__((__overloadable__,__always_inline__,const)) mul_hi( int2 a, int2 b );
#if __OPENCL_C_VERSION__ >= 110
int3 __attribute__((__overloadable__,__always_inline__,const)) mul_hi( int3 a, int3 b );
#endif
int4 __attribute__((__overloadable__,__always_inline__,const)) mul_hi( int4 a, int4 b );
int8 __attribute__((__overloadable__,__always_inline__,const)) mul_hi( int8 a, int8 b );
int16 __attribute__((__overloadable__,__always_inline__,const)) mul_hi( int16 a, int16 b );
uint __attribute__((__overloadable__,__always_inline__,const)) mul_hi(uint a, uint b );
uint2 __attribute__((__overloadable__,__always_inline__,const)) mul_hi( uint2 a, uint2 b );
#if __OPENCL_C_VERSION__ >= 110
uint3 __attribute__((__overloadable__,__always_inline__,const)) mul_hi( uint3 a, uint3 b );
#endif
uint4 __attribute__((__overloadable__,__always_inline__,const)) mul_hi( uint4 a, uint4 b );
uint8 __attribute__((__overloadable__,__always_inline__,const)) mul_hi( uint8 a, uint8 b );
uint16 __attribute__((__overloadable__,__always_inline__,const)) mul_hi( uint16 a, uint16 b );
long __attribute__((__overloadable__,__always_inline__,const)) mul_hi(long a, long b );
long2 __attribute__((__overloadable__,__always_inline__,const)) mul_hi( long2 a, long2 b );
#if __OPENCL_C_VERSION__ >= 110
long3 __attribute__((__overloadable__,__always_inline__,const)) mul_hi( long3 a, long3 b );
#endif
long4 __attribute__((__overloadable__,__always_inline__,const)) mul_hi( long4 a, long4 b );
long8 __attribute__((__overloadable__,__always_inline__,const)) mul_hi( long8 a, long8 b );
long16 __attribute__((__overloadable__,__always_inline__,const)) mul_hi( long16 a, long16 b );
ulong __attribute__((__overloadable__,__always_inline__,const)) mul_hi(ulong a, ulong b );
ulong2 __attribute__((__overloadable__,__always_inline__,const)) mul_hi( ulong2 a, ulong2 b );
#if __OPENCL_C_VERSION__ >= 110
ulong3 __attribute__((__overloadable__,__always_inline__,const)) mul_hi( ulong3 a, ulong3 b );
#endif
ulong4 __attribute__((__overloadable__,__always_inline__,const)) mul_hi( ulong4 a, ulong4 b );
ulong8 __attribute__((__overloadable__,__always_inline__,const)) mul_hi( ulong8 a, ulong8 b );
ulong16 __attribute__((__overloadable__,__always_inline__,const)) mul_hi( ulong16 a, ulong16 b );
char __attribute__((__overloadable__,__always_inline__,const)) rotate(char a, char b );
char2 __attribute__((__overloadable__,__always_inline__,const)) rotate( char2 a, char2 b );
#if __OPENCL_C_VERSION__ >= 110
char3 __attribute__((__overloadable__,__always_inline__,const)) rotate( char3 a, char3 b );
#endif
char4 __attribute__((__overloadable__,__always_inline__,const)) rotate( char4 a, char4 b );
char8 __attribute__((__overloadable__,__always_inline__,const)) rotate( char8 a, char8 b );
char16 __attribute__((__overloadable__,__always_inline__,const)) rotate( char16 a, char16 b );
uchar __attribute__((__overloadable__,__always_inline__,const)) rotate(uchar a, uchar b );
uchar2 __attribute__((__overloadable__,__always_inline__,const)) rotate( uchar2 a, uchar2 b );
#if __OPENCL_C_VERSION__ >= 110
uchar3 __attribute__((__overloadable__,__always_inline__,const)) rotate( uchar3 a, uchar3 b );
#endif
uchar4 __attribute__((__overloadable__,__always_inline__,const)) rotate( uchar4 a, uchar4 b );
uchar8 __attribute__((__overloadable__,__always_inline__,const)) rotate( uchar8 a, uchar8 b );
uchar16 __attribute__((__overloadable__,__always_inline__,const)) rotate( uchar16 a, uchar16 b );
short __attribute__((__overloadable__,__always_inline__,const)) rotate(short a, short b );
short2 __attribute__((__overloadable__,__always_inline__,const)) rotate( short2 a, short2 b );
#if __OPENCL_C_VERSION__ >= 110
short3 __attribute__((__overloadable__,__always_inline__,const)) rotate( short3 a, short3 b );
#endif
short4 __attribute__((__overloadable__,__always_inline__,const)) rotate( short4 a, short4 b );
short8 __attribute__((__overloadable__,__always_inline__,const)) rotate( short8 a, short8 b );
short16 __attribute__((__overloadable__,__always_inline__,const)) rotate( short16 a, short16 b );
ushort __attribute__((__overloadable__,__always_inline__,const)) rotate(ushort a, ushort b );
ushort2 __attribute__((__overloadable__,__always_inline__,const)) rotate( ushort2 a, ushort2 b );
#if __OPENCL_C_VERSION__ >= 110
ushort3 __attribute__((__overloadable__,__always_inline__,const)) rotate( ushort3 a, ushort3 b );
#endif
ushort4 __attribute__((__overloadable__,__always_inline__,const)) rotate( ushort4 a, ushort4 b );
ushort8 __attribute__((__overloadable__,__always_inline__,const)) rotate( ushort8 a, ushort8 b );
ushort16 __attribute__((__overloadable__,__always_inline__,const)) rotate( ushort16 a, ushort16 b );
int __attribute__((__overloadable__,__always_inline__,const)) rotate(int a, int b );
int2 __attribute__((__overloadable__,__always_inline__,const)) rotate( int2 a, int2 b );
#if __OPENCL_C_VERSION__ >= 110
int3 __attribute__((__overloadable__,__always_inline__,const)) rotate( int3 a, int3 b );
#endif
int4 __attribute__((__overloadable__,__always_inline__,const)) rotate( int4 a, int4 b );
int8 __attribute__((__overloadable__,__always_inline__,const)) rotate( int8 a, int8 b );
int16 __attribute__((__overloadable__,__always_inline__,const)) rotate( int16 a, int16 b );
uint __attribute__((__overloadable__,__always_inline__,const)) rotate(uint a, uint b );
uint2 __attribute__((__overloadable__,__always_inline__,const)) rotate( uint2 a, uint2 b );
#if __OPENCL_C_VERSION__ >= 110
uint3 __attribute__((__overloadable__,__always_inline__,const)) rotate( uint3 a, uint3 b );
#endif
uint4 __attribute__((__overloadable__,__always_inline__,const)) rotate( uint4 a, uint4 b );
uint8 __attribute__((__overloadable__,__always_inline__,const)) rotate( uint8 a, uint8 b );
uint16 __attribute__((__overloadable__,__always_inline__,const)) rotate( uint16 a, uint16 b );
long __attribute__((__overloadable__,__always_inline__,const)) rotate(long a, long b );
long2 __attribute__((__overloadable__,__always_inline__,const)) rotate( long2 a, long2 b );
#if __OPENCL_C_VERSION__ >= 110
long3 __attribute__((__overloadable__,__always_inline__,const)) rotate( long3 a, long3 b );
#endif
long4 __attribute__((__overloadable__,__always_inline__,const)) rotate( long4 a, long4 b );
long8 __attribute__((__overloadable__,__always_inline__,const)) rotate( long8 a, long8 b );
long16 __attribute__((__overloadable__,__always_inline__,const)) rotate( long16 a, long16 b );
ulong __attribute__((__overloadable__,__always_inline__,const)) rotate(ulong a, ulong b );
ulong2 __attribute__((__overloadable__,__always_inline__,const)) rotate( ulong2 a, ulong2 b );
#if __OPENCL_C_VERSION__ >= 110
ulong3 __attribute__((__overloadable__,__always_inline__,const)) rotate( ulong3 a, ulong3 b );
#endif
ulong4 __attribute__((__overloadable__,__always_inline__,const)) rotate( ulong4 a, ulong4 b );
ulong8 __attribute__((__overloadable__,__always_inline__,const)) rotate( ulong8 a, ulong8 b );
ulong16 __attribute__((__overloadable__,__always_inline__,const)) rotate( ulong16 a, ulong16 b );
char __attribute__((__overloadable__,__always_inline__,const)) sub_sat(char a, char b );
char2 __attribute__((__overloadable__,__always_inline__,const)) sub_sat( char2 a, char2 b );
#if __OPENCL_C_VERSION__ >= 110
char3 __attribute__((__overloadable__,__always_inline__,const)) sub_sat( char3 a, char3 b );
#endif
char4 __attribute__((__overloadable__,__always_inline__,const)) sub_sat( char4 a, char4 b );
char8 __attribute__((__overloadable__,__always_inline__,const)) sub_sat( char8 a, char8 b );
char16 __attribute__((__overloadable__,__always_inline__,const)) sub_sat( char16 a, char16 b );
uchar __attribute__((__overloadable__,__always_inline__,const)) sub_sat(uchar a, uchar b );
uchar2 __attribute__((__overloadable__,__always_inline__,const)) sub_sat( uchar2 a, uchar2 b );
#if __OPENCL_C_VERSION__ >= 110
uchar3 __attribute__((__overloadable__,__always_inline__,const)) sub_sat( uchar3 a, uchar3 b );
#endif
uchar4 __attribute__((__overloadable__,__always_inline__,const)) sub_sat( uchar4 a, uchar4 b );
uchar8 __attribute__((__overloadable__,__always_inline__,const)) sub_sat( uchar8 a, uchar8 b );
uchar16 __attribute__((__overloadable__,__always_inline__,const)) sub_sat( uchar16 a, uchar16 b );
short __attribute__((__overloadable__,__always_inline__,const)) sub_sat(short a, short b );
short2 __attribute__((__overloadable__,__always_inline__,const)) sub_sat( short2 a, short2 b );
#if __OPENCL_C_VERSION__ >= 110
short3 __attribute__((__overloadable__,__always_inline__,const)) sub_sat( short3 a, short3 b );
#endif
short4 __attribute__((__overloadable__,__always_inline__,const)) sub_sat( short4 a, short4 b );
short8 __attribute__((__overloadable__,__always_inline__,const)) sub_sat( short8 a, short8 b );
short16 __attribute__((__overloadable__,__always_inline__,const)) sub_sat( short16 a, short16 b );
ushort __attribute__((__overloadable__,__always_inline__,const)) sub_sat(ushort a, ushort b );
ushort2 __attribute__((__overloadable__,__always_inline__,const)) sub_sat( ushort2 a, ushort2 b );
#if __OPENCL_C_VERSION__ >= 110
ushort3 __attribute__((__overloadable__,__always_inline__,const)) sub_sat( ushort3 a, ushort3 b );
#endif
ushort4 __attribute__((__overloadable__,__always_inline__,const)) sub_sat( ushort4 a, ushort4 b );
ushort8 __attribute__((__overloadable__,__always_inline__,const)) sub_sat( ushort8 a, ushort8 b );
ushort16 __attribute__((__overloadable__,__always_inline__,const)) sub_sat( ushort16 a, ushort16 b );
int __attribute__((__overloadable__,__always_inline__,const)) sub_sat(int a, int b );
int2 __attribute__((__overloadable__,__always_inline__,const)) sub_sat( int2 a, int2 b );
#if __OPENCL_C_VERSION__ >= 110
int3 __attribute__((__overloadable__,__always_inline__,const)) sub_sat( int3 a, int3 b );
#endif
int4 __attribute__((__overloadable__,__always_inline__,const)) sub_sat( int4 a, int4 b );
int8 __attribute__((__overloadable__,__always_inline__,const)) sub_sat( int8 a, int8 b );
int16 __attribute__((__overloadable__,__always_inline__,const)) sub_sat( int16 a, int16 b );
uint __attribute__((__overloadable__,__always_inline__,const)) sub_sat(uint a, uint b );
uint2 __attribute__((__overloadable__,__always_inline__,const)) sub_sat( uint2 a, uint2 b );
#if __OPENCL_C_VERSION__ >= 110
uint3 __attribute__((__overloadable__,__always_inline__,const)) sub_sat( uint3 a, uint3 b );
#endif
uint4 __attribute__((__overloadable__,__always_inline__,const)) sub_sat( uint4 a, uint4 b );
uint8 __attribute__((__overloadable__,__always_inline__,const)) sub_sat( uint8 a, uint8 b );
uint16 __attribute__((__overloadable__,__always_inline__,const)) sub_sat( uint16 a, uint16 b );
long __attribute__((__overloadable__,__always_inline__,const)) sub_sat(long a, long b );
long2 __attribute__((__overloadable__,__always_inline__,const)) sub_sat( long2 a, long2 b );
#if __OPENCL_C_VERSION__ >= 110
long3 __attribute__((__overloadable__,__always_inline__,const)) sub_sat( long3 a, long3 b );
#endif
long4 __attribute__((__overloadable__,__always_inline__,const)) sub_sat( long4 a, long4 b );
long8 __attribute__((__overloadable__,__always_inline__,const)) sub_sat( long8 a, long8 b );
long16 __attribute__((__overloadable__,__always_inline__,const)) sub_sat( long16 a, long16 b );
ulong __attribute__((__overloadable__,__always_inline__,const)) sub_sat(ulong a, ulong b );
ulong2 __attribute__((__overloadable__,__always_inline__,const)) sub_sat( ulong2 a, ulong2 b );
#if __OPENCL_C_VERSION__ >= 110
ulong3 __attribute__((__overloadable__,__always_inline__,const)) sub_sat( ulong3 a, ulong3 b );
#endif
ulong4 __attribute__((__overloadable__,__always_inline__,const)) sub_sat( ulong4 a, ulong4 b );
ulong8 __attribute__((__overloadable__,__always_inline__,const)) sub_sat( ulong8 a, ulong8 b );
ulong16 __attribute__((__overloadable__,__always_inline__,const)) sub_sat( ulong16 a, ulong16 b );
char __attribute__((__overloadable__,__always_inline__,const)) mad_hi(char a, char b, char c);
char2 __attribute__((__overloadable__,__always_inline__,const)) mad_hi( char2 a, char2 b, char2 c );
#if __OPENCL_C_VERSION__ >= 110
char3 __attribute__((__overloadable__,__always_inline__,const)) mad_hi( char3 a, char3 b, char3 c );
#endif
char4 __attribute__((__overloadable__,__always_inline__,const)) mad_hi( char4 a, char4 b, char4 c );
char8 __attribute__((__overloadable__,__always_inline__,const)) mad_hi( char8 a, char8 b, char8 c );
char16 __attribute__((__overloadable__,__always_inline__,const)) mad_hi( char16 a, char16 b, char16 c );
uchar __attribute__((__overloadable__,__always_inline__,const)) mad_hi(uchar a, uchar b, uchar c);
uchar2 __attribute__((__overloadable__,__always_inline__,const)) mad_hi( uchar2 a, uchar2 b, uchar2 c );
#if __OPENCL_C_VERSION__ >= 110
uchar3 __attribute__((__overloadable__,__always_inline__,const)) mad_hi( uchar3 a, uchar3 b, uchar3 c );
#endif
uchar4 __attribute__((__overloadable__,__always_inline__,const)) mad_hi( uchar4 a, uchar4 b, uchar4 c );
uchar8 __attribute__((__overloadable__,__always_inline__,const)) mad_hi( uchar8 a, uchar8 b, uchar8 c );
uchar16 __attribute__((__overloadable__,__always_inline__,const)) mad_hi( uchar16 a, uchar16 b, uchar16 c );
short __attribute__((__overloadable__,__always_inline__,const)) mad_hi(short a, short b, short c);
short2 __attribute__((__overloadable__,__always_inline__,const)) mad_hi( short2 a, short2 b, short2 c );
#if __OPENCL_C_VERSION__ >= 110
short3 __attribute__((__overloadable__,__always_inline__,const)) mad_hi( short3 a, short3 b, short3 c );
#endif
short4 __attribute__((__overloadable__,__always_inline__,const)) mad_hi( short4 a, short4 b, short4 c );
short8 __attribute__((__overloadable__,__always_inline__,const)) mad_hi( short8 a, short8 b, short8 c );
short16 __attribute__((__overloadable__,__always_inline__,const)) mad_hi( short16 a, short16 b, short16 c );
ushort __attribute__((__overloadable__,__always_inline__,const)) mad_hi(ushort a, ushort b, ushort c);
ushort2 __attribute__((__overloadable__,__always_inline__,const)) mad_hi( ushort2 a, ushort2 b, ushort2 c );
#if __OPENCL_C_VERSION__ >= 110
ushort3 __attribute__((__overloadable__,__always_inline__,const)) mad_hi( ushort3 a, ushort3 b, ushort3 c );
#endif
ushort4 __attribute__((__overloadable__,__always_inline__,const)) mad_hi( ushort4 a, ushort4 b, ushort4 c );
ushort8 __attribute__((__overloadable__,__always_inline__,const)) mad_hi( ushort8 a, ushort8 b, ushort8 c );
ushort16 __attribute__((__overloadable__,__always_inline__,const)) mad_hi( ushort16 a, ushort16 b, ushort16 c );
int __attribute__((__overloadable__,__always_inline__,const)) mad_hi(int a, int b, int c);
int2 __attribute__((__overloadable__,__always_inline__,const)) mad_hi( int2 a, int2 b, int2 c );
#if __OPENCL_C_VERSION__ >= 110
int3 __attribute__((__overloadable__,__always_inline__,const)) mad_hi( int3 a, int3 b, int3 c );
#endif
int4 __attribute__((__overloadable__,__always_inline__,const)) mad_hi( int4 a, int4 b, int4 c );
int8 __attribute__((__overloadable__,__always_inline__,const)) mad_hi( int8 a, int8 b, int8 c );
int16 __attribute__((__overloadable__,__always_inline__,const)) mad_hi( int16 a, int16 b, int16 c );
uint __attribute__((__overloadable__,__always_inline__,const)) mad_hi(uint a, uint b, uint c);
uint2 __attribute__((__overloadable__,__always_inline__,const)) mad_hi( uint2 a, uint2 b, uint2 c );
#if __OPENCL_C_VERSION__ >= 110
uint3 __attribute__((__overloadable__,__always_inline__,const)) mad_hi( uint3 a, uint3 b, uint3 c );
#endif
uint4 __attribute__((__overloadable__,__always_inline__,const)) mad_hi( uint4 a, uint4 b, uint4 c );
uint8 __attribute__((__overloadable__,__always_inline__,const)) mad_hi( uint8 a, uint8 b, uint8 c );
uint16 __attribute__((__overloadable__,__always_inline__,const)) mad_hi( uint16 a, uint16 b, uint16 c );
long __attribute__((__overloadable__,__always_inline__,const)) mad_hi(long a, long b, long c);
long2 __attribute__((__overloadable__,__always_inline__,const)) mad_hi( long2 a, long2 b, long2 c );
#if __OPENCL_C_VERSION__ >= 110
long3 __attribute__((__overloadable__,__always_inline__,const)) mad_hi( long3 a, long3 b, long3 c );
#endif
long4 __attribute__((__overloadable__,__always_inline__,const)) mad_hi( long4 a, long4 b, long4 c );
long8 __attribute__((__overloadable__,__always_inline__,const)) mad_hi( long8 a, long8 b, long8 c );
long16 __attribute__((__overloadable__,__always_inline__,const)) mad_hi( long16 a, long16 b, long16 c );
ulong __attribute__((__overloadable__,__always_inline__,const)) mad_hi(ulong a, ulong b, ulong c);
ulong2 __attribute__((__overloadable__,__always_inline__,const)) mad_hi( ulong2 a, ulong2 b, ulong2 c );
#if __OPENCL_C_VERSION__ >= 110
ulong3 __attribute__((__overloadable__,__always_inline__,const)) mad_hi( ulong3 a, ulong3 b, ulong3 c );
#endif
ulong4 __attribute__((__overloadable__,__always_inline__,const)) mad_hi( ulong4 a, ulong4 b, ulong4 c );
ulong8 __attribute__((__overloadable__,__always_inline__,const)) mad_hi( ulong8 a, ulong8 b, ulong8 c );
ulong16 __attribute__((__overloadable__,__always_inline__,const)) mad_hi( ulong16 a, ulong16 b, ulong16 c );
char __attribute__((__overloadable__,__always_inline__,const)) mad_sat(char a, char b, char c);
char2 __attribute__((__overloadable__,__always_inline__,const)) mad_sat( char2 a, char2 b, char2 c );
#if __OPENCL_C_VERSION__ >= 110
char3 __attribute__((__overloadable__,__always_inline__,const)) mad_sat( char3 a, char3 b, char3 c );
#endif
char4 __attribute__((__overloadable__,__always_inline__,const)) mad_sat( char4 a, char4 b, char4 c );
char8 __attribute__((__overloadable__,__always_inline__,const)) mad_sat( char8 a, char8 b, char8 c );
char16 __attribute__((__overloadable__,__always_inline__,const)) mad_sat( char16 a, char16 b, char16 c );
uchar __attribute__((__overloadable__,__always_inline__,const)) mad_sat(uchar a, uchar b, uchar c);
uchar2 __attribute__((__overloadable__,__always_inline__,const)) mad_sat( uchar2 a, uchar2 b, uchar2 c );
#if __OPENCL_C_VERSION__ >= 110
uchar3 __attribute__((__overloadable__,__always_inline__,const)) mad_sat( uchar3 a, uchar3 b, uchar3 c );
#endif
uchar4 __attribute__((__overloadable__,__always_inline__,const)) mad_sat( uchar4 a, uchar4 b, uchar4 c );
uchar8 __attribute__((__overloadable__,__always_inline__,const)) mad_sat( uchar8 a, uchar8 b, uchar8 c );
uchar16 __attribute__((__overloadable__,__always_inline__,const)) mad_sat( uchar16 a, uchar16 b, uchar16 c );
short __attribute__((__overloadable__,__always_inline__,const)) mad_sat(short a, short b, short c);
short2 __attribute__((__overloadable__,__always_inline__,const)) mad_sat( short2 a, short2 b, short2 c );
#if __OPENCL_C_VERSION__ >= 110
short3 __attribute__((__overloadable__,__always_inline__,const)) mad_sat( short3 a, short3 b, short3 c );
#endif
short4 __attribute__((__overloadable__,__always_inline__,const)) mad_sat( short4 a, short4 b, short4 c );
short8 __attribute__((__overloadable__,__always_inline__,const)) mad_sat( short8 a, short8 b, short8 c );
short16 __attribute__((__overloadable__,__always_inline__,const)) mad_sat( short16 a, short16 b, short16 c );
ushort __attribute__((__overloadable__,__always_inline__,const)) mad_sat(ushort a, ushort b, ushort c);
ushort2 __attribute__((__overloadable__,__always_inline__,const)) mad_sat( ushort2 a, ushort2 b, ushort2 c );
#if __OPENCL_C_VERSION__ >= 110
ushort3 __attribute__((__overloadable__,__always_inline__,const)) mad_sat( ushort3 a, ushort3 b, ushort3 c );
#endif
ushort4 __attribute__((__overloadable__,__always_inline__,const)) mad_sat( ushort4 a, ushort4 b, ushort4 c );
ushort8 __attribute__((__overloadable__,__always_inline__,const)) mad_sat( ushort8 a, ushort8 b, ushort8 c );
ushort16 __attribute__((__overloadable__,__always_inline__,const)) mad_sat( ushort16 a, ushort16 b, ushort16 c );
int __attribute__((__overloadable__,__always_inline__,const)) mad_sat(int a, int b, int c);
int2 __attribute__((__overloadable__,__always_inline__,const)) mad_sat( int2 a, int2 b, int2 c );
#if __OPENCL_C_VERSION__ >= 110
int3 __attribute__((__overloadable__,__always_inline__,const)) mad_sat( int3 a, int3 b, int3 c );
#endif
int4 __attribute__((__overloadable__,__always_inline__,const)) mad_sat( int4 a, int4 b, int4 c );
int8 __attribute__((__overloadable__,__always_inline__,const)) mad_sat( int8 a, int8 b, int8 c );
int16 __attribute__((__overloadable__,__always_inline__,const)) mad_sat( int16 a, int16 b, int16 c );
uint __attribute__((__overloadable__,__always_inline__,const)) mad_sat(uint a, uint b, uint c);
uint2 __attribute__((__overloadable__,__always_inline__,const)) mad_sat( uint2 a, uint2 b, uint2 c );
#if __OPENCL_C_VERSION__ >= 110
uint3 __attribute__((__overloadable__,__always_inline__,const)) mad_sat( uint3 a, uint3 b, uint3 c );
#endif
uint4 __attribute__((__overloadable__,__always_inline__,const)) mad_sat( uint4 a, uint4 b, uint4 c );
uint8 __attribute__((__overloadable__,__always_inline__,const)) mad_sat( uint8 a, uint8 b, uint8 c );
uint16 __attribute__((__overloadable__,__always_inline__,const)) mad_sat( uint16 a, uint16 b, uint16 c );
long __attribute__((__overloadable__,__always_inline__,const)) mad_sat(long a, long b, long c);
long2 __attribute__((__overloadable__,__always_inline__,const)) mad_sat( long2 a, long2 b, long2 c );
#if __OPENCL_C_VERSION__ >= 110
long3 __attribute__((__overloadable__,__always_inline__,const)) mad_sat( long3 a, long3 b, long3 c );
#endif
long4 __attribute__((__overloadable__,__always_inline__,const)) mad_sat( long4 a, long4 b, long4 c );
long8 __attribute__((__overloadable__,__always_inline__,const)) mad_sat( long8 a, long8 b, long8 c );
long16 __attribute__((__overloadable__,__always_inline__,const)) mad_sat( long16 a, long16 b, long16 c );
ulong __attribute__((__overloadable__,__always_inline__,const)) mad_sat(ulong a, ulong b, ulong c);
ulong2 __attribute__((__overloadable__,__always_inline__,const)) mad_sat( ulong2 a, ulong2 b, ulong2 c );
#if __OPENCL_C_VERSION__ >= 110
ulong3 __attribute__((__overloadable__,__always_inline__,const)) mad_sat( ulong3 a, ulong3 b, ulong3 c );
#endif
ulong4 __attribute__((__overloadable__,__always_inline__,const)) mad_sat( ulong4 a, ulong4 b, ulong4 c );
ulong8 __attribute__((__overloadable__,__always_inline__,const)) mad_sat( ulong8 a, ulong8 b, ulong8 c );
ulong16 __attribute__((__overloadable__,__always_inline__,const)) mad_sat( ulong16 a, ulong16 b, ulong16 c );
int __attribute__((__overloadable__,__always_inline__,const)) mad24(int a, int b, int c);
int __attribute__((__overloadable__,__always_inline__,const)) mul24(int a, int b);
int2 __attribute__((__overloadable__,__always_inline__,const)) mad24( int2 a, int2 b, int2 c);
int2 __attribute__((__overloadable__,__always_inline__,const)) mul24( int2 a, int2 b );
#if __OPENCL_C_VERSION__ >= 110
int3 __attribute__((__overloadable__,__always_inline__,const)) mad24( int3 a, int3 b, int3 c);
int3 __attribute__((__overloadable__,__always_inline__,const)) mul24( int3 a, int3 b );
#endif
int4 __attribute__((__overloadable__,__always_inline__,const)) mad24( int4 a, int4 b, int4 c);
int4 __attribute__((__overloadable__,__always_inline__,const)) mul24( int4 a, int4 b );
int8 __attribute__((__overloadable__,__always_inline__,const)) mad24( int8 a, int8 b, int8 c);
int8 __attribute__((__overloadable__,__always_inline__,const)) mul24( int8 a, int8 b );
int16 __attribute__((__overloadable__,__always_inline__,const)) mad24( int16 a, int16 b, int16 c);
int16 __attribute__((__overloadable__,__always_inline__,const)) mul24( int16 a, int16 b );
uint __attribute__((__overloadable__,__always_inline__,const)) mad24(uint a, uint b, uint c);
uint __attribute__((__overloadable__,__always_inline__,const)) mul24(uint a, uint b);
uint2 __attribute__((__overloadable__,__always_inline__,const)) mad24( uint2 a, uint2 b, uint2 c);
uint2 __attribute__((__overloadable__,__always_inline__,const)) mul24( uint2 a, uint2 b );
#if __OPENCL_C_VERSION__ >= 110
uint3 __attribute__((__overloadable__,__always_inline__,const)) mad24( uint3 a, uint3 b, uint3 c);
uint3 __attribute__((__overloadable__,__always_inline__,const)) mul24( uint3 a, uint3 b );
#endif
uint4 __attribute__((__overloadable__,__always_inline__,const)) mad24( uint4 a, uint4 b, uint4 c);
uint4 __attribute__((__overloadable__,__always_inline__,const)) mul24( uint4 a, uint4 b );
uint8 __attribute__((__overloadable__,__always_inline__,const)) mad24( uint8 a, uint8 b, uint8 c);
uint8 __attribute__((__overloadable__,__always_inline__,const)) mul24( uint8 a, uint8 b );
uint16 __attribute__((__overloadable__,__always_inline__,const)) mad24( uint16 a, uint16 b, uint16 c);
uint16 __attribute__((__overloadable__,__always_inline__,const)) mul24( uint16 a, uint16 b );
int __attribute__((__overloadable__,__always_inline__,const)) isequal(float a, float b);
int2 __attribute__((__overloadable__,__always_inline__,const)) isequal(float2 a, float2 b);
#if __OPENCL_C_VERSION__ >= 110
int3 __attribute__((__overloadable__,__always_inline__,const)) isequal(float3 a, float3 b);
#endif
int4 __attribute__((__overloadable__,__always_inline__,const)) isequal(float4 a, float4 b);
int8 __attribute__((__overloadable__,__always_inline__,const)) isequal(float8 a, float8 b);
int16 __attribute__((__overloadable__,__always_inline__,const)) isequal(float16 a, float16 b);
int __attribute__((__overloadable__,__always_inline__,const)) isequal(double a, double b);
long2 __attribute__((__overloadable__,__always_inline__,const)) isequal(double2 a, double2 b);
#if __OPENCL_C_VERSION__ >= 110
long3 __attribute__((__overloadable__,__always_inline__,const)) isequal(double3 a, double3 b);
#endif
long4 __attribute__((__overloadable__,__always_inline__,const)) isequal(double4 a, double4 b);
long8 __attribute__((__overloadable__,__always_inline__,const)) isequal(double8 a, double8 b);
long16 __attribute__((__overloadable__,__always_inline__,const)) isequal(double16 a, double16 b);
int __attribute__((__overloadable__,__always_inline__,const)) isnotequal(float a, float b);
int2 __attribute__((__overloadable__,__always_inline__,const)) isnotequal(float2 a, float2 b);
#if __OPENCL_C_VERSION__ >= 110
int3 __attribute__((__overloadable__,__always_inline__,const)) isnotequal(float3 a, float3 b);
#endif
int4 __attribute__((__overloadable__,__always_inline__,const)) isnotequal(float4 a, float4 b);
int8 __attribute__((__overloadable__,__always_inline__,const)) isnotequal(float8 a, float8 b);
int16 __attribute__((__overloadable__,__always_inline__,const)) isnotequal(float16 a, float16 b);
int __attribute__((__overloadable__,__always_inline__,const)) isnotequal(double a, double b);
long2 __attribute__((__overloadable__,__always_inline__,const)) isnotequal(double2 a, double2 b);
#if __OPENCL_C_VERSION__ >= 110
long3 __attribute__((__overloadable__,__always_inline__,const)) isnotequal(double3 a, double3 b);
#endif
long4 __attribute__((__overloadable__,__always_inline__,const)) isnotequal(double4 a, double4 b);
long8 __attribute__((__overloadable__,__always_inline__,const)) isnotequal(double8 a, double8 b);
long16 __attribute__((__overloadable__,__always_inline__,const)) isnotequal(double16 a, double16 b);
int __attribute__((__overloadable__,__always_inline__,const)) isgreater(float a, float b);
int2 __attribute__((__overloadable__,__always_inline__,const)) isgreater(float2 a, float2 b);
#if __OPENCL_C_VERSION__ >= 110
int3 __attribute__((__overloadable__,__always_inline__,const)) isgreater(float3 a, float3 b);
#endif
int4 __attribute__((__overloadable__,__always_inline__,const)) isgreater(float4 a, float4 b);
int8 __attribute__((__overloadable__,__always_inline__,const)) isgreater(float8 a, float8 b);
int16 __attribute__((__overloadable__,__always_inline__,const)) isgreater(float16 a, float16 b);
int __attribute__((__overloadable__,__always_inline__,const)) isgreater(double a, double b);
long2 __attribute__((__overloadable__,__always_inline__,const)) isgreater(double2 a, double2 b);
#if __OPENCL_C_VERSION__ >= 110
long3 __attribute__((__overloadable__,__always_inline__,const)) isgreater(double3 a, double3 b);
#endif
long4 __attribute__((__overloadable__,__always_inline__,const)) isgreater(double4 a, double4 b);
long8 __attribute__((__overloadable__,__always_inline__,const)) isgreater(double8 a, double8 b);
long16 __attribute__((__overloadable__,__always_inline__,const)) isgreater(double16 a, double16 b);
int __attribute__((__overloadable__,__always_inline__,const)) isgreaterequal(float a, float b);
int2 __attribute__((__overloadable__,__always_inline__,const)) isgreaterequal(float2 a, float2 b);
#if __OPENCL_C_VERSION__ >= 110
int3 __attribute__((__overloadable__,__always_inline__,const)) isgreaterequal(float3 a, float3 b);
#endif
int4 __attribute__((__overloadable__,__always_inline__,const)) isgreaterequal(float4 a, float4 b);
int8 __attribute__((__overloadable__,__always_inline__,const)) isgreaterequal(float8 a, float8 b);
int16 __attribute__((__overloadable__,__always_inline__,const)) isgreaterequal(float16 a, float16 b);
int __attribute__((__overloadable__,__always_inline__,const)) isgreaterequal(double a, double b);
long2 __attribute__((__overloadable__,__always_inline__,const)) isgreaterequal(double2 a, double2 b);
#if __OPENCL_C_VERSION__ >= 110
long3 __attribute__((__overloadable__,__always_inline__,const)) isgreaterequal(double3 a, double3 b);
#endif
long4 __attribute__((__overloadable__,__always_inline__,const)) isgreaterequal(double4 a, double4 b);
long8 __attribute__((__overloadable__,__always_inline__,const)) isgreaterequal(double8 a, double8 b);
long16 __attribute__((__overloadable__,__always_inline__,const)) isgreaterequal(double16 a, double16 b);
int __attribute__((__overloadable__,__always_inline__,const)) isless(float a, float b);
int2 __attribute__((__overloadable__,__always_inline__,const)) isless(float2 a, float2 b);
#if __OPENCL_C_VERSION__ >= 110
int3 __attribute__((__overloadable__,__always_inline__,const)) isless(float3 a, float3 b);
#endif
int4 __attribute__((__overloadable__,__always_inline__,const)) isless(float4 a, float4 b);
int8 __attribute__((__overloadable__,__always_inline__,const)) isless(float8 a, float8 b);
int16 __attribute__((__overloadable__,__always_inline__,const)) isless(float16 a, float16 b);
int __attribute__((__overloadable__,__always_inline__,const)) isless(double a, double b);
long2 __attribute__((__overloadable__,__always_inline__,const)) isless(double2 a, double2 b);
#if __OPENCL_C_VERSION__ >= 110
long3 __attribute__((__overloadable__,__always_inline__,const)) isless(double3 a, double3 b);
#endif
long4 __attribute__((__overloadable__,__always_inline__,const)) isless(double4 a, double4 b);
long8 __attribute__((__overloadable__,__always_inline__,const)) isless(double8 a, double8 b);
long16 __attribute__((__overloadable__,__always_inline__,const)) isless(double16 a, double16 b);
int __attribute__((__overloadable__,__always_inline__,const)) islessequal(float a, float b);
int2 __attribute__((__overloadable__,__always_inline__,const)) islessequal(float2 a, float2 b);
#if __OPENCL_C_VERSION__ >= 110
int3 __attribute__((__overloadable__,__always_inline__,const)) islessequal(float3 a, float3 b);
#endif
int4 __attribute__((__overloadable__,__always_inline__,const)) islessequal(float4 a, float4 b);
int8 __attribute__((__overloadable__,__always_inline__,const)) islessequal(float8 a, float8 b);
int16 __attribute__((__overloadable__,__always_inline__,const)) islessequal(float16 a, float16 b);
int __attribute__((__overloadable__,__always_inline__,const)) islessequal(double a, double b);
long2 __attribute__((__overloadable__,__always_inline__,const)) islessequal(double2 a, double2 b);
#if __OPENCL_C_VERSION__ >= 110
long3 __attribute__((__overloadable__,__always_inline__,const)) islessequal(double3 a, double3 b);
#endif
long4 __attribute__((__overloadable__,__always_inline__,const)) islessequal(double4 a, double4 b);
long8 __attribute__((__overloadable__,__always_inline__,const)) islessequal(double8 a, double8 b);
long16 __attribute__((__overloadable__,__always_inline__,const)) islessequal(double16 a, double16 b);
int __attribute__((__overloadable__,__always_inline__,const)) islessgreater(float a, float b);
int2 __attribute__((__overloadable__,__always_inline__,const)) islessgreater(float2 a, float2 b);
#if __OPENCL_C_VERSION__ >= 110
int3 __attribute__((__overloadable__,__always_inline__,const)) islessgreater(float3 a, float3 b);
#endif
int4 __attribute__((__overloadable__,__always_inline__,const)) islessgreater(float4 a, float4 b);
int8 __attribute__((__overloadable__,__always_inline__,const)) islessgreater(float8 a, float8 b);
int16 __attribute__((__overloadable__,__always_inline__,const)) islessgreater(float16 a, float16 b);
int __attribute__((__overloadable__,__always_inline__,const)) islessgreater(double a, double b);
long2 __attribute__((__overloadable__,__always_inline__,const)) islessgreater(double2 a, double2 b);
#if __OPENCL_C_VERSION__ >= 110
long3 __attribute__((__overloadable__,__always_inline__,const)) islessgreater(double3 a, double3 b);
#endif
long4 __attribute__((__overloadable__,__always_inline__,const)) islessgreater(double4 a, double4 b);
long8 __attribute__((__overloadable__,__always_inline__,const)) islessgreater(double8 a, double8 b);
long16 __attribute__((__overloadable__,__always_inline__,const)) islessgreater(double16 a, double16 b);
int __attribute__((__overloadable__,__always_inline__,const)) isordered(float a, float b);
int2 __attribute__((__overloadable__,__always_inline__,const)) isordered(float2 a, float2 b);
#if __OPENCL_C_VERSION__ >= 110
int3 __attribute__((__overloadable__,__always_inline__,const)) isordered(float3 a, float3 b);
#endif
int4 __attribute__((__overloadable__,__always_inline__,const)) isordered(float4 a, float4 b);
int8 __attribute__((__overloadable__,__always_inline__,const)) isordered(float8 a, float8 b);
int16 __attribute__((__overloadable__,__always_inline__,const)) isordered(float16 a, float16 b);
int __attribute__((__overloadable__,__always_inline__,const)) isordered(double a, double b);
long2 __attribute__((__overloadable__,__always_inline__,const)) isordered(double2 a, double2 b);
#if __OPENCL_C_VERSION__ >= 110
long3 __attribute__((__overloadable__,__always_inline__,const)) isordered(double3 a, double3 b);
#endif
long4 __attribute__((__overloadable__,__always_inline__,const)) isordered(double4 a, double4 b);
long8 __attribute__((__overloadable__,__always_inline__,const)) isordered(double8 a, double8 b);
long16 __attribute__((__overloadable__,__always_inline__,const)) isordered(double16 a, double16 b);
int __attribute__((__overloadable__,__always_inline__,const)) isunordered(float a, float b);
int2 __attribute__((__overloadable__,__always_inline__,const)) isunordered(float2 a, float2 b);
#if __OPENCL_C_VERSION__ >= 110
int3 __attribute__((__overloadable__,__always_inline__,const)) isunordered(float3 a, float3 b);
#endif
int4 __attribute__((__overloadable__,__always_inline__,const)) isunordered(float4 a, float4 b);
int8 __attribute__((__overloadable__,__always_inline__,const)) isunordered(float8 a, float8 b);
int16 __attribute__((__overloadable__,__always_inline__,const)) isunordered(float16 a, float16 b);
int __attribute__((__overloadable__,__always_inline__,const)) isunordered(double a, double b);
long2 __attribute__((__overloadable__,__always_inline__,const)) isunordered(double2 a, double2 b);
#if __OPENCL_C_VERSION__ >= 110
long3 __attribute__((__overloadable__,__always_inline__,const)) isunordered(double3 a, double3 b);
#endif
long4 __attribute__((__overloadable__,__always_inline__,const)) isunordered(double4 a, double4 b);
long8 __attribute__((__overloadable__,__always_inline__,const)) isunordered(double8 a, double8 b);
long16 __attribute__((__overloadable__,__always_inline__,const)) isunordered(double16 a, double16 b);
int  __attribute__((__overloadable__,__always_inline__,const))  isnormal(float x);
int  __attribute__((__overloadable__,__always_inline__,const))  isnormal(double x);
int2 __attribute__((__overloadable__,__always_inline__,const)) isnormal( float2 a );
#if __OPENCL_C_VERSION__ >= 110
int3 __attribute__((__overloadable__,__always_inline__,const)) isnormal( float3 a );
#endif
int4 __attribute__((__overloadable__,__always_inline__,const)) isnormal( float4 a );
int8 __attribute__((__overloadable__,__always_inline__,const)) isnormal( float8 a );
int16 __attribute__((__overloadable__,__always_inline__,const)) isnormal( float16 a );
long2 __attribute__((__overloadable__,__always_inline__,const)) isnormal( double2 a );
#if __OPENCL_C_VERSION__ >= 110
long3 __attribute__((__overloadable__,__always_inline__,const)) isnormal( double3 a );
#endif
long4 __attribute__((__overloadable__,__always_inline__,const)) isnormal( double4 a );
long8 __attribute__((__overloadable__,__always_inline__,const)) isnormal( double8 a );
long16 __attribute__((__overloadable__,__always_inline__,const)) isnormal( double16 a );
#define CLK_R 0x10B0
#define CLK_A 0x10B1
#define CLK_RG 0x10B2
#define CLK_RA 0x10B3
#define CLK_RGB 0x10B4
#define CLK_RGBA 0x10B5
#define CLK_BGRA 0x10B6
#define CLK_ARGB 0x10B7
#define CLK_INTENSITY 0x10B8
#define CLK_LUMINANCE 0x10B9
#if __OPENCL_C_VERSION__ >= 110
#define CLK_Rx 0x10BA
#define CLK_RGx 0x10BB
#define CLK_RGBx 0x10BC
#endif
#if __OPENCL_C_VERSION__ >= 120
#define CLK_DEPTH 0x10BD
#endif
#if __OPENCL_C_VERSION__ >= 200
#define CLK_sRGBA 0x10BE
#endif
#define CLK_SNORM_INT8 0x10D0
#define CLK_SNORM_INT16 0x10D1
#define CLK_UNORM_INT8 0x10D2
#define CLK_UNORM_INT16 0x10D3
#define CLK_UNORM_SHORT_565 0x10D4
#define CLK_UNORM_SHORT_555 0x10D5
#define CLK_UNORM_INT_101010 0x10D6
#define CLK_SIGNED_INT8 0x10D7
#define CLK_SIGNED_INT16 0x10D8
#define CLK_SIGNED_INT32 0x10D9
#define CLK_UNSIGNED_INT8 0x10DA
#define CLK_UNSIGNED_INT16 0x10DB
#define CLK_UNSIGNED_INT32 0x10DC
#define CLK_HALF_FLOAT 0x10DD
#define CLK_FLOAT 0x10DE
#define CLK_ADDRESS_NONE 0x00
#if __OPENCL_C_VERSION__ >= 110
#define CLK_ADDRESS_MIRRORED_REPEAT 0x01
#endif
#define CLK_ADDRESS_REPEAT 0x02
#define CLK_ADDRESS_CLAMP_TO_EDGE 0x03
#define CLK_ADDRESS_CLAMP 0x04
#define CLK_NORMALIZED_COORDS_FALSE 0x00
#define CLK_NORMALIZED_COORDS_TRUE 0x08
#define CLK_FILTER_NEAREST 0x00
#define CLK_FILTER_LINEAR 0x10
#pragma OPENCL EXTENSION cl_khr_3d_image_writes : enable

#if __OPENCL_C_VERSION__ >= 120
int __acl__get_image1d_array_array_size_ro (read_only image1d_array_t image);
int __acl__get_image1d_array_channel_data_type_ro (read_only image1d_array_t image);
int __acl__get_image1d_array_channel_order_ro (read_only image1d_array_t image);
size_t __acl__get_image1d_array_element_size_ro (read_only image1d_array_t image);
int __acl__get_image1d_array_width_ro (read_only image1d_array_t image);
int __acl__get_image1d_channel_data_type_ro (read_only image1d_t image);
int __acl__get_image1d_channel_order_ro (read_only image1d_t image);
size_t __acl__get_image1d_element_size_ro (read_only image1d_t image);
int __acl__get_image1d_width_ro (read_only image1d_t image);
int __acl__get_image2d_array_array_size_ro (read_only image2d_array_t image);
int __acl__get_image2d_array_channel_data_type_ro (read_only image2d_array_t image);
int __acl__get_image2d_array_channel_order_ro (read_only image2d_array_t image);
int2 __acl__get_image2d_array_dim_ro (read_only image2d_array_t image);
size_t __acl__get_image2d_array_element_size_ro (read_only image2d_array_t image);
int __acl__get_image2d_array_height_ro (read_only image2d_array_t image);
#endif
int __acl__get_image2d_channel_data_type_ro (read_only image2d_t image);
int __acl__get_image2d_channel_order_ro (read_only image2d_t image);
int2 __acl__get_image2d_dim_ro (read_only image2d_t image);
size_t __acl__get_image2d_element_size_ro (read_only image2d_t image);
int __acl__get_image2d_array_width_ro (read_only image2d_array_t image);
int __acl__get_image2d_height_ro (read_only image2d_t image);
int __acl__get_image2d_width_ro (read_only image2d_t image);
int __acl__get_image3d_channel_data_type_ro (read_only image3d_t image);
int __acl__get_image3d_channel_order_ro (read_only image3d_t image);
int __acl__get_image3d_depth_ro (read_only image3d_t image);
int4 __acl__get_image3d_dim_ro (read_only image3d_t image);
size_t __acl__get_image3d_element_size_ro (read_only image3d_t image);
int __acl__get_image3d_height_ro (read_only image3d_t image);
int __acl__get_image3d_width_ro (read_only image3d_t image);
#if __OPENCL_C_VERSION__ >= 120
int __acl__get_image1d_array_array_size_wo (write_only image1d_array_t image);
int __acl__get_image1d_array_channel_data_type_wo (write_only image1d_array_t image);
int __acl__get_image1d_array_channel_order_wo (write_only image1d_array_t image);
size_t __acl__get_image1d_array_element_size_wo (write_only image1d_array_t image);
int __acl__get_image1d_array_width_wo (write_only image1d_array_t image);
int __acl__get_image1d_channel_data_type_wo (write_only image1d_t image);
int __acl__get_image1d_channel_order_wo (write_only image1d_t image);
size_t __acl__get_image1d_element_size_wo (write_only image1d_t image);
int __acl__get_image1d_width_wo (write_only image1d_t image);
int __acl__get_image2d_array_array_size_wo (write_only image2d_array_t image);
int __acl__get_image2d_array_channel_data_type_wo (write_only image2d_array_t image);
int __acl__get_image2d_array_channel_order_wo (write_only image2d_array_t image);
int2 __acl__get_image2d_array_dim_wo (write_only image2d_array_t image);
size_t __acl__get_image2d_array_element_size_wo (write_only image2d_array_t image);
int __acl__get_image2d_array_height_wo (write_only image2d_array_t image);
#endif
int __acl__get_image2d_channel_data_type_wo (write_only image2d_t image);
int __acl__get_image2d_channel_order_wo (write_only image2d_t image);
int2 __acl__get_image2d_dim_wo (write_only image2d_t image);
size_t __acl__get_image2d_element_size_wo (write_only image2d_t image);
int __acl__get_image2d_array_width_wo (write_only image2d_array_t image);
int __acl__get_image2d_height_wo (write_only image2d_t image);
int __acl__get_image2d_width_wo (write_only image2d_t image);
int __acl__get_image3d_channel_data_type_wo (write_only image3d_t image);
int __acl__get_image3d_channel_order_wo (write_only image3d_t image);
int __acl__get_image3d_depth_wo (write_only image3d_t image);
int4 __acl__get_image3d_dim_wo (write_only image3d_t image);
size_t __acl__get_image3d_element_size_wo (write_only image3d_t image);
int __acl__get_image3d_height_wo (write_only image3d_t image);
int __acl__get_image3d_width_wo (write_only image3d_t image);
#if __OPENCL_C_VERSION__ >= 120
float4 __acl__read_image1df_array_fcoord (read_only image1d_array_t image, sampler_t sampler, float2 coord);
float4 __acl__read_image1df_array_icoord (read_only image1d_array_t image, sampler_t sampler, int2 coord);
float4 __acl__read_image1df_fcoord (read_only image1d_t image, sampler_t sampler, float coord);
float4 __acl__read_image1df_icoord (read_only image1d_t image, sampler_t sampler, int coord);
int4 __acl__read_image1di_array_fcoord (read_only image1d_array_t image, sampler_t sampler, float2 coord);
int4 __acl__read_image1di_array_icoord (read_only image1d_array_t image, sampler_t sampler, int2 coord);
int4 __acl__read_image1di_fcoord (read_only image1d_t image, sampler_t sampler, float coord);
int4 __acl__read_image1di_icoord (read_only image1d_t image, sampler_t sampler, int coord);
uint4 __acl__read_image1dui_array_fcoord (read_only image1d_array_t image, sampler_t sampler, float2 coord);
uint4 __acl__read_image1dui_array_icoord (read_only image1d_array_t image, sampler_t sampler, int2 coord);
uint4 __acl__read_image1dui_fcoord (read_only image1d_t image, sampler_t sampler, float coord);
float4 __acl__read_image2df_array_icoord (read_only image2d_array_t image, sampler_t sampler, int4 coord);
int4 __acl__read_image2di_array_fcoord (read_only image2d_array_t image, sampler_t sampler, float4 coord);
int4 __acl__read_image2di_array_icoord (read_only image2d_array_t image, sampler_t sampler, int4 coord);
#endif
float4 __acl__read_image2df_fcoord (read_only image2d_t image, sampler_t sampler, float2 coord);
float4 __acl__read_image2df_icoord (read_only image2d_t image, sampler_t sampler, int2 coord);
int4 __acl__read_image2di_fcoord (read_only image2d_t image, sampler_t sampler, float2 coord);
int4 __acl__read_image2di_icoord (read_only image2d_t image, sampler_t sampler, int2 coord);
uint4 __acl__read_image2dui_array_fcoord (read_only image2d_array_t image, sampler_t sampler, float4 coord);
uint4 __acl__read_image2dui_array_icoord (read_only image2d_array_t image, sampler_t sampler, int4 coord);
uint4 __acl__read_image2dui_fcoord (read_only image2d_t image, sampler_t sampler, float2 coord);
uint4 __acl__read_image2dui_icoord (read_only image2d_t image, sampler_t sampler, int2 coord);
float4 __acl__read_image3df_fcoord (read_only image3d_t image, sampler_t sampler, float4 coord);
float4 __acl__read_image3df_icoord (read_only image3d_t image, sampler_t sampler, int4 coord);
int4 __acl__read_image3di_fcoord (read_only image3d_t image, sampler_t sampler, float4 coord);
int4 __acl__read_image3di_icoord (read_only image3d_t image, sampler_t sampler, int4 coord);
uint4 __acl__read_image3dui_fcoord (read_only image3d_t image, sampler_t sampler, float4 coord);
uint4 __acl__read_image3dui_icoord (read_only image3d_t image, sampler_t sampler, int4 coord);
#if __OPENCL_C_VERSION__ >= 120
void __acl__write_image1df (write_only image1d_t image, int coord, float4 colour);
void __acl__write_image1df_array (write_only image1d_array_t image, int2 coord, float4 colour);
void __acl__write_image1di (write_only image1d_t image, int coord, int4 colour);
void __acl__write_image1di_array (write_only image1d_array_t image, int2 coord, int4 colour);
void __acl__write_image1dui (write_only image1d_t image, int coord, uint4 colour);
void __acl__write_image2df_array (write_only image2d_array_t image, int4 coord, float4 colour);
void __acl__write_image2di_array (write_only image2d_array_t image, int4 coord, int4 colour);
void __acl__write_image2dui_array (write_only image2d_array_t image, int4 coord, uint4 colour);
#endif
void __acl__write_image2di (write_only image2d_t image, int2 coord, int4 colour);
void __acl__write_image2dui (write_only image2d_t image, int2 coord, uint4 colour);
void __acl__write_image3df (write_only image3d_t image, int4 coord, float4 colour);
void __acl__write_image3di (write_only image3d_t image, int4 coord, int4 colour);
void __acl__write_image3dui (write_only image3d_t image, int4 coord, uint4 colour);
#if __OPENCL_C_VERSION__ >= 120
float4 __acl__read_image2df_array_fcoord (read_only image2d_array_t image, sampler_t sampler, float4 coord);
uint4 __acl__read_image1dui_icoord (read_only image1d_t image, sampler_t sampler, int coord);
#endif
void __acl__write_image2df (write_only image2d_t image, int2 coord, float4 colour);
#if __OPENCL_C_VERSION__ >= 120
void __acl__write_image1dui_array (write_only image1d_array_t image, int2 coord, uint4 colour);
__attribute__((__overloadable__,__always_inline__)) float4 read_imagef (read_only image1d_t image,                    sampler_t sampler,                    int coord);
__attribute__((__overloadable__,__always_inline__)) float4 read_imagef (read_only image1d_t image,                    sampler_t sampler,                    float coord);
__attribute__((__overloadable__,__always_inline__)) int4 read_imagei (read_only image1d_t image,                  sampler_t sampler,                  int coord);
__attribute__((__overloadable__,__always_inline__)) int4 read_imagei (read_only image1d_t image,                  sampler_t sampler,                  float coord);
__attribute__((__overloadable__,__always_inline__)) uint4 read_imageui (read_only image1d_t image,                    sampler_t sampler,                    int coord);
__attribute__((__overloadable__,__always_inline__)) uint4 read_imageui (read_only image1d_t image,                    sampler_t sampler,                    float coord);
__attribute__((__overloadable__,__always_inline__)) void write_imagef (write_only image1d_t image,                   int coord,                   float4 color);
__attribute__((__overloadable__,__always_inline__)) void write_imagei (write_only image1d_t image,                   int coord,                   int4 color);
__attribute__((__overloadable__,__always_inline__)) void write_imageui (write_only image1d_t image,                    int coord,                    uint4 color);
__attribute__((__overloadable__,__always_inline__)) float4 read_imagef (read_only image1d_array_t image,                    sampler_t sampler,                    int2 coord);
__attribute__((__overloadable__,__always_inline__)) float4 read_imagef (read_only image1d_array_t image,                    sampler_t sampler,                    float2 coord);
__attribute__((__overloadable__,__always_inline__)) int4 read_imagei (read_only image1d_array_t image,                  sampler_t sampler,                  int2 coord);
__attribute__((__overloadable__,__always_inline__)) int4 read_imagei (read_only image1d_array_t image,                  sampler_t sampler,                  float2 coord);
__attribute__((__overloadable__,__always_inline__)) uint4 read_imageui (read_only image1d_array_t image,                    sampler_t sampler,                    int2 coord);
__attribute__((__overloadable__,__always_inline__)) uint4 read_imageui (read_only image1d_array_t image,                    sampler_t sampler,                    float2 coord);
__attribute__((__overloadable__,__always_inline__)) void write_imagef (write_only image1d_array_t image,                   int2 coord,                   float4 color);
__attribute__((__overloadable__,__always_inline__)) void write_imagei (write_only image1d_array_t image,                   int2 coord,                   int4 color);
__attribute__((__overloadable__,__always_inline__)) void write_imageui (write_only image1d_array_t image,                   int2 coord,                   uint4 color);
#endif
__attribute__((__overloadable__,__always_inline__)) float4 read_imagef (read_only image2d_t image,                    sampler_t sampler,                    int2 coord);
__attribute__((__overloadable__,__always_inline__)) float4 read_imagef (read_only image2d_t image,                    sampler_t sampler,                    float2 coord);
__attribute__((__overloadable__,__always_inline__)) int4 read_imagei (read_only image2d_t image,                  sampler_t sampler,                  int2 coord);
__attribute__((__overloadable__,__always_inline__)) int4 read_imagei (read_only image2d_t image,                  sampler_t sampler,                  float2 coord);
__attribute__((__overloadable__,__always_inline__)) uint4 read_imageui (read_only image2d_t image,                    sampler_t sampler,                    int2 coord);
__attribute__((__overloadable__,__always_inline__)) uint4 read_imageui (read_only image2d_t image,                    sampler_t sampler,                    float2 coord);
__attribute__((__overloadable__,__always_inline__)) void write_imagef (write_only image2d_t image,                   int2 coord,                   float4 color);
__attribute__((__overloadable__,__always_inline__)) void write_imagei (write_only image2d_t image,                   int2 coord,                   int4 color);
__attribute__((__overloadable__,__always_inline__)) void write_imageui (write_only image2d_t image,                   int2 coord,                   uint4 color);
#if __OPENCL_C_VERSION__ >= 120
__attribute__((__overloadable__,__always_inline__)) float4 read_imagef (read_only image2d_array_t image,                    sampler_t sampler,                    int4 coord);
__attribute__((__overloadable__,__always_inline__)) float4 read_imagef (read_only image2d_array_t image,                    sampler_t sampler,                    float4 coord);
__attribute__((__overloadable__,__always_inline__)) int4 read_imagei (read_only image2d_array_t image,                  sampler_t sampler,                  int4 coord);
__attribute__((__overloadable__,__always_inline__)) int4 read_imagei (read_only image2d_array_t image,                  sampler_t sampler,                  float4 coord);
__attribute__((__overloadable__,__always_inline__)) uint4 read_imageui (read_only image2d_array_t image,                    sampler_t sampler,                    int4 coord);
__attribute__((__overloadable__,__always_inline__)) uint4 read_imageui (read_only image2d_array_t image,                    sampler_t sampler,                    float4 coord);
__attribute__((__overloadable__,__always_inline__)) void write_imagef (write_only image2d_array_t image,                   int4 coord,                   float4 color);
__attribute__((__overloadable__,__always_inline__)) void write_imagei (write_only image2d_array_t image,                   int4 coord,                   int4 color);
__attribute__((__overloadable__,__always_inline__)) void write_imageui (write_only image2d_array_t image,                    int4 coord,                    uint4 color);
#endif
__attribute__((__overloadable__,__always_inline__)) float4 read_imagef (read_only image3d_t image,                    sampler_t sampler,                    int4 coord);
__attribute__((__overloadable__,__always_inline__)) float4 read_imagef (read_only image3d_t image,                    sampler_t sampler,                    float4 coord);
__attribute__((__overloadable__,__always_inline__)) int4 read_imagei (read_only image3d_t image,                  sampler_t sampler,                  int4 coord);
__attribute__((__overloadable__,__always_inline__)) int4 read_imagei (read_only image3d_t image,                  sampler_t sampler,                  float4 coord);
__attribute__((__overloadable__,__always_inline__)) uint4 read_imageui (read_only image3d_t image,                    sampler_t sampler,                    int4 coord);
__attribute__((__overloadable__,__always_inline__)) uint4 read_imageui (read_only image3d_t image,                    sampler_t sampler,                    float4 coord);
__attribute__((__overloadable__,__always_inline__)) void write_imagef (write_only image3d_t image,                   int4 coord,                   float4 color);
__attribute__((__overloadable__,__always_inline__)) void write_imagei (write_only image3d_t image,                   int4 coord,                   int4 color);
__attribute__((__overloadable__,__always_inline__)) void write_imageui (write_only image3d_t image,                    int4 coord,                    uint4 color);
#if __OPENCL_C_VERSION__ >= 120
__attribute__((__overloadable__,__always_inline__)) int get_image_width (read_only image1d_t image);
__attribute__((__overloadable__,__always_inline__)) int get_image_channel_data_type (read_only image1d_t image);
__attribute__((__overloadable__,__always_inline__)) int  get_image_channel_order (read_only image1d_t image);
__attribute__((__overloadable__,__always_inline__))  size_t  get_image_element_size (read_only image1d_t image);
__attribute__((__overloadable__,__always_inline__)) int get_image_width (read_only image1d_array_t image);
__attribute__((__overloadable__,__always_inline__)) int get_image_array_size (read_only image1d_array_t image);
__attribute__((__overloadable__,__always_inline__))  int get_image_channel_data_type (read_only image1d_array_t image);
__attribute__((__overloadable__,__always_inline__))  int  get_image_channel_order (read_only image1d_array_t image);
__attribute__((__overloadable__,__always_inline__))  size_t  get_image_element_size (read_only image1d_array_t image);
#endif
__attribute__((__overloadable__,__always_inline__)) int get_image_width (read_only image2d_t image);
__attribute__((__overloadable__,__always_inline__)) int get_image_height (read_only image2d_t image);
__attribute__((__overloadable__,__always_inline__))  int get_image_channel_data_type (read_only image2d_t image);
__attribute__((__overloadable__,__always_inline__))  int  get_image_channel_order (read_only image2d_t image);
__attribute__((__overloadable__,__always_inline__))  int2  get_image_dim (read_only image2d_t image);
__attribute__((__overloadable__,__always_inline__))  size_t  get_image_element_size (read_only image2d_t image);
#if __OPENCL_C_VERSION__ >= 120
__attribute__((__overloadable__,__always_inline__)) int get_image_width (read_only image2d_array_t image);
__attribute__((__overloadable__,__always_inline__)) int get_image_height (read_only image2d_array_t image);
__attribute__((__overloadable__,__always_inline__)) int get_image_array_size (read_only image2d_array_t image);
__attribute__((__overloadable__,__always_inline__))  int get_image_channel_data_type (read_only image2d_array_t image);
__attribute__((__overloadable__,__always_inline__))  int  get_image_channel_order (read_only image2d_array_t image);
__attribute__((__overloadable__,__always_inline__))  int2  get_image_dim (read_only image2d_array_t image);
__attribute__((__overloadable__,__always_inline__))  size_t  get_image_element_size (read_only image2d_array_t image);
#endif
__attribute__((__overloadable__,__always_inline__)) int get_image_width (read_only image3d_t image);
__attribute__((__overloadable__,__always_inline__)) int get_image_height (read_only image3d_t image);
__attribute__((__overloadable__,__always_inline__)) int get_image_depth (read_only image3d_t image);
__attribute__((__overloadable__,__always_inline__))  int get_image_channel_data_type (read_only image3d_t image);
__attribute__((__overloadable__,__always_inline__))  int  get_image_channel_order (read_only image3d_t image);
__attribute__((__overloadable__,__always_inline__))  int4  get_image_dim (read_only image3d_t image);
__attribute__((__overloadable__,__always_inline__))  size_t  get_image_element_size (read_only image3d_t image);
#if __OPENCL_C_VERSION__ >= 120
__attribute__((__overloadable__,__always_inline__)) int get_image_width (write_only image1d_t image);
__attribute__((__overloadable__,__always_inline__))  int get_image_channel_data_type (write_only image1d_t image);
__attribute__((__overloadable__,__always_inline__))  int  get_image_channel_order (write_only image1d_t image);
__attribute__((__overloadable__,__always_inline__))  size_t  get_image_element_size (write_only image1d_t image);
__attribute__((__overloadable__,__always_inline__)) int get_image_width (write_only image1d_array_t image);
__attribute__((__overloadable__,__always_inline__)) int get_image_array_size (write_only image1d_array_t image);
__attribute__((__overloadable__,__always_inline__))  int get_image_channel_data_type (write_only image1d_array_t image);
__attribute__((__overloadable__,__always_inline__))  int  get_image_channel_order (write_only image1d_array_t image);
__attribute__((__overloadable__,__always_inline__))  size_t  get_image_element_size (write_only image1d_array_t image);
#endif
__attribute__((__overloadable__,__always_inline__)) int get_image_width (write_only image2d_t image);
__attribute__((__overloadable__,__always_inline__)) int get_image_height (write_only image2d_t image);
__attribute__((__overloadable__,__always_inline__))  int get_image_channel_data_type (write_only image2d_t image);
__attribute__((__overloadable__,__always_inline__))  int  get_image_channel_order (write_only image2d_t image);
__attribute__((__overloadable__,__always_inline__))  int2  get_image_dim (write_only image2d_t image);
__attribute__((__overloadable__,__always_inline__))  size_t  get_image_element_size (write_only image2d_t image);
#if __OPENCL_C_VERSION__ >= 120
__attribute__((__overloadable__,__always_inline__)) int get_image_width (write_only image2d_array_t image);
__attribute__((__overloadable__,__always_inline__)) int get_image_height (write_only image2d_array_t image);
__attribute__((__overloadable__,__always_inline__)) int get_image_array_size (write_only image2d_array_t image);
__attribute__((__overloadable__,__always_inline__))  int get_image_channel_data_type (write_only image2d_array_t image);
__attribute__((__overloadable__,__always_inline__))  int  get_image_channel_order (write_only image2d_array_t image);
__attribute__((__overloadable__,__always_inline__))  int2  get_image_dim (write_only image2d_array_t image);
__attribute__((__overloadable__,__always_inline__))  size_t  get_image_element_size (write_only image2d_array_t image);
#endif
__attribute__((__overloadable__,__always_inline__)) int get_image_width (write_only image3d_t image);
__attribute__((__overloadable__,__always_inline__)) int get_image_height (write_only image3d_t image);
__attribute__((__overloadable__,__always_inline__)) int get_image_depth (write_only image3d_t image);
__attribute__((__overloadable__,__always_inline__))  int get_image_channel_data_type (write_only image3d_t image);
__attribute__((__overloadable__,__always_inline__))  int  get_image_channel_order (write_only image3d_t image);
__attribute__((__overloadable__,__always_inline__))  int4  get_image_dim (write_only image3d_t image);
__attribute__((__overloadable__,__always_inline__))  size_t  get_image_element_size (write_only image3d_t image);
#pragma OPENCL EXTENSION cl_khr_3d_image_writes : disable
#if __OPENCL_C_VERSION__ >= 200
#define CL_DEVICE_PIPE_MAX_ACTIVE_RESERVATIONS 1
#define CLK_NULL_RESERVE_ID -1
#endif
#endif
