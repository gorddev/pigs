#pragma once


#include "ECSBitmask.hpp"

// made by gordie novak feb 11th, 2026

// this file creates static initialization storage to keep track of component ids

namespace gan {

    class ECSTypeIDRegister {
        friend class ECSTypeID;
        /// Gets the next available component ID
        static component_id getNextComponentID() {
            static component_id id = 0;
            return id++;
        }
    };

    class ECSTypeID {
        /// Gets the component idea for the struct of type <S>
        template<typename C>
        static component_id getComponentID() {
            static component_id instance_id = ECSTypeIDRegister::getNextComponentID();
            return instance_id;
        }

        friend class ComponentData;
        friend class ComponentBank;
        friend class ComponentVector;
        friend class ECSCore;
        template<typename... C> friend class EntityFilter;

        template<typename... C>
        static ECSBitmask createBitmask() {
            ECSBitmask bits;
            ([&]<typename T>() {
                bits.assignComponent(ECSTypeID::getComponentID<T>());
            }.template operator()<C>(), ...);
            return bits;
        }
    };
}