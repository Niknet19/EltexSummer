savedcmd_/home/user/module5/task1/hello.mod := printf '%s\n'   hello.o | awk '!x[$$0]++ { print("/home/user/module5/task1/"$$0) }' > /home/user/module5/task1/hello.mod
