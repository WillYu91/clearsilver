/* Copyright 2007 Google Inc. All Rights Reserved.
 * Author: falmeida@google.com (Filipe Almeida)
 */

#ifndef __NEO_JSPARSER_H_
#define __NEO_JSPARSER_H_

#include "statemachine.h"

#ifdef __cplusplus
namespace security_streamhtmlparser {
#endif /* __cplusplus */

/* Size of the ring buffer used to lookup the last token in the javascript
 * stream. The size is pretty much arbitrary at this point but must be bigger
 * than the biggest token we want to lookup plus 3: Two delimiters plus an empty
 * ring buffer slot. */
#define JSPARSER_RING_BUFFER_SIZE 18

enum js_state_external_enum {
    JSPARSER_STATE_TEXT,
    JSPARSER_STATE_Q,
    JSPARSER_STATE_DQ,
    JSPARSER_STATE_REGEXP,
    JSPARSER_STATE_COMMENT
};

/* Stores the context of the js scanner.
 * If this file is changed, jsparser_reset() should be updated accordingly.
 */
typedef struct jsparser_ctx_s {

  /* Reference to the statemachine context. */
  statemachine_ctx *statemachine;

  /* Reference to the statemachine definition. */
  statemachine_definition *statemachine_def;

  /* Index to the start of the buffer. */
  int buffer_start;

  /* Index the current writing position (end of the buffer plus one). */
  int buffer_end;

  /* Ring buffer used to lookup the last token. */
  char buffer[JSPARSER_RING_BUFFER_SIZE];

} jsparser_ctx;


void jsparser_reset(jsparser_ctx *ctx);
jsparser_ctx *jsparser_new(void);
int jsparser_state(jsparser_ctx *ctx);
int jsparser_parse(jsparser_ctx *ctx, const char *str, int size);

void jsparser_delete(jsparser_ctx *ctx);

/**
 * Ring buffer functions.
 *
 * These functions are only exported for testing and should not be called from
 * outside of jsparser.c in production code.
 */

/* Appends a character to the ring buffer.
 *
 * Sequences of whitespaces and newlines are folded into one character.
 */
void jsparser_buffer_append_chr(jsparser_ctx *js, char chr);

/* Appends a string to the ring buffer.
 *
 * Sequences of whitespaces and newlines are folded into one character.
 */
void jsparser_buffer_append_str(jsparser_ctx *js, const char *str);

/* Returns the last appended character and removes it from the buffer. If the
 * buffer is empty, then it returns ASCII 0 ('\0').
 */
char jsparser_buffer_pop(jsparser_ctx *js);

/* Returns the value of the character at a certain index in the buffer or an
 * ASCII 0 ('\0') character if the index is extends beyond the size of the
 * buffer, either because we don't have as many characters in the buffer, or
 * because the index points to a place bigger than the size of the buffer..
 *
 * Index positions must be negative, where -1 is the last character appended to
 * the buffer.
 */
char jsparser_buffer_get(jsparser_ctx *js, int pos);

/* Sets the value of the character at a certain index in the buffer. Returns
 * true if the write was successful or false if there was an attempt to write
 * outside of the buffer boundaries.
 *
 * Index positions are negative, were -1 is the last character appended to the
 * buffer. Using positive integers for the index will result in undefined
 * behaviour.
 */
int jsparser_buffer_set(jsparser_ctx *js, int pos, char value);

/* Copies a slice of the buffer to the string pointed to by output. start and
 * end are the indexes of the sliced region. If the start argument extends
 * beyond the beginning of the buffer, the slice will only contain characters
 * starting from beginning of the buffer.
 */
void jsparser_buffer_slice(jsparser_ctx *js, char *buffer, int start, int end);

/* Copy the last javascript identifier or keyword found in the buffer to the
 * string pointed by identifier.
 */
int jsparser_buffer_last_identifier(jsparser_ctx *js, char *identifier);


#define jsparser_parse_chr(a,b) jsparser_parse(a, &(b), 1);
#ifdef __cplusplus
#define jsparser_parse_str(a,b) jsparser_parse(a, b, \
                                               static_cast<int>(strlen(b)));
#else
#define jsparser_parse_str(a,b) jsparser_parse(a, b, (int)strlen(b));
#endif

#ifdef __cplusplus
}  /* namespace security_streamhtmlparser */
#endif /* __cplusplus */

#endif /* __NEO_JSPARSER_H_ */