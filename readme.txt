Algoritma pencarian MST yang digunakan oleh kelompok kami adalah algoritma Kruskal.

Algoritma MST OpenMPI

Paralelisasi dengan OpenMPI dilakukan pada saat melakukan sorting. Sorting ini dilakukan 2 kali, 
yaitu saat sorting edge berdasarkan weight dan pada saat sorting edge secara leksikografis.

Proses dengan rank 0 (Master) bertindak untuk menerima input memberi output dan juga mengatur alur 
utama program. Sementara proses dengan rank > 0 menunggu request untuk melakukan operasi merge sort.

Untuk setiap pemanggilan fungsi merge sort, 

apabila panjang array = 1, maka operasi tersebut telah selesai.

apabila panjang array > 1, maka akan dilakukan pemanggilan 2 fungsi merge sort yaitu untuk array bagian
kiri dan array bagian kanan. Array bagian kiri dan kanan memiliki panjang setengah kali array semula.

Pada pemanggilan merge sort untuk array bagian kiri, pemrosesan diSerahkan ke proses yang sama. 

Sementara untuk pemanggilan merge sort untuk array bagian kanan, apabila terdapat proses dengan rank 
lebih tinggi yang belum memiliki pekerjaan, pemrosesan ini akan diberikan kepada proses yang lebih tinggi.
Apabila tidak ada, maka proses akan dikerjakan pada proses tersebut.

Ilustrasi pengerjaan merge sort dengan 4 proses (angka melambangkan rank dari proses):

                    0
            0               2
        0       1       2       3  

Setelah masing-masing proses pada level terbawah, maka hasil akan dikirimkan ke parent yg memanggil proses tersebut:

        0       1       2       3  
            0               2
                    0


Algoritma MST OpenMP

Paralelisasi dilakukan pada operasi sorting edge. 
Sorting dilakukan dua kali, yakni saat sorting edge by weight, dan saat sorting edge secara leksikografis (untuk mencetak edge-edge pada MST).
Operasi sorting yang digunakan adalah merge sort. 
Paralelisasi dilakukan pada operasi merge sort ini, menggunakan section dan nested parallelization.
Dalam operasi merge sort sebuah array, terdapat dua proses yang independen, yakni saat melakukan merge sort secara rekursif pada bagian setengah pertama (kiri) dan bagian setengah kedua (kanan) array tersebut.
Kedua operasi ini diparalelisasi dengan membuat section (#pragma omp parallel sections), dan kedua operasi (yaitu merge sort bagian kiri dan merge sort bagian kanan) masing-masing menjadi sebuah section dan dilakukan secara paralel. Apabila kedua operasi telah selesai dilakukan, array bagian kiri dan bagian kanan yang masing-masing telah terurut tersebut disatukan.
