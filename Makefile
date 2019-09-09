build: libso_stdio.so

libso_stdio.so: so_fopen_fclose.o so_read_write.o so_fseek_ftell_fflush.o so_popen_pclose.o
	gcc -Wall -g -shared so_fopen_fclose.o so_read_write.o so_fseek_ftell_fflush.o so_popen_pclose.o -o libso_stdio.so

so_fopen_fclose.o: so_fopen_fclose.c
	gcc -Wall -g -fPIC -c so_fopen_fclose.c

so_read_write.o: so_read_write.c
	gcc -Wall -g -fPIC -c so_read_write.c

so_fseek_ftell_fflush.o: so_fseek_ftell_fflush.c
	gcc -Wall -g -fPIC -c so_fseek_ftell_fflush.c

so_popen_pclose.o: so_popen_pclose.c
	gcc -Wall -g -fPIC -c so_popen_pclose.c

clean:
	rm so_*.o 
	rm libso_stdio.so