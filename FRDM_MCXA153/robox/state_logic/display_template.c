#include "display_template.h"
    // TODO: Maak de startfunctie af
    // NOTE: Dit is alleen een test-commentaar
    // WARN: Let op, deze functie kan crashen
    // IDEA: Misschien kunnen we dit optimaliseren
    // BUG: Fout bij indexberekening
    // DONE: Deze functie is afgerond
    // DOING: Hier ben ik nu mee bezig
    // TEMP: Tijdelijke code, verwijderen later
    // TEST: Test deze functie grondig
    // FIXME: Hier zit een bug die gefixt moet worden
    // OPTIMIZE: Code kan sneller worden gemaakt
    // REVIEW: Laat iemand anders dit controleren
    // HACK: Snel opgelost, maar niet ideaal
    // DEBUG: Print debug informatie
    // QUESTION: Waarom doet dit niet wat verwacht?

//TODO Beter naamen geven aan de templates en de tekst in de templates aanpassen/ templates toe voegen
//TODO Mogelijk maken dat rile time data in templates komen 
//IDEA Mogelijk naam van kamer in kamers template en of antwoord template gebruiken
 
const char* displayTemplates[MAX_TEMPLATES] = {

    "NON\n",

    "|--------------------------------------------------------|\n"
    "|                                                        |\n"
    "|Idle pagina                                             |\n"
    "|                                                        |\n"
    "|--------------------------------------------------------|\n", // D_IDLE

    "|--------------------------------------------------------|\n"
    "|                                                        |\n"
    "|Def pagina                                              |\n"
    "|                                                        |\n"
    "|--------------------------------------------------------|\n", // D_DEV_PAGE

    "|--------------------------------------------------------|\n"
    "|Start spel                                              |\n"
    "|Het spel werkt zo                                       |\n"
    "|                                                        |\n"
    "|--------------------------------------------------------|\n", // START_D

    "|--------------------------------------------------------|\n"
    "|Ga naar de goede kamer                                  |\n"
    "|                                                        |\n"
    "|                                                        |\n"
    "|--------------------------------------------------------|\n", // KAMER_D

    "|--------------------------------------------------------|\n"
    "|Je bent in de goede kamer                               |\n"
    "|Wat is antwoord op vraag                                |\n"
    "|                                                        |\n"
    "|--------------------------------------------------------|\n", // ANTWOORD_D

    "|--------------------------------------------------------|\n"
    "|Jaa dat was het goede antowwrd                          |\n"
    "|Doe nu speshale action                                  |\n"
    "|                                                        |\n"
    "|--------------------------------------------------------|\n", // GOED_S_D

    "|--------------------------------------------------------|\n"
    "|Jaa dat was het goede antowwrd                          |\n"
    "|                                                        |\n"
    "|                                                        |\n"
    "|--------------------------------------------------------|\n", // GOED_D

    "|--------------------------------------------------------|\n"
    "|Jaa dat was het goede antowwrd                          |\n"
    "|Nu gaat ruimte open                                     |\n"
    "|                                                        |\n"
    "|--------------------------------------------------------|\n", // GOED_C_D

    "|--------------------------------------------------------|\n"
    "|NOPE foud                                               |\n"
    "|:)                                                      |\n"
    "|                                                        |\n"
    "|--------------------------------------------------------|\n", // FOUD_D

    "|--------------------------------------------------------|\n"
    "|Ga naar volgede kamer                                   |\n"
    "|                                                        |\n"
    "|                                                        |\n"
    "|--------------------------------------------------------|\n", // VOLGENDE_D

    "|--------------------------------------------------------|\n"
    "|Ga naar de laatse kamer                                 |\n"
    "|                                                        |\n"
    "|                                                        |\n"
    "|--------------------------------------------------------|\n", // LAATSTE_D

    "|--------------------------------------------------------|\n"
    "|De tijd is op                                           |\n"
    "|Boem                                                    |\n"
    "|                                                        |\n"
    "|--------------------------------------------------------|\n", // TIJD_D

    "|--------------------------------------------------------|\n"
    "|Je hebt het optijd gehaalt                              |\n"
    "|jeeee                                                   |\n"
    "|                                                        |\n"
    "|--------------------------------------------------------|\n", // GEHAALT_D

    "|--------------------------------------------------------|\n"
    "|Save data                                               |\n"
    "|                                                        |\n"
    "|                                                        |\n"
    "|--------------------------------------------------------|\n", // RESET_D

    "|--------------------------------------------------------|\n"
    "|Er is een kritiche error, prose kan niet veder gaan     |\n"
    "|                                                        |\n"
    "|                                                        |\n"
    "|--------------------------------------------------------|\n", // GET_DATA_D
};