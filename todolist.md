# To Do List

1. ~~Oddělat ptrGrabResult.~~
2. ~~Ukládání nahrávání jako video - [tutorial](https://learnopencv.
   com/read-write-and-display-a-video-using-opencv-cpp-python/). Defaultně ukládat video, ale možnost parametrem si
   nastavit ukládání obrázků, navíc jako argument parametru možnost definovat kvalitu obrázků (třetí parametr v
   imwrite) (-i QUALITY).~~
3. ~~Možnost zvolit si parametrem jaká je output složka (-o).~~
4. ~~Zapisování do souboru aby bylo threadsafe, možná bude třeba použití více souborů.
   Vyzkoušet `logFile. flush()` - zapsání bufferů, ale to možná nebude potřeba.~~
5. ~~Zjištění správného offsetu - systémový timestamp zjistit při volání software triggeru, uložit si ho jako property a
   spočítat s ním offset. Kdyby se nějak povedlo to vytáhnout už při inicializaci, bylo by to super, ale šance nejsou
   velké - [dokumentace](https://docs.baslerweb.com/timestamp).~~
6. ~~Zachycení Ctrl+C signálu, kterej zavře soubor a ukončí další věci (ošetření konce souboru .csv).~~
7. ~~U každé fotky si vytáhnout délku expozice, gain a vyvážení bílé a ukládat do CSV.~~
8. ~~Možnost nastavit si délku expozice parametrem (-e LENGTH).~~
9. ~~Omrknout, co se stane, když se odpojí kamera (výjimka, callback), nějak o tom informovat výpisem.~~
10. ~~Zkusit nějaké čekání na připojení další kamery (třeba každých 5 vteřin kontrolovat připojená zařízení, případně
    jestli dává kamera nějaký callback na nově připojenou kameru). Následně pak spuštění nahrávání u nově připojené
    kamery.~~
11. ~~Předělat nastavení intervalu na nastavení fps (z -t udělat -f), je to totiž potřeba ve framerate videa.~~
12. ~~Nahradit `sleep_for` za `sleep_until`, aby byl přesný frame rate.~~
13. Přidat do CSV čítač snímků.
14. Lepší výpisy na `stdout` - důležité informace vypsat vždy (počet nalezených kamer, připojení kamery, odpojení 
    kamery, aktivní čekání na odpojenou kameru a jak dlouho se už čeká) a mít u těchto informací i timestamp. Další 
    užitečné info vypsat ve 
    verbose režimu.
15. Z chunku si vzít exposuretime, gain a vyvážení bílé a uložit do CSV.
16. Při nezvolení nahrávacích parametrů je mít nastavené automaticky.
17. ISO formát času v názvu souboru.
18. Zdebugovat chybu při nahrávání videa - pravděpodobně špatné rozlišení, něco s area of interest.
