#pragma once

template<typename T>
class ArrayList {
private:
    size_t m_arraySize;
    size_t m_increment;
    size_t m_numElements;
    T *m_array;
    
public:
    ArrayList(int initialCapacity, int increment) :
        m_arraySize(initialCapacity),
        m_increment(increment),
        m_numElements(0),
        m_array(new T[initialCapacity])
    {}

    
    void add(T element) {
        if (m_numElements == m_arraySize) {
            T *newArray = new T[m_arraySize + m_increment];
            memcpy(m_array, newArray, m_arraySize * sizeof(T));
            m_array = newArray;
            m_arraySize += m_increment;
            delete [] m_array;
        }

        m_array[m_numElements] = element;
        m_numElements++;
    }


    T &get(size_t idx) {
        return m_array[idx];
    }
};
