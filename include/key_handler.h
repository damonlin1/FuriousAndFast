#ifndef __KEY_HANDLER_H__
#define __KEY_HANDLER_H__

// Values passed to a key handler when the given arrow key is pressed
typedef enum {
    NO_KEY = 0,
    LEFT_ARROW = 1, 
    UP_ARROW = 2, 
    RIGHT_ARROW = 3, 
    DOWN_ARROW = 4
} arrow_key_t;

/**
 * The possible types of key events.
 * Enum types in C are much more primitive than in Java; this is equivalent to:
 * typedef unsigned int KeyEventType;
 * #define KEY_PRESSED 0
 * #define KEY_RELEASED 1
 */
typedef enum {
    KEY_PRESSED,
    KEY_RELEASED
} key_event_type_t;

/**
 * A keypress handler.
 * When a key is pressed or released, the handler is passed its char value.
 * Most keys are passed as their char value, e.g. 'a', '1', or '\r'.
 * Arrow keys have the special values listed above.
 *
 * @param key a character indicating which key was pressed
 * @param type the type of key event (KEY_PRESSED or KEY_RELEASED)
 * @param held_time if a press event, the time the key has been held in seconds
 * @param aux auxiliary arguments for the handler
 */
typedef void (*key_handler_t)(char key, key_event_type_t type, double held_time, void *aux);

#endif // #ifndef __KEY_HANDLER_H__