OBJ = main.o readFile.o tablesOperations.o #printTables.o


minicron: $(OBJ)
	cc -o minicron $(OBJ)


.PHONY: clean
clean:
	rm -f minicron $(OBJ)

