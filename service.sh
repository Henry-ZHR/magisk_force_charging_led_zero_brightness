#!/sbin/sh

MODDIR=${0%/*}
EXEPATH=$MODDIR/bin/main
chmod +x $EXEPATH
exec $EXEPATH
