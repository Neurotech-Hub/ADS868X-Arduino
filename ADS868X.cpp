#include "ADS868X.h" // Include the renamed header file

using namespace ads868x;

// Main constructor
ADS868X::ADS868X(uint8_t cs_pin, Ranges range, References reference, double referenceVoltage)
    : _cs_pin(cs_pin), range(range), reference(reference), referenceVoltage(referenceVoltage)
{

    // Check for invalid internal reference voltage
    if (reference == References::Internal && referenceVoltage != internalReferenceVoltage)
    {
        // Handle the error (you can choose to return or set a flag)
        // For Arduino, typically using Serial or a return code for error handling
        Serial.println("Invalid Internal Reference Voltage");
        return;
    }

    // Initialize SPI
    SPI.begin();

    // Set the Chip Select pin as output and deselect the ADC initially
    pinMode(_cs_pin, OUTPUT);
    deselectADC();

    // Set SPI settings
    SPI.beginTransaction(SPISettings(16000000, MSBFIRST, SPI_MODE0));

    // Set the ADS to the requested range and reference
    uint16_t command = range | reference;
    uint16_t got = 0;

    do
    {
        sendCommand(Commands::WRITE, Registers::RANGE_SEL_REG_7_0, command);
        delay(10); // Delay to ensure the command is processed
        sendCommand(Commands::READ_HWORD, Registers::RANGE_SEL_REG_7_0, 0x0000);
        got = sendCommand(Commands::NOP, Registers::NO_OP_REG, 0x0000);
        delay(10);
    } while (got != command);

    sendCommand(Commands::NOP, Registers::NO_OP_REG, 0x0000);
}

// Send a command to the ADC
uint16_t ADS868X::sendCommand(Commands command, Registers address, uint16_t data)
{
    selectADC();

    // Prepare the command buffer
    uint8_t buff[4] = {0};
    buff[0] = static_cast<uint8_t>(command);
    buff[1] = static_cast<uint8_t>(address);
    buff[2] = (data >> 8) & 0xFF;
    buff[3] = data & 0xFF;

    // Transfer the data over SPI
    SPI.transfer(buff, sizeof(buff));

    deselectADC();

    // Return the received data
    return ((buff[0] << 8) | buff[1]);
}

// Read plain ADC value (16-bit)
uint16_t ADS868X::readPlainADC()
{
    return sendCommand(Commands::NOP, Registers::NO_OP_REG, 0x0000);
}

// Read ADC value and convert it to voltage
double ADS868X::readADC()
{
    double val = static_cast<double>(readPlainADC());
    double scalefactor = 0.0;

    // Calculate scale factor based on range
    switch (this->range)
    {
    case pm3Vref:
    case p3Vref:
        scalefactor = referenceVoltage * 3 / 65535.0;
        break;
    case pm25Vref:
    case p25Vref:
        scalefactor = referenceVoltage * 2.5 / 65535.0;
        break;
    case pm15Vref:
    case p15Vref:
        scalefactor = referenceVoltage * 1.5 / 65535.0;
        break;
    case pm125Vref:
    case p125Vref:
        scalefactor = referenceVoltage * 1.25 / 65535.0;
        break;
    case pm0625Vref:
        scalefactor = referenceVoltage * 0.625 / 65535.0;
        break;
    default:
        Serial.println("Invalid Range");
        return 0.0;
    }

    // Adjust for bipolar ranges
    switch (range)
    {
    case pm3Vref:
    case pm25Vref:
    case pm15Vref:
    case pm125Vref:
    case pm0625Vref:
        val -= 32768; // Center at 0 for bipolar ranges
        scalefactor *= 2;
        break;
    case p3Vref:
    case p25Vref:
    case p15Vref:
    case p125Vref:
        // Unipolar, no additional scaling needed
        break;
    default:
        Serial.println("Invalid Range");
        return 0.0;
    }

    // Return the scaled value
    return val * scalefactor;
}

// Select the ADC by pulling CS low
void ADS868X::selectADC()
{
    digitalWrite(_cs_pin, LOW);
}

// Deselect the ADC by pulling CS high
void ADS868X::deselectADC()
{
    digitalWrite(_cs_pin, HIGH);
}
