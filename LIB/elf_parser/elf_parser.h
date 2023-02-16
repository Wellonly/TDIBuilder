
#ifndef _ELF_PARSER
#define _ELF_PARSER

#include <compiler.h>
#include <stdbool.h>

#include "GlobalStatusErrorCodes.h"
#include "dwarf2_extender.h"

void syms_prn(); //debug only!!!

__EXTERN_C_BEGIN

G8 elf_parser(char *filename, int isExternalOnly);
struct dwarf2_extender *extend_bfd();

void FreeBfd();
//bfd* BfdFile();


void **SymsTable();
long SymsCount();
const char* SymName(void* symptr);
unsigned long SymModuleRelativeValue(void* symptr);
unsigned long SymSectionRelativeValue(void* symptr);
unsigned long SymAbsoluteValue(void* symptr);
int SymIsTextSection(void* symptr);
const char* SymSectionName(void* symptr);
unsigned long SymBodySize(void* symptr);
int SymClass(void* symptr);
int SymType(void* symptr);
int SymBind(void* symptr);
unsigned int SymFlags(void* symptr);
int Arch_bits_per_word();
const char* Arch_printable_name();
int SymIsFunction(void* symptr);

//extended symbol routines info................
extra_symbol_info* SymDwarf2ExtraInfo(void* symptr);
unsigned int SymSourceLine(void* symptr);
const char* SymSourceFileName(void* symptr);
int SymbolEncoding(void* symptr);
const char *SymbolTypeName(void *symptr);
const char *SymbolBaseTypeName(void *symptr);
long SymbolDataSize(void *symptr);
long Symbol_basetype_dataSize(void *symptr);
bool SymbolIsPointer(void* symptr);
bool SymbolIsBasetypePointer(void* symptr);

bool SymbolParameter_isBasetypePointer(void *symptr, int param_index);
const char *SymbolParameter_typeName(void *symptr, int param_index);
long SymbolParameter_dataSize(void *symptr, int param_index);
long SymbolParameter_basetype_dataSize(void *symptr, int param_index);
int SymbolParameter_encoding(void *symptr, int param_index);
int SymbolParameter_PointerSize(void* symptr, int param_index);
int SymbolParameter_BasetypePointerSize(void* symptr, int param_index);

__EXTERN_C_END

#endif // _ELF_PARSER

