#include "VtableHook.hpp"

using namespace std;

VtableHook::VtableHook()
    : m_rawData{},
    m_vtablePtr(),
    m_newVtable(nullptr),
    m_oldVtable(),
    m_vtableSize(0)
{}

VtableHook::VtableHook(Address target)
    : VtableHook()
{
    create(target);
}

VtableHook::VtableHook(VtableHook&& other)
    : m_rawData(move(other.m_rawData)),
    m_vtablePtr(other.m_vtablePtr),
    m_newVtable(other.m_newVtable),
    m_oldVtable(other.m_oldVtable),
    m_vtableSize(other.m_vtableSize)
{
    other.m_vtablePtr = nullptr;
    other.m_newVtable = nullptr;
    other.m_oldVtable = nullptr;
    other.m_vtableSize = 0;
}

VtableHook::~VtableHook() {
    remove();
}

bool VtableHook::create(Address target) {
    if (!m_rawData.empty()) {
        remove();
        m_rawData.clear();
    }

    m_vtablePtr = target;
    m_oldVtable = m_vtablePtr.to<Address>();
    m_vtableSize = getVtableSize(m_oldVtable);
    // RTTI.
    m_rawData.resize(m_vtableSize + 1);
    m_newVtable = m_rawData.data() + 1;

    memcpy(m_rawData.data(), m_oldVtable.as<Address*>() - 1, sizeof(Address) * (m_vtableSize + 1));

    // At this point we have the address of the old vtable, and a copy of it
    // stored in m_newVtable.  Set the target objects vtable
    // pointer to our copy of the original.
    *m_vtablePtr.as<Address*>() = m_newVtable;

    return true;
}

bool VtableHook::recreate() {
    if (m_vtablePtr != nullptr) {
        *m_vtablePtr.as<Address*>() = m_newVtable;
        return true;
    }

    return false;
}

bool VtableHook::remove() {
    // Can cause issues where we set the vtable/random memory of some other pointer.
    if (m_vtablePtr != nullptr && IsBadReadPtr(m_vtablePtr.ptr(), sizeof(void*)) == FALSE && m_vtablePtr.to<void*>() == m_newVtable) {
        *m_vtablePtr.as<Address*>() = m_oldVtable;
        return true;
    }

    return false;
}

bool VtableHook::hookMethod(uint32_t index, Address newMethod) {
    if (m_oldVtable != nullptr && m_newVtable != nullptr && index < m_vtableSize) {
        m_newVtable[index] = newMethod;
        return true;
    }

    return false;
}

size_t VtableHook::getVtableSize(Address vtable) {
    size_t i = 0;

    for (; vtable.as<Address*>()[i] != nullptr; ++i) {
        if (IsBadCodePtr(vtable.as<FARPROC*>()[i]) == TRUE) {
            break;
        }
    }

    return i;
}
