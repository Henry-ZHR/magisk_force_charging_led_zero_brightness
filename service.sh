#!/sbin/sh

MODDIR=${0%/*}
EXEPATH=$MODDIR/bin/force-charging-led-zero-brightness
chmod +x $EXEPATH
exec $EXEPATH