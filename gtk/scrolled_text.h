#ifndef SCROLLED_TEXT_H
#define SCROLLED_TEXT_H

#include <gtk/gtk.h>

/* this returns a GtkScrolledWindow object with GtkTextView widget as
   its child with end mark installed */
GtkWidget* scrolled_text_new(void);

/* this appends text to a GtkScrolledWindow object with GtkTextView
   widget as its child with end mark installed */
void scrolled_text_append(GtkScrolledWindow* scrolled_text, const char* text);

#endif
