#include "TDMAState.h"
#define CONTROL_MSG "CONTROLCONTROLCONTROLCONTROLCONTROLCONTROL"

TDMAState& TDMAState::instance() {
    static TDMAState inst;
    return inst;
}

void TDMAState::onEnter(Node& node) {
    TDMAheader = node.getNodeHeader();
    snprintf(controlMsg, sizeof(controlMsg), "Node%d:CONTROLCONTROLCONTROLCONTROLCONTROLCONTROL", node.getNodeId());
    LoRa.receive();
    //Serial.println("TDMA State");
    node.setSendTurn(true);
    node.startTimer();
}

void TDMAState::onExit(Node& node){
}

void TDMAState::run(Node& node) {
    unsigned long elapsed = node.elapsed();
    /*Serial.print("runtime:");
    Serial.println(elapsed);*/

    if(elapsed > SLOT_LEN * node.getMySlot() && node.getSendTurn()){
        /*Serial.print("elapsed_send: ");
        Serial.println(elapsed);*/
        LoRa.beginPacket();
        LoRa.write(TDMAheader == MSG_CONTROL_DATA ? MSG_CONTROL_DATA : MSG_CONTROL_ALOC);
        LoRa.write((uint8_t*)controlMsg, sizeof(controlMsg));
        LoRa.endPacket();
        node.setSendTurn(false);
        LoRa.receive();
        return;
    }

    int packetSize = LoRa.parsePacket();
    if(packetSize > 0){
        
        uint8_t type = LoRa.read(); //header 읽기
        char payload[64];
        
        
        for(int i = 0; i < packetSize - 1 && LoRa.available(); i++){
            payload[i] = (char)LoRa.read();
        }
        payload[packetSize-1] = '\0';
        
        if(node.isMaster()){
            if(type == MSG_CONTROL_ALOC){
                if (memcmp(&payload[6], CONTROL_MSG, sizeof(CONTROL_MSG) - 1) == 0) {
                    uint8_t slotNum = elapsed/SLOT_LEN;
                    if (payload[4] >= '0' && payload[4] <= '9') {
                        uint8_t id = payload[4] - '0';
                        /*Serial.print("elapsed_recv:");
                        Serial.println(elapsed);*/
                        Serial.print("id:");
                        Serial.println(id);
                        node.setSlot(slotNum, id);
                    }
                }
            }
            else{
                Serial.println(payload);
            }
        }
    }

    unsigned long timeout = node.isMaster()? SLOT_LEN * (node.getSlotCount() + 1) : SLOT_LEN * node.getSlotCount();
    if(elapsed > timeout){
        //여기서는 slot assign state로 돌아감
        if(node.isMaster()){
            node.onAllocateId();
        }
        else{
            node.onRecvFin();
        }
    }
}

void TDMAState::setTDMAHeader(uint8_t hdr){
    TDMAheader = hdr;
}