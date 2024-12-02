(*********************** setup gettext ***********************)

let _ =
  if not (Gettext.setup ()) then begin
    prerr_endline "libintl does not appear to be installed." ;
    prerr_endline "Translations will not be made." end ;

(***************** provide callbacks for GTK *****************)

  (* this is where all the action is: in a GTK program, once the GTK
     event loop has started everything happens by way of callbacks
     executed in the event loop *)
 
  (* have the button print to a ScrolledText.t widget *)
  let on_clicked =
    let counter = ref 0 in
    fun st -> incr counter ;
              let msg = Printf.sprintf "\nOCaml: %s%d"
                          Gettext.(gettext "Clicked: ")
                          !counter in
              Gtk.Scrolled_text.append st msg in
  Callback.register "button-clicked-cb" on_clicked ;

  (* have a menu activate callback carry out a task in a Thread.t
     thread and print the outcome in a ScrolledText.t widget via a
     Dispatcher.t dispatcher.  The name passed to Dispatcher.make can
     be any name (we never explicitly use the name again in user code)
     provided that it is unique: perhaps we should use a random text
     generator for it.  (One argument for leaving things as they are
     is that the uniqueness of string literals can be tested easily
     with grep.) *)
  let on_activated1 =
    let disp =
      Dispatcher.make
        (fun (st, msg) -> Gtk.Scrolled_text.append st msg ;
                          Gtk.(Object.unref (Scrolled_text.to_object st)))
        "st-disp" in
    fun st ->
    (* We apply 'ref' here so that 'st' remains valid when the main
       loop accesses it later.  This means that we should never apply
       Dispatcher.close to 'disp' unless all posts have executed, or
       'st' might never be finalised because the corresponding 'unref'
       would never be applied.

       In addition the worker thread should apply Gtk.Object.unref via
       Dispatcher.post if it terminates before applying
       Dispatcher.post to 'disp' (alternatively if we can guarantee
       that 'st' remains valid for the duration of the worker thread,
       we can instead apply 'ref' in the worker thread immediately
       before it applies Dispatcher.post to 'disp').

       This is a demo.  Applying ref/unref here is in this case
       unnecessary because the Scrolled_text widget stays valid
       throughout program execution anyway. *)
    Gtk.(Object.ref (Scrolled_text.to_object st)) ;
    ignore @@ Thread.(create
                        (fun () -> delay 0.2 ; (* simulate work *)
                                   Dispatcher.post disp (st, "\nTask1: hello from thread"))
                        ()) in
  Callback.register "win-task1-cb" on_activated1 ;

  (* see comments on on_activated1 *)
  let on_activated2 =
    (* beware, if code accessing 'counter' were running in a new
       domain, 'counter' should be atomic, but as only one Thread.t
       thread can hold the domain lock at a time, we should be OK in
       the code below with a plain int reference here (and of course
       losing the odd increment doesn't matter in this demo); but if
       in doubt use a mutex or atomics *)
    let counter = ref 0 in
    let disp =
      Dispatcher.make
        (fun (lbl, num) -> Gtk.Mainwindow.set_label_text lbl num ;
                           Gtk.(Object.unref (Mainwindow.label_to_object lbl)))
        "label-disp" in
    fun lbl ->
    incr counter ;
    Gtk.(Object.ref (Mainwindow.label_to_object lbl)) ;
    ignore @@ Thread.(create
                        (fun () -> delay 0.2 ; (* simulate work *)
                                   Dispatcher.post disp (lbl, !counter))
                        ()) in
  Callback.register "win-task2-cb" on_activated2 ;

(************************* start GTK *************************)

  exit (Gtk.start (Array.length Sys.argv) Sys.argv)
