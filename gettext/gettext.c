#include <locale.h>

#define CAML_NAME_SPACE 1
#include <caml/mlvalues.h>  // for value/type conversions (ocaml->C and C->ocaml)
#include <caml/alloc.h>     // for caml_copy_string
#include <caml/memory.h>    // for GC protection macros

#include <config.h>

#ifdef HAVE_LIBINTL

#include <libintl.h>

// It appears that dune's include form does not allow us to include
// quoted strings so we include it unquoted and stringify it here
#define xstr(a) str(a)
#define str(a) #a

CAMLprim value setup_stub(value unit) {
  //puts(xstr(LOCALEDIR));
  bindtextdomain("example-prog", xstr(LOCALEDIR));
  bind_textdomain_codeset("example-prog", "UTF-8");
  textdomain("example-prog");
  setlocale(LC_ALL,"");
  return Val_true;
}

CAMLprim value gettext_stub(value in) {
  CAMLparam1(in);
  CAMLreturn(caml_copy_string(gettext(String_val(in))));
}

#else

CAMLprim value setup_stub(value unit) {
  setlocale(LC_ALL,"");
  return Val_false;
}

CAMLprim value gettext_stub(value in) {
  return in;
}

#endif
