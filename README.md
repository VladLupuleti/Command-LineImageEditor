# Command-LineImageEditor
În funcția main, citim comenzi până când trebuie să afișăm un mesaj de eroare, până când am îndeplinit toate sarcinile programului sau întâlnim comanda exit.

Odată ce ajungem la o comandă de tip read, verificăm dacă ceea ce urmează după aceasta respectă formatul cerut de problemă. În caz contrar, afișăm mesajele de eroare corespunzătoare. Reținem câte o matrice pentru fiecare culoare: roșu, verde și albastru.

După ce am citit matricile, citim următoarele comenzi și acționăm asupra matricelor până întâlnim un nou read. Pentru fiecare comandă, am creat funcții specifice care implementează operațiile necesare.

Dacă ajungem la un nou read, ștergem matricile existente și reluăm procesul descris anterior.

Pentru comanda resize, am creat o funcție care returnează o matrice cu dimensiunea nouă. Fiecărui element din matricile inițiale îi corespunde o submatrice de dimensiune 4x4 în matricea nouă.

Pentru comanda grid, am creat o funcție care returnează matricea corespunzătoare comenzii. Calculăm mai întâi, într-o matrice numită media, media aritmetică a pixelilor pentru fiecare culoare. Apoi, pe baza acestei matrice, determinăm valoarea fiecărui nod și o reținem în structura Grid.

Pentru această comandă, am creat și o funcție separată pentru afișare, deoarece în comanda march avem nevoie de structura Grid fără afișare.

Pentru comanda init_contur, calculăm fiecare matrice corespunzătoare celor 16 cazuri posibile. În funcție de numărul de pixeli aprinși sau stinși din matrici, separăm rezolvarea în două cazuri și construim matricile pentru fiecare dintre cele 16 modele (patternuri).

Pentru comanda march, apelăm mai întâi funcția Grid, după care utilizăm funcția init_contur și reținem în matricile noastre valorile returnate de aceasta, la pozițiile destinate.

Pentru comanda write, afișăm pur și simplu matricile corespunzătoare fiecărei culori.

La întâlnirea comenzii exit, afișăm mesajul corespunzător, eliberăm memoria alocată pentru matrici și închidem programul.
