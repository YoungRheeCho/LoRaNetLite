#include "GeneralIdleState.h"

GeneralIdleState& GeneralIdleState::instance() {
    static GeneralIdleState inst;
    return inst;
}

void GeneralIdleState::onEnter(Node& node) {
    node.startTimer();
}

void GeneralIdleState::onExit(Node& node){
    LoRa.receive();
}

void GeneralIdleState::run(Node& node) { 
    // timeout시에 master가 됨
    if (node.timeout(5000)) {
        node.onIdleTimeout();
        return;
    }

    int packetSize = LoRa.parsePacket();
    if (packetSize < 2) {
        if (packetSize) {
            while (LoRa.available()) LoRa.read();
        }
        return;
    }

    uint8_t type = LoRa.read();
    if (type != MSG_ID_ASSIGN) {
        while (LoRa.available()) LoRa.read();
        return;
    }
    
    uint8_t recvId = LoRa.read();
    node.setNodeId(recvId);
    //snprintf(ackMsg, sizeof(ackMsg), "ACK:%d", node.getNodeId());

    Serial.print("[SLAVE] Sending ACK message: ");
    Serial.println(recvId);

    LoRa.beginPacket();
    LoRa.write(MSG_ASSIGN_ACK);
    LoRa.write(recvId);
    LoRa.endPacket();
    
    node.onIdAssigned();
}
