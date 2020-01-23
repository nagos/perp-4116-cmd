#ifndef VGPI_API
#define VGPI_API

#ifdef __cplusplus
extern "C" {
#endif

#define VGPI_PORT 10010
#define HEADER_SIZE 8
#define HANDSHAKE_SIZE 8
#define VGPI_TYPE_OFFSET 5

#define VGPI_FIELD_UINT16 1
#define VGPI_FIELD_INT32 2
#define VGPI_FIELD_STRING 3
#define VGPI_FIELD_SEQUENCE 4
#define VGPI_FIELD_BIN 5

#define VGPI_LEN_UINT16 3
#define VGPI_LEN_INT32 5

#define VGPI_FIELD_OFFSET_FIXED 1
#define VGPI_FIELD_OFFSET_VAR 3
#define VGPI_TYPE_SPECIFIC_OFFSET 6

#define VGPI_SEQUENCE_HDR VGPI_FIELD_OFFSET_VAR

extern uint8_t vgpi_handshake[];

int vgpi_read_uint16_raw(unsigned char * data);

int vgpi_read_uint16(unsigned char * data);
int vgpi_write_uint16(unsigned char * data, uint16_t value);
int vgpi_read_int32(unsigned char * data);
int vgpi_write_int32(unsigned char * data, int value);
char * vgpi_read_string(unsigned char * data);
int vgpi_write_string(unsigned char * data, const char * value);
int vgpi_read_binary(unsigned char * data, char ** out_data);
int vgpi_write_binary(unsigned char * data, const char * value, int len);
void vgpi_write_sequence_hdr(unsigned char * data, int len);

int vgpi_check_handshake(unsigned char * data);
int vgpi_get_packet_size(unsigned char * data);
void vgpi_set_packet_size(unsigned char * data, int len);
int vgpi_get_packet_type(unsigned char * data);
void vgpi_set_packet_type(unsigned char * data, int type);

int vgpi_get_type(unsigned char * data);
int vgpi_get_len(unsigned char * data);

uint16_t vgpi_get_type_specific(unsigned char * data);
void vgpi_set_type_specific(unsigned char * data, uint16_t value);

#ifdef __cplusplus
}
#endif
#endif
