// all constants taken from https://github.com/adafruit/Adafruit-Si4713-Library/blob/master/Adafruit_Si4713.h

#define SI4710_ADDR0 0x11 ///< if SEN is low
#define SI4710_ADDR1 0x63 ///< if SEN is high, default!
#define SI4710_STATUS_CTS 0x80 ///< read status

/* COMMANDS */
#define SI4710_CMD_POWER_UP \
    0x01 ///< Power up device and mode selection. Modes include FM transmit and
///< analog/digital audio interface configuration.
#define SI4710_CMD_GET_REV 0x10 ///< Returns revision information on the device.
#define SI4710_CMD_POWER_DOWN 0x11 ///< Power down device
#define SI4710_CMD_SET_PROPERTY 0x12 ///< Sets the value of a property.
#define SI4710_CMD_GET_PROPERTY 0x13 ///< Retrieves a property’s value.
#define SI4710_CMD_GET_INT_STATUS 0x14 ///< Read interrupt status bits.
#define SI4710_CMD_PATCH_ARGS 0x15 ///< Reserved command used for patch file downloads.
#define SI4710_CMD_PATCH_DATA 0x16 ///< Reserved command used for patch file downloads.
#define SI4710_CMD_TX_TUNE_FREQ 0x30 ///< Tunes to given transmit frequency.
#define SI4710_CMD_TX_TUNE_POWER \
    0x31 ///< Sets the output power level and tunes the antenna capacitor
#define SI4710_CMD_TX_TUNE_MEASURE \
    0x32 ///< Measure the received noise level at the specified frequency.
#define SI4710_CMD_TX_TUNE_STATUS \
    0x33 ///< Queries the status of a previously sent TX Tune Freq, TX Tune Power,
///< or TX Tune Measure command.
#define SI4710_CMD_TX_ASQ_STATUS 0x34 ///< Queries the TX status and input audio signal metrics.
#define SI4710_CMD_TX_RDS_BUFF \
    0x35 ///< Queries the status of the RDS Group Buffer and loads new data into
///< buffer.
#define SI4710_CMD_TX_RDS_PS 0x36 ///< Set up default PS strings
#define SI4710_CMD_GPO_CTL 0x80 ///< Configures GPO3 as output or Hi-Z.
#define SI4710_CMD_GPO_SET 0x81 ///< Sets GPO3 output level (low or high).

/* Parameters */
#define SI4713_PROP_GPO_IEN 0x0001 ///< Enables interrupt sources.
#define SI4713_PROP_DIGITAL_INPUT_FORMAT 0x0101 ///< Configures the digital input format.
#define SI4713_PROP_DIGITAL_INPUT_SAMPLE_RATE \
    0x0103 ///< Configures the digital input sample rate in 10 Hz steps. Default
///< is 0.
#define SI4713_PROP_REFCLK_FREQ \
    0x0201 ///< Sets frequency of the reference clock in Hz. The range is 31130 to
///< 34406 Hz, or 0 to disable the AFC. Default is 32768 Hz.
#define SI4713_PROP_REFCLK_PRESCALE 0x0202 ///< Sets the prescaler value for the reference clock.
#define SI4713_PROP_TX_COMPONENT_ENABLE \
    0x2100 ///< Enable transmit multiplex signal components. Default has pilot and
///< L-R enabled.
#define SI4713_PROP_TX_AUDIO_DEVIATION \
    0x2101 ///< Configures audio frequency deviation level. Units are in 10 Hz
///< increments. Default is 6285 (68.25 kHz).
#define SI4713_PROP_TX_PILOT_DEVIATION \
    0x2102 ///< Configures pilot tone frequency deviation level. Units are in 10
///< Hz increments. Default is 675 (6.75 kHz)
#define SI4713_PROP_TX_RDS_DEVIATION \
    0x2103 ///< Configures the RDS/RBDS frequency deviation level. Units are in 10
///< Hz increments. Default is 2 kHz.
#define SI4713_PROP_TX_LINE_LEVEL_INPUT_LEVEL \
    0x2104 ///< Configures maximum analog line input level to the LIN/RIN pins to
///< reach the maximum deviation level pro- grammed into the audio
///< deviation property TX Audio Deviation. Default is 636 mVPK.
#define SI4713_PROP_TX_LINE_INPUT_MUTE \
    0x2105 ///< Sets line input mute. L and R inputs may be indepen- dently muted.
///< Default is not muted.
#define SI4713_PROP_TX_PREEMPHASIS \
    0x2106 ///< Configures pre-emphasis time constant. Default is 0 (75 μS).
#define SI4713_PROP_TX_PILOT_FREQUENCY \
    0x2107 ///< Configures the frequency of the stereo pilot. Default is 19000 Hz.
#define SI4713_PROP_TX_ACOMP_ENABLE \
    0x2200 ///< Enables audio dynamic range control. Default is 0 (disabled).
#define SI4713_PROP_TX_ACOMP_THRESHOLD \
    0x2201 ///< Sets the threshold level for audio dynamic range control. Default
///< is –40 dB.
#define SI4713_PROP_TX_ATTACK_TIME \
    0x2202 ///< Sets the attack time for audio dynamic range control. Default is 0
///< (0.5 ms).
#define SI4713_PROP_TX_RELEASE_TIME \
    0x2203 ///< Sets the release time for audio dynamic range control. Default is
///< 4 (1000 ms).
#define SI4713_PROP_TX_ACOMP_GAIN \
    0x2204 ///< Sets the gain for audio dynamic range control. Default is 15 dB.
#define SI4713_PROP_TX_LIMITER_RELEASE_TIME \
    0x2205 ///< Sets the limiter release time. Default is 102 (5.01 ms)
#define SI4713_PROP_TX_ASQ_INTERRUPT_SOURCE \
    0x2300 ///< Configures measurements related to signal quality met- rics.
///< Default is none selected.
#define SI4713_PROP_TX_ASQ_LEVEL_LOW \
    0x2301 ///< Configures low audio input level detection threshold. This
///< threshold can be used to detect silence on the incoming audio.
#define SI4713_PROP_TX_ASQ_DURATION_LOW \
    0x2302 ///< Configures the duration which the input audio level must be below
///< the low threshold in order to detect a low audio condition.
#define SI4713_PROP_TX_AQS_LEVEL_HIGH \
    0x2303 ///< Configures high audio input level detection threshold. This
///< threshold can be used to detect activity on the incoming audio.
#define SI4713_PROP_TX_AQS_DURATION_HIGH \
    0x2304 ///< Configures the duration which the input audio level must be above
///< the high threshold in order to detect a high audio condition.
#define SI4713_PROP_TX_RDS_INTERRUPT_SOURCE \
    0x2C00 ///< Configure RDS interrupt sources. Default is none selected.
#define SI4713_PROP_TX_RDS_PI 0x2C01 ///< Sets transmit RDS program identifier.
#define SI4713_PROP_TX_RDS_PS_MIX 0x2C02 ///< Configures mix of RDS PS Group with RDS Group Buffer.
#define SI4713_PROP_TX_RDS_PS_MISC \
    0x2C03 ///< Miscellaneous bits to transmit along with RDS_PS Groups.
#define SI4713_PROP_TX_RDS_PS_REPEAT_COUNT \
    0x2C04 ///< Number of times to repeat transmission of a PS message before
///< transmitting the next PS mes- sage.
#define SI4713_PROP_TX_RDS_MESSAGE_COUNT 0x2C05 ///< Number of PS messages in use.
#define SI4713_PROP_TX_RDS_PS_AF \
    0x2C06 ///< RDS Program Service Alternate Fre- quency. This provides the
///< ability to inform the receiver of a single alternate frequency
///< using AF Method A coding and is transmitted along with the RDS_PS
///< Groups.
#define SI4713_PROP_TX_RDS_FIFO_SIZE \
    0x2C07 ///< Number of blocks reserved for the FIFO. Note that the value
///< written must be one larger than the desired FIFO size.