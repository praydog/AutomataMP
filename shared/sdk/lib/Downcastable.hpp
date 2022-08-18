#pragma once

namespace sdk::lib {
class Downcastable {
public:
    virtual void* get_some_tls_shit() = 0;
    virtual void* get_some_tls_shit2() = 0;
};
}