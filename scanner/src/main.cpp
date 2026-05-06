#include <Arduino.h>
#include <math.h>
#include <NimBLEDevice.h>
#include "ble-manufacturer.h"

float estimateDistanceMeters(int rssi, int txPowerAtOneMeter = -59) {
  if (rssi == 0) return -1.0;

  float ratio = (txPowerAtOneMeter - rssi) / 20.0;
  return pow(10.0, ratio);
}

int getAdvertisedBatteryPercent(const NimBLEAdvertisedDevice* device) {
  // Usually unavailable unless the device advertises battery service data.
  // Battery Service UUID = 0x180F, Battery Level Characteristic = 0x2A19.
  if (!device->haveServiceData()) {
    return -1;
  }

  std::string serviceData = device->getServiceData();

  if (serviceData.length() < 1) {
    return -1;
  }

  int battery = (uint8_t)serviceData[0];

  if (battery >= 0 && battery <= 100) {
    return battery;
  }

  return -1;
}

class ScanCallbacks : public NimBLEScanCallbacks {
  void onResult(const NimBLEAdvertisedDevice* device) override {
    int rssi = device->getRSSI();
    float distance = estimateDistanceMeters(rssi);
    unsigned long lastSeenMs = millis();

    Serial.println("\n--------------------");

    Serial.print("Address: ");
    Serial.println(device->getAddress().toString().c_str());

    Serial.print("Name: ");
    Serial.println(device->haveName() ? device->getName().c_str() : "Unknown");

    Serial.print("RSSI: ");
    Serial.print(rssi);
    Serial.println(" dBm");

    Serial.print("Estimated Distance: ");
    if (distance >= 0) {
      Serial.print(distance, 2);
      Serial.println(" m");
    } else {
      Serial.println("Unknown");
    }

    Serial.print("Last Seen: ");
    Serial.print(lastSeenMs / 1000);
    Serial.println(" sec since boot");

    int battery = getAdvertisedBatteryPercent(device);
    Serial.print("Battery: ");
    if (battery >= 0) {
      Serial.print(battery);
      Serial.println("%");
    } else {
      Serial.println("Unknown");
    }

    if (device->haveManufacturerData()) {
      std::string data = device->getManufacturerData();

      if (data.length() >= 2) {
        uint16_t manufacturerID =
        ((uint8_t)data[1] << 8) | (uint8_t)data[0];

        Serial.print("Manufacturer ID: ");
        Serial.println(BLEManufacturerIdHex(manufacturerID));

        Serial.print("Manufacturer: ");
        Serial.println(
          BLEManufacturerLookup(manufacturerID, "Unknown Manufacturer")
        );
      }
    } else {
      Serial.println("Manufacturer: None");
    }
  }
};

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Starting BLE-only scanner...");

  BLEManufacturerSetup();

  NimBLEDevice::init("");

  NimBLEScan* scan = NimBLEDevice::getScan();
  scan->setScanCallbacks(new ScanCallbacks());

  scan->setActiveScan(true);
  scan->setInterval(100);
  scan->setWindow(99);

  Serial.println("Scanning...");
  scan->start(0, false, false);
}

void loop() {
  delay(1000);
}
