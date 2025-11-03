# TDS Sensor Calibration Guide for UDAWA-Damodar

## Overview
This document provides guidance on calibrating the analog TDS (Total Dissolved Solids) sensor connected to the ADS1115 ADC on pin A0.

## Hardware Setup
- **TDS Sensor**: Analog TDS sensor connected to ADS1115 channel A0
- **Temperature Sensor**: DS18B20 waterproof sensor on GPIO2
- **ADS1115 Power**: 5V rail (actual: 5.64V from battery charger DC-DC step-up)
- **ADS1115 Communication**: I2C bus connected to ESP32
- **ADC Range**: Set to ±4096 mV for optimal TDS sensor range
- **Conversion Rate**: 128 SPS (samples per second)

### Important Power Supply Notes
The ADS1115 is powered from a 5V source (actual voltage: 5.64V from battery charger DC-DC step-up converter). While the I2C communication uses ESP32's 3.3V logic levels (which is compatible with ADS1115), the analog measurements and TDS sensor are referenced to the 5V power rail. The TDS sensor output voltage will swing relative to this 5V supply.

## Wiring Diagram

### ADS1115 Connections
```
ADS1115          ESP32
--------         -----
VDD    -------> 5V (5.64V from DC-DC converter)
GND    -------> GND
SCL    -------> SCL (GPIO22, with 4.7kΩ pull-up to 3.3V)
SDA    -------> SDA (GPIO21, with 4.7kΩ pull-up to 3.3V)
A0     -------> TDS Sensor Signal
```

### TDS Sensor Connections
```
TDS Sensor       Connection
----------       ----------
VCC (Red)   ---> 5V (same rail as ADS1115)
GND (Black) ---> GND
Signal      ---> ADS1115 A0 pin
```

### DS18B20 Connections
```
DS18B20          ESP32
-------          -----
VCC (Red)   ---> 3.3V or 5V
GND (Black) ---> GND
Data (Yellow)-> GPIO2 (with 4.7kΩ pull-up to 3.3V)
```

**Important Notes:**
- I2C pull-up resistors should connect to **3.3V**, not 5V
- DS18B20 pull-up resistor connects to **3.3V**
- TDS sensor and ADS1115 share the same **5V power rail**
- All devices share common **GND**

## TDS Sensor Calibration

### TDS Factor
The current `TDS_FACTOR` is set to `0.5` in the code, and `VREF` is set to `5.0` to match your 5V power supply. This value may need adjustment based on your specific sensor:

```cpp
const float VREF = 5.0;        // Reference voltage (TDS sensor powered from 5V rail)
const float TDS_FACTOR = 0.5;  // Adjust this value based on calibration
```

**Note**: If your actual supply voltage is significantly different from 5.0V (e.g., 5.64V as measured), you may need to adjust `VREF` to `5.64` for more accurate readings. However, start with `5.0` and calibrate using standard solutions to determine the optimal `TDS_FACTOR` first.

### Calibration Steps

1. **Prepare Calibration Solutions**
   - 0 ppm (distilled water)
   - 342 ppm (standard calibration solution)
   - 1413 ppm (optional, for wider range)

2. **Perform Calibration**
   - Rinse the probe with distilled water
   - Immerse probe in calibration solution
   - Wait for reading to stabilize (30-60 seconds)
   - Record the measured TDS value
   - Calculate adjustment factor:
     ```
     New TDS_FACTOR = (Expected_TDS / Measured_TDS) * Current_TDS_FACTOR
     ```

3. **Temperature Compensation**
   The code automatically compensates for temperature using:
   ```cpp
   float compensationCoefficient = 1.0 + 0.02 * (temp - 25.0);
   ```
   This assumes a 2% change per degree Celsius from 25°C reference.

## TDS to EC Conversion

The code converts TDS to EC (Electrical Conductivity) using:
```cpp
ec = tds * 2.0;  // EC in µS/cm
```

This is a simplified conversion. For more accurate results, use:
- **For NaCl solution**: EC (µS/cm) ≈ TDS (ppm) × 2.0
- **For KCl solution**: EC (µS/cm) ≈ TDS (ppm) × 1.9
- **For 442 solution**: EC (µS/cm) ≈ TDS (ppm) × 1.4

## Measurement Ranges

### TDS (Total Dissolved Solids)
- **Valid Range**: 0 - 5000 ppm
- **Typical Water Quality**:
  - 0-50 ppm: Distilled/Reverse Osmosis water
  - 50-150 ppm: Soft water
  - 150-500 ppm: Acceptable for drinking
  - 500-1000 ppm: Poor quality
  - 1000+ ppm: Not suitable for drinking

### EC (Electrical Conductivity)
- **Valid Range**: 0 - 10000 µS/cm
- **Alarm Threshold**: > 4000 µS/cm

### Temperature
- **Sensor Range**: -55°C to 125°C (DS18B20 spec)
- **Practical Water Range**: 0°C to 50°C
- **Alarm Thresholds**: Configurable via `appConfig.tempSafeHigh` and `appConfig.tempSafeLow`

## Polynomial TDS Calculation

The code uses a third-order polynomial for voltage-to-TDS conversion:
```cpp
tds = (133.42 * V³ - 255.86 * V² + 857.39 * V) * TDS_FACTOR
```

Where V is the temperature-compensated voltage. This polynomial is based on typical TDS sensor characteristics.

## Data Smoothing

The code implements a moving average filter with 30 samples (SCOUNT):
```cpp
const int SCOUNT = 30;  // Number of samples for averaging
```

This helps reduce noise in the readings. You can adjust this value:
- **Lower values** (10-20): Faster response, more noise
- **Higher values** (30-50): Slower response, smoother readings

## Monitoring and Alarms

### Sensor Status Flags
- `appState.fAdc`: TDS sensor status
- `appState.fDs18B20`: Temperature sensor status

### Alarm Codes
- **ALARM_WEATHER_SENSOR_INIT_FAIL** (120): DS18B20 initialization failed
- **ALARM_AC_SENSOR_INIT_FAIL** (140): TDS sensor initialization failed
- **ALARM_WEATHER_TEMP_OUT_OF_RANGE** (121): Temperature out of range (0-50°C)
- **ALARM_AC_CURRENT_OUT_OF_RANGE** (142): TDS out of range (0-2000 ppm)
- **ALARM_AC_VOLTAGE_OUT_OF_RANGE** (141): EC out of range (0-4000 µS/cm)

## Data Output

### Web Interface
Broadcasts real-time data every `appConfig.intvWeb` seconds:
```json
{
  "tempSensor": {"temp": 25.4},
  "tdsSensor": {"tds": 345.2, "ec": 690.4}
}
```

### IoT Attributes
Sends current values every `appConfig.intvAttr` seconds:
```json
{
  "_temp": 25.4,
  "_tds": 345.2,
  "_ec": 690.4
}
```

### IoT Telemetry
Sends aggregated statistics every `appConfig.intvTele` seconds:
```json
{
  "temp_avg": 25.3, "temp_min": 24.8, "temp_max": 25.9,
  "tds_avg": 342.1, "tds_min": 335.0, "tds_max": 350.0,
  "ec_avg": 684.2, "ec_min": 670.0, "ec_max": 700.0
}
```

## Troubleshooting

### Issue: TDS readings are too high/low
**Solution**: 
1. First, calibrate with standard solutions and adjust `TDS_FACTOR`
2. If readings are still off, measure your actual 5V rail voltage (you mentioned 5.64V)
3. Update `VREF` to match your measured voltage (e.g., `const float VREF = 5.64;`)
4. Re-calibrate `TDS_FACTOR` with the new `VREF` value

### Issue: Readings fluctuate wildly
**Solution**: 
- Increase `SCOUNT` for more smoothing
- Check sensor probe for debris/buildup
- Ensure proper grounding (especially important with 5V-powered sensor and 3.3V I2C logic levels)
- Verify I2C pull-up resistors are appropriate (typically 4.7kΩ for 3.3V logic)

### Issue: Temperature compensation not working
**Solution**: Verify DS18B20 is reading correctly before TDS measurement begins.

### Issue: Sensor initialization fails
**Solution**: 
- Check I2C connections for ADS1115 (SDA/SCL to ESP32)
- Verify ADS1115 has proper power (5V supply and GND)
- Verify DS18B20 is on GPIO2 with 4.7kΩ pull-up resistor to 3.3V
- Check I2C address (default 0x48 for ADS1115)
- Use I2C scanner to verify ADS1115 is detected on the bus

### Issue: I2C communication errors
**Solution**:
- The ADS1115 is 5V tolerant on I2C pins, but ESP32 uses 3.3V logic
- Ensure I2C pull-up resistors connect to 3.3V rail, NOT 5V
- If you have pull-ups to 5V, add level shifters or move pull-ups to 3.3V
- Typical I2C pull-up values: 4.7kΩ (for short distances) or 10kΩ (for longer traces)

## Advanced Configuration

### Voltage Reference Adjustment

Your system uses a 5V power supply from a battery charger DC-DC step-up converter (measured at 5.64V). For optimal accuracy:

1. **Measure Actual Voltage**: Use a multimeter to measure the actual 5V rail voltage
2. **Update VREF**: If consistently different from 5.0V, update the constant:
   ```cpp
   const float VREF = 5.64;  // Your measured voltage
   ```
3. **Re-calibrate**: After changing VREF, recalibrate TDS_FACTOR with standard solutions

### Voltage-to-TDS Polynomial

To modify the voltage-to-TDS polynomial, update these coefficients in the code:
```cpp
tds = (133.42 * V³ - 255.86 * V² + 857.39 * V) * TDS_FACTOR;
```

These values are based on typical TDS sensor characteristics with 5V supply. For custom calibration:
1. Collect voltage readings at known TDS values
2. Perform polynomial regression (order 3)
3. Update the coefficients in the code

### I2C Level Shifting Considerations

Since ADS1115 runs on 5V but ESP32 uses 3.3V logic:
- **Best Practice**: Pull-up resistors on SDA/SCL should connect to 3.3V (ESP32 logic level)
- **ADS1115 Compatibility**: The chip is 5V tolerant but can work with 3.3V I2C signals
- **If Issues Arise**: Consider bidirectional level shifters if you experience communication problems
- **Current Setup**: Should work fine as ADS1115 accepts 3.3V logic levels on I2C pins

## Maintenance

- **Weekly**: Clean sensor probe with distilled water
- **Monthly**: Calibrate with standard solution
- **Quarterly**: Replace sensor probe if accuracy degrades
- **As needed**: Update `TDS_FACTOR` after calibration
