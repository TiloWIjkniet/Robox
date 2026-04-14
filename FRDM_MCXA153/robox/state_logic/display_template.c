#include "display_template.h"
const char displayTemplatesNL[MAX_TEMPLATES][DISPLAY_LEN] = 
{

"NON\n",

"De bom staat klaar\nDurf jij hem te stoppen?\nDruk op een toets om te starten\n",

"DEVELOPMENT MODE\nVoer kamernummer in voor data\n9999 = open alles | 0000 = exit\n",

"De bom is geactiveerd\nJe hebt [tijd] min\nMaak geen fouten...\n",

"Verkeerde kamer\nGa naar: [kamer naam]\nBlijf niet hangen\n",

"Dit klopt niet\nZoek: [kamer naam]\nJe zit op het verkeerde spoor\n",

"Foute plek\nGa snel naar: [kamer naam]\nElke seconde telt\n",

"Bijna klaar\nGa naar de laatste kamer: [kamer naam]\nMaak het af\n",

"Kamer: [kamer naam]\nHier moet het gebeuren\nMaak geen fouten\n",

"Dit is de laatste kamer\nHier beslist alles\nGeen fouten meer\n",

"Voer het antwoord in\nDenk goed na\nEen fout kan fataal zijn\n",

"Dit is het moment\nAlles hangt hiervan af\nVoer het antwoord in\n",

"Correct\nDat was scherp\nGa verder\n",

"Goed antwoord\nJe zit op het juiste pad\nVolgende stap\n",

"Dat klopt\nNog even doorgaan\nJe bent dichtbij\n",

"Eerste stap gelukt\nJe bent begonnen\nBlijf doorgaan\n",

"Fout antwoord\nPas op...\nNog zo'n fout en het gaat mis\n",

"Niet correct\nDat zag er niet goed uit\nProbeer het opnieuw\n",

"Dat klopt niet\nDe bom tikt door...\nDenk sneller\n",

"Scan je vinger\nWacht op bevestiging\nNiet bewegen\n",

"Draai de sleutel\n\n\n",

"Zet de schakelaar om\n\n\n",

"Verkeerde handeling\nDit had niet moeten gebeuren\nProbeer iets anders\n",

"Goede handeling\nDit is correct\nGa door\n",

"Compartiment geopend\nEr is iets vrijgegeven\nKijk snel wat er is veranderd\n",

"Te laat\nDe bom is afgegaan\nEinde spel\n",

"Gelukt\nDe bom is ontmanteld\nNet op tijd\n",

"Kritische fout\nSysteem gestopt\nHerstart vereist\n",

"Schakelaar en/of sleutel niet gereset\nReset alstublieft de schakelaar en/of sleutel\nProbeer het daarna opnieuw\n"
};

const char displayTemplatesSafeNL[MAX_TEMPLATES][DISPLAY_LEN] = {

"NON\n",

"Het systeem staat klaar\nDurf jij de uitdaging aan?\nDruk op een toets om te starten\n",

"DEVELOPMENT MODE\nVoer kamernummer in voor data\n9999 = open alles | 0000 = exit\n",

"Het systeem is geactiveerd\nJe hebt [tijd] min\nMaak geen fouten...\n",

"Verkeerde kamer\nGa naar: [kamer naam]\nBlijf niet hangen\n",

"Dit klopt niet\nZoek: [kamer naam]\nJe zit op het verkeerde spoor\n",

"Foute plek\nGa snel naar: [kamer naam]\nElke seconde telt\n",

"Bijna klaar\nGa naar de laatste kamer: [kamer naam]\nMaak het af\n",

"Kamer: [kamer naam]\nHier moet het gebeuren\nMaak geen fouten\n",

"Dit is de laatste kamer\nHier beslist alles\nGeen fouten meer\n",

"Voer het antwoord in\nDenk goed na\nEen fout kan gevolgen hebben\n",

"Dit is het moment\nAlles hangt hiervan af\nVoer het antwoord in\n",

"Correct\nDat was scherp\nGa verder\n",

"Goed antwoord\nJe zit op het juiste pad\nVolgende stap\n",

"Dat klopt\nNog even doorgaan\nJe bent dichtbij\n",

"Eerste stap gelukt\nJe bent begonnen\nBlijf doorgaan\n",

"Fout antwoord\nPas op...\nNog zo'n fout en het kan misgaan\n",

"Niet correct\nDat zag er niet goed uit\nProbeer het opnieuw\n",

"Dat klopt niet\nDe tijd tikt door...\nDenk sneller\n",

"Scan je vinger\nWacht op bevestiging\nNiet bewegen\n",

"Draai de sleutel\n\n\n",

"Zet de schakelaar om\n\n\n",

"Verkeerde handeling\nDit had niet moeten gebeuren\nProbeer iets anders\n",

"Goede handeling\nDit is correct\nGa door\n",

"Compartiment geopend\nEr is iets vrijgegeven\nKijk snel wat er is veranderd\n",

"Tijd voorbij\nDe opdracht is mislukt\nEinde spel\n",

"Gelukt\nDe opdracht is voltooid\nNet op tijd\n",

"Kritische fout\nSysteem gestopt\nHerstart vereist\n",

"Schakelaar en/of sleutel niet gereset\nReset alstublieft de schakelaar en/of sleutel\nProbeer het daarna opnieuw\n"
};

const char displayTemplatesEn[MAX_TEMPLATES][DISPLAY_LEN] = {

"NONE\n",

"The bomb is ready\nDo you dare to stop it?\nPress any key to start\n",

"DEVELOPMENT MODE\nEnter room number for data\n9999 = unlock all | 0000 = exit\n",

"The bomb is activated\nYou have [time] min\nNo mistakes allowed...\n",

"Wrong room\nGo to: [room name]\nDon’t linger\n",

"That’s not correct\nLook for: [room name]\nYou’re on the wrong track\n",

"Wrong spot\nHurry to: [room name]\nEvery second counts\n",

"Almost there\nHead to the final room: [room name]\nFinish it\n",

"Room: [room name]\nThis is where it happens\nNo mistakes allowed\n",

"This is the final room\nEverything is decided here\nNo more mistakes\n",

"Enter your answer\nThink carefully\nOne mistake can be fatal\n",

"This is the moment\nEverything depends on it\nEnter your answer\n",

"Correct\nSharp thinking\nProceed\n",

"Right answer\nYou’re on the right track\nNext step\n",

"That’s correct\nKeep going\nYou’re close\n",

"First step done\nYou’ve started\nKeep pushing\n",

"Wrong answer\nBe careful...\nOne more mistake and it’s over\n",

"Incorrect\nThat didn’t look right\nTry again\n",

"That’s not correct\nTime keeps ticking...\nThink faster\n",

"Scan your finger\nWait for confirmation\nDon’t move\n",

"Turn the key\n\n\n",

"Flip the switch\n\n\n",

"Wrong action\nThat shouldn’t have happened\nTry something else\n",

"Correct action\nThis is right\nContinue\n",

"Compartment opened\nSomething has been released\nCheck what changed quickly\n",

"Time’s up\nThe bomb went off\nGame over\n",

"Success\nThe bomb has been defused\nJust in time\n",

"Critical error\nSystem stopped\nRestart required\n",

"Switch or/and key not reset\nPlease reset the switch or key\nThen try again\n"
};

const char displayTemplatesSafeEn[MAX_TEMPLATES][DISPLAY_LEN] = {

"NONE\n",

"The system is ready\nDo you dare to take the challenge?\nPress any key to start\n",

"DEVELOPMENT MODE\nEnter room number for data\n9999 = unlock all | 0000 = exit\n",

"The system is activated\nYou have [time] min\nNo mistakes allowed...\n",

"Wrong room\nGo to: [room name]\nDon’t linger\n",

"That’s not correct\nLook for: [room name]\nYou’re on the wrong track\n",

"Wrong spot\nHurry to: [room name]\nEvery second counts\n",

"Almost there\nHead to the final room: [room name]\nFinish it\n",

"Room: [room name]\nThis is where it happens\nNo mistakes allowed\n",

"This is the final room\nEverything is decided here\nNo more mistakes\n",

"Enter your answer\nThink carefully\nMistakes can have consequences\n",

"This is the moment\nEverything depends on it\nEnter your answer\n",

"Correct\nSharp thinking\nProceed\n",

"Right answer\nYou’re on the right track\nNext step\n",

"That’s correct\nKeep going\nYou’re close\n",

"First step done\nYou’ve started\nKeep pushing\n",

"Wrong answer\nBe careful...\nOne more mistake and it could go wrong\n",

"Incorrect\nThat didn’t look right\nTry again\n",

"That’s not correct\nTime keeps ticking...\nThink faster\n",

"Scan your finger\nWait for confirmation\nDon’t move\n",

"Turn the key\n\n\n",

"Flip the switch\n\n\n",

"Wrong action\nThat shouldn’t have happened\nTry something else\n",

"Correct action\nThis is right\nContinue\n",

"Compartment opened\nSomething has been released\nCheck what changed quickly\n",

"Time’s up\nThe task failed\nGame over\n",

"Success\nThe task is completed\nJust in time\n",

"Critical error\nSystem stopped\nRestart required\n",

"Switch or/and key not reset\nPlease reset the switch or key\nThen try again\n"
};