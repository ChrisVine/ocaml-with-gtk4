module C = Configurator.V1

let test_libintl =
  {| #include <libintl.h>
   int main() {
     bind_textdomain_codeset("example-prog", "UTF-8");
     return 0;}
   |}

let () =
  C.main ~name:"example-prog" (fun c ->
      let have_libintl =
        C.c_test c test_libintl in
      C.C_define.gen_header_file c ~fname:"config.h"
        [ "HAVE_LIBINTL", Switch have_libintl ])
