

void dev_page_onEntry(void)
{
    isplayLoadTemplate(NON, 0, true);
    //Zet esp aan
    //Stuur run data naar esp
}

void dev_page_onUpdate(void)
{
    //Ontvang data van esp
    //Kijk of je uit dev mode gaat
    FSM_addEvent(E_EXIT_DEV); 
}

void dev_page_onExit(void)
{
    //Sla ontvange data op in epromp
}
