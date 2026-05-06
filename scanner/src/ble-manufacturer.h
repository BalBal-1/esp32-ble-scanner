#pragma once
#include <Arduino.h>

#define BLE_MANUFACTURER_ID_UNKNOWN 0xffff

void BLEManufacturerSetup(void);
const char *BLEManufacturerLookup(const uint16_t id, const char *none);
const char *BLEManufacturerIdHex(const uint16_t id);
