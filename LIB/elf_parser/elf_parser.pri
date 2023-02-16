
INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/../binutils/binutils
INCLUDEPATH += $$PWD/../binutils/bfd
INCLUDEPATH += $$PWD/../binutils/include
INCLUDEPATH += $$PWD/zvvconfig
#INCLUDEPATH += $$PWD/zvvconfig/zlib
INCLUDEPATH += $$PWD/../../../libmcu
INCLUDEPATH += $$PWD/../binutils/libiberty
#INCLUDEPATH += $$PWD/../binutils/zlib

DEFINES += HAVE_CONFIG_H
#DEFINES += HAVE_ZLIB_H
#WARNING: DEFINES do not allow define a values!!! so next defines moved to .h file!!!
#DEFINES += DEFAULT_VECTOR=bfd_elf32_littlearm_vec
#DEFINES += SELECT_VECS='&bfd_elf32_littlearm_vec,&bfd_elf32_bigarm_vec,&bfd_elf32_little_generic_vec,&bfd_elf32_big_generic_vec,&plugin_vec'
#DEFINES += SELECT_ARCHITECTURES='&bfd_arm_arch,&bfd_plugin_arch'

#Deprecated,use QT_DEBUG which defined by Qt!!! DEFINES += DEBUG

CONFIG += no_lflags_merge
QMAKE_CFLAGS += -std=c11

#LIBS += $$PWD/lib/libbfd.a
#LIBS += $$PWD/lib/libiberty.a
#LIBS += $$PWD/lib/libz.a
LIBS += -L/usr/lib/x86_64-linux-gnu -ldl

#//TODO: find out what is not requested by the project!!!
SOURCES += \
    $$PWD/elf_parser.c \
    $$PWD/dwarf2_extender.c \ #includes dwarf2.c
    $$PWD/../binutils/bfd/elf.c \
    $$PWD/../binutils/bfd/syms.c \
    $$PWD/../binutils/bfd/compress.c \
    $$PWD/../binutils/bfd/simple.c \
    $$PWD/../binutils/bfd/section.c \
    $$PWD/../binutils/bfd/init.c \
    $$PWD/../binutils/bfd/opncls.c \
    $$PWD/../binutils/bfd/format.c \
    $$PWD/../binutils/bfd/hash.c \
    $$PWD/../binutils/bfd/bfd.c \
    $$PWD/../binutils/bfd/bfdio.c \
    $$PWD/../binutils/bfd/elf-attrs.c \
    $$PWD/../binutils/bfd/archures.c \
    $$PWD/../binutils/bfd/elf-strtab.c \
    $$PWD/../binutils/bfd/elflink.c \
    $$PWD/../binutils/bfd/reloc.c \
    $$PWD/../binutils/bfd/archive.c \
    $$PWD/../binutils/bfd/corefile.c \
    $$PWD/../binutils/bfd/libbfd.c \
    $$PWD/../binutils/bfd/targets.c \
    $$PWD/../binutils/bfd/linker.c \
    $$PWD/../binutils/bfd/cache.c \
    $$PWD/../binutils/bfd/binary.c \
    $$PWD/../binutils/bfd/coffgen.c \
    $$PWD/../binutils/bfd/dwarf1.c \
    $$PWD/../binutils/bfd/merge.c \
    $$PWD/../binutils/bfd/stabs.c \
    $$PWD/../binutils/bfd/elf-eh-frame.c \
    $$PWD/../binutils/bfd/archive64.c \
    $$PWD/../binutils/bfd/cpu-arm.c \
    $$PWD/../binutils/bfd/cpu-plugin.c \
    $$PWD/../binutils/bfd/plugin.c \
    $$PWD/../binutils/bfd/srec.c \
    $$PWD/../binutils/bfd/elf32-arm.c \
    $$PWD/../binutils/bfd/elf-vxworks.c \
    $$PWD/../binutils/bfd/elf-nacl.c \
    $$PWD/../binutils/bfd/tekhex.c \
    $$PWD/../binutils/bfd/ihex.c \
    $$PWD/../binutils/bfd/verilog.c \
    $$PWD/../binutils/bfd/elf32.c \
    $$PWD/../binutils/bfd/elf32-gen.c \
#    $$PWD/../binutils/bfd/ \
    $$PWD/../binutils/libiberty/xmalloc.c \
    $$PWD/../binutils/libiberty/lbasename.c \
    $$PWD/../binutils/libiberty/xstrerror.c \
    $$PWD/../binutils/libiberty/objalloc.c \
    $$PWD/../binutils/libiberty/filename_cmp.c \
    $$PWD/../binutils/libiberty/getpwd.c \
    $$PWD/../binutils/libiberty/lrealpath.c \
    $$PWD/../binutils/libiberty/unlink-if-ordinary.c \
    $$PWD/../binutils/libiberty/hashtab.c \
    $$PWD/../binutils/libiberty/concat.c \
    $$PWD/../binutils/libiberty/make-relative-prefix.c \
    $$PWD/../binutils/libiberty/xexit.c \
    $$PWD/../binutils/libiberty/cplus-dem.c \
    $$PWD/../binutils/libiberty/hex.c \
    $$PWD/../binutils/libiberty/xstrdup.c \
    $$PWD/../binutils/libiberty/make-temp-file.c \
    $$PWD/../binutils/libiberty/safe-ctype.c \
    $$PWD/../binutils/libiberty/cp-demangle.c \
    $$PWD/../binutils/libiberty/dwarfnames.c \
#    $$PWD/../binutils/libiberty/ \
#    $$PWD/../binutils/zlib/inflate.c \
#    $$PWD/../binutils/zlib/compress.c \
#    $$PWD/../binutils/zlib/zutil.c \
#    $$PWD/../binutils/zlib/crc32.c \
#    $$PWD/../binutils/zlib/adler32.c \
#    $$PWD/../binutils/zlib/inftrees.c \
#    $$PWD/../binutils/zlib/inffast.c \
#    $$PWD/../binutils/zlib/deflate.c \
#    $$PWD/../binutils/zlib/trees.c \
#    $$PWD/../binutils/zlib/ \
#    $$PWD/../binutils/zlib/ \
#    $$PWD/../binutils/zlib/ \
#    $$PWD/../binutils/zlib/ \
    $$PWD/../binutils/binutils/elfcomm.c \
    $$PWD/../binutils/binutils/dwarf.c \
    $$PWD/../binutils/binutils/bucomm.c \
    $$PWD/../binutils/binutils/filemode.c \
    $$PWD/../binutils/binutils/version.c \
    $$PWD/../binutils/binutils/debug.c \
    $$PWD/../binutils/binutils/ieee.c

HEADERS += \
    $$PWD/elf_parser_config.h \
    $$PWD/elf_parser.h \
    $$PWD/../binutils/bfd/bfd-in.h \
    $$PWD/../binutils/bfd/elfcode.h \
    $$PWD/dwarf2_extender.h

