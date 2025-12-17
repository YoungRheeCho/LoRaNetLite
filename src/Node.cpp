#include "Node.h"
#include "states/GeneralIdleState.h"
#include "states/MasterIdleState.h"
#include "states/JoinedState.h"
#include "states/SlotAssignState.h"
#include "states/ControlState.h"
#include <Arduino.h>

Node::Node()
    :nodeId(0),
    nodeRole(Role::GENERAL){
}          // 아직 ID 없음

//void Node::init(NodeState& initial){
void Node::init(){
    state = &GeneralIdleState::instance();
    state->onEnter(*this);
}

void Node::clearRxBuffer(){
    while (true) {
        int packetSize = LoRa.parsePacket();
        if (!packetSize) {
            break;
        }

        while (LoRa.available()) {
            LoRa.read();
        }
    }
}

void Node::changeState(NodeState& next){
    state->onExit(*this);

    state = &next;
    state->onEnter(*this);
}

void Node::onIdleTimeout(){
    changeState(MasterIdleState::instance());
}

void Node::onAllocateId(){
    changeState(ControlState::instance());
}

void Node::onIdAssigned(){
    changeState(JoinedState::instance());
}

void Node::onRecvFin(){
    changeState(SlotAssignState::instance());
}

bool Node::hasNodeId(){
    return getNodeId() != 0;
}

void Node::setNodeId(uint8_t id){
    nodeId = id;
}

uint8_t Node::getNodeId(){
    return nodeId;
}

void Node::setRole(Role role){
    nodeRole = role;
}

void Node::setMaster(){
    nodeRole = Role::MASTER;
    if(!hasNodeId()){
        setNodeId(1);
    }
    static MasterContext ctx;
    masterCtx = &ctx;
    ctx.controlPhase = ControlPhase::INIT;
    ctx.nodeCount = 1;
    memset(ctx.slot, 0x00, sizeof(ctx.slot));
}

Role Node::getRole(){
    return nodeRole;
}

void Node::increaseNodeCount(){
    masterCtx->nodeCount++;
}

uint8_t Node::getNodeCount(){
    return masterCtx->nodeCount;
}

void Node::setSlotCount(uint8_t count){
    slotCount = count;
}

uint8_t Node::getSlotCount(){
    return slotCount;
}

void Node::setControlPhase(ControlPhase p){
    masterCtx->controlPhase = p;
}

ControlPhase Node::getControlPhase() const{
    return masterCtx->controlPhase;
}

void Node::setSlot(int idx, uint8_t id){
    masterCtx->slot[idx] = id;
}

uint8_t (&Node::getSlot())[8] {
    return masterCtx->slot;   // ctrl.slot은 uint8_t[8]
}

void Node::startTimer() {
    timerStart = millis();
}

unsigned long Node::elapsed() const {
    return millis() - timerStart;
}

bool Node::timeout(unsigned long ms) const {
    return elapsed() >= ms;
}

void Node::run(){
    state->run(*this);
}