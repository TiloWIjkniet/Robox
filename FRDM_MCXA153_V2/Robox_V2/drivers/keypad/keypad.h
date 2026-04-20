#ifndef KEYPAD_H
#define KEYPAD_H

#define MAX_INPUT_DATA_LEN 9
typedef char inputData_t[MAX_INPUT_DATA_LEN];

void keypad_init(void);
/**
 * @brief Initializes the keypad GPIO configuration.
 */


 /**
 * @brief Checks if a complete keypad input answer is available.
 *
 * @return true  An answer is available in the input buffer.
 * @return false No complete input has been received yet.
 */
bool keypad_answerAvailable(void);

/**
 * @brief Copies the current keypad input buffer to the provided output buffer.
 *
 * This function copies the internally stored keypad input (inputData)
 * into the provided buffer so it can be used by the caller.
 *
 * @param answer Destination buffer where the keypad input will be copied.
 */
void keypad_getAnswer(inputData_t *answer);

/**
 * @brief Updates keypad input state and processes key events.
 *
 * - Normal character input (stored in buffer)
 * - Delete action ('*')
 * - Confirm action ('#')
 * *
 * @return true  A key event was processed.
 * @return false No key was pressed (getKey() returned '\0').
 *
 * @note
 * - Must be called regularly in the main loop.
 */
bool keypad_update(void);

#endif