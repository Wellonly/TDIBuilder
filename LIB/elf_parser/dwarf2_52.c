/*
//suffix: _52 for functions from binutils v5.2
//after binutils version had changed this functions should be removed and
//referencies have to be fixed to originals!!!
*/

static bfd_uint64_t
read_address_52 (struct comp_unit *unit, bfd_byte *buf, bfd_byte * buf_end)
{
  int signed_vma = 0;

  if (bfd_get_flavour (unit->abfd) == bfd_target_elf_flavour)
    signed_vma = get_elf_backend_data (unit->abfd)->sign_extend_vma;

  if (buf + unit->addr_size > buf_end)
    return 0;

  if (signed_vma)
    {
      switch (unit->addr_size)
    {
    case 8:
      return bfd_get_signed_64 (unit->abfd, buf);
    case 4:
      return bfd_get_signed_32 (unit->abfd, buf);
    case 2:
      return bfd_get_signed_16 (unit->abfd, buf);
    default:
      abort ();
    }
    }
  else
    {
      switch (unit->addr_size)
    {
    case 8:
      return bfd_get_64 (unit->abfd, buf);
    case 4:
      return bfd_get_32 (unit->abfd, buf);
    case 2:
      return bfd_get_16 (unit->abfd, buf);
    default:
      abort ();
    }
    }
}

static unsigned int
read_1_byte_52 (bfd *abfd ATTRIBUTE_UNUSED, bfd_byte *buf, bfd_byte *end)
{
  if (buf + 1 > end)
    return 0;
  return bfd_get_8 (abfd, buf);
}

static unsigned int
read_2_bytes_52 (bfd *abfd, bfd_byte *buf, bfd_byte *end)
{
  if (buf + 2 > end)
    return 0;
  return bfd_get_16 (abfd, buf);
}

static unsigned int
read_4_bytes_52 (bfd *abfd, bfd_byte *buf, bfd_byte *end)
{
  if (buf + 4 > end)
    return 0;
  return bfd_get_32 (abfd, buf);
}

static bfd_uint64_t
read_8_bytes_52 (bfd *abfd, bfd_byte *buf, bfd_byte *end)
{
  if (buf + 8 > end)
    return 0;
  return bfd_get_64 (abfd, buf);
}

static bfd_byte *
read_n_bytes_52 (bfd *abfd ATTRIBUTE_UNUSED,
          bfd_byte *buf,
          bfd_byte *end,
          unsigned int size ATTRIBUTE_UNUSED)
{
  if (buf + size > end)
    return NULL;
  return buf;
}

/* Scans a NUL terminated string starting at BUF, returning a pointer to it.
   Returns the number of characters in the string, *including* the NUL byte,
   in BYTES_READ_PTR.  This value is set even if the function fails.  Bytes
   at or beyond BUF_END will not be read.  Returns NULL if there was a
   problem, or if the string is empty.  */

static char *
read_string_52 (bfd *          abfd ATTRIBUTE_UNUSED,
         bfd_byte *     buf,
         bfd_byte *     buf_end,
         unsigned int * bytes_read_ptr)
{
  bfd_byte *str = buf;

  if (buf >= buf_end)
    {
      * bytes_read_ptr = 0;
      return NULL;
    }

  if (*str == '\0')
    {
      * bytes_read_ptr = 1;
      return NULL;
    }

  while (buf < buf_end)
    if (* buf ++ == 0)
      {
    * bytes_read_ptr = buf - str;
    return (char *) str;
      }

  * bytes_read_ptr = buf - str;
  return NULL;
}

/* Reads an offset from BUF and then locates the string at this offset
   inside the debug string section.  Returns a pointer to the string.
   Returns the number of bytes read from BUF, *not* the length of the string,
   in BYTES_READ_PTR.  This value is set even if the function fails.  Bytes
   at or beyond BUF_END will not be read from BUF.  Returns NULL if there was
   a problem, or if the string is empty.  Does not check for NUL termination
   of the string.  */

static char *
read_indirect_string_52 (struct comp_unit * unit,
              bfd_byte *         buf,
              bfd_byte *         buf_end,
              unsigned int *     bytes_read_ptr)
{
  bfd_uint64_t offset;
  struct dwarf2_debug *stash = unit->stash;
  char *str;

  if (buf + unit->offset_size > buf_end)
    {
      * bytes_read_ptr = 0;
      return NULL;
    }

  if (unit->offset_size == 4)
    offset = read_4_bytes_52 (unit->abfd, buf, buf_end);
  else
    offset = read_8_bytes_52 (unit->abfd, buf, buf_end);

  *bytes_read_ptr = unit->offset_size;

  if (! read_section (unit->abfd, &stash->debug_sections[debug_str],
              stash->syms, offset,
              &stash->dwarf_str_buffer, &stash->dwarf_str_size))
    return NULL;

  if (offset >= stash->dwarf_str_size)
    return NULL;
  str = (char *) stash->dwarf_str_buffer + offset;
  if (*str == '\0')
    return NULL;
  return str;
}

/* Like read_indirect_string but uses a .debug_str located in
   an alternate file pointed to by the .gnu_debugaltlink section.
   Used to impement DW_FORM_GNU_strp_alt.  */

static char *
read_alt_indirect_string_52 (struct comp_unit * unit,
              bfd_byte *         buf,
              bfd_byte *         buf_end,
              unsigned int *     bytes_read_ptr)
{
  bfd_uint64_t offset;
  struct dwarf2_debug *stash = unit->stash;
  char *str;

  if (buf + unit->offset_size > buf_end)
    {
      * bytes_read_ptr = 0;
      return NULL;
    }

  if (unit->offset_size == 4)
    offset = read_4_bytes_52 (unit->abfd, buf, buf_end);
  else
    offset = read_8_bytes_52 (unit->abfd, buf, buf_end);

  *bytes_read_ptr = unit->offset_size;

  if (stash->alt_bfd_ptr == NULL)
    {
      bfd *  debug_bfd;
      char * debug_filename = bfd_follow_gnu_debugaltlink (unit->abfd, DEBUGDIR);

      if (debug_filename == NULL)
    return NULL;

      if ((debug_bfd = bfd_openr (debug_filename, NULL)) == NULL
      || ! bfd_check_format (debug_bfd, bfd_object))
    {
      if (debug_bfd)
        bfd_close (debug_bfd);

      /* FIXME: Should we report our failure to follow the debuglink ?  */
      free (debug_filename);
      return NULL;
    }
      stash->alt_bfd_ptr = debug_bfd;
    }

  if (! read_section (unit->stash->alt_bfd_ptr,
              stash->debug_sections + debug_str_alt,
              NULL, /* FIXME: Do we need to load alternate symbols ?  */
              offset,
              &stash->alt_dwarf_str_buffer,
              &stash->alt_dwarf_str_size))
    return NULL;

  if (offset >= stash->alt_dwarf_str_size)
    return NULL;
  str = (char *) stash->alt_dwarf_str_buffer + offset;
  if (*str == '\0')
    return NULL;

  return str;
}

bfd_vma
safe_read_leb128 (bfd *abfd ATTRIBUTE_UNUSED,
          bfd_byte *data,
          unsigned int *length_return,
          bfd_boolean sign,
          const bfd_byte * const end)
{
  bfd_vma result = 0;
  unsigned int num_read = 0;
  unsigned int shift = 0;
  unsigned char byte = 0;

  while (data < end)
    {
      byte = bfd_get_8 (abfd, data);
      data++;
      num_read++;

      result |= ((bfd_vma) (byte & 0x7f)) << shift;

      shift += 7;
      if ((byte & 0x80) == 0)
    break;
    }

  if (length_return != NULL)
    *length_return = num_read;

  if (sign && (shift < 8 * sizeof (result)) && (byte & 0x40))
    result |= -((bfd_vma) 1 << shift);

  return result;
}

static inline bfd_boolean
is_str_attr (enum dwarf_form form)
{
  return form == DW_FORM_string || form == DW_FORM_strp || form == DW_FORM_GNU_strp_alt;
}

static bfd_byte *
read_attribute_value_52 (struct attribute *  attr,
              unsigned            form,
              struct comp_unit *  unit,
              bfd_byte *          info_ptr,
              bfd_byte *          info_ptr_end)
{
  bfd *abfd = unit->abfd;
  unsigned int bytes_read;
  struct dwarf_block *blk;
  bfd_size_type amt;

  if (info_ptr >= info_ptr_end)
    {
      (*_bfd_error_handler) (_("Dwarf Error: Info pointer extends beyond end of attributes"));
      bfd_set_error (bfd_error_bad_value);
      return info_ptr;
    }

  attr->form = (enum dwarf_form) form;

  switch (form)
    {
    case DW_FORM_ref_addr:
      /* DW_FORM_ref_addr is an address in DWARF2, and an offset in
     DWARF3.  */
      if (unit->version == 3 || unit->version == 4)
    {
      if (unit->offset_size == 4)
        attr->u.val = read_4_bytes_52 (unit->abfd, info_ptr, info_ptr_end);
      else
        attr->u.val = read_8_bytes_52 (unit->abfd, info_ptr, info_ptr_end);
      info_ptr += unit->offset_size;
      break;
    }
      /* FALLTHROUGH */
    case DW_FORM_addr:
      attr->u.val = read_address_52 (unit, info_ptr, info_ptr_end);
      info_ptr += unit->addr_size;
      break;
    case DW_FORM_GNU_ref_alt:
    case DW_FORM_sec_offset:
      if (unit->offset_size == 4)
    attr->u.val = read_4_bytes_52 (unit->abfd, info_ptr, info_ptr_end);
      else
    attr->u.val = read_8_bytes_52 (unit->abfd, info_ptr, info_ptr_end);
      info_ptr += unit->offset_size;
      break;
    case DW_FORM_block2:
      amt = sizeof (struct dwarf_block);
      blk = (struct dwarf_block *) bfd_alloc (abfd, amt);
      if (blk == NULL)
    return NULL;
      blk->size = read_2_bytes_52 (abfd, info_ptr, info_ptr_end);
      info_ptr += 2;
      blk->data = read_n_bytes_52 (abfd, info_ptr, info_ptr_end, blk->size);
      info_ptr += blk->size;
      attr->u.blk = blk;
      break;
    case DW_FORM_block4:
      amt = sizeof (struct dwarf_block);
      blk = (struct dwarf_block *) bfd_alloc (abfd, amt);
      if (blk == NULL)
    return NULL;
      blk->size = read_4_bytes_52 (abfd, info_ptr, info_ptr_end);
      info_ptr += 4;
      blk->data = read_n_bytes_52 (abfd, info_ptr, info_ptr_end, blk->size);
      info_ptr += blk->size;
      attr->u.blk = blk;
      break;
    case DW_FORM_data2:
      attr->u.val = read_2_bytes_52 (abfd, info_ptr, info_ptr_end);
      info_ptr += 2;
      break;
    case DW_FORM_data4:
      attr->u.val = read_4_bytes_52 (abfd, info_ptr, info_ptr_end);
      info_ptr += 4;
      break;
    case DW_FORM_data8:
      attr->u.val = read_8_bytes_52 (abfd, info_ptr, info_ptr_end);
      info_ptr += 8;
      break;
    case DW_FORM_string:
      attr->u.str = read_string_52 (abfd, info_ptr, info_ptr_end, &bytes_read);
      info_ptr += bytes_read;
      break;
    case DW_FORM_strp:
      attr->u.str = read_indirect_string_52 (unit, info_ptr, info_ptr_end, &bytes_read);
      info_ptr += bytes_read;
      break;
    case DW_FORM_GNU_strp_alt:
      attr->u.str = read_alt_indirect_string_52 (unit, info_ptr, info_ptr_end, &bytes_read);
      info_ptr += bytes_read;
      break;
    case DW_FORM_exprloc:
    case DW_FORM_block:
      amt = sizeof (struct dwarf_block);
      blk = (struct dwarf_block *) bfd_alloc (abfd, amt);
      if (blk == NULL)
    return NULL;
      blk->size = safe_read_leb128 (abfd, info_ptr, &bytes_read, FALSE, info_ptr_end);
      info_ptr += bytes_read;
      blk->data = read_n_bytes_52 (abfd, info_ptr, info_ptr_end, blk->size);
      info_ptr += blk->size;
      attr->u.blk = blk;
      break;
    case DW_FORM_block1:
      amt = sizeof (struct dwarf_block);
      blk = (struct dwarf_block *) bfd_alloc (abfd, amt);
      if (blk == NULL)
    return NULL;
      blk->size = read_1_byte_52 (abfd, info_ptr, info_ptr_end);
      info_ptr += 1;
      blk->data = read_n_bytes_52 (abfd, info_ptr, info_ptr_end, blk->size);
      info_ptr += blk->size;
      attr->u.blk = blk;
      break;
    case DW_FORM_data1:
      attr->u.val = read_1_byte_52 (abfd, info_ptr, info_ptr_end);
      info_ptr += 1;
      break;
    case DW_FORM_flag:
      attr->u.val = read_1_byte_52 (abfd, info_ptr, info_ptr_end);
      info_ptr += 1;
      break;
    case DW_FORM_flag_present:
      attr->u.val = 1;
      break;
    case DW_FORM_sdata:
      attr->u.sval = safe_read_leb128 (abfd, info_ptr, &bytes_read, TRUE, info_ptr_end);
      info_ptr += bytes_read;
      break;
    case DW_FORM_udata:
      attr->u.val = safe_read_leb128 (abfd, info_ptr, &bytes_read, FALSE, info_ptr_end);
      info_ptr += bytes_read;
      break;
    case DW_FORM_ref1:
      attr->u.val = read_1_byte_52 (abfd, info_ptr, info_ptr_end);
      info_ptr += 1;
      break;
    case DW_FORM_ref2:
      attr->u.val = read_2_bytes_52 (abfd, info_ptr, info_ptr_end);
      info_ptr += 2;
      break;
    case DW_FORM_ref4:
      attr->u.val = read_4_bytes_52 (abfd, info_ptr, info_ptr_end);
      info_ptr += 4;
      break;
    case DW_FORM_ref8:
      attr->u.val = read_8_bytes_52 (abfd, info_ptr, info_ptr_end);
      info_ptr += 8;
      break;
    case DW_FORM_ref_sig8:
      attr->u.val = read_8_bytes_52 (abfd, info_ptr, info_ptr_end);
      info_ptr += 8;
      break;
    case DW_FORM_ref_udata:
      attr->u.val = safe_read_leb128 (abfd, info_ptr, &bytes_read, FALSE, info_ptr_end);
      info_ptr += bytes_read;
      break;
    case DW_FORM_indirect:
      form = safe_read_leb128 (abfd, info_ptr, &bytes_read, FALSE, info_ptr_end);
      info_ptr += bytes_read;
      info_ptr = read_attribute_value_52 (attr, form, unit, info_ptr, info_ptr_end);
      break;
    default:
      (*_bfd_error_handler) (_("Dwarf Error: Invalid or unhandled FORM value: %#x."),
                 form);
      bfd_set_error (bfd_error_bad_value);
      return NULL;
    }
  return info_ptr;
}

//end _52 suffix functions //////////////////////////////////////////////////////////////////////

