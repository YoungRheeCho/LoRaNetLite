#include "GeneralIdleState.h"

GeneralIdleState& GeneralIdleState::instance() {
    static GeneralIdleState inst;
    return inst;
}

void GeneralIdleState::onEnter(Node& node) {
    node.startTimer();
}

void GeneralIdleState::onExit(Node& node){
}

void GeneralIdleState::run(Node& node) {
    if (node.hasNodeId()) {
        node.onIdAssigned();
        return;
    }
    
    // timeout시에 master가 됨
    if (node.timeout(5000)) {
        node.onIdleTimeout();
        return;
    }

    int packetSize = LoRa.parsePacket();
    if (!packetSize) {
        return;
    }

    String incoming;
    char ackMsg[16];

    while (LoRa.available()) {
        incoming += (char)LoRa.read();
    }

    node.setNodeId(incoming.toInt());
    snprintf(ackMsg, sizeof(ackMsg), "ACK:%d", node.getNodeId());

    Serial.print("[SLAVE] Sending ACK message: ");
    Serial.println(ackMsg);

    LoRa.beginPacket();
    LoRa.print(ackMsg);
    LoRa.endPacket();
    LoRa.receive();
    node.onIdAssigned();
}
