// Include MSVC internal RTTI headers
#include <vcruntime.h>
#include <rttidata.h>

#include <spdlog/spdlog.h>

#include "Module.hpp"
#include "RTTI.hpp"

namespace utility {
namespace rtti {
bool derives_from(const void* obj, const std::string& type_name) {
    if (obj == nullptr) {
        return false;
    }

    const auto locator = *(_s_RTTICompleteObjectLocator**)(*(uintptr_t*)obj - sizeof(void*));

    if (locator == nullptr) {
        return false;
    }

    const auto module_within = ::utility::get_module_within(locator);

    if (!module_within) {
        return false;
    }

    const auto module = (uintptr_t)*module_within;
    const auto class_hierarchy = (_s_RTTIClassHierarchyDescriptor*)(module + locator->pClassDescriptor);

    if (class_hierarchy == nullptr) {
        return false;
    }

    const auto base_classes = (_s_RTTIBaseClassArray*)(module + class_hierarchy->pBaseClassArray);

    if (base_classes == nullptr) {
        return false;
    }

    for (auto i = 0; i < class_hierarchy->numBaseClasses; ++i) {
        const auto desc_offset = base_classes->arrayOfBaseClassDescriptors[i];

        if (desc_offset == 0) {
            continue;
        }

        const auto desc = (_s_RTTIBaseClassDescriptor*)(module + desc_offset);

        if (desc == nullptr) {
            continue;
        }

        const auto ti = (std::type_info*)(module + desc->pTypeDescriptor);

        if (ti == nullptr) {
            continue;
        }

        if (ti->name() == type_name) {
            return true;
        }
    }

    return false;
}
}
}