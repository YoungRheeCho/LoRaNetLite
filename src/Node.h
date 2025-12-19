#pragma once

#include <LoRa.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include "NodeState.h"

#define MAX_NODE_CNT 4
#define MSG_ID_ASSIGN 0x01
#define MSG_ASSIGN_ACK 0X02
#define MSG_FIN 0X03
#define MSG_DATA 0X04
#define MSG_CONTROL_INIT 0X05
#define MSG_CONTROL_RTMR 0X06
#define MSG_CONTROL_DATA 0X07
#define MSG_CONTROL_ALOC 0x08
#define SLOT_LEN 80 //한 TIME SLOT의 길이(ms)
#define MAX_SLOT_CNT 8
#define NODE_ID 1

enum class Role : uint8_t {
    MASTER = 0,
    GENERAL = 1
};

enum class ControlPhase : uint8_t {
    INIT,
    RTMR,
    DATA
};

struct MasterContext {
    ControlPhase controlPhase;
    uint8_t nodeCount;
    uint8_t nextNodeId;
    uint8_t slot[8];
    uint8_t assignedSlotCount;
    bool allSlotsAssigned;
};

class Node {
public:
    //상태 관련
    Node();
    //void init(NodeState& initial);
    void init();
    void clearRxBuffer();
    void changeState(NodeState& next);

    void onIdleTimeout();
    void onAllocateId();
    void onIdAssigned();
    void onRecvFin();
    void onSlotAssigned();
    //void on
    
    //node 변수 관련
    bool hasNodeId();
    void setNodeId(uint8_t id);
    uint8_t getNodeId();
    void setRole(Role role);
    void setMaster();
    bool isMaster();
    Role getRole();
    void increaseNodeCount();
    uint8_t getNodeCount();
    void setSlotCount(uint8_t count);
    uint8_t getSlotCount();
    void setControlPhase(ControlPhase p);
    ControlPhase getControlPhase() const;
    bool setSlot(int idx, uint8_t id);
    uint8_t (&getSlot())[8];
    void setMySlot(uint8_t slotNum);
    uint8_t getMySlot();
    void setSendTurn(bool tf);
    bool getSendTurn();
    void increaseAssignedSlotCount();
    uint8_t getAssignedSlotCount();
    void setAllSlotAssigned(bool tf);
    bool ifAllSlotAssigned();
    void setNodeHeader(uint8_t hdr);
    uint8_t getNodeHeader();
    void setNextNodeId(uint8_t next);
    uint8_t getNextNodeId();
    //node timer
    void startTimer();
    unsigned long elapsed() const;
    bool timeout(unsigned long ms) const;

    void run();
    //void onTimeSlot(uint8_t slot);
    // 상태가 쓰는 공용 API
    //void send(const uint8_t* data, size_t len);
    //void setMySlot(uint8_t slot);

private:
    NodeState* state;
    uint8_t nodeId;
    Role nodeRole;
    uint8_t mySlot;
    unsigned long timerStart;
    uint8_t slotCount;
    bool sendTurn;
    uint8_t header;
    MasterContext* masterCtx = nullptr;
};