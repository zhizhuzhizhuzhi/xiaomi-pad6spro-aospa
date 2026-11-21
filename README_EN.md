# AOSPA for Xiaomi Pad 6S Pro 12.4 (sheng)

[中文](README.md)

This single repository contains the pinned manifest, source patches, overlays, input checks and build/packaging entry points for AOSPA Calcite / Android 17. Device repositories and the SM8550 platform tree are reused.

## Checkout

Use Ubuntu 22.04/24.04 x86_64 with Android build dependencies, git-lfs and repo installed. 64 GB RAM and 500 GB free storage are recommended.

```sh
git clone https://github.com/zhizhuzhizhuzhi/xiaomi-pad6spro-aospa.git aospa-sheng-kit
mkdir android-sheng && cd android-sheng
repo init -u https://github.com/zhizhuzhizhuzhi/xiaomi-pad6spro-aospa.git -b main -m manifests/release.xml --git-lfs
repo sync -c --no-tags -j8
python3 ../aospa-sheng-kit/scripts/apply.py "$PWD"
```

## Inputs

Use the matching Sheng Global OS3.0.6.0.WNXMIXM / Android 16 partition dump and a matched kernel/DTB/DTBO/module payload. Build the separate Caps Lock module as described in [KEYBOARD_MODULE.md](docs/KEYBOARD_MODULE.md).

```sh
bash ../aospa-sheng-kit/scripts/prepare.sh "$PWD" /path/to/stock-dump \
  --source /path/to/kernel-payload --source /path/to/prepared-compatible-inputs \
  --built-capslock out-sheng-capslock/module/hid-nanosic-capslock.ko \
  --patchelf prebuilts/extract-tools/linux-x86/bin/patchelf-0_17_2
```

See [DEVICE_INPUTS.md](docs/DEVICE_INPUTS.md). `dependencies.json` pins the exact prepared files. Missing inputs stop the build; firmware, proprietary payloads and private signing keys are not committed.

Prepared inputs from a matching source tree can be installed directly:

```sh
python3 ../aospa-sheng-kit/scripts/fetch-dependencies.py "$PWD" \
  --source /path/to/prepared-sheng-inputs --replace-stock-inputs
```

## Build and package

```sh
BUILD_NUMBER=20261121 bash ../aospa-sheng-kit/scripts/build.sh "$PWD" user "$(nproc)"
OUT_DIR=out-sheng-user python3 ../aospa-sheng-kit/scripts/package.py "$PWD" \
  out-sheng-user/target/product/sheng/obj/PACKAGING/target_files_intermediates/aospa_sheng-target_files.zip \
  "$PWD/aospa_sheng-fastboot-user.zip"
```

For userdebug, select `userdebug` and use `OUT_DIR=out-sheng-userdebug`. Outputs stay separate. Sheng uses its matched prebuilt kernel, not Fuxi's kernel preparation. These commands produce development builds; selecting user does not perform release signing.

See [licensing](LICENSE.md).
