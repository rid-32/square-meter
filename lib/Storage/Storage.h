#include <Arduino.h>

#ifndef STORAGE
#define STORAGE

#define INIT_ADDR 1023 // номер резервной ячейки
#define INIT_KEY 50 // ключ первого запуска. 0-254, на выбор
#define ADDRESS 0

namespace strg {
template <class Storage_Data> class Storage {
private:
  void init(Storage_Data *);

public:
  Storage(Storage_Data *);
  Storage_Data get();
  void set(Storage_Data *);
};
} // namespace strg

#endif
