*Este proyecto se ha creado como parte del currículum de 42 por acarranz y sarodrig.*

# ft_irc

[English](README.md) · [Català](README.ca.md) · **Español**

## Descripción

`ft_irc` es un **servidor de Internet Relay Chat (IRC)** propio, escrito desde cero en **C++98** como parte del Common Core de 42.

El objetivo del proyecto es implementar el núcleo del protocolo IRC: un servidor TCP capaz de atender a varios clientes a la vez con un único bucle `poll()`, con E/S no bloqueante y sin `fork`. El servidor permite que clientes IRC reales se conecten, se autentiquen, elijan un nickname y un nombre de usuario, entren en canales, intercambien mensajes públicos y privados, y gestionen los canales con comandos y modos de operador.

Como pide el enunciado, el proyecto **no** implementa un cliente IRC ni la comunicación entre servidores.

## Funcionalidades

- Servidor TCP no bloqueante gestionado con un único bucle `poll()`.
- Varios clientes conectados a la vez.
- Autenticación con la contraseña del servidor (`PASS`).
- Registro de usuarios (`NICK`, `USER`).
- Gestión de canales (creación, entrada, miembros, operadores).
- Mensajes públicos (en canal) y privados (`PRIVMSG`).
- Gestión del tema del canal con restricciones para operadores (`TOPIC`).
- Expulsar usuarios de un canal (`KICK`).
- Invitar usuarios a un canal (`INVITE`).
- Modos de canal (`MODE`): `i`, `t`, `k`, `o`, `l`.
- Mensajes parciales o fragmentados (la entrada se guarda en un buffer y se reconstruye al llegar `\n`).

## Arquitectura

- **`Server`**: crea y configura el socket de escucha, ejecuta el bucle principal de eventos con `poll()`, acepta conexiones nuevas, lee y guarda en un buffer lo que envía cada cliente, lo divide en líneas completas y envía cada comando a su gestor.
- **`Client`**: representa un usuario conectado (descriptor de fichero, nickname, nombre de usuario, buffer de entrada y estado del registro).
- **`Channel`**: representa un canal IRC (nombre, tema, contraseña, límite de usuarios, miembros, operadores, lista de invitados y envío de mensajes a todos los miembros).

### Comandos soportados

| Comando | Descripción | Ejemplo |
|---|---|---|
| `PASS` | Autentica al cliente con la contraseña del servidor. | `PASS server_password` |
| `NICK` | Fija o cambia el nickname del cliente. | `NICK nick` |
| `USER` | Fija el nombre de usuario y los datos de registro. | `USER nick 0 * :Real Name` |
| `JOIN` | Entra en un canal o lo crea. | `JOIN #channel` |
| `PRIVMSG` | Envía un mensaje privado a un usuario o un mensaje a un canal. | `PRIVMSG nick :Hello!` o `PRIVMSG #channel :Hello everyone!` |
| `KICK` | Expulsa a un usuario de un canal (solo operadores). | `KICK #channel nick :Rule violation` |
| `INVITE` | Invita a un usuario a un canal. | `INVITE nick #channel` |
| `TOPIC` | Muestra o cambia el tema de un canal. | `TOPIC #channel :Welcome to the channel` |
| `MODE` | Consulta o cambia los modos de un canal. | `MODE #channel +i` |
| `PART` | Sale de un canal. | `PART #channel` |
| `QUIT` | Desconecta al cliente del servidor. | `QUIT :Goodbye` |

### Modos de canal (`MODE`)

| Modo | Descripción | Activar | Desactivar |
|---|---|---|---|
| `i` | Solo pueden entrar los usuarios invitados. | `MODE #channel +i` | `MODE #channel -i` |
| `t` | Solo los operadores pueden cambiar el tema. | `MODE #channel +t` | `MODE #channel -t` |
| `k` | Pone o quita la contraseña del canal. | `MODE #channel +k secret123` | `MODE #channel -k secret123` |
| `o` | Da o quita permisos de operador. | `MODE #channel +o nick` | `MODE #channel -o nick` |
| `l` | Pone o quita el número máximo de miembros. | `MODE #channel +l 10` | `MODE #channel -l` |

## Instrucciones

### Compilación

```bash
make
```

Compila el proyecto con `c++` y las opciones `-Wall -Wextra -Werror -std=c++98`, y genera el ejecutable `ircserv`.

Otras reglas disponibles:

```bash
make clean
make fclean
make re
```

### Uso

```bash
./ircserv <port> <password>
```

- `port`: el puerto TCP en el que escuchará el servidor (1024–65535).
- `password`: la contraseña que cualquier cliente IRC tiene que enviar con `PASS` antes de registrarse.

Ejemplo:

```bash
./ircserv 6667 mypassword
```

### Conectarse y probar

#### 1. Probar con Netcat (`nc`)
Con el servidor en marcha, abre otra terminal para conectarte. Puedes mirar la terminal del servidor para ver los logs y el tráfico de las conexiones en directo.

Conéctate con Netcat:
```bash
nc -C 127.0.0.1 6667
```

Una vez conectado, autentícate y registra tu usuario escribiendo estos comandos **en formato crudo** (pulsa Enter después de cada línea):
```text
PASS mypassword
NICK santi_nick
USER santi_user 0 * :santi_real
```

Después del registro, puedes probar los canales y los mensajes:
```text
JOIN #channel1
PRIVMSG #channel1 :Hello everyone
```

* **Mensajes en canal:** abre otra terminal, ejecuta `nc` otra vez y registra un segundo usuario. Entra en el mismo canal (`JOIN #channel1`) para ver cómo se envían y reciben los mensajes entre usuarios.
* **Mensajes privados:** para enviar un mensaje directo a un usuario, conecta un segundo usuario desde otra terminal y usa esta sintaxis:
  ```text
  PRIVMSG santi_nick :Hola santi
  ```

* **Otras funcionalidades:** también puedes probar el resto de modos de canal (`+i`, `+t`, `+k`, `+o`, `+l`) y los comandos de operador (`KICK`, `INVITE`, `TOPIC`) con la sintaxis estándar de IRC.

#### 2. Probar con Irssi
Para probar el servidor con un cliente de terminal real como **Irssi**, abre otra terminal y sigue estos pasos:

1. Arranca Irssi:
   ```bash
   irssi
   ```

2. **Conectarse y autenticarse:** conéctate pasando la IP del servidor, el puerto y la contraseña **en este orden exacto**. Irssi enviará automáticamente los comandos de registro `PASS`, `NICK` y `USER` a tu servidor:
   ```text
   /connect 127.0.0.1 6667 mypassword
   ```

3. **Probar canales y comunicación:**
   * **Entrar en un canal (`JOIN`):** escribe `/join #channel1` para crear un canal o entrar en él.
   * **Mensaje público (`PRIVMSG` a un canal):** escribe el texto y pulsa Enter dentro de la ventana del canal para enviarlo a todos.
   * **Mensaje privado (`PRIVMSG` a un usuario):** para abrir un chat privado con otro usuario conectado, usa:
	 ```text
	 /msg santi_nick Hi!This is a private message
	 ```

Irssi usa ventanas separadas para el servidor, los canales y las conversaciones privadas.
El número de la ventana activa aparece en la barra de estado de Irssi.
Pulsa `Esc`, suéltalo y después pulsa el número de la ventana que quieras abrir.

## Recursos

### Referencias del protocolo IRC

- [RFC 1459 — Internet Relay Chat Protocol](https://datatracker.ietf.org/doc/html/rfc1459)
- [RFC 2812 — IRC Client Protocol](https://datatracker.ietf.org/doc/html/rfc2812)
- [Internet Relay Chat Protocol Documentation ](https://dd.ircdocs.horse/)
- [modern.ircdocs.info — Modern IRC Client Protocol](https://modern.ircdocs.info/)
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)
- `man poll`, `man fcntl`, `man socket`

### Uso de IA
Durante el desarrollo de este proyecto se ha usado Inteligencia Artificial (Gemini) como herramienta auxiliar para estas tareas:

* **Depuración y resolución de problemas:** ayudar a encontrar la causa de errores lógicos, gestionar casos límite en las desconexiones de sockets y comportamientos inesperados del programa.
* **Ideas para las pruebas:** proponer ideas y métodos para organizar escenarios de prueba que validen el comportamiento de los canales y los casos límite.
* **Documentación y formato:** estructurar y traducir el contenido de este `README.md`.
