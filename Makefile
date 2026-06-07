.PHONY: assembler clean_assembler linker clean_linker emulator clean_emulator

assembler:
	$(MAKE) --file=Makefile.assembler assembler

clean_assembler:
	$(MAKE) --file=Makefile.assembler clean_assembler

linker:
	$(MAKE) --file=Makefile.linker linker

clean_linker:
	$(MAKE) --file=Makefile.linker clean

emulator:
	$(MAKE) --file=Makefile.emulator emulator

clean_emulator:
	$(MAKE) --file=Makefile.emulator clean




