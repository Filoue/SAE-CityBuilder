//
// Created by petit on 03.06.2026.
//

#ifndef CITYBUILDER_BT_ACTION_H
#define CITYBUILDER_BT_ACTION_H

#include <functional>

#include "bt_node.h"



namespace  core::ai::behaviour_tree {
    template<std::invocable Taction>
    class ActionNode : public Node {
        Taction callbackaction_;
    public:
        explicit ActionNode(const Taction& callbackaction) : callbackaction_(callbackaction) {}
        ~ActionNode() override = default;
        void Reset() override {}

        Status Tick() override{
            return callbackaction_();
        }
    };
}


#endif //CITYBUILDER_BT_ACTION_H