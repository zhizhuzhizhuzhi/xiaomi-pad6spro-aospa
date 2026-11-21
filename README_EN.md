# AOSPA Android 17 for Xiaomi Pad 6S Pro 12.4 (sheng)

[中文](README.md)

Build entry point for this device. The [manifest](https://github.com/zhizhuzhizhuzhi/android_local_manifests/tree/aospa-17) pins the source repositories. [Shared integration](https://github.com/zhizhuzhizhuzhi/android_vendor_xiaomi_sm8550-integration/tree/aospa-17) owns common source patches and build tools; the [device repository](https://github.com/zhizhuzhizhuzhi/android_device_xiaomi_sheng/tree/aospa-17) owns device configuration.

Use Ubuntu 22.04/24.04 x86_64 with Android build dependencies, Git, Git LFS, repo and Python 3.11+. Keep the relative paths listed in the input manifests when preparing input files.

```sh
mkdir android-sheng && cd android-sheng
repo init -u https://github.com/zhizhuzhizhuzhi/android_local_manifests.git -b aospa-17 -m sheng.xml --git-lfs
repo sync -c --no-tags -j8
python3 vendor/xiaomi/sm8550-integration/tools/prepare-source.py "$PWD" --device sheng
python3 vendor/xiaomi/sm8550-integration/tools/prepare-inputs.py "$PWD" --device sheng \
  --source /path/to/prepared-sheng-inputs --replace-stock-inputs
BUILD_NUMBER=20261121 bash vendor/xiaomi/sm8550-integration/tools/build.sh "$PWD" sheng userdebug 16
```

Select `user` instead of `userdebug` for that build variant. Prepared input hashes are stored in `common/inputs.json` and `profiles/sheng/inputs.json` in the integration repository.

Package the target-files archive:

```sh
python3 vendor/xiaomi/sm8550-integration/tools/package.py sheng "$PWD" \
  /path/to/target_files.zip /path/to/aospa_sheng-fastboot.zip
```

The compatibility wrappers in `scripts/` invoke the same tools from the source checkout. AOSPA and Cloyuri use separate manifests and source branches. See [licensing](LICENSE.md).
