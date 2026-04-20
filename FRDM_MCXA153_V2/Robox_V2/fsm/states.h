#ifndef STATES_H
#define STATES_H

typedef enum
{
    S_INIT,        /**< Gebruikt bij het starten van het programma (default state) */
    S_IDLE,        /**< Standaard state als het spel nog niet is gestart */
    S_DEV_PAGE,    /**< Developer/instellingspagina voor debugging of configuratie */
    S_FIRST_ROOM,  /**< Eerste kamer van het spel */
    S_ROOM_LOOP,   /**< Loopt door de kamers tijdens het spel */
    S_LAST_ROOM,      /**< De laatste kamer van het spel */
    S_COMPLETED,   /**< Alle kamers zijn op tijd voltooid */
    S_TIMEOUT,     /**< Niet alle kamers op tijd doorlopen (bom gaat af) */
    S_RESET        /**< Verwerkt resultaten, slaat data op en zet het spel terug naar idle */

}state_t;

#endif