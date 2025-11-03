# I2C Troubleshooting Guide for UDAWA-Damodar

## I2C Scanner Function

The `coreroutineScanI2C()` function has been added to automatically scan and detect all I2C devices on the bus during startup.

### When It Runs
- Automatically called in `coreroutineSetup()` right after I2C initialization
- Runs once at device startup before any sensor initialization

### What It Reports

The scanner will log each detected device with its address and potential identification:

```
I2C device found at address 0x48 (ADS1115 or similar ADC)
I2C device found at address 0x36 (MAX17048 Battery Gauge)
I2C device found at address 0x68 (DS3231 RTC or MPU6050)
```

## Common I2C Addresses

| Address | Device | Notes |
|---------|--------|-------|
| 0x48 | ADS1115 | Default address (required for Damodar) |
| 0x49 | ADS1115 | Alternate address (ADDR pin to VDD) |
| 0x4A | ADS1115 | Alternate address (ADDR pin to SDA) |
| 0x4B | ADS1115 | Alternate address (ADDR pin to SCL) |
| 0x36 | MAX17048 | Battery fuel gauge |
| 0x68 | DS3231 | Real-time clock |
| 0x76, 0x77 | BME280/BMP280 | Environmental sensors |
| 0x20-0x27 | PCF8574 | I/O Expander |

## ADS1115 Initialization Failed - Troubleshooting

### Error Message
```
E (6955) coreroutineWaterSensorTaskRoutine: ADS1115 initialization failed!
```

### Diagnostic Steps

#### 1. Check I2C Scanner Output
Look for the scanner output in your logs:
```
I2C device found at address 0x48 (ADS1115 or similar ADC)
```

**If ADS1115 is detected (0x48 appears):**
- Hardware connection is OK
- Issue is with ADS1115 library initialization
- Check power supply voltage to ADS1115
- Verify ADS1115 is getting stable 5V

**If ADS1115 is NOT detected:**
- Hardware connection problem
- Follow hardware troubleshooting below

#### 2. Hardware Checklist

**Power Supply:**
- [ ] ADS1115 VDD connected to 5V rail (your 5.64V DC-DC output)
- [ ] ADS1115 GND connected to common ground with ESP32
- [ ] Voltage at VDD pin measures ~5V (use multimeter)
- [ ] No voltage drops under load

**I2C Connections:**
- [ ] SDA wire connected: ADS1115 SDA → ESP32 GPIO21
- [ ] SCL wire connected: ADS1115 SCL → ESP32 GPIO22
- [ ] No loose connections or broken wires
- [ ] Wires are not too long (keep under 30cm if possible)

**Pull-up Resistors:**
- [ ] 4.7kΩ resistor from SDA to 3.3V (NOT 5V!)
- [ ] 4.7kΩ resistor from SCL to 3.3V (NOT 5V!)
- [ ] If ADS1115 board has onboard pull-ups, check their connection point
- [ ] Total pull-up resistance should be 2-10kΩ (parallel resistors)

**Address Pin (ADDR):**
- [ ] ADDR pin connected to GND for address 0x48 (default)
- [ ] If using different address, modify code accordingly

#### 3. Measure Voltages

Use a multimeter to verify:

| Point | Expected Voltage | What It Means |
|-------|------------------|---------------|
| ADS1115 VDD to GND | ~5.0-5.7V | Power supply OK |
| ADS1115 SDA to GND (idle) | ~3.3V | Pull-up working |
| ADS1115 SCL to GND (idle) | ~3.3V | Pull-up working |
| ADS1115 SDA to GND (active) | Pulsing | I2C communication |
| ADS1115 SCL to GND (active) | Pulsing | I2C clock |

#### 4. Check for Address Conflicts

If scanner shows a different address:
- ADS1115 ADDR pin might be connected incorrectly
- Another device might be using 0x48

**ADS1115 Address Selection:**
```
ADDR Pin → GND:  Address 0x48 (default)
ADDR Pin → VDD:  Address 0x49
ADDR Pin → SDA:  Address 0x4A
ADDR Pin → SCL:  Address 0x4B
```

If your ADS1115 is at a different address, update the code:
```cpp
// In coreroutine.cpp, find:
ADS1115_WE adc = ADS1115_WE(0x48);

// Change to your actual address, e.g.:
ADS1115_WE adc = ADS1115_WE(0x49);
```

#### 5. Test I2C Bus Manually

Add this to your serial monitor to test I2C communication:
```cpp
Wire.beginTransmission(0x48);
byte error = Wire.endTransmission();
if (error == 0) {
  Serial.println("ADS1115 responds at 0x48");
} else {
  Serial.printf("Error %d when trying to contact 0x48\n", error);
}
```

Error codes:
- 0: Success (device acknowledged)
- 1: Data too long for buffer
- 2: NACK on address (device not present)
- 3: NACK on data
- 4: Other error

#### 6. Common Issues and Solutions

**Issue: No devices found at all**
- Check I2C pins are correct (GPIO21=SDA, GPIO22=SCL)
- Verify ESP32 is using correct GPIO mapping
- Check for shorts between SDA and SCL
- Try slower I2C speed: `Wire.setClock(100000);` (100kHz instead of 400kHz)

**Issue: Wrong address detected**
- Check ADDR pin connection on ADS1115
- Update code to match actual address

**Issue: Multiple unknown devices**
- You may have additional I2C devices on the bus
- Check for IO expanders, sensors, or other modules

**Issue: Intermittent detection**
- Loose connections or bad solder joints
- Electromagnetic interference from other wires
- Power supply noise or instability
- Try adding 100nF capacitor across ADS1115 VDD-GND

**Issue: Device detected but init fails**
- ADS1115 may be damaged
- Wrong library version
- Power supply voltage too low or unstable
- Try power cycling the ADS1115

#### 7. Advanced Debugging

**Enable I2C debugging:**
Add to your setup before Wire.begin():
```cpp
#define I2C_DEBUG
Wire.setDebugLevel(3);
```

**Try different I2C speed:**
```cpp
Wire.setClock(100000);  // Slower, more reliable
// or
Wire.setClock(400000);  // Faster (default)
```

**Check for stuck I2C bus:**
Sometimes the I2C bus can get stuck. Try this reset:
```cpp
pinMode(21, OUTPUT); // SDA
pinMode(22, OUTPUT); // SCL
digitalWrite(21, HIGH);
digitalWrite(22, HIGH);
delay(10);
// Then call Wire.begin() again
```

## Expected Damodar I2C Devices

For proper operation, you should see:

1. **ADS1115** at 0x48 (required for TDS sensor)
2. **MAX17048** at 0x36 (optional, battery gauge)
3. **DS3231** at 0x68 (optional, hardware RTC)

Minimum required:
- ADS1115 at 0x48

## Serial Monitor Output Examples

### Good Output (All Devices Found)
```
I: coreroutineScanI2C: Scanning I2C bus...
I: coreroutineScanI2C: I2C device found at address 0x36 (MAX17048 Battery Gauge)
I: coreroutineScanI2C: I2C device found at address 0x48 (ADS1115 or similar ADC)
I: coreroutineScanI2C: I2C device found at address 0x68 (DS3231 RTC or MPU6050)
I: coreroutineScanI2C: I2C scan complete. Found 3 device(s)
```

### Problem: No Devices Found
```
I: coreroutineScanI2C: Scanning I2C bus...
W: coreroutineScanI2C: No I2C devices found!
W: coreroutineScanI2C: Check connections:
W: coreroutineScanI2C:   - SDA (GPIO21) connected?
W: coreroutineScanI2C:   - SCL (GPIO22) connected?
W: coreroutineScanI2C:   - Pull-up resistors (4.7kΩ to 3.3V)?
W: coreroutineScanI2C:   - Device power supply (VDD/GND)?
```

### Problem: Wrong Address
```
I: coreroutineScanI2C: Scanning I2C bus...
I: coreroutineScanI2C: I2C device found at address 0x49 (ADS1115 alternate address)
I: coreroutineScanI2C: I2C scan complete. Found 1 device(s)
E: coreroutineWaterSensorTaskRoutine: ADS1115 initialization failed!
```
**Solution**: Change ADS1115 address in code from 0x48 to 0x49

## Quick Fix Checklist

When you see "ADS1115 initialization failed":

1. ✅ Run I2C scanner (it runs automatically now)
2. ✅ Check if 0x48 appears in scan results
3. ✅ If no devices: Check wiring and power
4. ✅ If wrong address: Update code or fix ADDR pin
5. ✅ If address OK but init fails: Check 5V power supply
6. ✅ Measure voltages with multimeter
7. ✅ Try slower I2C speed (100kHz)
8. ✅ Check for loose connections
9. ✅ Power cycle everything
10. ✅ Replace ADS1115 if all else fails

## Useful Commands

### Enable Verbose Logging
In `params.h`, set:
```cpp
static const uint8_t logLev = 5; // VERBOSE
```

### Manual I2C Scan (if needed)
You can call the scanner manually from anywhere:
```cpp
#ifdef USE_I2C
coreroutineScanI2C();
#endif
```

## Getting Help

When reporting I2C issues, include:
1. Complete I2C scanner output
2. Measured voltages (VDD, SDA, SCL)
3. Photo of wiring connections
4. ADS1115 board model/manufacturer
5. Length of I2C wires
6. Any other I2C devices on the same bus
