module C = Configurator.V1

let () =
  C.main ~name:"example-prog" (fun c ->
      let conf =
        match C.Pkg_config.get c with
        | None -> failwith "pkg-config executable not found"
        | Some pc ->
           match (C.Pkg_config.query_expr_err
                    pc ~package:"gtk4" ~expr:"gtk4 >= 4.0.0") with
           | Error msg -> failwith msg
           | Ok deps -> deps
      in
      C.Flags.write_sexp "config.cflags" conf.cflags ;
      C.Flags.write_sexp "config.libs" conf.libs )
