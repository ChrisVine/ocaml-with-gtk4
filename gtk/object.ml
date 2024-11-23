type t

(* 'ref' is thread safe *)
external ref: t -> unit = "object_ref_stub"

(* 'unref' is in theory thread safe on its own terms but in practice
   it isn't because GTK finalisation must occur in the GTK thread. *)
external unref: t -> unit = "object_unref_stub"
