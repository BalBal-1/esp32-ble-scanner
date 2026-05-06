#include <Arduino.h>
#include "ble-manufacturer.h"

typedef struct {
    uint16_t id;
    const char *name;
} BLE_MANUFACTURER;

static const BLE_MANUFACTURER manufacturers[] = {
    #if __has_include("ble-manufacturer-list.h")
    #include "ble-manufacturer-list.h"
    #endif
    { 0, 0 }
};

#define BLE_MANUFACTURER_LIST_LENGTH \
(sizeof(manufacturers) / sizeof(manufacturers[0]) - 1)

void BLEManufacturerSetup(void) {
    Serial.print("Manufacturer entries loaded: ");
    Serial.println(BLE_MANUFACTURER_LIST_LENGTH);
}

const char *BLEManufacturerLookup(const uint16_t id, const char *none) {
    int low = 0;
    int high = BLE_MANUFACTURER_LIST_LENGTH - 1;

    while (low <= high) {
        int mid = low + (high - low) / 2;
        long cmp = (long)id - (long)manufacturers[mid].id;

        if (cmp > 0) {
            low = mid + 1;
        } else if (cmp < 0) {
            high = mid - 1;
        } else {
            return manufacturers[mid].name;
        }
    }

    return none;
}

const char *BLEManufacturerIdHex(const uint16_t id) {
    static char hexid[8];
    sprintf(hexid, "0x%04X", id);
    return hexid;
}
