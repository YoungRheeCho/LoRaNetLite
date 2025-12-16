#pragma once

#include <stdint.h>

class Node;

class NodeState {
public:
    virtual ~NodeState() = default;
    virtual void onEnter(Node& node) {}
    virtual void onExit(Node& node) {}
    virtual void run(Node& node) = 0;
    //virtual const char* name() const = 0;
};