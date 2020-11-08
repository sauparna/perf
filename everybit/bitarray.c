/* Implements the ADT specified in bitarray.h as a packed array of bits; a bit
   array containing bit_sz bits will consume roughly bit_sz / 8 bytes of
   memory.
 */

#include "./bitarray.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#include <sys/types.h>

/***************************************************************************/
/* Types                                                                   */
/***************************************************************************/

/* Concrete data type representing an array of bits. */
struct bitarray {
    size_t bit_sz; /* The number of bits represented by this bit array */
    int_t* buf; /* The underlying memory buffer that stores the bits */
    size_t int_sz;
};

static int_t setbit[64];

/***************************************************************************/
/* Prototypes for static functions                                         */
/***************************************************************************/

static int modulo(const int x, const int y);
static void build_setbit_array(const size_t int_sz);
static inline void bitarray_reverse(bitarray_t* const bitarray, size_t i, size_t j);

/***************************************************************************/
/* Functions                                                               */
/***************************************************************************/

bitarray_t* bitarray_new(const size_t bit_sz)
{
    size_t int_sz = sizeof(int_t) * 8;
    build_setbit_array(int_sz);

    /* Allocate underlying buffer of ceil(bit_sz / int_sz) bytes. */
    int_t* const buf = calloc((bit_sz / int_sz) + 1, sizeof(int_sz));
    if (buf == NULL)
        return NULL;

    /* Allocate space for the struct. */
    bitarray_t* const bitarray = malloc(sizeof(struct bitarray));
    if (bitarray == NULL)
    {
        free(buf);
        return NULL;
    }

    bitarray->buf = buf;
    bitarray->bit_sz = bit_sz;
    bitarray->int_sz = int_sz;
    return bitarray;
}

void bitarray_free(bitarray_t* const bitarray)
{
    if (bitarray == NULL)
        return;
    free(bitarray->buf);
    bitarray->buf = NULL;
    free(bitarray);
}

size_t bitarray_get_bit_sz(const bitarray_t* const bitarray)
{
    return bitarray->bit_sz;
}

inline bool bitarray_get(const bitarray_t* const bitarray, const size_t bit_index)
{
    size_t int_sz = bitarray->int_sz;
    return (bitarray->buf[bit_index / int_sz] & setbit[bit_index % int_sz]) ?
         true : false;
}    

inline void bitarray_set(bitarray_t* const bitarray,
                         const size_t bit_index,
                         const bool value)
{
    size_t int_sz = bitarray->int_sz;
    bitarray->buf[bit_index / int_sz] =
        (bitarray->buf[bit_index / int_sz] & ~setbit[bit_index % int_sz]) |
        (value ? setbit[bit_index % int_sz] : 0);
}

void bitarray_randfill(bitarray_t* const bitarray)
{
    int64_t* ptr = (int64_t*)bitarray->buf;
    for (int64_t i = 0; i < bitarray->bit_sz / 64 + 1; ++i)
        ptr[i] = rand();
}

static inline void bitarray_reverse(bitarray_t* const bitarray, size_t i, size_t j)
{
    if (!bitarray) return;
    while (i < j)
    {
        bool bit_i = bitarray_get(bitarray, i);
        bitarray_set(bitarray, i, bitarray_get(bitarray, j));
        bitarray_set(bitarray, j, bit_i);
        ++i; --j;
    }
}

void bitarray_rotate(bitarray_t* const bitarray,
                     const size_t bit_offset,
                     const size_t bit_length,
                     const ssize_t shift)
{
    assert(bit_offset + bit_length <= bitarray->bit_sz);
    if (bit_length == 0)
        return;

    /* bit_length >= 0, therefore, 0 <= modulo(shift, bit_length) < bit_length */
    size_t shift_ = (size_t)modulo(shift, bit_length);
    
    size_t p, q, r;
    p = bit_offset;
    r = p + bit_length - 1;
    q = r - shift_ + 1;

    int i, j, di, mi, dj, mj;
    int_t* buf = bitarray->buf;
    size_t int_sz = bitarray->int_sz;

    /* reverse bits in range [p, q - 1] */
    i = p; j = q - 1;
       while (i < j)
    {
        di = i / int_sz; mi = i % int_sz;
        dj = j / int_sz; mj = j % int_sz;

        int_t bi = buf[di] & setbit[mi];
        int_t bj = buf[dj] & setbit[mj];
        if (bi != 0 && bj == 0)
        {
            buf[di] &= ~setbit[mi];
            buf[dj] |=  setbit[mj];
        }
        else if (bi == 0 && bj != 0)
        {
            buf[di] |=  setbit[mi];
            buf[dj] &= ~setbit[mj];
        }
        ++i; --j;
    }
    
    /* reverse bits in range [q, r] */
    i = q; j = r;
    while (i < j)
    {
        di = i / int_sz; mi = i % int_sz;
        dj = j / int_sz; mj = j % int_sz;

        int_t bi = buf[di] & setbit[mi];
        int_t bj = buf[dj] & setbit[mj];
        if (bi != 0 && bj == 0)
        {
            buf[di] &= ~setbit[mi];
            buf[dj] |=  setbit[mj];
        }
        else if (bi == 0 && bj != 0)
        {
            buf[di] |=  setbit[mi];
            buf[dj] &= ~setbit[mj];
        }
        ++i; --j;
    }
    
    /* reverse bits in range [p, r] */
    i = p; j = r;
    while (i < j)
    {
        di = i / int_sz; mi = i % int_sz;
        dj = j / int_sz; mj = j % int_sz;

        int_t bi = buf[di] & setbit[mi];
        int_t bj = buf[dj] & setbit[mj];
        if (bi != 0 && bj == 0)
        {
            buf[di] &= ~setbit[mi];
            buf[dj] |=  setbit[mj];
        }
        else if (bi == 0 && bj != 0)
        {
            buf[di] |=  setbit[mi];
            buf[dj] &= ~setbit[mj];
        }
        ++i; --j;
    }

    buf = NULL;
}

static int modulo(const int x, const int y)
{
    if (y == 0) return 0;
    return x - y * (int)(floorf((float)x / (float)y));
}

static void build_setbit_array(const size_t int_sz)
{
    setbit[int_sz - 1] = 1;
    for (int i = int_sz - 2; i >= 0; --i)
        setbit[i] = setbit[i + 1] << 1;
}
