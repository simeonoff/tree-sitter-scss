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
  "<"
  "+"
  "-"
  "*"
  "/"
  "="
  "=="
  "!="
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

(pseudo_element_selector "::" (tag_name) @selector.pseudo)
(pseudo_class_selector ":" (class_name) @selector.pseudo)
(page_pseudo_class) @selector.pseudo

[
  (variable_name)
  (variable_value)
  (container_statement (container_name))
] @variable.other.member

(argument_name) @variable.parameter

[
  (feature_name)
  (identifier)
  (property_name)
] @property

(id_name) @selector.id
(class_name) @selector.class
(placeholder_name) @selector.class
(namespace_name) @namespace
(namespace_selector (tag_name) @namespace "|")
(variable_module (module) @namespace)

(attribute_name) @attribute

[
  (function_name)
  (mixin_name)
] @function

(function_statement (name) @function)
(mixin_statement (name) @function)

[
  (plain_value)
  (keyframes_name)
  (keyword_query)
  (feature_value)
] @constant.builtin

(interpolation "#{" @punctuation.special "}" @punctuation.special)

[
  "@media"
  "@charset"
  "@namespace"
  "@supports"
  "@keyframes"
  "@container"
  "@layer"
  "@scope"
  "@property"
  "@starting-style"
  "@view-transition"
  "@font-face"
  "@counter-style"
  "@position-try"
  "@font-palette-values"
  "@page"
  "@font-feature-values"
  "@at-root"
  "@debug"
  "@error"
  "@extend"
  "@mixin"
  "@warn"
  "as"
  "with"
  "hide"
  "show"
  (at_keyword)
  (to)
  (from)
  (important)
  (default)
  (global)
  (margin_at_keyword)
  (font_feature_value_keyword)
] @keyword

[
  (selector_query "selector")
  (style_query "style")
  (scroll_state_query "scroll-state")
  (font_tech_query "font-tech")
  (font_format_query "font-format")
  (at_rule_query "at-rule")
  (named_feature_query "named-feature")
  (import_layer "layer")
  (import_supports "supports")
  (if_style_condition "style")
  (if_media_condition "media")
  (if_supports_condition "supports")
  (if_sass_condition "sass")
] @function.builtin

(if_expression (function_name) @function.builtin)
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
  "if"
] @keyword.control.conditional

[
  "@while"
  "@each"
  "@for"
  "through"
  "in"
  "from"
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
