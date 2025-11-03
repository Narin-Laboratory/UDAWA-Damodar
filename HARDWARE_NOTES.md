# UDAWA-Damodar Hardware Configuration Notes

## Power Supply Configuration

### Main Power Rails
- **5V Rail**: 5.64V actual (from battery charger DC-DC step-up converter)
  - Powers: ADS1115, TDS Sensor
  - Note: Voltage not adjustable to exactly 5.0V
- **3.3V Rail**: From ESP32 regulator
  - Powers: ESP32, DS18B20, I2C pull-ups

## I2C Bus Configuration

### Devices on I2C Bus
1. **ADS1115** (Address: 0x48)
   - 16-bit ADC for TDS sensor
   - Powered from 5V rail
   - I2C logic compatible with 3.3V

2. **Other I2C Devices** (if applicable)
   - Check for address conflicts

### I2C Pull-up Resistors
- **Value**: 4.7kΩ recommended
- **Connection**: Must connect to 3.3V rail (NOT 5V)
- **Pins**: SDA (GPIO21), SCL (GPIO22)

## Sensor Configuration

### TDS Sensor (Analog)
- **Connection**: ADS1115 Channel A0
- **Power**: 5V rail (5.64V actual)
- **Type**: Analog output (voltage proportional to TDS)
- **Range**: Typically 0-2.3V for 0-1000 ppm TDS

### DS18B20 Temperature Sensor
- **Connection**: GPIO2
- **Power**: 3.3V or 5V (3.3V recommended)
- **Pull-up**: 4.7kΩ to 3.3V on data line
- **Protocol**: 1-Wire (Dallas/Maxim)
- **Type**: Waterproof probe

## ADC Configuration

### ADS1115 Settings
```cpp
// In coreroutine.cpp
adc.setVoltageRange_mV(ADS1115_RANGE_4096);  // ±4096 mV range
adc.setCompareChannels(ADS1115_COMP_0_GND);  // Single-ended A0
adc.setConvRate(ADS1115_128_SPS);            // 128 samples/second
adc.setMeasureMode(ADS1115_CONTINUOUS);      // Continuous conversion
```

## Calibration Constants

### Current Values (in coreroutine.cpp)
```cpp
const float VREF = 5.0;         // Reference voltage (5V rail)
const float TDS_FACTOR = 0.5;   // TDS conversion factor
const int SCOUNT = 30;          // Averaging samples
```

### Adjustment Notes
- **VREF**: Set to 5.0V (your actual is 5.64V)
  - Can adjust to 5.64 if needed after testing
  - Better to calibrate TDS_FACTOR with VREF=5.0 first
- **TDS_FACTOR**: Calibrate with standard solutions (342 ppm or 1413 ppm)
- **SCOUNT**: Increase for smoother readings, decrease for faster response

## Pin Assignments

| Device/Function | GPIO Pin | Notes |
|----------------|----------|-------|
| I2C SDA | GPIO21 | Standard ESP32 I2C |
| I2C SCL | GPIO22 | Standard ESP32 I2C |
| DS18B20 Data | GPIO2 | 1-Wire protocol |
| LED Red | GPIO27 | Status indicator |
| LED Green | GPIO14 | Status indicator |
| LED Blue | GPIO12 | Status indicator |
| Buzzer | GPIO32 | Alarm output |

## Important Considerations

### Mixed Voltage Levels
✅ **Safe Configuration**:
- ADS1115 VDD: 5V (5.64V actual)
- I2C pull-ups: 3.3V
- ADS1115 I2C pins accept 3.3V logic levels

❌ **Avoid**:
- Connecting I2C pull-ups to 5V rail
- Directly connecting 5V signals to ESP32 GPIO pins

### Grounding
- **Critical**: All devices must share common ground
- **Battery System**: Ensure DC-DC converter ground is tied to ESP32 ground

### Power Sequencing
1. 5V rail powers up (from battery charger DC-DC)
2. ESP32 3.3V regulator powers up
3. I2C devices initialize
4. Sensors begin reading

## Troubleshooting Quick Checks

### ADS1115 Not Detected
- [ ] Check 5V power supply (should be 5-5.7V)
- [ ] Verify I2C wiring (SDA to GPIO21, SCL to GPIO22)
- [ ] Check I2C pull-ups are to 3.3V
- [ ] Run I2C scanner code to verify address 0x48

### TDS Readings Invalid
- [ ] Verify TDS sensor is powered from 5V rail
- [ ] Check sensor probe is clean and in water
- [ ] Confirm ADS1115 A0 connection to sensor signal wire
- [ ] Calibrate with standard solution

### DS18B20 Not Reading
- [ ] Verify 4.7kΩ pull-up resistor on GPIO2 to 3.3V
- [ ] Check waterproof probe connections (may need re-soldering)
- [ ] Test with `ds18b20.getDeviceCount()` - should return 1 or more

### Temperature Compensation Issues
- [ ] DS18B20 must initialize before TDS readings
- [ ] Check temperature reading is reasonable (10-40°C typical)
- [ ] Verify temperature is being used in TDS calculation

## Voltage Measurement Tips

To measure actual supply voltages:
1. **5V Rail**: Measure between ADS1115 VDD and GND pins
2. **3.3V Rail**: Measure between ESP32 3V3 and GND pins
3. **TDS Sensor Output**: Measure ADS1115 A0 to GND (in water)

Expected values:
- 5V rail: ~5.64V (your system)
- 3.3V rail: 3.2-3.4V (typical)
- TDS sensor: 0-2.5V depending on water quality

## Next Steps for Optimization

1. **Initial Testing**:
   - Upload code and verify sensor initialization
   - Check raw voltage readings from ADS1115
   - Confirm DS18B20 temperature readings

2. **Calibration**:
   - Use 342 ppm standard solution
   - Adjust TDS_FACTOR for accurate readings
   - Document calibration factor in code comments

3. **Fine-tuning** (if needed):
   - Adjust VREF to 5.64 if readings consistently off
   - Increase SCOUNT if readings too noisy
   - Adjust temperature compensation coefficient

4. **Validation**:
   - Test with multiple standard solutions (0, 342, 1413 ppm)
   - Verify temperature compensation works across range
   - Monitor long-term stability
