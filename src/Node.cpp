#include "Node.h"
#include "states/GeneralIdleState.h"
#include "states/MasterIdleState.h"
#include "states/JoinedState.h"
#include "states/SlotAssignState.h"
#include "states/ControlState.h"
#include "states/TDMAState.h"
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

void Node::onSlotAssigned(){
    changeState(TDMAState::instance());
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
    ctx.assignedSlotCount = 0;
    ctx.allSlotsAssigned = false;
    ctx.nextNodeId = 1;
    memset(ctx.slot, 0x00, sizeof(ctx.slot));
}

bool Node::isMaster(){
    return nodeRole == Role::MASTER;
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

bool Node::setSlot(int idx, uint8_t id){
    if (!masterCtx) return false;
    if (id < 1 || id > MAX_NODE_CNT) {
        return false;
    }
    if (idx < 0 || idx >= 8) {
        return false;
    }

    for(int i = 0; i < 8; i++){
        if(masterCtx->slot[i] == id){
            return false;
        }
    }

    if(masterCtx->slot[idx] != 0x00){
        return false;
    }

    masterCtx->slot[idx] = id;
    increaseAssignedSlotCount();
    Serial.print("Assigned Slot count: ");
    Serial.println(getAssignedSlotCount());
    setAllSlotAssigned(getAssignedSlotCount() == getNodeCount());
    return true;
}

void Node::increaseAssignedSlotCount(){
    masterCtx->assignedSlotCount++;
}

uint8_t Node::getAssignedSlotCount(){
    return masterCtx->assignedSlotCount;
}

void Node::setAllSlotAssigned(bool tf){
    masterCtx->allSlotsAssigned = tf;
}
bool Node::ifAllSlotAssigned(){
    return masterCtx->allSlotsAssigned;
}

uint8_t (&Node::getSlot())[8] {
    return masterCtx->slot;   // ctrl.slot은 uint8_t[8]
}

void Node::setMySlot(uint8_t slotNum){
    mySlot = slotNum;
}

uint8_t Node::getMySlot(){
    return mySlot;
}

void Node::setSendTurn(bool tf){
    sendTurn = tf;
}

bool Node::getSendTurn(){
    return sendTurn;
}

void Node::setNodeHeader(uint8_t hdr){
    header = hdr;
}
uint8_t Node::getNodeHeader(){
    return header;
}

void Node::setNextNodeId(uint8_t next){
    masterCtx->nextNodeId = next;
}
uint8_t Node::getNextNodeId(){
    return masterCtx->nextNodeId;
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