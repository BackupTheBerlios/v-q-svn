--
-- Selected TOC Entries:
--
\connect - poczta

--
-- TOC Entry ID 51 (OID 16660)
--
-- Name: domains Type: TABLE Owner: poczta
--

CREATE TABLE "domains" (
	"id" integer DEFAULT nextval('domains_id_seq'::text) NOT NULL,
	"domain" text NOT NULL,
	"quota_max" double precision DEFAULT 0 NOT NULL,
	"quota_akt" double precision DEFAULT 0 NOT NULL,
	CONSTRAINT "domains_domains_empty" CHECK ((domain <> ''::text)),
	Constraint "domains_pkey" Primary Key ("domain")
);

--
-- TOC Entry ID 52 (OID 16660)
--
-- Name: domains Type: ACL Owner: 
--

REVOKE ALL on "domains" from PUBLIC;
GRANT ALL on "domains" to "poczta";

--
-- TOC Entry ID 2 (OID 16666)
--
-- Name: ip_id_seq Type: SEQUENCE Owner: poczta
--

CREATE SEQUENCE "ip_id_seq" start 1 increment 1 maxvalue 9223372036854775807 minvalue 1 cache 1;

--
-- TOC Entry ID 4 (OID 16666)
--
-- Name: ip_id_seq Type: ACL Owner: 
--

REVOKE ALL on "ip_id_seq" from PUBLIC;
GRANT ALL on "ip_id_seq" to "poczta";

--
-- TOC Entry ID 53 (OID 16668)
--
-- Name: ip Type: TABLE Owner: poczta
--

CREATE TABLE "ip" (
	"id" integer DEFAULT nextval('"ip_id_seq"'::text) NOT NULL,
	"ip" text NOT NULL,
	"domain" integer,
	CONSTRAINT "ip_ip" CHECK ((ip <> ''::text)),
	Constraint "ip_pkey" Primary Key ("ip")
);

--
-- TOC Entry ID 54 (OID 16668)
--
-- Name: ip Type: ACL Owner: 
--

REVOKE ALL on "ip" from PUBLIC;
GRANT ALL on "ip" to "poczta";

--
-- TOC Entry ID 59 (OID 213690)
--
-- Name: domains_all Type: TABLE Owner: poczta
--

CREATE TABLE "domains_all" (
	"id" integer DEFAULT nextval('domains_all_id_seq'::text) NOT NULL,
	"name" text NOT NULL,
	"domain_id" integer,
	CONSTRAINT "domains_all_name_empty" CHECK ((name <> ''::text)),
	Constraint "domains_all_pkey" Primary Key ("id")
);

--
-- TOC Entry ID 60 (OID 213690)
--
-- Name: domains_all Type: ACL Owner: 
--

REVOKE ALL on "domains_all" from PUBLIC;
GRANT ALL on "domains_all" to "poczta";

--
-- TOC Entry ID 61 (OID 213726)
--
-- Name: domains_prefixes Type: TABLE Owner: poczta
--

CREATE TABLE "domains_prefixes" (
	"id" integer DEFAULT nextval('domains_prefixes_id_seq'::text) NOT NULL,
	"prefix" text NOT NULL,
	"dir" text NOT NULL,
	"subdir" text DEFAULT '' NOT NULL,
	"domain_id" integer,
	CONSTRAINT "domains_prefixes_dir_empty" CHECK ((dir <> ''::text)),
	CONSTRAINT "domains_prefixes_prefix_empty" CHECK ((prefix <> ''::text)),
	Constraint "domains_prefixes_pkey" Primary Key ("id")
);

--
-- TOC Entry ID 62 (OID 213726)
--
-- Name: domains_prefixes Type: ACL Owner: 
--

REVOKE ALL on "domains_prefixes" from PUBLIC;
GRANT ALL on "domains_prefixes" to "poczta";

--
-- TOC Entry ID 41 (OID 288323)
--
-- Name: domains_id_seq Type: SEQUENCE Owner: poczta
--

CREATE SEQUENCE "domains_id_seq" start 1 increment 1 maxvalue 9223372036854775807 minvalue 1 cache 1;

--
-- TOC Entry ID 43 (OID 288331)
--
-- Name: domains_all_id_seq Type: SEQUENCE Owner: poczta
--

CREATE SEQUENCE "domains_all_id_seq" start 1 increment 1 maxvalue 9223372036854775807 minvalue 1 cache 1;

--
-- TOC Entry ID 45 (OID 288333)
--
-- Name: domains_prefixes_id_seq Type: SEQUENCE Owner: poczta
--

CREATE SEQUENCE "domains_prefixes_id_seq" start 1 increment 1 maxvalue 9223372036854775807 minvalue 1 cache 1;

--
-- Data for TOC Entry ID 119 (OID 16660)
--
-- Name: domains Type: TABLE DATA Owner: poczta
--


COPY "domains" FROM stdin;
11	izbaarchitektow.pl	0	0
9	sarp.org.pl	0	0
10	iarp.pl	0	0
\.
--
-- Data for TOC Entry ID 120 (OID 16668)
--
-- Name: ip Type: TABLE DATA Owner: poczta
--


COPY "ip" FROM stdin;
12	195.116.83.197	\N
13	195.116.83.198	\N
14	195.116.83.199	\N
15	195.116.83.200	\N
16	195.116.83.201	\N
17	195.116.83.202	\N
18	195.116.83.203	\N
19	195.116.83.204	\N
20	195.116.83.205	\N
21	195.116.83.206	\N
22	195.116.83.207	\N
23	195.116.83.208	\N
24	195.116.83.209	\N
25	195.116.83.210	\N
26	195.116.83.211	\N
27	195.116.83.212	\N
28	195.116.83.213	\N
29	195.116.83.214	\N
30	195.116.83.215	\N
31	195.116.83.216	\N
32	195.116.83.217	\N
33	195.116.83.218	\N
34	195.116.83.219	\N
35	195.116.83.220	\N
36	195.116.83.221	\N
37	195.116.83.222	\N
9	195.116.83.194	9
10	195.116.83.195	10
11	195.116.83.196	11
\.
--
-- Data for TOC Entry ID 123 (OID 213690)
--
-- Name: domains_all Type: TABLE DATA Owner: poczta
--


COPY "domains_all" FROM stdin;
8	izbaarchitektow.pl	11
5	sarp.org.pl	9
6	iarp.pl	10
\.
--
-- Data for TOC Entry ID 124 (OID 213726)
--
-- Name: domains_prefixes Type: TABLE DATA Owner: poczta
--


COPY "domains_prefixes" FROM stdin;
27	amp	/amp		5
28	pow-uia	/pow-uia		5
29	gorzow	gorzow		5
30	jgora	jgora		5
31	katowice	katowice		5
32	koszalin	koszalin		5
33	lodz	lodz		5
34	poznan	poznan		5
35	warszawa	warszawa		5
36	wroclaw	wroclaw		5
37	tuczno	tuczno		5
38	lodzka	lodzka		6
39	lubelska	lubelska		6
40	lubuska	lubuska		6
41	malopolska	malopolska		6
42	mazowiecka	mazowiecka		6
43	podlaska	podlaska		6
44	pomorska	pomorska		6
45	slaska	slaska		6
46	wielkopolska	wielkopolska		6
47	wm	wm		6
48	zachodniopomorska	zachodniopomorska		6
77	www.zachodniopomorska	zachodniopomorska		6
54	www	/		6
55	www.lodzka	lodzka		6
56	www.lubelska	lubelska		6
57	www.lubuska	lubuska		6
58	www.malopolska	malopolska		6
59	www.mazowiecka	mazowiecka		6
60	www.podlaska	podlaska		6
61	www.pomorska	pomorska		6
62	www.slaska	slaska		6
63	www.wielkopolska	wielkopolska		6
64	www.wm	wm		6
65	www.amp	amp		5
66	www.gorzow	gorzow		5
67	www.jgora	jgora		5
68	www.katowice	katowice		5
69	www.koszalin	koszalin		5
70	www.lodz	lodz		5
71	www.pow-uia	pow-uia		5
72	www.poznan	poznan		5
73	www.tuczno	tuczno		5
74	www.warszawa	warszawa		5
20	www	/		5
75	www.wroclaw	wroclaw		5
76	www	/		8
\.

--
-- TOC Entry ID 91 (OID 16689)
--
-- Name: "domeny_id_key" Type: INDEX Owner: poczta
--

CREATE UNIQUE INDEX domains_id_key ON domains USING btree (id);

--
-- TOC Entry ID 92 (OID 16690)
--
-- Name: "ip_id_key" Type: INDEX Owner: poczta
--

CREATE UNIQUE INDEX ip_id_key ON ip USING btree (id);

--
-- TOC Entry ID 93 (OID 213696)
--
-- Name: "domeny_wszystkie_nazwa_key" Type: INDEX Owner: poczta
--

CREATE UNIQUE INDEX domains_all_name_key ON domains_all USING btree (name);

--
-- TOC Entry ID 146 (OID 16692)
--
-- Name: "RI_ConstraintTrigger_16691" Type: TRIGGER Owner: poczta
--

CREATE CONSTRAINT TRIGGER "ip_domain_id_fkey" AFTER INSERT OR UPDATE ON "ip"  FROM "domains" DEFERRABLE INITIALLY DEFERRED FOR EACH ROW EXECUTE PROCEDURE "RI_FKey_check_ins" ('ip_domain_id_fkey', 'ip', 'domains', 'UNSPECIFIED', 'domain', 'id');

--
-- TOC Entry ID 136 (OID 16694)
--
-- Name: "RI_ConstraintTrigger_16693" Type: TRIGGER Owner: poczta
--

CREATE CONSTRAINT TRIGGER "ip_domain_id_fkey" AFTER DELETE ON "domains"  FROM "ip" DEFERRABLE INITIALLY DEFERRED FOR EACH ROW EXECUTE PROCEDURE "RI_FKey_setnull_del" ('ip_domain_id_fkey', 'ip', 'domains', 'UNSPECIFIED', 'domain', 'id');

--
-- TOC Entry ID 137 (OID 16696)
--
-- Name: "RI_ConstraintTrigger_16695" Type: TRIGGER Owner: poczta
--

CREATE CONSTRAINT TRIGGER "ip_domain_id_fkey" AFTER UPDATE ON "domains"  FROM "ip" DEFERRABLE INITIALLY DEFERRED FOR EACH ROW EXECUTE PROCEDURE "RI_FKey_cascade_upd" ('ip_domain_id_fkey', 'ip', 'domains', 'UNSPECIFIED', 'domain', 'id');

--
-- TOC Entry ID 148 (OID 213698)
--
-- Name: "RI_ConstraintTrigger_213697" Type: TRIGGER Owner: poczta
--

CREATE CONSTRAINT TRIGGER "domains_all_domain_id_fkey" AFTER INSERT OR UPDATE ON "domains_all"  FROM "domains" NOT DEFERRABLE INITIALLY IMMEDIATE FOR EACH ROW EXECUTE PROCEDURE "RI_FKey_check_ins" ('domains_all_domain_id_fkey', 'domains_all', 'domains', 'UNSPECIFIED', 'domain_id', 'id');

--
-- TOC Entry ID 145 (OID 213700)
--
-- Name: "RI_ConstraintTrigger_213699" Type: TRIGGER Owner: poczta
--

CREATE CONSTRAINT TRIGGER "domains_all_domain_id_fkey" AFTER DELETE ON "domains"  FROM "domains_all" NOT DEFERRABLE INITIALLY IMMEDIATE FOR EACH ROW EXECUTE PROCEDURE "RI_FKey_cascade_del" ('domains_all_domain_id_fkey', 'domains_all', 'domains', 'UNSPECIFIED', 'domain_id', 'id');

--
-- TOC Entry ID 144 (OID 213702)
--
-- Name: "RI_ConstraintTrigger_213701" Type: TRIGGER Owner: poczta
--

CREATE CONSTRAINT TRIGGER "domains_all_domain_id_fkey" AFTER UPDATE ON "domains"  FROM "domains_all" NOT DEFERRABLE INITIALLY IMMEDIATE FOR EACH ROW EXECUTE PROCEDURE "RI_FKey_cascade_upd" ('domains_all_domain_id_fkey', 'domains_all', 'domains', 'UNSPECIFIED', 'domain_id', 'id');

--
-- TOC Entry ID 151 (OID 213733)
--
-- Name: "RI_ConstraintTrigger_213732" Type: TRIGGER Owner: poczta
--

CREATE CONSTRAINT TRIGGER "domains_prefixes_domains_all" AFTER INSERT OR UPDATE ON "domains_prefixes"  FROM "domains_all" NOT DEFERRABLE INITIALLY IMMEDIATE FOR EACH ROW EXECUTE PROCEDURE "RI_FKey_check_ins" ('domains_prefixes_domains_all', 'domains_prefixes', 'domains_all', 'UNSPECIFIED', 'domain_id', 'id');

--
-- TOC Entry ID 150 (OID 213735)
--
-- Name: "RI_ConstraintTrigger_213734" Type: TRIGGER Owner: poczta
--

CREATE CONSTRAINT TRIGGER "domains_prefixes_domains_all" AFTER DELETE ON "domains_all"  FROM "domains_prefixes" NOT DEFERRABLE INITIALLY IMMEDIATE FOR EACH ROW EXECUTE PROCEDURE "RI_FKey_cascade_del" ('domains_prefixes_domains_all', 'domains_prefixes', 'domains_all', 'UNSPECIFIED', 'domain_id', 'id');

--
-- TOC Entry ID 149 (OID 213737)
--
-- Name: "RI_ConstraintTrigger_213736" Type: TRIGGER Owner: poczta
--

CREATE CONSTRAINT TRIGGER "domains_prefixes_domains_all" AFTER UPDATE ON "domains_all"  FROM "domains_prefixes" NOT DEFERRABLE INITIALLY IMMEDIATE FOR EACH ROW EXECUTE PROCEDURE "RI_FKey_cascade_upd" ('domains_prefixes_domains_all', 'domains_prefixes', 'domains_all', 'UNSPECIFIED', 'domain_id', 'id');

--
-- TOC Entry ID 3 (OID 16666)
--
-- Name: ip_id_seq Type: SEQUENCE SET Owner: poczta
--

SELECT setval ('"ip_id_seq"', 37, true);

--
-- TOC Entry ID 42 (OID 288323)
--
-- Name: domains_id_seq Type: SEQUENCE SET Owner: poczta
--

SELECT setval ('"domains_id_seq"', 50, true);

--
-- TOC Entry ID 44 (OID 288331)
--
-- Name: domains_all_id_seq Type: SEQUENCE SET Owner: poczta
--

SELECT setval ('"domains_all_id_seq"', 50, true);

--
-- TOC Entry ID 46 (OID 288333)
--
-- Name: domains_prefixes_id_seq Type: SEQUENCE SET Owner: poczta
--

SELECT setval ('"domains_prefixes_id_seq"', 50, true);

