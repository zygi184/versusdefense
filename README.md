# Versus Defense - Dokumentacja Projektu (Godot 4 & C++)



Projekt Versus Defense to gra strategiczna z gatunku Tower Defense, zrealizowana w silniku Godot 4.3 przy użyciu technologii GDExtension oraz natywnego języka C++. Głównym założeniem technicznym projektu było odrzucenie standardowego języka skryptowego silnika na rzecz czystego kodu C++, co zapewnia maksymalną wydajność obliczeniową przy zarządzaniu licznymi obiektami na scenie.



## 1. Architektura Techniczna Projektu



Gra opiera się na architekturze komponentowej dostarczanej przez Godot 4, jednak cała logika logiczno-matematyczna została przeniesiona do warstwy C++. Wszystkie klasy gry dziedziczą po węzłach silnika i są rejestrowane w systemie przy starcie modułu za pośrednictwem pliku register_types.cpp.



### Struktura Klas C++:

* *Spawner*: Główny zarządca logiki gry, systemu fal oraz trybu budowania.

* *Tower*: Logika podstawowej wieży obronnej oraz systemu globalnych ulepszeń.

* *Sniper*: Logika zaawansowanej jednostki dalekosiężnej o wysokich obrażeniach.

* *Enemy*: Komponent sterujący pojedynczym przeciwnikiem, jego punktami życia i prędkością.



## 2. Szczegółowy Opis Mechanik (Jak to działa pod maską)



### System Budowania i Raycasting 3D (Klasa Spawner)

Wybór wieży w interfejsie użytkownika aktywuje odpowiedni stan budowy w klasie Spawner (is_building_mode lub is_building_sniper_mode). System blokuje jednoczesne włączenie obu trybów.

Gdy gracz kliknie lewy przycisk myszy, pozycja kursora 2D z rzutni (Viewport) jest zamieniana przez kamerę 3D na promień fizyczny w przestrzeni trójwymiarowej (project_ray_origin i project_ray_normal). Silnik za pomocą funkcji intersect_ray sprawdza kolizję. Jeśli trafiony obiekt należy do grupy "ziemia", system pobiera odpowiednią kwotę złota (200 za wieżę podstawową, 400 za snajpera), tworzy instancję obiektu z pliku .tscn i umieszcza go dokładnie w punkcie uderzenia promienia.



### Skalowanie Trudności i System Fal (Klasa Spawner)

Spawner kontroluje napływ wrogów za pomocą wewnętrznych stoperów czasu (delta). Fale przedzielone są bezpiecznym czasem na budowę wynoszącym 5 sekund.

Poziom trudności jest dynamiczny. Każda kolejna fala zwiększa bazowe punkty życia nowo tworzonych wrogów. Dodatkowo gra wspiera dwa poziomy trudności wybierane w lobby: normalny oraz trudny (zmieniający mnożnik trudności na wartość 2, co podwaja punkty życia przeciwników). Od fali 10. aktywowany jest mechanizm przyspieszenia, który z każdą kolejną falą zwiększa bazową prędkość wagoników o 0.2 metra na sekundę.



### Inteligentny System Statystyk Śmierci (Klasy Spawner & Enemy)

W celu uniknięcia błędów segmentacji pamięci (próba odczytu danych z obiektu, który został usunięty z kolejki silnika), system zliczania zabójstw opiera się na architekturze statycznej.

Klasa Spawner posiada publiczną zmienną statyczną zabici_wrogowie. Podczas przyjmowania obrażeń w funkcji Enemy::take_damage, wróg sam sprawdza swój stan zdrowia. W momencie gdy jego HP spada do zera lub poniżej, obiekt inkrementuje globalny licznik Spawner::zabici_wrogowie ułamek sekundy przed wywołaniem procedury samoniszczenia queue_free().



### Interfejs Konńca Gry (Game Over)

W każdej klatce obrazu Spawner monitoruje stan etykiety interfejsu odpowiedzialnej za punkty życia bazy. W momencie gdy wartość spadnie do zera, Spawner wstrzymuje dalszą produkcję jednostek, pobiera z kontenera GUI węzeł GameOverPanel i modyfikuje jego wewnętrzny komponent tekstowy EndLabel. Za pomocą formatowania nowej linii (\n) tworzy dynamiczne podsumowanie bitwy wyświetlające przetrwane fale, zbudowane wieże i zabitych wrogów.



### Precyzyjne Renderowanie Wiązki Lasera (Klasa Sniper i Tower)

System ataku wieży snajperskiej wykorzystuje zaawansowane operacje na macierzach transformacji, aby dynamicznie generować wiązkę lasera pomiędzy lufą a poruszającym się celem.

Proces ten wymaga ścisłego rygoru kolejności operacji matematycznych w C++:

1. Przeniesienie lasera w punkt centralny pomiędzy wieżą a wrogiem (lerp).

2. Normalizacja skali lasera do wartości domyślnych (1, 1, 1).

3. Obrócenie lasera w stronę przeciwnika (look_at).

4. Wykonanie lokalnego obrotu o 90 stopni, aby dopasować pionowy cylinder do osi strzału.

5. Ostateczne nałożenie odpowiedniej skali szerokości oraz wyliczonego dystansu jako długości.



## 4. Instrukcja Kompilacji Projektu



Projekt korzysta z systemu budowania SCons. Aby skompilować najnowszą wersję kodu C++ i wdrożyć ją do silnika:

1. Otwórz dedykowany terminal deweloperski (np. x64 Native Tools Command Prompt dla VS 2022).

2. Przejdź do głównego folderu projektu (tam gdzie znajduje się plik SConstruct).

3. Wpisz komendę scons i zatwierdź klawiszem Enter.

4. Po pomyślnym zakończeniu kompilacji uruchom edytor Godot 4, aby przetestować zmiany.
