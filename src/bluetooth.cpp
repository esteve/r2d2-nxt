#include <r2d2.hpp>
#include <r2d2/bluetooth.hpp>

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
