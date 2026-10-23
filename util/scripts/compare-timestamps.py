#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0-only
"""Compare two `cbmem -t` timestamp captures entry by entry.

Useful for A/B evaluation of boot-time changes (e.g. SPD cache on vs. off).
Each entry is matched by its numeric timestamp ID and the per-step delta is
diffed so regressions and improvements stand out.

Usage:
    util/scripts/compare-timestamps.py BASELINE.txt CHANGED.txt

The inputs are the plain output of `cbmem -t`, e.g.:

    58 entries total:

       0:1st timestamp                    135,468 (0)
      11:start of bootblock               148,660 (13,192)
      ...
"""

import argparse
import re
import sys

# e.g. "  950:calling FspMemoryInit          1,113,172 (110,384)"
LINE_RE = re.compile(r"^\s*(\d+):(.+?)\s{2,}([\d,]+)\s*\(([\d,]+)\)\s*$")


def parse(path):
    """Return (ordered list of keys, {key: (label, cumulative, delta)}).

    A timestamp ID can occur more than once per boot (e.g. the LZMA/LZ4
    decompress steps run in several stages), so entries are keyed by
    (id, occurrence) and matched positionally between the two captures.
    """
    order = []
    entries = {}
    seen = {}
    with open(path) as f:
        for line in f:
            m = LINE_RE.match(line)
            if not m:
                continue
            tid = int(m.group(1))
            label = m.group(2).strip()
            cumulative = int(m.group(3).replace(",", ""))
            delta = int(m.group(4).replace(",", ""))
            occ = seen.get(tid, 0)
            seen[tid] = occ + 1
            key = (tid, occ)
            order.append(key)
            entries[key] = (label, cumulative, delta)
    return order, entries


def fmt(us):
    """Format microseconds with thousands separators, keeping the sign."""
    return f"{us:+,}" if us else "0"


def main():
    ap = argparse.ArgumentParser(
        description="Compare two `cbmem -t` captures entry by entry.")
    ap.add_argument("baseline", help="baseline cbmem -t capture")
    ap.add_argument("changed", help="changed cbmem -t capture")
    ap.add_argument("-s", "--sort", choices=("boot", "delta"), default="boot",
                    help="order rows by boot sequence (default) or by the "
                         "size of the per-step delta difference")
    ap.add_argument("-t", "--threshold", type=int, default=0, metavar="US",
                    help="only show rows whose delta differs by at least US "
                         "microseconds")
    args = ap.parse_args()

    order_a, a = parse(args.baseline)
    order_b, b = parse(args.changed)

    if not a or not b:
        sys.exit("error: no timestamp entries parsed; is this `cbmem -t` output?")

    # Union of keys, keeping baseline boot order first, then any extras.
    keys = list(order_a) + [k for k in order_b if k not in a]

    rows = []
    for key in keys:
        tid = key[0]
        la, ca, da = a.get(key, (None, None, None))
        lb, cb, db = b.get(key, (None, None, None))
        label = la if la is not None else lb
        # delta difference (changed minus baseline); only when present in both
        ddelta = db - da if (da is not None and db is not None) else None
        rows.append((tid, label, ca, cb, da, db, ddelta))

    if args.sort == "delta":
        rows.sort(key=lambda r: abs(r[6]) if r[6] is not None else -1, reverse=True)

    if args.threshold:
        rows = [r for r in rows
                if r[6] is None or abs(r[6]) >= args.threshold]

    # Column widths
    lw = max((len(r[1]) for r in rows), default=5)
    hdr = f"{'ID':>4}  {'step':<{lw}}  {'delta A':>12}  {'delta B':>12}  {'diff':>12}"
    print(hdr)
    print("-" * len(hdr))

    tot_a = tot_b = 0
    for tid, label, ca, cb, da, db, ddelta in rows:
        if da is None:
            marker = "  [only in CHANGED]"
            da_s, db_s, diff_s = "-", f"{db:,}", "-"
        elif db is None:
            marker = "  [only in BASELINE]"
            da_s, db_s, diff_s = f"{da:,}", "-", "-"
        else:
            tot_a += da
            tot_b += db
            marker = ""
            da_s, db_s = f"{da:,}", f"{db:,}"
            diff_s = fmt(ddelta)
        print(f"{tid:>4}  {label:<{lw}}  {da_s:>12}  {db_s:>12}  {diff_s:>12}{marker}")

    print("-" * len(hdr))
    print(f"{'':>4}  {'TOTAL (matched steps)':<{lw}}  "
          f"{tot_a:>12,}  {tot_b:>12,}  {fmt(tot_b - tot_a):>12}")
    # Total boot time from the last cumulative value on each side.
    boot_a = a[order_a[-1]][1] if order_a else 0
    boot_b = b[order_b[-1]][1] if order_b else 0
    print(f"{'':>4}  {'TOTAL boot (last cumulative)':<{lw}}  "
          f"{boot_a:>12,}  {boot_b:>12,}  {fmt(boot_b - boot_a):>12}")
    print("\nAll values in microseconds. diff = B - A "
          "(negative means CHANGED is faster).")


if __name__ == "__main__":
    main()
