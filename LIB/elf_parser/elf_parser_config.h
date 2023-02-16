#ifndef _ELF_PARSER_CONFIG_H
#define _ELF_PARSER_CONFIG_H

#define DEFAULT_VECTOR bfd_elf32_littlearm_vec
#define SELECT_VECS &bfd_elf32_littlearm_vec,&bfd_elf32_bigarm_vec,&bfd_elf32_little_generic_vec,&bfd_elf32_big_generic_vec,&plugin_vec

#define DEBUGDIR NULL
#define bin_dummy_emulation bin_vanilla_emulation
#define SELECT_ARCHITECTURES &bfd_arm_arch,&bfd_plugin_arch
#define OBJDUMP_PRIVATE_VECTORS
#define BINDIR ""

//#define UNSIGNED_64BIT_TYPE /*by zvv, only for eliminate unused warning for:htab_mod_1() in hashtab.c */

//from config.log...
//#define PACKAGE_NAME "bfd"
//#define PACKAGE_TARNAME "bfd"
//#define PACKAGE_VERSION "2.24.0"
//#define PACKAGE_STRING "bfd 2.24.0"
//#define PACKAGE_BUGREPORT ""
//#define PACKAGE_URL ""
//#define PACKAGE "bfd"
//#define VERSION "2.24.0"
//#define STDC_HEADERS 1
//#define HAVE_SYS_TYPES_H 1
//#define HAVE_SYS_STAT_H 1
//#define HAVE_STDLIB_H 1
//#define HAVE_STRING_H 1
//#define HAVE_MEMORY_H 1
//#define HAVE_STRINGS_H 1
//#define HAVE_INTTYPES_H 1
//#define HAVE_STDINT_H 1
//#define HAVE_UNISTD_H 1
//#define __EXTENSIONS__ 1
//#define _ALL_SOURCE 1
//#define _GNU_SOURCE 1
//#define _POSIX_PTHREAD_SEMANTICS 1
//#define _TANDEM_SOURCE 1
//#define HAVE_DLFCN_H 1
//#define LT_OBJDIR ".libs/"
//#define SIZEOF_LONG_LONG 8
//#define SIZEOF_VOID_P 8
//#define SIZEOF_LONG 8
//#define HAVE_ALLOCA_H 1
//#define HAVE_STDDEF_H 1
//#define HAVE_STRING_H 1
//#define HAVE_STRINGS_H 1
//#define HAVE_STDLIB_H 1
//#define HAVE_TIME_H 1
//#define HAVE_UNISTD_H 1
//#define HAVE_FCNTL_H 1
//#define HAVE_SYS_FILE_H 1
//#define HAVE_SYS_TIME_H 1
//#define HAVE_SYS_STAT_H 1
//#define HAVE_SYS_RESOURCE_H 1
//#define TIME_WITH_SYS_TIME 1
//#define HAVE_DIRENT_H 1
//#define HAVE_DLFCN_H 1
//#define STRING_WITH_STRINGS 1
//#define HAVE_FCNTL 1
//#define HAVE_GETPAGESIZE 1
//#define HAVE_SETITIMER 1
//#define HAVE_SYSCONF 1
//#define HAVE_FDOPEN 1
//#define HAVE_GETUID 1
//#define HAVE_GETGID 1
//#define HAVE_FILENO 1
//#define HAVE_STRTOULL 1
//#define HAVE_GETRLIMIT 1
//#define HAVE_DECL_BASENAME 1
//#define HAVE_DECL_FTELLO 1
//#define HAVE_DECL_FTELLO64 1
//#define HAVE_DECL_FSEEKO 1
//#define HAVE_DECL_FSEEKO64 1
//#define HAVE_DECL_FFS 1
//#define HAVE_DECL_FREE 1
//#define HAVE_DECL_GETENV 1
//#define HAVE_DECL_MALLOC 1
//#define HAVE_DECL_REALLOC 1
//#define HAVE_DECL_STPCPY 1
//#define HAVE_DECL_STRSTR 1
//#define HAVE_DECL_SNPRINTF 1
//#define HAVE_DECL_VSNPRINTF 1
//#define HAVE_ZLIB_H 1
//#define HAVE_FTELLO 1
//#define HAVE_FTELLO64 1
//#define HAVE_FSEEKO 1
//#define HAVE_FSEEKO64 1
//#define HAVE_FOPEN64 1
//#define SIZEOF_OFF_T 8
//#define HAVE_STDLIB_H 1
//#define HAVE_UNISTD_H 1
//#define HAVE_GETPAGESIZE 1
//#define HAVE_MMAP 1
//#define HAVE_MADVISE 1
//#define HAVE_MPROTECT 1
//end from config.log


/* Compress sections in this BFD with SHF_COMPRESSED from gABI.  */
//zvv: added with v52:
//#define BFD_COMPRESS_GABI 0x20000

#endif // _ELF_PARSER_CONFIG_H

