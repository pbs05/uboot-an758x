#!/bin/sh

set -eu

usage()
{
	cat >&2 <<'EOF'
usage: build-an758x.sh <hg5382a|hg5585f-ct|hg5585f-cu|xg2010g|zn504xg-d|zn515xg-d|ung00a|xg-040g-md|xg-040g-tf|xg-040g-mf|hm2004-du>

Required environment:
  CROSS_COMPILE         AArch64 toolchain prefix
  ARM32_CROSS_COMPILE   Arm bare-metal toolchain prefix for BL2
  MBEDTLS_DIR           Mbed TLS 3.4.1 source directory

Optional environment:
  BUILD_ROOT            Build workspace (default: <repo>/build)
  OUTPUT_ROOT           Firmware output directory (default: <repo>/output)
  BUILD_JOBS            Parallel jobs (default: detected CPU count)
  AIROHA_SIGN_KEY_PATH   RSA-4096 PEM private-key file (first choice)
  AIROHA_SIGN_KEY        RSA-4096 PEM private-key contents
EOF
	exit 2
}

[ "$#" -eq 1 ] || usage
board="$1"

script_dir="$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)"
repo_dir="$(CDPATH= cd -- "$script_dir/.." && pwd)"
build_root="${BUILD_ROOT:-$repo_dir/build}"
output_root="${OUTPUT_ROOT:-$repo_dir/output}"
jobs="${BUILD_JOBS:-$(getconf _NPROCESSORS_ONLN 2>/dev/null || echo 1)}"

mkdir -p "$build_root" "$output_root"
build_root="$(CDPATH= cd -- "$build_root" && pwd)"
output_root="$(CDPATH= cd -- "$output_root" && pwd)"

: "${CROSS_COMPILE:?CROSS_COMPILE is required}"
: "${ARM32_CROSS_COMPILE:?ARM32_CROSS_COMPILE is required}"
: "${MBEDTLS_DIR:?MBEDTLS_DIR is required}"

emmc=0

case "$board" in
hg5382a)
	defconfig=an7581_fiberhome_hg5382a_defconfig
	artifact_prefix=an7581-fiberhome-hg5382a
	soc=EN7581
	parallel_nand=1
	;;
hg5585f-ct)
	defconfig=an7581_fiberhome_hg5585f-ct_defconfig
	artifact_prefix=an7581-fiberhome-hg5585f-ct
	soc=EN7581
	parallel_nand=1
	;;
hg5585f-cu)
	defconfig=an7581_fiberhome_hg5585f-cu_defconfig
	artifact_prefix=an7581-fiberhome-hg5585f-cu
	soc=EN7581
	parallel_nand=1
	;;
xg2010g)
	defconfig=an7581_gemtek_xg2010g_defconfig
	artifact_prefix=an7581-gemtek-xg2010g
	soc=EN7581
	parallel_nand=0
	;;
zn504xg-d)
	defconfig=an7581_znxt_zn504xg-d_defconfig
	artifact_prefix=an7581-znxt-zn504xg-d
	soc=EN7581
	parallel_nand=0
	;;
zn515xg-d)
	defconfig=an7581_znxt_zn515xg-d_defconfig
	artifact_prefix=an7581-znxt-zn515xg-d
	soc=EN7581
	parallel_nand=0
	;;
ung00a)
	defconfig=an7581_unionman_ung00a_defconfig
	artifact_prefix=an7581-unionman-ung00a
	soc=EN7581
	parallel_nand=0
	;;
xg-040g-md)
	defconfig=an7581_nokia_xg-040g-md_defconfig
	artifact_prefix=an7581-nokia-xg-040g-md
	soc=EN7581
	parallel_nand=0
	;;
xg-040g-tf)
	defconfig=an7581_nokia_xg-040g-tf_defconfig
	artifact_prefix=an7581-nokia-xg-040g-tf
	soc=EN7581
	parallel_nand=0
	;;
xg-040g-mf)
	defconfig=an7583_nokia_xg-040g-mf_defconfig
	artifact_prefix=an7583-nokia-xg-040g-mf
	soc=AN7583
	parallel_nand=0
	;;
hm2004-du)
	defconfig=an7581_h3c_hm2004-du_defconfig
	artifact_prefix=an7581-h3c-hm2004-du
	soc=EN7581
	parallel_nand=0
	;;
*)
	usage
	;;
esac

uboot_build="$build_root/$board/u-boot"
tfa_build="$build_root/$board/tf-a"
board_output="$output_root/$board"
host_tools="$build_root/host-tools"
lzma_source="$repo_dir/tools/lzma-sdk-4.65"
lzma_build="$host_tools/lzma-sdk-4.65"
lzma_tool="$host_tools/lzma"

mkdir -p "$uboot_build" "$board_output" "$host_tools"

# The TF-A decoder reads the exact output length from the LZMA-Alone header.
# LZMA SDK 4.65 records this length for every encoded boot image.
case "$lzma_build" in
"$build_root"/*) ;;
*)
	echo "invalid LZMA build path: $lzma_build" >&2
	exit 1
	;;
esac
rm -rf "$lzma_build"
cp -a "$lzma_source" "$lzma_build"
make -C "$lzma_build/CPP/7zip/Compress/LZMA_Alone" \
	-f makefile.gcc lzma_alone
cp "$lzma_build/CPP/7zip/Compress/LZMA_Alone/lzma_alone" "$lzma_tool"
chmod +x "$lzma_tool"

# Out-of-tree U-Boot builds keep generated headers and DT artifacts outside
# the upstream-derived source tree.
make -C "$repo_dir" O="$uboot_build" CROSS_COMPILE="$CROSS_COMPILE" \
	"$defconfig"
make -C "$repo_dir" O="$uboot_build" CROSS_COMPILE="$CROSS_COMPILE" \
	-j"$jobs"
"$lzma_tool" e "$uboot_build/u-boot.bin" "$board_output/u-boot.lzma"
cp "$uboot_build/u-boot.bin" "$board_output/u-boot.bin"
cp "$uboot_build/u-boot.dtb" "$board_output/u-boot.dtb"

# A per-board TF-A tree isolates BL21/BL22/BL23 intermediate files because
# each stage uses the same build and output paths.
case "$tfa_build" in
"$build_root"/*) ;;
*)
	echo "invalid TF-A build path: $tfa_build" >&2
	exit 1
	;;
esac
rm -rf "$tfa_build"
cp -a "$repo_dir/tf-a" "$tfa_build"

common_atf_args="PLAT=en7523 CONFIG_ECNT=1 TCSUPPORT_OPENWRT=1 TCSUPPORT_ATF_UNOPEN=0 TCSUPPORT_CPU_$soc=1 TCSUPPORT_CPU_EN7523=1 TCSUPPORT_CPU_ARMV8=1 TCSUPPORT_UBOOT_64BIT=1 TCSUPPORT_UBOOT=1 TCSUPPORT_BL2_OPTIMIZATION=1 MBEDTLS_DIR=$MBEDTLS_DIR TOOLS_DIR=$host_tools"

if [ "$parallel_nand" -eq 1 ]; then
	common_atf_args="$common_atf_args TCSUPPORT_PARALLEL_NAND=1"
fi
if [ "$emmc" -eq 1 ]; then
	common_atf_args="$common_atf_args TCSUPPORT_EMMC=1"
fi

# shellcheck disable=SC2086
make -C "$tfa_build" -j"$jobs" CROSS_COMPILE="$CROSS_COMPILE" \
	ARCH=aarch64 $common_atf_args bl31
"$lzma_tool" e "$tfa_build/build/en7523/release/bl31.bin" \
	"$board_output/bl31.lzma"
# shellcheck disable=SC2086
make -C "$tfa_build" CROSS_COMPILE="$CROSS_COMPILE" \
	ARCH=aarch64 $common_atf_args clean

for stage in 21 22 23; do
	case "$stage" in
	21) stage_flag=IMAGE_BL21=1 ;;
	22) stage_flag=IMAGE_BL22=1 ;;
	23) stage_flag=IMAGE_BL23=1 ;;
	esac
	# shellcheck disable=SC2086
	make -C "$tfa_build" -j"$jobs" CROSS_COMPILE="$CROSS_COMPILE" \
		ARM32TOOLCHAIN_BASE="$ARM32_CROSS_COMPILE" ARCH=aarch32 \
		$stage_flag $common_atf_args bl2
	if [ "$stage" -ne 23 ]; then
		# shellcheck disable=SC2086
		make -C "$tfa_build" CROSS_COMPILE="$CROSS_COMPILE" \
			ARM32TOOLCHAIN_BASE="$ARM32_CROSS_COMPILE" ARCH=aarch32 \
			$stage_flag $common_atf_args clean
	fi
done

# The flash table generator emits flash_table.bin in its working directory.
cc -DFLASH_TABLE_OPEN -DTCSUPPORT_BL2_OPTIMIZATION \
	-I"$tfa_build/plat/ecnt/en7523/include" \
	-o "$host_tools/airoha-atf-gen-flash-table" \
	"$tfa_build/plat/ecnt/common/drivers/flash/spi_nand_flash_table.c"
(
	cd "$tfa_build"
	"$host_tools/airoha-atf-gen-flash-table"
)
"$lzma_tool" e "$tfa_build/flash_table.bin" "$tfa_build/flash_table.lzma"

# BL1 transfers control to the 14 KiB BL21 image at the beginning of TB_FW.
# BL21 initializes the execution context used to expand BL22 and BL23.
"$repo_dir/scripts/airoha_pack_bl2.sh" \
	"$tfa_build/bl21.bin" \
	"$tfa_build/bl22.lzma" \
	"$tfa_build/bl23.lzma" \
	"$tfa_build/flash_table.lzma" \
	"$board_output/bl2.bin"

make -C "$tfa_build/tools/fiptool" -j"$jobs"
fiptool="$tfa_build/tools/fiptool/fiptool"
packaged="$tfa_build/packaged"
mkdir -p "$packaged"

"$fiptool" create --align 1024 \
	--tb-fw "$board_output/bl2.bin" \
	"$packaged/$artifact_prefix-preloader.bin"

"$fiptool" create --align 1024 \
	--soc-fw "$board_output/bl31.lzma" \
	--nt-fw "$board_output/u-boot.lzma" \
	"$packaged/$artifact_prefix-bl31-u-boot.fip"

sh "$repo_dir/scripts/sign-an758x-fip.sh" "$tfa_build" \
	"$packaged/$artifact_prefix-preloader.bin" \
	"$packaged/$artifact_prefix-bl31-u-boot.fip"

# Packing after signing keeps the first eraseblock and standalone preloader
# identical at the FIP offset consumed by the boot ROM.
if [ "$parallel_nand" -eq 1 ]; then
	sh "$repo_dir/scripts/airoha_pack_firstblock.sh" \
		"$packaged/$artifact_prefix-preloader.bin" \
		"$packaged/$artifact_prefix-firstblock.bin"
fi

cp "$packaged/"* "$board_output/"

printf 'Artifacts written to %s\n' "$board_output"
