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
        Serial.println("[MASTER] All node IDs assigned. Switching to RUNNING_AS_MASTER.");
        //다음 state로
        return;
    }

    Serial.print("sending node: ");
    Serial.println(nextId);
    LoRa.beginPacket();
    LoRa.print(nextId);
    LoRa.endPacket();

    LoRa.receive();
    //응답 대기
    node.startTimer();
    while (!node.timeout(RESPONSE_TIMEOUT)) {
        int packetSize = LoRa.parsePacket();
        if (packetSize) {
            String incoming = "";
            while (LoRa.available()) {
                incoming += (char)LoRa.read();
            }

            Serial.print("[MASTER] Received: ");
            Serial.println(incoming);

            if (incoming.startsWith("ACK:")) {
                int ackId = incoming.substring(4).toInt();
                if (ackId == node.getNodeCount() + 1) {
                    Serial.print("[MASTER] Node ");

                    Serial.print(ackId);
                    Serial.println(" confirmed. Assign next ID.");
                    node.increaseNodeCount();
                    retry = 0;
                    return;
                }
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