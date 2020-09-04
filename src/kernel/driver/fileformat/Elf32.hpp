/**
 * Elf32.hpp
 *
 * @since 08.02.2011
 * @author Arne Simon => email::[arnsimon@gmx.de]
 */

#ifndef ELF32_HPP_
#define ELF32_HPP_

#include <kernel/Process.hpp>
#include <kernel/Thread.hpp>
#include <lib/File.hpp>

namespace kernel {

  namespace driver {

    namespace fileformat {

      /**
       * An Elf32 file.
       *
       * @since 08.02.2011
       * @author Arne Simon => email::[arnsimon@gmx.de]
       */
      struct Elf32 {
          typedef unsigned short Elf32_Half;
          typedef unsigned int Elf32_Word;
          typedef int Elf32_Sword;
          typedef unsigned int Elf32_Addr;
          typedef unsigned int Elf32_Off;
          typedef unsigned char uchar;

          /**
           * The header of an ELF32 file.
           */
          struct Header {
              static const Elf32_Word EI_NIDENT = 16;

              static const Elf32_Half TYPE_NONE = 0;      ///< No file type
              static const Elf32_Half TYPE_REL = 1;      ///< Relocatable file
              static const Elf32_Half TYPE_EXEC = 2;      ///< Executable file
              static const Elf32_Half TYPE_DYN = 3;      ///< Shared object file
              static const Elf32_Half TYPE_CORE = 4;      ///< Core file
              static const Elf32_Half TYPE_LOPROC = 0xff00;      ///< Processor-specific
              static const Elf32_Half TYPE_HIPROC = 0xffff; ///< Processor-specific

              uchar ident[ EI_NIDENT ];
              Elf32_Half type;
              Elf32_Half machine;
              Elf32_Word version;
              Elf32_Addr entry;
              Elf32_Off phoff;
              Elf32_Off shoff;
              Elf32_Word flags;
              Elf32_Half ehsize;
              Elf32_Half phentsize;
              Elf32_Half phnum;
              Elf32_Half shentsize;
              Elf32_Half shnum;
              Elf32_Half shstrndx;
          }__attribute__((packed));

          struct SectionHeader {
              Elf32_Word name;
              Elf32_Word type;
              Elf32_Word flags;
              Elf32_Addr addr;
              Elf32_Off offset;
              Elf32_Word size;
              Elf32_Word link;
              Elf32_Word info;
              Elf32_Word addralign;
              Elf32_Word entsize;
          };

          struct Symbole {
              Elf32_Word name;
              Elf32_Addr value;
              Elf32_Word size;
              uchar info;
              uchar other;
              Elf32_Half shndx;

          };

          struct ProgrammHeader {
              static const Elf32_Word NULL = 0;
              static const Elf32_Word LOAD = 1;
              static const Elf32_Word DYNAMIC = 2;
              static const Elf32_Word INTERP = 3;
              static const Elf32_Word NOTE = 4;
              static const Elf32_Word SHLIB = 5;
              static const Elf32_Word PHDR = 6;
              static const Elf32_Word LOPROC = 0x70000000;
              static const Elf32_Word HIPROC = 0x7fffffff;

              Elf32_Word type;
              Elf32_Off offset;
              Elf32_Addr vaddr;
              Elf32_Addr paddr;
              Elf32_Word filesz;
              Elf32_Word memsz;
              Elf32_Word flags;
              Elf32_Word align;
          };

          lib::File* _file;
          char* data;
          uint32 length;
          Header* header;
          ProgrammHeader** progammheaders;

          Elf32( lib::File* file );

          void execute();

          virtual ~Elf32();
      };

      class Elf32Process: public Process {
        public:
          Elf32Process( Elf32* Elf );
      };

      class Elf32Thread: public Thread {
        public:
          Elf32Thread( Process* P, Elf32* Elf );

          virtual ~Elf32Thread();
      };

    }

  }

}

#endif /* ELF32_HPP_ */
