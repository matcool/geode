#pragma once

#include "Traits.hpp"

#include <Geode/loader/Loader.hpp>
#include "../utils/MiniFunction.hpp"
#include <cocos2d.h>
#include <vector>

namespace cocos2d {
    class CCNode;
}

namespace geode {
    template <class, class>
    class Modify;
}

namespace geode::modifier {
    class FieldContainer {
    private:
        std::vector<void*> m_containedFields;
        std::vector<utils::MiniFunction<void(void*)>> m_destructorFunctions;

    public:
        ~FieldContainer() {
            for (auto i = 0u; i < m_containedFields.size(); i++) {
                m_destructorFunctions[i](m_containedFields[i]);
                operator delete(m_containedFields[i]);
            }
        }

        void* getField(size_t index) {
            if (m_containedFields.size() <= index) {
                m_containedFields.resize(index + 1);
                m_destructorFunctions.resize(index + 1);
            }
            return m_containedFields.at(index);
        }

        void* setField(size_t index, size_t size, utils::MiniFunction<void(void*)> destructor) {
            m_containedFields.at(index) = operator new(size);
            m_destructorFunctions.at(index) = destructor;
            return m_containedFields.at(index);
        }

        static FieldContainer* from(cocos2d::CCNode* node) {
            return node->getFieldContainer();
        }
    };

    GEODE_DLL size_t getFieldIndexForClass(size_t hash);

    template <class Parent, class Base>
    class FieldIntermediate {
        using Intermediate = Modify<Parent, Base>;
        // Padding used for guaranteeing any member of parents
        // will be in between sizeof(Intermediate) and sizeof(Parent)
        uintptr_t m_padding;

    public:
        // the constructor that constructs the fields.
        // we construct the Parent first,
        static void fieldConstructor(void* offsetField) {
            std::array<std::byte, sizeof(Parent)> parentContainer;

            auto parent = new (parentContainer.data()) Parent();

            parent->Intermediate::~Intermediate();

            std::memcpy(
                offsetField,
                std::launder(&parentContainer[sizeof(Intermediate)]),
                sizeof(Parent) - sizeof(Intermediate)
            );
        }

        static void fieldDestructor(void* offsetField) {
            std::array<std::byte, sizeof(Parent)> parentContainer;

            auto parent = new (parentContainer.data()) Intermediate();

            std::memcpy(
                std::launder(&parentContainer[sizeof(Intermediate)]),
                offsetField,
                sizeof(Parent) - sizeof(Intermediate)
            );

            static_cast<Parent*>(parent)->Parent::~Parent();
        }

        template <class = std::enable_if_t<true>>
        Parent* operator->() {
            // get the this pointer of the base
            // field intermediate is the first member of Modify
            // meaning we canget the base from ourself
            auto node = reinterpret_cast<Parent*>(reinterpret_cast<std::byte*>(this) - sizeof(Base));
            static_assert(sizeof(Base) == offsetof(Parent, m_fields), "offsetof not correct");

            // generating the container if it doesn't exist
            auto container = FieldContainer::from(node);

            // the index is global across all mods, so the
            // function is defined in the loader source
            static size_t index = getFieldIndexForClass(typeid(Base).hash_code());

            // the fields are actually offset from their original
            // offset, this is done to save on allocation and space
            auto offsetField = container->getField(index);
            if (!offsetField) {
                offsetField = container->setField(
                    index, sizeof(Parent) - sizeof(Intermediate), &FieldIntermediate::fieldDestructor
                );

                FieldIntermediate::fieldConstructor(offsetField);
            }

            return reinterpret_cast<Parent*>(
                reinterpret_cast<std::byte*>(offsetField) - sizeof(Intermediate)
            );
        }
    };

}
