# Validation

The 2026-10-28 Sheng standard layout passed native `lunch aospa_sheng cp2a user` and a complete incremental `target-files-package` build, reusing its existing output cache. Target-files ZIP CRC and device/build metadata were checked. That result is historical, not a new full build of this source kit.

For this conversion, 41 project deltas were recreated from their exact pinned base revisions and the tested source snapshots. Every included modified/added text file was reconstructed in an isolated Git index and compared byte for byte with the reference snapshot. Original CRLF content is retained where present; Git may report whitespace warnings for those upstream files.

Device/vendor extraction, proprietary inputs and the separately built keyboard module remain required. `omitted-binaries.json` records historical external binary substitutions rather than embedding those payloads in Git. These are historical shared-export substitutions, not a claim that every item belongs to the Sheng product. The current 719 device/shared inputs are pinned independently in dependencies.json. No fresh source sync or new hardware validation is claimed for this packaging change.
