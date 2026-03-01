void idle_onEntry(void)
{
    displayLoadTemplate(NON, 0, true);
    //Laat start tijd zien
}

void idle_onUpdate(void)
{
    //Kijk of dev mode enablet word
    //kijk of spel gestart word
    FSM_addEvent(E_START_DEV); 
    FSM_addEvent(E_START_GAME); 
}

void idle_onExit(void)
{

}