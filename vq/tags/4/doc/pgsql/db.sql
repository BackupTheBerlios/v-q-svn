--
-- PostgreSQL database dump
--

\connect - pgsql

SET search_path = public, pg_catalog;

--
-- TOC entry 9 (OID 16977)
-- Name: plpgsql_call_handler (); Type: FUNCTION; Schema: public; Owner: pgsql
--

CREATE FUNCTION plpgsql_call_handler () RETURNS language_handler
    AS '/usr/local/pgsql/lib//plpgsql.so', 'plpgsql_call_handler'
    LANGUAGE c;


--
-- TOC entry 8 (OID 16978)
-- Name: plpgsql; Type: PROCEDURAL LANGUAGE; Schema: public; Owner: 
--

CREATE TRUSTED PROCEDURAL LANGUAGE plpgsql HANDLER plpgsql_call_handler;


\connect - mail

SET search_path = public, pg_catalog;

--
-- TOC entry 2 (OID 16979)
-- Name: ip2domain; Type: TABLE; Schema: public; Owner: mail
--

CREATE TABLE ip2domain (
    ip text NOT NULL,
    "domain" text NOT NULL,
    CONSTRAINT ip2domain_domain CHECK (("domain" <> ''::text)),
    CONSTRAINT ip2domain_ip CHECK ((ip <> ''::text))
);


--
-- TOC entry 3 (OID 16979)
-- Name: ip2domain; Type: ACL; Schema: public; Owner: mail
--

REVOKE ALL ON TABLE ip2domain FROM PUBLIC;


--
-- TOC entry 4 (OID 16995)
-- Name: login_banned; Type: TABLE; Schema: public; Owner: mail
--

CREATE TABLE login_banned (
    login text NOT NULL,
    CONSTRAINT login_banned_login CHECK ((login <> ''::text))
);


--
-- TOC entry 5 (OID 16995)
-- Name: login_banned; Type: ACL; Schema: public; Owner: mail
--

REVOKE ALL ON TABLE login_banned FROM PUBLIC;


--
-- Data for TOC entry 10 (OID 16979)
-- Name: ip2domain; Type: TABLE DATA; Schema: public; Owner: mail
--

COPY ip2domain (ip, "domain") FROM stdin;
\.


--
-- Data for TOC entry 11 (OID 16995)
-- Name: login_banned; Type: TABLE DATA; Schema: public; Owner: mail
--

COPY login_banned (login) FROM stdin;
\.


--
-- TOC entry 6 (OID 16986)
-- Name: ip2domain_pkey; Type: CONSTRAINT; Schema: public; Owner: mail
--

ALTER TABLE ONLY ip2domain
    ADD CONSTRAINT ip2domain_pkey PRIMARY KEY (ip);


--
-- TOC entry 7 (OID 17001)
-- Name: login_banned_pkey; Type: CONSTRAINT; Schema: public; Owner: mail
--

ALTER TABLE ONLY login_banned
    ADD CONSTRAINT login_banned_pkey PRIMARY KEY (login);


