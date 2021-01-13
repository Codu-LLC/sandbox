mkdir rootfs
mount --bind /mnt/rootfs rootfs
cd rootfs
mkdir put_old
pivot_root . put_old
mount --rbind put_old/dev dev
mount --make-rslave dev
mount --rbind put_old/proc proc
mount --make-rslave proc
mount --rbind put_old/sys sys
mount --make-rslave sys
mount --rbind put_old/tmp tmp
# TODO(conankun): sync /usr /bin /etc /lib /lib64 if needed.
umount -l put_old
rm -rf put_old
