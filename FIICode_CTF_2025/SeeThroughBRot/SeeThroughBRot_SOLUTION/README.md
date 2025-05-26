# SeeThroughBRot

## Flag

Flag format: "FIICODE25{ITALIAN_BRAINROT_VSK}"

## Description

```
The image includes two pieces of data: a HINT and THE FLAG.
The HINT can be found by taking the characters associated with the values of the alpha channel for black pixels (r, g, b) = (0, 0, 0).
This will yield "FOLLOW (380, 350)".

Starting from this position in the image, by taking the character associated with the value of the blue pixels and moving one position according to the value of the alpha channel, one can find the final flag.

The direction is encoded in the last 4 bits of the alpha channel:
	- MOVE UP ONE PIXEL    =  8 = 00001000
	- MOVE DOWN ONE PIXEL  =  4 = 00000100
	- MOVE LEFT ONE PIXEL  =  2 = 00000010
	- MOVE RIGHT ONE PIXEL =  1 = 00000001
```
