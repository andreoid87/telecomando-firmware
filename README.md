# Telecomando volume - firmware PIC16F84

Firmware per un modulo di aumento/diminuzione volume con trasmissione IR.
Il codice e scritto nello stile mikroC PRO for PIC (uso di `sbit`, `Delay_ms`, ecc.).
Il file progetto `.mcppi` indica `P16F84` a 4 MHz.

## Contenuto repo
- `Telecomando.c`: sorgente firmware.
- `Telecomando.mcppi`: file progetto mikroC (device, clock, file).
- `Telecomando.cfg`: configurazione device (config word).

## Requisiti
- mikroC PRO for PIC (o ambiente compatibile con sintassi `sbit`).
- MCU: PIC16F84
- Clock: 4 MHz (come da `Telecomando.mcppi`)

## Mappa I/O (dal sorgente)
- `RA2` (`anodo`): uscita che pilota l'anodo LED IR (attiva bassa, `ON=0`).
- `RA0`: uscita di abilitazione durante la trasmissione (viene portata alta nei while).
- `RA1`: ingresso usato per distinguere tra `VOLMENO` e `STANDBY`.
- `RB4` (`l1l2`): ingresso pulsante L1/L2.
- `RB5` (`l3l4`): ingresso pulsante L3/L4.
- `RB6` (`l5mute`): ingresso pulsante L5/MUTE.
- `RB7` (`vol`): ingresso pulsante VOL+/VOL-.

Gli ingressi sono considerati attivi bassi (`PON=0`, `POFF=1`) con pull-up interni.

## Logica firmware
- `main`:
  - Configura RA2, RA0, RA1 come output (RA1 temporaneamente diventa input quando serve).
  - Imposta `TRISB = 0xFF` (tutti gli ingressi).
  - Abilita interrupt su PORTB (`RBIE`) e interrupt globali (`GIE`).
  - Abilita i pull-up interni su PORTB.
  - Entra in loop infinito con `SLEEP` per basso consumo.
- `interrupt` (vector 0x0004):
  - Si attiva su cambio stato a PORTB; finche il tasto resta premuto, invia il codice relativo.
  - `l1l2`: invia `L1` se il pin torna alto, `L2` se resta basso.
  - `l3l4`: invia `L3` o `L4` con la stessa logica.
  - `l5mute`: invia `L5` o `MUTE` (MUTE ha ritardo piu lungo).
  - `vol`: se premuto, legge `RA1`:
    - `RA1==0` -> `STANDBY`
    - `RA1==1` -> `VOLMENO`
    - se il tasto torna alto subito -> `VOLPIU`
  - Alla fine ripristina `anodo=OFF` e azzera `RBIF`.

## Trasmissione IR
La funzione `trasmetti` invia 12 bit, MSB first, usando:
- `uno()`: genera una sequenza di impulsi con duty e pausa piu lunga.
- `zero()`: genera una sequenza di impulsi con duty e pausa piu corta.

Le temporizzazioni sono ottenute con loop e `NOP`, quindi dipendono dal clock a 4 MHz.

## Note
`Telecomando (2).c` e un duplicato del sorgente e non e incluso in questo repo.
