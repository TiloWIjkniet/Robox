#include "display_template.h"
const char displayTemplatesNL[MAX_TEMPLATES][DISPLAY_LEN] = 
{
"NON\n",

"De bom staat klaar\nDurf je te stoppen?\nDruk toets om te\nstarten\n",

"DEV MODE\nVoer kamer in\n9999=open alles\n0000=exit\n",

"Bom geactiveerd\nJe hebt [tijd] min\nMaak geen fouten\n",

"Verkeerde kamer\nGa naar:\n[kamer naam]\n",

"Dit klopt niet\nZoek:\n[kamer naam]\n",

"Foute plek\nGa naar:\n[kamer naam]\n",

"Bijna klaar\nLaatste kamer:\n[kamer naam]\n",

"Kamer:\n[kamer naam]\nMaak geen fouten\n",

"Laatste kamer\nAlles beslist hier\nGeen fouten meer\n",

"Voer antwoord in\nDenk goed na\nGeen fouten\n",

"Dit is het moment\nAlles hangt af\nVoer antwoord\n",

"Correct\nGoed gedaan\nGa verder\n",

"Goed antwoord\nJuiste pad\nVolgende stap\n",

"Dat klopt\nNog even\nJe bent dichtbij\n",

"Eerste stap\nGoed begin\nBlijf gaan\n",

"Fout antwoord\nPas op\nNog 1 fout = mis\n",

"Niet correct\nProbeer opnieuw\n",

"Dat klopt niet\nBom tikt door\nSneller!\n",

"Scan vinger\nWacht even\nNiet bewegen\n",

"Draai sleutel\n\n\n",

"Zet schakelaar\n\n\n",

"Verkeerde actie\nProbeer iets anders\n",

"Goede actie\nGa door\n",

"Compartiment open\nIets is vrij\nCheck snel\n",

"Te laat\nBom afgegaan\nEinde spel\n",

"Gelukt\nBom ontmanteld\nNet op tijd\n",

"Kritische fout\nSysteem stopt\nHerstart nodig\n",

"Niet gereset\nReset schakelaar\nEn sleutel\n",

"Het spel is nog steeds bezig\nVoer 1 in om niet door te gaan\n\n"
};

const char displayTemplatesSafeNL[MAX_TEMPLATES][DISPLAY_LEN] = {

"NON\n",

"Systeem klaar\nDurf je het aan?\nDruk toets om te\nstarten\n",

"DEV MODE\nVoer kamer in\n9999=open alles\n0000=exit\n",

"Systeem actief\nJe hebt [tijd] min\nMaak geen fouten\n",

"Verkeerde kamer\nGa naar:\n[kamer naam]\n",

"Dit klopt niet\nZoek:\n[kamer naam]\n",

"Foute plek\nGa snel naar:\n[kamer naam]\n",

"Bijna klaar\nLaatste kamer:\n[kamer naam]\n",

"Kamer:\n[kamer naam]\nMaak geen fouten\n",

"Laatste kamer\nAlles beslist\nGeen fouten meer\n",

"Voer antwoord\nDenk goed na\nLet op fouten\n",

"Dit is het moment\nAlles hangt af\nVoer antwoord\n",

"Correct\nGoed gedaan\nGa verder\n",

"Goed antwoord\nJuiste pad\nVolgende stap\n",

"Dat klopt\nNog even\nJe bent dichtbij\n",

"Eerste stap\nGoed begin\nBlijf gaan\n",

"Fout antwoord\nPas op\nNog 1 fout\n",

"Niet correct\nProbeer opnieuw\n",

"Dat klopt niet\nTijd tikt door\nSneller denken\n",

"Scan vinger\nWacht even\nNiet bewegen\n",

"Draai sleutel\n\n\n",

"Zet schakelaar\n\n\n",

"Verkeerde actie\nProbeer anders\n",

"Goede actie\nGa door\n",

"Compartiment open\nIets is vrij\nKijk snel\n",

"Tijd voorbij\nOpdracht mislukt\nEinde spel\n",

"Gelukt\nOpdracht klaar\nNet op tijd\n",

"Kritische fout\nSysteem stopt\nHerstart nodig\n",

"Niet gereset\nReset schakelaar\nEn sleutel\n",

"Het spel is nog steeds bezig\nVoer 1 in om niet door te gaan\n\n"
};

const char displayTemplatesEn[MAX_TEMPLATES][DISPLAY_LEN] = {

"NONE\n",

"Bomb ready\nDo you dare?\nPress key to\nstart\n",

"DEV MODE\nEnter room\n9999=unlock all\n0000=exit\n",

"Bomb active\nYou have [time]\nNo mistakes\n",

"Wrong room\nGo to:\n[room name]\n",

"Not correct\nLook for:\n[room name]\n",

"Wrong spot\nGo to:\n[room name]\n",

"Almost there\nFinal room:\n[room name]\n",

"Room:\n[room name]\nNo mistakes\n",

"Final room\nAll is decided\nNo mistakes\n",

"Enter answer\nThink well\nNo mistakes\n",

"Moment is here\nAll depends\nEnter answer\n",

"Correct\nWell done\nProceed\n",

"Right answer\nGood path\nNext step\n",

"Correct\nKeep going\nAlmost there\n",

"First step\nGood start\nKeep going\n",

"Wrong answer\nBe careful\n1 more = over\n",

"Incorrect\nTry again\n",

"Not correct\nTime runs\nThink faster\n",

"Scan finger\nWait\nDon’t move\n",

"Turn key\n\n\n",

"Flip switch\n\n\n",

"Wrong action\nTry something\n",

"Correct action\nContinue\n",

"Compartment open\nSomething freed\nCheck fast\n",

"Time’s up\nBomb exploded\nGame over\n",

"Success\nBomb defused\nJust in time\n",

"Critical error\nSystem stopped\nRestart needed\n",

"Not reset\nReset switch\nAnd key\n",

"The game is still running\nEnter 1 to not continue\n\n"
};

const char displayTemplatesSafeEn[MAX_TEMPLATES][DISPLAY_LEN] = {

"NONE\n",

"System ready\nDo you dare?\nPress key to\nstart\n",

"DEV MODE\nEnter room\n9999=unlock all\n0000=exit\n",

"System active\nYou have [time]\nNo mistakes\n",

"Wrong room\nGo to:\n[room name]\n",

"Not correct\nLook for:\n[room name]\n",

"Wrong spot\nGo to:\n[room name]\n",

"Almost there\nFinal room:\n[room name]\n",

"Room:\n[room name]\nNo mistakes\n",

"Final room\nAll is decided\nNo mistakes\n",

"Enter answer\nThink well\nBe careful\n",

"Moment is here\nAll depends\nEnter answer\n",

"Correct\nWell done\nProceed\n",

"Right answer\nGood path\nNext step\n",

"Correct\nKeep going\nAlmost there\n",

"First step\nGood start\nKeep going\n",

"Wrong answer\nBe careful\n1 more risk\n",

"Incorrect\nTry again\n",

"Not correct\nTime runs\nThink faster\n",

"Scan finger\nWait\nDon’t move\n",

"Turn key\n\n\n",

"Flip switch\n\n\n",

"Wrong action\nTry something\n",

"Correct action\nContinue\n",

"Compartment open\nSomething freed\nCheck fast\n",

"Time’s up\nTask failed\nGame over\n",

"Success\nTask complete\nIn time\n",

"Critical error\nSystem stopped\nRestart needed\n",

"Not reset\nReset switch\nAnd key\n",

"The game is still running\nEnter 1 to not continue\n\n"
};