#include <stdio.h>
#include <string.h> // for strcmp()
#include <ctype.h> // for isprint()

#include "hashtools.h"

HashIndex hashByAscii(AAKeyType key, size_t keyLength, HashIndex size);

/** check if the two keys are the same */
int
doKeysMatch(AAKeyType key1, size_t key1len, AAKeyType key2, size_t key2len)
{
	/** if the lengths don't match, the keys can't match */
	if (key1len != key2len)
		return 0;

	return memcmp(key1, key2, key1len) == 0;
}

/* provide the hex representation of a value */
static char toHex(int val)
{
	if (val < 10) return (char) ('0' + val);
	return (char) ('a' + (val - 10));
}

/**
 * Provide the key in a printable form.  Uses a static buffer,
 * which means that not only is this not thread-safe, but
 * even runs into trouble if called twice in the same printf().
 *
 * That said, is does provide a memory clean way to give a 
 * printable string return value to the calling code
 */
int
printableKey(char *buffer, int bufferlen, AAKeyType key, size_t printlen)
{
	int i, allChars = 1;
	char *loadptr;


	for (i = 0; allChars && i < printlen; i++) {
		if ( ! isprint(key[i])) allChars = 0;
	}

	if (allChars) {
		snprintf(buffer, bufferlen, "char key:[%s]", (char *) key);
	} else {
		snprintf(buffer, bufferlen, "hex key:[0x");
		loadptr = &buffer[strlen(buffer)];
		for (i = 0; i < printlen && loadptr - buffer < bufferlen - 4; i++) {
			*loadptr++ = toHex((key[i] & 0xf0) >> 4); // top nybble -> first hext digit
			*loadptr++ = toHex(key[i] & 0x0f);        // bottom nybble -> second digit
		}
		*loadptr++ = ']';
		*loadptr++ = 0;
	}
	return 1;
}

/**
 * Calculate a hash value based on the length of the key
 *
 * Calculate an integer index in the range [0...size-1] for
 * 		the given string key
 *
 *  @param  key  key to calculate mapping upon
 *  @param  size boundary for range of allowable return values
 *  @return      integer index associated with key
 *
 *  @see    HashAlgorithm
 */
HashIndex hashByLength(AAKeyType key, size_t keyLength, HashIndex size)
{
	return keyLength % size;
}



/**
 * Calculate a hash value based on the sum of the values in the key
 *
 * Calculate an integer index in the range [0...size-1] for
 * 		the given string key, based on the sum of the values
 *		in the key
 *
 *  param  key  key to calculate mapping upon
 *  param  size boundary for range of allowable return values
 *  return      integer index associated with key
 */
HashIndex hashBySum(AAKeyType key, size_t keyLength, HashIndex size)
{
	//printf("HASH BY SUM\n");

	HashIndex sum = 0;

	// sum of bytes
	for (int i = 0; i < keyLength; i++) {
		sum = (sum + key[i]);
	}

	/**
	 * TO DO: you will need to implement a summation based
	 * hashing algorithm here, using a sum-of-bytes
	 * strategy such as that discussed in class.  Take
	 * a look at HashByLength if you want an example
	 * of a "working" (but not very smart) hashing
	 * algorithm.
	 */

	return sum % size;
}


#include <stdlib.h>
HashIndex hashByAscii(AAKeyType key, size_t keyLength, HashIndex size)
{
	/* gets hash by subtracting the key's last 
	ascii value from the first */

	int asciiVal1 = (int)key[0];
	int asciiVal2 = (int)key[keyLength-1];

	/* if statement to check if key is an int: 
	key[0] will always equal key[keyLength-1] if key is an int */
	if (asciiVal2 - asciiVal1 != 0) {
		asciiVal1 = abs(asciiVal2 - asciiVal1);
	}

	return asciiVal1 % size;
}


/**
 * Locate an empty position in the given array, starting the
 * search at the indicated index, and restricting the search
 * to locations in the range [0...size-1]
 *
 *  @param  index where to begin the search
 *  @param  AssociativeArray associated AssociativeArray we are probing
 *  @param  invalidEndsSearch should the identification of a
 *				KeyDataPair marked invalid end our search?
 *				This is true if we are looking for a location
 *				to insert new data
 *  @return index of location where search stopped, or -1 if
 *				search failed
 *
 *  @see    HashProbe
 */
HashIndex linearProbe(AssociativeArray *hashTable,
		AAKeyType key, size_t keylength,
		int index, int invalidEndsSearch, int *cost
	)
{
	int count = 0;
	//printf("Linear probe:\n");

	/* increments by 1 until empty space is found */
	while (1) {

		// if index reaches end of table, index is set to 0
		if(index == hashTable->size) {
			index = 0;
		}

		/* invalidEndsSearch = 1: performs operations for aaInsert -- finds index of empty spots in table
		invalidEndsSearch = 0: performs operations for aaLookup -- finds index of matching key value */

		if (invalidEndsSearch == 1) {

			// breaks loop if empty space is found
			if (hashTable->table[index].validity == HASH_EMPTY) {
				//printf("HASH EMPTY\n");
				break;
			}

			// breaks loop if space is deleted and invalidEndsSearch is true
			if (hashTable->table[index].validity == HASH_DELETED) {
				//printf("HASH DELETED\n");
				break;
			}
		}
		
		if (invalidEndsSearch == 0) {

			// breaks loop if matching key value is found and key isn't deleted
			if (hashTable->table[index].validity == HASH_USED && 			// checks if key exists and not deleted
			memcmp(hashTable->table[index].key, key, keylength) == 0 &&		// compares if keys match
			hashTable->table[index].keylen == keylength) {					// compares if key length is the same (used for incorrect match)
				//printf("Found key");											// e.g. key = Broccoli
				break;															// table.key = Broccoli rabe
			}
		}

		index++;
		count++;

		// no empty space is found
		if(count == hashTable->size) {
			//printf("NO SPACE\n");
			return -1;
		}


	}

	*cost += count;		// for insertion, seach, and deletion costs
	return index;

	/**
	 * TO DO: you will need to implement an algorithm
	 * that probes until it finds an "empty" slot in
	 * the hashTable.  Note that because of tombstones,
	 * there are multiple ways of a slot being empty.
	 * Additionally, the code in HashTable depends on
	 * this code to find an actually empty slot, so
	 * this code is called with the results of the
	 * hash -- this means that the "index" value may
	 * already be valid on entry.
	 *
	 * Note that if an empty place cannot be found,
	 * you are to return (-1).  If a zero or positive
	 * value is returned, the calling code <i>will</i>
	 * use it, so be sure your return values are correct!
	 *
	 * For this routine, implement a "linear" probing
	 * strategy, such as that discussed in class.
	 */
	
}


/**
 * Locate an empty position in the given array, starting the
 * search at the indicated index, and restricting the search
 * to locations in the range [0...size-1]
 *
 *  @param  index where to begin the search
 *  @param  hashTable associated HashTable we are probing
 *  @param  invalidEndsSearch should the identification of a
 *				KeyDataPair marked invalid end our search?
 *				This is true if we are looking for a location
 *				to insert new data
 *  @return index of location where search stopped, or -1 if
 *				search failed
 *
 *  @see    HashProbe
 */
HashIndex quadraticProbe(AssociativeArray *hashTable, AAKeyType key, size_t keylen,
		int startIndex, int invalidEndsSearch,
		int *cost
	)
{
	//printf("Quadratic probe:\n");

	int count = 0;
	int index = startIndex;
	//printf("INDEX = %d\n", index);

	while (1) {

		/* invalidEndsSearch = 1: performs operations for aaInsert -- finds index of empty spots in table
		invalidEndsSearch = 0: performs operations for aaLookup -- finds index of matching key value */

		if (invalidEndsSearch == 1) {

			// breaks loop if empty space is found
			if (hashTable->table[index].validity == HASH_EMPTY) {
				//printf("HASH EMPTY\n");
				break;
			}

			// breaks loop if space is deleted and invalidEndsSearch is true
			if (hashTable->table[index].validity == HASH_DELETED) {
				//printf("HASH DELETED\n");
				break;
			}
		
		}

		if (invalidEndsSearch == 0) {

			// breaks loop if matching key value is found and key isn't deleted
			if (hashTable->table[index].validity == HASH_USED && 			// checks if key exists and not deleted
			memcmp(hashTable->table[index].key, key, keylen) == 0 &&		// compares if keys match
			hashTable->table[index].keylen == keylen) {					// compares if key length is the same (used for incorrect match)
				//printf("Found key");											// e.g. key = Broccoli
				break;															// table.key = Broccoli rabe
			}
		}

		count++;
		index = (startIndex + count*count) % hashTable->size;

		if(count == hashTable->size) {
			//printf("NO SPACE\n");
			return -1;
		}
		//printf("%d ", index);

	}
	//printf("\n");

	*cost += count;		// for insertion, seach, and deletion costs
	return index;
	/**
	 * TO DO: you will need to implement an algorithm
	 * that probes until it finds an "empty" slot in
	 * the hashTable.  Note that because of tombstones,
	 * there are multiple ways of a slot being empty.
	 * Additionally, the code in HashTable depends on
	 * this code to find an actually empty slot, so
	 * this code is called with the results of the
	 * hash -- this means that the "index" value may
	 * already be valid on entry.
	 *
	 * Note that if an empty place cannot be found,
	 * you are to return (-1).  If a zero or positive
	 * value is returned, the calling code <i>will</i>
	 * use it, so be sure your return values are correct!
	 *
	 * For this routine, implement a "quadratic" probing
	 * strategy, such as that discussed in class.
	 */

	//return -1;
}


/**
 * Locate an empty position in the given array, starting the
 * search at the indicated index, and restricting the search
 * to locations in the range [0...size-1]
 *
 *  @param  index where to begin the search
 *  @param  hashTable associated HashTable we are probing
 *  @param  invalidEndsSearch should the identification of a
 *				KeyDataPair marked invalid end our search?
 *				This is true if we are looking for a location
 *				to insert new data
 *  @return index of location where search stopped, or -1 if
 *				search failed
 *
 *  @see    HashProbe
 */
HashIndex doubleHashProbe(AssociativeArray *hashTable, AAKeyType key, size_t keylen,
		int startIndex, int invalidEndsSearch,
		int *cost
	)
{
	//printf("DOUBLE HASH\n");

	int index = startIndex;
	int count = 0;

	HashIndex stepSize = (hashTable->hashAlgorithmSecondary)(key, keylen, hashTable->size);

	/* increments by 1 until empty space is found */
	while (1) {

		/* invalidEndsSearch = 1: performs operations for aaInsert -- finds index of empty spots in table
		invalidEndsSearch = 0: performs operations for aaLookup -- finds index of matching key value */

		if (invalidEndsSearch == 1) {

			// breaks loop if empty space is found
			if (hashTable->table[index].validity == HASH_EMPTY) {
				//printf("HASH EMPTY\n");
				break;
			}

			// breaks loop if space is deleted and invalidEndsSearch is true
			if (hashTable->table[index].validity == HASH_DELETED) {
				//printf("HASH DELETED\n");
				break;
			}
		}
		
		if (invalidEndsSearch == 0) {

			// breaks loop if matching key value is found and key isn't deleted
			if (hashTable->table[index].validity == HASH_USED && 			// checks if key exists and not deleted
			memcmp(hashTable->table[index].key, key, keylen) == 0 &&		// compares if keys match
			hashTable->table[index].keylen == keylen) {					// compares if key length is the same (used for incorrect match)
				//printf("Found key");											// e.g. key = Broccoli
				break;															// table.key = Broccoli rabe
			}
		}

		index = (index + stepSize) % hashTable->size;
		count++;

		// if no empty space is found
		if(count == hashTable->size) {
			//printf("NO SPACE\n");
			return -1;
		}
	}

	/**
	 * TO DO: you will need to implement an algorithm
	 * that calls a second hash function (listed
	 * in the hashTable) and uses the value obtained
	 * as a result from that as the step size.
	 *
	 * Beyond that, the algorithm proceeds as with
	 * the above strategies.
	 */

	*cost += count;		// for insertion, seach, and deletion costs
	return index;
}

