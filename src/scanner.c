#include "tree_sitter/parser.h"
#include <wctype.h>
#include <stdio.h>
#include <stdlib.h>

#define DEBUG 0

#if DEBUG == 1
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

typedef enum TokenType {
  DESCENDANT_OP,
  PSEUDO_CLASS_SELECTOR_COLON,
  NO_WHITESPACE,
  SINGLE_QUOTED_STRING_SEGMENT,
  DOUBLE_QUOTED_STRING_SEGMENT,
  APPLY_VALUE,
  VARIABLE_WITHOUT_REST,
  VARIABLE_WITH_REST,
  ERROR_SENTINEL,
  SASSDOC_MARKER,
  SASSDOC_CONTENT
} TokenType;

static inline void advance(TSLexer *lexer) { lexer->advance(lexer, false); }

static inline void skip(TSLexer *lexer) { lexer->advance(lexer, true); }

typedef struct {
  bool in_sassdoc_block;
} Scanner;

void *tree_sitter_scss_external_scanner_create() {
  Scanner *s = calloc(1, sizeof(Scanner));
  return s;
}

void tree_sitter_scss_external_scanner_destroy(void *p) { free(p); }

void tree_sitter_scss_external_scanner_reset(void *p) {
  Scanner *s = (Scanner *)p;
  s->in_sassdoc_block = false;
}

unsigned tree_sitter_scss_external_scanner_serialize(void *p, char *buffer) {
  Scanner *s = (Scanner *)p;
  buffer[0] = s->in_sassdoc_block ? 1 : 0;
  return 1;
}

void tree_sitter_scss_external_scanner_deserialize(void *p, const char *b, unsigned n) {
  Scanner *s = (Scanner *)p;
  s->in_sassdoc_block = (n > 0 && b[0] == 1);
}

static bool scan_for_string_segment(TSLexer *lexer, char delimiter, TokenType stringTokenType) {
  char c = lexer->lookahead;
  bool escaped = false;
  int initialColumn = lexer->get_column(lexer);
  // The column of the most recent backslash character. (Assigned an initial
  // value that can't possibly confuse us.)
  int lastEscape = -2;
  int col;

  if (c == delimiter) {
    return false;
  }

  while (c) {
    if (lexer->eof(lexer)) {
      return false;
    }
    col = lexer->get_column(lexer);
    escaped = col == (lastEscape + 1);
    // PRINTF("Considering char: %c at column: %i and delimiter: %c and escaped: %i\n", c, col, delimiter, escaped);
    if (c == '\\') {
      // Mark the position of this escape so that we know we'll be escaped the
      // next time through the loop.
      lastEscape = col;
    }

    if (c == delimiter && !escaped) {
      // PRINTF("&&& Found matching delimiter at col: %i\n", lexer->get_column(lexer));
      lexer->mark_end(lexer);
      lexer->result_symbol = stringTokenType;
      return true;
    }

    if (c == '#' && !escaped) {
      lexer->mark_end(lexer);
      lexer->result_symbol = stringTokenType;
      lexer->advance(lexer, false);
      if (lexer->lookahead == '{') {
        if (col > initialColumn) {
          return true;
        } else {
          // This token _started_ with `#{`, so there's no preceding string.
          // This is an interpolation.
          return false;
        }
      }
    }

    if (c == '\n' && !escaped) {
      // Parsing error. Newlines must be escaped in strings.
      return false;
    }

    lexer->advance(lexer, false);
    c = lexer->lookahead;
  }
  return false;
}

// @apply values (of PostCSS/Tailwind fame) are like a black hole. Any valid
// class name is a valid space-separated value. Exclamation points are ruled
// out here so we don't match `!important`, but not much else is.
static bool scan_for_apply_value(TSLexer *lexer) {
  while (iswspace(lexer->lookahead)) {
    lexer->advance(lexer, true);
  }
  if (lexer->lookahead == ';' || lexer->lookahead == '!' || lexer->lookahead == '{') {
    return false;
  }
  while (!iswspace(lexer->lookahead) && lexer->lookahead != ';') {
    if (lexer->eof(lexer)) {
      return false;
    }
    if (lexer->lookahead == '!') {
      return false;
    }
    if (lexer->lookahead == '{' || lexer->lookahead == '}') {
      return false;
    }
    lexer->advance(lexer, false);
  }
  lexer->mark_end(lexer);
  lexer->result_symbol = APPLY_VALUE;
  return true;
}

static bool scan_for_variable(TSLexer *lexer, const bool *valid_symbols) {
  if (lexer->lookahead != '$') return false;
  PRINTF(
    "Starting var scan at: %i valid: with? %i without? %i\n",
    lexer->get_column(lexer),
    valid_symbols[VARIABLE_WITH_REST],
    valid_symbols[VARIABLE_WITHOUT_REST]
  );
  // We've already seen the `$`, so we can skip past it.
  lexer->advance(lexer, false);
  if (lexer->eof(lexer)) return false;

  char c = lexer->lookahead;
  // These are the characters that can validly begin a variable.
  if (iswalpha(c) || c == '-' || c == '_') {
    lexer->advance(lexer, false);
  } else {
    return false;
  }
  // At this point, we have a valid variable. Keep going until we reach a
  // character that isn't valid in a variable name.
  while (true) {
    if (lexer->eof(lexer)) return false;
    c = lexer->lookahead;
    // Numbers are allowed in variable names after the first character.
    if (!iswalnum(c) && c != '-' && c != '_') {
      break;
    }
    lexer->advance(lexer, false);
  }
  PRINTF("Marking end at: %i\n", lexer->get_column(lexer));
  lexer->mark_end(lexer);
  lexer->result_symbol = VARIABLE_WITHOUT_REST;

  // We know we have a variable now. But what comes after it? Let's look ahead
  // to make sure it's not a spread operator (`...`). Spread syntax is valid in
  // so few contexts that we treat a variable immediately before `...` as a
  // _separate symbol_.
  for (int i = 0; i < 3; i++) {
    if (lexer->lookahead != '.') {
      return valid_symbols[VARIABLE_WITHOUT_REST];
    }
    lexer->advance(lexer, false);
  }

  // Now we've seen exactly three dots. If the next character _isn't_ a dot,
  // then we've got a rest parameter.
  if (lexer->lookahead == '.') {
    // `....` isn't valid syntax. It'll cause a problem in a minute, but for
    // now let's just tell the parser this is an ordinary non-rest variable.
    return valid_symbols[VARIABLE_WITHOUT_REST];
  } else {
    // We have a valid spread operator ahead of us.
    lexer->result_symbol = VARIABLE_WITH_REST;
    return valid_symbols[VARIABLE_WITH_REST];
  }
}

// Scan for the /// marker that starts a sassdoc line.
// Matches exactly /// (not //// or more).
// Skips leading whitespace (including newlines) so that repeat1(sassdoc_line)
// can match consecutive lines across newline boundaries.
//
// Block-breaking: when already inside a sassdoc_block (in_block is true),
// refuses to match if a blank line (2+ newlines) was encountered while
// skipping whitespace. This causes repeat1(sassdoc_line) to end, splitting
// separate documentation sections into distinct sassdoc_block nodes.
// When starting a new block (in_block is false), blank lines are allowed.
static bool scan_for_sassdoc_marker(TSLexer *lexer, bool in_block) {
  int newline_count = 0;

  // Skip whitespace, counting newlines to detect blank lines.
  while (iswspace(lexer->lookahead)) {
    if (lexer->eof(lexer)) return false;
    if (lexer->lookahead == '\n') newline_count++;
    lexer->advance(lexer, true);
  }

  // A blank line (2+ newlines) breaks the sassdoc block,
  // but only when continuing an existing block.
  if (in_block && newline_count >= 2) return false;

  if (lexer->lookahead != '/') return false;

  // Mark the start of our token.
  lexer->mark_end(lexer);

  lexer->advance(lexer, false);
  if (lexer->lookahead != '/') return false;
  lexer->advance(lexer, false);
  if (lexer->lookahead != '/') return false;
  lexer->advance(lexer, false);

  // If the next char is '/', this is //// or more — not a sassdoc marker.
  if (lexer->lookahead == '/') return false;

  lexer->mark_end(lexer);
  lexer->result_symbol = SASSDOC_MARKER;
  return true;
}

// Scan for sassdoc content: everything after /// up to end of line.
// This is the content portion that gets injected into the sassdoc parser.
// Called immediately after _sassdoc_marker has been consumed.
// Must start at column >= 3 (right after ///) to prevent matching on a new line
// after the parser skips whitespace.
static bool scan_for_sassdoc_content(TSLexer *lexer) {
  // Must not be at start of line — sassdoc_content only appears after ///.
  // If column is 0, the parser skipped a newline and we're on a new line.
  if (lexer->get_column(lexer) < 3) return false;

  // Must have at least some content (not just newline/EOF).
  if (lexer->eof(lexer) || lexer->lookahead == '\n') return false;

  lexer->mark_end(lexer);

  // Consume everything up to newline or EOF.
  while (!lexer->eof(lexer) && lexer->lookahead != '\n') {
    lexer->advance(lexer, false);
  }

  lexer->mark_end(lexer);
  lexer->result_symbol = SASSDOC_CONTENT;
  return true;
}

bool tree_sitter_scss_external_scanner_scan(void *payload, TSLexer *lexer, const bool *valid_symbols) {
  PRINTF(
    "SCAN character: [%c] col:%i validity: %i, %i, %i, %i, %i, %i, %i, %i, %i, sdm:%i, sdc:%i\n",
    lexer->lookahead,
    lexer->get_column(lexer),
    valid_symbols[DESCENDANT_OP],
    valid_symbols[PSEUDO_CLASS_SELECTOR_COLON],
    valid_symbols[NO_WHITESPACE],
    valid_symbols[SINGLE_QUOTED_STRING_SEGMENT],
    valid_symbols[DOUBLE_QUOTED_STRING_SEGMENT],
    valid_symbols[APPLY_VALUE],
    valid_symbols[VARIABLE_WITH_REST],
    valid_symbols[VARIABLE_WITHOUT_REST],
    valid_symbols[ERROR_SENTINEL],
    valid_symbols[SASSDOC_MARKER],
    valid_symbols[SASSDOC_CONTENT]
  );

  // Check for sassdoc marker (///) FIRST, even during error recovery.
  // The scanner skips leading whitespace so that repeat1(sassdoc_line)
  // can consume consecutive lines across newline boundaries.
  if (valid_symbols[SASSDOC_MARKER] &&
      (lexer->lookahead == '/' || iswspace(lexer->lookahead))) {
    Scanner *s = (Scanner *)payload;
    PRINTF("SASSDOC_MARKER is valid, trying scan at col %i, in_block: %i\n",
           lexer->get_column(lexer), s->in_sassdoc_block);
    bool result = scan_for_sassdoc_marker(lexer, s->in_sassdoc_block);
    PRINTF("SASSDOC_MARKER scan result: %i\n", result);
    if (result) {
      // After matching, we're inside a block. Reset happens when
      // the scanner fails to match (block ends) or on deserialize.
      s->in_sassdoc_block = true;
      return true;
    }
    // Match failed — if we were in a block, the block just ended.
    s->in_sassdoc_block = false;
  }

  // We might want more nuanced behavior here in the future, but for now we'll
  // simply decline to use the external scanner during error recovery.
  if (valid_symbols[ERROR_SENTINEL]) return false;

  // Check for sassdoc content (rest of line after ///).
  // IMPORTANT: Only match content immediately after ///, never after whitespace.
  // This prevents the parser from skipping newlines and consuming the next line.
  // NOTE: This check must come AFTER the ERROR_SENTINEL bail-out to prevent
  // error recovery from greedily swallowing code lines as sassdoc_content.
  if (valid_symbols[SASSDOC_CONTENT]) {
    PRINTF("SASSDOC_CONTENT is valid, trying scan, lookahead: [%c]\n", lexer->lookahead);
    bool result = scan_for_sassdoc_content(lexer);
    PRINTF("SASSDOC_CONTENT scan result: %i\n", result);
    if (result) return true;
  }

  // First, consider the tokens for which whitespace is significant.
  if (!iswspace(lexer->lookahead) && valid_symbols[NO_WHITESPACE]) {
    lexer->result_symbol = NO_WHITESPACE;
    lexer->mark_end(lexer);
    return true;
  }

  if (iswspace(lexer->lookahead) && valid_symbols[DESCENDANT_OP]) {
    lexer->result_symbol = DESCENDANT_OP;

    lexer->advance(lexer, true);
    while (iswspace(lexer->lookahead)) {
      lexer->advance(lexer, true);
    }
    lexer->mark_end(lexer);

    if (
      lexer->lookahead == '#' ||
      lexer->lookahead == '.' ||
      lexer->lookahead == '[' ||
      lexer->lookahead == '-' ||
      lexer->lookahead == '*' ||
      lexer->lookahead == '&' ||
      iswalnum(lexer->lookahead)
    ) {
      return true;
    }

    if (lexer->lookahead == ':') {
      lexer->advance(lexer, false);
      if (iswspace(lexer->lookahead)) return false;
      for (;;) {
        if (
          lexer->lookahead == ';' ||
          lexer->lookahead == '}' ||
          lexer->eof(lexer)
        ) return false;
        if (lexer->lookahead == '{') {
          return true;
        }
        lexer->advance(lexer, false);
      }
    }
  }

  if (valid_symbols[PSEUDO_CLASS_SELECTOR_COLON]) {
    while (iswspace(lexer->lookahead)) {
      skip(lexer);
    }
    if (lexer->lookahead == ':') {
      advance(lexer);
      if (iswspace(lexer->lookahead) || lexer->lookahead == ':') {
        return false;
      }
      lexer->mark_end(lexer);
      // If we reach a `{` first, we're in a selector. If we reach a `;` first
      // We need a `{` to be a pseudo class selector; `;` indicates a property.
      while (lexer->lookahead != ';' && lexer->lookahead != '}' && !lexer->eof(lexer)) {
        advance(lexer);
        if (lexer->lookahead == '{') {
          lexer->result_symbol = PSEUDO_CLASS_SELECTOR_COLON;
          return true;
        }
      }
      return false;
    }
  }

  // Now that we've ruled out any whitespace-significant tokens, we can advance
  // to the next non-whitespace character and consider everything else.
  while (iswspace(lexer->lookahead)) {
    lexer->advance(lexer, true);
  }

  if (lexer->lookahead == '$' && (valid_symbols[VARIABLE_WITH_REST] || valid_symbols[VARIABLE_WITHOUT_REST])) {
    return scan_for_variable(lexer, valid_symbols);
  }

  if (valid_symbols[SINGLE_QUOTED_STRING_SEGMENT]) {
    return scan_for_string_segment(lexer, '\'', SINGLE_QUOTED_STRING_SEGMENT);
  }

  if (valid_symbols[DOUBLE_QUOTED_STRING_SEGMENT]) {
    return scan_for_string_segment(lexer, '"', DOUBLE_QUOTED_STRING_SEGMENT);
  }

  if (valid_symbols[APPLY_VALUE]) {
    return scan_for_apply_value(lexer);
  }

  return false;
}
