#include <callback_exn.h>

#include <glib.h>

#include <caml/mlvalues.h>  // for value type, Is_exception_result and
			    // Extract_exception
#include <caml/memory.h>    // for GC protection macros
#include <caml/printexc.h>  // for caml_format_exception
#include <caml/callback.h>  // for caml_callback_exn()

/* this function must be applied with the OCaml runtime already
   acquired, and f must have been previously obtained by dereferencing
   the pointer returned by caml_named_value; it will call up f and
   report any exception using g_critical */
void call_with_exn(value f, value arg, const char* name) {
  CAMLparam2(f, arg);
  CAMLlocal1(res);

  res = caml_callback_exn(f, arg)  ;
  if (Is_exception_result(res)) {
    res = Extract_exception(res);
    g_critical("Ocaml callback %s raised an exception: %s",
	       name, caml_format_exception(res));
  }
  CAMLreturn0;
}
