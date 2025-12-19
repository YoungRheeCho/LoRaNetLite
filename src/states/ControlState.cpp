#include "ControlState.h"
#define CONTROL_MSG "CONTROLCONTROLCONTROLCONTROLCONTROLCONTROL"


ControlState& ControlState::instance() {
    static ControlState inst;
    return inst;
}

void ControlState::onEnter(Node& node) {
    node.clearRxBuffer();
    Serial.println("Control State");
    if(node.ifAllSlotAssigned()){
        node.setControlPhase(ControlPhase::DATA);
    }
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
    Serial.print("Slot:");
    for(int i = 0; i < 8; i++){
        Serial.print(node.getSlot()[i]);
    }
    Serial.println();
    switch (node.getControlPhase()){
        case ControlPhase::INIT: //자기 id에 맞게 time slot에 메시지 보냄
            node.startTimer();
            node.setMySlot(node.getNodeId() - 1);
            node.setSlot(0,node.getNodeId());
            sendInit(node);
            //node.setControlPhase(ControlPhase::RTMR);
            //LoRa.receive();
            /*node.startTimer();
            while(!node.timeout(20)){}*/
            node.setControlPhase(ControlPhase::RTMR);
            node.setNodeHeader(MSG_CONTROL_ALOC);
            node.onSlotAssigned();
            break;

        case ControlPhase::RTMR: 
            sendRtmr(node);
            node.setNodeHeader(MSG_CONTROL_ALOC);
            node.onSlotAssigned();
            break;

        case ControlPhase::DATA:
            sendData(node);
            node.setNodeHeader(MSG_CONTROL_DATA);
            node.onSlotAssigned();
            break;

        default:
            break;
    }

    // 남은 모든 바이트 플러시
    while(LoRa.available()) LoRa.read();
}
