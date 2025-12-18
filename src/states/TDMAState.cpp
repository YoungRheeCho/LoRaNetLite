#include "TDMAState.h"
#define CONTROL_MSG "CONTROLCONTROLCONTROLCONTROLCONTROLCONTROL"

TDMAState& TDMAState::instance() {
    static TDMAState inst;
    return inst;
}

void TDMAState::onEnter(Node& node) {
    snprintf(controlMsg, sizeof(controlMsg), "Node%d:CONTROLCONTROLCONTROLCONTROLCONTROLCONTROL", node.getNodeId());
    LoRa.receive();
    Serial.println("TDMA State");
    node.setSendTurn(true);
    node.startTimer();
}

void TDMAState::onExit(Node& node){
}

void TDMAState::run(Node& node) {
    if(node.elapsed() > SLOT_LEN * node.getMySlot() && node.getSendTurn()){
        LoRa.beginPacket();
        LoRa.write(MSG_CONTROL_ALOC);
        LoRa.write((uint8_t*)controlMsg, sizeof(controlMsg));
        LoRa.endPacket();
        node.setSendTurn(false);
        LoRa.receive();
        return;
    }

    int packetSize = LoRa.parsePacket();
    if(packetSize > 0){
        //Serial.print("packet size: ");
        //Serial.println(packetSize);
    
        uint8_t type = LoRa.read(); //header 읽기
        uint8_t slotNum = node.elapsed()/SLOT_LEN;
        char payload[64];
        
    
        for(int i = 0; i < packetSize - 1 && LoRa.available(); i++){
            payload[i] = (char)LoRa.read();
        }
        payload[packetSize-1] = '\0';
    
        if(node.isMaster()){
            if (memcmp(&payload[6], CONTROL_MSG, sizeof(CONTROL_MSG) - 1) == 0) {
                if (payload[4] >= '0' && payload[4] <= '9') {
                    uint8_t id = payload[4] - '0';
                    node.setSlot(id, slotNum);
                }
            }
        }
        //Serial.println(payload);
    }

    if(node.elapsed() > SLOT_LEN * node.getSlotCount()){
        //여기서는 slot assign state로 돌아감
        if(node.isMaster()){
            node.onAllocateId();
        }
        else{
            node.onRecvFin();
        }
    }
}