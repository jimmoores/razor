/*
 * aarch64_simd_math.c - ARM64 SIMD optimized trigonometric functions
 * Copyright (C) 2024 Amazon Q
 *
 * High-performance trigonometric functions using ARM64 NEON SIMD instructions
 * These provide hardware-accelerated alternatives to library functions
 */

#include <arm_neon.h>
#include <math.h>

/* ARM64 SIMD optimized cosine using polynomial approximation */
float __aarch64_cos_simd(float x) {
    /* Use NEON SIMD for vectorized polynomial evaluation */
    float32x4_t vx = vdupq_n_f32(x);
    
    /* Range reduction: x = x - 2*pi*round(x/(2*pi)) */
    const float32x4_t two_pi = vdupq_n_f32(6.28318530718f);
    const float32x4_t inv_two_pi = vdupq_n_f32(0.15915494309f);
    
    float32x4_t n = vrndnq_f32(vmulq_f32(vx, inv_two_pi));
    vx = vfmsq_f32(vx, n, two_pi);
    
    /* Polynomial approximation for cos(x) in [-pi, pi] */
    /* cos(x) ≈ 1 - x²/2! + x⁴/4! - x⁶/6! + x⁸/8! */
    const float32x4_t c0 = vdupq_n_f32(1.0f);
    const float32x4_t c2 = vdupq_n_f32(-0.5f);
    const float32x4_t c4 = vdupq_n_f32(0.04166666666f);
    const float32x4_t c6 = vdupq_n_f32(-0.00138888888f);
    const float32x4_t c8 = vdupq_n_f32(0.00002480158f);
    
    float32x4_t x2 = vmulq_f32(vx, vx);
    float32x4_t x4 = vmulq_f32(x2, x2);
    float32x4_t x6 = vmulq_f32(x4, x2);
    float32x4_t x8 = vmulq_f32(x4, x4);
    
    float32x4_t result = c0;
    result = vfmaq_f32(result, c2, x2);
    result = vfmaq_f32(result, c4, x4);
    result = vfmaq_f32(result, c6, x6);
    result = vfmaq_f32(result, c8, x8);
    
    return vgetq_lane_f32(result, 0);
}

/* ARM64 SIMD optimized sine using polynomial approximation */
float __aarch64_sin_simd(float x) {
    /* Use NEON SIMD for vectorized polynomial evaluation */
    float32x4_t vx = vdupq_n_f32(x);
    
    /* Range reduction: x = x - 2*pi*round(x/(2*pi)) */
    const float32x4_t two_pi = vdupq_n_f32(6.28318530718f);
    const float32x4_t inv_two_pi = vdupq_n_f32(0.15915494309f);
    
    float32x4_t n = vrndnq_f32(vmulq_f32(vx, inv_two_pi));
    vx = vfmsq_f32(vx, n, two_pi);
    
    /* Polynomial approximation for sin(x) in [-pi, pi] */
    /* sin(x) ≈ x - x³/3! + x⁵/5! - x⁷/7! + x⁹/9! */
    const float32x4_t c1 = vdupq_n_f32(1.0f);
    const float32x4_t c3 = vdupq_n_f32(-0.16666666666f);
    const float32x4_t c5 = vdupq_n_f32(0.00833333333f);
    const float32x4_t c7 = vdupq_n_f32(-0.00019841269f);
    const float32x4_t c9 = vdupq_n_f32(0.00000275573f);
    
    float32x4_t x2 = vmulq_f32(vx, vx);
    float32x4_t x3 = vmulq_f32(x2, vx);
    float32x4_t x5 = vmulq_f32(x3, x2);
    float32x4_t x7 = vmulq_f32(x5, x2);
    float32x4_t x9 = vmulq_f32(x7, x2);
    
    float32x4_t result = vmulq_f32(c1, vx);
    result = vfmaq_f32(result, c3, x3);
    result = vfmaq_f32(result, c5, x5);
    result = vfmaq_f32(result, c7, x7);
    result = vfmaq_f32(result, c9, x9);
    
    return vgetq_lane_f32(result, 0);
}

/* ARM64 SIMD optimized tangent */
float __aarch64_tan_simd(float x) {
    /* tan(x) = sin(x) / cos(x) using SIMD operations */
    float sin_x = __aarch64_sin_simd(x);
    float cos_x = __aarch64_cos_simd(x);
    
    /* Use NEON division */
    float32x2_t vsin = vdup_n_f32(sin_x);
    float32x2_t vcos = vdup_n_f32(cos_x);
    float32x2_t result = vdiv_f32(vsin, vcos);
    
    return vget_lane_f32(result, 0);
}

/* ARM64 SIMD optimized arc cosine */
float __aarch64_acos_simd(float x) {
    /* Use hardware sqrt and polynomial approximation */
    if (x >= 1.0f) return 0.0f;
    if (x <= -1.0f) return 3.14159265359f;
    
    /* acos(x) = π/2 - asin(x) for |x| <= 1 */
    const float pi_2 = 1.57079632679f;
    return pi_2 - __aarch64_asin_simd(x);
}

/* ARM64 SIMD optimized arc sine */
float __aarch64_asin_simd(float x) {
    /* Use NEON SIMD for polynomial approximation */
    if (x >= 1.0f) return 1.57079632679f;
    if (x <= -1.0f) return -1.57079632679f;
    
    float32x4_t vx = vdupq_n_f32(x);
    float32x4_t abs_x = vabsq_f32(vx);
    
    /* Polynomial approximation for asin(x) */
    const float32x4_t c0 = vdupq_n_f32(1.5707963050f);
    const float32x4_t c1 = vdupq_n_f32(-0.2145988016f);
    const float32x4_t c2 = vdupq_n_f32(0.0889789874f);
    const float32x4_t c3 = vdupq_n_f32(-0.0501743046f);
    
    float32x4_t sqrt_term = vsqrtq_f32(vsubq_f32(vdupq_n_f32(1.0f), abs_x));
    
    float32x4_t poly = c0;
    poly = vfmaq_f32(poly, c1, abs_x);
    poly = vfmaq_f32(poly, c2, vmulq_f32(abs_x, abs_x));
    poly = vfmaq_f32(poly, c3, vmulq_f32(vmulq_f32(abs_x, abs_x), abs_x));
    
    float32x4_t result = vfmsq_f32(vdupq_n_f32(1.5707963267f), poly, sqrt_term);
    
    /* Handle sign */
    float val = vgetq_lane_f32(result, 0);
    return (x < 0.0f) ? -val : val;
}

/* ARM64 SIMD optimized arc tangent */
float __aarch64_atan_simd(float x) {
    /* Use NEON SIMD for polynomial approximation */
    float32x4_t vx = vdupq_n_f32(x);
    float32x4_t abs_x = vabsq_f32(vx);
    
    /* Use different approximations for different ranges */
    float32x4_t result;
    
    if (vgetq_lane_f32(abs_x, 0) <= 1.0f) {
        /* Polynomial for |x| <= 1 */
        const float32x4_t c1 = vdupq_n_f32(0.99997726f);
        const float32x4_t c3 = vdupq_n_f32(-0.33262347f);
        const float32x4_t c5 = vdupq_n_f32(0.19354346f);
        const float32x4_t c7 = vdupq_n_f32(-0.11643287f);
        
        float32x4_t x2 = vmulq_f32(abs_x, abs_x);
        float32x4_t x3 = vmulq_f32(x2, abs_x);
        float32x4_t x5 = vmulq_f32(x3, x2);
        float32x4_t x7 = vmulq_f32(x5, x2);
        
        result = vmulq_f32(c1, abs_x);
        result = vfmaq_f32(result, c3, x3);
        result = vfmaq_f32(result, c5, x5);
        result = vfmaq_f32(result, c7, x7);
    } else {
        /* For |x| > 1, use atan(x) = π/2 - atan(1/x) */
        float32x4_t inv_x = vdivq_f32(vdupq_n_f32(1.0f), abs_x);
        result = vsubq_f32(vdupq_n_f32(1.5707963267f), __aarch64_atan_simd(vgetq_lane_f32(inv_x, 0)));
    }
    
    /* Handle sign */
    float val = vgetq_lane_f32(result, 0);
    return (x < 0.0f) ? -val : val;
}