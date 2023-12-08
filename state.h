#ifndef STATE_H
#define STATE_H

#include <stdbool.h>

typedef enum {
    STATE_INTERFACE_INITIALIZATION,
    STATE_SLEEP,
    STATE_INACTIVE,
    STATE_CLOCK,
    STATE_TIMER,
    STATE_STOPWATCH,
    STATE_SETTINGS,
} state_id_t;

typedef volatile struct state_t state_t;

struct state_t
{
    state_id_t current_state;
    state_id_t _previous_state;
    state_id_t _next_state;
    bool _state_change_pending;

    void (*change_state)(state_id_t next_state);
};

#endif // STATE_H

void state_init(state_t *state);