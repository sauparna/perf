#ifndef BITARRAY_H
#define BITARRAY_H

#include <sys/types.h>
#include <stdbool.h>
#include <stdint.h>

/***************************************************************************/
/* Types                                                                   */
/***************************************************************************/

typedef uint64_t int_t;
typedef struct bitarray bitarray_t; /* ADT representing an array of bits */

/***************************************************************************/
/* Prototypes                                                              */
/***************************************************************************/

/* Allocate space for a new bit array. 
   bit_sz is the number of bits storable in the resultant bit array.
*/
bitarray_t* bitarray_new(const size_t bit_sz);

/* Free a bit array allocated by bitarray_new. */
void bitarray_free(bitarray_t* const bitarray);

/* Get the number of bits stored in a bit array.
   Invariant: bitarray_get_bit_sz(bitarray_new(n)) = n.
*/
size_t bitarray_get_bit_sz(const bitarray_t* const bitarray);

/* Fill in random bits the enitre bit array */
void bitarray_randfill(bitarray_t* const bitarray);

/* Retreive the bit at the specified zero-based index */
extern bool bitarray_get(const bitarray_t* const bitarray, const size_t bit_index);

/* Set the bit at the specified zero-based index */
extern void bitarray_set(bitarray_t* const bitarray,
                  const size_t bit_index,
                  const bool value);

/* Rotate a subarray.
   
   bit_offset is the index of the start of the subarray
   bit_length is the lenght of the subarray, in bits
   bit_right_amount is the number of places to rotate the subarray right

   The subarray spans the half-open interval
   [bit_offset, bit_offset + bit_length)
   That is, the start is inclusive, but the end is exclusive.

   Note: bit_right_amount can be negative, in which case a left rotation is
   performed.

   Example:
   Let ba be a bit array containing the bytes 0b10010110; then,
   bitarray_rotate(ba, 0, bitarray_get_bit_sz(ba), -1)
   left-rotates the entire bit array in place. After the rotation, ba
   contains the byte 0b00101101.

   Example:
   Let ba be a bit array containing the bytes 0b10010110; then,
   bitarray_rotate(ba, 2, 5, 2) rotates the third through seventh
   (inclusive) bits right two places. After the rotation, ba contains the
   bytes 0b10110100.
 */
void bitarray_rotate(bitarray_t* const bitarray,
                     const size_t bit_offset,
                     const size_t bit_length,
                     const ssize_t shift);

#endif // BITARRAY_H
