mkdir /mnt/sandbox
cd /mnt/sandbox

mkdir dev proc sys tmp

mount --rbind /dev dev
mount --make-rslave dev
mount -t proc /proc proc
mount --rbind /sys sys
mount --make-rslave sys
mount --rbind /tmp tmp

cp -r /usr .
cp -r /bin .
cp -r /etc .
cp -r /lib .
cp -r /lib64 .
