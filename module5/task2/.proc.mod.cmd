savedcmd_/home/user/module5/task2/proc.mod := printf '%s\n'   proc.o | awk '!x[$$0]++ { print("/home/user/module5/task2/"$$0) }' > /home/user/module5/task2/proc.mod
