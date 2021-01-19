export GROUP=${USER}
sudo cgcreate -a ${USER}:${GROUP} -g cpu,memory:/test_sandbox
sudo chown ${USER}:${GROUP} /sys/fs/cgroup/memory/test_sandbox/tasks
sudo chown ${USER}:${GROUP} /sys/fs/cgroup/cpu/test_sandbox/tasks
