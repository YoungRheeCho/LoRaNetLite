#include "ControlState.h"
#define CONTROL_MSG "CONTROLCONTROLCONTROLCONTROLCONTROLCONTROL"


ControlState& ControlState::instance() {
    static ControlState inst;
    return inst;
}

void ControlState::onEnter(Node& node) {
    node.clearRxBuffer();
    node.setControlPhase(ControlPhase::INIT);
    Serial.println("Control State");
    node.startTimer();
}

void ControlState::onExit(Node& node){
}

void ControlState::sendInit(Node& node){
    LoRa.beginPacket();
    LoRa.write(MSG_CONTROL_INIT);                  // 1 byte
    LoRa.write(node.getSlot(), sizeof(node.getSlot()));
    LoRa.endPacket();
}

void ControlState::sendRtmr(Node& node){
    LoRa.beginPacket();
    LoRa.write(MSG_CONTROL_RTMR);                  // 1 byte
    LoRa.write(node.getSlot(), sizeof(node.getSlot()));
    LoRa.endPacket();
}

void ControlState::sendData(Node& node){
    LoRa.beginPacket();
    LoRa.write(MSG_CONTROL_DATA);
    LoRa.write(node.getSlot(), sizeof(node.getSlot()));
    LoRa.endPacket();
}

void ControlState::run(Node& node) {
    //node.startTimer();
    switch (node.getControlPhase()){
        case ControlPhase::INIT: //자기 id에 맞게 time slot에 메시지 보냄
            node.startTimer();
            node.setMySlot(node.getNodeId());
            sendInit(node);
            Serial.print("Slot:");
            for(int i = 0; i < 8; i++){
                Serial.print(node.getSlot()[i]);
            }
            Serial.println();
            //while(!node.timeout(8)){} //guard 8ms
            node.onSlotAssigned(MSG_CONTROL_ALOC);
            break;

        case ControlPhase::RTMR: 
            node.startTimer();
            sendRtmr(node);
            //while(!node.timeout(8)){} //guard 8ms
            break;

        case ControlPhase::DATA:
            node.startTimer();
            sendData(node);
            //while(!node.timeout(8)){} //guard 8ms
            break;

        default:
            break;
    }

    // 남은 모든 바이트 플러시
    while(LoRa.available()) LoRa.read();
}
