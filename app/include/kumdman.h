#pragma once

extern char outtxt[0x12];

typedef struct {
    unsigned char peripheral_device_type;
    unsigned char removable;
    unsigned char standard_ver;
    unsigned char atapi_response;
    unsigned int additional;
    char vendor_id[8];
    char product_id[16];
    char product_rev[4];
    char sony_spec[0x14];
} ATAPI_INQURIY;

extern ATAPI_INQURIY ai;
extern u8 buf[0x38];
extern u8 param[4];

void *pspUmdManGetUmdDrive(int driveNum);
int pspUmdExecInquiryCmd(void *drive, u8 *param, u8 *buf);
