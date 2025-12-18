#include "SlotAssignState.h"

SlotAssignState& SlotAssignState::instance() {
    static SlotAssignState inst;
    return inst;
}

void SlotAssignState::onEnter(Node& node) {
    Serial.println("Slot Assign Phase");
    LoRa.receive();
}

void SlotAssignState::onExit(Node& node){
    //LoRa.receive();
}

void SlotAssignState::run(Node& node) {
    int packetSize = LoRa.parsePacket();
    if(packetSize <= 0) return;
    node.startTimer();
    uint8_t type = LoRa.read();
    uint8_t slot[30];

    switch (type){
    case MSG_CONTROL_INIT:
        /*자기 id를 myslot으로 등록*/
        /*tdma state로 INIT CTX 가지고 이동*/
        node.setMySlot(node.getNodeId() - 1);
        //node.clearRxBuffer();
        node.onSlotAssigned(MSG_CONTROL_INIT);
        Serial.print("slot: ");
        for(int i = 0; i < packetSize - 1 && LoRa.available(); i++){    //Assigned Slot들을 받음
            slot[i] = LoRa.read();
            Serial.print(slot[i]);
        }
        Serial.println();
        break;
    case MSG_CONTROL_RTMR: 
        /*payload에 자기id 없으면 random back off, 있으면 그대로 tdma state*/
        /*tdma state로 RTMR CTX 가지고 이동*/
        for(int i = 0; i < packetSize - 1 && LoRa.available(); i++){    //Assigned Slot들을 받음
            slot[i] = LoRa.read();    
        }
        if(slot[node.getMySlot()] != node.getNodeId()){
            //random back off 적용해서 slot 다시 넣음 -> node.setMySlot(node.getNodeId() - 1);
        }
        node.onSlotAssigned(MSG_CONTROL_RTMR);
        break;

    case MSG_CONTROL_DATA: 
        /*tdma state로 DATA CTX 가지고 이동*/
        node.clearRxBuffer();
        node.onSlotAssigned(MSG_CONTROL_DATA);
        break;
    default:
        break;
    }

    // 남은 모든 바이트 플러시
    while(LoRa.available()) LoRa.read();
}
