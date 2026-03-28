/*
 *	atomics.h -- atomic/synchronising operations (aarch64 version)
 *	Copyright (C) 2000-2004 Fred Barnes
 *	Modifications Copyright (C) 2007 Carl Ritson <cgr@kent.ac.uk>
 *	Copyright (C) 2024 Amazon Q Developer (aarch64 adaptation)
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <inlining.h>

#ifndef AARCH64_ATOMICS_H
#define AARCH64_ATOMICS_H

#ifdef __GNUC__
#define _PACK_STRUCT __attribute__ ((packed))
#else
#warning "Unable to enforce alignment and packing on structures."
#define _PACK_STRUCT
#endif

typedef struct _atomic_t atomic_t;
struct _atomic_t {
	#if MAX_RUNTIME_THREADS > 1
	volatile unsigned int value;
	#else
	unsigned int value;
	#endif
} _PACK_STRUCT;

#undef _PACK_STRUCT

#define att_init(X, V)	do { (X)->value = (V); } while (0)

/*{{{  unsigned int att_val (atomic_t *atval)*/
static INLINE unsigned int att_safe_val (atomic_t *atval)
{
	return __atomic_load_n(&atval->value, __ATOMIC_ACQUIRE);
}
#define att_unsafe_val(X) 	((X)->value)
/*}}}*/
/*{{{  void att_set (atomic_t *atval, unsigned int value)*/
static INLINE void att_safe_set (atomic_t *atval, unsigned int value)
{
	__atomic_store_n(&atval->value, value, __ATOMIC_RELEASE);
}
#define att_unsafe_set(X,V)	do { (X)->value = (V); } while (0)
/*}}}*/
/*{{{  void att_inc (atomic_t *atval)*/
static INLINE void att_safe_inc (atomic_t *atval)
{
	__atomic_add_fetch(&atval->value, 1, __ATOMIC_ACQ_REL);
}
#define att_unsafe_inc(X)	do { (X)->value++; } while (0)
/*}}}*/
/*{{{  void att_dec (atomic_t *atval)*/
static INLINE void att_safe_dec (atomic_t *atval)
{
	__atomic_sub_fetch(&atval->value, 1, __ATOMIC_ACQ_REL);
}
#define att_unsafe_dec(X)	do { (X)->value--; } while (0)
/*}}}*/
/*{{{  unsigned int att_dec_z (atomic_t *atval)*/
static INLINE unsigned int att_safe_dec_z (atomic_t *atval)
{
	return __atomic_sub_fetch(&atval->value, 1, __ATOMIC_ACQ_REL) == 0;
}
#define att_unsafe_dec_z(X)	(!(--((X)->value)))
/*}}}*/
/*{{{  void att_add (atomic_t *atval, unsigned int value)*/
static INLINE void att_safe_add (atomic_t *atval, unsigned int value)
{
	__atomic_add_fetch(&atval->value, value, __ATOMIC_ACQ_REL);
}
#define att_unsafe_add(X,V)	do { (X)->value += (V); } while (0)
/*}}}*/
/*{{{  void att_sub (atomic_t *atval, unsigned int value)*/
static INLINE void att_safe_sub (atomic_t *atval, unsigned int value)
{
	__atomic_sub_fetch(&atval->value, value, __ATOMIC_ACQ_REL);
}
#define att_unsafe_sub(X,V)	do { (X)->value -= (V); } while (0)
/*}}}*/
/*{{{  unsigned int att_sub_z (atomic_t *atval, unsigned int value)*/
static INLINE unsigned int att_safe_sub_z (atomic_t *atval, unsigned int value)
{
	return __atomic_sub_fetch(&atval->value, value, __ATOMIC_ACQ_REL) == 0;
}

static INLINE unsigned int att_unsafe_sub_z (atomic_t *atval, unsigned int value)
{
	atval->value -= value;
	return atval->value == 0;
}
/*}}}*/
/*{{{  void att_or (atomic_t *atval, unsigned int bits)*/
static INLINE void att_safe_or (atomic_t *atval, unsigned int bits)
{
	__atomic_or_fetch(&atval->value, bits, __ATOMIC_ACQ_REL);
}
#define att_unsafe_or(X,M)	do { (X)->value |= (M); } while (0)
/*}}}*/
/*{{{  void att_and (atomic_t *atval, unsigned int bits)*/
static INLINE void att_safe_and (atomic_t *atval, unsigned int bits)
{
	__atomic_and_fetch(&atval->value, bits, __ATOMIC_ACQ_REL);
}
#define att_unsafe_and(X,M)	do { (X)->value &= (M); } while (0)
/*}}}*/
/*{{{  unsigned int att_swap (atomic_t *atval, unsigned int nv)*/
static INLINE unsigned int att_safe_swap (atomic_t *atval, unsigned int nv)
{
	return __atomic_exchange_n(&atval->value, nv, __ATOMIC_ACQ_REL);
}

static INLINE unsigned int att_unsafe_swap (atomic_t *atval, unsigned int nv) {
	unsigned int ov = atval->value;
	atval->value = nv;
	return ov;
}
/*}}}*/
/*{{{  unsigned int att_cas (atomic_t *atval, unsigned int ov, unsigned int nv) */
static INLINE unsigned int att_safe_cas (atomic_t *atval, unsigned int ov, unsigned int nv)
{
	return __atomic_compare_exchange_n(&atval->value, &ov, nv, 0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE);
}

static INLINE unsigned int att_unsafe_cas (atomic_t *atval, unsigned int ov, unsigned int nv)
{
	if (atval->value == ov) {
		atval->value = nv;
		return 1;
	}
	return 0;
}
/*}}}*/
/*{{{  void att_set_bit (atomic_t *atval, unsigned int bit)*/
static INLINE void att_safe_set_bit (atomic_t *atval, unsigned int bit)
{
	__atomic_or_fetch(&atval->value, 1U << bit, __ATOMIC_ACQ_REL);
}

static INLINE void att_unsafe_set_bit (atomic_t *atval, unsigned int bit)
{
	atval->value |= (1U << bit);
}
/*}}}*/
/*{{{  void att_clear_bit (atomic_t *atval, unsigned int bit)*/
static INLINE void att_safe_clear_bit (atomic_t *atval, unsigned int bit)
{
	__atomic_and_fetch(&atval->value, ~(1U << bit), __ATOMIC_ACQ_REL);
}

static INLINE void att_unsafe_clear_bit (atomic_t *atval, unsigned int bit)
{
	atval->value &= ~(1U << bit);
}
/*}}}*/
/*{{{  unsigned int att_test_set_bit (atomic_t *atval, unsigned int bit)*/
static INLINE unsigned int att_safe_test_set_bit (atomic_t *atval, unsigned int bit)
{
	unsigned int old = __atomic_fetch_or(&atval->value, 1U << bit, __ATOMIC_ACQ_REL);
	return (old >> bit) & 1;
}

static INLINE unsigned int att_unsafe_test_set_bit (atomic_t *atval, unsigned int bit)
{
	unsigned int old = atval->value;
	atval->value |= (1U << bit);
	return (old >> bit) & 1;
}
/*}}}*/
/*{{{  unsigned int att_test_clear_bit (atomic_t *atval, unsigned int bit)*/
static INLINE unsigned int att_safe_test_clear_bit (atomic_t *atval, unsigned int bit)
{
	unsigned int old = __atomic_fetch_and(&atval->value, ~(1U << bit), __ATOMIC_ACQ_REL);
	return (old >> bit) & 1;
}

static INLINE unsigned int att_unsafe_test_clear_bit (atomic_t *atval, unsigned int bit)
{
	unsigned int old = atval->value;
	atval->value &= ~(1U << bit);
	return (old >> bit) & 1;
}
/*}}}*/

/*{{{  atomic mappings */
#if MAX_RUNTIME_THREADS > 1
#define att_val(X)		att_safe_val(X)
#define att_set(X,V)		att_safe_set(X, V)
#define att_inc(X)		att_safe_inc(X)
#define att_dec(X)		att_safe_dec(X)
#define att_dec_z(X)		att_safe_dec_z(X)
#define att_add(X,V)		att_safe_add(X, V)
#define att_sub(X,V)		att_safe_sub(X, V)
#define att_sub_z(X,V)		att_safe_sub_z(X, V)
#define att_and(X,M)		att_safe_and(X, M)
#define att_swap(X,V)		att_safe_swap(X, V)
#define att_cas(X,O,N)		att_safe_cas(X, O, N)
#define	att_set_bit(X,B) 	att_safe_set_bit(X, B)
#define att_clear_bit(X,B)	att_safe_clear_bit(X, B)
#define att_test_set_bit(X,B)	att_safe_test_set_bit(X, B)
#define att_test_clear_bit(X,B)	att_safe_test_clear_bit(X, B)
#else
#define att_val(X)		att_unsafe_val(X)
#define att_set(X,V)		att_unsafe_set(X, V)
#define att_inc(X)		att_unsafe_inc(X)
#define att_dec(X)		att_unsafe_dec(X)
#define att_dec_z(X)		att_unsafe_dec_z(X)
#define att_add(X,V)		att_unsafe_add(X, V)
#define att_sub(X,V)		att_unsafe_sub(X, V)
#define att_sub_z(X,V)		att_unsafe_sub_z(X, V)
#define att_or(X,M)		att_unsafe_or(X, M)
#define att_and(X,M)		att_unsafe_and(X, M)
#define att_swap(X,V)		att_unsafe_swap(X, V)
#define att_cas(X,O,N)		att_unsafe_cas(X, O, N)
#define	att_set_bit(X,B) 	att_unsafe_set_bit(X, B)
#define att_clear_bit(X,B)	att_unsafe_clear_bit(X, B)
#define att_test_set_bit(X,B)	att_unsafe_test_set_bit(X, B)
#define att_test_clear_bit(X,B)	att_unsafe_test_clear_bit(X, B)
#endif
/*}}}*/

/*{{{  word size atomics */
#if defined(TARGET_CPU_AARCH64) || defined(TARGET_CPU_X64)
/* 64-bit pointer-sized atomics */
#define atw_val(W) 		((word) __atomic_load_n((word *)(W), __ATOMIC_ACQUIRE))
#define atw_set(W,V) 		__atomic_store_n((word *)(W), (word)(V), __ATOMIC_RELEASE)
#define atw_safe_val(W) 	((word) __atomic_load_n((word *)(W), __ATOMIC_ACQUIRE))
#define atw_safe_set(W,V)	__atomic_store_n((word *)(W), (word)(V), __ATOMIC_RELEASE)
#define atw_inc(W) 		__atomic_add_fetch((word *)(W), 1, __ATOMIC_ACQ_REL)
#define atw_dec(W) 		__atomic_sub_fetch((word *)(W), 1, __ATOMIC_ACQ_REL)
#define atw_dec_z(W) 		(__atomic_sub_fetch((word *)(W), 1, __ATOMIC_ACQ_REL) == 0)
#define atw_add(W,V) 		__atomic_add_fetch((word *)(W), (word)(V), __ATOMIC_ACQ_REL)
#define atw_sub(W,V) 		__atomic_sub_fetch((word *)(W), (word)(V), __ATOMIC_ACQ_REL)
#define atw_sub_z(W,V) 		(__atomic_sub_fetch((word *)(W), (word)(V), __ATOMIC_ACQ_REL) == 0)
#define atw_and(W,M)		__atomic_and_fetch((word *)(W), (word)(M), __ATOMIC_ACQ_REL)
#define atw_safe_swap(W,V)	((word) __atomic_exchange_n((word *)(W), (word)(V), __ATOMIC_ACQ_REL))
#define atw_swap(W,V)		((word) __atomic_exchange_n((word *)(W), (word)(V), __ATOMIC_ACQ_REL))
#define atw_safe_cas(W,O,N)	__atomic_compare_exchange_n((word *)(W), &(word){(word)(O)}, (word)(N), 0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)
#define atw_cas(W,O,N)		__atomic_compare_exchange_n((word *)(W), &(word){(word)(O)}, (word)(N), 0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)
#define	atw_set_bit(W,B) 	__atomic_or_fetch((word *)(W), (word)(1UL << (B)), __ATOMIC_ACQ_REL)
#define atw_clear_bit(W,B)	__atomic_and_fetch((word *)(W), (word)(~(1UL << (B))), __ATOMIC_ACQ_REL)
#define atw_test_set_bit(W,B)	((__atomic_fetch_or((word *)(W), (word)(1UL << (B)), __ATOMIC_ACQ_REL) >> (B)) & 1)
#define atw_test_clear_bit(W,B)	((__atomic_fetch_and((word *)(W), (word)(~(1UL << (B))), __ATOMIC_ACQ_REL) >> (B)) & 1)
#else
/* 32-bit atomics */
#define atw_val(W) 		((word) (att_val ((atomic_t *)((W)))))
#define atw_set(W,V) 		att_set ((atomic_t *)((W)), (unsigned int)(V))
#define atw_safe_val(W) 	((word) (att_safe_val ((atomic_t *)((W)))))
#define atw_safe_set(W,V)	att_safe_set ((atomic_t *)((W)), (unsigned int)(V))
#define atw_inc(W) 		att_inc ((atomic_t *)((W)))
#define atw_dec(W) 		att_dec ((atomic_t *)((W)))
#define atw_dec_z(W) 		(att_dec_z ((atomic_t *)((W))))
#define atw_add(W,V) 		att_add ((atomic_t *)((W)), (V))
#define atw_sub(W,V) 		att_sub ((atomic_t *)((W)), (V))
#define atw_sub_z(W,V) 		(att_sub_z ((atomic_t *)((W)), (V)))
#define atw_and(W,M)		att_and ((atomic_t *)((W)), (M))
#define atw_safe_swap(W,V)	((word) (att_safe_swap ((atomic_t *)((W)), (V))))
#define atw_swap(W,V)		((word) (att_swap ((atomic_t *)((W)), (V))))
#define atw_safe_cas(W,O,N)	(att_safe_cas ((atomic_t *)((W)), (O), (N)))
#define atw_cas(W,O,N)		(att_cas ((atomic_t *)((W)), (O), (N)))
#define	atw_set_bit(W,B) 	att_set_bit ((atomic_t *)((W)), (B))
#define atw_clear_bit(W,B)	att_clear_bit ((atomic_t *)((W)), (B))
#define atw_test_set_bit(W,B)	(att_test_set_bit ((atomic_t *)((W)), (B)))
#define atw_test_clear_bit(W,B)	(att_test_clear_bit ((atomic_t *)((W)), (B)))
#endif
/*}}}*/

#endif	/* !AARCH64_ATOMICS_H */