(* There is no language support for ocaml code in GNU xgettext when
   building pot files.  So we use xgettext's support for lisp which
   scans for parenthetical S-expressions with 'gettext' in first
   (function application) position and a string in second (argument)
   position.  This means that we have to ensure that every gettext
   function call in ocaml is surrounded by parentheses, even when not
   required by the ocaml compiler, and that no gettext expression
   comes after and on the same line as a ';' sequencing operator
   (which would be interpreted as the beginning of a comment).
   Because xgettext in lisp mode looks for a form in parentheses where
   the first word is 'gettext' and the second is a string, it is also
   necessary to provide 'gettext' as an alias for Gettext.gettext or
   use the form 'Gettext.(gettext "...")'.  Note that ocaml comments
   (such as this comment block) are not recognised as comments and
   therefore will be scanned, so avoid a gettext S-expression
   appearing in comments if text which is not in fact for translation
   is to be excluded from the pot file. *)

external setup: unit -> bool = "setup_stub"
external gettext: string-> string = "gettext_stub"
