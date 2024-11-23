#include <glib-object.h>

#define CAML_NAME_SPACE 1
#include <caml/mlvalues.h>  // for value/type conversions (ocaml->C and C->ocaml)

CAMLprim value object_ref_stub(value obj) {
  // We do not need to use the CAMLparam/CAMLreturn macros to obtain
  // protection from the garbage collector here as Data_abstract_val
  // does not allocate - instead it just does bit twiddling.  This
  // function is thread safe.
  GObject* c_obj = *((GObject**)Data_abstract_val(obj));
  g_object_ref(c_obj);
  return Val_unit;
}

CAMLprim value object_unref_stub(value obj) {
  // We do not need to use the CAMLparam/CAMLreturn macros to obtain
  // protection from the garbage collector here as Data_abstract_val
  // does not allocate - instead it just does bit twiddling.  This
  // function is in theory thread safe on its own terms but in
  // practice it isn't because GTK finalisation must occur in the GTK
  // thread.
  GObject* c_obj = *((GObject**)Data_abstract_val(obj));
  g_object_unref(c_obj);
  return Val_unit;
}

/* so as to be able to apply Gtk.Object.ref/unref to widgets such as
   Scrolled_text.t entities with type safe upcasts */
CAMLprim value identity_stub(value w) {
  return w;
}
