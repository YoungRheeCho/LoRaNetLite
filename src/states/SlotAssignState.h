#pragma once

#include "NodeState.h"
#include "Node.h"
#include "String"

class SlotAssignState : public NodeState {
public:
    // singleton
    static SlotAssignState& instance();

    //const char* name() const override { return "GeneralIdle"; }

    void onEnter(Node& node) override;
    void onExit(Node& node) override;
    void run(Node& node) override;
    //void onTimeSlot(Node& node, uint8_t slot) override;
    //void onMessage(Node& node, const uint8_t* data, size_t len) override;

private:
    // 외부 생성/복사 금지
    SlotAssignState() = default;
    SlotAssignState(const SlotAssignState&) = delete;
    SlotAssignState& operator=(const SlotAssignState&) = delete;
};