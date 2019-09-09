=== Organizare si Implementare ===
Functiile specificate din biblioteca <stdio.h> au fost implementate
folosind functii POSIX. Generarea multor intreruperi de sistem prin
intermediul functiilor read(), write() nu este eficienta, mai ales
cand un proces doreste sa execute citirea/scrierea unui singur caracter,
de exemplu. Problema este rezolvata prin intermediul crearii unui buffer:
	-> in momentul in care procesul doreste sa citeasca date dintr-un
	fisier, apelul read() nu va extrage din fisier doar numarul specificat
	de caractere, ci va popula intreg buffer-ul. Astfel, apelurile
	de citire ulterioare vor extrage datele necesare din buffer,
	evitandu-se astfel overhead-ul creat de un apel de sistem.

	-> datele pe care procesul doreste sa le scrie in fisier sunt stocate
	in prima instanta intr-un buffer. In momentul in care buffer-ul este
	plin(in urma mai multor apeluri al unor functii de scriere de catre 
	proces), procesul va apela functia write(), scriind astfel in fisier
	toate datele din buffer.

Bufferul este implementat sub forma unui array static, retinand elemente
de tipul unsigned char, avand marimea implicita de 4096 octeti.

Pentru a stii in orice moment la ce caracter ne aflam in buffer, structura
"SO_FILE" retine in campul "buf_point" pozitia curenta din array. In functie
de operatiile ce se executa pe structura SO_FILE, pozitia buf_pointer-ului
va avansa in array. De exemplu, operatiile fgetc/fputc vor avansa 
buf_pointer-ul cu o pozitie, extragand/adaugand in buffer un caracter.

Functiile fread/fwrite sunt implementate prin apeluri succesive ale
functiilor fgetc/fputc, pentru numarul de caractere specificat.

Functia fseek() este implementata cu ajutorul functiei POSIX lseek().
Bufpointer-ul nu va coincide cu file-pointer-ul, astfel, trebuie
avuta in vedere adunarea/scaderea offset-ului dintre acestia, atunci
cand fseek() este apelat. De asemenea, schimbarea pozitiei in fisier presupune
invalidarea buffer-ului, in cazul in care ultima operatie a fost de citire,
sau scrierea in fisier a tuturor caracterelor din buffer in cazul in care
ultima operatie a fost de scriere.

Functia fopen presupune crearea unui proces copil, crearea unui canal de
comunicatie intre procesul parinte si copil si inlocuirea imaginii proce-
sului copil cu comanda specificata. Canalul de comunicate este implementat
sub forma unui pipe anonim. Unul din procese va scrie la capatul de citire,
iar celalalt la citi de la celalalt capat al pipe-ului(sau invers, in functie
de modul specificat de functie).

Aparitia erorilor va seta campul "err" al structurii SO_FILE pe 1. Functia
"ferror" va intoarce aceasta valoare.

Cand se ajunge la sfarsitul fisierului, campul "eof" va fi setat si el pe 1.
Functia "feof" va intoarce aceasta valoare.

Intregul enunt al temei este implementat. Consider ca implementarea acestei
biblioteci a constituit un pilej bun de a observa ce se intampla de fapt
"in spatele" apelului.

=== Compilare si Rulare ===

Pentru obtinerea bibliotecii dinamice libso_stdio.so se va apela regula
implicita de build din Makefile:
	
	make -f Makefile build

Conditii: 
	-> Este necesar ca toate fisierele din arhvia sa se afle in acelasi
director.

