#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "vgpi-api.h"

static void vgpi_set_type(unsigned char * data, uint8_t type);
static void vgpi_set_len(unsigned char * data, int len);

uint8_t vgpi_handshake[] = {'P', 'E', 'R', 'P', 0, 1, 0, 0};

/**
*  Считать сырое uint8
**/
int8_t vgpi_read_uint8_raw(unsigned char * data)
{
        uint8_t ret = 0;

        ret |= data[0];
        return ret;
}

/**
*  Записать сырое uint8
**/
int8_t vgpi_write_uint8_raw(unsigned char * data, uint8_t value)
{
        data[0] = value;
}

/**
*  Считать сырое uint16
**/
int vgpi_read_uint16_raw(unsigned char * data)
{
        unsigned int ret = 0;

        ret |= data[0];
        ret <<= 8;
        ret |= data[1];

        return ret;
}

/**
*  Записать сырое uint16
**/
void vgpi_write_uint16_raw(unsigned char * data, uint16_t value)
{
        data[1] = value & 0xFF;
        value >>= 8;
        data[0] = value & 0xFF;
}

/**
*  Считать сырое int32
**/
int vgpi_read_int32_raw(unsigned char * data)
{
        int ret = 0;

        ret |= data[0];
        ret <<= 8;
        ret |= data[1];
        ret <<= 8;
        ret |= data[2];
        ret <<= 8;
        ret |= data[3];

        return ret;
}

/**
*  Записать сырое int32
**/
void vgpi_write_int32_raw(unsigned char * data, int value)
{
        data[3] = value & 0xFF;
        value >>= 8;
        data[2] = value & 0xFF;
        value >>= 8;
        data[1] = value & 0xFF;
        value >>= 8;
        data[0] = value & 0xFF;
}

/**
*  Считать поле uint16
**/
int vgpi_read_uint16(unsigned char * data)
{
        return vgpi_read_uint16_raw(data+1);
}

/**
*  Записать поле uint16
**/
int vgpi_write_uint16(unsigned char * data, uint16_t value)
{
        vgpi_set_type(data, VGPI_FIELD_UINT16);
        vgpi_write_uint16_raw(data + VGPI_FIELD_OFFSET_FIXED, value);
        return VGPI_LEN_UINT16;
}

/**
*  Считать поле int32
**/
int vgpi_read_int32(unsigned char * data)
{
        return vgpi_read_int32_raw(data+VGPI_FIELD_OFFSET_FIXED);
}

/**
*  Записать поле int32
**/
int vgpi_write_int32(unsigned char * data, int value)
{
        vgpi_set_type(data, VGPI_FIELD_INT32);
        vgpi_write_int32_raw(data + VGPI_FIELD_OFFSET_FIXED, value);
        return VGPI_LEN_INT32;
}

/**
*  Считать поле string
**/
char * vgpi_read_string(unsigned char * data)
{
        char * ret;
        int len;

        len = vgpi_read_uint16_raw(data+1);
        ret = malloc(len+1);
        assert(ret);
        memcpy(ret, data+VGPI_FIELD_OFFSET_VAR, len);
        ret[len] = '\0';
        return ret;
}

/**
*  Записать поле string
**/
int vgpi_write_string(unsigned char * data, const char * value)
{
        int len = strlen(value);

        vgpi_set_type(data, VGPI_FIELD_STRING);
        memcpy(data + VGPI_FIELD_OFFSET_VAR, value, len);
        vgpi_set_len(data, len);
        return len+VGPI_FIELD_OFFSET_VAR;
}

/**
*  Считать поле binary
**/
int vgpi_read_binary(unsigned char * data, char ** out_data)
{
        int len;

        len = vgpi_read_uint16_raw(data+1);
        *out_data = malloc(len);
        assert(*out_data);
        memcpy(*out_data, data+VGPI_FIELD_OFFSET_VAR, len);
        return len;
}

/**
*  Записать поле binary
**/
int vgpi_write_binary(unsigned char * data, const char * value, int len)
{
        vgpi_set_type(data, VGPI_FIELD_STRING);
        memcpy(data + VGPI_FIELD_OFFSET_VAR, value, len);
        vgpi_set_len(data, len);
        return len+VGPI_FIELD_OFFSET_VAR;
}

/**
*  Записать заголовок поля sequence
**/
void vgpi_write_sequence_hdr(unsigned char * data, int len)
{
        vgpi_set_type(data, VGPI_FIELD_SEQUENCE);
        vgpi_set_len(data, len);
}

/**
*  Проверить handshake
**/
int vgpi_check_handshake(unsigned char * data)
{
        if(memcmp(data, vgpi_handshake, HANDSHAKE_SIZE)==0)
                return 1;
        else
                return 0;
}

/**
*  Считать размер пакета
**/
int vgpi_get_packet_size(unsigned char * data)
{
        return vgpi_read_int32_raw(data);
}

/**
*  Записать размер пакета
**/
void vgpi_set_packet_size(unsigned char * data, int len)
{
        vgpi_write_int32_raw(data, len);
}

/**
*  Считать тип пакета
**/
int vgpi_get_packet_type(unsigned char * data)
{
        return vgpi_read_uint8_raw(data+VGPI_TYPE_OFFSET);
}

/**
*  Записать тип пакета
**/
void vgpi_set_packet_type(unsigned char * data, int type)
{
        vgpi_write_uint8_raw(data+VGPI_TYPE_OFFSET, type);
}

/**
*  Считать тип поля
**/
int vgpi_get_type(unsigned char * data)
{
        int field_type;

        field_type = data[0];
        return field_type;

}

/**
*  Записать тип поля
**/
static void vgpi_set_type(unsigned char * data, uint8_t type)
{
        data[0] = type;
}

/**
*  Считать длину поля
**/
int vgpi_get_len(unsigned char * data)
{
        int ret = 0;
        switch(vgpi_get_type(data)){
        case VGPI_FIELD_UINT16:
                ret = VGPI_LEN_UINT16;
                break;
        case VGPI_FIELD_INT32:
                ret = VGPI_LEN_INT32;
                break;
        case VGPI_FIELD_STRING:
        case VGPI_FIELD_SEQUENCE:
                ret = vgpi_read_uint16_raw(data+1) + VGPI_FIELD_OFFSET_VAR;
                break;
        default:
                assert(0);
                break;
        }

        return ret;
}

/**
*  Записать длину поля
**/
static void vgpi_set_len(unsigned char * data, int len)
{
        vgpi_write_uint16_raw(data+1, len);
}

/**
*  Считать поле type_specifi
**/
uint16_t vgpi_get_type_specific(unsigned char * data)
{
        return vgpi_read_uint16_raw(data + VGPI_TYPE_SPECIFIC_OFFSET);
}

/**
*  Записать поле type_specifi
**/
void vgpi_set_type_specific(unsigned char * data, uint16_t value)
{
        vgpi_write_uint16_raw(data + VGPI_TYPE_SPECIFIC_OFFSET, value);
}