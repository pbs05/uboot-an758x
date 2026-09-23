# H3C HM2004-DU — 新增机型补丁包

SoC: AN7581CT (EN7581) · 512 MiB DDR4 · 256 MiB SPI-NAND · 1×2.5G (EN8811H) + 3×GE

## 1. 新增文件（直接放到仓库根目录对应路径）

| 包内路径 | 仓库目标路径 |
| --- | --- |
| `dts/upstream/src/arm64/airoha/an7581-h3c-hm2004-du.dts` | `dts/upstream/src/arm64/airoha/an7581-h3c-hm2004-du.dts` |
| `arch/arm/dts/an7581-h3c-hm2004-du-u-boot.dtsi` | `arch/arm/dts/an7581-h3c-hm2004-du-u-boot.dtsi` |
| `board/airoha/an7581/an7581_h3c_hm2004-du.env` | `board/airoha/an7581/an7581_h3c_hm2004-du.env` |
| `configs/an7581_h3c_hm2004-du_defconfig` | `configs/an7581_h3c_hm2004-du_defconfig` |

## 2. 修改文件（用包内版本覆盖，或手工打这三处）

- `scripts/build-an758x.sh`：`usage()` 目标列表 + `case` 分支新增 `hm2004-du`
  （`defconfig=an7581_h3c_hm2004-du_defconfig`，`artifact_prefix=an7581-h3c-hm2004-du`，`soc=EN7581`，`parallel_nand=0`）
- `.github/workflows/build.yml`：`matrix.target` 增加 `- hm2004-du`
- `README.md`：支持机型表增加一行 `` `hm2004-du` ``

## 3. 必须用实测值替换的三处（搜 `TODO(bring-up)`）

1. **SPI-NAND 型号** — `winbond,w25n02kv` 是占位（256 MiB = 2 GiB-bit）。
   从串口日志的 JEDEC ID 确认后换成真实型号，备选：`esmt,f50l2g41xa` / `macronix,mx35lf2ge4ad`。
   注意：dts 与 `-u-boot.dtsi` 两处都要同步改。
2. **状态 LED** — 现填 `&en7581_pinctrl 17`，label `green:power`。
   三处必须一致：dts 的 `leds/led-power` 节点名与 label、`-u-boot.dtsi` 的 `status_led: &{/leds/led-power}`、env 的 `bootled_status=green:power`。
3. **factory 卷** — 现填 `0x200000` + `static`。按原厂 UBI 布局核对大小与 static/dynamic。

复位键沿用 GPIO0（现有 AN7581 板卡一致），若不同需改 `keys` 节点。

## 4. 构建

```sh
export CROSS_COMPILE=/path/to/aarch64-linux-musl-
export ARM32_CROSS_COMPILE=/path/to/arm-none-eabi-
export MBEDTLS_DIR=/path/to/mbedtls-3.4.1
./scripts/build-an758x.sh hm2004-du
```

产物在 `output/hm2004-du/`：`an7581-h3c-hm2004-du-bl31-u-boot.fip`、`-preloader.bin`。

## 5. 关于网口

U-Boot 的 Web 恢复只走 `gdm1`（internal fixed-link 到内置交换），**不需要** EN8811H PHY 驱动，
所以 2.5G 口与三个千兆口的 switch/phy 描述属于 Linux DTS（`target/linux/airoha/dts`），
本包未写入。U-Boot 起来后接任意网口访问 `http://192.168.0.1/` 即可。
