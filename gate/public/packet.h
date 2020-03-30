#ifndef PACKET_INCLUDE_H
#define PACKET_INCLUDE_H

#include <stdint.h>
#include "global.h"

#define SESSIONSZ  32
#define AESKEYSZ   16
#define SERVIDSZ   50

#define HEADER_SIGN          (0x5F5F)
#define PROTO_BUF_SIGN       (0xF5F5F5F5)

namespace packet {

#pragma pack(1)

	//@@ enctypeE ��������
	enum enctypeE {
		PUBENC_JSON_NONE         = 0x01,
		PUBENC_PROTOBUF_NONE     = 0x02,
		PUBENC_JSON_BIT_MASK     = 0x11,
		PUBENC_PROTOBUF_BIT_MASK = 0x12,
		PUBENC_JSON_RSA          = 0x21,
		PUBENC_PROTOBUF_RSA      = 0x22,
		PUBENC_JSON_AES          = 0x31,
		PUBENC_PROTOBUF_AES      = 0x32,
	};

	//@@ header_t ���ݰ�ͷ
	struct header_t {
		uint16_t len;      //���ܳ���
		uint16_t crc;      //CRCУ��λ
		uint16_t ver;      //�汾��
		uint16_t sign;     //ǩ��
		uint8_t  mainID;   //����ϢmainID
		uint8_t  subID;    //����ϢsubID
		uint8_t  enctype;  //��������
		uint8_t  reserved; //Ԥ��
		uint32_t reqID;
		uint16_t realsize; //�û����ݳ���
	};

	//@@ internal_prev_header_t ���ݰ�ͷ(�ڲ�ʹ��)
	struct internal_prev_header_t {
		uint16_t len;
		int16_t  kicking;
		int32_t  ok;
		int64_t  userID;
		uint32_t ipaddr;             //��ʵIP
		uint8_t  session[SESSIONSZ]; //�û��Ự
		uint8_t  aesKey[AESKEYSZ];   //AES_KEY
		uint8_t  servID[SERVIDSZ];   //���Խڵ�ID
		uint16_t checksum;           //У���CHKSUM
	};

#pragma pack()

	//@@
	static const size_t kHeaderLen = sizeof(header_t);
	static const size_t kPrevHeaderLen = sizeof(internal_prev_header_t);
	static const size_t kMaxPacketSZ = 60 * 1024;
	static const size_t kMinPacketSZ = sizeof(int16_t);

	//getCheckSum ����У���
	static uint16_t getCheckSum(uint8_t const* header, size_t size) {
		uint16_t sum = 0;
		uint16_t const* ptr = (uint16_t const*)header;
		for (size_t i = 0; i < size / 2; ++i) {
			//��ȡuint16��2�ֽ�
			sum += *ptr++;
		}
		if (size % 2) {
			//��ȡuint8��1�ֽ�
			sum += *(uint8_t const*)ptr;
		}
		return sum;
	}

	//setCheckSum ����У���
	static void setCheckSum(internal_prev_header_t* header) {
		uint16_t sum = 0;
		uint16_t* ptr = (uint16_t*)header;
		for (size_t i = 0; i < kPrevHeaderLen / 2 - 1; ++i) {
			//��ȡuint16��2�ֽ�
			sum += *ptr++;
		}
		//CRCУ��λ
		*ptr = sum;
	}

	//checkCheckSum ����У���
	static bool checkCheckSum(internal_prev_header_t const* header) {
		uint16_t sum = 0;
		uint16_t const* ptr = (uint16_t const*)header;
		for (size_t i = 0; i < kPrevHeaderLen / 2 - 1; ++i) {
			//��ȡuint16��2�ֽ�
			sum += *ptr++;
		}
		//У��CRC
		return *ptr == sum;
	}

}//namespace packet

#endif