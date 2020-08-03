#include <Windows.h>
#include "MyFunc.h"
#include "DEAPIProtocol.h"

INT GetDEAPIPacketStr(tagDEAPIPacketStr *DAPdest, TCHAR *src, INT cbSourceMAX)
{
	USHORT cnt = A2CtoHex(src[4], src[5]);
	USHORT length;
	length = cnt;
	cnt = A2CtoHex(src[2], src[3]);
	cnt <<= 8;
	length |= cnt;
	src[0] = DAPdest->stx[0];
	src[1] = DAPdest->stx[1];
	src[2] = DAPdest->lengthmsb[0];
	src[3] = DAPdest->lengthmsb[1];
	src[4] = DAPdest->lengthlsb[0];
	src[5] = DAPdest->lengthlsb[1];
	src[6] = DAPdest->apiID[0];
	src[7] = DAPdest->apiID[1];
	src[8] = DAPdest->cmd[0];
	src[9] = DAPdest->cmd[1];
	src[10] = DAPdest->status[0];
	src[11] = DAPdest->status[1];
	src[12] = DAPdest->frameID[0];
	src[13] = DAPdest->frameID[1];
	src[14] = DAPdest->optionH[0];
	src[15] = DAPdest->optionH[1];
	src[16] = DAPdest->optionL[0];
	src[17] = DAPdest->optionL[1];
	for (cnt = 0; cnt < (length-6); cnt++) {
		if ((18 + cnt) > cbSourceMAX) return -1;
		src[18 + cnt] = DAPdest->data[cnt];
	}
	DAPdest->data[cnt] = NULL;
	src[18 + cnt] = DAPdest->ckSum[0];
	src[19 + cnt] = DAPdest->ckSum[1];
	return 20 + cnt;
}

INT GetDEAPIPacket(tagDEAPIPacket *DAPdest, const TCHAR *src, INT cbSourceMAX)
{
	INT cnt;
	INT cnt2 = 0;
	USHORT valueShort1;
	USHORT valueShort2;
	DAPdest->stx = A2CtoHex(src[0], src[1]);
	if (DAPdest->stx != (TCHAR)0xde) return -1;
	valueShort1 = A2CtoHex(src[2], src[3]);	
	valueShort2 = A2CtoHex(src[4], src[5]);	
	valueShort1 <<= 8;
	valueShort1 |= valueShort2;
	DAPdest->length = valueShort1;
	DAPdest->apiID = A2CtoHex(src[6], src[7]);
	DAPdest->cmd = A2CtoHex(src[8], src[9]);
	DAPdest->status = A2CtoHex(src[10], src[11]);
	DAPdest->frameID = A2CtoHex(src[12], src[13]);
	DAPdest->optionH = A2CtoHex(src[14], src[15]);
	DAPdest->optionL = A2CtoHex(src[16], src[17]);
	if ((valueShort1 + 10) > cbSourceMAX) return -1;
	for (cnt = 0; cnt < (valueShort1 - 6); cnt++) {
		if ((18 + cnt) > cbSourceMAX) return -1;
		DAPdest->data[cnt] = A2CtoHex(src[18 + cnt2], src[19 + cnt2]);
		cnt2 += 2;
	}
	DAPdest->ckSum = A2CtoHex(src[18 + cnt2], src[19 + cnt2]);
	return 20 + cnt2;
}

INT GetMSGVerifiedDEAPIPacket(TCHAR *destMSG, const tagDEAPIPacket *DAPsrc, INT optionflag, INT cbMSGMAX)
{
	USHORT valueShort1;
	INT cnt;
	TCHAR buff[36] = "";
	TCHAR msgBuff[DEAPI_DATAMAXLENGTH] = "";
	if (cbMSGMAX < 50) return -1;
	if (DAPsrc->stx != (TCHAR)0xde) return -1;
	switch (optionflag) {
	case DEAPI_STX:
		if (DAPsrc->stx == (TCHAR)0xde) strcpy_s(destMSG, 5, "TRUE");
		else strcpy_s(destMSG, 6, "FALSE");
		break;
	/*case DEAPI_LENGTH:
		if (DAPsrc->length == (TCHAR)(lstrlen(DAPsrc->data) - 6)) strcpy_s(destMSG, 5, "TRUE");
		else strcpy_s(destMSG, 6, "FALSE");
		break;*/
	case DEAPI_APIID:
		if (DAPsrc->apiID == (TCHAR)0x10) strcpy_s(destMSG, 20, "485포트로 요청");
		else if (DAPsrc->apiID == (TCHAR)0x90) strcpy_s(destMSG, 20, "485포트로 응답");
		else if (DAPsrc->apiID == (TCHAR)0x98) strcpy_s(destMSG, 20, "로라서버로 전송");
		else strcpy_s(destMSG, 20, "FALSE");
		break;
	case DEAPI_CMD:
		if (DAPsrc->cmd == (TCHAR)0x31) strcpy_s(destMSG, 20, "모듈센서 1ch");
		else if (DAPsrc->cmd == (TCHAR)0x32) strcpy_s(destMSG, 20, "모듈센서 2ch");
		else if (DAPsrc->cmd == (TCHAR)0x45) strcpy_s(destMSG, 20, "환경센서");
		else if (DAPsrc->cmd == (TCHAR)0x49) strcpy_s(destMSG, 20, "인버터");
		else strcpy_s(destMSG, 20, "FALSE");
		break;
	case DEAPI_STATUS:		
		if (DAPsrc->status == (TCHAR)0x00) strcpy_s(destMSG, 20, "정상");
		else if (DAPsrc->status == (TCHAR)0x80) strcpy_s(destMSG, 20, "응답없음");
		else if (DAPsrc->status == (TCHAR)0x81) strcpy_s(destMSG, 20, "모드 셋팅 에러");
		else if (DAPsrc->status == (TCHAR)0x82) strcpy_s(destMSG, 25, "ZB 갯수 셋팅 에러");
		else if (DAPsrc->status == (TCHAR)0x83) strcpy_s(destMSG, 25, "인버터 누적발전량 값 에러");
		else strcpy_s(destMSG, 6, "FALSE");
		break;
	case DEAPI_FRAMEID:		
		if ((DAPsrc->cmd == (TCHAR)0x31 || DAPsrc->cmd == (TCHAR)0x32) && (DAPsrc->apiID == (TCHAR)0x10 || DAPsrc->apiID == (TCHAR)0x90))
			wsprintf(destMSG, "중계기ID_%d", DAPsrc->frameID);
		else if ((DAPsrc->cmd == (TCHAR)0x31 || DAPsrc->cmd == (TCHAR)0x32) && DAPsrc->apiID == (TCHAR)0x98)
			wsprintf(destMSG, "페이지번호_%d", DAPsrc->frameID);
		else if (DAPsrc->cmd == (TCHAR)0x49) wsprintf(destMSG, "인버터번호_%d", DAPsrc->frameID);
		else if (DAPsrc->cmd == (TCHAR)0x45) wsprintf(destMSG, "환경센서구분자_%d", DAPsrc->frameID);
		else strcpy_s(destMSG, 6, "-");
		break;
	case DEAPI_OPTIONH:
	case DEAPI_OPTIONL:		
		valueShort1 = H2ValuetoShort(DAPsrc->optionH, DAPsrc->optionL, 0);
		if ((DAPsrc->cmd == (TCHAR)0x31 || DAPsrc->cmd == (TCHAR)0x32)) wsprintf(destMSG, "모듈센서시작번호_%d", valueShort1);
		else if (DAPsrc->cmd == (TCHAR)0x49) wsprintf(destMSG, "인버터 Phase_%d", DAPsrc->optionL);
		else strcpy_s(destMSG, 6, "-");
		break;
	case DEAPI_DATA:
		if (DAPsrc->status != 0) { strcpy_s(destMSG, 6, "-"); break; }
		if ((DAPsrc->cmd == (TCHAR)0x31 || DAPsrc->cmd == (TCHAR)0x32)) {
			for (cnt = 0; cnt < (DAPsrc->length-6); cnt++) {				
				wsprintf(buff, "%02d ", DAPsrc->data[cnt]);
				strcat_s(msgBuff, buff);
			}
			strcpy_s(destMSG, sizeof(msgBuff), msgBuff);
		}
		else if (DAPsrc->cmd == (TCHAR)0x45) {
			valueShort1 = H2ValuetoShort(DAPsrc->data[0], DAPsrc->data[1], 0);
			USHORT valueShort2 = H2ValuetoShort(DAPsrc->data[2], DAPsrc->data[3], 0);;
			USHORT valueShort3 = H2ValuetoShort(DAPsrc->data[4], DAPsrc->data[5], 0);;
			USHORT valueShort4 = H2ValuetoShort(DAPsrc->data[6], DAPsrc->data[7], 0);;
			wsprintf(msgBuff, "수평일사량: %d\r\n경사일사량: %d\r\n외기온도: %d.%d\r\n모듈온도: %d.%d",
				valueShort1, valueShort2,
				valueShort3 / 10, valueShort3 % 10, valueShort4 / 10, valueShort4 % 10);
			strcpy_s(destMSG, sizeof(msgBuff), msgBuff);
		}
		else if (DAPsrc->cmd == (TCHAR)0x49) {			
			switch (DAPsrc->data[0]) {
			case 1:
				strcpy_s(buff, "인버터타입_DASS ");
				break;
			case 2:
				strcpy_s(buff, "인버터타입_E&P3 ");
				break;
			case 3:
				strcpy_s(buff, "인버터타입_E&P5 ");
				break;
			case 4:
				strcpy_s(buff, "인버터타입_HANS ");
				break;
			case 5:
				strcpy_s(buff, "인버터타입_HEXP ");
				break;
			case 6:
				strcpy_s(buff, "인버터타입_EKOS ");
				break;
			case 7:
				strcpy_s(buff, "인버터타입_WILL ");
				break;
			case 8:
				strcpy_s(buff, "인버터타입_ABBI ");
				break;
			case 9:
				strcpy_s(buff, "인버터타입_REFU ");
				break;
			case 10:
				strcpy_s(buff, "인버터타입_REMS ");
				break;
			case 11:
				strcpy_s(buff, "인버터타입_SUNG ");
				break;
			}
			strcpy_s(msgBuff, buff);
			wsprintf(buff, "인버터ID_%02d\r\n", DAPsrc->data[1]);
			strcat_s(msgBuff, buff);
			valueShort1 = H2ValuetoShort(DAPsrc->data[2], DAPsrc->data[3], 0);
			wsprintf(buff, "태양광전압_%dV\t", valueShort1);
			strcat_s(msgBuff, buff);
			valueShort1 = H2ValuetoShort(DAPsrc->data[4], DAPsrc->data[5], 0);
			wsprintf(buff, "태양광전류_%d.%dA\t", valueShort1 / 10, valueShort1 % 10);
			strcat_s(msgBuff, buff);
			valueShort1 = H2ValuetoShort(DAPsrc->data[6], DAPsrc->data[7], 0);
			wsprintf(buff, "태양광전력_%d.%dkW\r\n", valueShort1 / 10, valueShort1 % 10);
			strcat_s(msgBuff, buff);
			if (DAPsrc->optionL == 1) {
				valueShort1 = H2ValuetoShort(DAPsrc->data[8], DAPsrc->data[9], 0);
				wsprintf(buff, "계통전압_%dV\t", valueShort1);
				strcat_s(msgBuff, buff);
				valueShort1 = H2ValuetoShort(DAPsrc->data[10], DAPsrc->data[11], 0);
				wsprintf(buff, "계통전류_%d.%dA\t", valueShort1 / 10, valueShort1 % 10);
				strcat_s(msgBuff, buff);
				valueShort1 = H2ValuetoShort(DAPsrc->data[12], DAPsrc->data[13], 0);
				wsprintf(buff, "계통전력_%d.%dkW\r\n", valueShort1 / 10, valueShort1 % 10);
				strcat_s(msgBuff, buff);
				valueShort1 = H2ValuetoShort(DAPsrc->data[14], DAPsrc->data[15], 0);
				wsprintf(buff, "역률_%d.%d\t", valueShort1 / 10, valueShort1 % 10);
				strcat_s(msgBuff, buff);
				valueShort1 = H2ValuetoShort(DAPsrc->data[16], DAPsrc->data[17], 0);
				wsprintf(buff, "주파수_%d.%d\r\n", valueShort1 / 10, valueShort1 % 10);
				strcat_s(msgBuff, buff);
				TCHAR totalP[4];				
				totalP[3] = DAPsrc->data[18];
				totalP[2] = DAPsrc->data[19];
				totalP[1] = DAPsrc->data[20];
				totalP[0] = DAPsrc->data[21];				
				wsprintf(buff, "누적발전량_%dkWh\r\n", *((INT*)totalP));
				strcat_s(msgBuff, buff);
				if (DAPsrc->data[22] &0b10000000) strcat_s(msgBuff, "인버터에러_기타\r\n");
				else if (DAPsrc->data[22] & 0b00010000) strcat_s(msgBuff, "인버터에러_지락(누전)\r\n");
				else if (DAPsrc->data[22] & 0b00001000) strcat_s(msgBuff, "인버터에러_단독운전(정전)\r\n");
				else if (DAPsrc->data[22] & 0b00000100) strcat_s(msgBuff, "인버터에러_계통저주파수\r\n");
				else if (DAPsrc->data[22] & 0b00000010) strcat_s(msgBuff, "인버터에러_계통과주파수\r\n");
				else if (DAPsrc->data[22] & 0b00000001) strcat_s(msgBuff, "인버터에러_계통과전류\r\n");
				else if (DAPsrc->data[23] & 0b10000000) strcat_s(msgBuff, "인버터에러_계통저전압\r\n");
				else if (DAPsrc->data[23] & 0b01000000) strcat_s(msgBuff, "인버터에러_계통과전압\r\n");
				else if (DAPsrc->data[23] & 0b00100000) strcat_s(msgBuff, "인버터에러_과열\r\n");
				else if (DAPsrc->data[23] & 0b00010000) strcat_s(msgBuff, "인버터에러_IGBT에러\r\n");
				else if (DAPsrc->data[23] & 0b00001000) strcat_s(msgBuff, "인버터에러_태양전지과전류\r\n");
				else if (DAPsrc->data[23] & 0b00000100) strcat_s(msgBuff, "인버터에러_태양전지저전압\r\n");
				else if (DAPsrc->data[23] & 0b00000010) strcat_s(msgBuff, "인버터에러_태양전지과전압\r\n");
				else strcat_s(msgBuff, "인버터정상\r\n");
				if (DAPsrc->data[24]==1) strcat_s(msgBuff, "인버터_동작중\r\n");
				else  strcat_s(msgBuff, "인버터_정지\r\n");
			}
			else if (DAPsrc->optionL == 3)
			{
				valueShort1 = H2ValuetoShort(DAPsrc->data[8], DAPsrc->data[9], 0);
				wsprintf(buff, "계통R전압_%dV\t", valueShort1);
				strcat_s(msgBuff, buff);
				valueShort1 = H2ValuetoShort(DAPsrc->data[10], DAPsrc->data[11], 0);
				wsprintf(buff, "계통S전압_%dV\t", valueShort1);
				strcat_s(msgBuff, buff);
				valueShort1 = H2ValuetoShort(DAPsrc->data[12], DAPsrc->data[13], 0);
				wsprintf(buff, "계통T전압_%dV\r\n", valueShort1);
				strcat_s(msgBuff, buff);
				valueShort1 = H2ValuetoShort(DAPsrc->data[14], DAPsrc->data[15], 0);
				wsprintf(buff, "계통R전류_%d.%dA\t", valueShort1 / 10, valueShort1 % 10);
				strcat_s(msgBuff, buff);
				valueShort1 = H2ValuetoShort(DAPsrc->data[16], DAPsrc->data[17], 0);
				wsprintf(buff, "계통S전류_%d.%dA\t", valueShort1 / 10, valueShort1 % 10);
				strcat_s(msgBuff, buff);
				valueShort1 = H2ValuetoShort(DAPsrc->data[18], DAPsrc->data[19], 0);
				wsprintf(buff, "계통T전류_%d.%dA\r\n", valueShort1 / 10, valueShort1 % 10);
				strcat_s(msgBuff, buff);
				valueShort1 = H2ValuetoShort(DAPsrc->data[20], DAPsrc->data[21], 0);
				wsprintf(buff, "계통전력_%d.%dkW\r\n", valueShort1 / 10, valueShort1 % 10);
				strcat_s(msgBuff, buff);
				valueShort1 = H2ValuetoShort(DAPsrc->data[22], DAPsrc->data[23], 0);
				if (valueShort1>1000) wsprintf(buff, "역률_\t\t", NULL);
				else wsprintf(buff, "역률_%d.%d\t", valueShort1 / 10, valueShort1 % 10);
				strcat_s(msgBuff, buff);
				valueShort1 = H2ValuetoShort(DAPsrc->data[24], DAPsrc->data[25], 0);
				wsprintf(buff, "주파수_%d.%d\r\n", valueShort1 / 10, valueShort1 % 10);
				strcat_s(msgBuff, buff);
				TCHAR totalP[4];
				totalP[3] = DAPsrc->data[26];
				totalP[2] = DAPsrc->data[27];
				totalP[1] = DAPsrc->data[28];
				totalP[0] = DAPsrc->data[29];
				wsprintf(buff, "누적발전량_%dkWh\r\n", *((INT*)totalP));
				strcat_s(msgBuff, buff);
				if (DAPsrc->data[30] & 0b10000000) strcat_s(msgBuff, "인버터에러_기타\r\n");
				else if (DAPsrc->data[30] & 0b00010000) strcat_s(msgBuff, "인버터에러_지락(누전)\r\n");
				else if (DAPsrc->data[30] & 0b00001000) strcat_s(msgBuff, "인버터에러_단독운전(정전)\r\n");
				else if (DAPsrc->data[30] & 0b00000100) strcat_s(msgBuff, "인버터에러_계통저주파수\r\n");
				else if (DAPsrc->data[30] & 0b00000010) strcat_s(msgBuff, "인버터에러_계통과주파수\r\n");
				else if (DAPsrc->data[30] & 0b00000001) strcat_s(msgBuff, "인버터에러_계통과전류\r\n");
				else if (DAPsrc->data[31] & 0b10000000) strcat_s(msgBuff, "인버터에러_계통저전압\r\n");
				else if (DAPsrc->data[31] & 0b01000000) strcat_s(msgBuff, "인버터에러_계통과전압\r\n");
				else if (DAPsrc->data[31] & 0b00100000) strcat_s(msgBuff, "인버터에러_과열\r\n");
				else if (DAPsrc->data[31] & 0b00010000) strcat_s(msgBuff, "인버터에러_IGBT에러\r\n");
				else if (DAPsrc->data[31] & 0b00001000) strcat_s(msgBuff, "인버터에러_태양전지과전류\r\n");
				else if (DAPsrc->data[31] & 0b00000100) strcat_s(msgBuff, "인버터에러_태양전지저전압\r\n");
				else if (DAPsrc->data[31] & 0b00000010) strcat_s(msgBuff, "인버터에러_태양전지과전압\r\n");
				else strcat_s(msgBuff, "인버터정상\r\n");
				if (DAPsrc->data[32] == 1) strcat_s(msgBuff, "인버터_동작중\r\n");
				else  strcat_s(msgBuff, "인버터_정지\r\n");
			}
			strcpy_s(destMSG, sizeof(msgBuff), msgBuff);
		}		
		break;
	case DEAPI_CKSUM:
		if (DAPsrc->length > 256) break;
		TCHAR cksum = 0;
		cksum ^= (TCHAR)(DAPsrc->apiID ^ DAPsrc->cmd ^ DAPsrc->status ^ DAPsrc->frameID
			^ DAPsrc->optionH ^ DAPsrc->optionL);
		for (cnt = 0; cnt < DAPsrc->length-6; cnt++) {
			cksum ^= (TCHAR)DAPsrc->data[cnt];
		}
		if (cksum==DAPsrc->ckSum) strcpy_s(destMSG, 5, "TRUE");
		else strcpy_s(destMSG, 6, "FALSE");
		break;
	}
	return TRUE;
}

INT GetMSGVerifiedDEAPIPacket(TCHAR *destMSG, const tagDEAPIPacketStr *DAPsrc, INT optionflag, INT cbMSGMAX)
{
	if (cbMSGMAX < 50) return -1;
	USHORT cnt = A2CtoHex(DAPsrc->lengthlsb[0], DAPsrc->lengthlsb[1]);
	USHORT length;
	TCHAR cmd = A2CtoHex(DAPsrc->cmd[0], DAPsrc->cmd[1]);
	TCHAR apiID = A2CtoHex(DAPsrc->apiID[0], DAPsrc->apiID[1]);
	length = cnt;
	cnt = A2CtoHex(DAPsrc->lengthmsb[0], DAPsrc->lengthmsb[1]);
	cnt <<= 8;
	length |= cnt;
	switch (optionflag) {
	case DEAPI_STX:
		if ((DAPsrc->stx[0] == 'd' || DAPsrc->stx[0] == 'D') && (DAPsrc->stx[1] == 'e' || DAPsrc->stx[1] == 'E'))
			strcpy_s(destMSG, 5, "TRUE");
		else strcpy_s(destMSG, 6, "FALSE");
		break;
	case DEAPI_LENGTH:
		if (length != (lstrlen(DAPsrc->data) - 6)) strcpy_s(destMSG, 5, "TRUE");
		else strcpy_s(destMSG, 6, "FALSE");
		break;
	case DEAPI_APIID:		
		if (apiID == 0x10) strcpy_s(destMSG, 20, "request to 485");
		else if (apiID == 0x90) strcpy_s(destMSG, 20, "response to 485");
		else if (apiID == 0x98) strcpy_s(destMSG, 20, "send to lora");
		else strcpy_s(destMSG, 20, "FALSE");
		break;
	case DEAPI_CMD:		
		if (cmd == 0x31) strcpy_s(destMSG, 20, "Module Sensor 1ch");
		else if (cmd == 0x32) strcpy_s(destMSG, 20, "Module Sensor 2ch");
		else if (cmd == 0x45) strcpy_s(destMSG, 20, "PVE sensor");
		else if (cmd == 0x49) strcpy_s(destMSG, 20, "Inverter");
		else strcpy_s(destMSG, 20, "FALSE");
		break;
	case DEAPI_STATUS:
		cnt = A2CtoHex(DAPsrc->status[0], DAPsrc->status[1]);
		if (cnt == 0x00) strcpy_s(destMSG, 20, "Normal");
		else if (cnt == 0x80) strcpy_s(destMSG, 20, "No response");	
		else if (cnt == 0x81) strcpy_s(destMSG, 20, "Wrong mode set");
		else if (cnt == 0x82) strcpy_s(destMSG, 25, "ZB counter set error");
		else if (cnt == 0x83) strcpy_s(destMSG, 25, "Inverter total error");
		else strcpy_s(destMSG, 6, "FALSE");
		break;
	case DEAPI_FRAMEID:
		cnt = A2CtoHex(DAPsrc->frameID[0], DAPsrc->frameID[1]);
		if ((cmd == 0x31 || cmd == 0x32) && (apiID == 0x10 || apiID == 0x90)) wsprintf(destMSG, "중계기번호: %d", cnt);
		else if ((cmd == 0x31 || cmd == 0x32) && apiID == 0x98) wsprintf(destMSG, "페이지번호: %d", cnt);
		else if (cmd == 0x49) wsprintf(destMSG, "인버터번호: %d", cnt);
		else if (cmd == 0x45) wsprintf(destMSG, "환경센서구분자: %d", cnt);				
		else strcpy_s(destMSG, 6, "-");
		break;
	case DEAPI_OPTIONH:		
	case DEAPI_OPTIONL:
		cnt = A2CtoHex(DAPsrc->optionH[0], DAPsrc->optionH[1]);
		length = A2CtoHex(DAPsrc->optionL[0], DAPsrc->optionL[1]);
		cnt <<= 8;
		cnt |= length;
		if ((cmd == 0x31 || cmd == 0x32)) wsprintf(destMSG, "모듈센서시작번호: %d", cnt);
		else if (cmd == 0x49) wsprintf(destMSG, "인버터 Phase: %s", DAPsrc->optionL);
		else strcpy_s(destMSG, 6, "-");
		break;
	case DEAPI_DATA:

		break;
	case DEAPI_CKSUM:
		break;
	}
	return TRUE;
}