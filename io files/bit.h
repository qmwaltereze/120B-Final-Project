bit.h
Today
12:43 PM
Q
You moved an item to
Google Drive Folder
io files
C
bit.h
12:43 PM
Q
You renamed an item
C
bit.h
Copy of bit.h
12:41 PM
Q
You moved an item to
Google Drive Folder
my custom project
C
Copy of bit.h
12:39 PM
Q
You created an item in
My Drive
C
Copy of bit.h
// Permission to copy is granted provided that this header remains intact. 
// This software is provided with no warranties.

////////////////////////////////////////////////////////////////////////////////

#ifndef BIT_H
#define BIT_H

////////////////////////////////////////////////////////////////////////////////
//Functionality - Sets bit on a PORTx
//Parameter: Takes in a uChar for a PORTx, the pin number and the binary value 
//Returns: The new value of the PORTx
unsigned char SetBit(unsigned char pin, unsigned char number, unsigned char bin_value) 
{
	return (bin_value ? pin | (0x01 << number) : pin & ~(0x01 << number));
}

////////////////////////////////////////////////////////////////////////////////
//Functionality - Gets bit from a PINx
//Parameter: Takes in a uChar for a PINx and the pin number
//Returns: The value of the PINx
unsigned char GetBit(unsigned char port, unsigned char number) 
{
	return ( port & (0x01 << number) );
}

#endif //BIT_H
