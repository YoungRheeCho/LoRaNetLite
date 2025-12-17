#include "JoinedState.h"

JoinedState& JoinedState::instance() {
    static JoinedState inst;
    return inst;
}

void JoinedState::onEnter(Node& node) {
    Serial.println("waiting for fin");
}

void JoinedState::onExit(Node& node){
}

void JoinedState::run(Node& node) {
    int packetSize = LoRa.parsePacket();
    if(packetSize <= 0) return;

    uint8_t type = LoRa.read();

    if(type == MSG_FIN) {
        uint8_t slotCount = LoRa.read();
        node.setSlotCount(slotCount);
        Serial.print("FIN ARRIVED");
        Serial.print("/Slot Count:");
        Serial.println(node.getSlotCount());
        node.onRecvFin();
    }

    // 남은 모든 바이트 플러시
    while(LoRa.available()) LoRa.read();
}
