Structura principala a proiectului este definita de o structura (stru) care
centralizeaza toate datele relevante despre imagine. Aceasta include: 
formatul imaginii (P1, P2, P3, P4, P5, P6), dimensiunile imaginii (latime si 
inaltime), valoarea maxima a pixelilor (de exemplu, 255 pentru intensitatea 
maxima a culorii), matricea pixelilor (alocata dinamic), informatii despre 
selectia curenta (coordonatele (x1, y1) si (x2, y2)). Aceasta structura 
permite gestionarea operatiilor asupra imaginii. 

Un punct central al proiectului este functia load_image, care implementeaza 
un algoritm robust pentru incarcarea imaginilor. Procesul incepe cu determinarea 
formatului imaginii, analizand antetul fisierului pentru identificarea „magic word”-ului. 
Urmeaza sa se determine daca imaginea este grayscale sau color, respectiv daca datele 
sunt codificate in ASCII sau binar. Algoritmul gestioneaza eficient comentariile 
printr-o functie dedicata, skip_comments, care elimina liniile ce incep cu #. 
Dupa acest pas, dimensiunile imaginii si valoarea maxima a pixelilor sunt citite, 
cu o verificare suplimentara pentru a asigura conformitatea cu limita maxima de 
255. Matricea de pixeli este alocata dinamic, iar pixelii sunt procesati diferit 
in functie de format: fie prin citire individuala, utilizand fscanf pentru ASCII, 
fie in blocuri mari, cu ajutorul functiei fread, in cazul fisierelor binare. In 
caz de erori, algoritmul elibereaza toate resursele si returneaza un mesaj explicit. 

Selectia regiunilor, esentiala pentru operatii localizate, este gestionata prin 
functiile SELECT si SELECT_ALL. Algoritmul functiei SELECT verifica validitatea 
coordonatelor introduse de utilizator. Daca acestea sunt inversate algoritmul 
corecteaza automat coordonatele pentru a asigura o selectie valida. In caz contrar, 
afiseaza mesajul „Invalid set of coordinates” si mentine selectia anterioara. 
Functia SELECT_ALL utilizeaza un algoritm simplu, care reseteaza selectia la 
intreaga imagine. 

Un alt algoritm remarcabil este cel din functia CROP, care taie imaginea la 
dimensiunile regiunii selectate. Acesta determina noile dimensiuni ale imaginii 
si creeaza o matrice noua, in care copiaza doar pixelii din selectia curenta. 
Matricea veche este eliberata din memorie si peste ea este copiata noua matrice.  

Pentru functiile APPLY fiecare filtru utilizeaza o matrice kernel specifica. 
Algoritmul parcurge pixelii din regiunea selectata si aplica un kernel 3x3, 
unde fiecare pixel este recalculat pe baza vecinilor sai si a valorilor din kernel. 
Aceasta metoda este aplicata independent pe fiecare canal de culoare (R, G, B). 
Pentru margini, algoritmul pastreaza pixelii nemodificati, evitand accesarea 
pozitiilor invalide. Dupa procesare, valorile sunt restranse in intervalul [0, 255] 
printr-o functie de limitare, clamp. 

Functia HISTOGRAM calculeaza si afiseaza distributia intensitatilor pixelilor. 
Algoritmul parcurge matricea imaginii alb-negru si construieste un vector de 
frecvente. Aceste valori sunt apoi grupate in intervale egale (bin-uri), iar 
frecventele sunt modificate pentru afisare grafica sub forma de asteriscuri. 
Numarul de bin-uri este verificat pentru a fi o putere a lui 2. 

Functia EQUALIZE folosita pentru imaginile alb-negru, foloseste un algoritm 
bazat pe histograma. Pixelii sunt actualizati folosind formula data. 

Rotatia imaginilor este implementata prin functiile ROTATE_FULL_IMAGE si 
ROTATE. Algoritmul presupune validarea unghiului de rotatie, care trebuie 
sa fie un multiplu de 90, si verificarea daca selectia este patrata. In functie
de unghi (90, 180, 270), coordonatele pixelilor sunt transformate, utilizand 
formule diferite pentru fiecare caz. Pentru 360, imaginea nu se schimba. Pentru 
matricea completa, matricea rezultata inlocuieste matricea originala, iar 
dimensiunile imaginii sunt interschimbate. 

In final, functia SAVE implementeaza un algoritm simplu pentru salvarea imaginilor 
in format ASCII sau binar. In cazul ASCII, pixelii sunt scrisi individual cu fprintf, 
iar pentru formatul binar, intreaga matrice este salvata compact cu fwrite. Mesajele 
de succes sau eroare sunt afisate utilizatorului pentru confirmare. 

Gestionarea memoriei este centralizata in functia free_image, care elibereaza toate
resursele alocate dinamic, asigurand functionarea corecta si evitarea scurgerilor de memorie. 
