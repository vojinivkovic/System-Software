.PHONY assembler clean_assembler 

assembler:
	$(MAKE) --file=Makefile.assembler assembler

clean_assembler:
	$(MAKE) --file=Makefile.assembler clean_assembler


