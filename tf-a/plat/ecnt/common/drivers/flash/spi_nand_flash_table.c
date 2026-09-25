/***************************************************************************************
 *      Copyright(c) 2014 ECONET Incorporation All rights reserved.
 *
 *      This is unpublished proprietary source code of ECONET Networks Incorporation
 *
 *      The copyright notice above does not evidence any actual or intended
 *      publication of such source code.
 ***************************************************************************************
 */

/*======================================================================================
 * MODULE NAME: spi
 * FILE NAME: spi_nand_flash_table.c
 * DATE: 2017/08/06
 * VERSION: 1.00
 * PURPOSE: To Provide SPI NAND Access interface.
 * NOTES:
 *======================================================================================
 */

#include <ecnt_spi_nand_flash.h>

#if !defined(TCSUPPORT_BL2_OPTIMIZATION) || defined(FLASH_TABLE_OPEN) || !defined(IMAGE_BL2)

struct spi_nand_flash_ooblayout ooblayout_type1 = {
	.oobsize = 24, 
	.oobfree = {{4,4}, {16,8}, {32,8}, {48,4}, {0, 0}}
};

struct spi_nand_flash_ooblayout ooblayout_type2 = {
	.oobsize = 42, 
	.oobfree = {{4,12}, {20,12}, {36,12}, {52,8}, {0, 0}}
};

struct spi_nand_flash_ooblayout ooblayout_type3 = {
	.oobsize = 56, 
	.oobfree = {{4,56}, {0, 0}}
};

struct spi_nand_flash_ooblayout ooblayout_type4 = {
	.oobsize = 120, 
	.oobfree = {{4,120}, {0, 0}}
};

struct spi_nand_flash_ooblayout ooblayout_type5 = {
	.oobsize = 92, 
	.oobfree = {{4,12}, {20,12}, {36,12}, {52,12}, {68,12}, {84,12}, {100,12}, {116,8}, {0, 0}}
};

struct spi_nand_flash_ooblayout ooblayout_type6 = {
	.oobsize = 25, 
	.oobfree = {{11,5}, {24,8}, {40,8}, {56,4}, {0,0}}
};

struct spi_nand_flash_ooblayout ooblayout_type7 = {
	.oobsize = 20, 
	.oobfree = {{0,4}, {4,4}, {20,4}, {36,4}, {52,4}}
};

#if 0 /* this is same to ooblayout_type7 */
struct spi_nand_flash_ooblayout ooblayout_type8 = {
	.oobsize = 20, 
	.oobfree = {{0,4}, {4,4}, {20,4}, {36,4}, {52,4}}
};
#endif

struct spi_nand_flash_ooblayout ooblayout_type9 = {
	.oobsize = 56, 
	.oobfree = {{4,12}, {30,16}, {60,16}, {90,12}, {0,0}}
};

struct spi_nand_flash_ooblayout ooblayout_type10 = {
	.oobsize = 64, 
	.oobfree = {{4,14}, {32,18}, {64,18}, {96,14}, {0,0}}
};

struct spi_nand_flash_ooblayout ooblayout_type11 = {
	.oobsize = 136, 
	.oobfree = {{4,14}, {32,18}, {64,18}, {96,18}, {128,18}, {160,18}, {192,18}, {224,14}, {0,0}}
};

struct spi_nand_flash_ooblayout ooblayout_type12 = {
	.oobsize = 136, 
	.oobfree = {{4,136}, {0,0}}
};

struct spi_nand_flash_ooblayout ooblayout_type13 = {
	.oobsize = 28, 
	.oobfree = {{32,28}, {0,0}}
};

#if 0
struct spi_nand_flash_ooblayout ooblayout_type14 = {
	.oobsize = 20, 
	.oobfree = {{0,4}, {4,4}, {36,4}, {68,4}, {100,4}}
};
#endif

struct spi_nand_flash_ooblayout ooblayout_type15 = {
	.oobsize = 28, 
	.oobfree = {{4,8}, {20,8}, {36,8}, {52,4}, {0,0}}
};

#if 0
struct spi_nand_flash_ooblayout ooblayout_type16 = {
	.oobsize = 44, 
	.oobfree = {{0,4}, {4,2}, {19,2}, {34,2}, {49,2}, {96,32}}
};

struct spi_nand_flash_ooblayout ooblayout_type17 = {
	.oobsize = 36, 
	.oobfree = {{0,4}, {4,4}, {36,4}, {68,4}, {100,4}, {132,4}, {164,4}, {196,4}, {228,4}}
};
#endif

struct spi_nand_flash_ooblayout ooblayout_type18 = {
	.oobsize = 88, 
	.oobfree = {{4,20}, {32,24},  {64,24}, {96,20}, {0, 0}}
};

struct spi_nand_flash_ooblayout ooblayout_type19 = {
	.oobsize = 36, 
	.oobfree = {{8,36}, {0, 0}}
};

struct spi_nand_flash_ooblayout ooblayout_type20 = {
	.oobsize = 52, 
	.oobfree = {{4, 14}, {22, 14}, {40, 14}, {58, 10}, {0, 0}}
};

struct spi_nand_flash_ooblayout ooblayout_type21 = {
	.oobsize = 44,
	.oobfree = {{4,12}, {20,12}, {36,12}, {52,8}, {0, 0}}
};

struct spi_nand_flash_ooblayout ooblayout_type22 = {
	.oobsize = 8,
	.oobfree = {{36,4}, {68,4}, {0,0}}
};

/* XTX C-series (128-byte OOB): bad-block marker at byte 0, free bytes 8..127 */
struct spi_nand_flash_ooblayout ooblayout_type23 = {
	.oobsize = 120,
	.oobfree = {{8,120}, {0,0}}
};

/* ===== OOB reserve for feature7 ===== */
#if 0 //move to spi_nand_flash.c, because BL2 optimize
struct spi_nand_flash_ooblayout ooblayout_feature7 = {
	.oobsize = 3, 
	.oobfree = {{8, 3}, {0, 0}}
};
#endif

/* ===== SPI NAND Controller ECC ooblayout ===== */
struct spi_nand_flash_ooblayout ooblayout_spi_controller_ecc = {
	.oobsize = 24, 
	.oobfree = {{4, 24}, {0, 0}}
};

/*****************************[ Notice]******************************/
/* If new spi nand chip have page size more than 4KB,  or oob size more than 256 bytes,  than*/
/* it will need to adjust the #define of _SPI_NAND_PAGE_SIZE and _SPI_NAND_OOB_SIZE */
/*****************************[ Notice]******************************/

const struct SPI_NAND_FLASH_INFO_T spi_nand_flash_tables[] = {
	/* ===== Flash ECC ===== */

	/* ===== GigaDevice ===== */
#if 0
	{
		mfr_id:						_SPI_NAND_MANUFACTURER_ID_GIGADEVICE,
		dev_id:						_SPI_NAND_DEVICE_ID_GD5F1GQ4UAYIG,
		ptr_name:					"_SPI_NAND_DEVICE_ID_GD5F1GQ4UA",
		device_size:				_SPI_NAND_CHIP_SIZE_1GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_64BYTE,
		erase_size:					_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode:					SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout : 			&ooblayout_type15,
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_LOAD_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},
#endif
	{
		mfr_id:						_SPI_NAND_MANUFACTURER_ID_GIGADEVICE,
		dev_id:						_SPI_NAND_DEVICE_ID_GD5F1GQ4UBYIG,
		ptr_name:					(const u8 *) "_SPI_NAND_DEVICE_ID_GD5F1GQ4UB",
		device_size:				_SPI_NAND_CHIP_SIZE_1GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_128BYTE,
		erase_size:					_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode:					SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout : 			&ooblayout_type2,
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_LOAD_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},
#if 0
	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_GIGADEVICE,
		dev_id: 					_SPI_NAND_DEVICE_ID_GD5F1GQ4UCYIG,
		ptr_name:					"_SPI_NAND_DEVICE_ID_GD5F1GQ4UC",
		device_size:				_SPI_NAND_CHIP_SIZE_1GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_128BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_PREPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout : 			&ooblayout_type3,
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x70, 0x70},
		write_en_type:				SPI_NAND_FLASH_WRITE_LOAD_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},
#endif

	{
		mfr_id:						_SPI_NAND_MANUFACTURER_ID_GIGADEVICE,
		dev_id:						_SPI_NAND_DEVICE_ID_GD5F2GM7UEYIG,
		ptr_name:					(const u8 *) "_SPI_NAND_DEVICE_ID_GD5F2GM7UEYIG",
		device_size:				_SPI_NAND_CHIP_SIZE_2GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_128BYTE,
		erase_size:					_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode:					SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout:			&ooblayout_type2,
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_LOAD_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},

	{
		mfr_id:						_SPI_NAND_MANUFACTURER_ID_GIGADEVICE,
		dev_id:						_SPI_NAND_DEVICE_ID_GD5F2GQ4UBYIG,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_GD5F2GQ4UB",
		device_size:				_SPI_NAND_CHIP_SIZE_2GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_128BYTE,
		erase_size:					_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode:					SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout : 			&ooblayout_type2,
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_LOAD_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},

	{
		mfr_id:						_SPI_NAND_MANUFACTURER_ID_GIGADEVICE,
		dev_id:						_SPI_NAND_DEVICE_ID_GD5F2GQ4UE9IS,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_GD5F2GQ4UE9IS",
		device_size:				_SPI_NAND_CHIP_SIZE_2GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_128BYTE,
		erase_size:					_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode:					SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout : 			&ooblayout_type2,
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_LOAD_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},
#if 0
	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_GIGADEVICE,
		dev_id: 					_SPI_NAND_DEVICE_ID_GD5F2GQ4UCYIG,
		ptr_name:					"_SPI_NAND_DEVICE_ID_GD5F2GQ4UC",
		device_size:				_SPI_NAND_CHIP_SIZE_2GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_128BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_PREPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout : 			&ooblayout_type3,
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x70, 0x70},
		write_en_type:				SPI_NAND_FLASH_WRITE_LOAD_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},
#endif
	{
		mfr_id:						_SPI_NAND_MANUFACTURER_ID_GIGADEVICE,
		dev_id:						_SPI_NAND_DEVICE_ID_GD5F4GQ4UBYIG,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_GD5F4GQ4UB",
		device_size:				_SPI_NAND_CHIP_SIZE_4GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_4KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_256BYTE,
		erase_size:					_SPI_NAND_BLOCK_SIZE_256KBYTE,
		dummy_mode:					SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout : 			&ooblayout_type5,
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS | SPI_NAND_FLASH_READ_ECC_ERROR_BIT_CHECK,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_LOAD_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
		read_ecc_ceck:				{_SPI_NAND_CHECK_ECC_THROSHOLD_BY_2REG, {0x30, 0x10, 0x0}, {_SPI_NAND_ADDR_STATUS_6, 0x20, 0x30}},
	},
#if 0
	{
		mfr_id:						_SPI_NAND_MANUFACTURER_ID_GIGADEVICE,
		dev_id:						_SPI_NAND_DEVICE_ID_GD5F4GQ4UCYIG,
		ptr_name:					"_SPI_NAND_DEVICE_ID_GD5F4GQ4UC",
		device_size:				_SPI_NAND_CHIP_SIZE_4GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_4KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_256BYTE,
		erase_size:					_SPI_NAND_BLOCK_SIZE_256KBYTE,
		dummy_mode:					SPI_NAND_FLASH_READ_DUMMY_BYTE_PREPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout : 			&ooblayout_type4,
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x70, 0x70},
		write_en_type:				SPI_NAND_FLASH_WRITE_LOAD_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},	
#endif
	/* ===== ESMT ===== */
#if 0
	{
		mfr_id:						_SPI_NAND_MANUFACTURER_ID_ESMT,
		dev_id:						_SPI_NAND_DEVICE_ID_F50L512M41A,
		ptr_name:					"_SPI_NAND_DEVICE_ID_F50L512",
		device_size:				_SPI_NAND_CHIP_SIZE_512MBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_64BYTE,
		erase_size:					_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode:					SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout : 			&ooblayout_type6,
		feature:					 SPI_NAND_FLASH_OOB_RESERVE_FOR_BMT | SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x0, 0x0},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},
#endif
	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_ESMT,
		dev_id: 					_SPI_NAND_DEVICE_ID_F50L1G41A0,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_F50L1G",
		device_size:				_SPI_NAND_CHIP_SIZE_1GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_64BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout : 			&ooblayout_type6,
		feature:					SPI_NAND_FLASH_OOB_RESERVE_FOR_BMT | SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x0, 0x0},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},	

	{
		mfr_id:						_SPI_NAND_MANUFACTURER_ID_ESMT,
		dev_id:						_SPI_NAND_DEVICE_ID_F50L1G41LB,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_F50L1G41LB",
		device_size:				_SPI_NAND_CHIP_SIZE_1GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_64BYTE,
		erase_size:					_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode:					SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout : 			&ooblayout_type7,
		feature:					SPI_NAND_FLASH_FEATURE_NONE,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x7C, 0x0},
		quad_en:					{0x0, 0x0},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},

	{
		mfr_id:						_SPI_NAND_MANUFACTURER_ID_ESMT,
		dev_id:						_SPI_NAND_DEVICE_ID_F50L2G41LB,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_F50L2G41LB",
		device_size:				_SPI_NAND_CHIP_SIZE_2GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_64BYTE,
		erase_size:					_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode:					SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout : 			&ooblayout_type7,
		feature:					SPI_NAND_FLASH_DIE_SELECT_1_HAVE,
		die_num:					2,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x7C, 0x0},
		quad_en:					{0x0, 0x0},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},

	/* ===== Winbond ===== */
	{
		mfr_id:						_SPI_NAND_MANUFACTURER_ID_WINBOND,
		dev_id:						_SPI_NAND_DEVICE_ID_W25N01GV,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_W25N01G",
		device_size:				_SPI_NAND_CHIP_SIZE_1GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_64BYTE,
		erase_size:					_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode:					SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout : 			&ooblayout_type7,
		feature:					SPI_NAND_FLASH_BLANK_PAGE_FIXUP,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0xFB, 0x0},
		quad_en:					{0x0, 0x0},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				12,
#endif
		extend_dev_id:				_SPI_NAND_DEVICE_ID_W25N01GV_EXTEND,
	},

	{
		mfr_id:						_SPI_NAND_MANUFACTURER_ID_WINBOND,
		dev_id:						_SPI_NAND_DEVICE_ID_W25N01KV,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_W25N01KV",
		device_size:				_SPI_NAND_CHIP_SIZE_1GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_96BYTE,
		erase_size:					_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode:					SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout : 			&ooblayout_type21,
		feature:					SPI_NAND_FLASH_READ_ECC_ERROR_BIT_CHECK,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x78, 0x0},
		quad_en:					{0x0, 0x0},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				12,
#endif
		extend_dev_id:				_SPI_NAND_DEVICE_ID_W25N01KV_EXTEND,
		read_ecc_ceck:				{_SPI_NAND_CHECK_ECC_THROSHOLD_BY_FLASH, {0x30, 0x0, 0x0}, {_SPI_NAND_ADDR_EXTEND_BFD, 0x30, 0x0}},
	},

	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_WINBOND,
		dev_id: 					_SPI_NAND_DEVICE_ID_W25N02KV,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_W25N02K",
		device_size:				_SPI_NAND_CHIP_SIZE_2GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_128BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode: 				SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout :			&ooblayout_type21,
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS | SPI_NAND_FLASH_READ_ECC_ERROR_BIT_CHECK,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x7C, 0x0},
		quad_en:					{0x0, 0x0},
		ecc_en: 					{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				12,
#endif
		extend_dev_id:				_SPI_NAND_DEVICE_ID_W25N02KV_EXTEND,
		read_ecc_ceck:				{_SPI_NAND_CHECK_ECC_THROSHOLD_BY_FLASH, {0x30, 0x0, 0x0}, {_SPI_NAND_ADDR_EXTEND_BFD, 0x60, 0x0}},
	},

	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_WINBOND,
		dev_id: 					_SPI_NAND_DEVICE_ID_W25N04KV,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_W25N04K",
		device_size:				_SPI_NAND_CHIP_SIZE_4GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_128BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode: 				SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout :			&ooblayout_type21,
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS | SPI_NAND_FLASH_READ_ECC_ERROR_BIT_CHECK,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x7C, 0x0},
		quad_en:					{0x0, 0x0},
		ecc_en: 					{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DEVICE_ID_W25N04KV_EXTEND,
		read_ecc_ceck:				{_SPI_NAND_CHECK_ECC_THROSHOLD_BY_FLASH, {0x30, 0x0, 0x0}, {_SPI_NAND_ADDR_EXTEND_BFD, 0x60, 0x0}},
	},

	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_WINBOND,
		dev_id: 					_SPI_NAND_DEVICE_ID_W25M02GV,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_W25M02G",
		device_size:				_SPI_NAND_CHIP_SIZE_2GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_64BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout : 			&ooblayout_type7,
		feature:					SPI_NAND_FLASH_DIE_SELECT_1_HAVE | SPI_NAND_FLASH_BLANK_PAGE_FIXUP,
		die_num:					2,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0xFB, 0x0},
		quad_en:					{0x0, 0x0},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				12,
#endif
		extend_dev_id:				_SPI_NAND_DEVICE_ID_W25N02GV_EXTEND,
	},		

	/* ===== MXIC ===== */
	{
		mfr_id:						_SPI_NAND_MANUFACTURER_ID_MXIC,
		dev_id:						_SPI_NAND_DEVICE_ID_MXIC35LF1GE4AB,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_MXIC35LF1G",
		device_size:				_SPI_NAND_CHIP_SIZE_1GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_64BYTE,
		erase_size:					_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode:					SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout : 			&ooblayout_type7,
		feature:					SPI_NAND_FLASH_FEATURE_NONE,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},

	{
		mfr_id:						_SPI_NAND_MANUFACTURER_ID_MXIC,
		dev_id:						_SPI_NAND_DEVICE_ID_MXIC35LF2GE4AB,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_MXIC35LF2G",
		device_size:				_SPI_NAND_CHIP_SIZE_2GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_64BYTE,
		erase_size:					_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode:					SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout : 			&ooblayout_type2,
		feature:					SPI_NAND_FLASH_PLANE_SELECT_HAVE | SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},
	
	{
		mfr_id:						_SPI_NAND_MANUFACTURER_ID_MXIC,
		dev_id:						_SPI_NAND_DEVICE_ID_MXIC35LF2GE4AD,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_MXIC35LF2GE4AD",
		device_size:				_SPI_NAND_CHIP_SIZE_2GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_128BYTE,
		erase_size:					_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode:					SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout : 			&ooblayout_type2,
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},

	/* ===== Zentel ===== */
#if 0
	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_ZENTEL,
		dev_id: 					_SPI_NAND_DEVICE_ID_A5U12A21ASC,
		ptr_name:					"_SPI_NAND_DEVICE_ID_A5U12A21ASC",
		device_size:				_SPI_NAND_CHIP_SIZE_512MBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_64BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout : 			&ooblayout_type6,		
		feature:					SPI_NAND_FLASH_OOB_RESERVE_FOR_BMT | SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x0, 0x0},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},
	
	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_ZENTEL,
		dev_id: 					_SPI_NAND_DEVICE_ID_A5U1GA21BWS,
		ptr_name:					"_SPI_NAND_DEVICE_ID_A5U1GA21BWS",
		device_size:				_SPI_NAND_CHIP_SIZE_1GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_64BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout : 			&ooblayout_type6,		
		feature:					SPI_NAND_FLASH_OOB_RESERVE_FOR_BMT | SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x0, 0x0},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},	
#endif

	/* ===== Etron ===== */

	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_ETRON,
		dev_id: 					_SPI_NAND_DEVICE_ID_EM73C044SNB,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_EM73C044SNB",
		device_size:				_SPI_NAND_CHIP_SIZE_1GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_128BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout : 			&ooblayout_type9,		
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				4,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},

	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_ETRON,
		dev_id: 					_SPI_NAND_DEVICE_ID_EM73C044SND,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_EM73C044SND",
		device_size:				_SPI_NAND_CHIP_SIZE_1GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_64BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout : 			&ooblayout_type1,		
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				4,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},

	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_ETRON,
		dev_id: 					_SPI_NAND_DEVICE_ID_EM73C044SNF,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_EM73C044SNF",
		device_size:				_SPI_NAND_CHIP_SIZE_1GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_128BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout : 			&ooblayout_type10,		
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				4,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},

	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_ETRON,
		dev_id: 					_SPI_NAND_DEVICE_ID_EM73C044VCA,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_EM73C044VCA",
		device_size:				_SPI_NAND_CHIP_SIZE_1GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_64BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout : 			&ooblayout_type1,		
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				4,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},

	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_ETRON,
		dev_id: 					_SPI_NAND_DEVICE_ID_EM73C044VCD,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_EM73C044VCD",
		device_size:				_SPI_NAND_CHIP_SIZE_1GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_64BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout : 			&ooblayout_type1,		
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				4,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},

	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_ETRON,
		dev_id: 					_SPI_NAND_DEVICE_ID_EM73D044SNA,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_EM73D044SNA",
		device_size:				_SPI_NAND_CHIP_SIZE_2GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_128BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout :			&ooblayout_type10,		
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				4,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},

	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_ETRON,
		dev_id: 					_SPI_NAND_DEVICE_ID_EM73D044SNC,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_EM73D044SNC",
		device_size:				_SPI_NAND_CHIP_SIZE_2GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_128BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout :			&ooblayout_type9,		
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				4,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},

	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_ETRON,
		dev_id: 					_SPI_NAND_DEVICE_ID_EM73D044SND,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_EM73D044SND",
		device_size:				_SPI_NAND_CHIP_SIZE_2GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_64BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout :			&ooblayout_type1,
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				4,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},

	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_ETRON,
		dev_id: 					_SPI_NAND_DEVICE_ID_EM73D044SNF,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_EM73D044SNF",
		device_size:				_SPI_NAND_CHIP_SIZE_2GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_128BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout :			&ooblayout_type10,		
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				4,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},

	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_ETRON,
		dev_id: 					_SPI_NAND_DEVICE_ID_EM73D044VCA,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_EM73D044VCA",
		device_size:				_SPI_NAND_CHIP_SIZE_2GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_128BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout :			&ooblayout_type18,		
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				4,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},

	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_ETRON,
		dev_id: 					_SPI_NAND_DEVICE_ID_EM73D044VCB,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_EM73D044VCB",
		device_size:				_SPI_NAND_CHIP_SIZE_2GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_64BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout :			&ooblayout_type1,		
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				4,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},

	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_ETRON,
		dev_id: 					_SPI_NAND_DEVICE_ID_EM73D044VCD,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_EM73D044VCD",
		device_size:				_SPI_NAND_CHIP_SIZE_2GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_128BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout :			&ooblayout_type10,		
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				4,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},

	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_ETRON,
		dev_id: 					_SPI_NAND_DEVICE_ID_EM73D044VCG,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_EM73D044VCG",
		device_size:				_SPI_NAND_CHIP_SIZE_2GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_64BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout :			&ooblayout_type1,		
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				4,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},

	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_ETRON,
		dev_id: 					_SPI_NAND_DEVICE_ID_EM73D044VCH,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_EM73D044VCH",
		device_size:				_SPI_NAND_CHIP_SIZE_2GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_64BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout :			&ooblayout_type1,		
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				4,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},

	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_ETRON,
		dev_id: 					_SPI_NAND_DEVICE_ID_EM73E044SNA,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_EM73E044SNA",
		device_size:				_SPI_NAND_CHIP_SIZE_4GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_4KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_256BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_256KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout :			&ooblayout_type11,		
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				4,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},

	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_ETRON,
		dev_id: 					_SPI_NAND_DEVICE_ID_EM73F044SNA,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_EM73F044SNA",
		device_size:				_SPI_NAND_CHIP_SIZE_8GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_4KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_256BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_256KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout :			&ooblayout_type12,		
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				4,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},
	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_ETRON,
		dev_id: 					_SPI_NAND_DEVICE_ID_EM73E044VCA,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_EM73E044VCA",
		device_size:				_SPI_NAND_CHIP_SIZE_4GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_4KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_256BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_256KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode: 				SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout :			&ooblayout_type12,		
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en: 					{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},

	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_ETRON,
		dev_id: 					_SPI_NAND_DEVICE_ID_EM73E044VCB,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_EM73E044VCB",
		device_size:				_SPI_NAND_CHIP_SIZE_4GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_128BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode: 				SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout :			&ooblayout_type20,		
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en: 					{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},


	/* ===== Toshiba ===== */
	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_TOSHIBA,
		dev_id: 					_SPI_NAND_DEVICE_ID_TC58CVG0S3H,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_TC58CVG0S3H",
		device_size:				_SPI_NAND_CHIP_SIZE_1GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_64BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout : 			&ooblayout_type3,		
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x0, 0x0},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},		

	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_TOSHIBA,
		dev_id: 					_SPI_NAND_DEVICE_ID_TC58CVG1S3H,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_TC58CVG1S3H",
		device_size:				_SPI_NAND_CHIP_SIZE_2GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_64BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout : 			&ooblayout_type3,		
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x0, 0x0},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	}, 

	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_TOSHIBA,
		dev_id: 					_SPI_NAND_DEVICE_ID_TC58CVG2S0H,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_TC58CVG2S0H",
		device_size:				_SPI_NAND_CHIP_SIZE_4GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_4KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_128BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_256KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout : 			&ooblayout_type4,		
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x0, 0x0},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	}, 

	/* ===== Micron ===== */
	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_MICRON,
		dev_id: 					_SPI_NAND_DEVICE_ID_MT29F1G01,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_MT29F1G01",
		device_size:				_SPI_NAND_CHIP_SIZE_1GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_128BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout :			&ooblayout_type13, 	
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x70, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x7C, 0x0},
		quad_en:					{0x0, 0x0},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},		

	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_MICRON,
		dev_id: 					_SPI_NAND_DEVICE_ID_MT29F2G01,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_MT29F2G01",
		device_size:				_SPI_NAND_CHIP_SIZE_2GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_128BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout :			&ooblayout_type13, 	
		feature:					SPI_NAND_FLASH_PLANE_SELECT_HAVE | SPI_NAND_FLASH_ERASE_STATISTICS | SPI_NAND_FLASH_READ_ECC_ERROR_BIT_CHECK,
		die_num:					1,
		ecc_fail_check_info:		{0x70, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x7C, 0x0},
		quad_en:					{0x0, 0x0},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
		read_ecc_ceck:				{_SPI_NAND_CHECK_ECC_THROSHOLD_BY_REG, {0x50, 0x0, 0x0}, {0x0, 0x0, 0x0}},
	},

	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_MICRON,
		dev_id: 					_SPI_NAND_DEVICE_ID_MT29F4G01,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_MT29F4G01",
		device_size:				_SPI_NAND_CHIP_SIZE_4GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_128BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout :			&ooblayout_type13, 	
		feature:					SPI_NAND_FLASH_PLANE_SELECT_HAVE | SPI_NAND_FLASH_DIE_SELECT_2_HAVE | SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					2,
		ecc_fail_check_info:		{0x70, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x7C, 0x0},
		quad_en:					{0x0, 0x0},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},

	/* ===== HeYangTek ===== */
	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_HEYANG,
		dev_id: 					_SPI_NAND_DEVICE_ID_HYF2GQ4UAACAE,
		ptr_name:					(const u8 *) "_SPI_NAND_DEVICE_ID_HYF2GQ4UAACAE",
		device_size:				_SPI_NAND_CHIP_SIZE_2GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_128BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout :			&ooblayout_type22, 	
		feature:					SPI_NAND_FLASH_BLANK_PAGE_FIXUP | SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},

	/* ===== SKYHIGH ===== */
	{
		mfr_id:						_SPI_NAND_MANUFACTURER_ID_SKYHIGH,
		dev_id:						_SPI_NAND_DEVICE_ID_S35ML02G3,
		ptr_name:					(const u8 *) "_SPI_NAND_DEVICE_ID_S35ML02G3",
		device_size:				_SPI_NAND_CHIP_SIZE_2GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_128BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode: 				SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout :			&ooblayout_type4,
		feature:					SPI_NAND_FLASH_BLANK_PAGE_FIXUP | SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x30},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x7A, 0x2},
		quad_en:					{0x00, 0x00},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},

#if 0 /* not verify */
{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_MICRON,
		dev_id: 					_SPI_NAND_DEVICE_ID_MT29F4G01_33V,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_MT29F4G01_33V",
		device_size:				_SPI_NAND_CHIP_SIZE_4GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_4KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_256BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_256KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout :			&ooblayout_type4, 	
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x70, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x7C, 0x0},
		quad_en:					{0x0, 0x0},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},
#endif

#if 0
	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_MICRON,
		dev_id: 					_SPI_NAND_DEVICE_ID_MT29F4G01AD,
		ptr_name:					"_SPI_NAND_DEVICE_ID_MT29F4G01AD",
		device_size:				_SPI_NAND_CHIP_SIZE_4GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_4KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_128BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_256KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout :			&ooblayout_type4, 	
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x70, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x7C, 0x0},
		quad_en:					{0x0, 0x0},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},
#endif

#if 0
	/* ===== HeYangTek ===== */
	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_HEYANG,
		dev_id: 					_SPI_NAND_DEVICE_ID_HYF1GQ4UAACAE,
		ptr_name:					"_SPI_NAND_DEVICE_ID_HYF1GQ4UAACAE",
		device_size:				_SPI_NAND_CHIP_SIZE_1GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_128BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout :			&ooblayout_type14, 	
		feature:					SPI_NAND_FLASH_FEATURE_NONE,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},

	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_HEYANG,
		dev_id: 					_SPI_NAND_DEVICE_ID_HYF2GQ4UHCCAE,
		ptr_name:					"_SPI_NAND_DEVICE_ID_HYF2GQ4UHCCAE",
		device_size:				_SPI_NAND_CHIP_SIZE_2GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_128BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout :			&ooblayout_type14, 	
		feature:					SPI_NAND_FLASH_FEATURE_NONE,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},

	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_HEYANG,
		dev_id: 					_SPI_NAND_DEVICE_ID_HYF4GQ4UAACBE,
		ptr_name:					"_SPI_NAND_DEVICE_ID_HYF4GQ4UAACBE",
		device_size:				_SPI_NAND_CHIP_SIZE_4GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_4KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_256BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_256KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout :			&ooblayout_type17, 	
		feature:					SPI_NAND_FLASH_FEATURE_NONE,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},

	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_HEYANG,
		dev_id: 					_SPI_NAND_DEVICE_ID_HYF1GQ4UDACAE,
		ptr_name:					"_SPI_NAND_DEVICE_ID_HYF1GQ4UDACAE",
		device_size:				_SPI_NAND_CHIP_SIZE_1GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_64BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout :			&ooblayout_type7,
		feature:					SPI_NAND_FLASH_FEATURE_NONE,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},

	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_HEYANG,
		dev_id: 					_SPI_NAND_DEVICE_ID_HYF2GQ4UDACAE,
		ptr_name:					"_SPI_NAND_DEVICE_ID_HYF2GQ4UDACAE",
		device_size:				_SPI_NAND_CHIP_SIZE_2GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_64BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout :			&ooblayout_type7,
		feature:					SPI_NAND_FLASH_FEATURE_NONE,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},
#endif

#if !defined(TCSUPPORT_AUTOBENCH)
#if 0
	/* ===== PN ===== */
	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_PN,
		dev_id: 					_SPI_NAND_DEVICE_ID_PN26G01AWSIUG,
		ptr_name:					"_SPI_NAND_DEVICE_ID_PN26G01A-X",
		device_size:				_SPI_NAND_CHIP_SIZE_1GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_128BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout :			&ooblayout_type16, 	
		feature:					SPI_NAND_FLASH_FEATURE_NONE,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_LOAD_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_ECC, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},

	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_PN,
		dev_id: 					_SPI_NAND_DEVICE_ID_PN26G02AWSIUG,
		ptr_name:					"_SPI_NAND_DEVICE_ID_PN26G02A-X",
		device_size:				_SPI_NAND_CHIP_SIZE_2GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_128BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout :			&ooblayout_type16, 	
		feature:					SPI_NAND_FLASH_FEATURE_NONE,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_LOAD_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_ECC, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},
#endif

	/* ===== ATO ===== */
	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_ATO,
		dev_id: 					_SPI_NAND_DEVICE_ID_ATO25D1GA,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_ATO25D1GA",
		device_size:				_SPI_NAND_CHIP_SIZE_1GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_64BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_SINGLE,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout :			&ooblayout_type3, 	
		feature:					SPI_NAND_FLASH_NO_ECC_STATUS_HAVE | SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x0, 0x0},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x00, 0x00},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},

	/* ===== FM ===== */
	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_FM,
		dev_id: 					_SPI_NAND_DEVICE_ID_FM25S01,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_FM25S01",
		device_size:				_SPI_NAND_CHIP_SIZE_1GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_128BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout :			&ooblayout_type3, 	
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x78, 0x0},
		quad_en:					{0x00, 0x00},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},
	
	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_FM,
		dev_id: 					_SPI_NAND_DEVICE_ID_FM25G01B,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_FM25G01B",
		device_size:				_SPI_NAND_CHIP_SIZE_1GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_128BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout :			&ooblayout_type3, 	
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x70, 0x70},
		write_en_type:				SPI_NAND_FLASH_WRITE_LOAD_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_ECC, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},

	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_FM,
		dev_id: 					_SPI_NAND_DEVICE_ID_FM25G02B,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_FM25G02B",
		device_size:				_SPI_NAND_CHIP_SIZE_2GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_128BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout :			&ooblayout_type3, 	
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x70, 0x70},
		write_en_type:				SPI_NAND_FLASH_WRITE_LOAD_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_ECC, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},

	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_FM,
		dev_id: 					_SPI_NAND_DEVICE_ID_FM25G02,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_FM25G02",
		device_size:				_SPI_NAND_CHIP_SIZE_2GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_64BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout :			&ooblayout_type1, 	
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x70, 0x70},
		write_en_type:				SPI_NAND_FLASH_WRITE_LOAD_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_ECC, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},

	/* ===== XTX ===== */
	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_XTX,
		dev_id: 					_SPI_NAND_DEVICE_ID_XT26G02B,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_XT26G02B",
		device_size:				_SPI_NAND_CHIP_SIZE_2GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_64BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout :			&ooblayout_type1, 	
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x70, 0x70},
		write_en_type:				SPI_NAND_FLASH_WRITE_LOAD_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_ECC, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},

	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_XTX,
		dev_id: 					_SPI_NAND_DEVICE_ID_XT26G01A,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_XT26G01A",
		device_size:				_SPI_NAND_CHIP_SIZE_1GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_64BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout :			&ooblayout_type19, 	
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x3C, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_LOAD_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},

	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_XTX,
		dev_id: 					_SPI_NAND_DEVICE_ID_XT26G02A,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_XT26G02A",
		device_size:				_SPI_NAND_CHIP_SIZE_2GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_64BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout :			&ooblayout_type19, 	
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_LOAD_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},

	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_XTX,
		dev_id: 					_SPI_NAND_DEVICE_ID_XT26G01C,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_XT26G01C",
		device_size:				_SPI_NAND_CHIP_SIZE_1GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_128BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout :			&ooblayout_type23,
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_LOAD_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},

	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_XTX,
		dev_id: 					_SPI_NAND_DEVICE_ID_XT26G02C,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_XT26G02C",
		device_size:				_SPI_NAND_CHIP_SIZE_2GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_128BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout :			&ooblayout_type23,
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_LOAD_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},


	/* ===== MIRA ===== */
	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_MIRA,
		dev_id: 					_SPI_NAND_DEVICE_ID_PSU1GS20BN,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_PSU1GS20BN",
		device_size:				_SPI_NAND_CHIP_SIZE_1GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_64BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout : 			&ooblayout_type6,
		feature:					SPI_NAND_FLASH_OOB_RESERVE_FOR_BMT | SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x0, 0x0},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},

	/* ===== BIWIN ===== */
	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_BIWIN,
		dev_id: 					_SPI_NAND_DEVICE_ID_BWJX08U,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_BWJX08U",
		device_size:				_SPI_NAND_CHIP_SIZE_2GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_64BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout :			&ooblayout_type15, 	
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},

	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_BIWIN,
		dev_id: 					_SPI_NAND_DEVICE_ID_BWET08U,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_BWET08U",
		device_size:				_SPI_NAND_CHIP_SIZE_2GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_128BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout :			&ooblayout_type10, 	
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x30, 0x20},
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},

	/* ===== FORESEE ===== */
	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_FORESEE,
		dev_id: 					_SPI_NAND_DEVICE_ID_FS35ND01GD1F1,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_FS35ND01GD1F1",
		device_size:				_SPI_NAND_CHIP_SIZE_1GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_64BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout :			&ooblayout_type1, 	
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x70, 0x70},
		write_en_type:				SPI_NAND_FLASH_WRITE_LOAD_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_ECC, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},
	
	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_FORESEE,
		dev_id: 					_SPI_NAND_DEVICE_ID_FS35ND02GS2F1,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_FS35ND02GS2F1",
		device_size:				_SPI_NAND_CHIP_SIZE_2GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_128BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout :			&ooblayout_type1, 	
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x70, 0x70},
		write_en_type:				SPI_NAND_FLASH_WRITE_LOAD_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_ECC, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},

	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_FORESEE,
		dev_id: 					_SPI_NAND_DEVICE_ID_FS35ND02GD1F1,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_FS35ND02GD1F1",
		device_size:				_SPI_NAND_CHIP_SIZE_2GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_128BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout :			&ooblayout_type1, 	
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x70, 0x70},
		write_en_type:				SPI_NAND_FLASH_WRITE_LOAD_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_ECC, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},

	/* ===== DOSILICON ===== */
	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_DOSILICON,
		dev_id: 					_SPI_NAND_DEVICE_ID_DS35Q1GA,
		ptr_name:					(const u8 *) " DS35Q1GA",
		device_size:				_SPI_NAND_CHIP_SIZE_1GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_64BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout :			&ooblayout_type1, 	
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x70, 0x70},
		write_en_type:				SPI_NAND_FLASH_WRITE_LOAD_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_ECC, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},

	{
		mfr_id: 					_SPI_NAND_MANUFACTURER_ID_DOSILICON,
		dev_id: 					_SPI_NAND_DEVICE_ID_DS35M1GA,
		ptr_name:					(const u8 *) " DS35M1GA",
		device_size:				_SPI_NAND_CHIP_SIZE_1GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_64BYTE,
		erase_size: 				_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode: 				SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout :			&ooblayout_type1, 	
		feature:					SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		ecc_fail_check_info:		{0x70, 0x70},
		write_en_type:				SPI_NAND_FLASH_WRITE_LOAD_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_ECC, 0x10, 0x10},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
	},

	/* ===== SPI controller ECC ===== */
		
	/* ===== MXIC ===== */
	{
		mfr_id:						_SPI_NAND_MANUFACTURER_ID_MXIC,
		dev_id:						_SPI_NAND_DEVICE_ID_MXIC35LF2G14AC,
		ptr_name:					(const u8 *) " _SPI_NAND_DEVICE_ID_MXIC35LF2G14AC",
		device_size:				_SPI_NAND_CHIP_SIZE_2GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_64BYTE,
		erase_size:					_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode:					SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout : 			&ooblayout_spi_controller_ecc,
		feature:					SPI_NAND_FLASH_PLANE_SELECT_HAVE | SPI_NAND_FLASH_ERASE_STATISTICS,
		die_num:					1,
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x00, 0x00},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
		soc_ecc_ability:			4,
	},
#endif
	{
		mfr_id:						_SPI_NAND_MANUFACTURER_ID_MXIC,
		dev_id:						_SPI_NAND_DEVICE_ID_MXIC35LF1G24AD,
		ptr_name:					(const u8 *)"_SPI_NAND_DEVICE_ID_MXIC35LF1G24AD",
		device_size:				_SPI_NAND_CHIP_SIZE_1GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_128BYTE,
		erase_size:					_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode:					SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout : 			&ooblayout_spi_controller_ecc,
		feature:					SPI_NAND_FLASH_NO_ECC_STATUS_HAVE | SPI_NAND_FLASH_DISABLE_CONTINUOUS_RD,
		die_num:					1,
		write_en_type:				SPI_NAND_FLASH_WRITE_EN_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x00, 0x00},
#ifdef TCSUPPORT_NAND_FLASH_OTP
		otp_page_num:				-1,
#endif
		soc_ecc_ability:			12,
	},

#if 0
	/* ===== GD ===== */
	{
		mfr_id:						_SPI_NAND_MANUFACTURER_ID_GIGADEVICE,
		dev_id:						_SPI_NAND_DEVICE_ID_GD5E1GQ4UBYIG,
		ptr_name:					(const u8 *) "_SPI_NAND_DEVICE_ID_GD5E1GQ4UBYIG",
		device_size:				_SPI_NAND_CHIP_SIZE_1GBIT,
		page_size:					_SPI_NAND_PAGE_SIZE_2KBYTE,
		oob_size:					_SPI_NAND_OOB_SIZE_128BYTE,
		erase_size:					_SPI_NAND_BLOCK_SIZE_128KBYTE,
		dummy_mode:					SPI_NAND_FLASH_READ_DUMMY_BYTE_APPEND,
		read_mode:					SPI_NAND_FLASH_READ_SPEED_MODE_DUAL,
		write_mode:					SPI_NAND_FLASH_WRITE_SPEED_MODE_SINGLE,
		oob_free_layout : 			&ooblayout_spi_controller_ecc,
		feature:					SPI_NAND_FLASH_FEATURE_NONE,
		die_num:					1,
		write_en_type:				SPI_NAND_FLASH_WRITE_LOAD_FIRST,
		unlock_block_info:			{0x38, 0x0},
		quad_en:					{0x01, 0x01},
		ecc_en:						{_SPI_NAND_ADDR_FEATURE, 0x00, 0x00},
		extend_dev_id:				_SPI_NAND_DUMMY_EXTEND_DEVICE_ID,
		soc_ecc_ability:			12,
	},
#endif
};

#else

#include <platform_def.h>


struct SPI_NAND_FLASH_INFO_T spi_nand_flash_tables[1];
/* ===== SPI NAND Controller ECC ooblayout ===== */
struct spi_nand_flash_ooblayout ooblayout_spi_controller_ecc;

#endif

#ifndef FLASH_TABLE_OPEN

static SPI_NAND_EXTEND_DEVICE_ID_STATUS_T check_extend_dev_id (struct SPI_NAND_FLASH_INFO_T *ptr_rtn_device_t, u32 index)
{
	u8 i, result = SPI_NAND_EXTEND_DEVICE_ID_MATCH;

	for (i=0 ; i<spi_nand_flash_tables[index].extend_dev_id.extend_len ; i++)
	{
		if (ptr_rtn_device_t->extend_dev_id.extend_id[i] != spi_nand_flash_tables[index].extend_dev_id.extend_id[i])
		{
			result = SPI_NAND_EXTEND_DEVICE_ID_UNMATCH;

			break;
		}
	}

	return result;
}

#if defined(TCSUPPORT_BL2_OPTIMIZATION) && defined(IMAGE_BL2)

SPI_NAND_FLASH_RTN_T get_spi_nand_from_mem(struct SPI_NAND_FLASH_INFO_T *ptr_rtn_device_t)
{

	int i = 0;
	int len = 0;
	int index = 0;
	unsigned char * currentP = (unsigned char *)ECNT_FLASH_TABLE_BASE;

	struct bl2_flash_H flash_h;

	memcpy(&flash_h, currentP, sizeof(struct bl2_flash_H));

	currentP += sizeof(struct bl2_flash_H);

	for (i = 0; i < flash_h.flash_entry; i++)
	{
		memcpy(&spi_nand_flash_tables[0].mfr_id, currentP, sizeof(spi_nand_flash_tables[0].mfr_id));
		currentP+=sizeof(spi_nand_flash_tables[0].mfr_id);
		memcpy(&spi_nand_flash_tables[0].dev_id, currentP, sizeof(spi_nand_flash_tables[0].dev_id));
		currentP+=sizeof(spi_nand_flash_tables[0].dev_id);
		memcpy(&spi_nand_flash_tables[0].device_size, currentP, sizeof(spi_nand_flash_tables[0].device_size));
		currentP+=sizeof(spi_nand_flash_tables[0].device_size);
		memcpy(&spi_nand_flash_tables[0].page_size, currentP, sizeof(spi_nand_flash_tables[0].page_size));
		currentP+=sizeof(spi_nand_flash_tables[0].page_size);
		memcpy(&spi_nand_flash_tables[0].erase_size, currentP, sizeof(spi_nand_flash_tables[0].erase_size));
		currentP+=sizeof(spi_nand_flash_tables[0].erase_size);
		memcpy(&spi_nand_flash_tables[0].oob_size, currentP, sizeof(spi_nand_flash_tables[0].oob_size));
		currentP+=sizeof(spi_nand_flash_tables[0].oob_size);
		memcpy(&spi_nand_flash_tables[0].dummy_mode, currentP, sizeof(spi_nand_flash_tables[0].dummy_mode));
		currentP+=sizeof(spi_nand_flash_tables[0].dummy_mode);
		memcpy(&spi_nand_flash_tables[0].read_mode, currentP, sizeof(spi_nand_flash_tables[0].read_mode));
		currentP+=sizeof(spi_nand_flash_tables[0].read_mode);
		memcpy(&spi_nand_flash_tables[0].die_num, currentP, sizeof(spi_nand_flash_tables[0].die_num));
		currentP+=sizeof(spi_nand_flash_tables[0].die_num);
		memcpy(&spi_nand_flash_tables[0].write_mode, currentP, sizeof(spi_nand_flash_tables[0].write_mode));
		currentP+=sizeof(spi_nand_flash_tables[0].write_mode);
		memcpy(&spi_nand_flash_tables[0].feature, currentP, sizeof(spi_nand_flash_tables[0].feature));
		currentP+=sizeof(spi_nand_flash_tables[0].feature);
		memcpy(&spi_nand_flash_tables[0].ecc_fail_check_info, currentP, sizeof(spi_nand_flash_tables[0].ecc_fail_check_info));
		currentP+=sizeof(spi_nand_flash_tables[0].ecc_fail_check_info);
		memcpy(&spi_nand_flash_tables[0].write_en_type, currentP, sizeof(spi_nand_flash_tables[0].write_en_type));
		currentP+=sizeof(spi_nand_flash_tables[0].write_en_type);
		memcpy(&spi_nand_flash_tables[0].unlock_block_info, currentP, sizeof(spi_nand_flash_tables[0].unlock_block_info));
		currentP+=sizeof(spi_nand_flash_tables[0].unlock_block_info);
		memcpy(&spi_nand_flash_tables[0].quad_en, currentP, sizeof(spi_nand_flash_tables[0].quad_en));
		currentP+=sizeof(spi_nand_flash_tables[0].quad_en);
		memcpy(&spi_nand_flash_tables[0].ecc_en, currentP, sizeof(spi_nand_flash_tables[0].ecc_en));
		currentP+=sizeof(spi_nand_flash_tables[0].ecc_en);
#ifdef TCSUPPORT_NAND_FLASH_OTP
		memcpy(&spi_nand_flash_tables[0].otp_page_num, currentP, sizeof(spi_nand_flash_tables[0].otp_page_num));
		currentP+=sizeof(spi_nand_flash_tables[0].otp_page_num);
#endif
		memcpy(&spi_nand_flash_tables[0].soc_ecc_ability, currentP, sizeof(spi_nand_flash_tables[0].soc_ecc_ability));
		currentP+=sizeof(spi_nand_flash_tables[0].soc_ecc_ability);

		memcpy(&spi_nand_flash_tables[0].extend_dev_id, currentP, sizeof(spi_nand_flash_tables[0].extend_dev_id));
		currentP+=sizeof(spi_nand_flash_tables[0].extend_dev_id);

		if (((ptr_rtn_device_t->mfr_id == spi_nand_flash_tables[0].mfr_id) && (ptr_rtn_device_t->dev_id == spi_nand_flash_tables[0].dev_id) && 
			(check_extend_dev_id(ptr_rtn_device_t, 0) == SPI_NAND_EXTEND_DEVICE_ID_MATCH)))
		{
			index = i;
			/* skip scanning the rest table*/
			currentP = (unsigned char *)(ECNT_FLASH_TABLE_BASE + flash_h.flash_name_off);

			break;
		}

	}

	for (i = 0; i < flash_h.flash_entry; i++)
	{
		memcpy(&len, currentP, sizeof(int));
		currentP+=sizeof(int);
	
		if(i == index)
		{
			/* assign pointer to ptr_name.*/
			spi_nand_flash_tables[0].ptr_name = currentP;
			currentP = (unsigned char *)(ECNT_FLASH_TABLE_BASE + flash_h.flash_oob_off);
			break;

		}
		currentP+=len;

	}

	for (i = 0; i < flash_h.flash_entry; i++)
	{
		if(i == index)
		{
			spi_nand_flash_tables[0].oob_free_layout = (struct spi_nand_flash_ooblayout *)currentP;
			break;
		}
		currentP+=sizeof(struct spi_nand_flash_ooblayout);


	}

	return SPI_NAND_FLASH_RTN_NO_ERROR;
}

#endif

SPI_NAND_FLASH_RTN_T scan_spi_nand_table(struct SPI_NAND_FLASH_INFO_T *ptr_rtn_device_t)
{
	u32 i = 0;
	SPI_NAND_FLASH_RTN_T rtn_status = SPI_NAND_FLASH_RTN_PROBE_ERROR;

#if defined(TCSUPPORT_BL2_OPTIMIZATION) && defined(IMAGE_BL2)
	get_spi_nand_from_mem(ptr_rtn_device_t);
	
	for (i = 0; i < 1; i++)
#else
	for (i = 0; i < (sizeof(spi_nand_flash_tables) / sizeof(spi_nand_flash_tables[0])) ; i++) 
#endif
	{
		if ( ( (ptr_rtn_device_t->mfr_id) == spi_nand_flash_tables[i].mfr_id) &&
		     ( (ptr_rtn_device_t->dev_id) == spi_nand_flash_tables[i].dev_id) &&
		     (check_extend_dev_id(ptr_rtn_device_t, i) == SPI_NAND_EXTEND_DEVICE_ID_MATCH) ) 
		{
			ptr_rtn_device_t->device_size 			= spi_nand_flash_tables[i].device_size;
			ptr_rtn_device_t->erase_size  			= spi_nand_flash_tables[i].erase_size;
			ptr_rtn_device_t->page_size	  			= spi_nand_flash_tables[i].page_size;
			ptr_rtn_device_t->oob_size	  			= spi_nand_flash_tables[i].oob_size;
			ptr_rtn_device_t->dummy_mode  			= spi_nand_flash_tables[i].dummy_mode;
			ptr_rtn_device_t->read_mode	  			= spi_nand_flash_tables[i].read_mode;
			ptr_rtn_device_t->write_mode			= spi_nand_flash_tables[i].write_mode;
#ifdef TCSUPPORT_NAND_FLASH_OTP
			ptr_rtn_device_t->otp_page_num			= spi_nand_flash_tables[i].otp_page_num;
#endif
			ptr_rtn_device_t->soc_ecc_ability			= spi_nand_flash_tables[i].soc_ecc_ability;
			memcpy( &(ptr_rtn_device_t->ptr_name) , &(spi_nand_flash_tables[i].ptr_name), sizeof(ptr_rtn_device_t->ptr_name));
			memcpy( &(ptr_rtn_device_t->oob_free_layout) , &(spi_nand_flash_tables[i].oob_free_layout), sizeof(ptr_rtn_device_t->oob_free_layout));
			ptr_rtn_device_t->feature = spi_nand_flash_tables[i].feature;
			ptr_rtn_device_t->die_num = spi_nand_flash_tables[i].die_num;
			memcpy( &(ptr_rtn_device_t->ecc_fail_check_info) , &(spi_nand_flash_tables[i].ecc_fail_check_info), sizeof(struct SPI_NAND_ECC_FAIL_CHECK_INFO_T));
			ptr_rtn_device_t->write_en_type = spi_nand_flash_tables[i].write_en_type;
			memcpy( &(ptr_rtn_device_t->unlock_block_info) , &(spi_nand_flash_tables[i].unlock_block_info), sizeof(struct SPI_NAND_UNLOCK_BLOCK_INFO_T));
			memcpy( &(ptr_rtn_device_t->quad_en) , &(spi_nand_flash_tables[i].quad_en), sizeof(struct SPI_NAND_QUAD_EN_INFO_T));
			memcpy( &(ptr_rtn_device_t->ecc_en) , &(spi_nand_flash_tables[i].ecc_en), sizeof(struct SPI_NAND_ECC_EN_INFO_T));
			memcpy( &(ptr_rtn_device_t->extend_dev_id) , &(spi_nand_flash_tables[i].extend_dev_id), sizeof(struct SPI_NAND_EXTEND_ID_T));
			memcpy( &(ptr_rtn_device_t->read_ecc_ceck) , &(spi_nand_flash_tables[i].read_ecc_ceck), sizeof(struct SPI_NAND_READ_ECC_CHECK_T));

			rtn_status = SPI_NAND_FLASH_RTN_NO_ERROR;
			break;
		}
	}

	return rtn_status;
}
#else

#include <stdio.h>
#include <stdlib.h>

int main(void){
	int num=sizeof(spi_nand_flash_tables) / sizeof(spi_nand_flash_tables[0]);
	int i;
	FILE *fp;
	int name_len = 0;
	unsigned int total_size = 0;
	int buf_size = 1000000; //16M
	struct bl2_flash_H flash_h;
	char *buf = NULL;
	fp = fopen("flash_table.bin","w+");
	if(fp == NULL){
		printf("open flash_table.bin fail!!\r\n");
		return 0;
	}

	if (!(buf = malloc(buf_size))) {
		printf("malloc failed\n");
		fclose(fp);
		return -1;
	}

	flash_h.flash_entry = num;

	total_size += sizeof(struct bl2_flash_H);

	for (i = 0 ; i < num ;i++)
	{
		memcpy((buf+total_size), &spi_nand_flash_tables[i].mfr_id, sizeof(spi_nand_flash_tables[i].mfr_id));
		total_size +=sizeof(spi_nand_flash_tables[i].mfr_id);

		memcpy((buf+total_size), &spi_nand_flash_tables[i].dev_id, sizeof(spi_nand_flash_tables[i].dev_id));
		total_size +=sizeof(spi_nand_flash_tables[i].dev_id);

		memcpy((buf+total_size), &spi_nand_flash_tables[i].device_size, sizeof(spi_nand_flash_tables[i].device_size));
		total_size +=sizeof(spi_nand_flash_tables[i].device_size);

		memcpy((buf+total_size), &spi_nand_flash_tables[i].page_size, sizeof(spi_nand_flash_tables[i].page_size));
		total_size +=sizeof(spi_nand_flash_tables[i].page_size);

		memcpy((buf+total_size), &spi_nand_flash_tables[i].erase_size, sizeof(spi_nand_flash_tables[i].erase_size));
		total_size +=sizeof(spi_nand_flash_tables[i].erase_size);

		memcpy((buf+total_size), &spi_nand_flash_tables[i].oob_size, sizeof(spi_nand_flash_tables[i].oob_size));
		total_size +=sizeof(spi_nand_flash_tables[i].oob_size);

		memcpy((buf+total_size), &spi_nand_flash_tables[i].dummy_mode, sizeof(spi_nand_flash_tables[i].dummy_mode));
		total_size +=sizeof(spi_nand_flash_tables[i].dummy_mode);

		memcpy((buf+total_size), &spi_nand_flash_tables[i].read_mode, sizeof(spi_nand_flash_tables[i].read_mode));
		total_size +=sizeof(spi_nand_flash_tables[i].read_mode);
		memcpy((buf+total_size), &spi_nand_flash_tables[i].die_num, sizeof(spi_nand_flash_tables[i].die_num));
		total_size +=sizeof(spi_nand_flash_tables[i].die_num);
		
		memcpy((buf+total_size), &spi_nand_flash_tables[i].write_mode, sizeof(spi_nand_flash_tables[i].write_mode));
		total_size +=sizeof(spi_nand_flash_tables[i].write_mode);

		memcpy((buf+total_size), &spi_nand_flash_tables[i].feature, sizeof(spi_nand_flash_tables[i].feature));
		total_size +=sizeof(spi_nand_flash_tables[i].feature);

		memcpy((buf+total_size), &spi_nand_flash_tables[i].ecc_fail_check_info, sizeof(spi_nand_flash_tables[i].ecc_fail_check_info));
		total_size +=sizeof(spi_nand_flash_tables[i].ecc_fail_check_info);

		memcpy((buf+total_size), &spi_nand_flash_tables[i].write_en_type, sizeof(spi_nand_flash_tables[i].write_en_type));
		total_size +=sizeof(spi_nand_flash_tables[i].write_en_type);

		memcpy((buf+total_size), &spi_nand_flash_tables[i].unlock_block_info, sizeof(spi_nand_flash_tables[i].unlock_block_info));
		total_size +=sizeof(spi_nand_flash_tables[i].unlock_block_info);
		
		memcpy((buf+total_size), &spi_nand_flash_tables[i].quad_en, sizeof(spi_nand_flash_tables[i].quad_en));
		total_size +=sizeof(spi_nand_flash_tables[i].quad_en);

		memcpy((buf+total_size), &spi_nand_flash_tables[i].ecc_en, sizeof(spi_nand_flash_tables[i].ecc_en));
		total_size +=sizeof(spi_nand_flash_tables[i].ecc_en);

#ifdef TCSUPPORT_NAND_FLASH_OTP

		memcpy((buf+total_size), &spi_nand_flash_tables[i].otp_page_num, sizeof(spi_nand_flash_tables[i].otp_page_num));
		total_size +=sizeof(spi_nand_flash_tables[i].otp_page_num);

#endif
		memcpy((buf+total_size), &spi_nand_flash_tables[i].soc_ecc_ability, sizeof(spi_nand_flash_tables[i].soc_ecc_ability));
		total_size +=sizeof(spi_nand_flash_tables[i].soc_ecc_ability);

		memcpy((buf+total_size), &spi_nand_flash_tables[i].extend_dev_id, sizeof(spi_nand_flash_tables[i].extend_dev_id));
		total_size +=sizeof(spi_nand_flash_tables[i].extend_dev_id);
	}

	flash_h.flash_name_off = total_size;

	for(i=0; i<num; i++){
		name_len = strlen(spi_nand_flash_tables[i].ptr_name);
		name_len++;
		memcpy((buf+total_size), &name_len, 4);

		memcpy((buf+total_size+4), spi_nand_flash_tables[i].ptr_name, name_len);
		total_size += (name_len+4);
	}
	/* for 4 byte-alignment */
	if ((total_size % 4)!= 0 )
	{
		memcpy((buf+total_size), &total_size, 4-(total_size %4));
		total_size += (4-(total_size %4));
	}
	flash_h.flash_oob_off = total_size;
	
	for(i=0; i<num; i++){
		memcpy((buf+total_size), spi_nand_flash_tables[i].oob_free_layout, sizeof(struct spi_nand_flash_ooblayout));
		total_size += sizeof(struct spi_nand_flash_ooblayout);
	}

	memcpy(buf, &flash_h, sizeof(struct bl2_flash_H));

	fwrite(buf, total_size, 1 ,fp);
	fclose(fp);

	free(buf);

	return 0;
}

#endif
/* End of [spi_nand_flash_table.c] package */
