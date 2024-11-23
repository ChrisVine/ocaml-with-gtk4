#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <gtk/gtk.h>

/* this returns a GtkApplicationWindow object with its child widgets
   installed */
GtkWidget* mainwindow_new(GtkApplication* app);

#endif
