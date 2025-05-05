#include <glib.h>

#define CAML_NAME_SPACE 1
#include <caml/mlvalues.h>  // for value/type conversions (ocaml->C and C->ocaml)
#include <caml/memory.h>    // for GC protection macros
#include <caml/callback.h>  // for caml_named_value()
#include <caml/threads.h>   // for caml_acquire_runtime_system() and
			    // caml_release_runtime_system()

#include <callback_exn.h>

static gboolean callback_func(const char* c_name) {
  caml_acquire_runtime_system();
  /* 'cb' does not need protection from garbage collection as it is a
     C pointer to a value type (and a constant in OCaml): the value
     type is not crystalised until 'cb' is dereferenced when calling
     call_with_exn().  call_with_exn() could trigger a collection but
     by then it doesn't matter. */
  /* we cannot cache cb, as it depends on the value of c_name */
  const value* cb = caml_named_value(c_name);
  if (cb != NULL) call_with_exn(*cb, Val_unit, c_name);
  else g_critical("Ocaml callback %s has not been registered (callback_func())",
		  c_name);
  caml_release_runtime_system();

  g_free((void*)c_name);
  return G_SOURCE_REMOVE;
}

/* mainloop_post_stub is thread safe and may be invoked in any thread.
   It is mainly intended to enable OCaml worker threads to post their
   continuation to GTK's main loop for execution in the main program
   thread (the GTK thread); but if wanted it can be called in the main
   program thread also.  It returns immediately, leaving the
   continuation to execute via the GTK main loop.

   OCaml has no easy way of protecting a function object received as a
   value type by a C function from garbage collection or compaction if
   it is retained as a callback for later execution by the main loop
   (the CAMLparamX macros only work while the body of the function,
   here 'mainloop_post_stub', is executing).  So in callback_func we
   invoke by name a thunk previously registered by Callback.register.
   The registered callback is a thunk because passing an OCaml
   argument to the registered callback is difficult: if the argument
   is not an immediate value it will be allocated on the OCaml heap
   and so cannot be stored directly in a C closure as it will not be
   visible to the garbage collector and so could be prematurely
   collected in the period before the closure executes in the GTK main
   loop, and it could also be moved on heap compaction.  Likewise, you
   cannot reasonably register a new OCaml callback for every datum
   that is to be passed as an OCaml closure.  The answer we adopt is
   to have, as a closure of the thunk, an OCaml queue to pass the
   argument, which still ensures that the argument remains visible to
   and relocatable by the garbage collector.  The Dispatcher module in
   the src directory does this. */
CAMLprim value mainloop_post_stub(value name) {
  /* 'name' does not need protection with CAMLparam macros as nothing
     below could trigger a collection */
  const char* c_name = g_strdup(String_val(name));
  g_idle_add_full(G_PRIORITY_DEFAULT, (GSourceFunc)callback_func,
		  (gpointer)c_name, NULL);
  return Val_unit;
}
