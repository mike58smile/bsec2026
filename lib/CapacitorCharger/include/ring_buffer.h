#pragma once

#include <stdint.h>

#ifndef RING_BUFFER_SIZE
#define RING_BUFFER_SIZE 4
#endif

class RingBuffer {
public:
    RingBuffer() : m_index(0), m_count(0) {
        for (int i = 0; i < RING_BUFFER_SIZE; i++) {
            m_buffer[i] = 0.0f;
        }
    }

    void add_number(float number) {
        m_buffer[m_index] = number;
        m_index = (m_index + 1) % RING_BUFFER_SIZE;
        if (m_count < RING_BUFFER_SIZE) {
            m_count++;
        }
    }

    float get_mean() {
        if (m_count == 0) {
            return 0.0f;
        }
        float accumulator = 0.0f;
        for (int i = 0; i < m_count; i++) {
            accumulator += m_buffer[i];
        }
        return accumulator / m_count;
    }

    float get_max() {
        if (m_count == 0) {
            return 0.0f;
        }
        float lmax = m_buffer[0];
        for (int i = 1; i < m_count; i++) {
            if (m_buffer[i] > lmax) {
                lmax = m_buffer[i];
            }
        }
        return lmax;
    }

    float get_min() {
        if (m_count == 0) {
            return 0.0f;
        }
        float lmin = m_buffer[0];
        for (int i = 1; i < m_count; i++) {
            if (m_buffer[i] < lmin) {
                lmin = m_buffer[i];
            }
        }
        return lmin;
    }

    void clear() {
        m_index = 0;
        m_count = 0;
        for (int i = 0; i < RING_BUFFER_SIZE; i++) {
            m_buffer[i] = 0.0f;
        }
    }

    int count() {
        return m_count;
    }

    bool is_full() {
        return m_count == RING_BUFFER_SIZE;
    }

private:
    float m_buffer[RING_BUFFER_SIZE];
    int m_index;
    int m_count;
};
