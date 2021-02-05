DROP TABLE Sala CASCADE CONSTRAINTS;
DROP TABLE Lekcia CASCADE CONSTRAINTS;
DROP TABLE Kurz CASCADE CONSTRAINTS;
DROP TABLE Zakaznik CASCADE CONSTRAINTS;
DROP TABLE Instruktor CASCADE CONSTRAINTS;
DROP TABLE Registroval_Kurz CASCADE CONSTRAINTS;
DROP TABLE Registroval_Lekciu CASCADE CONSTRAINTS;

DROP SEQUENCE ID_saly_seq;

CREATE TABLE Sala(
  ID_saly INTEGER NOT NULL,
  nazov VARCHAR(20) NOT NULL,
  umiestnenie CHAR(1) NOT NULL,
  maximalna_kapacita INTEGER NOT NULL CHECK(maximalna_kapacita > 0),
  pocet_miest INTEGER CHECK(pocet_miest > 0)
);

ALTER SESSION SET NLS_DATE_FORMAT = 'HH24:MI';

CREATE TABLE Lekcia(
  ID_lekcie NUMBER GENERATED ALWAYS as IDENTITY(START with 1 INCREMENT by 1),
  cena DECIMAL(10,2) NOT NULL CHECK(cena > 0),
  popis VARCHAR(30) NOT NULL,
  cas_zaciatku DATE NOT NULL,
  cas_konca DATE NOT NULL,
  den VARCHAR(3) NOT NULL CHECK(den = 'Po' or den = 'Ut' or den = 'St' or den = 'Št' or den = 'Pi' or den = 'So' or den = 'Ne'),
  maximalna_kapacita INTEGER NOT NULL CHECK(maximalna_kapacita > 0),
  obtiaznost INTEGER NOT NULL CHECK(obtiaznost > 0 AND obtiaznost <= 10),
  Sala_FK INTEGER NOT NULL,
  Kurz_FK INTEGER,
  Instruktor_FK CHAR(11) NOT NULL,
  CONSTRAINT check_Date CHECK(cas_zaciatku < cas_konca)
);

CREATE TABLE Kurz(
  ID_kurzu NUMBER GENERATED ALWAYS as IDENTITY(START with 1 INCREMENT by 1),
  dlzka_trvania INTEGER NOT NULL CHECK(dlzka_trvania > 0),
  obtiaznost INTEGER NOT NULL CHECK(obtiaznost > 0 AND obtiaznost <= 10),
  popis VARCHAR(30) NOT NULL,
  cena DECIMAL(10,2) NOT NULL CHECK(cena > 0),
  Instruktor_FK CHAR(11) NOT NULL
);

CREATE TABLE Zakaznik(
  rodne_cislo CHAR(11) NOT NULL,
  meno VARCHAR(20) NOT NULL,
  priezvisko VARCHAR(20) NOT NULL,
  e_mail VARCHAR(30),
  telefonne_cislo VARCHAR(13),
  ulica VARCHAR(20) NOT NULL,
  cislo_popisne INTEGER NOT NULL CHECK(cislo_popisne > 0),
  mesto VARCHAR(20) NOT NULL,
  PSC INTEGER NOT NULL,
  cislo_karty INTEGER NOT NULL,
  stav_clenstva VARCHAR(9) NOT NULL CHECK(stav_clenstva = 'aktivne' OR stav_clenstva = 'neaktivne'),
  CONSTRAINT mail_check_Zakaznik CHECK(REGEXP_LIKE(e_mail, '^(\S+)\@(\S+)\.(\S+)$')),
  CONSTRAINT t_cislo_check_Zakaznik CHECK(REGEXP_LIKE(telefonne_cislo, '^(\+\d{12}|\d{10})$'))
);

CREATE TABLE Instruktor(
  rodne_cislo CHAR(11) NOT NULL,
  meno VARCHAR(20) NOT NULL,
  priezvisko VARCHAR(20) NOT NULL,
  e_mail VARCHAR(30),
  telefonne_cislo VARCHAR(13),
  ulica VARCHAR(20) NOT NULL,
  cislo_popisne INTEGER NOT NULL,
  mesto VARCHAR(20) NOT NULL,
  PSC INTEGER NOT NULL,
  CONSTRAINT mail_check_Instruktor CHECK(REGEXP_LIKE(e_mail, '^(\S+)\@(\S+)\.(\S+)$')),
  CONSTRAINT t_cislo_check_Instrutktor CHECK(REGEXP_LIKE(telefonne_cislo, '^(\+\d{12}|\d{10})$'))
);

ALTER TABLE Sala ADD CONSTRAINT Sala_PK PRIMARY KEY (ID_saly); 
ALTER TABLE Lekcia ADD CONSTRAINT Lekcia_PK PRIMARY KEY (ID_lekcie); 
ALTER TABLE Kurz ADD CONSTRAINT Kurz_PK PRIMARY KEY (ID_kurzu); 
ALTER TABLE Zakaznik ADD CONSTRAINT Zakaznik_PK PRIMARY KEY (rodne_cislo); 
ALTER TABLE Instruktor ADD CONSTRAINT Instruktor_PK PRIMARY KEY (rodne_cislo); 

ALTER TABLE Lekcia ADD CONSTRAINT Lekcia_sala_FK FOREIGN KEY (Sala_FK) REFERENCES Sala(ID_saly) ON DELETE CASCADE;
ALTER TABLE Lekcia ADD CONSTRAINT Kurz_lekcia_FK FOREIGN KEY(Kurz_FK) REFERENCES Kurz(ID_kurzu) ON DELETE CASCADE;
ALTER TABLE Kurz ADD CONSTRAINT Instruktor_kurz_FK FOREIGN KEY (Instruktor_FK) REFERENCES Instruktor(rodne_cislo) ON DELETE CASCADE;
ALTER TABLE Lekcia ADD CONSTRAINT Instruktor_lekcia_FK FOREIGN KEY (Instruktor_FK) REFERENCES Instruktor(rodne_cislo) ON DELETE CASCADE;

CREATE TABLE Registroval_Kurz(
  rodne_cislo_ZakaznikaK CHAR(11) NOT NULL,
  ID_kurzu INTEGER NOT NULL
);

CREATE TABLE Registroval_Lekciu(
  rodne_cislo_ZakaznikaL CHAR(11) NOT NULL,
  ID_lekcie INTEGER NOT NULL
);

ALTER TABLE Registroval_Kurz ADD CONSTRAINT Registroval_Kurz_PK PRIMARY KEY (rodne_cislo_ZakaznikaK, ID_kurzu); 
ALTER TABLE Registroval_Lekciu ADD CONSTRAINT Registroval_Lekciu_PK PRIMARY KEY (rodne_cislo_ZakaznikaL, ID_lekcie); 

-- Trigger na automaticke generovanie hodnot primarnych klucov
CREATE SEQUENCE ID_saly_seq START WITH 1 INCREMENT BY 1;
CREATE OR REPLACE TRIGGER ID_saly_trig
    BEFORE INSERT ON Sala
    FOR EACH ROW
BEGIN
    SELECT ID_saly_seq.NEXTVAL
    INTO : NEW.ID_saly
    FROM dual;
END;
/

INSERT INTO Sala(nazov, umiestnenie, maximalna_kapacita) VALUES('posilovna', 'E', 30);
INSERT INTO Sala(nazov, umiestnenie, maximalna_kapacita) VALUES('plavaren', 'A', 15);
INSERT INTO Sala(nazov, umiestnenie, maximalna_kapacita) VALUES('sauna', 'B', 8);
INSERT INTO Sala(nazov, umiestnenie, maximalna_kapacita) VALUES('kardio', 'E', 25);
INSERT INTO Sala(nazov, umiestnenie, maximalna_kapacita) VALUES('boxerna', 'B', 6);

-- Ukazka funkcnosti triggeru na automaticke generovanie hodnot primarneho kluca
SELECT *FROM sala;

INSERT INTO Instruktor VALUES('985327/5344', 'David', 'Janku', 'davidjanku2@pokec.cz', '+420164985925', 'Jiraskova', 58, 'Brno', '94201');
INSERT INTO Instruktor VALUES('961221/3985', 'Marek', 'Kovac', 'optimus@seznam.cz', '0979613631', 'Purkynova', 64, 'Imel', '94652');
INSERT INTO Instruktor VALUES('935824/1409', 'Sona', 'Bratislavska', 'sonicka14@gmail.com', '0590062222', 'Gyopsor', 12, 'Tesedikovo', '94617');
INSERT INTO Instruktor VALUES('995901/1480', 'Simona', 'Sladka', 'cukricek128@centrum.sk', '+421625996624', 'Nalepkova', 142, 'Hurbanovo', '94701');

INSERT INTO Zakaznik VALUES('910902/3187', 'Silvie', 'Sebestova', 'sebesticka33@yahoo.com', '+421807504515', 'Slana', 8, 'Praha', '68201', 84568, 'aktivne');
INSERT INTO Zakaznik VALUES('950322/2872', 'Lenka', 'Lorenzova', 'formulka99@motogp.cz', '0931983923', 'Kladna', 64, 'Olomouc', '67652', 56485, 'neaktivne');
INSERT INTO Zakaznik VALUES('936021/5975', 'Filip', 'Brazdil', 'kapitanBrazdil@tankiste.cz', '+420632949188', 'Kvetna', 74, 'Komarno', '94722', 54848651, 'aktivne');
INSERT INTO Zakaznik VALUES('955902/3177', 'Kvido', 'Tomanek', 'dankvido@parl.sk', '+421248962282', 'Hlavna', 823, 'Zilina', '94855', 5413364, 'aktivne');

INSERT INTO Kurz(dlzka_trvania, obtiaznost, popis, cena, Instruktor_FK) VALUES(30, 5, 'Kurz zumba zaciatocnici', 1000, '935824/1409');
INSERT INTO Kurz(dlzka_trvania, obtiaznost, popis, cena, Instruktor_FK) VALUES(30, 7, 'Kurz zumba pokrocili', 1200, '995901/1480');
INSERT INTO Kurz(dlzka_trvania, obtiaznost, popis, cena, Instruktor_FK) VALUES(60, 9, 'Do formy s Davidom', 1500, '985327/5344');
INSERT INTO Kurz(dlzka_trvania, obtiaznost, popis, cena, Instruktor_FK) VALUES(8, 3, 'Zaklady plavania', 500, '961221/3985');

-- Trigger na prevod ceny kurzu z CZK na Eura
CREATE OR REPLACE TRIGGER Prevod_meny_na_euro
    BEFORE INSERT ON Kurz
    FOR EACH ROW
BEGIN
    :NEW.cena := :NEW.cena / 25;
END;
/

INSERT INTO Kurz(dlzka_trvania, obtiaznost, popis, cena, Instruktor_FK) VALUES(30, 5, 'Crossfit', 500, '995901/1480');
INSERT INTO Kurz(dlzka_trvania, obtiaznost, popis, cena, Instruktor_FK) VALUES(15, 7, 'Orientacny beh', 800, '935824/1409');

-- Ukazka vypisu informacii o lekcii vcetne ceny v eurach
SELECT ID_kurzu, dlzka_trvania, obtiaznost, popis, cena AS cena_euro, instruktor_FK FROM Kurz WHERE ID_kurzu BETWEEN 5 AND 6;

INSERT INTO Lekcia(cena, popis, cas_zaciatku, cas_konca, den, maximalna_kapacita, obtiaznost, Sala_FK, Kurz_FK, Instruktor_FK) VALUES(60, 'skupinove potapanie', '14:30', '15:30', 'Po', 5, 8, 2, NULL, '961221/3985');
INSERT INTO Lekcia(cena, popis, cas_zaciatku, cas_konca, den, maximalna_kapacita, obtiaznost, Sala_FK, Kurz_FK, Instruktor_FK) VALUES(80, 'zumba', '14:30', '15:30', 'Po', 20, 5, 4, 1, '935824/1409');
INSERT INTO Lekcia(cena, popis, cas_zaciatku, cas_konca, den, maximalna_kapacita, obtiaznost, Sala_FK, Kurz_FK, Instruktor_FK) VALUES(120, 'zumba', '16:30', '17:30', 'Pi', 20, 7, 4, 2, '995901/1480');
INSERT INTO Lekcia(cena, popis, cas_zaciatku, cas_konca, den, maximalna_kapacita, obtiaznost, Sala_FK, Kurz_FK, Instruktor_FK) VALUES(100, 'kruhovy trening', '14:00', '15:30', 'Ut', 2, 9, 1, 3, '985327/5344');
INSERT INTO Lekcia(cena, popis, cas_zaciatku, cas_konca, den, maximalna_kapacita, obtiaznost, Sala_FK, Kurz_FK, Instruktor_FK) VALUES(40, 'plavanie na vydrz', '09:30', '11:00', 'So', 15, 8, 2, 4, '961221/3985');

-- Trigger na kontrolu ci maximalna kapacita Lekcie nepresahuje maximalnu kapacitu Saly
CREATE OR REPLACE TRIGGER kontrola_kapacity 
BEFORE INSERT OR UPDATE OF maximalna_kapacita ON Lekcia
FOR EACH ROW
    DECLARE kapacita NUMBER;
BEGIN
    SELECT maximalna_kapacita
    INTO kapacita
    FROM Sala
    WHERE Sala.ID_saly = :NEW.Sala_FK;
IF (:NEW.maximalna_kapacita > kapacita)
THEN raise_application_error(-20001, 'Kapacita lekcie presahuje kapacitu saly');
END IF;
END;
/

SET SERVEROUTPUT ON;
-- Procedura ktora zisti percentualne vyuzitie vsetkych sal spolu v zadany den
-- Den je zadany ako datum
CREATE OR REPLACE PROCEDURE vyuzitie_saly
(den IN DATE) AS
celkovo NUMBER;
zaciatok Lekcia.cas_zaciatku%TYPE;
den_v_tyzdni VARCHAR(3);
koniec Lekcia.cas_konca%TYPE;
pocet INTEGER;
percenta NUMBER;
BEGIN 
    zaciatok := den + INTERVAL '6' HOUR;
    koniec := den + INTERVAL '22' HOUR;
    den_v_tyzdni := TO_CHAR(den, 'Dy');
SELECT SUM(cas_konca - cas_zaciatku)*24 
INTO celkovo
FROM Lekcia L, Sala S
WHERE L.den = den_v_tyzdni AND L.Sala_FK = S.ID_saly;
IF celkovo IS NULL THEN
    RAISE NO_DATA_FOUND;
END IF;
SELECT COUNT(*) INTO pocet FROM Sala;
percenta := celkovo / ((koniec - zaciatok)*24*pocet);
IF pocet = 0 THEN
    RAISE ZERO_DIVIDE;
END IF;
DBMS_OUTPUT.put_line('Celkove vyuzitie sal pre den ' || den_v_tyzdni || ' je: ' || percenta*100 || '%');
EXCEPTION WHEN ZERO_DIVIDE THEN
BEGIN
    DBMS_OUTPUT.put_line('Tabulka Sala je prazdna');
END;
WHEN NO_DATA_FOUND THEN
BEGIN
    DBMS_OUTPUT.put_line('Nenasli sa lekcie pre dany den');
END;
END vyuzitie_saly;
/
 
EXECUTE vyuzitie_saly(TO_DATE('2019-04-15','YYYY-MM-DD')); 
 
INSERT INTO Registroval_Kurz VALUES('910902/3187', 1);
INSERT INTO Registroval_Kurz VALUES('910902/3187', 4);
INSERT INTO Registroval_Kurz VALUES('936021/5975', 3);
INSERT INTO Registroval_Kurz VALUES('955902/3177', 3);

INSERT INTO Registroval_Lekciu VALUES('936021/5975', 5);
INSERT INTO Registroval_Lekciu VALUES('936021/5975', 1);
INSERT INTO Registroval_Lekciu VALUES('910902/3187', 3);
INSERT INTO Registroval_Lekciu VALUES('955902/3177', 1);

--Select pre dve tabulky 
--Vypis popisu, ceny, obtiaznosti a dlzky trvania kurzov ktore vedie instruktor s menom David Janku
SELECT K.popis, K.cena, K.obtiaznost, K.dlzka_trvania
FROM Instruktor I, Kurz K
WHERE I.rodne_cislo = K.Instruktor_FK and I.meno = 'David' and I.priezvisko = 'Janku';
--Vypis popisu, ceny, obtiaznosti , dna, kapacity pre lekcie ktore su vedene v plavarni
SELECT L.popis, L.cena, L.obtiaznost, L.den, L.maximalna_kapacita
FROM Sala S, Lekcia L
WHERE S.nazov = 'plavaren' and S.ID_saly = L.SALA_FK;

--Select pre tri tabulky 
--Vypis mena a priezviska zakaznikov a popis Kurzov ktore si registrovali
SELECT Z.meno, Z.priezvisko, K.popis AS Popis_kurzu
FROM Zakaznik Z, Kurz K, Registroval_Kurz RK
WHERE Z.rodne_cislo = RK.rodne_cislo_ZakaznikaK and K.ID_kurzu = RK.ID_kurzu;

--Dotazy s klauzuli GROUP BY a agregacni funkci
--Vypis poctu lekcii danej obtiaznosti
SELECT obtiaznost, count(ID_lekcie) AS Pocet_lekcii
FROM Lekcia
GROUP BY obtiaznost
ORDER BY obtiaznost;
--Vypis najlacnejsej ceny lekcie v dany den
SELECT den, min(cena) AS Min_cena
FROM Lekcia
GROUP BY den
ORDER BY decode(den, 'Pon', 1, 'Uto', 2, 'Str', 3, 'Stv', 4, 'Pia', 5, 'Sob', 6, 'Ned', 7);

--Dotaz obsahujici predikat EXISTS
--Vypis zakaznikov ktori si registrovali nejaky kurz
SELECT *
FROM Zakaznik
WHERE EXISTS(SELECT rodne_cislo_ZakaznikaK FROM Registroval_Kurz WHERE rodne_cislo_ZakaznikaK = Zakaznik.rodne_cislo);

--Dotaz s predikatem IN s vnorenym selectem
--Vypis Kurzu v ktorom cena jednej lekcie nepresahuje 84
SELECT popis, obtiaznost, dlzka_trvania, INSTRUKTOR_FK AS Instruktor
FROM Kurz
WHERE ID_kurzu
IN(SELECT Kurz_FK FROM Lekcia WHERE cena < 85);

EXPLAIN PLAN FOR
SELECT Z.meno, Z.priezvisko, COUNT(*) AS Pocet_lekcii, SUM(Cena) AS Celkova_cena_lekcii 
FROM Zakaznik Z, Registroval_Lekciu RL, Lekcia L
WHERE Z.rodne_cislo = RL.rodne_cislo_ZakaznikaL AND RL.ID_lekcie = L.ID_lekcie
GROUP BY Z.meno, Z.priezvisko;

SELECT PLAN_TABLE_OUTPUT FROM TABLE(DBMS_XPLAN.DISPLAY());

-- Zistit kam sa ma dat index a ci je ten select na tento index dobry
CREATE INDEX index_lekcia ON Lekcia(cena);

EXPLAIN PLAN FOR
SELECT Z.meno, Z.priezvisko, COUNT(*) AS Pocet_lekcii, SUM(Cena) AS Celkova_cena_lekcii 
FROM Zakaznik Z, Registroval_Lekciu RL, Lekcia L
WHERE Z.rodne_cislo = RL.rodne_cislo_ZakaznikaL AND RL.ID_lekcie = L.ID_lekcie
GROUP BY Z.meno, Z.priezvisko;

SELECT PLAN_TABLE_OUTPUT FROM TABLE(DBMS_XPLAN.DISPLAY());

DROP INDEX index_lekcia;

-- Pridelenie pristupovych prav, simulacia cinnosti instruktora
GRANT ALL ON Sala TO xzigot00;
GRANT ALL ON Lekcia TO xzigot00;
GRANT ALL ON Kurz TO xzigot00;
GRANT SELECT ON Instruktor TO xzigot00;
GRANT SELECT ON Zakaznik TO xzigot00;

GRANT EXECUTE ON vyuzitie_saly TO xzigot00;