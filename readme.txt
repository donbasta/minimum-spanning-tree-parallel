Algoritma pencarian MST yang digunakan oleh kelompok kami adalah algoritma Kruskal.


Algoritma MST OpenMP

Paralelisasi dilakukan pada operasi sorting edge. 
Sorting dilakukan dua kali, yakni saat sorting edge by weight, dan saat sorting edge secara leksikografis (untuk mencetak edge-edge pada MST).
Operasi sorting yang digunakan adalah merge sort. 
Paralelisasi dilakukan pada operasi merge sort ini, menggunakan section dan nested parallelization.
Dalam operasi merge sort sebuah array, terdapat dua proses yang independen, yakni saat melakukan merge sort secara rekursif pada bagian setengah pertama (kiri) dan bagian setengah kedua (kanan) array tersebut.
Kedua operasi ini diparalelisasi dengan membuat section (#pragma omp parallel sections), dan kedua section (yaitu merge sort bagian kiri dan merge sort bagian kanan)
masing-masing menjadi sebuah section dan dilakukan secara paralel. Apabila kedua operasi telah selesai dilakukan, array bagian kiri dan bagian kanan yang masing-masing telah terurut tersebut
disatukan.
