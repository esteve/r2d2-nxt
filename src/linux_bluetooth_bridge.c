#include <stdlib.h>
#include <unistd.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#include <r2d2/linux_bluetooth_bridge.h>

int r2d2_bt_create_socket() {
    return socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
}

int r2d2_bt_connect_socket(int sock, void *addr) {
    return connect(sock, (struct sockaddr *)(addr), sizeof(struct sockaddr_rc));
}

void r2d2_bt_scan(void (*f)(void *, void *), void *arg) {
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
            struct sockaddr_rc *addr = malloc(sizeof(struct sockaddr_rc));
            // set the connection parameters (who to connect to)
            addr->rc_family = AF_BLUETOOTH;
            addr->rc_channel = (uint8_t) 1;
            memcpy(&(addr->rc_bdaddr), ba, sizeof(bdaddr_t));

            f((void *)addr, arg);
        }
    }

    free( ii );
    close( sock );
}
