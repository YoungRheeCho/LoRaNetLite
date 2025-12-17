#include "ControlState.h"
#define CONTROL_MSG "CONTROLCONTROLCONTROLCONTROLCONTROLCONTROL"


ControlState& ControlState::instance() {
    static ControlState inst;
    return inst;
}

void ControlState::onEnter(Node& node) {
    node.clearRxBuffer();
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

void ControlState::sendMessage(Node& node){
    LoRa.beginPacket();
    LoRa.write(MSG_CONTROL_ALOC);
    LoRa.write((uint8_t*)"Node1:CONTROLCONTROLCONTROLCONTROLCONTROLCONTROL", 48);
    LoRa.endPacket();
}

void ControlState::recvSlotMsg(Node& node){
    int packetSize = LoRa.parsePacket();
    int cnt = 0; //buf index count
    char buf[50];
    if(!packetSize){
        return;
    }

    uint8_t slotNum = node.elapsed()/SLOT_LEN;

    uint8_t type = LoRa.read();
    if(type != MSG_CONTROL_ALOC){
        node.clearRxBuffer();
        return;
    }

    while (LoRa.available() && cnt < packetSize - 1) {
        buf[cnt] = (char)LoRa.read();
        cnt++;
    }
    buf[cnt] = '\0';
    Serial.println(buf);
    if (memcmp(&buf[6], CONTROL_MSG, sizeof(CONTROL_MSG) - 1) == 0) {
        if (buf[4] >= '0' && buf[4] <= '9') {
            uint8_t id = buf[4] - '0';
            node.setSlot(id, slotNum);
        }
    }
}

void ControlState::run(Node& node) {
    //node.startTimer();
    switch (node.getControlPhase()){
        case ControlPhase::INIT: //자기 id에 맞게 time slot에 메시지 보냄
        node.startTimer();
        sendInit(node);
        while(!node.timeout(8)){} //guard 8ms
        node.setControlPhase(ControlPhase::SEND);
        break;

    case ControlPhase::RTMR: 
        node.startTimer();
        sendRtmr(node);
        while(!node.timeout(8)){} //guard 8ms
        node.setControlPhase(ControlPhase::SEND);
        break;

    case ControlPhase::DATA:
        node.startTimer();
        sendData(node);
        while(!node.timeout(8)){} //guard 8ms
        //node.setControlPhase(ControlPhase::SEND);
        //다음 state로 넘어갈 것
        break;

    case ControlPhase::SEND:
        node.startTimer(); //timeslot의 시작점
        sendMessage(node);
        node.setControlPhase(ControlPhase::RECV);
        LoRa.receive();
        break;
        
    case ControlPhase::RECV:
        recvSlotMsg(node);
        if(node.elapsed() > SLOT_LEN * node.getSlotCount()){ //DATA 혹은 RTMR로 넘어가야 함.
            node.setControlPhase(ControlPhase::DATA);
        }
        break;

    default:
        break;
    }

    // 남은 모든 바이트 플러시
    while(LoRa.available()) LoRa.read();
}
