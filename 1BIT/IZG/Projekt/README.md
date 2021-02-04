# Zadání projektu do předmětu IZG 2017 - 2018.
Vašim úkolem je naimplementovat softwarový vykreslovací řetězec (pipeline). Pomocí vykreslovacího řetězce vizualizovat model králička s phongovým osvělovacím modelem a phongovým stínováním. V tomto projektu nebudeme pracovat s GPU, ale budeme se snažit simulovat její práci. Cílem je pochopit jak vykreslovací řetěc funguje, z čeho je složený a jaká data se v něm pohybují.

Váš úkol je složen ze tří částí: napsat kódy pro CPU stranu, pro virtuální GPU stranu a napsat shadery. Musíte doplnit implementace několika funkcí a rozchodit kreslení modelu králička. Funkce mají pevně daný interface (Vstupy a výstupy). Seznam všech úkolů naleznete zde **todo.html**. Úkoly týkající se pouze CPU strany naleznete zde **CPU**. Úkoly týkající se pouze GPU strany naleznete zde **GPU**. Úkoly týkající se pouze shaderů naleznete zde **Shadery**.

Každý úkol má přiřazen akceptační test, takže si můžete snadno ověřit funkčnosti vaší implementace.

V projektu je přítomen i příklad vykreslení jednoho trojúhelníku: v **Triangle Example**. Tento příklad můžete využít pro inspiraci a návod jak napsat cpu stranu a shadery.

Pro implementace třetí části - gpu části využijte teorii na této stránce, doxygen dokumentaci a teorii probíranou na přednáškách.

# Rozdělení
Projekt je rozdělen do několika podsložek:

**student/** Tato složka obsahuje soubory, které využijete při implementaci projektu. Složka obsahuje soubory, které budete odevzávat a podpůrné knihovny. Všechny soubory v této složce jsou napsány v jazyce C abyste se mohli podívat jak jednotlivé části fungují.

**tests/** Tato složka obsahuje akceptační a performanční testy projektu. Akceptační testy jsou napsány s využitím knihovny catch. Testy jsou rozděleny do testovacích případů (TEST_CASE). Daný TEST_CASE testuje jednu podčást projektu.

**examples/** Tato složka obsahuje kompletní příklad kreslení jednoho trojúhelníku pomocí virtuální gpu API.

**doc/** Tato složka obsahuje doxygen dokumentaci projektu. Můžete ji přegenerovat pomocí příkazu doxygen spuštěného v root adresáři projektu.

**3rdParty/** Tato složka obsahuje hlavičkový soubor pro unit testy - catch.hpp. Z pohledu projektu je nezajímavá. Catch je knihovna složená pouze z hlavičkového souboru napsaného v jazyce C++. Poskytuje několik užitečných maker pro svoji obsluhu. TEST_CASE - testovací případ (například pro testování jedné funkce). WHEN - toto makro popisuje způsob použití (například volání funkce s parametery nastavenými na krajní hodnoty). REQUIRE - toto makro vyhodnotí podmínku a případně vypíše chybu (například chcete ověřit, že vaše funkce vrátila správnou hodnotu).

**CMakeModules/** Tato složka obsahuje skripty pro CMake. Z pohledu projektu je nezajímavá.

**gpu/** Tato složka obsahuje implementace virtuální GPU (hlavně management paměti). Z pohledu projektu je nezajímavá. Virtuální GPU je napsáno v jazyce C++.

**images/** Tato složka obsahuje doprovodné obrázky pro dokumentaci v doxygenu. Z pohledu projektu je nezajímavá.

Složka student/ obsahuje soubory, které se vás přímo týkají:

`student_cpu.c` obsahuje cpu stranu vykreslování - tento soubor budete editovat.

`student_pipeline.c` obsahuje zobrazovací řetězec - tento soubor budete editovat.

`student_shader.c` obsahuje shadery - tento soubor budete editovat.

`buffer.h` slouží pro práci s buffery.

`program.h` slouží pro práci s shader programy.

`vertexPuller.h` slouží pro práci s vertex puller objekty.

`uniforms.h` slouží pro práci s uniformními proměnnými.

`linearAlgebra.h` slouží pro vektorové a maticové operace.

`bunny.h` obsahuje model králíčka.

`gpu.h` obsahuje interface pro virtuální GPU (gpu paměť).

Soubory, které se týkají projektu, ale ne přímo studentské práce:

`main.c` obsahuje main funkci.

`camera.h` obsahuje funkce pro výpočet orbit manipulátoru kamery.

`mouseCamera.h` obsahuje manipulaci kamery pomoci myši.

`swapBuffers.h` obsahuje funkci pro přepnutí vykreslovacích snímků.

`fwd.h` obsahuje forward deklarace.

Funkce s předponou cpu_ můžou být volány pouze na straně CPU, v souboru `student_cpu.c`. Funkce s předponou gpu_ můžou být volány pouze na straně GPU, v souboru `student_pipeline.c`. Funkce s předponou shader_ můžou být volány pouze v rámci shaderů (vertex i fragment), v souboru `student_shader.c`. Funkce s předponou vs_ můžou být volány pouze v rámci vertex shaderu, v souboru `student_shader.c`. Funkce s předponou fs_ můžou být volány pouze v rámci fragment shaderu v souboru `student_shader.c`. Funkce bez předpony můžou být volány na CPU, GPU tak v rámci shaderu.

Struktury, které se vyskytují pouze na GPU straně jsou uvozeny prefixem GPU. Struktury bez předpony lze využít jak na CPU tak GPU straně či v shaderu.

Projekt je postaven nad filozofií OpenGL. Spousta funkcí má podobné jméno.
