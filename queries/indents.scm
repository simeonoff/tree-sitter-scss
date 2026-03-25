; Block indentation (CSS & SCSS constructs using { })
(block) @indent.begin
(block "}" @indent.branch @indent.dedent)

; Mixin blocks (used by @mixin and @include)
(mixin_block) @indent.begin
(mixin_block "}" @indent.branch @indent.dedent)

; Parenthesized groups (maps, lists, arguments)
[
  (map_value)
  (list_value)
  (parameters)
  (arguments)
] @indent.begin
(map_value ")" @indent.end)
(list_value ")" @indent.end)
(parameters ")" @indent.end)
(arguments ")" @indent.end)

(comment) @indent.ignore
