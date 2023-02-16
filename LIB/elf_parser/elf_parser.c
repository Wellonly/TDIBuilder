/* elf_parser.c - retrieve elf info
   Copyright 2015, 2016
   Well Software Foundation, Inc.
  */

#include "sysdep.h"
#include "bfd.h"
#include "progress.h"
#include "getopt.h"
#include "aout/stab_gnu.h"
#include "aout/ranlib.h"
#include "demangle.h"
#include "libiberty.h"
#include "elf-bfd.h"
#include "elf/common.h"
#include "bucomm.h"
#include "plugin.h"

#include "elfcomm.h"
#include "dwarf.h"

#include "GlobalStatusErrorCodes.h"
#include "c_cpp_macros.h"
#include "elf_parser.h"

static int external_only = 0;	/* Print external symbols only.(0/1: w/wo static symbols)  */
static const int defined_only = 1;	/* Print defined symbols only.  */
static const int do_demangle = 0;	/* Pretty print C++ symbol names. zvv: increases cpu-cost!!! */
static const int dynamic = 0;
static const int undefined_only = 0;   /* Print undefined symbols only.  */
static const int print_debug_syms = 0; /* Print debugger-only symbols too.  */
static const int sort_by_size = 0;	/* Sort by size of symbol.  */
static const int allow_special_symbols = 0;  /* Allow special symbols.  */

static bfd_boolean is_relocatable = TRUE; //FALSE;

static bfd *file = NULL;
static const unsigned int symptr_size = sizeof(void*);

/* The symbol table.  */
//static elf_symbol_type** elfsyms = NULL; //asymbol or elf_symbol_type
static asymbol** syms = NULL;
static long symcount = 0; /* Number of symbols in `syms'.  */
static void *minisyms = NULL;

void
free_debug_section (enum dwarf_section_display_enum debug)
{
  struct dwarf_section *section = &debug_displays [debug].section;

  if (section->start == NULL)
    return;

  free ((char *) section->start);
  section->start = NULL;
  section->address = 0;
  section->size = 0;
}

static int
load_specific_debug_section (enum dwarf_section_display_enum debug, asection *sec, void *file)
{
  struct dwarf_section *section = &debug_displays [debug].section;
  bfd *abfd = (bfd *) file;
  bfd_boolean ret;

  /* If it is already loaded, do nothing.  */
  if (section->start != NULL)
    return 1;

  section->address = 0;
  section->size = bfd_get_section_size (sec);
  section->start = NULL;
  ret = bfd_get_full_section_contents (abfd, sec, &section->start);

  if (! ret)
    {
      free_debug_section (debug);
      printf (_("\nCan't get contents for section '%s'.\n"),
          section->name);
      return 0;
    }

  if (is_relocatable && debug_displays [debug].relocate)
    {
      bfd_cache_section_contents (sec, section->start);

      ret = bfd_simple_get_relocated_section_contents (abfd,
                               sec,
                               section->start,
                               syms) != NULL;

      if (! ret)
        {
          free_debug_section (debug);
          printf (_("\nCan't get contents for section '%s'.\n"),
              section->name);
          return 0;
        }
    }

  return 1;
}

// from objdump
int
load_debug_section (enum dwarf_section_display_enum debug, void *file)
{
  struct dwarf_section *section = &debug_displays [debug].section;
  bfd *abfd = (bfd *) file;
  asection *sec;

  /* If it is already loaded, do nothing.  */
  if (section->start != NULL)
    return 1;

  /* Locate the debug section.  */
  sec = bfd_get_section_by_name (abfd, section->uncompressed_name);
  if (sec != NULL)
    section->name = section->uncompressed_name;
  else
    {
      sec = bfd_get_section_by_name (abfd, section->compressed_name);
      if (sec != NULL)
        section->name = section->compressed_name;
    }
  if (sec == NULL)
    return 0;

  return load_specific_debug_section (debug, sec, file);
}

static void
print_symbol (bfd *abfd ATTRIBUTE_UNUSED, asymbol *sym)
{
//    symbol_info syminfo;

//    bfd_get_symbol_info (abfd, sym, &syminfo);
//  elf_symbol_type *elfinfo = (elf_symbol_type *) sym;

//    printf ("%08lx|%s|%s \n", sym->value, sym->name, sym->section->name); //  format->print_symbol_info (&info, abfd);
    printf ("prn sym:%08lx|%s\n", (ulong)sym, sym->name); //  format->print_symbol_info (&info, abfd);

}

//fill syms table...
static G8
print_symbols (bfd *abfd, bfd_boolean is_dynamic, void *minisyms, long symcount)
{
    asymbol *store;
    bfd_byte *from, *fromend;

    store = bfd_make_empty_symbol (abfd);
    if (store == NULL) return err_NoEnoughSpace;

    syms = (asymbol**) xmalloc (symcount * symptr_size);

    asymbol** tosyms = syms;

    from = (bfd_byte *) minisyms;
    fromend = from + symcount * symptr_size;
    for (; from < fromend; from += symptr_size)
    {
        asymbol *sym;
        sym = bfd_minisymbol_to_symbol (abfd, is_dynamic, from, store);
        if (sym == NULL) return err_Range;
        *tosyms++ = sym;
#ifndef QT_NO_DEBUG
        printf ("from:%08lx|sym:%08lx|%s\n",(ulong)from, (ulong)sym, sym->name); //  format->print_symbol_info (&info, abfd);
        // print_symbol (abfd, sym);
#endif
    }
    return msg_Success;
}

/* Choose which symbol entries to print;
   compact them downward to get rid of the rest.
   Return the number of symbols to be printed.  */

static long
filter_symbols (bfd *abfd, bfd_boolean is_dynamic, void *minisyms, long symcount)
{
  bfd_byte *from, *fromend, *to;
  asymbol *store;

  store = bfd_make_empty_symbol (abfd);
  if (store == NULL)
    bfd_fatal (bfd_get_filename (abfd));

  from = (bfd_byte *) minisyms;
  fromend = from + symcount * symptr_size;
  to = (bfd_byte *) minisyms;

  for (; from < fromend; from += symptr_size)
    {
      int keep = 0;
      asymbol *sym;

      sym = bfd_minisymbol_to_symbol (abfd, is_dynamic, (const void *) from, store);
      if (sym == NULL)
        bfd_fatal (bfd_get_filename (abfd));

      if (undefined_only)
        keep = bfd_is_und_section (sym->section);
      else if (external_only)
        /* PR binutls/12753: Unique symbols are global too.  */
        keep = ((sym->flags & (BSF_GLOBAL
                       | BSF_WEAK
                       | BSF_GNU_UNIQUE)) != 0
            || bfd_is_und_section (sym->section)
            || bfd_is_com_section (sym->section));
      else
        keep = 1;

      if (keep
          && ! print_debug_syms
          && (sym->flags & BSF_DEBUGGING) != 0)
        keep = 0;

      if (keep
          && defined_only)
        {
          if (bfd_is_und_section (sym->section))
            keep = 0;
        }

      if (keep
          && bfd_is_target_special_symbol (abfd, sym)
          && ! allow_special_symbols)
        keep = 0;

      if (keep && *(char *)sym->name == '_')
        keep = 0;

      if (keep)
        {
          if (to != from)
            memcpy (to, from, symptr_size);
          to += symptr_size;
        }
    }
  return (to - (bfd_byte *) minisyms) / symptr_size;
}

static G8 display_rel_file(bfd *abfd) //display_rel_file (bfd *abfd, bfd *archive_bfd)
{
    if (!(bfd_get_file_flags (abfd) & HAS_SYMS))
    {
        return wrn_NotFound; //non_fatal (_("%s: no symbols"), bfd_get_filename (abfd));
    }

    unsigned int sy_size;
    symcount = bfd_read_minisymbols (abfd, dynamic, &minisyms, &sy_size);
    if (symptr_size != sy_size) return err_WrongAddress;
    if (symcount <= 0) return wrn_NotFound;

    symcount = filter_symbols (abfd, dynamic, minisyms, symcount);

    G8 ret = print_symbols (abfd, dynamic, minisyms, symcount);

    //syms_prn();
    return ret;
}

/////////////////////////////////////API... ////////////////////////////////////////

void syms_prn()
{
    int i=0;
    for (; i < symcount; i++)
    {
        print_symbol (file, syms[i]);
    }
}

//return 0 (msg_Success) if ok, else a G8 status code!!!
G8
elf_parser(char *filename, int isExternalOnly)
{
    external_only = isExternalOnly;
    xmalloc_set_program_name (filename);
    bfd_init ();
    set_default_bfd_target ();

    file = bfd_fopen (filename, NULL, FOPEN_RB, -1);
    if (file == NULL) return err_IOError;

    G8 retValue = msg_Success;

    file->flags |= BFD_DECOMPRESS;   /* If printing line numbers, decompress the debug sections.  */

    char **matching;
    if (bfd_check_format_matches (file, bfd_object, &matching))
    {
        retValue = display_rel_file (file);
    }
    else {
        retValue = err_Unsupported;
    }

    if (retValue == msg_Success) {
        if (!dwarf2_extender_init(file)) retValue = err_NoEnoughSpace;
    }

    if (retValue != msg_Success) {
        FreeBfd();
    }

//    syms_prn();
    return retValue;
}

void FreeBfd() {
    //will be freed by bfd?!!    //free_extra_comp_units();
    if (syms) {free (syms); syms=NULL;}
    if (minisyms) {free(minisyms); minisyms=NULL;}
    if (file) {bfd_close (file); file= NULL;}
}

long SymsCount() {
    return symcount;
}

//asymbol from elf_symbol_type
void** SymsTable() {
    return (void**)syms;
}

const char *SymName(void* symptr) {
    asymbol* sy = (asymbol*)symptr;
    return sy->name;
}

unsigned long SymModuleRelativeValue(void* symptr) {
    return ((asymbol*)symptr)->value;
}
unsigned long SymSectionRelativeValue(void* symptr)
{ //TODO: test on other arch then arm (e.g. AVR)
    unsigned long result = SymAbsoluteValue(symptr);
    return result & 0xFFFFFF;
}
unsigned long SymAbsoluteValue(void* symptr) {
    return ((elf_symbol_type*)symptr)->internal_elf_sym.st_value;
}

int SymIsTextSection(void* symptr) {
    return !strcmp(SymSectionName(symptr), ".text");
}

const char* SymSectionName(void* symptr) {
    return ((asymbol*)symptr)->section->name;
}

//SymBodySize() returns size of code for functions and size of data for objects!!! (previous: SymDataSize())
unsigned long SymBodySize(void* symptr) {
    elf_symbol_type* sy = (elf_symbol_type*)symptr;
    return sy->internal_elf_sym.st_size;
}

int SymClass(void* symptr) {
    return bfd_decode_symclass((asymbol*)symptr);
}

//#define STT_NOTYPE	0		/* Symbol type is unspecified */
//#define STT_OBJECT	1		/* Symbol is a data object */
//#define STT_FUNC	2		/* Symbol is a code object */
//#define STT_SECTION	3		/* Symbol associated with a section */
//#define STT_FILE	4		/* Symbol gives a file name */
//#define STT_COMMON	5		/* An uninitialised common block */
//#define STT_TLS		6		/* Thread local data object */
//and so on... see elf/common.h for details!!!
//st_info contains symbol binding and symbol type
int SymType(void* symptr) {
    return ELF_ST_TYPE (((elf_symbol_type*)symptr)->internal_elf_sym.st_info);
}

int SymIsFunction(void* symptr) {
    return ELF_ST_TYPE (((elf_symbol_type*)symptr)->internal_elf_sym.st_info) == STT_FUNC;
}

//#define STB_LOCAL	0		/* Symbol not visible outside obj */
//#define STB_GLOBAL	1		/* Symbol visible outside obj */
//#define STB_WEAK	2		/* Like globals, lower precedence */
//and so on... see elf/common.h for details!!!
//st_info contains symbol binding and symbol type
int SymBind(void* symptr) {
    return ELF_ST_BIND (((elf_symbol_type*)symptr)->internal_elf_sym.st_info);
}

unsigned int SymFlags(void* symptr) {
    return ((asymbol*)symptr)->flags;
}

int Arch_bits_per_word() {
    if (syms)
    { //get the bfd from first symbol!!!
        asymbol* sym = *syms;
        if (sym && sym->the_bfd && sym->the_bfd->arch_info)
            return sym->the_bfd->arch_info->bits_per_word;
    }
    return 0;
}

const char* Arch_printable_name() {
    if (syms)
    { //get the bfd from first symbol!!!
        asymbol* sym = *syms;
        if (sym && sym->the_bfd && sym->the_bfd->arch_info)
            return sym->the_bfd->arch_info->printable_name;
    }
    return NULL;
}



////////// extended symbol routines info................ //////////////////////////////////////////
//static struct dwarf2_debug* dw2deb;
//struct dwarf2_debug;
extra_symbol_info* SymDwarf2ExtraInfo(void* symptr) {
    extra_symbol_info* result = (extra_symbol_info*)((asymbol*)symptr)->udata.p;
    if (!result) {
        const char *lastSourceFname = NULL;
        unsigned int lastSourceLineno = 0;
        if (bfd_find_line (file, syms, symptr, &lastSourceFname, &lastSourceLineno)) {
            result = find_extra_symbol_info(symptr, lastSourceFname, lastSourceLineno);
        }
    }
    return result;
}

unsigned int SymSourceLine(void* symptr) {
    extra_symbol_info* inf = SymDwarf2ExtraInfo(symptr);
    if (inf) return inf->sourceLineno;
    return 0; //an error or unavailable!
}
const char *SymSourceFileName(void *symptr)
{
    extra_symbol_info* inf = SymDwarf2ExtraInfo(symptr);
    if (inf) return inf->sourceFname;
    return NULL;
}

//for code meaning: see read_and_display_attr_value() in dwarf.c
int SymbolEncoding(void* symptr) {
    return extSymbol_encoding(SymDwarf2ExtraInfo(symptr));
}

const char *SymbolTypeName(void *symptr)
{
    return extSymbol_typename(SymDwarf2ExtraInfo(symptr));
}

const char *SymbolBaseTypeName(void *symptr)
{
    return extSymbol_basetypename(SymDwarf2ExtraInfo(symptr));
}

long SymbolDataSize(void *symptr)
{
    if (SymIsFunction(symptr)) {
        return extSymbol_size(SymDwarf2ExtraInfo(symptr));
    }
    return SymBodySize(symptr);
}

//NOTE: sizes for data objects and functions is different!!!
long Symbol_basetype_dataSize(void *symptr)
{
    if (SymIsFunction(symptr)) {
        return extSymbol_basetype_size(SymDwarf2ExtraInfo(symptr));
    }
    return SymBodySize(symptr);
}

bool SymbolIsPointer(void* symptr) {
    return extSymbol_is_pointer(SymDwarf2ExtraInfo(symptr));
}

bool SymbolIsBasetypePointer(void* symptr) {
    return extSymbol_is_basetype_pointer(SymDwarf2ExtraInfo(symptr));
}

//////////////Parameter's routines... /////////////////////////////////

bool SymbolParameter_isBasetypePointer(void *symptr, int param_index)
{
    if (SymIsFunction(symptr)) {
        extra_symbol_info* syminf = SymDwarf2ExtraInfo(symptr);
        if (syminf) {
            extra_symbol_info* paramSymInf = scan_symbol_for_parameter_info(syminf, param_index);
            return iif(paramSymInf, extSymbol_is_basetype_pointer(paramSymInf), false);
        }
    }
    return false;
}


const char *SymbolParameter_typeName(void *symptr, int param_index)
{
    if (SymIsFunction(symptr)) {
        extra_symbol_info* syminf = SymDwarf2ExtraInfo(symptr);
        if (syminf) {
            return extSymbol_typename(scan_symbol_for_parameter_info(syminf, param_index));
        }
    }
    return NULL;
}

//NOTE: sizes for data objects and functions is different!!!
//-1:error,-2:no parameter found
long SymbolParameter_dataSize(void *symptr, int param_index)
{
    if (SymIsFunction(symptr)) {
        extra_symbol_info* syminf = SymDwarf2ExtraInfo(symptr);
        if (syminf) {
            extra_symbol_info* paramSymInf = scan_symbol_for_parameter_info(syminf, param_index);
            return iif(paramSymInf, extSymbol_size(paramSymInf), -2);
        }
    }
    return -1;
}

//NOTE: sizes for data objects and functions is different!!!
//-1:error,-2:no parameter found
long SymbolParameter_basetype_dataSize(void *symptr, int param_index)
{
    if (SymIsFunction(symptr)) {
        extra_symbol_info* syminf = SymDwarf2ExtraInfo(symptr);
        if (syminf) {
            extra_symbol_info* paramSymInf = scan_symbol_for_parameter_info(syminf, param_index);
            return iif(paramSymInf, extSymbol_basetype_size(paramSymInf), -2);
        }
    }
    return -1;
}

//-1:error,-2:no parameter found
int SymbolParameter_encoding(void *symptr, int param_index)
{
    if (SymIsFunction(symptr)) {
        extra_symbol_info* syminf = SymDwarf2ExtraInfo(symptr);
        if (syminf) {
            extra_symbol_info* paramSymInf = scan_symbol_for_parameter_info(syminf, param_index);
            return iif(paramSymInf, extSymbol_encoding(paramSymInf), -2);
        }
    }
    return -1;
}

//-1:error,-2:no parameter found
int SymbolParameter_PointerSize(void *symptr, int param_index)
{
    if (SymIsFunction(symptr)) {
        extra_symbol_info* syminf = SymDwarf2ExtraInfo(symptr);
        if (syminf) {
            extra_symbol_info* paramSymInf = scan_symbol_for_parameter_info(syminf, param_index);
            return iif(paramSymInf, extSymbol_pointer_size(paramSymInf), -2);
        }
    }
    return -1;
}

//-1:error,-2:no parameter found
int SymbolParameter_BasetypePointerSize(void *symptr, int param_index)
{
    if (SymIsFunction(symptr)) {
        extra_symbol_info* syminf = SymDwarf2ExtraInfo(symptr);
        if (syminf) {
            extra_symbol_info* paramSymInf = scan_symbol_for_parameter_info(syminf, param_index);
            return iif(paramSymInf, extSymbol_basetype_pointer_size(paramSymInf), -2);
        }
    }
    return -1;
}

//////////////Enumerations's routines... /////////////////////////////////

