#pragma once

#include "NodeState.h"
#include "Node.h"
#include "String"

class GeneralIdleState : public NodeState {
public:
    // singleton
    static GeneralIdleState& instance();

    //const char* name() const override { return "GeneralIdle"; }

    void onEnter(Node& node) override;
    void onExit(Node& node) override;
    void run(Node& node) override;
    //void onTimeSlot(Node& node, uint8_t slot) override;
    //void onMessage(Node& node, const uint8_t* data, size_t len) override;

private:
    // 외부 생성/복사 금지
    GeneralIdleState() = default;
    GeneralIdleState(const GeneralIdleState&) = delete;
    GeneralIdleState& operator=(const GeneralIdleState&) = delete;
};