#!/usr/bin/env bash

set -o pipefail

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

cd "$SCRIPT_DIR"

export GST_PLUGIN_PATH=$PWD

gst-launch-1.0 videotestsrc ! mymetawriter ! fakesink
