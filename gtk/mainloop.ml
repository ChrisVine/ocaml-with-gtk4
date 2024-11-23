(* Gtk.mainloop.post may be called in any thread.  The callbacks which
   will eventually execute will execute in the glib mainloop thread,
   which is the main program thread.  The callbacks invoke
   caml_callback_exn(); as it is not clear whether that function,
   together with Callback.register, are thread-safe, the best rule is
   only to apply Callback.register in the main program thread. *)

external post: string -> unit = "mainloop_post_stub"
