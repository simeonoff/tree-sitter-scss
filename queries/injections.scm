; Inject sassdoc into /// documentation comments
; Each /// comment line is parsed separately by sassdoc
; The sassdoc grammar handles /// prefixes via extras
((single_line_comment) @injection.content
  (#match? @injection.content "^///")
  (#set! injection.language "sassdoc"))
