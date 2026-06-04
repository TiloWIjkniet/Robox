#include "display_template.h"
const char displayTemplatesNL[MAX_TEMPLATES][DISPLAY_LEN] = 
{
"NON\n",

//referentie
//------------------\n

"De bom staat op \nscherp! Druk op een\n toets om te starten\n",

"DEV MODE voer kamer\nnummer in|9999=open\nalles|0000=exit\n",

"Bom geactiveerd\nJe hebt [time] min\nSucces!\n",

"Verkeerde kamer\nGa naar:\n[room name]\n",

"Dit klopt niet\nZoek:\n[room name]\n",

"Foute plek\nGa naar:\n[room name]\n",

"Bijna klaar\nLaatste kamer:\n[room name]\n",

"Zoek kamer:\n[room name]\ndoe je best!\n",

"Laatste kamer!\nDe eindstreep\nis in zicht!\n",

"Voer antwoord in\nDenk goed na!\n",

"Dit is het moment\nwees op je hoede\n",

"Correct\nGoed gedaan\nGa nu verder\n",

"Goed antwoord\nVolgende stap\n",

"Dat klopt\nNog even!\n",

"Eerste stap\nGoed begin!\nga zo door!\n",

"Fout antwoord\nlaat dit een wake up\ncall zijn (marcus)\n",

"Dit is onjuist\nProbeer opnieuw\n",

"Dat klopt niet\nDe bom tikt door\nDat kan sneller!\n",

"Scan je vinger\nEven wachten\nNiet bewegen!!\n",

"Draai de sleutel\n\n\n",

"Zet de rode\nschakelaar om\n\n",

"Verkeerde actie\nProbeer iets anders!\n",

"actie geslaagd!\nGa door!\n",

"Compartiment open\nJullie hebben dingen\ngevonden!\n",

"Te laat\nDe bom is afgegaan!\nEinde spel\n",

"Gelukt\nBom ontmanteld\nNet op tijd\n",

"Kritische fout\nSysteem stopt\nHerstart nodig\n",

"Niet gereset\nReset schakelaar\nEn sleutel\n",

"spel onderbroken\n1: stoppen\n[keycode]: ga door\n"
};

//--------------------------------------------------------------------------------------------

const char displayTemplatesSafeNL[MAX_TEMPLATES][DISPLAY_LEN] = {

"NON\n",

//referentie
//------------------\n

"De tocht gaat van\nstart! Druk op een\n toets om te starten\n",

"DEV MODE- voer kamer\nnummer in|9999=open\nalles|0000=exit\n",

"tocht gestart!\nJe hebt [time] min\nSucces!\n",

"Verkeerde kamer\nGa naar:\n[room name]\n",

"Dit klopt niet\nZoek:\n[room name]\n",

"Foute plek\nGa naar:\n[room name]\n",

"Bijna klaar\nLaatste kamer:\n[room name]\n",

"Zoek kamer:\n[room name]\ndoe je best!\n",

"Laatste kamer!\nDe eindstreep\nis in zicht!\n",

"Voer antwoord in\nDenk goed na!\n",

"Dit is het moment\nblijf je best doen!\n",

"Correct\nGoed gedaan\nGa nu verder\n",

"Goed antwoord\nVolgende stap\n",

"Dat klopt\nNog even!\n",

"Eerste stap\nGoed begin!\nga zo door!\n",

"Fout antwoord\nlaat dit een wake up\ncall zijn (marcus)\n",

"Dit is onjuist\nProbeer opnieuw\n",

"Dat klopt niet\nblijf proberen!\n\n",

"Scan je vinger\nEven wachten\nNiet bewegen!!\n",

"Draai de sleutel\n\n\n",

"Zet de rode\nschakelaar om\n\n",

"Verkeerde actie\nProbeer iets anders!\n",

"actie geslaagd!\nGa door!\n",

"Compartiment open\nJullie hebben dingen\ngevonden!\n",

"Te laat\nde tijd is om!\nEinde spel\n",

"Gelukt\nmissie geslaagd!\nNet op tijd\n",

"Kritische fout\nSysteem stopt\nHerstart nodig\n",

"Niet gereset\nReset schakelaar\nEn sleutel\n",

"spel onderbroken\n1: stoppen\nkeycode: ga door\n"
};

//-------------------------------------------------------------------

const char displayTemplatesEn[MAX_TEMPLATES][DISPLAY_LEN] = {

"NONE\n",

"The bomb is armed\nPress any key\nto start\n",

"DEV MODE\nEnter room no.\n9999=unlock all\n",

"Bomb activated\nYou have [time]\nGood luck!\n",

"Wrong room\nGo to:\n[room name]\n",

"Not correct\nFind:\n[room name]\n",

"Wrong location\nGo to:\n[room name]\n",

"Almost done\nFinal room:\n[room name]\n",

"Find room:\n[room name]\nGood luck!\n",

"Final room!\nThe finish line\nis in sight!\n",

"Enter answer\nThink carefully\n",

"This is the\nmoment of truth\n",

"Correct\nWell done\nMove on\n",

"Good answer\nNext step\n",

"That's right\nAlmost there!\n",

"First step\nGreat start!\nKeep going!\n",

"Wrong answer\nlet this be a\nwake-up call\n",

"Incorrect\nTry again\n",

"Not correct\nThe bomb keeps\nticking!\n",

"Scan your finger\nPlease wait\nDon't move!\n",

"Turn the key\n\n\n",

"Flip the red\nswitch\n\n",

"Wrong action\nTry something else\n",

"Action success!\nContinue!\n",

"Compartment open\nYou found items!\n",

"Too late\nThe bomb exploded\nGame over\n",

"Success\nBomb defused\nJust in time\n",

"Critical error\nSystem halted\nRestart needed\n",

"Not reset\nReset switch\nAnd key\n",

"Game paused\n1: quit\n[keycode]: resume\n"
};

//----------------------------------------------------------------------------------------------------------------

const char displayTemplatesSafeEn[MAX_TEMPLATES][DISPLAY_LEN] = {

"NONE\n",

"The quest begins\nPress any key\nto start\n",

"DEV MODE\nEnter room no.\n9999=unlock all\n",

"Quest started\nYou have [time]\nGood luck!\n",

"Wrong room\nGo to:\n[room name]\n",

"Not correct\nFind:\n[room name]\n",

"Wrong location\nGo to:\n[room name]\n",

"Almost done\nFinal room:\n[room name]\n",

"Find room:\n[room name]\nGood luck!\n",

"Final room!\nThe finish line\nis in sight!\n",

"Enter answer\nThink carefully\n",

"This is the\nmoment to shine\n",

"Correct\nWell done\nMove on\n",

"Good answer\nNext step\n",

"That's right\nAlmost there!\n",

"First step\nGreat start!\nKeep going!\n",

"Wrong answer\nLet this be a\nwake-up call\n",

"Incorrect\nTry again\n",

"Not correct\nKeep trying!\n",

"Scan your finger\nPlease wait\nDon't move!\n",

"Turn the key\n\n\n",

"Flip the red\nswitch\n\n",

"Wrong action\nTry something else\n",

"Action success!\nContinue!\n",

"Compartment open\nYou found items!\n",

"Too late\nTime is up!\nGame over\n",

"Success\nMission complete\nJust in time\n",

"Critical error\nSystem halted\nRestart needed\n",

"Not reset\nReset switch\nAnd key\n",

"Game paused\n1: quit\n0000: resume\n"
};