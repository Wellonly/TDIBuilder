/* dwarf2_extender.c
 * Extender for dwarf2.c functionality!!!
 *
*/

#include "dwarf2.c" //reusing!!!
#include "dwarf2_extender.h"
#include "dwarf2_52.c" //after dwarf2.c version had changed to v5.2 or higher this should be removed!!!
#include "c_cpp_macros.h"
#include <stdbool.h>

/* The enumerations chain */
//moved to extender: static enumeration_info* enums_chain = NULL;
//moved to extender: static struct extra_comp_unit* extra_units = NULL;
static struct dwarf2_extender *extender = NULL;

static const char* string_void = "void";
//static const char* string_const = "const";
//static const char* string_volatile = "volatile";

enum die_read_codes {
    die_read_code_ok,
    die_read_code_zero_abbrev,
    die_read_code_error,
};

//call dwarf2_symbol_CU() after find_line() only!!!
static struct comp_unit *dwarf2_symbol_CU(void* symptr, const char *sourceFname) {
    asymbol* sym = (asymbol*)symptr;
    struct dwarf2_debug* dwarf2debug = (struct dwarf2_debug*)elf_tdata(sym->the_bfd)->dwarf2_find_line_info;
    if ((void*)dwarf2debug->bfd_ptr != (void*)sym->the_bfd) {
        bfd_set_error (bfd_error_system_call); //bfd ptr's wrong!!!
        return NULL;
    }
    struct comp_unit* unit;
    if (sourceFname) {
        for (unit = dwarf2debug->all_comp_units; unit; unit = unit->next_unit) {
            if (strcmp(sourceFname, unit->name) == 0) {
                return unit;
            }
        }
    }
    //second stage scan...
    //we are here if unit's file name is NULL
    //or if sourceFname doesn't equal the symbol FName (it occurs when a unit contains a several .c files!!!)
    bfd_vma addrVMA = ((elf_symbol_type*)sym)->internal_elf_sym.st_value;
    const char *filename_ptr = 0;
    unsigned int linenumber_ptr = 0;
    if (sym->flags & BSF_FUNCTION) {
        for (unit = dwarf2debug->all_comp_units; unit; unit = unit->next_unit) {
            if (lookup_symbol_in_function_table (unit, sym, addrVMA, &filename_ptr, &linenumber_ptr)) {
                return unit;
            }
        }
    }
    else {
        for (unit = dwarf2debug->all_comp_units; unit; unit = unit->next_unit) {
            if (lookup_symbol_in_variable_table (unit, sym, addrVMA, &filename_ptr, &linenumber_ptr)) {
                return unit;
            }
        }
    }
    return NULL;
}

//next_attribute_offset() calc next attribute offset!!!
static bfd_byte *
next_attribute_offset(unsigned int form,
                struct comp_unit *    unit,
                bfd_byte *            info_ptr,
                bfd_byte *            info_ptr_end)
{
    bfd *abfd = unit->abfd;
    unsigned int bytes_read;

    if (info_ptr >= info_ptr_end) { bfd_set_error (bfd_error_no_contents); return NULL; }

    switch (form)
      {
      case DW_FORM_ref_addr: /* DW_FORM_ref_addr is an address in DWARF2, and an offset in DWARF3.*/
        if (unit->version == 3 || unit->version == 4)
        {
            info_ptr += unit->offset_size;
            break;
        }
        /* FALLTHROUGH */
      case DW_FORM_addr:
        info_ptr += unit->addr_size;
        break;
      case DW_FORM_GNU_ref_alt:
      case DW_FORM_sec_offset:
        info_ptr += unit->offset_size;
        break;
      case DW_FORM_block2:
        info_ptr += read_2_bytes_52 (abfd, info_ptr, info_ptr_end) + 2;
        break;
      case DW_FORM_block4:
        info_ptr += read_4_bytes_52 (abfd, info_ptr, info_ptr_end) + 4;
        break;
      case DW_FORM_data2:
        info_ptr += 2;
        break;
      case DW_FORM_data4:
        info_ptr += 4;
        break;
      case DW_FORM_data8:
        info_ptr += 8;
        break;
      case DW_FORM_string:
        read_string_52 (abfd, info_ptr, info_ptr_end, &bytes_read);
        info_ptr += bytes_read;
        break;
      case DW_FORM_strp:
      case DW_FORM_GNU_strp_alt:
        info_ptr += unit->offset_size;
        break;
      case DW_FORM_exprloc:
      case DW_FORM_block:
        info_ptr += safe_read_leb128 (abfd, info_ptr, &bytes_read, FALSE, info_ptr_end);
        info_ptr += bytes_read;
        break;
      case DW_FORM_block1:
        info_ptr += read_1_byte_52 (abfd, info_ptr, info_ptr_end) + 1;
        break;
      case DW_FORM_data1:
        info_ptr += 1;
        break;
      case DW_FORM_flag:
        info_ptr += 1;
        break;
      case DW_FORM_flag_present:
        break;
      case DW_FORM_sdata:
        safe_read_leb128 (abfd, info_ptr, &bytes_read, TRUE, info_ptr_end);
        info_ptr += bytes_read;
        break;
      case DW_FORM_udata:
        safe_read_leb128 (abfd, info_ptr, &bytes_read, FALSE, info_ptr_end);
        info_ptr += bytes_read;
        break;
      case DW_FORM_ref1:
        info_ptr += 1;
        break;
      case DW_FORM_ref2:
        info_ptr += 2;
        break;
      case DW_FORM_ref4:
        info_ptr += 4;
        break;
      case DW_FORM_ref8:
      case DW_FORM_ref_sig8:
        info_ptr += 8;
        break;
      case DW_FORM_ref_udata:
        safe_read_leb128 (abfd, info_ptr, &bytes_read, FALSE, info_ptr_end);
        info_ptr += bytes_read;
        break;
      case DW_FORM_indirect:
        form = safe_read_leb128 (abfd, info_ptr, &bytes_read, FALSE, info_ptr_end);
        info_ptr += bytes_read;
        info_ptr = next_attribute_offset(form, unit, info_ptr, info_ptr_end);
        break;
      default:
        bfd_set_error (bfd_error_bad_value); //(*_bfd_error_handler) (_("Dwarf Error: Invalid or unhandled FORM value: %#x."),form);
        return NULL;
      }
    return info_ptr;
}

/* Return the named attribute value or NULL if not there.  */
static void*
die_attr_read (const struct dieinfo *die, enum dwarf_attribute name)
{
    if (!die->abbrev) return NULL;
    struct attr_abbrev *attrs = die->abbrev->attrs;
    unsigned int attrnum = die->abbrev->num_attrs;
    unsigned int i;
    for (i = 0; i < attrnum; ++i)
    {
        if (attrs[i].name == name) {
            bfd_byte *iptr = die->attr_info_ptr[i];
            bfd_byte *eptr = die->next_die_info_ptr;
            if (iptr < eptr) {
                struct attribute attr;
                if (read_attribute_value_52 (&attr,
                                             attrs[i].form,
                                             die->dwarf2cu,
                                             iptr,
                                             eptr)) return (void*)attr.u.str;
            }
            break;
        }
    }
    return NULL;
}

#define die_pointers_length_max 22
static struct _static_buffer_dieinfo {
    struct dieinfo dinfo;
    bfd_byte *die_pointers[die_pointers_length_max];
} static_buffer_dieinfo;
//die_pointers_read() also returns: static_buffer_dieinfo.dinfo.next_die_info_ptr value
//  if NOT an error, which can retrieved by bfd_get_error()!!!
static enum die_read_codes die_pointers_read(struct comp_unit* unit, bfd_byte* info_ptr) {
    unsigned long abbrev_number;
    unsigned int bytes_read;
    bfd_byte *info_ptr_end = unit->stash->info_ptr_end;

    if (info_ptr >= info_ptr_end) {
        bfd_set_error (bfd_error_no_contents); return die_read_code_error; } /* PR 17512: file: 9f405d9d.  */
    abbrev_number = safe_read_leb128 (unit->abfd, info_ptr, &bytes_read, FALSE, info_ptr_end);
    info_ptr += bytes_read;
    if (! abbrev_number) {
        static_buffer_dieinfo.dinfo.next_die_info_ptr = info_ptr;
        return die_read_code_zero_abbrev;
    }
    struct abbrev_info *abbrev = lookup_abbrev (abbrev_number, unit->abbrevs);
    if (! abbrev) {
        bfd_set_error (bfd_error_bad_value); return die_read_code_error;
    }
    int abbrev_num_attrs = abbrev->num_attrs;
    if (abbrev_num_attrs > die_pointers_length_max) {
        bfd_set_error (bfd_error_file_too_big); return die_read_code_error;
    }

    static_buffer_dieinfo.dinfo.dwarf2cu = unit;
    static_buffer_dieinfo.dinfo.abbrev = abbrev;
    for (int i = 0; i < abbrev_num_attrs; ++i)
    {
        static_buffer_dieinfo.die_pointers[i] = info_ptr;
        info_ptr = next_attribute_offset(abbrev->attrs[i].form, unit, info_ptr, info_ptr_end);
        if (info_ptr == NULL) {
            bfd_set_error (bfd_error_no_contents); return die_read_code_error; }
    }
    static_buffer_dieinfo.dinfo.next_die_info_ptr = info_ptr;
    return die_read_code_ok;
}

//if create_dieinfo_entity() called with param dinfo == NULL the new entity was wo attribute's pointeres!!!
static void* create_dieinfo_entity(bfd* abfd, struct dieinfo* dinfo, size_t head_struct_sizeof) {
    unsigned int abbrev_num_attrs = iif(dinfo, dinfo->abbrev->num_attrs, 0);
    size_t attr_pointers_size = abbrev_num_attrs * sizeof (bfd_byte*);
    void* result = bfd_zalloc(abfd, head_struct_sizeof += attr_pointers_size);
    if (result) {
        if (dinfo) { //clone dinfo from the source...
            size_t dinfo_w_pointers_size = sizeof (struct dieinfo) + attr_pointers_size;
            intptr_t new_dinfo_ptr = (intptr_t)result + head_struct_sizeof - dinfo_w_pointers_size;
            memcpy((void*)new_dinfo_ptr, (const char*)dinfo, dinfo_w_pointers_size);
        }
    }
    else {
        bfd_set_error (bfd_error_no_memory);
    }
    return result;
}

/* Scan over each die in a comp. unit looking for extra info . */
static extra_symbol_info*
scan_unit_for_extra_info(struct extra_comp_unit* extra_unit, asymbol* sym)
{
    if (!sym) return NULL;
    struct comp_unit* unit = extra_unit->dwarf2cu;
    bfd_byte *info_ptr;
    extra_symbol_info *extsym;

    if (extra_unit->extra_symbols_chain)
    { //first: look through previous read extsyms stage...
        for (extsym = extra_unit->extra_symbols_chain; extsym; extsym = extsym->next) {
            if (strcmp(extsym->name, sym->name) == 0)
            { //found!
                extsym->sym = sym;
                return extsym;
            }
        }
    }
    //read stage...
    info_ptr = extra_unit->next_unread_die_info_ptr;
    if (!info_ptr) info_ptr = unit->first_child_die_ptr;

    while (info_ptr && info_ptr < unit->end_ptr) //(nesting_level)
    {
        struct abbrev_info *abbrev;
        enum die_read_codes rcode = die_pointers_read(unit, info_ptr);
        if (rcode == die_read_code_zero_abbrev)
        {
            info_ptr = static_buffer_dieinfo.dinfo.next_die_info_ptr;
            continue;
        }
        else if (rcode == die_read_code_error) {
            goto fail;
        }
        abbrev = static_buffer_dieinfo.dinfo.abbrev; //lookup_abbrev (abbrev_number,unit->abbrevs);
        if (   abbrev->tag == DW_TAG_subprogram
            || abbrev->tag == DW_TAG_entry_point
            || abbrev->tag == DW_TAG_inlined_subroutine
            || abbrev->tag == DW_TAG_variable)
        {
            const char* name = (const char*)die_attr_read(&static_buffer_dieinfo.dinfo, DW_AT_name);
            if (name && *name != '\x0' && *name != '_')
            { //name is ok...
                extsym = (extra_symbol_info*)create_dieinfo_entity(unit->abfd, &static_buffer_dieinfo.dinfo, sizeof (extra_symbol_info));
                if (!extsym) {
                    goto fail;
                }
                extsym->name = name;
                extsym->next = extra_unit->extra_symbols_chain;
                extra_unit->extra_symbols_chain = extsym;
                if (strcmp(name, sym->name) == 0) {
                    extsym->sym = sym; //extsym found: if so, link it together...
                    //but this must be linked outside:    sym->udata.p = extsym;
                    extra_unit->next_unread_die_info_ptr = static_buffer_dieinfo.dinfo.next_die_info_ptr;
                    return extsym;
                }
            }
        }

        if (abbrev->has_children)
        {
            while (abbrev->tag == DW_TAG_enumeration_type)
            { //add the enum to the enumeration collection...
                const char* name = (const char*)die_attr_read(&static_buffer_dieinfo.dinfo, DW_AT_name);
                if (!(name && *name != '\0' && *name != '_')) {
                    break; //filter up enumerations on name!!!
                }
                enumeration_info* enumeration;
                for (enumeration = extender->enums_chain; enumeration; enumeration = enumeration->next) {
                    if (strcmp(name, enumeration->name) == 0) {
                        break; //found enum!!!
                    }
                }
                if (enumeration) break; //exit on enum already collected!!!
                enumeration = (enumeration_info*)create_dieinfo_entity(unit->abfd, &static_buffer_dieinfo.dinfo, sizeof (enumeration_info));
                if (!enumeration)
                    goto fail;
                enumeration->name = name;
                enumeration->next = extender->enums_chain;
                extender->enums_chain = enumeration;
                break; //once passage enough!
            } //end collect enumerations
            void *next_sibling_address = die_attr_read(&static_buffer_dieinfo.dinfo, DW_AT_sibling);
            if (next_sibling_address) {
                info_ptr = unit->info_ptr_unit + (ulong)next_sibling_address;
            }
            else
            { //means: there are no DW_AT_sibling
                info_ptr = static_buffer_dieinfo.dinfo.next_die_info_ptr;
            }
        }
        else {
            info_ptr = static_buffer_dieinfo.dinfo.next_die_info_ptr;
        }
    } //while to end of the unit!
    if (info_ptr > extra_unit->next_unread_die_info_ptr)
    { //save info_ptr for next scans...
        extra_unit->next_unread_die_info_ptr = info_ptr;
    }
    return NULL;  //the sym does not found!!!
fail:
    (*_bfd_error_handler) (_("Dwarf2_extender error, bfd_error_type: %u."), (unsigned int)bfd_get_error());
    return NULL;
}

// symbol_extra_info() should be called once per sym after(with results of) find_line() only!!!
extra_symbol_info* find_extra_symbol_info(void* symptr, const char *sourceFname, unsigned int sourceLineno) {
    asymbol* sym = (asymbol*)symptr;
    struct comp_unit* unit = dwarf2_symbol_CU(symptr, sourceFname);
    if (!unit) return NULL;
    extra_symbol_info* result;
    // comp_unit found!!! next: looking for extra_unit...
    struct extra_comp_unit* extra_unit = extender->extra_units;
    while (extra_unit) {
        if (extra_unit->dwarf2cu && extra_unit->dwarf2cu->first_child_die_ptr == unit->first_child_die_ptr) {
            goto extra_unit_found;
        }
        extra_unit = extra_unit->next;
    }
    if (!extra_unit) { //if not found then we are create one...
        extra_unit = (struct extra_comp_unit*) bfd_zalloc (sym->the_bfd, sizeof (struct extra_comp_unit));
        if (!extra_unit) return NULL;
        extra_unit->next = extender->extra_units;
        extender->extra_units = extra_unit;
        extra_unit->dwarf2cu = unit;
    }
extra_unit_found: /*next: find extsym info...*/
    result = scan_unit_for_extra_info(extra_unit, sym);
    if (result) {
        sym->udata.p = result;
        result->sourceFname = sourceFname;
        result->sourceLineno = sourceLineno;
        return result;
    }
    return result;
}

//NOTE: sym->params == sym means: no params found!!
extra_symbol_info*
scan_symbol_for_parameter_info(extra_symbol_info* sym, int target_param_num)
{
    if (!sym) return NULL;
    if (sym->params == sym) return NULL; //no params found!!!
    bfd_byte *info_ptr;
    extra_symbol_info *param;
    extra_symbol_info *lastparam = NULL;
    int curr_param_num = 0;

    for (param = sym->params; param; param = param->next) {
        if (target_param_num == curr_param_num++) {
            return param;
        }
        lastparam = param;
    }

    struct comp_unit* unit = sym->dinfo.dwarf2cu;
    info_ptr = iif(lastparam, lastparam->dinfo.next_die_info_ptr, sym->dinfo.next_die_info_ptr);
    while (info_ptr)
    {
        struct abbrev_info *abbrev;
        enum die_read_codes rcode = die_pointers_read(unit, info_ptr);
        if (rcode != die_read_code_ok)
        {
            break;
        }
        abbrev = static_buffer_dieinfo.dinfo.abbrev; //static_buffer_dieinfo created by die_pointers_read()
        if (abbrev->tag == DW_TAG_formal_parameter)
        {
            param = (extra_symbol_info*)create_dieinfo_entity(unit->abfd, &static_buffer_dieinfo.dinfo, sizeof (extra_symbol_info));
            if (!param) break;
            if (lastparam) {
                lastparam->next = param;
            }
            else {
                sym->params = param;
            }
            if (target_param_num == curr_param_num++) {
                return param;
            }
            lastparam = param;
        }
        else
        {
            break;
        }
        info_ptr = static_buffer_dieinfo.dinfo.next_die_info_ptr;
    }
    if (!sym->params) sym->params = sym; //no params found!!!
    return NULL;
}

static dienode* die_node_read(struct comp_unit* unit, bfd_byte* info_ptr) {
    enum die_read_codes rcode = die_pointers_read(unit, info_ptr);
    if (rcode != die_read_code_ok) {
        return NULL;
    }
    return (dienode*)create_dieinfo_entity(unit->abfd, &static_buffer_dieinfo.dinfo, sizeof (dienode));
}

static dienode* die_node_for_void(struct comp_unit* unit)
{ //empty result for die_node_read()
static dienode* void_die_node = NULL;
    if (!void_die_node) {
        void_die_node = (dienode*)create_dieinfo_entity(unit->abfd, NULL, sizeof (dienode));
    }
    return void_die_node;
}

static inline dienode* baseTypeInfo_found(extra_symbol_info* esym, dienode* node, int encoding) {
    esym->encoding = iif(esym->pointerBaseref, encoding | DW_ATE_pointer, encoding);
    if (!esym->typeref) esym->typeref = node;
    esym->basetyperef = node;
    return node;
}

// extSymbol_type_read() fills extra_symbol_info struct and returns founded basetype node(), one of next:
// DW_TAG_base_type or DW_TAG_structure_type or DW_TAG_union_type or DW_TAG_enumeration_type
// besenode child order: b123n, where b is basetype and 123n its parents nodes(prefix)
static int _die_type_read_recursive_level = 0;
#define _die_type_read_recursive_level_max 22 //22 because of: a lot of typedefs may be defined!!!
static dienode* extSymbol_typeinfo_read(extra_symbol_info* esym, const struct dieinfo* die)
{ //extSymbol_typeinfo_read() may be called recursive!!!
    void *die_type_address = die_attr_read(die, DW_AT_type);
    if (!die_type_address) {
        return baseTypeInfo_found(esym, die_node_for_void(die->dwarf2cu), DW_ATE_void); // means: there are no DW_AT_type
    }
    dienode* node = die_node_read(die->dwarf2cu, die->dwarf2cu->info_ptr_unit + (ulong)die_type_address);
    if (!node) return NULL; //means: die_node_read() error!!!
    switch (node->dinfo.abbrev->tag) //enum dwarf_tag tag = node->dinfo.abbrev->tag;
    {
    case DW_TAG_structure_type:
        return baseTypeInfo_found(esym, node, DW_ATE_struct);
    case DW_TAG_union_type:
        return baseTypeInfo_found(esym, node, DW_ATE_union);
    case DW_TAG_enumeration_type:
        return baseTypeInfo_found(esym, node, DW_ATE_enum);
    case DW_TAG_base_type:
        return baseTypeInfo_found(esym, node, 0);
    case DW_TAG_typedef:
        if (!esym->typeref) esym->typeref = node; //first typedef
        break;
    case DW_TAG_pointer_type:
        if (esym->typeref) {
            esym->pointerBaseref = node;
        }
        else {
            esym->pointerTyperef = node;
        }
        break;
    case DW_TAG_const_type:
        if (esym->typeref) {
            esym->flags.isConstTypedef = 1;
        }
        else {
            esym->flags.isConstBasetype = 1;
        }
        break;
    case DW_TAG_volatile_type:
        esym->flags.isVolatile = 1;
        break;
    case DW_TAG_padding:
        return NULL;
    default:
        break;
    }
    //go by recursive way...
    if (_die_type_read_recursive_level > _die_type_read_recursive_level_max) {
        return NULL; //means: an recursive overflow error!!!
    }
    _die_type_read_recursive_level++;
    dienode* basenode = extSymbol_typeinfo_read(esym, &node->dinfo);
    --_die_type_read_recursive_level;
    if (basenode)
    { //child order changed from 123..b to b123.., where b is basetype and 123.. its prefix
        node->parent = basenode->parent;
        basenode->parent = node;
        return basenode;
    }
//we are here only if basetype has not found!!!
//will be freed by bfd?!! free(node);
    return NULL;
}

const char *extSymbol_typename(extra_symbol_info* esym) {
    if (!esym) return NULL;
    if (esym->typname) return esym->typname;
    if (esym->basetyperef || extSymbol_typeinfo_read(esym, &esym->dinfo)) {
        if (esym->typeref) {
            esym->typname = iif(esym->typeref->dinfo.abbrev,
                                (const char*)die_attr_read(&esym->typeref->dinfo, DW_AT_name),
                                string_void);
        }
    }
    return esym->typname;
}

const char *extSymbol_basetypename(extra_symbol_info* esym) {
    if (!esym) return NULL;
    if (esym->basetypname) return esym->basetypname;
    if (esym->basetyperef || extSymbol_typeinfo_read(esym, &esym->dinfo)) {
        esym->basetypname = iif(esym->basetyperef->dinfo.abbrev,
                            (const char*)die_attr_read(&esym->basetyperef->dinfo, DW_AT_name),
                            string_void);
    }
    return esym->basetypname;
}

int extSymbol_encoding(extra_symbol_info* esym) { //dwarf2ext_symbol_encoding
    if (!esym) return -1;
    if (esym->encoding) return esym->encoding;
    int result = -1;
    if (esym->basetyperef || extSymbol_typeinfo_read(esym, &esym->dinfo)) {
        if (esym->encoding) return esym->encoding; //auto filled encoding for structs,enums,unions!!!
        result = (int)(long)die_attr_read(&esym->basetyperef->dinfo, DW_AT_encoding);
    }
    return esym->encoding = result;
}

//NOTE: sizes for data objects and functions is different!!!
//extSymbol_size() returns -1 if an error
long extSymbol_size(extra_symbol_info* esym) {
    if (!esym) return -1;
    if (esym->type_size) return esym->type_size;
    long result;
    if (esym->basetyperef || extSymbol_typeinfo_read(esym, &esym->dinfo)) {
        struct dieinfo* dinfo;
        if (esym->pointerTyperef) {
            dinfo = &esym->pointerTyperef->dinfo;
        }
        else if(esym->pointerBaseref) {
            dinfo = &esym->pointerBaseref->dinfo;
        }
        else {
            dinfo = &esym->basetyperef->dinfo;
        }
        result = (long)die_attr_read(dinfo, DW_AT_byte_size);
    }
    else
    {
        result = -1;
    }
    return esym->type_size = result;
}

//NOTE: sizes for data objects and functions is different!!!
long extSymbol_basetype_size(extra_symbol_info* esym) {
    if (!esym) return -1;
    if (esym->base_type_size) return esym->base_type_size;
    long result;
    if (esym->basetyperef || extSymbol_typeinfo_read(esym, &esym->dinfo)) {
        result = (long)die_attr_read(&esym->basetyperef->dinfo, DW_AT_byte_size);
    }
    else
    {
        result = -1;
    }
    return esym->base_type_size = result;
}

int extSymbol_is_pointer(extra_symbol_info* esym) {
    if (!esym) return false;
    if (esym->basetyperef || extSymbol_typeinfo_read(esym, &esym->dinfo)) {
        return esym->pointerTyperef != 0;
    }
    return false;
}

int extSymbol_pointer_size(extra_symbol_info* esym) {
    if (!esym) return -1;
    if (esym->basetyperef || extSymbol_typeinfo_read(esym, &esym->dinfo)) {
        return iif(esym->pointerTyperef, (int)(long)die_attr_read(&esym->pointerTyperef->dinfo, DW_AT_byte_size), 0);
    }
    return -1;
}

int extSymbol_basetype_pointer_size(extra_symbol_info* esym) {
    if (!esym) return -1;
    if (esym->basetyperef || extSymbol_typeinfo_read(esym, &esym->dinfo)) {
        return iif(esym->pointerBaseref, (int)(long)die_attr_read(&esym->pointerBaseref->dinfo, DW_AT_byte_size), 0);
    }
    return -1;
}

int extSymbol_is_basetype_pointer(extra_symbol_info *esym)
{
    if (!esym) return false;
    if (esym->basetyperef || extSymbol_typeinfo_read(esym, &esym->dinfo)) {
        return esym->pointerBaseref != 0;
    }
    return false;
}

//////////////Enumerations's routines... /////////////////////////////////

static void Enumerations_read_from_extunit(struct extra_comp_unit* extra_unit)
{
    struct comp_unit* unit = extra_unit->dwarf2cu;
    bfd_byte *info_ptr = extra_unit->next_unread_die_info_ptr;
    if (!info_ptr) info_ptr = unit->first_child_die_ptr;

    while (info_ptr && info_ptr < unit->end_ptr)
    {
        struct abbrev_info *abbrev;
        enum die_read_codes rcode = die_pointers_read(unit, info_ptr);
        if (rcode == die_read_code_zero_abbrev)
        {
            info_ptr = static_buffer_dieinfo.dinfo.next_die_info_ptr;
            continue;
        }
        else if (rcode == die_read_code_error) {
            goto fail;
        }
        abbrev = static_buffer_dieinfo.dinfo.abbrev; //lookup_abbrev (abbrev_number,unit->abbrevs);
//TODO: find out: the previous unread symbols may be useful!!!
//        if (   abbrev->tag == DW_TAG_subprogram
//            || abbrev->tag == DW_TAG_entry_point
//            || abbrev->tag == DW_TAG_inlined_subroutine
//            || abbrev->tag == DW_TAG_variable)
//        {
//            const char* name = (const char*)die_attr_read(&static_buffer_dieinfo.dinfo, DW_AT_name);
//            if (name) {
//                if (*name == '\0')
//                    break;
//            }
//        }

        if (abbrev->has_children)
        {
            while (abbrev->tag == DW_TAG_enumeration_type)
            { //add the enum to the enumeration collection...
                const char* name = (const char*)die_attr_read(&static_buffer_dieinfo.dinfo, DW_AT_name);
                if (!(name && *name != '\0' && *name != '_')) {
                    break; //filter up enumerations on name!!!
                }
                enumeration_info* enumeration;
                for (enumeration = extender->enums_chain; enumeration; enumeration = enumeration->next) {
                    if (strcmp(name, enumeration->name) == 0) {
                        break; //found enum!!!
                    }
                }
                if (enumeration) break; //exit on enum already collected!!!
                enumeration = (enumeration_info*)create_dieinfo_entity(unit->abfd, &static_buffer_dieinfo.dinfo, sizeof (enumeration_info));
                if (!enumeration)
                    goto fail;
                enumeration->name = name;
                enumeration->next = extender->enums_chain;
                extender->enums_chain = enumeration;
                break; //once passage enough!
            } //end collect enumerations
            void *next_sibling_address = die_attr_read(&static_buffer_dieinfo.dinfo, DW_AT_sibling);
            if (next_sibling_address) {
                info_ptr = unit->info_ptr_unit + (ulong)next_sibling_address;
            }
            else
            { //means: there are no DW_AT_sibling
                info_ptr = static_buffer_dieinfo.dinfo.next_die_info_ptr;
            }
        }
        else {
            info_ptr = static_buffer_dieinfo.dinfo.next_die_info_ptr;
        }
    } //while to end of the unit!
    if (info_ptr > extra_unit->next_unread_die_info_ptr)
    { //save info_ptr for next scans...
        extra_unit->next_unread_die_info_ptr = info_ptr;
    }
    return;  //the sym does not found!!!
fail:
    (*_bfd_error_handler) (_("Dwarf2_extender error, bfd_error_type: %u."), (unsigned int)bfd_get_error());
    return;
}

//Enumerations_read() fills Enumerations
//TODO: find out: Enumerations_read() actual after extsymbols scan! see: scan_unit_for_extra_info()
static void Enumerations_read(void) {
    struct extra_comp_unit* extra_unit;
    for (extra_unit = extender->extra_units; extra_unit; extra_unit = extra_unit->next) {
        Enumerations_read_from_extunit(extra_unit);
    }
}

int Enumerations_count(void) {
    if (extender->enums_count) return extender->enums_count;
    Enumerations_read();
    int ret = 0;
    enumeration_info* enumeration;
    for (enumeration = extender->enums_chain; enumeration; enumeration = enumeration->next) {
        ret++;
    }
    if (ret) { //not needed: && !extender->enums_table
        size_t sz = sizeof (void*) * ret;
        void** result = (void**)bfd_zalloc(extender->abfd, sz);
        if (result) {
            extender->enums_table = (enumeration_info**)result;
            for (enumeration = extender->enums_chain; enumeration; enumeration = enumeration->next) {
                *result++ = enumeration;
            }
        }
    }
    return extender->enums_count = ret;
}

void** Enumeration_table(void) {
    if (extender->enums_table || Enumerations_count()) return (void**)extender->enums_table;
    return NULL;
}

void *Enumeration_first(void)
{
    if (extender->enums_table || Enumerations_count()) return *extender->enums_table;
    return NULL;
}

void *Enumeration_next(void *enumptr)
{
    if (!enumptr) return NULL;
    return ((enumeration_info*)enumptr)->next;
}

const char *Enumeration_name(void *enumptr)
{
    if (!enumptr) return NULL;
    return ((enumeration_info*)enumptr)->name;
}

int Enumeration_sizeb(void *enumptr)
{
    if (!enumptr) return -1;
    enumeration_info* enm = (enumeration_info*)enumptr;
    if (enm->sizeb) return enm->sizeb;
    int result = (int)(long)die_attr_read(&enm->dinfo, DW_AT_byte_size);
    return enm->sizeb = result;
}

///enumerators... ///////////////////////////////////////////////////////
/// \brief enumerators_scan
/// \param enumeration
/// \param scanname if NULL then returns next unread enumerator!!!
/// \return
///
void *Enumerators_scan_for_name(void* enumeration_ptr, const char* scanname) {
    if (!enumeration_ptr) return NULL;
    enumeration_info* enumeration = (enumeration_info*)enumeration_ptr;
    enumerator_info* enumr;
    if (scanname) {
        for (enumr = enumeration->enumerator_first; enumr; enumr = enumr->next) {
            if (strcmp(enumr->name, scanname) == 0) {
                return enumr;
            }
        }
    }
    struct comp_unit* unit = enumeration->dinfo.dwarf2cu;
    bfd_byte *info_ptr = iif(enumeration->enumerator_last, enumeration->enumerator_last->dinfo.next_die_info_ptr, enumeration->dinfo.next_die_info_ptr);
    while (info_ptr)
    {
        struct abbrev_info *abbrev;
        enum die_read_codes rcode = die_pointers_read(unit, info_ptr);
        if (rcode != die_read_code_ok) {
            if (rcode == die_read_code_zero_abbrev)
            {
                enumeration->flags.isReadDone = 1;
            }
            else if (rcode == die_read_code_error) {
                enumeration->flags.isError = 1;
            }
            break;
        }
        abbrev = static_buffer_dieinfo.dinfo.abbrev; //static_buffer_dieinfo created by die_pointers_read()
        if (abbrev->tag == DW_TAG_enumerator)
        {
            const char* name = (const char*)die_attr_read(&static_buffer_dieinfo.dinfo, DW_AT_name);
            if (name && *name != '\x0' && *name != '_')
            { //name is ok...
                enumr = (enumerator_info*)create_dieinfo_entity(unit->abfd, &static_buffer_dieinfo.dinfo, sizeof (enumerator_info));
                if (!enumr) break;
                enumr->name = name;
                enumr->owner = enumeration;
                if (enumeration->enumerator_first) {
                    enumeration->enumerator_last->next = enumr;
                }
                else {
                    enumeration->enumerator_first = enumr;
                }
                enumeration->enumerator_last  = enumr;
                if (!scanname || strcmp(name, scanname) == 0) {
                    return enumr;
                }
            }
        }
        else
        {
            enumeration->flags.isReadDone = 1;
            break;
        }
        info_ptr = static_buffer_dieinfo.dinfo.next_die_info_ptr;
    } //while
    return NULL;
}

void *Enumerator_first(void *enumeration_ptr)
{
    if (!enumeration_ptr) return NULL;
    enumeration_info* ention = (enumeration_info*)enumeration_ptr;
    if (ention->enumerator_first) return ention->enumerator_first;
    return Enumerators_scan_for_name(ention, NULL);
}

void *Enumerator_next(void *enumerator_ptr)
{
    if (!enumerator_ptr) return NULL;
    enumerator_info* etor = (enumerator_info*)enumerator_ptr;
    if (etor->next) return etor->next;
    if (etor->owner->flags.isReadDone) return NULL;
    return Enumerators_scan_for_name(etor->owner, NULL);
}

const char *Enumerator_name(void *enumerator_ptr)
{
    if (!enumerator_ptr) return NULL;
    return ((enumerator_info*)enumerator_ptr)->name;
}

long Enumerator_value(void *enumerator_ptr)
{
    if (!enumerator_ptr) return 0;
    enumerator_info* e = (enumerator_info*)enumerator_ptr;
    if (e->flags.isReadDone) return e->value;
    long result = (long)die_attr_read(&e->dinfo, DW_AT_const_value);
    e->flags.isReadDone = 1;
    return e->value = result;
}

void* dwarf2_extender_init(bfd *file)
{
    extender = (struct dwarf2_extender*)bfd_zalloc(file, sizeof (struct dwarf2_extender));
    if (extender) {
        extender->abfd = file;
    }
    return extender;
}

const char *Enumeration_sourceFileName(void *enumptr)
{
    if (!enumptr) return NULL;
    return ((enumeration_info*)enumptr)->dinfo.dwarf2cu->name;
}

