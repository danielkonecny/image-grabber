# To Do List

0. Oddělat ptrGrabResult.
1. Ukládání nahrávání jako video -- [tutorial](https://learnopencv.com/read-write-and-display-a-video-using-opencv-cpp-python/). Defaultně ukládat video, ale možnost parametrem si nastavit ukládání obrázků, navíc jako argument parametru možnost definovat kvalitu obrázků (třetí parametr v imwrite) (-p QUALITY).
2. Možnost zvolit si parametrem jaká je output složka (-o).
3. Zapisování do souboru aby bylo threadsafe, možná bude třeba použití více souborů. Vyzkoušet `timestampFile.flush()` -- zapsání bufferů, sel to možná nebude potřeba.
4. Zjištění správného offsetu -- systémový timestamp zjistit při volání software triggeru, uložit si ho jako property a spočítat s ním offset. Kdyby se nějak povedlo to vytáhnout už při inicializaci, bylo by to super, ale šance nejsou velké -- [dokumentace](https://docs.baslerweb.com/timestamp).
5. Zachycení Ctrl+C signálu, kterej zavře soubor a ukončí další věci (ošetření konce souboru .csv).
6. U každé fotky si vytáhnout délku expozice, gain a vyvážení bílé a ukládat do CSV.
7. Možnost nastavit si délku expozice parametrem (-e LENGTH).
8. Omrknout, co se stane, když se odpojí kamera (výjimka, callback), nějak o tom informovat výpisem.
9. Zkusit nějaké čekání na připojení další kamery (třeba každých 5 vteřin kontrolovat připojená zařízení, případně jestli dává kamera nějaký callback na nově připojenou kameru). Následně pak spuštění nahrávání u nově připojené kamery.
