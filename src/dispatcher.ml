(* The scheme adopted in this program is that the gui is constructed
   using GTK, but other things are done in OCaml.  This means that the
   program may wish to carry out blocking operations, including i/o,
   in OCaml in a separate Thread.t thread or domain, but post its
   continuation for execution in the main program thread, which is the
   thread in which GTK runs.  This module facilitates that.

   We may want to pass a different value on every occasion on which a
   callback of a given registered name is invoked by
   Gtk.Mainloop.post, and we cannot reasonably register a new OCaml
   callback for every such value as an OCaml closure.  For immediate
   (unboxed) values such as integers, we could have a version of
   Gtk.Mainloop.post which takes an argument and stores it in a C
   struct.  However that won't work for objects allocated on the OCaml
   heap as they would not be visible to the garbage collector, which
   means they could be prematurely freed or, even if visible by virtue
   of being stored elsewhere at a place known to OCaml, relocated on
   compaction.  This Dispatcher module is therefore a more general
   solution which provides a queue which remains visible to, and
   relocatable by, the garbage collector, implemented in a module
   which helps ensure that a value cannot be popped from the queue
   unless a corresponding post operation has been made (that is, a
   posting to the main loop is associated with a push to the queue).

   The 'make' function takes as arguments first the function taking an
   argument which is to be executed in the glib main loop, and as its
   second argument the name with which the callback is to be
   registered.  It returns a Dispatcher object of type 'a t, where 'a
   is the type of the item passed and t is abstract and so opaque.

   The 'post' function takes as it first argument the relevant
   Dispatcher object of type 'a t, and as it second argument the value
   to be made available to the registered callback when executed.

   The 'count' function returns the number of enqueued postings
   remaining unexecuted.

   The 'close' function empties the queue of any previous postings
   which have not yet executed in the GTK main loop.  It also
   substitutes for the internal callback registered with OCaml a thunk
   which does nothing: this ensures that if there are prior postings
   to the main loop which have not yet executed they will do nothing,
   which is normally what is wanted, and that the internal queue can
   be freed by the garbage collector (otherwise a reference to the
   queue would remain as a closure of the originally registered
   callback).  Once a Dispatcher object has been closed it cannot be
   reused - further applications of 'post' or 'close' will cause a
   Dispatcher_closed exception to be raised.

   'make' must not be called twice (or more) with the same name nor
   Callback.register be called with a name already given to 'make',
   unless earlier Dispatcher objects using that name have been closed.
   Unfortunately this cannot be enforced by Dispatcher
   programmatically because user code always has Callback.register
   available to it, so strict discipline by the programmer is
   required.  Once a Dispatcher object has been closed, the name
   previously given to it may be reused with a new Dispatcher object
   created by 'make', but be careful.  It is best only to do this if
   g_application_run() in the relevant main loop has returned -
   otherwise any previously queued but as yet unexecuted postings may
   attempt to execute the newly registered callback and thereby try to
   extract a non-existent item from its queue: that would not cause
   program termination, but a g_critical warning would be issued by
   the implementation of post_mainloop.  (glib also has the quirk
   that, if a callback calls g_main_loop_quit(), another callback in
   the "same run" as the one doing the quitting may still execute even
   if it was posted immediately after the quitting one, which is why
   it is best only to reuse a name after g_main_loop_run() or
   g_application_run() has actually returned.)

   As the name passed to 'make' is never explictly called up in user
   code again (it is only used in this module) and its main
   requirement is that it should be unique, perhaps a random text
   generator should be used.  (One argument for leaving things as they
   are is that the uniqueness of string literals can be tested easily
   with grep.  YMMV.)

   A mutex is used to protect the queue so all but the 'make' or
   'close' functions are thread-safe and can be applied by any thread.
   The 'make' and 'close' functions are problematic because they apply
   Callback.register, and the callback invoked via Gtk.Mainloop.post
   calls caml_named_value() in the GTK main loop thread (the main
   program thread) by reference to the registered name, and it is not
   clear whether these are thread-safe.  Accordingly, 'make' and
   'close' should only be applied in the main program thread.  As a
   further consequence, only the main program thread should call
   Callback.register.

   If a worker thread is making posts at a high frequency greater than
   the capacity of the GTK main loop to deal with them, continuations
   could accumulate until the resources of the OCaml heap used by
   OCaml, or the C heap used by GTK, are exhausted; and long before
   then GTK is likely to have become unresponsive.  It is improbable
   in normal use, but where that could happen one way of dealing with
   the matter is, before posting, to interrogate the number of
   outstanding tasks using the 'count' function and if it exceeds,
   say, a thousand to apply back pressure by inserting a delay in the
   thread before it posts (the delay ramping up if tasks continue to
   accumulate). *)

type 'a t = {name:string ; q:'a Queue.t; mutex:Mutex.t; mutable closed:bool}

exception Dispatcher_closed

let make f name =
  let q = Queue.create () 
  and mutex = Mutex.create () in
  Callback.register
    name
    (fun () -> f (Mutex.lock mutex ;
                  Fun.protect
                    ~finally:(fun () -> Mutex.unlock mutex)
                    (fun () -> Queue.pop q))) ;
  {name; q; mutex; closed = false}

let post (p:'a t) item =
  Mutex.lock p.mutex ;
  Fun.protect
    ~finally:(fun () -> Mutex.unlock p.mutex)
    (fun () -> if p.closed then raise Dispatcher_closed ;
               Queue.push item p.q) ;
  Gtk.Mainloop.post p.name

let count (p:'a t) =
  Mutex.lock p.mutex ;
  Fun.protect
    ~finally:(fun () -> Mutex.unlock p.mutex)
    (fun () -> Queue.length p.q)

let close (p:'a t) =
  Mutex.lock p.mutex ;
  Fun.protect
    ~finally:(fun () -> Mutex.unlock p.mutex)
    (fun () -> if p.closed then raise Dispatcher_closed ;
               Callback.register p.name (fun () -> ()) ;
               Queue.clear p.q ;
               p.closed <- true)

let is_closed (p:'a t) =
  Mutex.lock p.mutex ;
  Fun.protect
    ~finally:(fun () -> Mutex.unlock p.mutex)
    (fun () -> p.closed)
