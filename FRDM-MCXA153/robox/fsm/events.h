#ifndef EVENTS_H
#define EVENTS_H

typedef enum 
{
    E_INIT_COMPLETE,        /**< Initialisatie klaar → INIT naar IDLE */
    E_START_DEV,            /**< Dev pagina geopend → IDLE naar DEV_PAGE */
    E_EXIT_DEV,             /**< Dev pagina gesloten → DEV_PAGE naar IDLE */
    E_START_GAME,           /**< Start het spel → IDLE naar FIRST_ROOM */
    E_ROOM_COMPLETED,       /**< Kamer voltooid → FIRST_ROOM naar ROOM_LOOP of ROOM_LOOP naar ROOM_LOOP*/
    E_ROOM_LOOP_TO_LAST,      /**< Speciaal event: ROOM_LOOP → LAST_ROOM */
    E_ROOM_TIMEOUT,         /**< Tijd verlopen voor een kamer → FIRST_ROOM/ROOM_LOOP/LAST_ROOM naar TIMEOUT */
    E_LAST_ROOM_COMPLETED,  /**< Laatste kamer voltooid → LAST_ROOM naar COMPLETED */
    E_GAME_COMPLETED,       /**< Spel volledig voltooid → COMPLETED naar RESET of optioneel naar ROOM_LOOP */
    E_GAME_TIMEOUT,         /**< Spel gefaald door tijdslimiet → TIMEOUT naar RESET */
    E_RESET_COMPLETE,        /**< Reset klaar → RESET naar IDLE */
    E_GEEN
} event_t;


#endif