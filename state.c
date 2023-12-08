#include "state.h"
#include "common.h"


void change_state(state_t *state, state_id_t new_state)
{
    state->_next_state = new_state;
    state->_state_change_pending = true;
}


// Function to initialize the state
void state_init(state_t *state)
{
    state->current_state = STATE_INTERFACE_INITIALIZATION;
    state->_previous_state = STATE_INTERFACE_INITIALIZATION;
    state->_next_state = STATE_INTERFACE_INITIALIZATION;
    state->_state_change_pending = false;

    state->change_state = change_state; 
}