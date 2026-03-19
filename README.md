# tree-sitter-scss

SCSS grammar for [tree-sitter](https://github.com/tree-sitter/tree-sitter).

Fork of [savetheclocktower/tree-sitter-scss](https://github.com/savetheclocktower/tree-sitter-scss), based on [tree-sitter-css](https://github.com/tree-sitter/tree-sitter-css).

## Why this fork

The upstream parser covers core SCSS well but lacks modern CSS features and has slowed in development. This fork adds:

- Modern CSS at-rules: `@container`, `@scope`, `@layer`, `@property`, `@starting-style`, `@view-transition`, `@position-try`, `@font-face`, `@page`, `@counter-style`, `@font-feature-values`, `@font-palette-values`
- CSS `if()` function with `style()`, `media()`, `supports()` conditions
- CSS `attr()` function with `type()` and fallback syntax
- Sass `if()` with `sass()` condition per the [breaking change spec](https://sass-lang.com/documentation/breaking-changes/if-function/)
- SassDoc (`///`) support via `sassdoc_block` nodes with [tree-sitter-sassdoc](https://github.com/simeonoff/tree-sitter-sassdoc) injection
- Spread syntax (`$args...`), named arguments, trailing commas in maps
- Module namespacing (`module.function()`, `module.$variable`)

## Installation

### Neovim (nvim-treesitter)

```lua
vim.api.nvim_create_autocmd('User', {
  pattern = 'TSUpdate',
  callback = function()
    require('nvim-treesitter.parsers').scss = {
      install_info = {
        url = 'https://github.com/simeonoff/tree-sitter-scss',
        branch = 'master',
      },
    }
  end,
})
```

Then `:TSInstall scss`.

For SassDoc highlighting, also install [tree-sitter-sassdoc](https://github.com/simeonoff/tree-sitter-sassdoc). The injection is configured in `injections.scm` and works automatically.

### Zed

Included upstream. No setup needed.

## Query files

| File | Purpose |
|------|---------|
| `highlights.scm` | Syntax highlighting |
| `injections.scm` | SassDoc injection, TODO/FIXME in comments |
| `folds.scm` | Code folding |
| `indents.scm` | Auto-indentation |
| `outline.scm` | Document symbols/outline |
| `brackets.scm` | Bracket pair matching |

## Development

```bash
npm install
npx tree-sitter generate
npx tree-sitter test
npx tree-sitter parse example.scss
```

## License

MIT
