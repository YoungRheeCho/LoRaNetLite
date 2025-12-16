#include "Node.h"
#include "states/GeneralIdleState.h"
#include "states/MasterIdleState.h"
#include "states/JoinedState.h"
#include "states/SlotAssignState.h"
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

void Node::changeState(NodeState& next){
    state->onExit(*this);

    state = &next;
    state->onEnter(*this);
}

void Node::onIdleTimeout(){
    changeState(MasterIdleState::instance());
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
    setNodeId(1);
    node_count = 1;
}

Role Node::getRole(){
    return nodeRole;
}

void Node::increaseNodeCount(){
    node_count++;
}

uint8_t Node::getNodeCount(){
    return node_count;
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