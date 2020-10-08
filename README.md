# Battleship
Implementation in C of the typical Battleship game.

funcs:
// inicia taulell defensa colocant tots els baixells automaticament
iniciaTaulellDefensaAutomatic (taulell_vaixells, dim)
// inicia taulell defensa manualment colocant vaixells 1 x 1
iniciaTaulellDefensaManualment()
// inicia taulell atac amb ?
iniciaTaulellAtac
// comprova si el vaixell ha sigut enfonsat o no
bool isSunk(taulell_vaixells, dim, unsigned int x, unsigned int y)
// puts water in a ship locayed by coordinates
floodSorroundings(taulell_vaixells, dim, , unsigned int x, unsigned int y)
// Show board
showBoard(taulell_vaixells, dim)
/* Aquesta funció actualitza el taulell de vaixells i informa del resultat del llançament:
Casella repetida (0). Quan ja s'ha disparat abans sobre la casella.
Aigua (1)
Tocat (2)
Tocat i enfonsat (3) */
dispara(taulell_vaixell, dim, unsigned int x, unsigned int y)
// Choose next movement while trying to minimize shoots
void computeNextMovement(taulell_vaixell, dim, *unsigned int x, *unsigned int y)
// calcula puntuacio
int calculateScore(dim, *unsigned int shootResults, unsigned int totalShoots)

Elements:
2 boards attack
2 boards defense
dim 8x8, 9 x 9, 10x10
2 vectors of result of shoots

board attack:
? casella no descoberta
~ aigua descoberta
X vaixell tocat

board defense:
~ aigua no descoberta
- aigua descoberta
X vaixell propi tocat
@ vaixells propis




