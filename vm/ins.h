/*
	vm/ins.h
	Copyright (C) 2020  Quasimodo

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#ifndef _INS_H_
#define _INS_H_
#include <vm.h>
#include <config.h>
void ADC_AbIb(LPVCPU cpu);
void ADC_AdId(LPVCPU cpu);
void ADC_AwIw(LPVCPU cpu);
void ADC_MbIb(LPVCPU cpu);
void ADC_MbRb(LPVCPU cpu);
void ADC_MdIb(LPVCPU cpu);
void ADC_MdId(LPVCPU cpu);
void ADC_MdRd(LPVCPU cpu);
void ADC_MwIb(LPVCPU cpu);
void ADC_MwIw(LPVCPU cpu);
void ADC_MwRw(LPVCPU cpu);
void ADC_RbIb(LPVCPU cpu);
void ADC_RbMb(LPVCPU cpu);
void ADC_RbRb(LPVCPU cpu);
void ADC_RdIb(LPVCPU cpu);
void ADC_RdId(LPVCPU cpu);
void ADC_RdMd(LPVCPU cpu);
void ADC_RdRd(LPVCPU cpu);
void ADC_RwIb(LPVCPU cpu);
void ADC_RwIw(LPVCPU cpu);
void ADC_RwMw(LPVCPU cpu);
void ADC_RwRw(LPVCPU cpu);
void ADD_AbIb(LPVCPU cpu);
void ADD_AdId(LPVCPU cpu);
void ADD_AwIw(LPVCPU cpu);
void ADD_MbIb(LPVCPU cpu);
void ADD_MbRb(LPVCPU cpu);
void ADD_MdIb(LPVCPU cpu);
void ADD_MdId(LPVCPU cpu);
void ADD_MdRd(LPVCPU cpu);
void ADD_MwIb(LPVCPU cpu);
void ADD_MwIw(LPVCPU cpu);
void ADD_MwRw(LPVCPU cpu);
void ADD_RbIb(LPVCPU cpu);
void ADD_RbMb(LPVCPU cpu);
void ADD_RbRb(LPVCPU cpu);
void ADD_RdIb(LPVCPU cpu);
void ADD_RdId(LPVCPU cpu);
void ADD_RdMd(LPVCPU cpu);
void ADD_RdRd(LPVCPU cpu);
void ADD_RwIb(LPVCPU cpu);
void ADD_RwIw(LPVCPU cpu);
void ADD_RwMw(LPVCPU cpu);
void ADD_RwRw(LPVCPU cpu);
void CBW_No(LPVCPU cpu);
void CDQ_No(LPVCPU cpu);
void CMPSB_No(LPVCPU cpu);
void CMPSD_No(LPVCPU cpu);
void CMPSW_No(LPVCPU cpu);
void CMPXCHG8B_Mq(LPVCPU cpu);
void CMPXCHG_MbRb(LPVCPU cpu);
void CMPXCHG_MdRd(LPVCPU cpu);
void CMPXCHG_MwRw(LPVCPU cpu);
void CMPXCHG_RbRb(LPVCPU cpu);
void CMPXCHG_RdRd(LPVCPU cpu);
void CMPXCHG_RwRw(LPVCPU cpu);
void CMP_AbIb(LPVCPU cpu);
void CMP_AdId(LPVCPU cpu);
void CMP_AwIw(LPVCPU cpu);
void CMP_MbIb(LPVCPU cpu);
void CMP_MbRb(LPVCPU cpu);
void CMP_MdIb(LPVCPU cpu);
void CMP_MdId(LPVCPU cpu);
void CMP_MdRd(LPVCPU cpu);
void CMP_MwIb(LPVCPU cpu);
void CMP_MwIw(LPVCPU cpu);
void CMP_MwRw(LPVCPU cpu);
void CMP_RbIb(LPVCPU cpu);
void CMP_RbMb(LPVCPU cpu);
void CMP_RbRb(LPVCPU cpu);
void CMP_RdIb(LPVCPU cpu);
void CMP_RdId(LPVCPU cpu);
void CMP_RdMd(LPVCPU cpu);
void CMP_RdRd(LPVCPU cpu);
void CMP_RwIb(LPVCPU cpu);
void CMP_RwIw(LPVCPU cpu);
void CMP_RwMw(LPVCPU cpu);
void CMP_RwRw(LPVCPU cpu);
void CWDE_No(LPVCPU cpu);
void CWD_No(LPVCPU cpu);
void DEC_Mb(LPVCPU cpu);
void DEC_Md(LPVCPU cpu);
void DEC_Mw(LPVCPU cpu);
void DEC_Rb(LPVCPU cpu);
void DEC_Rd(LPVCPU cpu);
void DEC_Rw(LPVCPU cpu);
void DIV_Mb(LPVCPU cpu);
void DIV_Md(LPVCPU cpu);
void DIV_Mw(LPVCPU cpu);
void DIV_Rb(LPVCPU cpu);
void DIV_Rd(LPVCPU cpu);
void DIV_Rw(LPVCPU cpu);
void IDIV_Mb(LPVCPU cpu);
void IDIV_Md(LPVCPU cpu);
void IDIV_Mw(LPVCPU cpu);
void IDIV_Rb(LPVCPU cpu);
void IDIV_Rd(LPVCPU cpu);
void IDIV_Rw(LPVCPU cpu);
void IMUL_Mb(LPVCPU cpu);
void IMUL_Md(LPVCPU cpu);
void IMUL_Mw(LPVCPU cpu);
void IMUL_Rb(LPVCPU cpu);
void IMUL_Rd(LPVCPU cpu);
void IMUL_RdMd(LPVCPU cpu);
void IMUL_RdMdIb(LPVCPU cpu);
void IMUL_RdMdId(LPVCPU cpu);
void IMUL_RdRd(LPVCPU cpu);
void IMUL_RdRdIb(LPVCPU cpu);
void IMUL_RdRdId(LPVCPU cpu);
void IMUL_Rw(LPVCPU cpu);
void IMUL_RwMw(LPVCPU cpu);
void IMUL_RwMwIb(LPVCPU cpu);
void IMUL_RwMwIw(LPVCPU cpu);
void IMUL_RwRw(LPVCPU cpu);
void IMUL_RwRwIb(LPVCPU cpu);
void IMUL_RwRwIw(LPVCPU cpu);
void INC_Mb(LPVCPU cpu);
void INC_Md(LPVCPU cpu);
void INC_Mw(LPVCPU cpu);
void INC_Rb(LPVCPU cpu);
void INC_Rd(LPVCPU cpu);
void INC_Rw(LPVCPU cpu);
void MUL_Mb(LPVCPU cpu);
void MUL_Md(LPVCPU cpu);
void MUL_Mw(LPVCPU cpu);
void MUL_Rb(LPVCPU cpu);
void MUL_Rd(LPVCPU cpu);
void MUL_Rw(LPVCPU cpu);
void NEG_Mb(LPVCPU cpu);
void NEG_Md(LPVCPU cpu);
void NEG_Mw(LPVCPU cpu);
void NEG_Rb(LPVCPU cpu);
void NEG_Rd(LPVCPU cpu);
void NEG_Rw(LPVCPU cpu);
void SBB_AbIb(LPVCPU cpu);
void SBB_AdId(LPVCPU cpu);
void SBB_AwIw(LPVCPU cpu);
void SBB_MbIb(LPVCPU cpu);
void SBB_MbRb(LPVCPU cpu);
void SBB_MdIb(LPVCPU cpu);
void SBB_MdId(LPVCPU cpu);
void SBB_MdRd(LPVCPU cpu);
void SBB_MwIb(LPVCPU cpu);
void SBB_MwIw(LPVCPU cpu);
void SBB_MwRw(LPVCPU cpu);
void SBB_RbIb(LPVCPU cpu);
void SBB_RbMb(LPVCPU cpu);
void SBB_RbRb(LPVCPU cpu);
void SBB_RdIb(LPVCPU cpu);
void SBB_RdId(LPVCPU cpu);
void SBB_RdMd(LPVCPU cpu);
void SBB_RdRd(LPVCPU cpu);
void SBB_RwIb(LPVCPU cpu);
void SBB_RwIw(LPVCPU cpu);
void SBB_RwMw(LPVCPU cpu);
void SBB_RwRw(LPVCPU cpu);
void SUB_AbIb(LPVCPU cpu);
void SUB_AdId(LPVCPU cpu);
void SUB_AwIw(LPVCPU cpu);
void SUB_MbIb(LPVCPU cpu);
void SUB_MbRb(LPVCPU cpu);
void SUB_MdIb(LPVCPU cpu);
void SUB_MdId(LPVCPU cpu);
void SUB_MdRd(LPVCPU cpu);
void SUB_MwIb(LPVCPU cpu);
void SUB_MwIw(LPVCPU cpu);
void SUB_MwRw(LPVCPU cpu);
void SUB_RbIb(LPVCPU cpu);
void SUB_RbMb(LPVCPU cpu);
void SUB_RbRb(LPVCPU cpu);
void SUB_RdIb(LPVCPU cpu);
void SUB_RdId(LPVCPU cpu);
void SUB_RdMd(LPVCPU cpu);
void SUB_RdRd(LPVCPU cpu);
void SUB_RwIb(LPVCPU cpu);
void SUB_RwIw(LPVCPU cpu);
void SUB_RwMw(LPVCPU cpu);
void SUB_RwRw(LPVCPU cpu);
void SCASB_No(LPVCPU cpu);
void SCASD_No(LPVCPU cpu);
void SCASW_No(LPVCPU cpu);
void XADD_MbRb(LPVCPU cpu);
void XADD_MdRd(LPVCPU cpu);
void XADD_MwRw(LPVCPU cpu);
void XADD_RbRb(LPVCPU cpu);
void XADD_RdRd(LPVCPU cpu);
void XADD_RwRw(LPVCPU cpu);

void AND_AbIb(LPVCPU cpu);
void AND_AdId(LPVCPU cpu);
void AND_AwIw(LPVCPU cpu);
void AND_MbIb(LPVCPU cpu);
void AND_MbRb(LPVCPU cpu);
void AND_MdIb(LPVCPU cpu);
void AND_MdId(LPVCPU cpu);
void AND_MdRd(LPVCPU cpu);
void AND_MwIb(LPVCPU cpu);
void AND_MwIw(LPVCPU cpu);
void AND_MwRw(LPVCPU cpu);
void AND_RbIb(LPVCPU cpu);
void AND_RbMb(LPVCPU cpu);
void AND_RbRb(LPVCPU cpu);
void AND_RdIb(LPVCPU cpu);
void AND_RdId(LPVCPU cpu);
void AND_RdMd(LPVCPU cpu);
void AND_RdRd(LPVCPU cpu);
void AND_RwIb(LPVCPU cpu);
void AND_RwIw(LPVCPU cpu);
void AND_RwMw(LPVCPU cpu);
void AND_RwRw(LPVCPU cpu);
void NOT_Mb(LPVCPU cpu);
void NOT_Md(LPVCPU cpu);
void NOT_Mw(LPVCPU cpu);
void NOT_Rb(LPVCPU cpu);
void NOT_Rd(LPVCPU cpu);
void NOT_Rw(LPVCPU cpu);
void OR_AbIb(LPVCPU cpu);
void OR_AdId(LPVCPU cpu);
void OR_AwIw(LPVCPU cpu);
void OR_MbIb(LPVCPU cpu);
void OR_MbRb(LPVCPU cpu);
void OR_MdIb(LPVCPU cpu);
void OR_MdId(LPVCPU cpu);
void OR_MdRd(LPVCPU cpu);
void OR_MwIb(LPVCPU cpu);
void OR_MwIw(LPVCPU cpu);
void OR_MwRw(LPVCPU cpu);
void OR_RbIb(LPVCPU cpu);
void OR_RbMb(LPVCPU cpu);
void OR_RbRb(LPVCPU cpu);
void OR_RdIb(LPVCPU cpu);
void OR_RdId(LPVCPU cpu);
void OR_RdMd(LPVCPU cpu);
void OR_RdRd(LPVCPU cpu);
void OR_RwIb(LPVCPU cpu);
void OR_RwIw(LPVCPU cpu);
void OR_RwMw(LPVCPU cpu);
void OR_RwRw(LPVCPU cpu);
void TEST_AbIb(LPVCPU cpu);
void TEST_AdId(LPVCPU cpu);
void TEST_AwIw(LPVCPU cpu);
void TEST_MbIb(LPVCPU cpu);
void TEST_MbRb(LPVCPU cpu);
void TEST_MdIb(LPVCPU cpu);
void TEST_MdId(LPVCPU cpu);
void TEST_MdRd(LPVCPU cpu);
void TEST_MwIb(LPVCPU cpu);
void TEST_MwIw(LPVCPU cpu);
void TEST_MwRw(LPVCPU cpu);
void TEST_RbIb(LPVCPU cpu);
void TEST_RbMb(LPVCPU cpu);
void TEST_RbRb(LPVCPU cpu);
void TEST_RdIb(LPVCPU cpu);
void TEST_RdId(LPVCPU cpu);
void TEST_RdMd(LPVCPU cpu);
void TEST_RdRd(LPVCPU cpu);
void TEST_RwIb(LPVCPU cpu);
void TEST_RwIw(LPVCPU cpu);
void TEST_RwMw(LPVCPU cpu);
void TEST_RwRw(LPVCPU cpu);
void XOR_AbIb(LPVCPU cpu);
void XOR_AdId(LPVCPU cpu);
void XOR_AwIw(LPVCPU cpu);
void XOR_MbIb(LPVCPU cpu);
void XOR_MbRb(LPVCPU cpu);
void XOR_MdIb(LPVCPU cpu);
void XOR_MdId(LPVCPU cpu);
void XOR_MdRd(LPVCPU cpu);
void XOR_MwIb(LPVCPU cpu);
void XOR_MwIw(LPVCPU cpu);
void XOR_MwRw(LPVCPU cpu);
void XOR_RbIb(LPVCPU cpu);
void XOR_RbMb(LPVCPU cpu);
void XOR_RbRb(LPVCPU cpu);
void XOR_RdIb(LPVCPU cpu);
void XOR_RdId(LPVCPU cpu);
void XOR_RdMd(LPVCPU cpu);
void XOR_RdRd(LPVCPU cpu);
void XOR_RwIb(LPVCPU cpu);
void XOR_RwIw(LPVCPU cpu);
void XOR_RwMw(LPVCPU cpu);
void XOR_RwRw(LPVCPU cpu);

void CF_DAA(unsigned short ax);
void CF_DAS(unsigned short ax);
void AAA_No(LPVCPU cpu);
void AAD_Ib(LPVCPU cpu);
void AAM_Ib(LPVCPU cpu);
void AAS_No(LPVCPU cpu);
void DAA_No(LPVCPU cpu);
void DAS_No(LPVCPU cpu);

void ROR_Mb(LPVCPU cpu);
void ROR_Mw(LPVCPU cpu);
void ROR_Md(LPVCPU cpu);
void ROR_Rb(LPVCPU cpu);
void ROR_Rw(LPVCPU cpu);
void ROR_Rd(LPVCPU cpu);
void ROL_Mb(LPVCPU cpu);
void ROL_Mw(LPVCPU cpu);
void ROL_Md(LPVCPU cpu);
void ROL_Rb(LPVCPU cpu);
void ROL_Rw(LPVCPU cpu);
void ROL_Rd(LPVCPU cpu);
void RCR_Mb(LPVCPU cpu);
void RCR_Mw(LPVCPU cpu);
void RCR_Md(LPVCPU cpu);
void RCR_Rb(LPVCPU cpu);
void RCR_Rw(LPVCPU cpu);
void RCR_Rd(LPVCPU cpu);
void RCL_Mb(LPVCPU cpu);
void RCL_Mw(LPVCPU cpu);
void RCL_Md(LPVCPU cpu);
void RCL_Rb(LPVCPU cpu);
void RCL_Rw(LPVCPU cpu);
void RCL_Rd(LPVCPU cpu);
void SHR_Mb(LPVCPU cpu);
void SHR_Mw(LPVCPU cpu);
void SHR_Md(LPVCPU cpu);
void SHR_Rb(LPVCPU cpu);
void SHR_Rw(LPVCPU cpu);
void SHR_Rd(LPVCPU cpu);
void SHL_Mb(LPVCPU cpu);
void SHL_Mw(LPVCPU cpu);
void SHL_Md(LPVCPU cpu);
void SHL_Rb(LPVCPU cpu);
void SHL_Rw(LPVCPU cpu);
void SHL_Rd(LPVCPU cpu);
void SAR_Mb(LPVCPU cpu);
void SAR_Mw(LPVCPU cpu);
void SAR_Md(LPVCPU cpu);
void SAR_Rb(LPVCPU cpu);
void SAR_Rw(LPVCPU cpu);
void SAR_Rd(LPVCPU cpu);
void SAL_Mb(LPVCPU cpu);
void SAL_Mw(LPVCPU cpu);
void SAL_Md(LPVCPU cpu);
void SAL_Rb(LPVCPU cpu);
void SAL_Rw(LPVCPU cpu);
void SAL_Rd(LPVCPU cpu);
void SHLD_MdRdCL(LPVCPU cpu);
void SHLD_MdRdIb(LPVCPU cpu);
void SHLD_MwRwCL(LPVCPU cpu);
void SHLD_MwRwIb(LPVCPU cpu);
void SHLD_RdRdCL(LPVCPU cpu);
void SHLD_RdRdIb(LPVCPU cpu);
void SHLD_RwRwCL(LPVCPU cpu);
void SHLD_RwRwIb(LPVCPU cpu);
void SHRD_MdRdCL(LPVCPU cpu);
void SHRD_MdRdIb(LPVCPU cpu);
void SHRD_MwRwCL(LPVCPU cpu);
void SHRD_MwRwIb(LPVCPU cpu);
void SHRD_RdRdCL(LPVCPU cpu);
void SHRD_RdRdIb(LPVCPU cpu);
void SHRD_RwRwCL(LPVCPU cpu);
void SHRD_RwRwIb(LPVCPU cpu);

void BSF_RwMw(LPVCPU cpu);
void BSF_RwRw(LPVCPU cpu);
void BSR_RwMw(LPVCPU cpu);
void BSR_RwRw(LPVCPU cpu);
void BSF_RdRd(LPVCPU cpu);
void BSR_RdRd(LPVCPU cpu);
void BSF_RdMd(LPVCPU cpu);
void BSR_RdMd(LPVCPU cpu);
void BSWAP_Rd(LPVCPU cpu);
void BTC_MdIb(LPVCPU cpu);
void BTC_MdRd(LPVCPU cpu);
void BTC_MwIb(LPVCPU cpu);
void BTC_MwRw(LPVCPU cpu);
void BTC_RdIb(LPVCPU cpu);
void BTC_RdRd(LPVCPU cpu);
void BTC_RwIb(LPVCPU cpu);
void BTC_RwRw(LPVCPU cpu);
void BTR_MdIb(LPVCPU cpu);
void BTR_MwIb(LPVCPU cpu);
void BTR_MwRw(LPVCPU cpu);
void BTR_RdIb(LPVCPU cpu);
void BTR_RwRw(LPVCPU cpu);
void BTR_RwIb(LPVCPU cpu);
void BTR_RdRd(LPVCPU cpu);
void BTR_MdRd(LPVCPU cpu);
void BTS_MdIb(LPVCPU cpu);
void BTS_MwIb(LPVCPU cpu);
void BTS_MwRw(LPVCPU cpu);
void BTS_RdIb(LPVCPU cpu);
void BTS_RwIb(LPVCPU cpu);
void BTS_RwRw(LPVCPU cpu);
void BTS_RdRd(LPVCPU cpu);
void BTS_MdRd(LPVCPU cpu);
void BT_MdIb(LPVCPU cpu);
void BT_MwIb(LPVCPU cpu);
void BT_MwRw(LPVCPU cpu);
void BT_RdIb(LPVCPU cpu);
void BT_RwIb(LPVCPU cpu);
void BT_RwIb(LPVCPU cpu);
void BT_RwRw(LPVCPU cpu);
void BT_RdRd(LPVCPU cpu);
void BT_MdRd(LPVCPU cpu);
void SETA_Mb(LPVCPU cpu);
void SETA_Rb(LPVCPU cpu);
void SETC_Mb(LPVCPU cpu);
void SETC_Rb(LPVCPU cpu);
void SETG_Mb(LPVCPU cpu);
void SETG_Rb(LPVCPU cpu);
void SETL_Mb(LPVCPU cpu);
void SETL_Rb(LPVCPU cpu);
void SETNA_Mb(LPVCPU cpu);
void SETNA_Rb(LPVCPU cpu);
void SETNC_Mb(LPVCPU cpu);
void SETNC_Rb(LPVCPU cpu);
void SETNG_Mb(LPVCPU cpu);
void SETNG_Rb(LPVCPU cpu);
void SETNL_Mb(LPVCPU cpu);
void SETNL_Rb(LPVCPU cpu);
void SETNO_Mb(LPVCPU cpu);
void SETNO_Rb(LPVCPU cpu);
void SETNP_Mb(LPVCPU cpu);
void SETNP_Rb(LPVCPU cpu);
void SETNS_Mb(LPVCPU cpu);
void SETNS_Rb(LPVCPU cpu);
void SETNZ_Mb(LPVCPU cpu);
void SETNZ_Rb(LPVCPU cpu);
void SETO_Mb(LPVCPU cpu);
void SETO_Rb(LPVCPU cpu);
void SETP_Mb(LPVCPU cpu);
void SETP_Rb(LPVCPU cpu);
void SETS_Mb(LPVCPU cpu);
void SETS_Rb(LPVCPU cpu);
void SETZ_Mb(LPVCPU cpu);
void SETZ_Rb(LPVCPU cpu);

void CALL_IPd(LPVCPU cpu);
void CALL_IPw(LPVCPU cpu);
void CALL_Id(LPVCPU cpu);
void CALL_Iw(LPVCPU cpu);
void CALL_MPd(LPVCPU cpu);
void CALL_MPw(LPVCPU cpu);
void CALL_Md(LPVCPU cpu);
void CALL_Mw(LPVCPU cpu);
void CALL_Rd(LPVCPU cpu);
void CALL_Rw(LPVCPU cpu);
void IRET_No(LPVCPU cpu);
void JA_Ib(LPVCPU cpu);
void JA_Id(LPVCPU cpu);
void JA_Iw(LPVCPU cpu);
void JCXZ_Ib(LPVCPU cpu);
void JC_Ib(LPVCPU cpu);
void JC_Id(LPVCPU cpu);
void JC_Iw(LPVCPU cpu);
void JECXZ_Ib(LPVCPU cpu);
void JG_Ib(LPVCPU cpu);
void JG_Id(LPVCPU cpu);
void JG_Iw(LPVCPU cpu);
void JL_Ib(LPVCPU cpu);
void JL_Id(LPVCPU cpu);
void JL_Iw(LPVCPU cpu);
void JMP_IPd(LPVCPU cpu);
void JMP_IPw(LPVCPU cpu);
void JMP_Ib(LPVCPU cpu);
void JMP_Id(LPVCPU cpu);
void JMP_Iw(LPVCPU cpu);
void JMP_MPd(LPVCPU cpu);
void JMP_MPw(LPVCPU cpu);
void JMP_Md(LPVCPU cpu);
void JMP_Mw(LPVCPU cpu);
void JMP_Rd(LPVCPU cpu);
void JMP_Rw(LPVCPU cpu);
void JNA_Ib(LPVCPU cpu);
void JNA_Id(LPVCPU cpu);
void JNA_Iw(LPVCPU cpu);
void JNC_Ib(LPVCPU cpu);
void JNC_Id(LPVCPU cpu);
void JNC_Iw(LPVCPU cpu);
void JNG_Ib(LPVCPU cpu);
void JNG_Id(LPVCPU cpu);
void JNG_Iw(LPVCPU cpu);
void JNL_Ib(LPVCPU cpu);
void JNL_Id(LPVCPU cpu);
void JNL_Iw(LPVCPU cpu);
void JNO_Ib(LPVCPU cpu);
void JNO_Id(LPVCPU cpu);
void JNO_Iw(LPVCPU cpu);
void JNP_Ib(LPVCPU cpu);
void JNP_Id(LPVCPU cpu);
void JNP_Iw(LPVCPU cpu);
void JNS_Ib(LPVCPU cpu);
void JNS_Id(LPVCPU cpu);
void JNS_Iw(LPVCPU cpu);
void JNZ_Ib(LPVCPU cpu);
void JNZ_Id(LPVCPU cpu);
void JNZ_Iw(LPVCPU cpu);
void JO_Ib(LPVCPU cpu);
void JO_Id(LPVCPU cpu);
void JO_Iw(LPVCPU cpu);
void JP_Ib(LPVCPU cpu);
void JP_Id(LPVCPU cpu);
void JP_Iw(LPVCPU cpu);
void JS_Ib(LPVCPU cpu);
void JS_Id(LPVCPU cpu);
void JS_Iw(LPVCPU cpu);
void JZ_Ib(LPVCPU cpu);
void JZ_Id(LPVCPU cpu);
void JZ_Iw(LPVCPU cpu);
void RET_FIw(LPVCPU cpu);
void RET_FNo(LPVCPU cpu);
void RET_NIw(LPVCPU cpu);
void RET_NNo(LPVCPU cpu);
void IRETD_No(LPVCPU cpu);
void LOOP_Ib(LPVCPU cpu);
void LOOPE_Ib(LPVCPU cpu);
void LOOPNE_Ib(LPVCPU cpu);
void INT3_No(LPVCPU cpu);
void INTO_No(LPVCPU cpu);
void INT_Ib(LPVCPU cpu);

void POPAD_No(LPVCPU cpu);
void POPA_No(LPVCPU cpu);
void POPFD_No(LPVCPU cpu);
void POPF_No(LPVCPU cpu);
void POP_Rd(LPVCPU cpu);
void POP_Rw(LPVCPU cpu);
void POP_Sr(LPVCPU cpu);
void PUSHAD_No(LPVCPU cpu);
void PUSHA_No(LPVCPU cpu);
void PUSHFD_No(LPVCPU cpu);
void PUSHF_No(LPVCPU cpu);
void PUSH_Ib(LPVCPU cpu);
void PUSH_Id(LPVCPU cpu);
void PUSH_Iw(LPVCPU cpu);
void PUSH_Md(LPVCPU cpu);
void PUSH_Mw(LPVCPU cpu);
void PUSH_Rd(LPVCPU cpu);
void PUSH_Rw(LPVCPU cpu);
void PUSH_Sr(LPVCPU cpu);
void POP_Md(LPVCPU cpu);
void POP_Mw(LPVCPU cpu);

void LDS_RdMPd(LPVCPU cpu);
void LDS_RwMPw(LPVCPU cpu);
void LEA_RdMu(LPVCPU cpu);
void LEA_RwMu(LPVCPU cpu);
void LES_RdMPd(LPVCPU cpu);
void LES_RwMPw(LPVCPU cpu);
void LFS_RdMPd(LPVCPU cpu);
void LFS_RwMPw(LPVCPU cpu);
void LGDT_MPd(LPVCPU cpu);
void LGS_RdMPd(LPVCPU cpu);
void LGS_RwMPw(LPVCPU cpu);
void LIDT_MPd(LPVCPU cpu);
void LLDT_Mw(LPVCPU cpu);
void LLDT_Rw(LPVCPU cpu);
void LMSW_Mw(LPVCPU cpu);
void LMSW_Rw(LPVCPU cpu);
void LODSB_No(LPVCPU cpu);
void LODSD_No(LPVCPU cpu);
void LODSW_No(LPVCPU cpu);
void LSS_RdMPd(LPVCPU cpu);
void LSS_RwMPw(LPVCPU cpu);
void MOVSB_No(LPVCPU cpu);
void MOVSD_No(LPVCPU cpu);
void MOVSW_No(LPVCPU cpu);
void MOVSX_RdMb(LPVCPU cpu);
void MOVSX_RdMw(LPVCPU cpu);
void MOVSX_RdRb(LPVCPU cpu);
void MOVSX_RdRw(LPVCPU cpu);
void MOVSX_RwMb(LPVCPU cpu);
void MOVSX_RwRb(LPVCPU cpu);
void MOVZX_RdMb(LPVCPU cpu);
void MOVZX_RdMw(LPVCPU cpu);
void MOVZX_RdRb(LPVCPU cpu);
void MOVZX_RdRw(LPVCPU cpu);
void MOVZX_RwMb(LPVCPU cpu);
void MOVZX_RwRb(LPVCPU cpu);
void MOV_CdRd(LPVCPU cpu);
void MOV_DdRd(LPVCPU cpu);
void MOV_MbIb(LPVCPU cpu);
void MOV_MbRb(LPVCPU cpu);
void MOV_MdId(LPVCPU cpu);
void MOV_MdRd(LPVCPU cpu);
void MOV_MwIw(LPVCPU cpu);
void MOV_MwRw(LPVCPU cpu);
void MOV_MwSw(LPVCPU cpu);
void MOV_RbIb(LPVCPU cpu);
void MOV_RbMb(LPVCPU cpu);
void MOV_RbRb(LPVCPU cpu);
void MOV_RdCd(LPVCPU cpu);
void MOV_RdDd(LPVCPU cpu);
void MOV_RdId(LPVCPU cpu);
void MOV_RdMd(LPVCPU cpu);
void MOV_RdRd(LPVCPU cpu);
void MOV_RdTd(LPVCPU cpu);
void MOV_RwIw(LPVCPU cpu);
void MOV_RwMw(LPVCPU cpu);
void MOV_RwRw(LPVCPU cpu);
void MOV_RwSw(LPVCPU cpu);
void MOV_SwMw(LPVCPU cpu);
void MOV_SwRw(LPVCPU cpu);
void MOV_TdRd(LPVCPU cpu);
void RDMSR_No(LPVCPU cpu);
void RDPMC_No(LPVCPU cpu);
void RDTSC_No(LPVCPU cpu);
void SGDT_MPd(LPVCPU cpu);
void SIDT_MPd(LPVCPU cpu);
void SMSW_Mw(LPVCPU cpu);
void SMSW_Rw(LPVCPU cpu);
void WRMSR_No(LPVCPU cpu);
void XCHG_MbRb(LPVCPU cpu);
void XCHG_MdRd(LPVCPU cpu);
void XCHG_MwRw(LPVCPU cpu);
void XCHG_RbMb(LPVCPU cpu);
void XCHG_RbRb(LPVCPU cpu);
void XCHG_RdMd(LPVCPU cpu);
void XCHG_RdRd(LPVCPU cpu);
void XCHG_RwMw(LPVCPU cpu);
void XCHG_RwRw(LPVCPU cpu);
void XLATB_No(LPVCPU cpu);
void STOSB_No(LPVCPU cpu);
void STOSD_No(LPVCPU cpu);
void STOSW_No(LPVCPU cpu);
void MOV_AbMb(LPVCPU cpu);
void MOV_AwMw(LPVCPU cpu);
void MOV_AdMd(LPVCPU cpu);
void MOV_MbAb(LPVCPU cpu);
void MOV_MwAw(LPVCPU cpu);
void MOV_MdAd(LPVCPU cpu);

void LAHF_No(LPVCPU cpu);
void SAHF_No(LPVCPU cpu);
void CLC_No(LPVCPU cpu);
void CLD_No(LPVCPU cpu);
void CLI_No(LPVCPU cpu);
void CMC_No(LPVCPU cpu);
void STC_No(LPVCPU cpu);
void STD_No(LPVCPU cpu);
void STI_No(LPVCPU cpu);

void INSB_No(LPVCPU cpu);
void INSD_No(LPVCPU cpu);
void INSW_No(LPVCPU cpu);
void IN_AbDw(LPVCPU cpu);
void IN_AbIb(LPVCPU cpu);
void IN_AdDw(LPVCPU cpu);
void IN_AdIb(LPVCPU cpu);
void IN_AwDw(LPVCPU cpu);
void IN_AwIb(LPVCPU cpu);
void OUTSB_No(LPVCPU cpu);
void OUTSD_No(LPVCPU cpu);
void OUTSW_No(LPVCPU cpu);
void OUT_DwAb(LPVCPU cpu);
void OUT_DwAd(LPVCPU cpu);
void OUT_DwAw(LPVCPU cpu);
void OUT_IbAb(LPVCPU cpu);
void OUT_IbAd(LPVCPU cpu);
void OUT_IbAw(LPVCPU cpu);

void BOUND_RdMq(LPVCPU cpu);
void BOUND_RwMd(LPVCPU cpu);
void CLTS_No(LPVCPU cpu);
void CPUID_No(LPVCPU cpu);
void ENTER_IwIb(LPVCPU cpu);
void NOP_No(LPVCPU cpu);
void WBINVD_No(LPVCPU cpu);
void LEAVE_No(LPVCPU cpu);
void INVD_No(LPVCPU cpu);
void INVLPG_Mu(LPVCPU cpu);
void INS_ERR(LPVCPU cpu);
void HLT_No(LPVCPU cpu);
void SMSW_Rd(LPVCPU cpu);
void SMSW_Md(LPVCPU cpu);



#endif
