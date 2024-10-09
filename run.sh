#!/bin/bash

if [[ "$1" = "gdb" ]]; then
    qemu-system-x86_64 -s -S -d int -hda Image/x64BareBonesImage.qcow2 -m 512 2>&1 | grep "v="
elif [[ "$1" = "int" ]]; then
    qemu-system-x86_64 -d int -hda Image/x64BareBonesImage.qcow2 -m 512 2>&1 | grep "v="
else
    qemu-system-x86_64 -hda Image/x64BareBonesImage.qcow2 -m 512
fi
