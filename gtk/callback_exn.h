#ifndef CALLBACK_EXN_H
#define CALLBACK_EXN_H

#include <caml/mlvalues.h>  // for value type

void call_with_exn(value f, value arg, const char* name);

#endif
