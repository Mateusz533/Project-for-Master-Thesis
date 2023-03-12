#pragma once

/*Funkcja globalna przekazująca komunikat o błędzie o zadanym numerze,
  umożliwiając reakcję systemu na błąd w dowolnym miejscu kodu*/
void reportError(const String& code);
