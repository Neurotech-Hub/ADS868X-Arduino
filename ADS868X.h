#pragma once

#include <SPI.h> // Include the SPI library for Arduino

namespace ads868x
{

    enum struct Registers : uint8_t
    {
        NO_OP_REG = 0x00,           // No operation Register
        DEVICE_ID_REG_23_16 = 0x02, // Device ID
        RST_PWCTRL_REG_7_0 = 0x04,  // Reset and power control
        RST_PWCTRL_REG_15_8 = 0x05,
        SDI_CTL_REG_7_0 = 0x08, // SDI data input control
        SDO_CTL_REG_7_0 = 0x0C, // SDO-x data input control
        SDO_CTL_REG_15_8 = 0x0D,
        DATAOUT_CTL_REG_7_0 = 0x10, // Output data control
        DATAOUT_CTL_REG_15_8 = 0x11,
        RANGE_SEL_REG_7_0 = 0x14, // Input range selection control
        ALARM_REG_7_0 = 0x20,     // ALARM output register
        ALARM_REG_15_8 = 0x21,
        ALARM_H_TH_REG_7_0 = 0x24, // ALARM high threshold and hysteresis register
        ALARM_H_TH_REG_15_8 = 0x25,
        ALARM_H_TH_REG_31_24 = 0x27,
        ALARM_L_TH_REG_7_0 = 0x28, // ALARM low threshold register
        ALARM_L_TH_REG_15_8 = 0x29
    };

    enum struct Commands : uint8_t
    {
        NOP = 0x00,          // Send to read current 16-bit value
        CLEAR_HWORD = 0xC0,  // Any bits marked 1 in data will be set to 0 in register at those positions
        READ_HWORD = 0xC8,   // Read 16 bits from register
        READ = 0x48,         // Read 8 bits from register
        WRITE = 0xD0,        // Write 16 bits to register
        WRITE_MSBYTE = 0xD2, // Write 8 bits to register (MS Byte of data)
        WRITE_LSBYTE = 0xD4, // Write 8 bits to register (LS Byte of data)
        SET_HWORD = 0xD8     // Any bits marked 1 in data will be set to 1 in register at those positions
    };

    enum References
    {
        External = 0x0080,
        Internal = 0x0000,
    };

    enum Ranges
    {
        pm3Vref = 0x0000,    // ±3*Vref
        pm25Vref = 0x0001,   // ±2.5*Vref
        pm15Vref = 0x0002,   // ±2.5*Vref
        pm125Vref = 0x0003,  // ±1.25*Vref
        pm0625Vref = 0x0004, // ±0.625*Vref
        p3Vref = 0x0008,     // 3*Vref
        p25Vref = 0x0009,    // 2.5*Vref
        p15Vref = 0x000A,    // 1.5*Vref
        p125Vref = 0x000B    // 1.25*Vref
    };

    const float internalReferenceVoltage = 4.096; // Internal reference value

    class ADS868X
    {
    public:
        // Constructor: Initializes with Chip Select pin, Range, Reference, and Reference Voltage
        ADS868X(uint8_t cs_pin, Ranges range, References reference, double referenceVoltage);

        // Send a generic command
        uint16_t sendCommand(Commands command, Registers address, uint16_t data);

        // Read plain value from ADC
        uint16_t readPlainADC();

        // Read ADC value and convert to voltage
        double readADC();

    private:
        uint8_t _cs_pin;
        References _reference;
        Ranges _range;
        double _referenceVoltage;

        // Select the ADC by pulling CS low
        void selectADC();

        // Deselect the ADC by pulling CS high
        void deselectADC();
    };
}
