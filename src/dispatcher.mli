type 'a t
exception Dispatcher_closed

val make : ('a -> unit) -> string -> 'a t
val post : 'a t -> 'a -> unit
val count : 'a t -> int
val close : 'a t -> unit
val is_closed : 'a t -> bool
