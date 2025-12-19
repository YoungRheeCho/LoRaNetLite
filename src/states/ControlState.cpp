#include "ControlState.h"
#define CONTROL_MSG "CONTROLCONTROLCONTROLCONTROLCONTROLCONTROL"


ControlState& ControlState::instance() {
    static ControlState inst;
    return inst;
}

void ControlState::onEnter(Node& node) {
    node.clearRxBuffer();
    Serial.println("Control State");
}

void ControlState::onExit(Node& node){
}

void ControlState::sendControl(Node& node){
    LoRa.beginPacket();
    LoRa.write(MSG_CONTROL_DATA);
    LoRa.write(node.getSlot(), sizeof(node.getSlot()));
    LoRa.endPacket();
}

void ControlState::run(Node& node) {
    //node.startTimer();
    Serial.print("Slot:");
    for(int i = 0; i < 8; i++){
        Serial.print(node.getSlot()[i]);
    }
    Serial.println();
    sendControl(node);
    node.startTimer();
    while(!node.timeout(30)){}
    node.onSlotAssigned();
    // 남은 모든 바이트 플러시
    while(LoRa.available()) LoRa.read();
}
