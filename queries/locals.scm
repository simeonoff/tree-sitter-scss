; Scopes
(stylesheet) @local.scope
(block) @local.scope

; Definitions - Variables ($foo: value)
(declaration
  (variable_name) @local.definition.var)

; Definitions - Mixins
(mixin_statement
  (name) @local.definition.function)

; Definitions - Functions
(function_statement
  (name) @local.definition.function)

; Definitions - Parameters
(parameter
  (variable_name) @local.definition.parameter)

; Definitions - Placeholders
(placeholder
  (placeholder_selector
    (placeholder_name) @local.definition.var))

; Definitions - Keyframes
(keyframes_statement
  (keyframes_name) @local.definition.var)

; Definitions - Layers
(layer_statement
  (layer_name) @local.definition.var)

; References
(variable_value) @local.reference
(mixin_name) @local.reference
(function_name) @local.reference
