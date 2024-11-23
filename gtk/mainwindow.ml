type label

external set_label_text : label -> int -> unit = "set_label_text_stub"

(* type safe upcast *)
external label_to_object : label -> Object.t = "identity_stub"
