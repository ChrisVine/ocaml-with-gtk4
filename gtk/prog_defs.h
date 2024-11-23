#ifndef PROG_DEFS_H
#define PROG_DEFS_H

#include <config.h>

// deal with any configuration issues arising from config.h

#ifndef HAVE_LIBINTL
static inline const char* gettext(const char* text) {
  return text;
}
#endif

#endif // PROG_DEFS_H
