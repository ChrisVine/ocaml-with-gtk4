#include <locale.h>

#include <glib.h>
#include <gio/gio.h>
#include <gtk/gtk.h>

#include <prog_defs.h>

#ifdef HAVE_LIBINTL
#include <libintl.h>
#endif

#define CAML_NAME_SPACE 1
#include <caml/mlvalues.h>  // for value/type conversions (ocaml->C and C->ocaml)
#include <caml/memory.h>    // for GC protection macros
#include <caml/callback.h>  // for caml_named_value(), caml_callback_exn() and Is_exception_result
#include <caml/threads.h>   // for caml_release_runtime_system() and
                            // caml_acquire_runtime_system()

#include <mainwindow.h>


static void app_quit_func(GSimpleAction* act, GVariant* var, gpointer app) {
  g_application_quit(G_APPLICATION(app));
}

GActionEntry app_entries[] = {
  { "quit", app_quit_func, NULL, NULL, NULL },
};

static void startup(GApplication* app) {

  /* Here we provide an application menubar.  In GTK4 we need to call
     gtk_application_window_set_show_menubar explicitly, which we do
     in mainwindow_new().  It will appear at the top of the
     application window (as in gnome), or depending on the environment
     may appear centrally, separated from application windows
     (possibly MacOS).  It can contain items from the GtkApplication's
     action map (which will have the "app."  suffix), or from
     individual windows' action maps (GtkApplicationWindow action maps
     have the "win."  suffix).  If items from a window's action map
     are included and the item is not available for the window in
     focus, the menu item will be greyed out. */

  /* first add the actions to the GtkApplication's action map */
  g_action_map_add_action_entries(G_ACTION_MAP(app),
                                  app_entries,
                                  G_N_ELEMENTS(app_entries),
                                  app);
  /* now construct the menubar */
  GMenu* menubar = g_menu_new();
  /* now construct individual drop down menus for the menubar */
  GMenu* drop_menu1 = g_menu_new();
  GMenu* drop_menu2 = g_menu_new();
  g_menu_append_submenu(menubar, "Menu1", G_MENU_MODEL(drop_menu1));
  g_menu_append_submenu(menubar, "Menu2", G_MENU_MODEL(drop_menu2));
  g_menu_append(drop_menu1, "OCaml task1", "win.task1"); // action installed in mainwindow.c
  g_menu_append(drop_menu1, "_Quit", "app.quit");
  g_menu_append(drop_menu2, "OCaml task2", "win.task2"); // action installed in mainwindow.c
  gtk_application_set_menubar(GTK_APPLICATION(app),
                              G_MENU_MODEL(menubar));
  g_object_unref(menubar);
  g_object_unref(drop_menu1);
  g_object_unref(drop_menu2);
}

static void activate(GtkApplication* app, gpointer data) {
  GtkWindow* window = gtk_application_get_active_window(app);
  if (window != NULL) gtk_window_present(window);
  else {
    window = (GtkWindow*)mainwindow_new(app);
    gtk_window_set_title(window, "example-prog");
    gtk_window_set_default_size(window, 600, 400);
    gtk_window_present(window);
  }
}

static int start_gtk(int c_argc, char** c_argv) {
  GtkApplication* app = gtk_application_new ("org.cvine.example_prog",
					     G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  g_signal_connect(app, "startup", G_CALLBACK(startup), NULL);

  int ret = g_application_run(G_APPLICATION(app), c_argc, c_argv);
  g_object_unref(app);
  return ret;
}

CAMLprim value start_gtk_stub(value argv) {
  // using the ocaml macros to protect against garbage collection is
  // probably overzealous.  Nothing calls ocaml functions here capable
  // of causing a collection.  Wosize_val, String_val and Field are
  // just bit-twiddling C macros.
  CAMLparam1(argv);

  int length = Wosize_val(argv);
  char* arr[length + 1];
  int index = 0;
  for (index = 0; index < length; ++index) {
    arr[index] = g_strdup(String_val(Field(argv, index)));
  }
  arr[length] = NULL;
  caml_release_runtime_system();
  int ret = start_gtk(length, arr);
  caml_acquire_runtime_system();
  for (index = 0; index < length; ++index) {
    g_free(arr[index]);
  }
  CAMLreturn (Val_int(ret));
}
