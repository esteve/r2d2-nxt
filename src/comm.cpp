#include <r2d2.hpp>
#include <r2d2/comm.hpp>

#include <iostream>

bool Comm::open() {
    return this->transport_->open();
}

void Comm::sendMessage(Message &msg, uint8_t * re_buf, size_t re_buf_size) {
    std::string out = msg.get_value();

    this->transport_->devWrite((uint8_t *)out.c_str(), out.size());

    if (msg.requiresResponse()) {
        this->transport_->devRead(re_buf, re_buf_size);
    }
}
