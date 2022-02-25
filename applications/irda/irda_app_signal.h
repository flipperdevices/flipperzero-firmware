/**
  * @file irda_app_signal.h
  * Infrared: Signal class
  */
#pragma once
#include <irda_worker.h>
#include <stdint.h>
#include <string>
#include <irda.h>

/** Irda application signal class */
class IrdaAppSignal {
public:
    /** Raw signal structure */
    typedef struct {
        /** Timings amount */
        size_t timings_cnt;
        /** Samples of raw signal in ms */
        uint32_t* timings;
        /** PWM Frequency of raw signal */
        uint32_t frequency;
        /** PWM Duty cycle of raw signal */
        float duty_cycle;
    } RawSignal;

private:
    /** if true - signal is raw, if false - signal is parsed */
    bool raw_signal;
    /** signal data, either raw or parsed */
    union {
        /** signal data for parsed signal */
        IrdaMessage message;
        /** raw signal data */
        RawSignal raw;
    } payload;

    /** Copy raw signal into object
     *
     * @param timings - timings (samples) of raw signal
     * @param size - number of timings
     * @frequency - PWM frequency of raw signal
     * @duty_cycle - PWM duty cycle
     */
    void
        copy_raw_signal(const uint32_t* timings, size_t size, uint32_t frequency, float duty_cycle);
    /** Clear and free timings data */
    void clear_timings();

public:
    /** Construct Irda signal class */
    IrdaAppSignal() {
        raw_signal = false;
        payload.message.protocol = IrdaProtocolUnknown;
    }

    /** Destruct signal class and free all allocated data */
    ~IrdaAppSignal() {
        clear_timings();
    }

    /** Construct object with raw signal
     *
     * @param timings - timings (samples) of raw signal
     * @param size - number of timings
     * @frequency - PWM frequency of raw signal
     * @duty_cycle - PWM duty cycle
     */
    IrdaAppSignal(
        const uint32_t* timings,
        size_t timings_cnt,
        uint32_t frequency,
        float duty_cycle);

    /** Construct object with parsed signal
     *
     * @param irda_message - parsed_signal to construct from
     */
    IrdaAppSignal(const IrdaMessage* irda_message);

    /** Copy constructor */
    IrdaAppSignal(const IrdaAppSignal& other);
    /** Move constructor */
    IrdaAppSignal(IrdaAppSignal&& other);

    /** Assignment operator */
    IrdaAppSignal& operator=(const IrdaAppSignal& signal);

    /** Set object to parsed signal
     *
     * @param irda_message - parsed_signal to construct from
     */
    void set_message(const IrdaMessage* irda_message);

    /** Set object to raw signal
     *
     * @param timings - timings (samples) of raw signal
     * @param size - number of timings
     * @frequency - PWM frequency of raw signal
     * @duty_cycle - PWM duty cycle
     */
    void
        set_raw_signal(uint32_t* timings, size_t timings_cnt, uint32_t frequency, float duty_cycle);

    /** Transmit held signal (???) */
    void transmit() const;

    /** Show is held signal raw
     *
     * @retval true if signal is raw, false if signal is parsed
     */
    bool is_raw(void) const {
        return raw_signal;
    }

    /** Get parsed signal.
     * User must check is_raw() signal before calling this function.
     *
     * @retval parsed signal pointer
     */
    const IrdaMessage& get_message(void) const {
        furi_assert(!raw_signal);
        return payload.message;
    }

    /** Get raw signal.
     * User must check is_raw() signal before calling this function.
     *
     * @retval raw signal
     */
    const RawSignal& get_raw_signal(void) const {
        furi_assert(raw_signal);
        return payload.raw;
    }
};
