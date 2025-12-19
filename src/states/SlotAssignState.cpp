#include "SlotAssignState.h"

SlotAssignState& SlotAssignState::instance() {
    static SlotAssignState inst;
    return inst;
}

void SlotAssignState::onEnter(Node& node) {
    //Serial.println("Slot Assign Phase");
    node.startTimer();
    LoRa.receive();
}

void SlotAssignState::onExit(Node& node){
    //LoRa.receive();
}

void SlotAssignState::run(Node& node) {

    if(node.timeout(5000)){
        node.setMaster();
        node.onIdleTimeout();
        return;
    }

    int packetSize = LoRa.parsePacket();
    if(packetSize <= 0) return;
    uint8_t type = LoRa.read();
    uint8_t slot[30];
    
    if(type == MSG_CONTROL_DATA){
        /*payload에 자기id 없으면 random back off, 있으면 그대로 tdma state*/
        /*tdma state로 RTMR CTX 가지고 이동*/
        uint8_t emptyIdx[MAX_SLOT_CNT];
        uint8_t emptyCnt = 0;
        bool hasMySlot = false;
        for(int i = 0; i < packetSize - 1 && LoRa.available(); i++){
            slot[i] = LoRa.read();    
            //Serial.print(slot[i]);
            if(slot[i] == node.getNodeId()){
                node.setMySlot(i);
                hasMySlot = true;
                break;
            }
            if (slot[i] == 0x00 && i != node.getMySlot()) {
                emptyIdx[emptyCnt++] = i;
            }
        }
        //Serial.println();
        if(!hasMySlot){
            //random back off 적용해서 slot 다시 넣음 -> node.setMySlot(node.getNodeId() - 1);
            int r = random(emptyCnt);   // [0, emptyCnt-1]
            node.setMySlot(emptyIdx[r]);
            Serial.print("new slot: ");
            Serial.println(node.getMySlot());
        }
        node.onSlotAssigned();
    }
    // 남은 모든 바이트 플러시
    while(LoRa.available()) LoRa.read();
}
