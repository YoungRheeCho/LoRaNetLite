#include "SlotAssignState.h"

SlotAssignState& SlotAssignState::instance() {
    static SlotAssignState inst;
    return inst;
}

void SlotAssignState::onEnter(Node& node) {
    Serial.println("Slot Assign Phase");
}

void SlotAssignState::onExit(Node& node){
    while(!node.timeout(2000)){}
}

void SlotAssignState::run(Node& node) {
    int packetSize = LoRa.parsePacket();
    if(packetSize <= 0) return;

    uint8_t type = LoRa.read();

    switch (type){
    case MSG_CONTROL_INIT: //자기 id에 맞게 time slot에 메시지 보냄
        /*node.startTimer();
        while(node.timeout(1000)){}
        LoRa.write();*/
        Serial.println("INIT CHECK");
        break;
    case MSG_CONTROL_RTMR: 
        Serial.println("RTMR CHECK");
        break;
    case MSG_CONTROL_DATA:
        Serial.println("DATA CHECK");
        break;
    default:
        break;
    }

    // 남은 모든 바이트 플러시
    while(LoRa.available()) LoRa.read();
}
