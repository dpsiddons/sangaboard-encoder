
#include <stdint.h>

struct EEPROMClass
{
    //Basic user access methods.
    uint8_t read(int idx) { return -1; }
    void write(int idx, uint8_t val) { return; }
    void update(int idx, uint8_t val) { return; }

    template <typename T>
    T &get(int idx, T &t)
    {
        return t;
    }

    template <typename T>
    const T &put(int idx, const T &t)
    {
        return t;
    }
};

static EEPROMClass EEPROM;