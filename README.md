# tree-sitter-scss

SCSS grammar for [tree-sitter](https://github.com/tree-sitter/tree-sitter).

This is an actively maintained fork of [savetheclocktower/tree-sitter-scss](https://github.com/savetheclocktower/tree-sitter-scss), which itself was based on [tree-sitter-css](https://github.com/tree-sitter/tree-sitter-css).

![Showcase](.github/showcase.png)
*Neovim (tree-sitter-scss) vs VS Code vs Zed — highlighting CSS `if()`, `@property`, `@container`, `@scope`, and SassDoc*

## Why This Fork?

The upstream parser is excellent but development has slowed. This fork adds:

### New CSS/SCSS Features
- **CSS `if()` function** - Conditional values with `style()`, `media()`, `supports()` conditions
- **Sass `if()` with `sass()` condition** - Per the [breaking change](https://sass-lang.com/documentation/breaking-changes/if-function/), alongside legacy `if($cond, $val, $val)`
- **`@container` queries** - Container Queries with size, style, and scroll-state conditions
- **`@scope` at-rule** - CSS Cascading and Inheritance Level 6
- **`@layer` at-rule** - CSS Cascade Layers
- **`@property` at-rule** - CSS Custom Property registration
- **`@font-face`**, **`@page`**, **`@counter-style`**, **`@font-feature-values`**, **`@font-palette-values`** - First-class support
- **`@starting-style`**, **`@view-transition`**, **`@position-try`** - Modern CSS at-rules

### Improved SCSS Support
- **SassDoc integration** - `sassdoc_block` node groups consecutive `///` comments for proper injection
- **Named arguments** - `@include mixin($arg: value)`
- **Trailing commas** - Allowed in maps and lists
- **Hyphenated interpolations** - `#{$prefix}-class`
- **Spread syntax** - `$args...` in function/mixin parameters

### Enhanced Queries
- **highlights.scm** - Comprehensive semantic highlighting
- **injections.scm** - SassDoc language injection
- **textobjects.scm** - Text object selections for nvim-treesitter
- **folds.scm** - Code folding for blocks, functions, mixins, maps
- **outline.scm** - Document outline/symbols
- **indents.scm** - Auto-indentation rules
- **brackets.scm** - Bracket pair matching

## Installation

### Neovim (with nvim-treesitter)

Add the parser configuration:

```lua
local parser_config = require("nvim-treesitter.parsers").get_parser_configs()

parser_config.scss = {
  install_info = {
    url = "https://github.com/simeonoff/tree-sitter-scss",
    files = { "src/parser.c", "src/scanner.c" },
    branch = "master",
  },
  filetype = "scss",
}
```

Then install:

```vim
:TSInstall scss
```

### SassDoc Support

This parser includes a `sassdoc_block` node that groups consecutive `///` comments, enabling proper [SassDoc](http://sassdoc.com/) syntax highlighting when used with [tree-sitter-sassdoc](https://github.com/simeonoff/tree-sitter-sassdoc).

The injection is automatic via `injections.scm`:

```scss
/// @param {Color} $color - The color value
/// @returns {Map} A map with the color
@function process-color($color) {
  // ...
}
```

## Query Files

| File | Purpose |
|------|---------|
| `highlights.scm` | Syntax highlighting with semantic tokens |
| `injections.scm` | Language injection (SassDoc) |
| `folds.scm` | Code folding regions |
| `outline.scm` | Document symbols/outline |
| `textobjects.scm` | Text object selections |
| `indents.scm` | Auto-indentation |
| `brackets.scm` | Bracket matching |

## Supported Syntax

### At-Rules

| Rule | Status |
|------|--------|
| `@use`, `@forward` | ✅ Full support with `as`, `with`, `show`, `hide` |
| `@import` | ✅ |
| `@mixin`, `@include` | ✅ With named args and content blocks |
| `@function`, `@return` | ✅ With spread syntax |
| `@if`, `@else`, `@else if` | ✅ |
| `@for`, `@each`, `@while` | ✅ |
| `@extend` | ✅ |
| `@at-root` | ✅ |
| `@error`, `@warn`, `@debug` | ✅ |
| `@media`, `@supports` | ✅ |
| `@keyframes` | ✅ |
| `@container` | ✅ Container Queries (size, style, scroll-state) |
| `@scope` | ✅ CSS Cascading Level 6 |
| `@layer` | ✅ CSS Cascade Layers |
| `@property` | ✅ CSS Custom Properties |
| `@font-face` | ✅ Font declarations |
| `@page` | ✅ Print layout with pseudo-classes and margin at-rules |
| `@counter-style` | ✅ Custom counter styles |
| `@font-feature-values` | ✅ Named font feature sets with sub-blocks |
| `@font-palette-values` | ✅ Custom font palettes |
| `@starting-style` | ✅ Entry animations |
| `@view-transition` | ✅ View transition configuration |
| `@position-try` | ✅ Anchor positioning fallbacks |
| Unknown at-rules | ✅ Generic fallback (both `@rule;` and `@rule { }`) |

### Values

- CSS `if()`: `if(style(--scheme: dark): #eee; else: #333;)`
- Sass `if()`: `if(sass($condition): 10px; else: 20px)` and legacy `if($cond, $a, $b)`
- Variables: `$name`, `$hyphenated-name`
- Maps: `(key: value, ...)` with trailing comma support
- Lists: `(a, b, c)` and `a b c`
- Colors: `#hex`, `rgb()`, `hsl()`, named colors
- Numbers with units: `10px`, `1.5em`, `50%`
- Strings: `"quoted"` and `unquoted`
- Interpolation: `#{$var}`, `#{$prefix}-suffix`
- Boolean: `true`, `false`
- Null: `null`

### Operators

- Arithmetic: `+`, `-`, `*`, `/`, `%`
- Comparison: `==`, `!=`, `<`, `>`, `<=`, `>=`
- Boolean: `and`, `or`, `not`
- String concatenation

## Development

```bash
# Install dependencies
npm install

# Generate parser
npx tree-sitter generate

# Run tests
npx tree-sitter test

# Parse a file
npx tree-sitter parse example.scss
```

## License

MIT
