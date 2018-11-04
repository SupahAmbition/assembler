COMPFLAGS = -g -annot
OCC = ocamlfind ocamlc $(COMPFLAGS)
LIBS = str.cma unix.cma 
PKGS = -package getopt



all : assembler 

assembler : assembler.cmo util.cmo
	$(OCC) $(LIBS) -o assembler -linkpkg $(PKGS) util.cmo assembler.cmo 


assembler.cmo : assembler.ml 
	$(OCC) -c $(PKGS) util.ml assembler.ml

util.cmo: util.ml 
	$(OCC) -c $(PKGS) util.ml


clean : 
	rm -f *.cmo *.cmi *.mlt *.annot
	rm -f ./assembler
