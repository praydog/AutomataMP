#pragma once

#include <cstdint>
#include <vector>

#include <Windows.h>

#include "Address.hpp"

class VtableHook {
public:
    VtableHook();
    VtableHook(Address target);
    VtableHook(const VtableHook& other) = delete;
    VtableHook(VtableHook&& other);

    virtual ~VtableHook();

    bool create(Address target);
    bool recreate();
    bool remove();

    bool hookMethod(uint32_t index, Address newMethod);

    auto getInstance() {
        return m_vtablePtr;
    }

    // Access to original methods.
    Address getMethod(uint32_t index) {
        if (index < m_vtableSize && m_oldVtable && m_newVtable) {
            return m_oldVtable.as<Address*>()[index];
        }
        else {
            return nullptr;
        }
    }

    template <typename T>
    T getMethod(uint32_t index) {
        return (T)getMethod(index).ptr();
    }

private:
    std::vector<Address> m_rawData;
    Address m_vtablePtr;
    Address* m_newVtable;
    Address m_oldVtable;
    size_t m_vtableSize;

    size_t getVtableSize(Address vtable);
};