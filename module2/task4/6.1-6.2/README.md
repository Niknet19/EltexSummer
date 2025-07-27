## Сборка статической библиотеки
Команда `make static`:
- `gcc -g -Wall -Wextra -I./src -I./ui -I./list -c src/contacts.c -o src/contacts.o`
- `gcc -g -Wall -Wextra -I./src -I./ui -I./list -c src/ui/console_input.c -o src/ui/console_input.o`
- `gcc -g -Wall -Wextra -I./src -I./ui -I./list src/main.o src/contacts.o src/ui/console_input.o liblist.a -o list_static`

Выводим список контактов:  
<img width="300" height="300" alt="изображение" src="https://github.com/user-attachments/assets/d9d31cca-0c28-4c26-9a45-d1599d7e208b" />


Теперь изменияем функцию печати (добавим строку `printf("HELLO!!");`) и перекомпилируем библиотеку.  
`make lib-static`:
- `gcc -g -Wall -Wextra -I./src -I./ui -I./list -c src/list/list.c -o src/list/list.o`
- `ar rc liblist.a src/list/list.o`

Поведение не поменялось.

Теперь перекомпилируем бинарник вместе с библиотекой.
Видим что выводится строка `HELLO`
<img width="300" height="300" alt="изображение" src="https://github.com/user-attachments/assets/a827344b-3a05-40d1-96a3-f43b392035f1" />

## Сборка динамической библиотеки
Команда `make dynamic`:  
- `gcc -g -Wall -Wextra -I./src -I./ui -I./list -c src/main.c -o src/main.o`
- `gcc -g -Wall -Wextra -I./src -I./ui -I./list -c src/contacts.c -o src/contacts.o`
- `gcc -g -Wall -Wextra -I./src -I./ui -I./list -c src/ui/console_input.c -o src/ui/console_input.o`
- `gcc -g -Wall -Wextra -I./src -I./ui -I./list -fPIC -c src/list/list.c -o src/list/list.pic.o`
- `gcc -shared src/list/list.pic.o -o liblist.so`
- `gcc -g -Wall -Wextra -I./src -I./ui -I./list src/main.o src/contacts.o src/ui/console_input.o -L. -llist -Wl,-rpath,. -o list_dynamic`

Запускаем командой `make run-dynamic`  
Видим стандартное поведение.

Теперь соберем отдельно только динамическую библиотеку.
`make lib-dynamic`:
- `gcc -g -Wall -Wextra -I./src -I./ui -I./list -fPIC -c src/list/list.c -o src/list/list.pic.o`
- `gcc -shared src/list/list.pic.o -o liblist.so`

Видно что поведение поменялось, без перекомпиляции всей программы:  
<img width="300" height="300" alt="изображение" src="https://github.com/user-attachments/assets/e3b9e220-2544-400f-8575-ccebabaaeea6" />
