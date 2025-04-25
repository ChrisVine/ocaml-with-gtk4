module C = Configurator.V1

let test_libintl =
  {| #include <libintl.h>
   int main() {
     bind_textdomain_codeset("example-prog", "UTF-8");
     return 0;}
   |}

(* for cases where libintl is not incorporated in libc *)
let lib_flags = "-lintl"

let () =
  C.main ~name:"example-prog" (fun c ->
      if C.c_test c test_libintl then begin
        C.C_define.gen_header_file c ~fname:"config.h"
          [ "HAVE_LIBINTL", Switch true ] ;
        C.Flags.write_sexp "config_intl.libs" [] end
      else if C.c_test ~link_flags:[lib_flags] c test_libintl then begin
        C.C_define.gen_header_file c ~fname:"config.h"
          [ "HAVE_LIBINTL", Switch true ] ;
        C.Flags.write_sexp "config_intl.libs" [lib_flags] end
      else begin
        C.C_define.gen_header_file c ~fname:"config.h"
          [ "HAVE_LIBINTL", Switch false ] ;
        C.Flags.write_sexp "config_intl.libs" [] end)
