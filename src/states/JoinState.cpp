#include "JoinState.h"

JoinState& JoinState::instance() {
    static JoinState inst;
    return inst;
}

void JoinState::onEnter(Node& node) {
}

void JoinState::onExit(Node& node){
}

void JoinState::run(Node& node) {
    node.startTimer();
    while(!node.timeout(3000));
    Serial.println("waiting for fin");
}
