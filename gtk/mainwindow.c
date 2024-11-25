#include <prog_defs.h>

#include <stdio.h>

#ifdef HAVE_LIBINTL
#include <libintl.h>
#endif

#define CAML_NAME_SPACE 1
#include <caml/alloc.h>     // for caml_alloc()
#include <caml/mlvalues.h>  // for value/type conversions (ocaml->C and C->ocaml)
#include <caml/memory.h>    // for GC protection macros
#include <caml/callback.h>  // for caml_named_value(), caml_callback_exn() and Is_exception_result
#include <caml/threads.h>   // for caml_release_runtime_system() and
                            // caml_acquire_runtime_system()

#include <glib-object.h>

#include <mainwindow.h>
#include <scrolled_text.h>

/* We cannot call CAMLparam or CAMLlocal unless the runtime has been
   acquired.  This means that without a wrapper the calls to CAMLparam
   and CAMLlocal will not be the first things the function call does,
   and CAMLreturn0 will be applied after the release of the runtime
   again.  This is probably not an issue with CAMLparam0 but to avoid
   any debate we provide a wrapper function which acquires the
   runtime. */
static void button_clicked_func_impl(gpointer data) {
  CAMLparam0();
  CAMLlocal1(v_scrolled_text);

  GtkScrolledWindow* scrolled_text = GTK_SCROLLED_WINDOW(data);
  v_scrolled_text = caml_alloc(1, Abstract_tag);
  *((GtkScrolledWindow**)Data_abstract_val(v_scrolled_text)) = scrolled_text;

  /* 'cb' does not need protection from garbage collection as it is a
      C pointer to a value type (and a constant in OCaml): the value
      type is not crystalised until 'cb' is dereferenced when calling
      caml_callback_exn().  caml_callback_exn() could trigger a
      collection but by then it doesn't matter. */
  char* name = "button-clicked-cb";
  static const value* cb = NULL;
  if (cb == NULL) cb = caml_named_value(name);
  if (cb != NULL) {
    if (Is_exception_result(caml_callback_exn(*cb, v_scrolled_text)))
      g_critical("Ocaml callback %s threw an exception in button_clicked_func()",
                 name);
  }
  else g_critical("Ocaml callback %s has not been registered (button_clicked_func()))",
                  name);
  CAMLreturn0;
}

static void button_clicked_func(GtkButton* button,
				gpointer data) {
  caml_acquire_runtime_system();
  button_clicked_func_impl(data);
  caml_release_runtime_system();
}

/* see comments on button_clicked_func_impl() for why we have a
   wrapper for this function */
static void win_task1_func_impl(gpointer data) {
  CAMLparam0();
  CAMLlocal1(v_scrolled_text);

  GtkScrolledWindow* scrolled_text = GTK_SCROLLED_WINDOW(data);
  v_scrolled_text = caml_alloc(1, Abstract_tag);
  *((GtkScrolledWindow**)Data_abstract_val(v_scrolled_text)) = scrolled_text;

  /* 'cb' does not need protection from garbage collection as it is a
     C pointer to a value type (and a constant in OCaml): the value
     type is not crystalised until 'cb' is dereferenced when calling
     caml_callback_exn().  caml_callback_exn() could trigger a
     collection but by then it doesn't matter. */
  char* name = "win-task1-cb";
  static const value* cb = NULL;
  if (cb == NULL) cb = caml_named_value(name);
  if (cb != NULL) {
    if (Is_exception_result(caml_callback_exn(*cb, v_scrolled_text)))
      g_critical("Ocaml callback %s threw an exception in win_task1_func()",
                 name);
  }
  else g_critical("Ocaml callback %s has not been registered (win_task1_func()))",
                  name);
  CAMLreturn0;
}

static void win_task1_func(GSimpleAction* act, GVariant* var,
			  gpointer data) {
  caml_acquire_runtime_system();
  win_task1_func_impl(data);
  caml_release_runtime_system();
}

/* see comments on button_clicked_func_impl() for why we have a
   wrapper for this function */
static void win_task2_func_impl(gpointer data) {
  CAMLparam0();
  CAMLlocal1(v_label);

  GtkLabel* label = GTK_LABEL(data);
  v_label = caml_alloc(1, Abstract_tag);
  *((GtkLabel**)Data_abstract_val(v_label)) = label;

  /* 'cb' does not need protection from garbage collection as it is a
     C pointer to a value type (and a constant in OCaml): the value
     type is not crystalised until 'cb' is dereferenced when calling
     caml_callback_exn().  caml_callback_exn() could trigger a
     collection but by then it doesn't matter. */
  char* name = "win-task2-cb";
  static const value* cb = NULL;
  if (cb == NULL) cb = caml_named_value(name);
  if (cb != NULL) {
    if (Is_exception_result(caml_callback_exn(*cb, v_label)))
      g_critical("Ocaml callback %s threw an exception in win_task2_func()",
                 name);
  }
  else g_critical("Ocaml callback %s has not been registered (win_task2_func()))",
                  name);
  CAMLreturn0;
}

static void win_task2_func(GSimpleAction* act, GVariant* var,
			  gpointer data) {
  caml_acquire_runtime_system();
  win_task2_func_impl(data);
  caml_release_runtime_system();
}

GActionEntry win_entries1[] = {
  { "task1", win_task1_func, NULL, NULL, NULL },
};

GActionEntry win_entries2[] = {
  { "task2", win_task2_func, NULL, NULL, NULL },
};

GtkWidget* mainwindow_new(GtkApplication* app) {
  GtkWidget* window = gtk_application_window_new(app);

  // needed to show an application menubar in GTK4
  gtk_application_window_set_show_menubar(GTK_APPLICATION_WINDOW(window),
					  TRUE);
  GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
  gtk_widget_set_vexpand(box, TRUE);
  gtk_widget_set_valign(box, GTK_ALIGN_FILL);
  gtk_widget_set_margin_bottom(box, 10);
  gtk_widget_set_margin_top(box, 10);
  gtk_widget_set_margin_start(box, 10);
  gtk_widget_set_margin_end(box, 10);
  
  GtkWidget* bottom_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_box_set_homogeneous(GTK_BOX(bottom_box), TRUE);
  gtk_widget_set_vexpand(bottom_box, FALSE);
  gtk_widget_set_valign(bottom_box, GTK_ALIGN_CENTER);
  gtk_widget_set_hexpand(bottom_box, FALSE);
  gtk_widget_set_halign(bottom_box, GTK_ALIGN_CENTER);
  gtk_widget_set_margin_top(bottom_box, 10);

  GtkWidget* button = gtk_button_new_with_label("Click me");
  GtkWidget* scrolled_text = scrolled_text_new();
  GtkWidget* label = gtk_label_new("Task2: 0");
  g_signal_connect(button, "clicked",
		   G_CALLBACK(button_clicked_func), scrolled_text);

  gtk_window_set_child(GTK_WINDOW(window), box);
  gtk_box_append(GTK_BOX(box), scrolled_text);
  gtk_box_append(GTK_BOX(box), bottom_box);
  gtk_box_append(GTK_BOX(bottom_box), label);
  gtk_box_append(GTK_BOX(bottom_box), button);

  /* add the actions to the GtkApplicationWindow's action map */
  g_action_map_add_action_entries(G_ACTION_MAP(window),
                                  win_entries1,
                                  G_N_ELEMENTS(win_entries1),
                                  scrolled_text);
  g_action_map_add_action_entries(G_ACTION_MAP(window),
                                  win_entries2,
                                  G_N_ELEMENTS(win_entries2),
                                  label);

  gtk_window_set_default_widget(GTK_WINDOW(window), button);

  scrolled_text_append(GTK_SCROLLED_WINDOW(scrolled_text), gettext("hello"));
  scrolled_text_append(GTK_SCROLLED_WINDOW(scrolled_text), gettext(" world"));

  return window;
}

CAMLprim value set_label_text_stub(value label, value num) {
  // we do not need to use the CAMLparam/CAMLreturn macros to obtain
  // protection from the garbage collector here as Data_abstract_val
  // and Int_val do not allocate - instead they just do bit twiddling
  GtkLabel* c_label =
    GTK_LABEL(*((GObject**)Data_abstract_val(label)));
  char buf[32];
  snprintf(buf, 32, "Task2: %d", Int_val(num));
  gtk_label_set_text(c_label, buf);
  return Val_unit;
}
