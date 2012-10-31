#include <r2d2.hpp>

#include <sstream>
#include <boost/format.hpp>
#include <iostream>
#include <iomanip>

Message::Message(bool isDirect, bool requiresResponse) {
    this->isDirect_ = isDirect;
    this->requiresResponse_ = requiresResponse;
}

bool Message::requiresResponse() const {
    return this->requiresResponse_;
}

bool Message::isDirect() const {
    return this->isDirect_;
}

void Message::add_u8(uint8_t v) {
    this->sstream_ << v;
}

void Message::add_s8(int8_t v) {
    this->sstream_ << v;
}

void Message::add_string(size_t n_bytes, const std::string& v) {
    this->sstream_ << std::setfill(static_cast<char>(0x00)) << std::left << std::setw(n_bytes) << v;
}

std::string Message::get_value() {
    return this->sstream_.str();
}

Motor::Motor(NXT *nxt, uint8_t port) {
    this->nxt_ = nxt;
    this->port_ = port;
}

void Motor::setForward(uint8_t power) {
    Message msg(true, false);
    msg.add_u8(Message::START_MOTOR);
    msg.add_u8(this->port_);
    msg.add_s8(power);

    msg.add_u8(0x01 | 0x04); // UNKNOWN
    msg.add_u8(0x01); // UNKNOWN
    msg.add_u8(0x00); // UNKNOWN
    msg.add_u8(0x20); // UNKNOWN
    msg.add_u8(0x00); // UNKNOWN
    msg.add_u8(0x00); // UNKNOWN
    msg.add_u8(0x00); // UNKNOWN
    msg.add_u8(0x00); // UNKNOWN

    std::string out = msg.get_value();

    this->nxt_->sendDirectCommand( false, (int8_t *)out.c_str(), out.size(), NULL, 0);
}

void Motor::setReverse(uint8_t power) {
    Message msg(true, false);
    msg.add_u8(Message::START_MOTOR);
    msg.add_u8(this->port_);
    msg.add_s8(-power);

    msg.add_u8(0x01 | 0x04); // UNKNOWN
    msg.add_u8(0x01); // UNKNOWN
    msg.add_u8(0x00); // UNKNOWN
    msg.add_u8(0x20); // UNKNOWN
    msg.add_u8(0x00); // UNKNOWN
    msg.add_u8(0x00); // UNKNOWN
    msg.add_u8(0x00); // UNKNOWN
    msg.add_u8(0x00); // UNKNOWN

    std::string out = msg.get_value();

    this->nxt_->sendDirectCommand( false, (int8_t *)out.c_str(), out.size(), NULL, 0);
}

void Motor::stop(bool brake) {
    Message msg(true, false);
    msg.add_u8(Message::STOP_MOTOR);
    msg.add_u8(this->port_);
    msg.add_s8(0); // power 0?

    if (brake) {
        msg.add_u8(0x01 | 0x02 | 0x04); // UNKNOWN
        msg.add_u8(0x01); // UNKNOWN
        msg.add_u8(0x00); // UNKNOWN
        msg.add_u8(0x20); // UNKNOWN
    } else {
        msg.add_u8(0x00); // UNKNOWN
        msg.add_u8(0x00); // UNKNOWN
        msg.add_u8(0x00); // UNKNOWN
        msg.add_u8(0x00); // UNKNOWN
    }

    msg.add_u8(0x00); // UNKNOWN
    msg.add_u8(0x00); // UNKNOWN
    msg.add_u8(0x00); // UNKNOWN
    msg.add_u8(0x00); // UNKNOWN

    std::string out = msg.get_value();

    this->nxt_->sendDirectCommand( false, (int8_t *)out.c_str(), out.size(), NULL, 0);
}

Sensor::Sensor(NXT *nxt, uint8_t port) {
    this->nxt_ = nxt;
    this->port_ = port;
}

int Sensor::getValue() {
    Message msg(true, true);
    msg.add_u8(Message::GET_VALUE);
    msg.add_u8(this->port_);

    std::string out = msg.get_value();

    uint8_t responseBuffer[16];

    memset(responseBuffer, 1, sizeof(responseBuffer));

    this->nxt_->sendDirectCommand(true, (int8_t *)out.c_str(), out.size(), responseBuffer, sizeof(responseBuffer));

    return responseBuffer[13] * 256 + responseBuffer[12];
}

SonarSensor::SonarSensor(NXT *nxt, uint8_t port) : Sensor(nxt, port) {
    this->nxt_ = nxt;
    this->port_ = port;
}

int SonarSensor::lsGetStatus(uint8_t *outbuf) {
    Message msg(true, true);
    msg.add_u8(Message::LS_GET_STATUS);
    msg.add_u8(this->port_);

    std::string out = msg.get_value();

    uint8_t responseBuffer[4];

    memset(responseBuffer, 1, sizeof(responseBuffer));

    this->nxt_->sendDirectCommand(true, (int8_t *)out.c_str(), out.size(), responseBuffer, sizeof(responseBuffer));

    std::copy(responseBuffer, responseBuffer + sizeof(responseBuffer), outbuf);

    return static_cast<int>(responseBuffer[3]);
}

void SonarSensor::lsRead(uint8_t *outbuf) {
    Message msg(true, true);
    msg.add_u8(Message::LS_READ);
    msg.add_u8(this->port_);

    std::string out = msg.get_value();

    uint8_t responseBuffer[20];

    memset(responseBuffer, 1, sizeof(responseBuffer));

    this->nxt_->sendDirectCommand(true, (int8_t *)out.c_str(), out.size(), responseBuffer, sizeof(responseBuffer));

    std::copy(responseBuffer, responseBuffer + sizeof(responseBuffer), outbuf);
}

void SonarSensor::lsWrite(const std::string& indata, uint8_t *outBuf, size_t outSize) {
    Message msg(true, true);
    msg.add_u8(Message::LS_WRITE);
    msg.add_u8(this->port_);
    msg.add_u8(indata.size());
    msg.add_u8(outSize);
    msg.add_string(indata.size(), indata);

    std::string tosend = msg.get_value();

    uint8_t responseBuffer[3];

    memset(responseBuffer, 1, sizeof(responseBuffer));

    this->nxt_->sendDirectCommand(true, (int8_t *)(tosend.c_str()), tosend.size(), responseBuffer, sizeof(responseBuffer));

    std::memcpy(outBuf, responseBuffer, outSize * sizeof(uint8_t));
}


int SonarSensor::getSonarValue() {
    Message msg(true, true);
    msg.add_u8(0x02); // I2C_DEV
    msg.add_u8(0x41); // COMMAND
    msg.add_u8(0x02); // CONTINUOUS

    std::string out = msg.get_value();

    uint8_t outbuf2[66];
    memset(outbuf2, 1, sizeof(outbuf2));
    this->lsWrite(out, outbuf2, 0x00);

    while (1) {
        Message msg(true, true);
        msg.add_u8(0x02); // I2C_DEV
        msg.add_u8(0x42); // READ VALUE FROM I2C
        std::string out2 = msg.get_value();

        uint8_t outbuf3[66];
        memset(outbuf3, 1, sizeof(outbuf3));
        this->lsWrite(out2, outbuf3, 1);

        uint8_t outbuf4[4];
        memset(outbuf4, 1, sizeof(outbuf4));
        this->lsGetStatus(outbuf4);
        if (outbuf4[3] >= 1) { // 1 bytes
            break;
        }
    }

    uint8_t outbuf5[20];
    memset(outbuf5, 1, sizeof(outbuf5));
    this->lsRead(outbuf5);
    if (outbuf5[2] == 0) {
        return outbuf5[4];
    } else {
        return -1;
    }
}

BTComm::BTComm(struct sockaddr_rc addr) {
    this->addr_ = addr;
}

bool BTComm::open() {
    this->sock_ = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    int status = connect(this->sock_, (struct sockaddr *)&(this->addr_), sizeof(this->addr_));
    return (status == 0);
}

void BTComm::devWrite(uint8_t * buf, int buf_size) {
    uint8_t bf = buf_size;
    uint8_t header[] = {bf, 0x00};
    uint8_t outBuf[2 + buf_size];
    memcpy(outBuf, header, sizeof(header));
    memcpy(outBuf + 2, buf, buf_size);
    write(this->sock_, outBuf, sizeof(outBuf));
}

void BTComm::devRead(uint8_t * buf, int buf_size) {
    char reply[64];
    memset(reply, 0, sizeof(reply));

    // read data from the client
    int bytes_read = read(this->sock_, reply, 2);

    if ( bytes_read > 0 ) {
        int replylength = reply[0] + (reply[1] * 256);
        bytes_read = read(this->sock_, reply, replylength);
        if (bytes_read == replylength) {
            memcpy(buf, reply, buf_size);
        }
    }
}

bool USBComm::open() {

    int nEp = 0;

    if (this->usb_dev_ == 0) {
        std::cerr << "Device not found!" << std::endl;
        return false;
    }

    int r = libusb_open(this->usb_dev_, &(this->pUSBHandle_));

    if (r != 0) {
        std::cerr << "Not able to claim the USB device" << std::endl;
        return false;
    } else {
        libusb_config_descriptor *config;
        libusb_get_config_descriptor(this->usb_dev_, 0, &config);
        if (config->interface) {
            if (config->interface->altsetting) {
                r = libusb_claim_interface(this->pUSBHandle_, config->interface->altsetting->bInterfaceNumber);
                if ((nEp = config->interface->altsetting->bNumEndpoints)) {
                    if (config->interface->altsetting->endpoint) {
                        this->ucEpIn_ = (config->interface->altsetting->endpoint[0].bEndpointAddress);
                        if (nEp > 1) {
                            this->ucEpOut_ = (config->interface->altsetting->endpoint[1].bEndpointAddress);
                        }

                    }
                }
            }
        }
    }
    return true;
}

void USBComm::devWrite(uint8_t * buf, int buf_size) {
    boost::mutex::scoped_lock lock(this->io_mutex);
    if (this->pUSBHandle_) {
        int actual_length;
        int r = libusb_bulk_transfer(this->pUSBHandle_, this->ucEpIn_, buf, buf_size, &actual_length, TIMEOUT);
        if (r == 0 && actual_length == buf_size) {
        } else {
            std::cerr << "ERROR WRITING" << std::endl;
            std::cerr << "READ: " << actual_length << std::endl;
            std::cerr << "BUF LEN: " << buf_size << std::endl;
            std::cerr << "RES: " << r << std::endl;
        }
    }
}

void USBComm::devRead(uint8_t * buf, int buf_size) {
    boost::mutex::scoped_lock lock(this->io_mutex);
    if (this->pUSBHandle_) {
        int actual_length;
        int r = libusb_bulk_transfer(this->pUSBHandle_, this->ucEpOut_, buf, buf_size, &actual_length, TIMEOUT);
        if (r == 0 && actual_length <= buf_size) {
        } else {
            std::cerr << "ERROR READING" << std::endl;
            std::cerr << "READ: " << actual_length << std::endl;
            std::cerr << "BUF LEN: " << buf_size << std::endl;
            std::cerr << "RES: " << r << std::endl;
        }
    }
}

USBComm::USBComm(libusb_context *ctx, libusb_device *usb_dev) {
    this->ctx_ = ctx;
    this->usb_dev_ = usb_dev;
}

USBComm::~USBComm() {
    libusb_unref_device(this->usb_dev_);
    libusb_close(this->pUSBHandle_);
}


Sensor* NXT::makeTouch(uint8_t port) {
    Message msg(true, false);
    msg.add_u8(Message::SET_INPUT_MODE);
    msg.add_u8(port);
    msg.add_u8(Message::TOUCH);
    msg.add_u8(Mode::BOOLEAN);

    std::string out = msg.get_value();
    this->sendDirectCommand( false, (int8_t *)out.c_str(), out.size(), NULL, 0);
    return new Sensor(this, port);
}

SonarSensor* NXT::makeSonar(uint8_t port) {
    // 0x05 -> setInputMode
    // 0x0B -> LOWSPEED_9V
    // 0x00 -> RAW
    Message msg(true, false);
    msg.add_u8(Message::SET_INPUT_MODE);
    msg.add_u8(port);
    msg.add_u8(Message::LOWSPEED_9V);
    msg.add_u8(Mode::RAW);

    std::string out = msg.get_value();
    this->sendDirectCommand( false, (int8_t *)out.c_str(), out.size(), NULL, 0);
    return new SonarSensor(this, port);
}

Motor* NXT::makeMotor(uint8_t port) {
    return new Motor(this, port);
}

bool NXT::open() {
    return this->comm_->open();
}

bool NXT::isHalted() const {
    return this->halted;
}

void NXT::halt() {
    for (uint8_t port = 0; port < 3; ++port) {
        this->makeMotor(port)->stop(false);
    }
    this->halted = true;
}

double NXT::getFirmwareVersion() {
    uint8_t outBuf[7];
    const int min = 4, maj = 5;

    Message msg(false, true);
    msg.add_u8(0x88);
    std::string data = msg.get_value();

    // Send the system command to the NXT.
    this->sendSystemCommand(true, (int8_t *)data.c_str(), data.size(), outBuf, sizeof(outBuf));

    double version = outBuf[min];

    while (version >= 1)
        version /= 10;
    version += outBuf[maj];

    return version;
}

void NXT::getDeviceInfo(uint8_t * outBuf, size_t size) {
    //uint8_t inBuf[] = { 0x01, 0x9B };
    int8_t inBuf[] = { static_cast<int8_t>(0x9B) };

    // Send the system command to the NXT.
    this->sendSystemCommand(true, inBuf, sizeof(inBuf), outBuf, size);
}

std::string NXT::getName() {
    uint8_t outBuf[33];
    char name[15];

    name[0] = '\0';

    this->getDeviceInfo(outBuf, sizeof(outBuf));

    std::memcpy(name, outBuf + 2, 15 * sizeof(uint8_t));

    name[15] = '\0';

    return std::string(name);
}

NXT::NXT(Comm *comm) {
    this->comm_ = comm;
    this->halted = false;
}

void NXT::sendSystemCommand(bool response, int8_t * dc_buf,
                            size_t dc_buf_size, uint8_t * re_buf, size_t re_buf_size) {
    uint8_t buf[dc_buf_size + 1];

    std::copy(dc_buf, dc_buf + dc_buf_size, buf + 1);

    buf[0] = response ? 0x01 : 0x81;

    this->comm_->devWrite(buf, dc_buf_size + 1);

    if (response) {
        uint8_t tempreadbuf[re_buf_size];
        this->comm_->devRead(tempreadbuf, re_buf_size);

        std::copy(tempreadbuf + 1, tempreadbuf + re_buf_size, re_buf);
    }
}

void NXT::submitDirectCommand(Command *command) {
}

void NXT::sendDirectCommand(bool response, int8_t * dc_buf,
                            size_t dc_buf_size, uint8_t * re_buf, size_t re_buf_size) {
    uint8_t buf[dc_buf_size + 1];

    std::copy(dc_buf, dc_buf + dc_buf_size, buf + 1);

    buf[0] = response ? 0x00 : 0x80;

    this->comm_->devWrite(buf, dc_buf_size + 1);

    if (response) {
        this->comm_->devRead(re_buf, re_buf_size);
    }
}

std::vector<NXT *>* USBNXTManager::list() {

    // List all the NXT devices
    std::vector<NXT*>* v = new std::vector<NXT*>();

    libusb_device **devs;
    libusb_context *ctx = NULL;
    libusb_device *dev;

    int r;
    ssize_t cnt; //holding number of devices in list

    r = libusb_init(&ctx);

    if (r < 0) {
        std::cout<<"Init Error "<<r<<std::endl; //there was an error
        return v;
    }

    libusb_set_debug(ctx, 3); //set verbosity level to 3, as suggested in the documentation

    cnt = libusb_get_device_list(ctx, &devs); //get the list of devices
    if (cnt < 0) {
        std::cout<<"Get Device Error"<<std::endl; //there was an error
        return v;
    }

    std::cout<<cnt<<" Devices in list."<<std::endl; //print total number of usb devices
    ssize_t i; //for iterating through the list
    for (i = 0; i < cnt; i++) {
        libusb_device_descriptor desc;
        int r = libusb_get_device_descriptor(devs[i], &desc);
        if (r < 0) {
            std::cout<<"failed to get device descriptor"<<std::endl;
            continue;
        }


        if (desc.idVendor == NXT_VENDOR_ID && desc.idProduct == NXT_PRODUCT_ID) {
            libusb_device_handle *devh = NULL;
            libusb_open(devs[i], &devh);
            libusb_reset_device(devh);
            libusb_close(devh);
        }
    }


    cnt = libusb_get_device_list(ctx, &devs); //get the list of devices
    if (cnt < 0) {
        std::cout<<"Get Device Error"<<std::endl; //there was an error
        return v;
    }

    std::cout<<cnt<<" Devices in list."<<std::endl; //print total number of usb devices
    for (i = 0; i < cnt; i++) {
        libusb_device_descriptor desc;
        int r = libusb_get_device_descriptor(devs[i], &desc);
        if (r < 0) {
            std::cout<<"failed to get device descriptor"<<std::endl;
            continue;
        }

        if (desc.idVendor == NXT_VENDOR_ID && desc.idProduct == NXT_PRODUCT_ID) {
            dev = libusb_ref_device(devs[i]);
            USBComm *comm = new USBComm(ctx, dev);
            NXT *nxt = new NXT(comm);
            v->push_back(nxt);
        }
    }
    libusb_free_device_list(devs, 1); //free the list, unref the devices in it
//        libusb_exit(ctx); //close the session
    return v;
}

std::vector<NXT *>* BTNXTManager::list() {

    // List all the NXT devices
    std::vector<NXT*>* v = new std::vector<NXT*>();

    inquiry_info *ii = NULL;
    int max_rsp, num_rsp;
    int dev_id, sock, len, flags;
    int i;

    dev_id = hci_get_route(NULL);
    sock = hci_open_dev( dev_id );
    if (dev_id < 0 || sock < 0) {
        perror("opening socket");
        exit(1);
    }

    len  = 8;
    max_rsp = 255;
    flags = IREQ_CACHE_FLUSH;
    ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info));

    num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
    if ( num_rsp < 0 ) perror("hci_inquiry");

    for (i = 0; i < num_rsp; i++) {
        bdaddr_t *ba = &(ii+i)->bdaddr;

        if (ba->b[5] == 0x00 && ba->b[4] == 0x16 && ba->b[3] == 0x53) {
            struct sockaddr_rc addr;
            // set the connection parameters (who to connect to)
            addr.rc_family = AF_BLUETOOTH;
            addr.rc_channel = (uint8_t) 1;
            memcpy(&(addr.rc_bdaddr), ba, sizeof(bdaddr_t));

            BTComm *comm = new BTComm(addr);
            NXT *nxt = new NXT(comm);
            v->push_back(nxt);
        }
    }

    free( ii );
    close( sock );
    return v;
}
