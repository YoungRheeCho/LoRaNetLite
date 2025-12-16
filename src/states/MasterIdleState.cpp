#include "MasterIdleState.h"   // 전이 대상
#define RESPONSE_TIMEOUT 2000
#define MAX_RETRY 3

MasterIdleState& MasterIdleState::instance(){
    static MasterIdleState inst;
    return inst;
}

void MasterIdleState::onEnter(Node& node){
    node.setMaster();
}

void MasterIdleState::onExit(Node& node){

}

void MasterIdleState::run(Node& node){
    static int retry = 0;
    int nextId = node.getNodeCount() + 1;
    if (node.getNodeCount() == MAX_NODE_CNT) {
        node.startTimer();
        while(!node.timeout(2000)){}
        for(int i = 0; i < 10; i++){
            LoRa.beginPacket();
            LoRa.write(MSG_FIN);                  // 1 byte
            LoRa.endPacket();
        }
        Serial.println("[MASTER] All node IDs assigned. Switching to RUNNING_AS_MASTER.");
        //다음 state로
        
        return;
    }

    Serial.print("sending node: ");
    Serial.println(nextId);
    LoRa.beginPacket();
    LoRa.write(MSG_ID_ASSIGN);                  // 1 byte
    LoRa.write(nextId);     // payload
    LoRa.endPacket();

    LoRa.receive();
    //응답 대기
    node.startTimer();
    while (!node.timeout(RESPONSE_TIMEOUT)) {
        int packetSize = LoRa.parsePacket();
        if (packetSize >= 2) {
            uint8_t type = LoRa.read();
            if(type == MSG_ASSIGN_ACK){
                uint8_t ackId;
                ackId = LoRa.read();

                if(ackId == nextId){
                    node.increaseNodeCount();
                    retry = 0;
                    
                    Serial.print("[MASTER] Node ");
                    Serial.print(ackId);
                    Serial.println(" confirmed. Assign next ID.");
                    return;
                }
            }
            else{
               while (LoRa.available()) LoRa.read();
                continue;
            }
        }
    }

     // 응답이 없을 때 재시도 또는 종료
    retry++;
    if (retry >= MAX_RETRY) {
        Serial.println("[MASTER] No ACK received. Moving to next ID anyway.");
        node.increaseNodeCount();
        retry = 0;
    } else {
        Serial.println("[MASTER] No ACK, retrying...");
    }
}