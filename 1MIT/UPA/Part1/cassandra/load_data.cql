//cqlsh -u 'demo' -p 'demo' -f project-apache_cassandra.cql

CREATE KEYSPACE IF NOT EXISTS projekt WITH replication = {'class': 'SimpleStrategy', 'replication_factor' : 1};
USE projekt;

DROP TABLE projekt.nehody;

CREATE TABLE nehody(
	X float,
	Y float,
	OBJECTID int,
	id bigint,
	d float,
	e float,
	datum timestamp,
	den int,
	rok int,
	mesic int,
	zavineni text,
	viditelnost text,
	cas int,
	hodina int,
	mesic_t text,
	doba text,
	den_v_tydnu text,
	alkohol text,
	alkohol_vinik text,
	hlavni_pricina text,
	srazka text,
	pricina text,
	stav_vozovky text,
	povetrnostni_podm text,
	rozhled text,
	misto_nehody text,
	druh_komun text,
	usmrceno_os int,
	tezce_zran_os int,
	lehce_zran_os int,
	hmotna_skoda int,
	smrt boolean,
	tz int,
	lz int,
	smrt_dny int,
	smrt_po int,
	vek_skupina text,
	chodec_pohlavi text,
	kategorie_chodce text,
	stav_chodce text,
	chovani_chodce text,
	situace_nehody text,
	prvni_pomoc text,
	nasledky_chodce text,
	nazev text,
	POINT_X float,
	POINT_Y float,
	GlobalID text,
	PRIMARY KEY(datum, OBJECTID)
);


COPY projekt.nehody (X,Y,OBJECTID,id,d,e,datum,den,rok,mesic,zavineni,viditelnost,cas,hodina,mesic_t,doba,den_v_tydnu,alkohol,alkohol_vinik,hlavni_pricina,srazka,pricina,stav_vozovky,povetrnostni_podm,rozhled,misto_nehody,druh_komun,usmrceno_os,tezce_zran_os,lehce_zran_os,hmotna_skoda,smrt,tz,lz,smrt_dny,smrt_po,vek_skupina,chodec_pohlavi,kategorie_chodce,stav_chodce,chovani_chodce,situace_nehody,prvni_pomoc,nasledky_chodce,nazev,POINT_X,POINT_Y,GlobalID)
FROM 'modif_nehody.csv' WITH DELIMITER=',' AND HEADER=TRUE;

-- Queries:
-- 1)
SELECT datum, cas, nazev, hlavni_pricina, kategorie_chodce, vek_skupina, lehce_zran_os, tezce_zran_os, stav_vozovky
FROM nehody WHERE datum = '2016-01-12';

-- 2)
SELECT OBJECTID, id, zavineni, hmotna_skoda FROM nehody
WHERE datum = '2020-09-01' AND stav_vozovky = 'povrch mokrý' AND hmotna_skoda > 1 ALLOW FILTERING;

-- 3)
--DROP INDEX index_mesic;
--CREATE INDEX IF NOT EXISTS index_mesic ON nehody (mesic);
-- Wait for an index creation
--SELECT SUM(usmrceno_os) AS "osob usmrceno v lednu"
--FROM nehody WHERE mesic = 1 AND datum >= '2012-01-01' ALLOW FILTERING;

-- 4)
--DROP INDEX index_nazev;
--CREATE INDEX IF NOT EXISTS index_nazev ON nehody (nazev);
-- Wait for an index creation
--SELECT COUNT(*) AS "pocet nehod s chodci 19-24 let v Brno-stred"
--FROM nehody WHERE nazev='Brno-střed' AND vek_skupina='19-24' ALLOW FILTERING;

