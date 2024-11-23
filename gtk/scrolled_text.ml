type t

external append : t -> string -> unit = "scrolled_text_append_stub"

(* type safe upcast *)
external to_object : t -> Object.t = "identity_stub"
