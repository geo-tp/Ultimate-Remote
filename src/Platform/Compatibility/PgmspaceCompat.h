#ifndef PORTABLE_PGMSPACE_H
#define PORTABLE_PGMSPACE_H

#ifdef ARDUINO
#include <pgmspace.h>
#else
#include <cstring>
#ifndef PROGMEM
#define PROGMEM
#endif
#ifndef memcpy_P
#define memcpy_P(dest, src, size) std::memcpy((dest), (src), (size))
#endif
#endif

#endif
