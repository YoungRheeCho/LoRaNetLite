#pragma once

#include <LoRa.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include "NodeState.h"
#define MAX_NODE_CNT 3

enum class Role : uint8_t {
    MASTER = 0,
    GENERAL = 1
};

class Node {
public:
    //상태 관련
    Node();
    //void init(NodeState& initial);
    void init();
    void changeState(NodeState& next);

    void onIdleTimeout();
    void onIdAssigned();

    //node 변수 관련
    bool hasNodeId();
    void setNodeId(uint8_t id);
    uint8_t getNodeId();
    void setRole(Role role);
    void setMaster();
    Role getRole();
    void increaseNodeCount();
    uint8_t getNodeCount();

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
    uint8_t node_count;
};