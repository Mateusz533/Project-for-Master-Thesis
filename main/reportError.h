#pragma once

/*Funkcja globalna przekazująca komunikat o błędzie o zadanym numerze,
  umożliwiając reakcję systemu na błąd w dowolnym miejscu kodu*/
void reportError(const String& code);

/*Opis kodów błędów:
  0 - przepełnienie licznika
  1 - błąd synchronizacji zadań
  2 - błąd czujnika nacisku
  3 - błąd wzmacniacza
  4 - błąd czujnika temperatury
  5 - błąd grzałki
  ? - nieznany błąd oprogramowania
*/