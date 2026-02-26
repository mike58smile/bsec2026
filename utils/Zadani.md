SOLARCAP SPRINT

# Popis
Zkuste se na jedno dopoledne vžít do role našich vývojářů, kteří denně řeší optimalizační úlohy v systémech s obnovitelnými zdroji a akumulací. Jednou z klíčových výzev je co nejrychleji získat energii z FV zdroje, bezpečně ji uložit do akumulačního prvku a následně s ní co nejefektivněji hospodařit.

Vyzkoušíte si tedy technickou optimalizační úlohu i závod s časem, jak z názvu plyne: V co nejkratším čase nabít superkondenzátor a poté využít energii tak, aby se zařízení dostalo co nejdál od startovní čáry, ideálně až do cíle.

# Cíl
Vaším úkolem je sestrojit pohybující se zařízení (jakékoliv) které:
- Se po startu nejprve nabije ze světelného zdroje prostřednictvím FV panelu na startovní čáře.
- Po nabití se od startovní čáry začne pohybovat směrem k cíli.
- Je sestaveno jen z materiálů poskytnutých organizátory soutěže.
- Bylo vyvíjeno s ohledem na efektivitu využití materiálů, viz sekce Materiály a Stavba.
- Po startu čerpá energii pouze ze superkondenzátoru (dále jen „supercap“) a FV článku, s výjimkou řídicí jednotky, která je napájena ze ==samostatného akumulátoru==.
- Měří napětí na supercapu, ze kterého je zařízení napájeno a podle aktuálního stavu napětí mění pracovní režim zařízení, podle definice níže.
- Probudí se ze sleep módu, po dostatečném dobití energie z FV článku, podle definice níže.

# Popis režimů zařízení
Zařízení musí adaptovat chování mechanismu podle dostupné energie (napětí supercapu):

- TURBO – Vysoké Ucap: Všechno může jet na plný výkon
- ECO – Střední Ucap: Signalizace poklesu napětí
- SURVIVAL – Nízké Ucap: Odpojí všechny nepotřebné systémy

## Povinný stav SHUTDOWN
Nejpozději při poklesu pod Usleep zařízení musí vypnout motor, uložit poslední záznam a přejít do spánku.

## Doporučené prahy napětí:

|Parametr|hodnota|Popis|Poznámka|
|---|---|---|---|
|Ucap_max|5,4 V|Maximální povolené napětí supercapu|2 v sérii|
|---|---|---|---|
|Uturbo|>= 4,5 V|Hranice TURBO → ECO||
|---|---|---|---|
|Ueco|>= 3,0 V|Hranice ECO → SURVIVAL||
|---|---|---|---|
|Usurvival|>= 1.0 V|Hranice SURVIVAL → Sleep||
|---|---|---|---|
|Usleep|< 1.0 V|Odpojení periferií a uspání MCU|Musí zůstat energie na log + sleep.|
|---|---|---|---|
|Uwake|>= 2 V|Probuzení ze spánku|Start pohybu nemusí být hned po probuzení, ale až na určitý povel|
|---|---|---|---|

# Nabíjení ze slunce (MPPT light)
Cílem nabíjecí části je získat z panelu potřebnou energii v co nejkratším čase a bez překročení maximálního napětí supercapu. Při implementaci MPPT může být potřebný čas na nabití zkrácen, nicméně to není podmínkou. Pro nabití z FV panelu může stačit jednoduché řízení, které udržuje napětí panelu UPV poblíž bodu maximálního výkonu Umpp (např. 70–80 % napětí naprázdno UOC).

## Povinné funkce nabíjení:
- Zamezit zpětnému vybíjení supercapu do panelu.
- Omezit nabíjecí proud do supercapu.
- Nepřekročit Ucap_max.
- Měření výkonu proudící přes supercap

Jednoduché řízení podle UPV, příklad logiky: Měřte UPV, upravujte PWM nabíjení tak, aby UPV drželo cílovou hodnotu Upv_target. Při Ucap blízko Ucap_max PWM snižte nabíjecí proud nebo vypněte.

# Materiál a stavba
Stavebním materiálem je Merkur + různý spojovací materiál. Předmětem hodnocení bude i kolik materiálu budete v průběhu soutěže používat. Na začátku soutěže má každý tým 1000 bodů, které může libovolně utrácet (můžete jít i do mínusu). Různé typy součástí budou stát různý počet bodů. Ceny se v průběhu soutěže budou měnit, je tedy žádoucí plánovat dopředu a dobře se v týmu domluvit. Cílem je utratit co nejméně bodů (musí vám zůstat co nejvíce bodů).

# Hodnocení
Primární metrika, ovšem ne jediná, je zda zařízení dojede do cíle, nebo jak moc se mu přiblíží. Hodnotí se:

- Ujetá vzdálenost od startu do cíle
- Energetické řízení
- Optimalizace a řízení
- Nabíjení z FV a probuzení
- Počet zbylých bodů z nakupování materiálu
- **Kreativita**
- Prezentace