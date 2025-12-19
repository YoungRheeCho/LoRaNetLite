#include "TDMAState.h"
#define CONTROL_MSG "CONTROLCONTROLCONTROLCONTROLCONTROLCONTROL"

TDMAState& TDMAState::instance() {
    static TDMAState inst;
    return inst;
}

void TDMAState::onEnter(Node& node) {
    Serial.print("my slot: ");
    Serial.println(node.getMySlot());
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
    unsigned long st;
    unsigned long rt;
    // Serial.print("runtime:");
    // Serial.println(elapsed);

    if(elapsed > SLOT_LEN * node.getMySlot() && node.getSendTurn()){
        /*Serial.print("elapsed_send: ");
        Serial.println(elapsed);*/
        LoRa.beginPacket();
        LoRa.write(MSG_CONTROL_DATA);
        LoRa.write((uint8_t*)controlMsg, sizeof(controlMsg));
        LoRa.endPacket();
        st = node.elapsed();
        Serial.print("send time:");
        Serial.println(st);
        node.setSendTurn(false);
        LoRa.receive();
        return;
    }

    int packetSize = LoRa.parsePacket();
    if(packetSize > 0){
        rt = node.elapsed();
        Serial.print("recv time:");
        Serial.println(rt);
        uint8_t type = LoRa.read(); //header 읽기
        char payload[64];
        
        for(int i = 0; i < packetSize - 1 && LoRa.available(); i++){
            payload[i] = (char)LoRa.read();
        }
        payload[packetSize-1] = '\0';
        Serial.println(payload);
        
        if(node.isMaster()){
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
    }

    unsigned long timeout = node.isMaster()? SLOT_LEN * (MAX_SLOT_CNT + 1) : SLOT_LEN * MAX_SLOT_CNT;
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