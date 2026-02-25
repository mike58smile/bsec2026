#ifndef TIMED_EVENT_H
#define TIMED_EVENT_H

#include <Arduino.h>

/**
 * @brief Non-blocking periodic event timer, safe to call from an ISR.
 *
 * Uses a simple millisecond counter incremented externally (via tick())
 * instead of millis(), so it works correctly inside interrupts where
 * millis() does not advance.
 */
class TimedEvent
{
public:
  /**
   * @brief Construct a default TimedEvent (must call set() to configure).
   */
  TimedEvent() = default;

  /**
   * @brief Construct a TimedEvent with period and callback.
   * @param periodMs  Interval in milliseconds between callback invocations.
   * @param callback  Function pointer to call on each interval.
   * @param enabled   Whether the event starts enabled (default: true).
   */
  TimedEvent(unsigned long periodMs, void (*callback)(), bool enabled = true)
      : m_period(periodMs), m_callback(callback), m_enabled(enabled), m_counter(0) {}

  /**
   * @brief Configure the event after default construction.
   */
  void set(unsigned long periodMs, void (*callback)()) {
    m_period = periodMs;
    m_callback = callback;
    m_counter = 0;
    m_enabled = true;
  }

  /**
   * @brief Call this from a 1 ms hardware timer ISR.
   *        Increments the internal counter and fires the callback when due.
   */
  void tick() {
    if (!m_enabled || m_callback == nullptr) return;
    m_counter++;
    if (m_counter >= m_period) {
      m_counter = 0;
      m_callback();
    }
  }

  /**
   * @brief Convenience wrapper — same as tick(), kept for non-ISR use.
   *        When used outside ISR, call from loop() at ~1 ms intervals
   *        or use tick() from the hardware timer instead.
   */
  void update() { tick(); }

  /** @brief Enable the event. */
  void enable()  { m_enabled = true; }

  /** @brief Disable the event (callback stops firing). */
  void disable() { m_enabled = false; }

  /** @brief Check if the event is enabled. */
  bool isEnabled() const { return m_enabled; }

  /** @brief Reset the counter so the next callback fires after a full period. */
  void reset() { m_counter = 0; }

  /** @brief Change the period at runtime (milliseconds). */
  void setPeriod(unsigned long periodMs) { m_period = periodMs; }

  /** @brief Get the current period in milliseconds. */
  unsigned long getPeriod() const { return m_period; }

private:
  volatile unsigned long m_period   = 0;
  volatile unsigned long m_counter  = 0;
  void (*m_callback)()              = nullptr;
  volatile bool m_enabled           = false;
};

#endif
