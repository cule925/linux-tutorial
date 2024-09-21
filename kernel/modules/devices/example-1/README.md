# JEDNOSTAVNI ZNAKOVNI UREĐAJ

Primjer modula koji pri inicijalizaciji dinamički alocira veliki broj, stvara klasu uređaja i stvara datoteku uređaja koji pripada toj klasi. Modul ima implementirane operacije *open*, *release*, *read* i *write*. Prilikom čitanja datoteke uređaja s naredbom ```sudo cat /dev/basic_device``` ispisuje se "Hello World" do se prilikom pokušaja pisanja naredbom ```echo 1 | sudo tee /dev/basic_device``` u jezgrinom međuspremniku za zapise upisuje poruka "Sorry, this operation is not supported." što se može vidjeti naredbom ```sudo dmesg```.
