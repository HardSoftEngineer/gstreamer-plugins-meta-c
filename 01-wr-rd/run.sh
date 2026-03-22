#!/usr/bin/env bash

set -o pipefail

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

cd "$SCRIPT_DIR"

export GST_PLUGIN_PATH=$PWD

# # v1 ------------------------------------------
# gst-launch-1.0 videotestsrc ! mymetawriter ! fakesink


# # v2 ------------------------------------------
# gst-launch-1.0 videotestsrc ! mymetawriter ! mymetareader ! fakesink

# # Writer: frame=359 meta_pts=11933333333 sys_time=955523744826 delta_us=943590411493
# # Reader: frame=359 meta_pts=11933333333 meta_time=955523744826 sys_time=955523744837 delta meta-sys=11
# # Writer: frame=360 meta_pts=11966666666 sys_time=955523745761 delta_us=943557079095
# # Reader: frame=360 meta_pts=11966666666 meta_time=955523745761 sys_time=955523745770 delta meta-sys=9
# # Writer: frame=361 meta_pts=12000000000 sys_time=955523746323 delta_us=943523746323
# # Reader: frame=361 meta_pts=12000000000 meta_time=955523746323 sys_time=955523746328 delta meta-sys=5


# v3 ------------------------------------------
# gst-launch-1.0 videotestsrc ! queue ! mymetawriter ! queue ! mymetareader ! fakesink
gst-launch-1.0 videotestsrc ! queue max-size-buffers=10 max-size-bytes=0 max-size-time=0 ! mymetawriter ! queue max-size-buffers=5 ! mymetareader ! fakesink

# Writer: frame=700 meta_pts=23300000000 sys_time=955469790170 delta_us=932169790170
# Reader: frame=700 meta_pts=23300000000 meta_time=955469790170 sys_time=955469790200 delta meta-sys=30
# Writer: frame=701 meta_pts=23333333333 sys_time=955469790720 delta_us=932136457387
# Reader: frame=701 meta_pts=23333333333 meta_time=955469790720 sys_time=955469790735 delta meta-sys=15

