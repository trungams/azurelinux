# DNF5 repository snapshot: warm-query overhead

The [ACL tdnf study](https://github.com/microsoft/azure-container-linux/blob/aadagarwal/document-tdnf-snapshot-benchmark/acl/docs/tdnf-snapshot-benchmark.md)
observed about 190× warm-query overhead in six native ARM/AZL3 samples:
timestamp mode bypasses its parsed solver cache and repeatedly processes XML.
This **independent DNF5 on/off** measurement is neither a tdnf-versus-DNF5 speed comparison nor a whole-build result.

## Environment and observed result

Measured on 2026-11-06 UTC in a native x86_64 Azure Linux 4.0-38 mock root:
DNF5/libdnf5 5.2.18.0-11,
patched libsolv 0.7.40-7, and released dnf5-plugin-repo-snapshot 0.1.0-2
(CLI/backend both loaded at 0.1.0). The sole enabled fixture repo is the
**public AZL3 x86_64 base metadata feed**, not the host OS; installed AZL4
RPMs (including `sed`) and cached `ab-base` metadata stay unchanged.
Cutoff `1794001413` follows cache population; both outputs include installed sed and two repository sed versions.

| Metric (30 per mode) | Timestamp off: min / median / max | Timestamp on: min / median / max | Median on − off | On / off |
| --- | ---: | ---: | ---: | ---: |
| Wall time (ms) | 69.339 / 76.147 / 94.990 | 73.854 / 80.207 / 103.816 | +4.060 ms | 1.053× (+5.33%) |
| Child user + system CPU (ms) | 65.380 / 74.870 / 93.369 | 67.554 / 78.494 / 103.800 | +3.624 ms | 1.048× (+4.84%) |

All 60 queries and both warmups succeeded with byte-identical stdout
(SHA-256 `3f6c2a963b2e7ce316daa5430d99a60e7e09491394be9de3f22a198572694be1`);
all three metadata hashes matched before/after timing.

## Reproduce inside an isolated AZL4 environment

Prerequisites: Bash and an already-installed DNF5, patched libsolv
and v0.1.0 plugin (matching libdnf5 API) in a private AZL4 VM/chroot. Open a
shell **inside** that environment; the following commands run there. Use a
fresh writable directory under its `/workspace` (not the host's `/tmp`):

```bash
WORK="/workspace/dnf5-only-$(date -u +%Y%m%dT%H%M%SZ)"
mkdir -p "$WORK/repos.d"
cat > "$WORK/dnf.conf" <<EOF
[main]
reposdir=$WORK/repos.d
cachedir=$WORK/cache
system_cachedir=$WORK/cache
persistdir=$WORK/persist
system_state_dir=$WORK/state
logdir=$WORK/log
metadata_expire=-1
EOF
cat > "$WORK/repos.d/ab-base.repo" <<'EOF'
[ab-base]
name=Azure Linux 3.0 base x86_64 public metadata
baseurl=https://packages.microsoft.com/azurelinux/3.0/prod/base/x86_64/
enabled=1
gpgcheck=0
repo_gpgcheck=0
skip_if_unavailable=False
EOF
dnf5 --config="$WORK/dnf.conf" --refresh makecache
dnf5 --config="$WORK/dnf.conf" provides sed >"$WORK/warm-network.txt"
CUTOFF=$(date -u +%s)
dnf5 --config="$WORK/dnf.conf" --cacheonly provides sed
dnf5 --config="$WORK/dnf.conf" --cacheonly --snapshot-time="$CUTOFF" provides sed
```

`makecache` retrieves primary metadata; the following **untimed network
warmup** retrieves filelists needed by `provides`. Allow network only for
these two preparation commands. Signature checks are disabled **only in
this isolated fixture**, not production. All subsequent commands use
cache-only mode, without fetching. Repository metadata may change between runs.

## Benchmark method

1. Populate the cache and warm each query mode once; exclude preparation
   and warmup from the measurements. Keep the plugin loaded in both modes.
2. Run 30 pairs of the two cache-only queries above, reversing their order
   each pair: off/on, then on/off. Only the snapshot flag changes.
3. Measure each process's wall time with Python's `time.perf_counter_ns()`
   and child user/system CPU time with serial `resource.getrusage()` deltas.
   Do not include VM/chroot startup.
4. Record exit status and stdout/stderr for every attempt. Verify successful,
   identical output and unchanged cached metadata hashes before interpreting
   the timings.
5. Report each mode's minimum, median, and maximum. Calculate added overhead
   as `median_on - median_off` and percentage change as
   `100 * (median_on / median_off - 1)`.

These warm-query timings are not ARM64, whole-build, universal plugin
overhead, or absolute client speed. The public metadata feed and CPU/wall
noise can differ between runs.
