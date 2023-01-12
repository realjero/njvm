
#ifndef MAKRO_H
#define MAKRO_H

#define IMMEDIATE(x) ((x) & 0x00FFFFFF)                                     // Zahl <= 23 Bit
#define SIGN_EXTEND(i) ((i) & 0x00800000 ? (i) | 0xFF000000 : (i))          // Wenn Bit 23 => 1, dann negative Zahl, die durch 8 weitere 1-en in den höchstwertigen Bits erweitert werden muss.
#define MSB (1 << (8 * sizeof(unsigned int) - 1))
#define IS_PRIMITIVE(objRef) (((objRef)->size & MSB) == 0)
#define GET_ELEMENT_COUNT(objRef) ((objRef)->size & ~MSB)
#define GET_REFS_PTR(objRef) ((ObjRef *) (objRef)->data)

#endif //MAKRO_H
