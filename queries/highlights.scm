[
  (comment)
  (single_line_comment)
  (sassdoc_block)
] @comment

[
  (tag_name)
  (universal_selector)
  (nesting_selector)
] @tag
(attribute_selector (plain_value) @string)
(parenthesized_query
  (keyword_query) @property)

[
  "~"
  ">"
  "+"
  "-"
  "*"
  "/"
  "="
  "^="
  "|"
  "|="
  "~="
  "$="
  "*="
  ">="
  "<="
  (range_operator)
] @operator

[
  "and"
  "or"
  "not"
  "only"
] @keyword.operator

(attribute_selector (plain_value) @string)
(pseudo_element_selector "::" (tag_name) @selector.pseudo)
(pseudo_class_selector ":" (class_name) @selector.pseudo)

(variable_name) @variable.other.member
(variable_value) @variable.other.member
(argument_name) @variable.parameter

[
  (feature_name)
  (identifier)
] @property

((property_name) @property)

(id_name) @selector.id
(class_name) @selector.class
(namespace_name) @namespace
(namespace_selector (tag_name) @namespace "|")

(attribute_name) @attribute

(function_name) @function
(mixin_name) @function

(function_statement (name) @function)
(mixin_statement (name) @function)

[
  (plain_value)
  (keyframes_name)
  (keyword_query)
] @constant.builtin

(interpolation "#{" @punctuation.special "}" @punctuation.special)
(property_name (interpolation (variable_value) @variable.other.member))

[
  "@media"
  "@import"
  "@charset"
  "@namespace"
  "@supports"
  "@keyframes"
  "@container"
  "@utility"
  "@layer"
  "@scope"
  "@property"
  "@at-root"
  "@debug"
  "@error"
  "@extend"
  "@mixin"
  "@warn"
  (at_keyword)
  (to)
  (from)
  (important)
] @keyword

(container_statement
  (container_name) @variable.other.member)

(style_query "style" @function.builtin)
(scroll_state_query "scroll-state" @function.builtin)

(if_expression (function_name) @function.builtin)
(if_style_condition "style" @function.builtin)
(if_media_condition "media" @function.builtin)
(if_supports_condition "supports" @function.builtin)
(if_sass_condition "sass" @function.builtin)
(if_else_condition) @keyword.control.conditional

(style_condition
  (property_name) @property)

(scroll_state_condition
  (state_name) @property
  (state_value) @constant.builtin)

"@function" @keyword.function

"@return" @keyword.control.return

[
    "@else"
    "@if"
] @keyword.control.conditional

[
  "@while"
  "@each"
  "@for"
  "through"
  "in"
  "from"
  "if"
] @keyword.repeat

[
    "@forward"
    "@import"
    "@include"
    "@use"
] @keyword.control.import

(string_value) @string
(color_value) @string.special

[
  (integer_value)
  (float_value)
] @number
(unit) @type.unit

(boolean_value) @boolean
(null_value) @constant.builtin

[
  ","
  ":"
  "."
  "::"
  ";"
] @punctuation.delimiter
(id_selector "#" @punctuation.delimiter)

[
  "{"
  ")"
  "("
  "}"
  "["
  "]"
] @punctuation.bracket
