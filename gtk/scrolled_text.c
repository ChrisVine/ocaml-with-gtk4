#include <prog_defs.h>

#ifdef HAVE_LIBINTL
#include <libintl.h>
#endif

#define CAML_NAME_SPACE 1
#include <caml/mlvalues.h>  // for value/type conversions (ocaml->C and C->ocaml)

#include <glib-object.h>

#include <scrolled_text.h>

GtkWidget* scrolled_text_new(void) {
  GtkWidget* scrolled_window = gtk_scrolled_window_new();
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
				 GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
  gtk_scrolled_window_set_has_frame(GTK_SCROLLED_WINDOW(scrolled_window),
				    TRUE);
  gtk_widget_set_vexpand(scrolled_window, TRUE);

  GtkWidget* text_view = gtk_text_view_new();
  gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
  GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
  GtkTextIter iter;
  gtk_text_buffer_get_end_iter(buffer, &iter);
  (void)gtk_text_buffer_create_mark(buffer, "end-mark", &iter, FALSE);

  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window),
				text_view);

  return scrolled_window;
}

void scrolled_text_append(GtkScrolledWindow* scrolled_text, const char* text) {
  GtkWidget* text_view = gtk_scrolled_window_get_child(scrolled_text);
  GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
  GtkTextIter iter;
  gtk_text_buffer_get_end_iter(buffer, &iter);
  gtk_text_buffer_insert(buffer, &iter, text, -1);

  gboolean scrolling = FALSE;
  GtkAdjustment* adj = gtk_scrolled_window_get_vadjustment(scrolled_text);
  if (gtk_adjustment_get_value(adj)
      >= (gtk_adjustment_get_upper(adj) - gtk_adjustment_get_page_size(adj) - 1e-12)) {
    scrolling = TRUE;
  }
  if (scrolling) {
    GtkTextMark* mark = gtk_text_buffer_get_mark(buffer, "end-mark");
    gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(text_view), mark,
				 0.0, FALSE, 0.0, 0.0);
  }
}

CAMLprim value scrolled_text_append_stub(value scrolled_text, value text) {
  // we do not need to use the CAMLparam/CAMLreturn macros to obtain
  // protection from the garbage collector here as Data_abstract_val
  // and String_val do not allocate - instead they just do bit
  // twiddling
  GtkScrolledWindow* c_scrolled_text =
    GTK_SCROLLED_WINDOW(*((GObject**)Data_abstract_val(scrolled_text)));
  const char* c_text = String_val(text);
  scrolled_text_append(c_scrolled_text, c_text);
  return Val_unit;
}
