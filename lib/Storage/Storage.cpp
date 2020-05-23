#include "Storage.h"
#include <EEPROM.h>

using namespace strg;

template <class Storage_Data>
Storage<Storage_Data>::Storage(Storage_Data *data) {
  this->init(data);
}

template <class Storage_Data>
void Storage<Storage_Data>::init(Storage_Data *data) {
  if (EEPROM.read(INIT_ADDR) != INIT_KEY) {
    EEPROM.write(INIT_ADDR, INIT_KEY);

    EEPROM.put(ADDRESS, *data);
  }
}

template <class Storage_Data> Storage_Data Storage<Storage_Data>::get() {
  Storage_Data data;

  EEPROM.get(ADDRESS, data);

  return data;
}

template <class Storage_Data>
void Storage<Storage_Data>::set(Storage_Data *data) {
  EEPROM.put(0, *data);
}
