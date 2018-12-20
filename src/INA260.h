/*
 * Copyright 2018 Matthew Brush <mbrush@codebrainz.ca>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef INA260_H
#define INA260_H

#include <stdint.h>

class INA260 {
public:

  enum RegisterAddress {
    CONFIGURATION_REGISTER = 0x00,
    CURRENT_REGISTER = 0x01,
    BUS_VOLTAGE_REGISTER = 0x02,
    POWER_REGISTER = 0x03,
    MASK_ENABLE_REGISTER = 0x06,
    ALERT_LIMIT_REGISTER = 0x07,
    MANUFACTURER_ID_REGISTER = 0xFE,
    DIE_ID_REGISTER = 0xFF,
  };

  enum AveragingMode {
    AVG_1 = 0b000,
    AVG_4 = 0b001,
    AVG_16 = 0b010,
    AVG_64 = 0b011,
    AVG_128 = 0b100,
    AVG_256 = 0b101,
    AVG_512 = 0b110,
    AVG_1024 = 0b111,
  };

  enum VoltageConversionTime {
    VBUSCT_140US = 0b000,
    VBUSCT_204US = 0b001,
    VBUSCT_332US = 0b010,
    VBUSCT_588US = 0b011,
    VBUSCT_1_1MS = 0b100,
    VBUSCT_2_116MS = 0b101,
    VBUSCT_4_156MS = 0b110,
    VBUSCT_8_244MS = 0b0111,
  };

  enum CurrentConversionTime {
    ISHCT_140US = 0b000,
    ISHCT_204US = 0b001,
    ISHCT_332US = 0b010,
    ISHCT_588US = 0b011,
    ISHCT_1_1MS = 0b100,
    ISHCT_2_116MS = 0b101,
    ISHCT_4_156MS = 0b110,
    ISHCT_8_244MS = 0b111,
  };

  enum OperatingMode {
    MODE_POWER_DOWN1 = 0b000,
    MODE_ISH_TRIGGERED = 0b0001,
    MODE_VBUS_TRIGGERED = 0b010,
    MODE_ISH_VBUS_TRIGGERED = 0b011,
    MODE_POWER_DOWN2 = 0b0100,
    MODE_ISH_CONTINUOUS = 0b101,
    MODE_VBUS_CONTINUOUS = 0b110,
    MODE_ISH_VBUS_CONTINUOUS = 0b111,
  };

  enum AddressPin {
    ADDR_GND = 0b00,
    ADDR_VS = 0b01,
    ADDR_SDA = 0b10,
    ADDR_SCL = 0b11,
  };

  union ConfigurationRegister {
    struct __attribute__((packed)) {
      uint16_t mode : 3;
      uint16_t ishct : 3;
      uint16_t vbusct : 3;
      uint16_t avg : 3;
      uint16_t : 3;
      uint16_t rst : 1;
    };
    uint16_t rawValue;
  };

  union MaskEnableRegister {
    struct __attribute__((packed)) {
      uint16_t len : 1;
      uint16_t apol : 1;
      uint16_t ovf : 1;
      uint16_t cvrf : 1;
      uint16_t aff : 1;
      uint16_t : 5;
      uint16_t cnvr : 1;
      uint16_t pol : 1;
      uint16_t bul : 1;
      uint16_t bol : 1;
      uint16_t ucl : 1;
      uint16_t ocl : 1;
    };
    uint16_t rawValue;
  };

  union DieIdRegister {
    struct __attribute__((packed)) {
      uint16_t rid : 4;
      uint16_t did : 12;
    };
    uint16_t rawValue;
  };

  INA260(uint8_t addr = 0)
    : addr(0b1000000 | (addr & 0b1111)) {}

  INA260(AddressPin a0, AddressPin a1)
    : INA260((static_cast<uint8_t>(a1) << 2) | static_cast<uint8_t>(a0)) {}

  void begin();

  bool readRegister(uint8_t reg, uint16_t &value) const;
  bool writeRegister(uint8_t reg, uint16_t value) const;

  bool readConfigurationRegister(ConfigurationRegister &reg) const {
    return readRegister(CONFIGURATION_REGISTER, reg.rawValue);
  }

  bool writeConfigurationRegister(const ConfigurationRegister &reg) const {
    return writeRegister(CONFIGURATION_REGISTER, reg.rawValue);
  }

  bool readCurrentRegister(uint16_t &value) const {
    return readRegister(CURRENT_REGISTER, value);
  }

  bool readCurrentRegisterInAmps(double &value) const {
    uint16_t rawValue = 0;
    if (readCurrentRegister(rawValue)) {
      value = rawToAmps(static_cast<int16_t>(rawValue));
      return true;
    } else {
      return false;
    }
  }

  bool readBusVoltageRegister(uint16_t &value) const {
    return readRegister(BUS_VOLTAGE_REGISTER, value);
  }

  bool readBusVoltageRegisterInVolts(double &value) const {
    uint16_t rawValue = 0;
    if (readBusVoltageRegister(rawValue)) {
      value = rawToVolts(rawValue);
      return true;
    } else {
      return false;
    }
  }

  bool readPowerRegister(uint16_t &value) const {
    return readRegister(POWER_REGISTER, value);
  }

  bool readPowerRegisterInWatts(double &value) const {
    uint16_t rawValue = 0;
    if (readPowerRegister(rawValue)) {
      value = rawToWatts(rawValue);
      return true;
    } else {
      return false;
    }
  }

  bool readMaskEnableRegister(MaskEnableRegister &reg) const {
    return readRegister(MASK_ENABLE_REGISTER, reg.rawValue);
  }

  bool writeMaskEnableRegister(const MaskEnableRegister &reg) const {
    return writeRegister(MASK_ENABLE_REGISTER, reg.rawValue);
  }

  bool readAlertLimitRegister(uint16_t &value) const {
    return readRegister(ALERT_LIMIT_REGISTER, value);
  }

  bool readAlertLimitRegisterInAmps(double &value) const {
    uint16_t val = 0;
    if (readAlertLimitRegister(val)) {
      value = rawToAmps(static_cast<int16_t>(val));
      return true;
    } else {
      return false;
    }
  }

  bool readAlertLimitRegisterInVolts(double &value) const {
    uint16_t val = 0;
    if (readAlertLimitRegister(val)) {
      value = rawToVolts(val);
      return true;
    } else {
      return false;
    }
  }

  bool readAlertLimitRegisterInWatts(double &value) const {
    uint16_t val = 0;
    if (readAlertLimitRegister(val)) {
      value = rawToWatts(val);
      return true;
    } else {
      return false;
    }
  }

  bool writeAlertLimitRegister(uint16_t value) const {
    return writeRegister(ALERT_LIMIT_REGISTER, value);
  }

  bool writeAlertLimitRegisterInAmps(double value) const {
    return writeAlertLimitRegister(ampsToRaw(value));
  }

  bool writeAlertLimitRegisterInVolts(double value) const {
    return writeAlertLimitRegister(voltsToRaw(value));
  }

  bool writeAlertLimitRegisterInWatts(double value) const {
    return writeAlertLimitRegister(wattsToRaw(value));
  }

  bool readManufacturerIdRegister(char mfgStr[3]) const {
    uint16_t value = 0;
    if (readRegister(MANUFACTURER_ID_REGISTER, value)) {
      mfgStr[0] = static_cast<char>((value >> 8) & 0xFF);
      mfgStr[1] = static_cast<char>(value & 0xFF);
      mfgStr[2] = '\0';
      return true;
    } else {
      return false;
    }
  }

  bool readDieIdRegister(DieIdRegister &reg) const {
    return readRegister(DIE_ID_REGISTER, reg.rawValue);
  }

  bool reset() const {
    ConfigurationRegister reg{};
    reg.rst = 1;
    return writeConfigurationRegister(reg);
  }

  bool clearAlert() const {
    MaskEnableRegister reg{};
    return readMaskEnableRegister(reg);
  }

  static double rawToAmps(int16_t value) {
    return (value * 1.25) / 1000.0;
  }

  static int16_t ampsToRaw(double value) {
    return (value * 1000.0) / 1.25;
  }

  static double rawToVolts(uint16_t value) {
    return (value * 1.25) / 1000.0;
  }

  static uint16_t voltsToRaw(double value) {
    return (value * 1000.0) / 1.25;
  }

  static double rawToWatts(uint16_t value) {
    return (value * 10.0) / 1000.0;
  }

  static uint16_t wattsToRaw(double value) {
    return (value * 1000.0) / 10.0;
  }

private:
  uint8_t addr;
};

#endif // INA260_H
