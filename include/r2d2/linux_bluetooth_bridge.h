#ifndef R2D2_BLUETOOTH_BRIDGE_H
#define R2D2_BLUETOOTH_BRIDGE_H

#ifdef __cplusplus
extern "C" {
#endif
int r2d2_bt_create_socket();

int r2d2_bt_connect_socket(int sock, void *addr);

void r2d2_bt_scan(void (*f)(void *addr, void *arg), void *arg);
#ifdef __cplusplus
}
#endif

#endif
