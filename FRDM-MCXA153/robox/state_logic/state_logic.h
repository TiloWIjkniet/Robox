#ifdef STATE_LOGIC
#define STATE_LOGIC

void dev_page_onEntry(void);
void dev_page_onUpdate(void);
void dev_page_onExit(void);

void idle_onEntry(void);
void idle_onUpdate(void);
void idle_onExit(void);

void first_room_onEntry(void);
void first_room_onUpdate(void);
void first_room_onExit(void);

void room_loop_onEntry(void);
void room_loop_onUpdate(void);
void room_loop_onExit(void);

void last_room_onEntry(void);
void last_room_onUpdate(void);
void last_room_onExit(void);

void completed_onEntry(void);
void completed_onUpdate(void);
void completed_onExit(void);

void timeout_onEntry(void);
void timeout_onUpdate(void);
void timeout_onExit(void);

void reset_onEntry(void);
void reset_onUpdate(void);
void ireset_onExit(void);
#endif