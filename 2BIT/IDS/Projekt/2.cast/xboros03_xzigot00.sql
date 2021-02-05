DROP TABLE "Sala" CASCADE CONSTRAINTS;
DROP TABLE "Lekcia" CASCADE CONSTRAINTS;
DROP TABLE "Kurz" CASCADE CONSTRAINTS;
DROP TABLE "Zakaznik" CASCADE CONSTRAINTS;
DROP TABLE "Instruktor" CASCADE CONSTRAINTS;
DROP TABLE "Registroval_Kurz" CASCADE CONSTRAINTS;
DROP TABLE "Registroval_Lekciu" CASCADE CONSTRAINTS;

CREATE TABLE "Sala"(
  "ID_saly" INTEGER NOT NULL,
  "nazov" VARCHAR(12) NOT NULL,
  "umiestnenie" CHAR(1) NOT NULL,
  "maximalna kapacita" INTEGER NOT NULL CHECK("maximalna kapacita" > 0)
);

ALTER SESSION SET NLS_DATE_FORMAT = 'HH24:MI';

CREATE TABLE "Lekcia"(
  "ID_lekcie" INTEGER NOT NULL,
  "cena" DECIMAL(10,2) CHECK("cena" > 0),
  "popis" VARCHAR(30) NOT NULL,
  "cas zaciatku" DATE NOT NULL,
  "cas konca" DATE NOT NULL,
  "den" VARCHAR(3) NOT NULL CHECK("den" = 'Pon' or "den" = 'Uto' or "den" = 'Str' or "den" = 'Stv' or "den" = 'Pia' or "den" = 'Sob' or "den" = 'Ned'),
  "maximalna kapacita" INTEGER NOT NULL CHECK("maximalna kapacita" > 0),
  "obtiaznost" INTEGER NOT NULL CHECK("obtiaznost" > 0 AND "obtiaznost" <= 10),
  "Sala_FK" INTEGER NOT NULL,
  "Kurz_FK" INTEGER,
  "Instruktor_FK" CHAR(11) NOT NULL,
  CONSTRAINT "check Date" CHECK("cas zaciatku" < "cas konca")
);

CREATE TABLE "Kurz"(
  "ID_kurzu" INTEGER NOT NULL,
  "dlzka trvania" INTEGER NOT NULL CHECK("dlzka trvania" > 0),
  "obtiaznost" INTEGER NOT NULL CHECK("obtiaznost" > 0 AND "obtiaznost" <= 10),
  "popis" VARCHAR(30) NOT NULL,
  "cena" DECIMAL(10,2) NOT NULL CHECK("cena" > 0),
  "Instruktor_FK" CHAR(11) NOT NULL
);

CREATE TABLE "Zakaznik"(
  "rodne cislo" CHAR(11) NOT NULL,
  "meno" VARCHAR(20) NOT NULL,
  "priezvisko" VARCHAR(20) NOT NULL,
  "e-mail" VARCHAR(30),
  "telefonne cislo" VARCHAR(13),
  "ulica" VARCHAR(20) NOT NULL,
  "cislo popisne" INTEGER NOT NULL CHECK("cislo popisne" > 0),
  "mesto" VARCHAR(20) NOT NULL,
  "PSC" INTEGER NOT NULL,
  "cislo karty" INTEGER NOT NULL,
  "stav clenstva" VARCHAR(9) NOT NULL CHECK("stav clenstva" = 'aktivne' OR "stav clenstva" = 'neaktivne'),
  CONSTRAINT mailCheckZakaznik CHECK(REGEXP_LIKE("e-mail", '^(\S+)\@(\S+)\.(\S+)$'))
);

CREATE TABLE "Instruktor"(
  "rodne cislo" CHAR(11) NOT NULL,
  "meno" VARCHAR(20) NOT NULL,
  "priezvisko" VARCHAR(20) NOT NULL,
  "e-mail" VARCHAR(30),
  "telefonne cislo" VARCHAR(13),
  "ulica" VARCHAR(20) NOT NULL,
  "cislo popisne" INTEGER NOT NULL,
  "mesto" VARCHAR(20) NOT NULL,
  "PSC" INTEGER NOT NULL,
  CONSTRAINT mailCheckInstruktor CHECK(REGEXP_LIKE("e-mail", '^(\S+)\@(\S+)\.(\S+)$'))
);

ALTER TABLE "Sala" ADD CONSTRAINT "Sala_PK" PRIMARY KEY ("ID_saly"); 
ALTER TABLE "Lekcia" ADD CONSTRAINT "Lekcia_PK" PRIMARY KEY ("ID_lekcie"); 
ALTER TABLE "Kurz" ADD CONSTRAINT "Kurz_PK" PRIMARY KEY ("ID_kurzu"); 
ALTER TABLE "Zakaznik" ADD CONSTRAINT "Zakaznik_PK" PRIMARY KEY ("rodne cislo"); 
ALTER TABLE "Instruktor" ADD CONSTRAINT "Instruktor_PK" PRIMARY KEY ("rodne cislo"); 

ALTER TABLE "Lekcia" ADD CONSTRAINT "Lekcia_sala_FK" FOREIGN KEY ("Sala_FK") REFERENCES "Sala"("ID_saly") ON DELETE CASCADE;
ALTER TABLE "Lekcia" ADD CONSTRAINT "Kurz_lekcia_FK" FOREIGN KEY("Kurz_FK") REFERENCES "Kurz"("ID_kurzu") ON DELETE CASCADE;
ALTER TABLE "Kurz" ADD CONSTRAINT "Instruktor_kurz_FK" FOREIGN KEY ("Instruktor_FK") REFERENCES "Instruktor"("rodne cislo") ON DELETE CASCADE;
ALTER TABLE "Lekcia" ADD CONSTRAINT "Instruktor_lekcia_FK" FOREIGN KEY ("Instruktor_FK") REFERENCES "Instruktor"("rodne cislo") ON DELETE CASCADE;

CREATE TABLE "Registroval_Kurz"(
  "rodne cislo ZakaznikaK" CHAR(11) NOT NULL,
  "ID_kurzu" INTEGER NOT NULL
);

CREATE TABLE "Registroval_Lekciu"(
  "rodne cislo ZakaznikaL" CHAR(11) NOT NULL,
  "ID_lekcie" INTEGER NOT NULL
);

ALTER TABLE "Registroval_Kurz" ADD CONSTRAINT "Registroval_Kurz_PK" PRIMARY KEY ("rodne cislo ZakaznikaK", "ID_kurzu"); 
ALTER TABLE "Registroval_Lekciu" ADD CONSTRAINT "Registroval_Lekciu_PK" PRIMARY KEY ("rodne cislo ZakaznikaL", "ID_lekcie"); 

INSERT INTO "Sala" VALUES(1, 'posilovna', 'E', 30);
INSERT INTO "Sala" VALUES(2, 'plavaren', 'A', 15);
INSERT INTO "Sala" VALUES(3, 'sauna', 'B', 8);
INSERT INTO "Sala" VALUES(4, 'kardio', 'E', 25);
INSERT INTO "Sala" VALUES(5, 'boxerna', 'B', 6);

INSERT INTO "Instruktor" VALUES('985327/5344', 'David', 'Janku', 'davidjanku2@pokec.cz', '+420164985925', 'Jiraskova', 58, 'Brno', '94201');
INSERT INTO "Instruktor" VALUES('961221/3985', 'Marek', 'Kovac', 'optimus@seznam.cz', '+420679613631', 'Purkynova', 64, 'Imel', '94652');
INSERT INTO "Instruktor" VALUES('935824/1409', 'Sona', 'Bratislavska', 'sonicka14@gmail.com', '+420090062222', 'Gyopsor', 12, 'Tesedikovo', '94617');
INSERT INTO "Instruktor" VALUES('995901/1480', 'Simona', 'Sladka', 'cukricek128@centrum.sk', '+421625996624', 'Nalepkova', 142, 'Hurbanovo', '94701');

INSERT INTO "Zakaznik" VALUES('910902/3187', 'Silvie', 'Sebestova', 'sebesticka33@yahoo.com', '+421807504515', 'Slana', 8, 'Praha', '68201', 84568, 'aktivne');
INSERT INTO "Zakaznik" VALUES('950322/2872', 'Lenka', 'Lorenzova', 'formulka99@motogp.cz', '+421531983923', 'Kladna', 64, 'Olomouc', '67652', 56485, 'neaktivne');
INSERT INTO "Zakaznik" VALUES('936021/5975', 'Filip', 'Brazdil', 'kapitanBrazdil@tankiste.cz', '+421632949188', 'Kvetna', 74, 'Komarno', '94722', 54848651, 'aktivne');
INSERT INTO "Zakaznik" VALUES('955902/3177', 'Kvido', 'Tomanek', 'dankvido@parl.sk', '+421248962282', 'Hlavna', 823, 'Zilina', '94855', 5413364, 'aktivne');

INSERT INTO  "Kurz" VALUES(1, 30, 5, 'Kurz zumba zaciatocnici', 1000, '935824/1409');
INSERT INTO  "Kurz" VALUES(2, 30, 7, 'Kurz zumba pokrocili', 1200, '995901/1480');
INSERT INTO  "Kurz" VALUES(3, 60, 9, 'Do formy s Davidom', 1500, '985327/5344');
INSERT INTO  "Kurz" VALUES(4, 8, 3, 'Zaklady plavania', 500, '961221/3985');

INSERT INTO "Lekcia" VALUES(1, 60, 'skupinove potapanie', '14:30', '15:30', 'Pon', 5, 8, 2, NULL, '961221/3985');
INSERT INTO "Lekcia" VALUES(2, NULL, 'zumba', '14:30', '15:30', 'Pon', 20, 5, 4, 1, '935824/1409');
INSERT INTO "Lekcia" VALUES(3, 120, 'zumba', '16:30', '17:30', 'Pia', 20, 7, 4, 2, '995901/1480');
INSERT INTO "Lekcia" VALUES(4, NULL, 'kruhovy trening', '14:00', '15:30', 'Uto', 2, 9, 1, 3, '985327/5344');
INSERT INTO "Lekcia" VALUES(5, 40, 'plavanie na vydrz', '09:30', '11:00', 'Sob', 15, 8, 2, 4, '961221/3985');

INSERT INTO "Registroval_Kurz" VALUES('910902/3187', 1);
INSERT INTO "Registroval_Kurz" VALUES('910902/3187', 4);
INSERT INTO "Registroval_Kurz" VALUES('936021/5975', 3);
INSERT INTO "Registroval_Kurz" VALUES('955902/3177', 3);

INSERT INTO "Registroval_Lekciu" VALUES('936021/5975', 5);
INSERT INTO "Registroval_Lekciu" VALUES('936021/5975', 1);
INSERT INTO "Registroval_Lekciu" VALUES('910902/3187', 3);
INSERT INTO "Registroval_Lekciu" VALUES('955902/3177', 1);

SELECT *FROM "Instruktor";
SELECT *FROM "Zakaznik";
Select *from "Lekcia";
