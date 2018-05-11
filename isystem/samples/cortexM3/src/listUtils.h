#pragma once

#include "string.h"

/**
 * Finds maximum element in the list.
 *
 * @param inList input list
 * @param listSize number of elements in the input list
 *
 * return maximum element in the list, or MAX_INT if the list is empty.
 */
int maxElement(int *inList, size_t listSize);

/**
 * Finds minimum element in the list.
 *
 * @param inList input list
 * @param listSize number of elements in the input list
 *
 * return minimum element in the list, or MAX_INT if the list is empty.
 */
int minElement(int *inList, size_t listSize);

/**
 * Sums all elements in the input list. Does not check for overflow!
 *
 * @param inList input list
 * @param listSize number of elements in the input list
 *
 * @return sum of all elements in the list.
 */   
int sumList(int *inList, size_t listSize);

/**
 * Counts all elements in the input list, which are greater than the given limit.
 *
 * @param inList input list
 * @param listSize number of elements in the input list
 * @limit value to compare against.
 *
 * @return the number of elements greater than the given limit.
 */
int countGreaterThan(int *inList, size_t listSize, int limit);

/**
 * Counts all elements in the input list, which are greater than the given limit.
 *
 * @param inList input list
 * @param listSize number of elements in the input list
 * @limit value to compare against.
 *
 * @return the number of elements smaller than the given limit.
 */
int countLessThan(int *inList, size_t listSize, int limit);
