# Xiaomi Pad 6S Pro 12.4（sheng）AOSPA 构建

[English](README_EN.md)

AOSPA Calcite / Android 17，产品 `aospa_sheng`。构建资料集中在本仓库：固定版本 manifest、源码补丁、补充源码、依赖清单和构建/打包脚本。

## 环境

Ubuntu 22.04 / 24.04，x86_64，建议 64 GB 内存、500 GB 可用磁盘。

```sh
sudo apt update
sudo apt install git git-lfs curl python3 python-is-python3 build-essential zip unzip rsync bc bison flex gperf ccache libssl-dev libelf-dev libncurses-dev libxml2-utils xsltproc zlib1g-dev liblz4-tool
mkdir -p "$HOME/bin"
curl -o "$HOME/bin/repo" https://storage.googleapis.com/git-repo-downloads/repo
chmod +x "$HOME/bin/repo"
export PATH="$HOME/bin:$PATH"
git lfs install
```

## 获取源码

```sh
git clone https://github.com/zhizhuzhizhuzhi/xiaomi-pad6spro-aospa.git aospa-sheng-kit
mkdir android-sheng
cd android-sheng
repo init -u https://github.com/zhizhuzhizhuzhi/xiaomi-pad6spro-aospa.git -b main -m manifests/release.xml --git-lfs
repo sync -c --no-tags -j8
python3 ../aospa-sheng-kit/scripts/apply.py "$PWD"
```

manifest 固定设备树和上游版本；本仓库的 `patches/`、`overlay/` 提供配套修改，不依赖此前未上传的实现 fork。设备树继续复用已有仓库，公共平台内容继续共用。

## 准备设备输入

使用 **Sheng Global OS3.0.6.0.WNXMIXM / Android 16** 对应分区 dump。内核、DTB、DTBO 和模块必须为同一套匹配输入。

先按 [键盘模块说明](docs/KEYBOARD_MODULE.md) 构建 Caps Lock 模块，再提取 vendor 和校验依赖：

```sh
bash ../aospa-sheng-kit/scripts/prepare.sh "$PWD" /path/to/stock-dump \
  --source /path/to/kernel-payload --source /path/to/prepared-compatible-inputs \
  --built-capslock out-sheng-capslock/module/hid-nanosic-capslock.ko \
  --patchelf prebuilts/extract-tools/linux-x86/bin/patchelf-0_17_2
```

[设备输入说明](docs/DEVICE_INPUTS.md) 记录提取与兼容处理；`dependencies.json` 固定文件大小和 SHA256。原厂固件、专有文件及签名私钥不放入 Git。脚本不自动下载不明版本，缺少或不匹配时会停止。

## 编译和打包

### userdebug

```sh
BUILD_NUMBER=20261029 bash ../aospa-sheng-kit/scripts/build.sh "$PWD" userdebug "$(nproc)"
OUT_DIR=out-sheng-userdebug python3 ../aospa-sheng-kit/scripts/package.py "$PWD" \
  out-sheng-userdebug/target/product/sheng/obj/PACKAGING/target_files_intermediates/aospa_sheng-target_files.zip \
  "$PWD/aospa_sheng-fastboot-userdebug.zip"
```

### user

```sh
BUILD_NUMBER=20261029 bash ../aospa-sheng-kit/scripts/build.sh "$PWD" user "$(nproc)"
OUT_DIR=out-sheng-user python3 ../aospa-sheng-kit/scripts/package.py "$PWD" \
  out-sheng-user/target/product/sheng/obj/PACKAGING/target_files_intermediates/aospa_sheng-target_files.zip \
  "$PWD/aospa_sheng-fastboot-user.zip"
```

两种变体使用独立输出目录。Sheng 使用匹配的预编译内核，不运行 Fuxi 的内核准备流程。上述命令生成开发构建；`user` 不等于已完成自有密钥正式签名。

## 目录

- `manifests/`：固定源码版本。
- `patches/`、`overlay/`、`projects.json`：配套源码和基线。
- `scripts/`：准备输入、构建和 Fastboot 打包。
- `dependencies.json`：设备及公共输入校验清单。
- `licenses/`：上游许可证；原文件版权保留。
