#include "SlotAssignState.h"

SlotAssignState& SlotAssignState::instance() {
    static SlotAssignState inst;
    return inst;
}

void SlotAssignState::onEnter(Node& node) {
    Serial.println("Slot Assign Phase");
}

void SlotAssignState::onExit(Node& node){
}

void SlotAssignState::run(Node& node) {

}
