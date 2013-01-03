#include <r2d2.hpp>
#include <r2d2/comm.hpp>

bool Comm::open() {
    return this->transport_->open();
}

void Comm::sendSystemCommand(bool response, int8_t * dc_buf,
                            size_t dc_buf_size, uint8_t * re_buf, size_t re_buf_size) {
    uint8_t buf[dc_buf_size + 1];

    std::copy(dc_buf, dc_buf + dc_buf_size, buf + 1);

    buf[0] = response ? 0x01 : 0x81;

    this->transport_->devWrite(buf, dc_buf_size + 1);

    if (response) {
        unsigned char tempreadbuf[re_buf_size];
        this->transport_->devRead(tempreadbuf, re_buf_size);

        std::copy(tempreadbuf + 1, tempreadbuf + re_buf_size, re_buf);
    }
}

void Comm::sendDirectCommand(bool response, int8_t * dc_buf,
                            size_t dc_buf_size, unsigned char * re_buf, size_t re_buf_size) {
    uint8_t buf[dc_buf_size + 1];

    std::copy(dc_buf, dc_buf + dc_buf_size, buf + 1);

    buf[0] = response ? 0x00 : 0x80;

    this->transport_->devWrite(buf, dc_buf_size + 1);

    if (response) {
        this->transport_->devRead(re_buf, re_buf_size);
    }
}
