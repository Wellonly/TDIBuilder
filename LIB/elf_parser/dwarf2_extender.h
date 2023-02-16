#ifndef _DWARF2_EXTENDER
#define _DWARF2_EXTENDER

#include "config.h"
#include "compiler.h"
#include "bfd.h"
#include "smodatatype_calc.h"

struct dieinfo
{ //NOTE: member's positions has meaning!!! see the code!!!
    struct comp_unit   *dwarf2cu;
    struct abbrev_info *abbrev;
    bfd_byte           *next_die_info_ptr;
    bfd_byte           *attr_info_ptr[];   //attached list of attribute's pointers (in order with the abbrev)
};

typedef struct _dienode {
    struct _dienode *sibling; /* Its next sibling, if any. */
    struct _dienode *child;   /* Its first child, if any.  */
    struct _dienode *parent;  /* Its parent, if any.  */
    struct dieinfo   dinfo;   //last member position only!!!(see:create_dieinfo_entity())
} dienode;

union esym_flags {
    struct {
        unsigned int isError:1;
        unsigned int isVolatile:1;
        unsigned int isConstTypedef:1;
        unsigned int isConstBasetype:1;
//depr, defined in encoding:        unsigned int isVoid:1;
//depr, defined in encoding:        unsigned int isStruct:1;
//depr, defined in encoding:        unsigned int isUnion:1;
//depr, defined in encoding:        unsigned int isEnum:1;
//depr, defined in encoding: unsigned int isPointedTypedef:1;
//depr, defined in encoding: unsigned int isPointedBasetype:1;
    };
    unsigned int all;
};

typedef struct _extra_symbol_info {
    struct _extra_symbol_info *next;
    struct _extra_symbol_info *params;
    struct extra_comp_unit    *extra_cu;
    asymbol       *sym; //it's NULL if not linked!!!
    const char    *sourceFname;
    unsigned int   sourceLineno;
    union esym_flags flags;

    dienode      *pointerTyperef;
    dienode      *pointerBaseref;
    dienode      *basetyperef;
    dienode      *typeref;
    //next members used for buffering attribute's values (useful/interesting) ...
    const char   *name;
    const char   *typname;
    const char   *basetypname;
    int           encoding;
    long          type_size; //size
    long          base_type_size; //size
    //...
    struct dieinfo dinfo; //last member position only!!!(see:create_dieinfo_entity())
} extra_symbol_info;

union enumerator_flags {
    struct {
        unsigned int isError:1;
        unsigned int isReadDone:1;
    };
    unsigned int all;
};

typedef struct _enumerator_info {
    struct _enumerator_info*  next;
    struct _enumeration_info* owner;
    union enumerator_flags    flags;
    //next members used for buffering attribute's values (useful/interesting) ...
    const char   *name;
    long          value;
    //...
    struct dieinfo dinfo; //last member position only!!!(see:create_dieinfo_entity())
} enumerator_info;

union enumeration_flags {
    struct {
        unsigned int isError:1;
        unsigned int isReadDone:1;
    };
    unsigned int all;
};

typedef struct _enumeration_info {
    struct _enumeration_info *next;
    enumerator_info*  enumerator_last;
    enumerator_info*  enumerator_first;
    union enumeration_flags   flags;

    const char   *name;
    long          sizeb;
    //...
    struct dieinfo dinfo; //last member position only!!!(see:create_dieinfo_entity())
} enumeration_info;

struct extra_comp_unit
{
  struct extra_comp_unit *next;
  struct comp_unit       *dwarf2cu;

  extra_symbol_info      *extra_symbols_chain; /* A list of the symbols have been read in this comp. unit. */
  bfd_byte               *next_unread_die_info_ptr;

};

struct dwarf2_extender
{ //main dwarf2_extender struct...
    bfd* abfd;
    struct extra_comp_unit* extra_units;
    enumeration_info*  enums_chain;
    enumeration_info** enums_table;
    int                enums_count;
};

extra_symbol_info* find_extra_symbol_info(void* symptr, const char *sourceFname, unsigned int sourceLineno);
extra_symbol_info* scan_symbol_for_parameter_info(extra_symbol_info* sym, int target_param_num);

void* dwarf2_extender_init(bfd *file);
int extSymbol_encoding(extra_symbol_info* esym);
long extSymbol_size(extra_symbol_info* esym);
long extSymbol_basetype_size(extra_symbol_info* esym);
const char *extSymbol_typename(extra_symbol_info* esym);
const char *extSymbol_basetypename(extra_symbol_info* esym);
int extSymbol_is_pointer(extra_symbol_info* esym);
int extSymbol_pointer_size(extra_symbol_info* esym);
int extSymbol_is_basetype_pointer(extra_symbol_info* esym);
int extSymbol_basetype_pointer_size(extra_symbol_info* esym);

__EXTERN_C_BEGIN

void** Enumeration_table(void);
int Enumerations_count(void);
void* Enumeration_first(void);
void* Enumeration_next(void* enumptr);
const char* Enumeration_name(void* enumptr);
const char* Enumeration_sourceFileName(void* enumptr);
int Enumeration_sizeb(void* enumptr);

void* Enumerator_first(void* enumeration_ptr);
void* Enumerator_next(void* enumerator_ptr);
const char* Enumerator_name(void* enumerator_ptr);
long Enumerator_value(void* enumerator_ptr);
void* Enumerators_scan_for_name(void *enumeration_ptr, const char* scanname);

__EXTERN_C_END

void free_extra_comp_units();

#endif // _DWARF2_EXTENDER

