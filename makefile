
obj= AES_Main.o AES_Driver.o AES_Iterator.o AES_Key.o File.o Bit.o

DES:$(obj)
	gcc $(obj) -o AES

AES_Main.o:Sys.h Type.h Err.h AES_Driver.h
AES_Driver.o:Sys.h Type.h Err.h Length.h Bit.h AES_Key.h AES_Iterator.h File.h AES_Driver.h
AES_Iterator.o:Sys.h Type.h Err.h Length.h Table.h AES_Key.h AES_Iterator.h
AES_Key.o:Sys.h Type.h Err.h Length.h Table.h AES_Key.h
File.o:Sys.h Type.h Err.h File.h
Bit.o:Sys.h Type.h Err.h Bit.h

.PHONY:clean
clean:
	-rm $(obj)


