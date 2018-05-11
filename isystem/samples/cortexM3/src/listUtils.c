#include "listUtils.h"

#include "limits.h"

int maxElement(int *inList, size_t listSize)
{
    int elem = INT_MIN;
    for (size_t idx = 0; idx < listSize; idx++, inList++) {
        if (*inList > elem) {
            elem = *inList;
        }
    }

    return elem;
}


int minElement(int *inList, size_t listSize)
{
    int elem = INT_MAX;
    for (size_t idx = 0; idx < listSize; idx++, inList++) {
        if (*inList < elem) {
            elem = *inList;
        }
    }

    return elem;
}



int sumList(int *inList, size_t listSize)
{
    int sum = 0;
    for (size_t idx = 0; idx < listSize; idx++, inList++) {
        sum += *inList;
    }

    return sum;
}



int countGreaterThan(int *inList, size_t listSize, int limit)
{
    int count = 0;
    for (size_t idx = 0; idx < listSize; idx++, inList++) {
        if (*inList > limit) {
            count++;
        }
    }

    return count;
}



int countLessThan(int *inList, size_t listSize, int limit)
{
    int count = 0;
    for (size_t idx = 0; idx < listSize; idx++, inList++) {
        if (*inList < limit) {
            count++;
        }
    }

    return count;
}
