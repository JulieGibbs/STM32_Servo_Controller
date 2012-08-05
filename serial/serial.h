#ifndef	_SERIAL_H
#define	_SERIAL_H

#include	<stdint.h>
<<<<<<< HEAD
//#include	<avr/io.h>
//#include	<avr/pgmspace.h>
=======
#include	<avr/io.h>
#include	<avr/pgmspace.h>
>>>>>>> Using original teacup firmware files

// initialise serial subsystem
void serial_init(void);

// return number of characters in the receive buffer, and number of spaces in the send buffer
uint8_t serial_rxchars(void);
// uint8_t serial_txchars(void);

// read one character
uint8_t serial_popchar(void);
// send one character
void serial_writechar(uint8_t data);

// read/write many characters
// uint8_t serial_recvblock(uint8_t *block, int blocksize);
void serial_writeblock(void *data, int datalen);

void serial_writestr(uint8_t *data);

// write from flash
<<<<<<< HEAD
//void serial_writeblock_P(PGM_P data, int datalen);
//void serial_writestr_P(PGM_P data);
=======
void serial_writeblock_P(PGM_P data, int datalen);
void serial_writestr_P(PGM_P data);
>>>>>>> Using original teacup firmware files

#endif	/* _SERIAL_H */
