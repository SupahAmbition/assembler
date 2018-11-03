COMPFLAGS = -g -annot
OCC = ocamlfind ocamlc $(COMPFLAGS)
LIBS = str.cma unix.cma 
PKGS = -package getopt



all : assembler 

assembler : assembler.cmo
	$(OCC) $(LIBS) -o assembler -linkpkg $(PKGS) assembler.cmo


assembler.cmo : assembler.ml
	$(OCC) -c $(PKGS) assembler.ml


clean : 
	rm -f *.cmo *.cmi *.mlt *.annot
	rm -f ./assembler
