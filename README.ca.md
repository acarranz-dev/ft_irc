*Aquest projecte s'ha creat com a part del currículum de 42 per acarranz i sarodrig.*

# ft_irc

[English](README.md) · **Català** · [Español](README.es.md)

## Descripció

`ft_irc` és un **servidor d'Internet Relay Chat (IRC)** propi, escrit des de zero en **C++98** com a part del Common Core de 42.

L'objectiu del projecte és implementar el nucli del protocol IRC: un servidor TCP capaç d'atendre diversos clients alhora amb un sol bucle `poll()`, amb E/S no bloquejant i sense `fork`. El servidor permet que clients IRC reals es connectin, s'autentiquin, triïn un nickname i un nom d'usuari, entrin a canals, s'intercanviïn missatges públics i privats, i gestionin els canals amb ordres i modes d'operador.

Com demana l'enunciat, el projecte **no** implementa un client IRC ni la comunicació entre servidors.

## Funcionalitats

- Servidor TCP no bloquejant gestionat amb un sol bucle `poll()`.
- Diversos clients connectats alhora.
- Autenticació amb la contrasenya del servidor (`PASS`).
- Registre d'usuaris (`NICK`, `USER`).
- Gestió de canals (creació, entrada, membres, operadors).
- Missatges públics (al canal) i privats (`PRIVMSG`).
- Gestió del tema del canal amb restriccions per a operadors (`TOPIC`).
- Expulsar usuaris d'un canal (`KICK`).
- Convidar usuaris a un canal (`INVITE`).
- Modes de canal (`MODE`): `i`, `t`, `k`, `o`, `l`.
- Missatges parcials o fragmentats (l'entrada es guarda en un buffer i es reconstrueix quan arriba `\n`).

## Arquitectura

- **`Server`**: crea i configura el socket d'escolta, executa el bucle principal d'esdeveniments amb `poll()`, accepta connexions noves, llegeix i guarda en un buffer el que envia cada client, ho divideix en línies completes i envia cada ordre al seu gestor.
- **`Client`**: representa un usuari connectat (descriptor de fitxer, nickname, nom d'usuari, buffer d'entrada i estat del registre).
- **`Channel`**: representa un canal IRC (nom, tema, contrasenya, límit d'usuaris, membres, operadors, llista de convidats i enviament de missatges a tots els membres).

### Ordres admeses

| Ordre | Descripció | Exemple |
|---|---|---|
| `PASS` | Autentica el client amb la contrasenya del servidor. | `PASS server_password` |
| `NICK` | Fixa o canvia el nickname del client. | `NICK nick` |
| `USER` | Fixa el nom d'usuari i les dades de registre. | `USER nick 0 * :Real Name` |
| `JOIN` | Entra a un canal o el crea. | `JOIN #channel` |
| `PRIVMSG` | Envia un missatge privat a un usuari o un missatge a un canal. | `PRIVMSG nick :Hello!` o `PRIVMSG #channel :Hello everyone!` |
| `KICK` | Expulsa un usuari d'un canal (només operadors). | `KICK #channel nick :Rule violation` |
| `INVITE` | Convida un usuari a un canal. | `INVITE nick #channel` |
| `TOPIC` | Mostra o canvia el tema d'un canal. | `TOPIC #channel :Welcome to the channel` |
| `MODE` | Consulta o canvia els modes d'un canal. | `MODE #channel +i` |
| `PART` | Surt d'un canal. | `PART #channel` |
| `QUIT` | Desconnecta el client del servidor. | `QUIT :Goodbye` |

### Modes de canal (`MODE`)

| Mode | Descripció | Activar | Desactivar |
|---|---|---|---|
| `i` | Només hi poden entrar els usuaris convidats. | `MODE #channel +i` | `MODE #channel -i` |
| `t` | Només els operadors poden canviar el tema. | `MODE #channel +t` | `MODE #channel -t` |
| `k` | Posa o treu la contrasenya del canal. | `MODE #channel +k secret123` | `MODE #channel -k secret123` |
| `o` | Dona o treu permisos d'operador. | `MODE #channel +o nick` | `MODE #channel -o nick` |
| `l` | Posa o treu el nombre màxim de membres. | `MODE #channel +l 10` | `MODE #channel -l` |

## Instruccions

### Compilació

```bash
make
```

Compila el projecte amb `c++` i les opcions `-Wall -Wextra -Werror -std=c++98`, i genera l'executable `ircserv`.

Altres regles disponibles:

```bash
make clean
make fclean
make re
```

### Ús

```bash
./ircserv <port> <password>
```

- `port`: el port TCP on escoltarà el servidor (1024–65535).
- `password`: la contrasenya que qualsevol client IRC ha d'enviar amb `PASS` abans de registrar-se.

Exemple:

```bash
./ircserv 6667 mypassword
```

### Connectar-se i provar

#### 1. Provar amb Netcat (`nc`)
Amb el servidor en marxa, obre una altra terminal per connectar-te. Pots mirar la terminal del servidor per veure els logs i el trànsit de les connexions en directe.

Connecta't amb Netcat:
```bash
nc -C 127.0.0.1 6667
```

Un cop connectat, autentica't i registra el teu usuari escrivint aquestes ordres **en format cru** (prem Enter després de cada línia):
```text
PASS mypassword
NICK santi_nick
USER santi_user 0 * :santi_real
```

Després del registre, pots provar els canals i els missatges:
```text
JOIN #channel1
PRIVMSG #channel1 :Hello everyone
```

* **Missatges al canal:** obre una altra terminal, executa `nc` un altre cop i registra un segon usuari. Entra al mateix canal (`JOIN #channel1`) per veure com s'envien i es reben els missatges entre usuaris.
* **Missatges privats:** per enviar un missatge directe a un usuari, connecta un segon usuari des d'una altra terminal i fes servir aquesta sintaxi:
  ```text
  PRIVMSG santi_nick :Hola santi
  ```

* **Altres funcionalitats:** també pots provar la resta de modes de canal (`+i`, `+t`, `+k`, `+o`, `+l`) i les ordres d'operador (`KICK`, `INVITE`, `TOPIC`) amb la sintaxi estàndard d'IRC.

#### 2. Provar amb Irssi
Per provar el servidor amb un client de terminal real com **Irssi**, obre una altra terminal i segueix aquests passos:

1. Arrenca Irssi:
   ```bash
   irssi
   ```

2. **Connectar-se i autenticar-se:** connecta't passant la IP del servidor, el port i la contrasenya **en aquest ordre exacte**. Irssi enviarà automàticament les ordres de registre `PASS`, `NICK` i `USER` al teu servidor:
   ```text
   /connect 127.0.0.1 6667 mypassword
   ```

3. **Provar canals i comunicació:**
   * **Entrar a un canal (`JOIN`):** escriu `/join #channel1` per crear un canal o entrar-hi.
   * **Missatge públic (`PRIVMSG` a un canal):** escriu el text i prem Enter dins la finestra del canal per enviar-lo a tothom.
   * **Missatge privat (`PRIVMSG` a un usuari):** per obrir un xat privat amb un altre usuari connectat, fes servir:
	 ```text
	 /msg santi_nick Hi!This is a private message
	 ```

Irssi fa servir finestres separades per al servidor, els canals i les converses privades.
El número de la finestra activa surt a la barra d'estat d'Irssi.
Prem `Esc`, deixa'l anar i després prem el número de la finestra que vulguis obrir.

## Recursos

### Referències del protocol IRC

- [RFC 1459 — Internet Relay Chat Protocol](https://datatracker.ietf.org/doc/html/rfc1459)
- [RFC 2812 — IRC Client Protocol](https://datatracker.ietf.org/doc/html/rfc2812)
- [Internet Relay Chat Protocol Documentation ](https://dd.ircdocs.horse/)
- [modern.ircdocs.info — Modern IRC Client Protocol](https://modern.ircdocs.info/)
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)
- `man poll`, `man fcntl`, `man socket`

### Ús d'IA
Durant el desenvolupament d'aquest projecte s'ha fet servir Intel·ligència Artificial (Gemini) com a eina auxiliar per a aquestes tasques:

* **Depuració i resolució de problemes:** ajudar a trobar la causa d'errors lògics, gestionar casos límit en les desconnexions de sockets i comportaments inesperats del programa.
* **Idees per a les proves:** proposar idees i mètodes per organitzar escenaris de prova que validin el comportament dels canals i els casos límit.
* **Documentació i format:** estructurar i traduir el contingut d'aquest `README.md`.
