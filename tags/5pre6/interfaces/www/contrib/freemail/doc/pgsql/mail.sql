--
-- PostgreSQL database dump
--

\connect - pgsql

SET search_path = public, pg_catalog;

--
-- TOC entry 113 (OID 16977)
-- Name: plpgsql_call_handler (); Type: FUNCTION; Schema: public; Owner: pgsql
--

CREATE FUNCTION plpgsql_call_handler () RETURNS language_handler
    AS '/usr/local/pgsql/lib//plpgsql.so', 'plpgsql_call_handler'
    LANGUAGE c;


--
-- TOC entry 112 (OID 16978)
-- Name: plpgsql; Type: PROCEDURAL LANGUAGE; Schema: public; Owner: 
--

CREATE TRUSTED PROCEDURAL LANGUAGE plpgsql HANDLER plpgsql_call_handler;


\connect - mail

SET search_path = public, pg_catalog;

--
-- TOC entry 11 (OID 16979)
-- Name: ip2domain; Type: TABLE; Schema: public; Owner: mail
--

CREATE TABLE ip2domain (
    ip text NOT NULL,
    "domain" text NOT NULL,
    CONSTRAINT ip2domain_domain CHECK (("domain" <> ''::text)),
    CONSTRAINT ip2domain_ip CHECK ((ip <> ''::text))
);


--
-- TOC entry 12 (OID 16979)
-- Name: ip2domain; Type: ACL; Schema: public; Owner: mail
--

REVOKE ALL ON TABLE ip2domain FROM PUBLIC;


--
-- TOC entry 13 (OID 16995)
-- Name: login_banned; Type: TABLE; Schema: public; Owner: mail
--

CREATE TABLE login_banned (
    login text NOT NULL,
    CONSTRAINT login_banned_login CHECK ((login <> ''::text))
);


--
-- TOC entry 14 (OID 16995)
-- Name: login_banned; Type: ACL; Schema: public; Owner: mail
--

REVOKE ALL ON TABLE login_banned FROM PUBLIC;


--
-- TOC entry 118 (OID 17003)
-- Name: dom_add (text); Type: FUNCTION; Schema: public; Owner: mail
--

CREATE FUNCTION dom_add (text) RETURNS boolean
    AS '
declare
    dom alias for $1;
    user text := ''mail'';
begin
    EXECUTE ''create table '' || quote_ident(dom)
            || '' (id serial, login text unique not null check ''
            || ''(char_length(login)!=0), pass text not null, ''
            || ''flags int default 0 not null,''
            || ''ftpdir text,ftpuid int,''
            || ''ftpgid int,''
            || ''ftpsubdir text,''
            || ''date_crt date not null default now(),''
            || '' primary key(id))''; 
    EXECUTE ''create table '' || quote_ident(dom || ''_log'' )
            || '' (id serial, time timestamp not null default now(),''
            || ''TYPE char(1) not null,''
            || ''LOGIN text,'' 
            || ''MSG text,MSG1 text, '' 
            || ''IP text,''
            || ''primary key(id))'';
    EXECUTE ''create table '' || quote_ident(dom || ''_dot'' )
            || '' (id serial, type char(1),''
            || ''value text not null,''
            || ''ext text not null,''
            || ''uid int CONSTRAINT '' || quote_ident(dom || ''_dot_uid_fkey'') 
            || '' REFERENCES '' || quote_ident(dom) 
            || '' (id) ON UPDATE CASCADE ON DELETE CASCADE DEFERRABLE,''
            || ''primary key(id))'';
    EXECUTE ''create index '' || quote_ident(dom || ''_dot_type_idx'')
            || '' ON '' || quote_ident(dom || ''_dot'') || '' (type)'';
    EXECUTE ''GRANT ALL ON '' || quote_ident(dom)
            || '' TO '' || user;
    EXECUTE ''GRANT ALL ON '' || quote_ident(dom || ''_log'')
            || '' TO '' || user;
    EXECUTE ''GRANT ALL ON '' || quote_ident(dom || ''_dot'')
            || '' TO '' || user;
    EXECUTE ''GRANT ALL ON '' || quote_ident(dom || ''_id_seq'')
            || '' TO '' || user;
    EXECUTE ''GRANT ALL ON '' || quote_ident(dom || ''_log_id_seq'')
            || '' TO '' || user;
    EXECUTE ''GRANT ALL ON '' || quote_ident(dom || ''_dot_id_seq'')
            || '' TO '' || user;
    RETURN ''t''::boolean;
end;
'
    LANGUAGE plpgsql;


--
-- TOC entry 119 (OID 17004)
-- Name: dom_rm (text); Type: FUNCTION; Schema: public; Owner: mail
--

CREATE FUNCTION dom_rm (text) RETURNS boolean
    AS '
declare
    dom alias for $1;
begin
    EXECUTE ''drop table '' || quote_ident(dom) || '' CASCADE'';
    EXECUTE ''drop table '' || quote_ident(dom || ''_log'') || '' CASCADE'';
    EXECUTE ''drop table '' || quote_ident(dom || ''_dot'') || '' CASCADE'';
    EXECUTE ''DELETE FROM ip2domain WHERE domain='' || quote_literal(dom);
    RETURN ''t''::boolean;
end;
'
    LANGUAGE plpgsql;


--
-- TOC entry 114 (OID 17005)
-- Name: udot_add (text, text, text, character, text); Type: FUNCTION; Schema: public; Owner: mail
--

CREATE FUNCTION udot_add (text, text, text, character, text) RETURNS integer
    AS '
declare
    dom alias for $1;
    user alias for $2;
    ext alias for $3;
    tp alias for $4;
    val alias for $5;
    nv RECORD;
begin
    FOR nv IN EXECUTE ''SELECT nextval('' || quote_literal(dom 
                || ''_dot_id_seq'') || '')'' LOOP
            
            EXECUTE ''INSERT INTO '' || quote_ident(dom || ''_dot'') 
                || '' (ID,TYPE,VALUE,EXT,UID) VALUES(''
                || quote_literal(nv.nextval) || '',''
                || quote_literal(tp) || '',''
                || quote_literal(val) || '',''
                || quote_literal(user || ''-'' || ext) 
                || '',(SELECT id FROM ''|| quote_ident(dom) 
                ||'' WHERE login='' || quote_literal(user) || '' LIMIT 1))'';

            RETURN nv.nextval;
    END LOOP;
    RAISE EXCEPTION ''????'';
end;
'
    LANGUAGE plpgsql;


--
-- TOC entry 115 (OID 17007)
-- Name: user_exist (text, text); Type: FUNCTION; Schema: public; Owner: mail
--

CREATE FUNCTION user_exist (text, text) RETURNS integer
    AS '
declare
    dom alias for $1;
    user alias for $2;
    domrec RECORD;
begin
    SELECT INTO domrec 1 FROM pg_tables WHERE tablename=dom;
    IF NOT FOUND THEN
        RETURN 2;
    END IF;
    FOR domrec IN EXECUTE ''SELECT 1 FROM '' || quote_ident(dom)
            || '' WHERE login='' || quote_literal(user) LOOP
        RETURN 0;
    END LOOP;
    RETURN 1;
end;
'
    LANGUAGE plpgsql;


--
-- TOC entry 116 (OID 17008)
-- Name: user_pass (text, text, text); Type: FUNCTION; Schema: public; Owner: mail
--

CREATE FUNCTION user_pass (text, text, text) RETURNS boolean
    AS '
declare
    user alias for $1;
    dom alias for $2;
    pass alias for $3;
begin
    EXECUTE ''update '' || quote_ident(dom) || '' set pass='' 
            || quote_literal(pass) || '' where login=''
            || quote_literal(user);
    RETURN ''t''::boolean;
end;
'
    LANGUAGE plpgsql;


--
-- TOC entry 117 (OID 17009)
-- Name: user_rm (text, text); Type: FUNCTION; Schema: public; Owner: mail
--

CREATE FUNCTION user_rm (text, text) RETURNS boolean
    AS '
declare
    user alias for $1;
    dom alias for $2;
begin
    EXECUTE ''delete from '' || quote_ident(dom || ''_log'' )
            || '' where login ILIKE '' || quote_literal(user);
    EXECUTE ''delete from '' || quote_ident(dom)
            || '' where login='' || quote_literal(user);
    RETURN ''t''::boolean;
end;
'
    LANGUAGE plpgsql;


--
-- TOC entry 15 (OID 17027)
-- Name: log; Type: TABLE; Schema: public; Owner: mail
--

CREATE TABLE log (
    "type" character(1) NOT NULL,
    msg text DEFAULT '' NOT NULL,
    ip text DEFAULT '' NOT NULL,
    "domain" text DEFAULT '' NOT NULL,
    login text DEFAULT '' NOT NULL,
    "time" timestamp with time zone DEFAULT now() NOT NULL,
    msg1 text DEFAULT '' NOT NULL
);


--
-- TOC entry 16 (OID 17027)
-- Name: log; Type: ACL; Schema: public; Owner: mail
--

REVOKE ALL ON TABLE log FROM PUBLIC;


--
-- TOC entry 17 (OID 17156)
-- Name: www_adm; Type: TABLE; Schema: public; Owner: mail
--

CREATE TABLE www_adm (
    login text NOT NULL,
    pass text NOT NULL,
    CONSTRAINT www_adm_login CHECK ((login <> ''::text)),
    CONSTRAINT www_adm_pass CHECK ((pass <> ''::text))
);


--
-- TOC entry 18 (OID 17156)
-- Name: www_adm; Type: ACL; Schema: public; Owner: mail
--

REVOKE ALL ON TABLE www_adm FROM PUBLIC;
GRANT SELECT ON TABLE www_adm TO www_adm_sel;


--
-- TOC entry 19 (OID 17199)
-- Name: domains; Type: TABLE; Schema: public; Owner: mail
--

CREATE TABLE domains (
    id serial NOT NULL,
    name text NOT NULL,
    CONSTRAINT domains_name CHECK ((name <> ''::text))
);


--
-- TOC entry 20 (OID 17199)
-- Name: domains; Type: ACL; Schema: public; Owner: mail
--

REVOKE ALL ON TABLE domains FROM PUBLIC;
GRANT SELECT ON TABLE domains TO ui_sel;
GRANT SELECT ON TABLE domains TO dns_select;


--
-- TOC entry 21 (OID 17210)
-- Name: ip; Type: TABLE; Schema: public; Owner: mail
--

CREATE TABLE ip (
    ip inet NOT NULL,
    domain_id text
);


--
-- TOC entry 22 (OID 17245)
-- Name: domains_all; Type: TABLE; Schema: public; Owner: mail
--

CREATE TABLE domains_all (
    id serial NOT NULL,
    name text NOT NULL,
    domain_id integer,
    CONSTRAINT domains_all_name CHECK ((name <> ''::text))
);


--
-- TOC entry 23 (OID 17245)
-- Name: domains_all; Type: ACL; Schema: public; Owner: mail
--

REVOKE ALL ON TABLE domains_all FROM PUBLIC;
GRANT SELECT ON TABLE domains_all TO dns_select;


--
-- TOC entry 24 (OID 17260)
-- Name: domains_prefixes; Type: TABLE; Schema: public; Owner: mail
--

CREATE TABLE domains_prefixes (
    id serial NOT NULL,
    name text NOT NULL,
    dir text NOT NULL,
    domain_id integer,
    CONSTRAINT domains_prefixes_dir CHECK ((dir <> ''::text)),
    CONSTRAINT domains_prefixes_name CHECK ((name <> ''::text))
);


--
-- TOC entry 25 (OID 17260)
-- Name: domains_prefixes; Type: ACL; Schema: public; Owner: mail
--

REVOKE ALL ON TABLE domains_prefixes FROM PUBLIC;
GRANT SELECT ON TABLE domains_prefixes TO dns_select;


\connect - pgsql

SET search_path = public, pg_catalog;

--
-- TOC entry 120 (OID 17278)
-- Name: dump2file (text, text, text, text); Type: FUNCTION; Schema: public; Owner: pgsql
--

CREATE FUNCTION dump2file (text, text, text, text) RETURNS boolean
    AS '/usr/local/pgsql/pgutil.so', 'dump2file'
    LANGUAGE c;


--
-- TOC entry 121 (OID 17279)
-- Name: dump2file3 (text, text, text); Type: FUNCTION; Schema: public; Owner: pgsql
--

CREATE FUNCTION dump2file3 (text, text, text) RETURNS boolean
    AS '/usr/local/pgsql/pgutil.so', 'dump2file3'
    LANGUAGE c;


\connect - mail

SET search_path = public, pg_catalog;

--
-- TOC entry 122 (OID 17280)
-- Name: domains_dump (text, text); Type: FUNCTION; Schema: public; Owner: mail
--

CREATE FUNCTION domains_dump (text, text) RETURNS boolean
    AS '
declare
    sep alias for $1;
    file alias for $2;
begin
    PERFORM dump2file3(''select d.name,ip.ip from domains as d,ip where d.id=ip.domain_id'',sep,file);
    return ''t''::boolean;
end;
'
    LANGUAGE plpgsql;


--
-- TOC entry 123 (OID 17281)
-- Name: domains_dump (); Type: FUNCTION; Schema: public; Owner: mail
--

CREATE FUNCTION domains_dump () RETURNS boolean
    AS '
    SELECT domains_dump('' '',''/etc/pgsql/domains'');
'
    LANGUAGE sql;


--
-- TOC entry 124 (OID 17282)
-- Name: domains_all_dump (text, text); Type: FUNCTION; Schema: public; Owner: mail
--

CREATE FUNCTION domains_all_dump (text, text) RETURNS boolean
    AS '
declare
    sep alias for $1;
    file alias for $2;
begin
    PERFORM dump2file3(''select dw.name,ip.ip from domains_all as dw,ip,domains as d where dw.domain_id=ip.domain_id AND d.id=dw.domain_id AND dw.name!=d.name;'',sep,file);
    return ''t''::boolean;
end;
'
    LANGUAGE plpgsql;


--
-- TOC entry 125 (OID 17283)
-- Name: domains_all_dump (); Type: FUNCTION; Schema: public; Owner: mail
--

CREATE FUNCTION domains_all_dump () RETURNS boolean
    AS '
    SELECT domains_all_dump('' '',''/etc/pgsql/domains_all'');
'
    LANGUAGE sql;


--
-- TOC entry 126 (OID 17284)
-- Name: domains_prefixes_dump (text, text); Type: FUNCTION; Schema: public; Owner: mail
--

CREATE FUNCTION domains_prefixes_dump (text, text) RETURNS boolean
    AS '
declare
    sep alias for $1;
    file alias for $2;
begin
    PERFORM dump2file3(''select dp.name || \\''.\\'' || dw.name,ip.ip from domains_prefixes as dp,domains_all as dw,ip where dp.domain_id=dw.id and dw.domain_id=ip.domain_id;'',sep,file);
    return ''t''::boolean;
end;
'
    LANGUAGE plpgsql;


--
-- TOC entry 127 (OID 17285)
-- Name: domains_prefixes_dump (); Type: FUNCTION; Schema: public; Owner: mail
--

CREATE FUNCTION domains_prefixes_dump () RETURNS boolean
    AS '
    SELECT domains_prefixes_dump('' '',''/etc/pgsql/domains_prefixes'');
'
    LANGUAGE sql;


--
-- TOC entry 26 (OID 17288)
-- Name: nameservers; Type: TABLE; Schema: public; Owner: mail
--

CREATE TABLE nameservers (
    id serial NOT NULL,
    name text NOT NULL,
    ip text NOT NULL,
    CONSTRAINT nameservers_ip CHECK ((ip <> ''::text)),
    CONSTRAINT nameservers_name CHECK ((name <> ''::text))
);


--
-- TOC entry 128 (OID 17303)
-- Name: nameservers_dump (text, text); Type: FUNCTION; Schema: public; Owner: mail
--

CREATE FUNCTION nameservers_dump (text, text) RETURNS boolean
    AS '
declare
    sep alias for $1;
    file alias for $2;
begin
    PERFORM dump2file3(''select name,ip from nameservers'',sep,file);
    return ''t''::boolean;
end;
'
    LANGUAGE plpgsql;


--
-- TOC entry 129 (OID 17304)
-- Name: nameservers_dump (); Type: FUNCTION; Schema: public; Owner: mail
--

CREATE FUNCTION nameservers_dump () RETURNS boolean
    AS '
    SELECT nameservers_dump('' '',''/etc/pgsql/nameservers'');
'
    LANGUAGE sql;


--
-- TOC entry 27 (OID 17398)
-- Name: users_info_edu; Type: TABLE; Schema: public; Owner: mail
--

CREATE TABLE users_info_edu (
    id serial NOT NULL,
    name text NOT NULL,
    locale text NOT NULL,
    code integer NOT NULL,
    CONSTRAINT users_info_edu_locale CHECK ((locale <> ''::text)),
    CONSTRAINT users_info_edu_name CHECK ((name <> ''::text))
);


--
-- TOC entry 28 (OID 17398)
-- Name: users_info_edu; Type: ACL; Schema: public; Owner: mail
--

REVOKE ALL ON TABLE users_info_edu FROM PUBLIC;
GRANT ALL ON TABLE users_info_edu TO ui_sel;


--
-- TOC entry 29 (OID 17410)
-- Name: users_info_interests; Type: TABLE; Schema: public; Owner: mail
--

CREATE TABLE users_info_interests (
    id serial NOT NULL,
    name text NOT NULL,
    locale text NOT NULL,
    code integer NOT NULL,
    CONSTRAINT users_info_interests_locale CHECK ((locale <> ''::text)),
    CONSTRAINT users_info_interests_name CHECK ((name <> ''::text))
);


--
-- TOC entry 30 (OID 17410)
-- Name: users_info_interests; Type: ACL; Schema: public; Owner: mail
--

REVOKE ALL ON TABLE users_info_interests FROM PUBLIC;
GRANT ALL ON TABLE users_info_interests TO ui_sel;


--
-- TOC entry 31 (OID 17422)
-- Name: users_info_work; Type: TABLE; Schema: public; Owner: mail
--

CREATE TABLE users_info_work (
    id serial NOT NULL,
    name text NOT NULL,
    locale text NOT NULL,
    code integer NOT NULL,
    CONSTRAINT users_info_work_locale CHECK ((locale <> ''::text)),
    CONSTRAINT users_info_work_name CHECK ((name <> ''::text))
);


--
-- TOC entry 32 (OID 17422)
-- Name: users_info_work; Type: ACL; Schema: public; Owner: mail
--

REVOKE ALL ON TABLE users_info_work FROM PUBLIC;
GRANT ALL ON TABLE users_info_work TO ui_sel;


--
-- TOC entry 33 (OID 17437)
-- Name: users_info; Type: TABLE; Schema: public; Owner: mail
--

CREATE TABLE users_info (
    id serial NOT NULL,
    login text NOT NULL,
    domain_id integer,
    country text NOT NULL,
    city text DEFAULT '' NOT NULL,
    birthday date NOT NULL,
    re_question text DEFAULT '' NOT NULL,
    re_answer text DEFAULT '' NOT NULL,
    country_up text DEFAULT '' NOT NULL,
    locale text DEFAULT 'pl_PL' NOT NULL,
    city_up text DEFAULT '' NOT NULL,
    CONSTRAINT users_info_country CHECK ((country <> ''::text)),
    CONSTRAINT users_info_login CHECK ((login <> ''::text))
);


--
-- TOC entry 34 (OID 17437)
-- Name: users_info; Type: ACL; Schema: public; Owner: mail
--

REVOKE ALL ON TABLE users_info FROM PUBLIC;
GRANT ALL ON TABLE users_info TO ui_sel;


--
-- TOC entry 63 (OID 17437)
-- Name: users_info_id_seq; Type: ACL; Schema: public; Owner: mail
--

REVOKE ALL ON TABLE users_info_id_seq FROM PUBLIC;
GRANT ALL ON TABLE users_info_id_seq TO ui_sel;


--
-- TOC entry 35 (OID 17455)
-- Name: users_info_map_edu; Type: TABLE; Schema: public; Owner: mail
--

CREATE TABLE users_info_map_edu (
    user_id integer NOT NULL,
    val_id integer NOT NULL
);


--
-- TOC entry 36 (OID 17455)
-- Name: users_info_map_edu; Type: ACL; Schema: public; Owner: mail
--

REVOKE ALL ON TABLE users_info_map_edu FROM PUBLIC;
GRANT ALL ON TABLE users_info_map_edu TO ui_sel;


--
-- TOC entry 37 (OID 17467)
-- Name: users_info_map_work; Type: TABLE; Schema: public; Owner: mail
--

CREATE TABLE users_info_map_work (
    user_id integer NOT NULL,
    val_id integer NOT NULL
);


--
-- TOC entry 38 (OID 17467)
-- Name: users_info_map_work; Type: ACL; Schema: public; Owner: mail
--

REVOKE ALL ON TABLE users_info_map_work FROM PUBLIC;
GRANT ALL ON TABLE users_info_map_work TO ui_sel;


--
-- TOC entry 39 (OID 17479)
-- Name: users_info_map_ints; Type: TABLE; Schema: public; Owner: mail
--

CREATE TABLE users_info_map_ints (
    user_id integer NOT NULL,
    val_id integer NOT NULL
);


--
-- TOC entry 40 (OID 17479)
-- Name: users_info_map_ints; Type: ACL; Schema: public; Owner: mail
--

REVOKE ALL ON TABLE users_info_map_ints FROM PUBLIC;
GRANT ALL ON TABLE users_info_map_ints TO ui_sel;


--
-- TOC entry 41 (OID 17505)
-- Name: users_info_areas; Type: TABLE; Schema: public; Owner: mail
--

CREATE TABLE users_info_areas (
    id serial NOT NULL,
    name text NOT NULL,
    locale text NOT NULL,
    code integer NOT NULL,
    CONSTRAINT users_info_areas_locale CHECK ((locale <> ''::text)),
    CONSTRAINT users_info_areas_name CHECK ((name <> ''::text))
);


--
-- TOC entry 42 (OID 17505)
-- Name: users_info_areas; Type: ACL; Schema: public; Owner: mail
--

REVOKE ALL ON TABLE users_info_areas FROM PUBLIC;
GRANT SELECT ON TABLE users_info_areas TO ui_sel;


--
-- TOC entry 43 (OID 17516)
-- Name: users_info_map_area; Type: TABLE; Schema: public; Owner: mail
--

CREATE TABLE users_info_map_area (
    user_id integer NOT NULL,
    val_id integer NOT NULL
);


--
-- TOC entry 44 (OID 17516)
-- Name: users_info_map_area; Type: ACL; Schema: public; Owner: mail
--

REVOKE ALL ON TABLE users_info_map_area FROM PUBLIC;
GRANT ALL ON TABLE users_info_map_area TO ui_sel;


--
-- TOC entry 45 (OID 17552)
-- Name: users_info_sexes; Type: TABLE; Schema: public; Owner: mail
--

CREATE TABLE users_info_sexes (
    id serial NOT NULL,
    code character(1) NOT NULL,
    name text NOT NULL,
    locale text NOT NULL,
    CONSTRAINT users_info_sex_code CHECK (((code = 'f'::bpchar) OR (code = 'm'::bpchar))),
    CONSTRAINT users_info_sex_name CHECK ((name <> ''::text))
);


--
-- TOC entry 46 (OID 17552)
-- Name: users_info_sexes; Type: ACL; Schema: public; Owner: mail
--

REVOKE ALL ON TABLE users_info_sexes FROM PUBLIC;
GRANT SELECT ON TABLE users_info_sexes TO ui_sel;


--
-- TOC entry 47 (OID 17565)
-- Name: users_info_map_sex; Type: TABLE; Schema: public; Owner: mail
--

CREATE TABLE users_info_map_sex (
    user_id integer NOT NULL,
    val_id integer NOT NULL
);


--
-- TOC entry 48 (OID 17565)
-- Name: users_info_map_sex; Type: ACL; Schema: public; Owner: mail
--

REVOKE ALL ON TABLE users_info_map_sex FROM PUBLIC;
GRANT ALL ON TABLE users_info_map_sex TO ui_sel;


--
-- TOC entry 130 (OID 17591)
-- Name: users_info_country_up_set (); Type: FUNCTION; Schema: public; Owner: mail
--

CREATE FUNCTION users_info_country_up_set () RETURNS "trigger"
    AS '
begin
new.country_up:=upper(new.country);
return new;
end;
'
    LANGUAGE plpgsql;


--
-- TOC entry 131 (OID 17646)
-- Name: users_info_ints_uid_loc (integer, text); Type: FUNCTION; Schema: public; Owner: mail
--

CREATE FUNCTION users_info_ints_uid_loc (integer, text) RETURNS SETOF integer
    AS '
declare
uid alias for $1;
loc alias for $2;
map_ids record;
val_id record;
begin
for map_ids in select val_id from users_info_map_ints where user_id=uid loop
for val_id in select id from users_info_interests where (code=(select code from users_info_interests where id=map_ids.val_id) and locale=loc) or id=map_ids.val_id loop
return next val_id.id;
end loop;
end loop;
return;
end;
'
    LANGUAGE plpgsql;


--
-- TOC entry 132 (OID 17655)
-- Name: users_info_city_up_set (); Type: FUNCTION; Schema: public; Owner: mail
--

CREATE FUNCTION users_info_city_up_set () RETURNS "trigger"
    AS '
begin
new.city_up:=upper(new.city);
return new;
end;
'
    LANGUAGE plpgsql;


--
-- TOC entry 133 (OID 17661)
-- Name: users_info_re_ans_set (); Type: FUNCTION; Schema: public; Owner: mail
--

CREATE FUNCTION users_info_re_ans_set () RETURNS "trigger"
    AS '
begin
new.re_answer=upper(new.re_answer);
return new;
end;
'
    LANGUAGE plpgsql;


--
-- TOC entry 2 (OID 17664)
-- Name: users_all_seq; Type: SEQUENCE; Schema: public; Owner: mail
--

CREATE SEQUENCE users_all_seq
    START 1
    INCREMENT 1
    MAXVALUE 9223372036854775807
    MINVALUE 1
    CACHE 1;


--
-- TOC entry 4 (OID 17664)
-- Name: users_all_seq; Type: ACL; Schema: public; Owner: mail
--

REVOKE ALL ON TABLE users_all_seq FROM PUBLIC;
GRANT ALL ON TABLE users_all_seq TO ui_sel;


--
-- TOC entry 5 (OID 17666)
-- Name: users_today_add_seq; Type: SEQUENCE; Schema: public; Owner: mail
--

CREATE SEQUENCE users_today_add_seq
    START 1
    INCREMENT 1
    MAXVALUE 9223372036854775807
    MINVALUE 1
    CACHE 1;


--
-- TOC entry 7 (OID 17666)
-- Name: users_today_add_seq; Type: ACL; Schema: public; Owner: mail
--

REVOKE ALL ON TABLE users_today_add_seq FROM PUBLIC;
GRANT ALL ON TABLE users_today_add_seq TO ui_sel;


--
-- TOC entry 8 (OID 17668)
-- Name: users_today_rm_seq; Type: SEQUENCE; Schema: public; Owner: mail
--

CREATE SEQUENCE users_today_rm_seq
    START 1
    INCREMENT 1
    MAXVALUE 9223372036854775807
    MINVALUE 1
    CACHE 1;


--
-- TOC entry 10 (OID 17668)
-- Name: users_today_rm_seq; Type: ACL; Schema: public; Owner: mail
--

REVOKE ALL ON TABLE users_today_rm_seq FROM PUBLIC;
GRANT ALL ON TABLE users_today_rm_seq TO ui_sel;


--
-- TOC entry 134 (OID 17674)
-- Name: user_add (text, text, text, integer); Type: FUNCTION; Schema: public; Owner: mail
--

CREATE FUNCTION user_add (text, text, text, integer) RETURNS integer
    AS '
declare
    user alias for $1;
    dom alias for $2;
    pass alias for $3;
    flags alias for $4;
    users RECORD;
    first int := 4;
    id RECORD;
    ban RECORD;
begin
    IF flags & 1 = 0 THEN
      FOR ban IN SELECT 1 FROM login_banned WHERE login=user LOOP
        RETURN 1;
      END LOOP;
    END IF;
    FOR users IN EXECUTE ''SELECT id FROM '' || quote_ident(dom) 
            || '' WHERE login!=''|| quote_literal(''ftp'') ||'' LIMIT 1'' LOOP
            first := 0;
    END LOOP;
    FOR id IN EXECUTE ''SELECT nextval('' || quote_literal(dom || ''_id_seq'') || '')'' LOOP 
            EXECUTE ''insert into '' || quote_ident(dom) 
                    || '' (id,login,pass,flags) values('' 
                    || quote_literal(id.nextval)
                    || '','' || quote_literal(user) 
                    || '','' || quote_literal(pass) 
                    || '','' || first || '')'';
            EXECUTE ''insert into '' || quote_ident(dom || ''_dot'')
                    || '' (type,value,ext,uid) ''
                    || ''VALUES(''|| quote_literal(''M'') 
                    || '','' || quote_literal(''./'' || user || ''/Maildir/'') 
                    || '','' || quote_literal(user || ''-'')  
                    ||'','' || quote_literal(id.nextval) || '')'';
            RETURN 0;
    END LOOP;
    RAISE EXCEPTION ''select nextval'';
end;
'
    LANGUAGE plpgsql;


--
-- TOC entry 49 (OID 17818)
-- Name: foo_iarp_pl; Type: TABLE; Schema: public; Owner: mail
--

CREATE TABLE foo_iarp_pl (
    id serial NOT NULL,
    login text NOT NULL,
    pass text NOT NULL,
    flags integer DEFAULT 0 NOT NULL,
    ftpdir text,
    ftpuid integer,
    ftpgid integer,
    ftpsubdir text,
    date_crt date DEFAULT now() NOT NULL,
    CONSTRAINT foo_iarp_pl_login CHECK ((char_length(login) <> 0))
);


--
-- TOC entry 50 (OID 17818)
-- Name: foo_iarp_pl; Type: ACL; Schema: public; Owner: mail
--

REVOKE ALL ON TABLE foo_iarp_pl FROM PUBLIC;
GRANT SELECT ON TABLE foo_iarp_pl TO ui_sel;


--
-- TOC entry 67 (OID 17818)
-- Name: foo_iarp_pl_id_seq; Type: ACL; Schema: public; Owner: mail
--

REVOKE ALL ON TABLE foo_iarp_pl_id_seq FROM PUBLIC;


--
-- TOC entry 51 (OID 17833)
-- Name: foo_iarp_pl_log; Type: TABLE; Schema: public; Owner: mail
--

CREATE TABLE foo_iarp_pl_log (
    id serial NOT NULL,
    "time" timestamp without time zone DEFAULT now() NOT NULL,
    "type" character(1) NOT NULL,
    login text,
    msg text,
    msg1 text,
    ip text
);


--
-- TOC entry 52 (OID 17833)
-- Name: foo_iarp_pl_log; Type: ACL; Schema: public; Owner: mail
--

REVOKE ALL ON TABLE foo_iarp_pl_log FROM PUBLIC;


--
-- TOC entry 69 (OID 17833)
-- Name: foo_iarp_pl_log_id_seq; Type: ACL; Schema: public; Owner: mail
--

REVOKE ALL ON TABLE foo_iarp_pl_log_id_seq FROM PUBLIC;


--
-- TOC entry 53 (OID 17844)
-- Name: foo_iarp_pl_dot; Type: TABLE; Schema: public; Owner: mail
--

CREATE TABLE foo_iarp_pl_dot (
    id serial NOT NULL,
    "type" character(1),
    value text NOT NULL,
    ext text NOT NULL,
    uid integer
);


--
-- TOC entry 54 (OID 17844)
-- Name: foo_iarp_pl_dot; Type: ACL; Schema: public; Owner: mail
--

REVOKE ALL ON TABLE foo_iarp_pl_dot FROM PUBLIC;


--
-- TOC entry 71 (OID 17844)
-- Name: foo_iarp_pl_dot_id_seq; Type: ACL; Schema: public; Owner: mail
--

REVOKE ALL ON TABLE foo_iarp_pl_dot_id_seq FROM PUBLIC;


--
-- Data for TOC entry 135 (OID 16979)
-- Name: ip2domain; Type: TABLE DATA; Schema: public; Owner: mail
--

COPY ip2domain (ip, "domain") FROM stdin;
\.


--
-- Data for TOC entry 136 (OID 16995)
-- Name: login_banned; Type: TABLE DATA; Schema: public; Owner: mail
--

COPY login_banned (login) FROM stdin;
root
toor
admin
administrator
biuro
webmaster
postmaster
sex
\.


--
-- Data for TOC entry 137 (OID 17027)
-- Name: log; Type: TABLE DATA; Schema: public; Owner: mail
--

COPY log ("type", msg, ip, "domain", login, "time", msg1) FROM stdin;
\.


--
-- Data for TOC entry 138 (OID 17156)
-- Name: www_adm; Type: TABLE DATA; Schema: public; Owner: mail
--

COPY www_adm (login, pass) FROM stdin;
new	asdasd
roberts	@#dsdf24
\.


--
-- Data for TOC entry 139 (OID 17199)
-- Name: domains; Type: TABLE DATA; Schema: public; Owner: mail
--

COPY domains (id, name) FROM stdin;
1	foo.iarp.pl
\.


--
-- Data for TOC entry 140 (OID 17210)
-- Name: ip; Type: TABLE DATA; Schema: public; Owner: mail
--

COPY ip (ip, domain_id) FROM stdin;
192.168.174.62	1
\.


--
-- Data for TOC entry 141 (OID 17245)
-- Name: domains_all; Type: TABLE DATA; Schema: public; Owner: mail
--

COPY domains_all (id, name, domain_id) FROM stdin;
1	foo.iarp.pl	1
\.


--
-- Data for TOC entry 142 (OID 17260)
-- Name: domains_prefixes; Type: TABLE DATA; Schema: public; Owner: mail
--

COPY domains_prefixes (id, name, dir, domain_id) FROM stdin;
1	www	/	1
2	smtp	/	1
3	pop3	/	1
\.


--
-- Data for TOC entry 143 (OID 17288)
-- Name: nameservers; Type: TABLE DATA; Schema: public; Owner: mail
--

COPY nameservers (id, name, ip) FROM stdin;
1	a.ns.foo.iarp.pl	192.168.174.62
\.


--
-- Data for TOC entry 144 (OID 17398)
-- Name: users_info_edu; Type: TABLE DATA; Schema: public; Owner: mail
--

COPY users_info_edu (id, name, locale, code) FROM stdin;
1	podstawowe	pl_PL	1
2	średnie	pl_PL	2
4	policealne	pl_PL	3
5	wyższe	pl_PL	4
6	wyższe-magisterskie	pl_PL	5
\.


--
-- Data for TOC entry 145 (OID 17410)
-- Name: users_info_interests; Type: TABLE DATA; Schema: public; Owner: mail
--

COPY users_info_interests (id, name, locale, code) FROM stdin;
2	Aktualności (newsy)	pl_PL	3
1	AGD, RTV i elektronika	pl_PL	2
3	Biznes i gospodarka	pl_PL	4
6	Edukacja, nauka, szkolnictwo	pl_PL	5
8	Budownictwo, nieruchomości	pl_PL	6
9	Fantastyka	pl_PL	7
10	Fauna i flora	pl_PL	8
11	Film i kino	pl_PL	9
12	Firmy (produkcja, handel, us3ugi)	pl_PL	10
13	Fotografia	pl_PL	11
14	Gry komputerowe	pl_PL	12
15	Komputery i Internet	pl_PL	13
16	Kuchnia (kulinaria)	pl_PL	14
17	Kultura, sztuka, rozrywka, teatr	pl_PL	15
18	Majsterkowanie i modelarstwo	pl_PL	16
19	Media (prasa, radio, TV)	pl_PL	18
20	Medycyna i zdrowie	pl_PL	19
21	Moda	pl_PL	20
22	Motoryzacja	pl_PL	21
23	Muzyka	pl_PL	22
24	Praca i doradztwo personalne	pl_PL	23
25	Polityka i życie publiczne	pl_PL	24
26	Prawo, podatki i ubezpieczenia	pl_PL	25
27	Prognoza pogody	pl_PL	26
28	Sport	pl_PL	27
29	Telekomunikacja	pl_PL	28
30	Tematy rodzinne	pl_PL	29
31	Turystyka i wypoczynek	pl_PL	30
32	Uroda i kosmetyka	pl_PL	31
33	Zakupy przez Internet	pl_PL	32
\.


--
-- Data for TOC entry 146 (OID 17422)
-- Name: users_info_work; Type: TABLE DATA; Schema: public; Owner: mail
--

COPY users_info_work (id, name, locale, code) FROM stdin;
5	Prawo i administracja	pl_PL	5
6	Ekonomia	pl_PL	6
1	IT	pl_PL	2
8	Turystyka	pl_PL	8
9	Szkolnictwo	pl_PL	9
11	Budownictwo	pl_PL	14
13	Student	pl_PL	11
2	Inne	pl_PL	3
16	Nie pracuję 	pl_PL	15
17	Uczeń	pl_PL	12
7	Usługi	pl_PL	7
\.


--
-- Data for TOC entry 147 (OID 17437)
-- Name: users_info; Type: TABLE DATA; Schema: public; Owner: mail
--

COPY users_info (id, login, domain_id, country, city, birthday, re_question, re_answer, country_up, locale, city_up) FROM stdin;
36	th!	1	Polska	Koszalin	1980-11-19	gdzie robisz zakupy?	W BIEDRONCE, ONA JEST TAK BLISKO...	POLSKA	pl_PL	KOSZALIN
37	ciwe	1	Polska	Koszalin	1945-02-01	co?	JA	POLSKA	pl_PL	KOSZALIN
33	new	1	polska	koszalin	1980-01-01	adsasd	ASDSAD	POLSKA	pl_PL	KOSZALIN
44	zxc	1	zxczxc	zxczxc	1980-12-22	zxc	ZXC	ZXCZXC	pl_PL	ZXCZXC
43	jogi	1	polska	szczecinek	1977-03-12	ile mam lat	26	POLSKA	pl_PL	SZCZECINEK
42	new.test	1	asdasd	asdasd	1980-12-12	asd	ASD	ASDASD	pl_PL	ASDASD
30	kasia	1	polska	szczecinek	1977-03-12	straz graniczna	MACIEK	POLSKA	pl_PL	SZCZECINEK
31	new:pcol	1	polska	koszalin	1980-11-11	pcol?	PCOL	POLSKA	pl_PL	KOSZALIN
32	new:bugtraq	1	polska	koszalin	1980-11-11	pcol?	PCOL	POLSKA	pl_PL	KOSZALIN
34	tomek	1	polska	koszalin	1983-01-01	Piesio?	LORD	POLSKA	pl_PL	KOSZALIN
35	new:opencm	1	polska	koszalin	1980-02-01	fantastik	F	POLSKA	pl_PL	KOSZALIN
\.


--
-- Data for TOC entry 148 (OID 17455)
-- Name: users_info_map_edu; Type: TABLE DATA; Schema: public; Owner: mail
--

COPY users_info_map_edu (user_id, val_id) FROM stdin;
36	2
30	2
31	5
37	1
32	5
33	5
34	2
44	6
43	2
35	5
42	1
\.


--
-- Data for TOC entry 149 (OID 17467)
-- Name: users_info_map_work; Type: TABLE DATA; Schema: public; Owner: mail
--

COPY users_info_map_work (user_id, val_id) FROM stdin;
36	1
30	2
31	5
37	16
32	5
33	1
34	13
44	2
43	8
35	1
42	11
\.


--
-- Data for TOC entry 150 (OID 17479)
-- Name: users_info_map_ints; Type: TABLE DATA; Schema: public; Owner: mail
--

COPY users_info_map_ints (user_id, val_id) FROM stdin;
36	29
36	15
30	20
31	2
36	11
36	9
31	1
32	2
32	1
37	15
34	14
34	12
37	14
33	17
33	15
44	1
42	2
35	9
43	10
43	9
43	6
43	8
43	3
43	2
43	1
\.


--
-- Data for TOC entry 151 (OID 17505)
-- Name: users_info_areas; Type: TABLE DATA; Schema: public; Owner: mail
--

COPY users_info_areas (id, name, locale, code) FROM stdin;
1	dolnośląskie	pl_PL	2
2	kujawsko-pomorskie	pl_PL	3
3	lubelskie	pl_PL	4
4	lubuskie	pl_PL	5
5	łódzkie	pl_PL	6
6	małopolskie	pl_PL	7
7	mazowieckie	pl_PL	8
8	opolskie	pl_PL	9
9	podkarpackie	pl_PL	10
10	podlaskie	pl_PL	11
11	pomorskie	pl_PL	12
12	śląskie	pl_PL	13
13	świętokrzyskie	pl_PL	14
14	warmińsko-mazurskie	pl_PL	15
15	wielkopolskie	pl_PL	16
16	zachodniopomorskie	pl_PL	17
\.


--
-- Data for TOC entry 152 (OID 17516)
-- Name: users_info_map_area; Type: TABLE DATA; Schema: public; Owner: mail
--

COPY users_info_map_area (user_id, val_id) FROM stdin;
36	16
37	16
30	16
33	16
31	15
32	15
44	1
34	16
43	16
42	1
35	16
\.


--
-- Data for TOC entry 153 (OID 17552)
-- Name: users_info_sexes; Type: TABLE DATA; Schema: public; Owner: mail
--

COPY users_info_sexes (id, code, name, locale) FROM stdin;
1	f	Kobieta	pl_PL
2	m	Mężczyzna	pl_PL
\.


--
-- Data for TOC entry 154 (OID 17565)
-- Name: users_info_map_sex; Type: TABLE DATA; Schema: public; Owner: mail
--

COPY users_info_map_sex (user_id, val_id) FROM stdin;
36	2
30	1
31	1
37	2
32	1
33	2
34	2
44	1
43	1
35	2
42	1
\.


--
-- Data for TOC entry 155 (OID 17818)
-- Name: foo_iarp_pl; Type: TABLE DATA; Schema: public; Owner: mail
--

COPY foo_iarp_pl (id, login, pass, flags, ftpdir, ftpuid, ftpgid, ftpsubdir, date_crt) FROM stdin;
18	root	sdfEWsd	0	\N	\N	\N	\N	2003-05-06
19	th!	123456	0	\N	\N	\N	\N	2003-05-10
20	ciwe	ciwe12	0	\N	\N	\N	\N	2003-05-10
21	new.test	abc123	0	\N	\N	\N	\N	2003-07-10
22	jogi	misiugruby	0	\N	\N	\N	\N	2003-08-24
9	kasia	maciek	0	\N	\N	\N	\N	2003-05-06
10	new:pcol	0lewka	0	\N	\N	\N	\N	2003-05-06
11	new:bugtraq	0lewka	0	\N	\N	\N	\N	2003-05-06
12	tomek	abc123	0	\N	\N	\N	\N	2003-05-06
13	new	0lewka	0	\N	\N	\N	\N	2003-05-06
14	abuse	0lewka	0	\N	\N	\N	\N	2003-05-06
15	postmaster	0lewka	0	\N	\N	\N	\N	2003-05-06
16	admin	0lewka	0	\N	\N	\N	\N	2003-05-06
17	new:opencm	0lewka	0	\N	\N	\N	\N	2003-05-06
\.


--
-- Data for TOC entry 156 (OID 17833)
-- Name: foo_iarp_pl_log; Type: TABLE DATA; Schema: public; Owner: mail
--

COPY foo_iarp_pl_log (id, "time", "type", login, msg, msg1, ip) FROM stdin;
14	2003-05-06 12:00:32.39923	N	new	SMTP login successful	\N	217.153.109.154
15	2003-05-06 12:03:03.214218	N	new	SMTP login successful	\N	217.153.109.154
16	2003-05-06 12:03:56.625862	N	new	POP3 login successful	\N	192.168.174.62
17	2003-05-06 12:19:14.954543	N	new	POP3 login successful	\N	192.168.174.62
18	2003-05-06 12:34:17.947361	N	new	POP3 login successful	\N	192.168.174.62
19	2003-05-06 12:38:30.872543	N	new	SMTP login successful	\N	217.153.109.154
20	2003-05-06 12:41:48.404556	N	new:opencm	POP3 login successful	\N	
21	2003-05-06 15:17:14.41775	N	new:opencm	POP3 login successful	\N	
22	2003-05-06 17:37:16.135827	N	admin	POP3 login successful	\N	192.168.174.62
23	2003-05-06 17:37:31.477819	N	postmaster	POP3 login successful	\N	192.168.174.62
24	2003-05-06 17:37:39.700014	N	abuse	POP3 login successful	\N	192.168.174.62
25	2003-05-06 17:37:47.585904	E	root	User not found;AUTH= (0lewka)	\N	192.168.174.62
26	2003-05-07 08:53:04.5169	N	new:opencm	POP3 login successful	\N	
27	2003-05-07 08:55:51.229439	N	new	POP3 login successful	\N	
28	2003-05-07 11:19:33.373229	N	new:opencm	POP3 login successful	\N	
29	2003-05-07 12:53:45.075098	N	new	POP3 login successful	\N	
30	2003-05-07 12:54:11.063216	N	new	POP3 login successful	\N	
31	2003-05-08 21:07:22.212832	N	new:pcol	POP3 login successful	\N	
32	2003-05-10 14:44:22.963419	N	th!	POP3 login successful	\N	
33	2003-05-10 14:44:57.103775	N	th!	POP3 login successful	\N	
34	2003-05-10 14:45:35.357749	N	th!	POP3 login successful	\N	
35	2003-05-10 15:48:08.955641	N	ciwe	POP3 login successful	\N	
36	2003-05-10 16:54:49.7935	N	ciwe	POP3 login successful	\N	
37	2003-05-21 20:01:25.719056	N	new:pcol	POP3 login successful	\N	
38	2003-05-27 20:53:19.30046	N	new	POP3 login successful	\N	192.168.174.62
39	2003-05-31 17:17:53.747894	N	new	POP3 login successful	\N	
40	2003-06-01 14:00:40.692604	N	new	POP3 login successful	\N	192.168.174.1
41	2003-06-01 14:02:48.262497	N	new	POP3 login successful	\N	
42	2003-06-01 14:08:03.554593	N	new	POP3 login successful	\N	
43	2003-06-29 15:53:46.142711	N	new	POP3 login successful	\N	192.168.174.62
44	2003-07-20 17:35:50.242958	N	new	POP3 login successful	\N	192.168.174.62
45	2003-07-20 17:43:42.587817	N	new	POP3 login successful	\N	
46	2003-08-08 21:55:07.386422	N	new	POP3 login successful	\N	
47	2003-08-11 20:08:08.689241	N	ciwe	POP3 login successful	\N	
48	2003-08-14 18:39:49.777258	N	new	POP3 login successful	\N	
49	2003-08-23 00:35:32.173535	N	ciwe	POP3 login successful	\N	
50	2003-08-24 15:50:55.593529	N	jogi	POP3 login successful	\N	
51	2003-08-24 15:55:38.528545	N	jogi	POP3 login successful	\N	
52	2003-08-24 16:00:37.007922	N	jogi	POP3 login successful	\N	
53	2003-08-24 16:05:50.530709	E	new	invalid login (pass=4lldseri;resp=)	\N	
54	2003-08-24 16:05:57.516584	E	new	invalid login (pass=4lldseri;resp=)	\N	
55	2003-08-24 16:06:36.941383	N	new	POP3 login successful	\N	
56	2003-08-24 16:09:17.711659	N	new	POP3 login successful	\N	192.168.174.62
57	2003-08-24 16:09:57.541454	N	new	POP3 login successful	\N	192.168.174.62
59	2003-08-30 13:41:25.751264	N	new	POP3 login successful	\N	192.168.174.1
60	2003-08-30 13:43:25.043717	N	new	POP3 login successful	\N	192.168.174.62
61	2003-08-30 19:00:04.193971	N	new	POP3 login successful	\N	192.168.174.62
62	2003-08-30 19:00:16.638763	N	new	POP3 login successful	\N	192.168.174.62
\.


--
-- Data for TOC entry 157 (OID 17844)
-- Name: foo_iarp_pl_dot; Type: TABLE DATA; Schema: public; Owner: mail
--

COPY foo_iarp_pl_dot (id, "type", value, ext, uid) FROM stdin;
10	F	new@foo.iarp.pl	admin-	16
11	F	new@foo.iarp.pl	abuse-	14
12	F	new@foo.iarp.pl	postmaster-	15
15	M	./th!/Maildir/	th!-	19
14	F	new@foo.iarp.pl	root-	18
16	M	./ciwe/Maildir/	ciwe-	20
31	M	./new.test/Maildir/	new.test-	21
9	M	./new:opencm/Maildir/	new:opencm-	17
35	F	new@slesz.pl	jogi-	22
36	R	49534f2d383835392d320073706164616a20676c75706961206d616c706f2069206e6965207069737a20646f206d6e69652077696563656a20447a69ea6b69	jogi-	22
33	R	49534f2d383835392d320061737a7ab36bf3bfbce6	new-	13
30	M	./new/Maildir/	new-	13
\.


--
-- TOC entry 80 (OID 17274)
-- Name: domains_prefixes_name_domain_id_idx; Type: INDEX; Schema: public; Owner: mail
--

CREATE UNIQUE INDEX domains_prefixes_name_domain_id_idx ON domains_prefixes USING btree (name, domain_id);


--
-- TOC entry 78 (OID 17275)
-- Name: domains_all_name_idx; Type: INDEX; Schema: public; Owner: mail
--

CREATE UNIQUE INDEX domains_all_name_idx ON domains_all USING btree (name);


--
-- TOC entry 91 (OID 17454)
-- Name: users_info_login_domain_id_idx; Type: INDEX; Schema: public; Owner: mail
--

CREATE UNIQUE INDEX users_info_login_domain_id_idx ON users_info USING btree (login, domain_id);


--
-- TOC entry 85 (OID 17546)
-- Name: users_info_edu_code_locale_idx; Type: INDEX; Schema: public; Owner: mail
--

CREATE UNIQUE INDEX users_info_edu_code_locale_idx ON users_info_edu USING btree (code, locale);


--
-- TOC entry 99 (OID 17547)
-- Name: users_info_areas_code_locale_idx; Type: INDEX; Schema: public; Owner: mail
--

CREATE UNIQUE INDEX users_info_areas_code_locale_idx ON users_info_areas USING btree (code, locale);


--
-- TOC entry 87 (OID 17548)
-- Name: users_info_interests_code_locale_idx; Type: INDEX; Schema: public; Owner: mail
--

CREATE UNIQUE INDEX users_info_interests_code_locale_idx ON users_info_interests USING btree (code, locale);


--
-- TOC entry 89 (OID 17549)
-- Name: users_info_work_code_locale_idx; Type: INDEX; Schema: public; Owner: mail
--

CREATE UNIQUE INDEX users_info_work_code_locale_idx ON users_info_work USING btree (code, locale);


--
-- TOC entry 104 (OID 17562)
-- Name: users_info_sexes_code_locale_idx; Type: INDEX; Schema: public; Owner: mail
--

CREATE UNIQUE INDEX users_info_sexes_code_locale_idx ON users_info_sexes USING btree (code, locale);


--
-- TOC entry 94 (OID 17649)
-- Name: users_info_map_edu_uid_idx; Type: INDEX; Schema: public; Owner: mail
--

CREATE INDEX users_info_map_edu_uid_idx ON users_info_map_edu USING btree (user_id);


--
-- TOC entry 96 (OID 17650)
-- Name: users_info_map_work_uid_idx; Type: INDEX; Schema: public; Owner: mail
--

CREATE INDEX users_info_map_work_uid_idx ON users_info_map_work USING btree (user_id);


--
-- TOC entry 102 (OID 17651)
-- Name: users_info_map_area_uid_idx; Type: INDEX; Schema: public; Owner: mail
--

CREATE INDEX users_info_map_area_uid_idx ON users_info_map_area USING btree (user_id);


--
-- TOC entry 106 (OID 17652)
-- Name: users_info_map_sex_uid_idx; Type: INDEX; Schema: public; Owner: mail
--

CREATE INDEX users_info_map_sex_uid_idx ON users_info_map_sex USING btree (user_id);


--
-- TOC entry 98 (OID 17653)
-- Name: users_info_map_ints_uid_idx; Type: INDEX; Schema: public; Owner: mail
--

CREATE INDEX users_info_map_ints_uid_idx ON users_info_map_ints USING btree (user_id);


--
-- TOC entry 111 (OID 17856)
-- Name: foo_iarp_pl_dot_type_idx; Type: INDEX; Schema: public; Owner: mail
--

CREATE INDEX foo_iarp_pl_dot_type_idx ON foo_iarp_pl_dot USING btree ("type");


--
-- TOC entry 72 (OID 16986)
-- Name: ip2domain_pkey; Type: CONSTRAINT; Schema: public; Owner: mail
--

ALTER TABLE ONLY ip2domain
    ADD CONSTRAINT ip2domain_pkey PRIMARY KEY (ip);


--
-- TOC entry 73 (OID 17001)
-- Name: login_banned_pkey; Type: CONSTRAINT; Schema: public; Owner: mail
--

ALTER TABLE ONLY login_banned
    ADD CONSTRAINT login_banned_pkey PRIMARY KEY (login);


--
-- TOC entry 74 (OID 17163)
-- Name: www_adm_pkey; Type: CONSTRAINT; Schema: public; Owner: mail
--

ALTER TABLE ONLY www_adm
    ADD CONSTRAINT www_adm_pkey PRIMARY KEY (login);


--
-- TOC entry 76 (OID 17206)
-- Name: domains_pkey; Type: CONSTRAINT; Schema: public; Owner: mail
--

ALTER TABLE ONLY domains
    ADD CONSTRAINT domains_pkey PRIMARY KEY (id);


--
-- TOC entry 75 (OID 17208)
-- Name: domains_name_key; Type: CONSTRAINT; Schema: public; Owner: mail
--

ALTER TABLE ONLY domains
    ADD CONSTRAINT domains_name_key UNIQUE (name);


--
-- TOC entry 77 (OID 17215)
-- Name: ip_pkey; Type: CONSTRAINT; Schema: public; Owner: mail
--

ALTER TABLE ONLY ip
    ADD CONSTRAINT ip_pkey PRIMARY KEY (ip);


--
-- TOC entry 158 (OID 17217)
-- Name: $1; Type: CONSTRAINT; Schema: public; Owner: mail
--

ALTER TABLE ONLY ip
    ADD CONSTRAINT "$1" FOREIGN KEY (domain_id) REFERENCES domains(id) ON UPDATE NO ACTION ON DELETE SET NULL;


--
-- TOC entry 79 (OID 17252)
-- Name: domains_all_pkey; Type: CONSTRAINT; Schema: public; Owner: mail
--

ALTER TABLE ONLY domains_all
    ADD CONSTRAINT domains_all_pkey PRIMARY KEY (id);


--
-- TOC entry 159 (OID 17254)
-- Name: $1; Type: CONSTRAINT; Schema: public; Owner: mail
--

ALTER TABLE ONLY domains_all
    ADD CONSTRAINT "$1" FOREIGN KEY (domain_id) REFERENCES domains(id) ON UPDATE NO ACTION ON DELETE CASCADE;


--
-- TOC entry 81 (OID 17268)
-- Name: domains_prefixes_pkey; Type: CONSTRAINT; Schema: public; Owner: mail
--

ALTER TABLE ONLY domains_prefixes
    ADD CONSTRAINT domains_prefixes_pkey PRIMARY KEY (id);


--
-- TOC entry 160 (OID 17270)
-- Name: $1; Type: CONSTRAINT; Schema: public; Owner: mail
--

ALTER TABLE ONLY domains_prefixes
    ADD CONSTRAINT "$1" FOREIGN KEY (domain_id) REFERENCES domains_all(id) ON UPDATE NO ACTION ON DELETE CASCADE;


--
-- TOC entry 84 (OID 17296)
-- Name: nameservers_pkey; Type: CONSTRAINT; Schema: public; Owner: mail
--

ALTER TABLE ONLY nameservers
    ADD CONSTRAINT nameservers_pkey PRIMARY KEY (id);


--
-- TOC entry 83 (OID 17298)
-- Name: nameservers_name_key; Type: CONSTRAINT; Schema: public; Owner: mail
--

ALTER TABLE ONLY nameservers
    ADD CONSTRAINT nameservers_name_key UNIQUE (name);


--
-- TOC entry 82 (OID 17300)
-- Name: nameservers_ip_key; Type: CONSTRAINT; Schema: public; Owner: mail
--

ALTER TABLE ONLY nameservers
    ADD CONSTRAINT nameservers_ip_key UNIQUE (ip);


--
-- TOC entry 86 (OID 17406)
-- Name: users_info_edu_pkey; Type: CONSTRAINT; Schema: public; Owner: mail
--

ALTER TABLE ONLY users_info_edu
    ADD CONSTRAINT users_info_edu_pkey PRIMARY KEY (id);


--
-- TOC entry 88 (OID 17418)
-- Name: users_info_interests_pkey; Type: CONSTRAINT; Schema: public; Owner: mail
--

ALTER TABLE ONLY users_info_interests
    ADD CONSTRAINT users_info_interests_pkey PRIMARY KEY (id);


--
-- TOC entry 90 (OID 17430)
-- Name: users_info_work_pkey; Type: CONSTRAINT; Schema: public; Owner: mail
--

ALTER TABLE ONLY users_info_work
    ADD CONSTRAINT users_info_work_pkey PRIMARY KEY (id);


--
-- TOC entry 92 (OID 17448)
-- Name: users_info_pkey; Type: CONSTRAINT; Schema: public; Owner: mail
--

ALTER TABLE ONLY users_info
    ADD CONSTRAINT users_info_pkey PRIMARY KEY (id);


--
-- TOC entry 161 (OID 17450)
-- Name: $1; Type: CONSTRAINT; Schema: public; Owner: mail
--

ALTER TABLE ONLY users_info
    ADD CONSTRAINT "$1" FOREIGN KEY (domain_id) REFERENCES domains_all(id) ON UPDATE NO ACTION ON DELETE NO ACTION;


--
-- TOC entry 93 (OID 17457)
-- Name: users_info_map_edu_pkey; Type: CONSTRAINT; Schema: public; Owner: mail
--

ALTER TABLE ONLY users_info_map_edu
    ADD CONSTRAINT users_info_map_edu_pkey PRIMARY KEY (user_id, val_id);


--
-- TOC entry 163 (OID 17459)
-- Name: $1; Type: CONSTRAINT; Schema: public; Owner: mail
--

ALTER TABLE ONLY users_info_map_edu
    ADD CONSTRAINT "$1" FOREIGN KEY (user_id) REFERENCES users_info(id) ON UPDATE NO ACTION ON DELETE CASCADE;


--
-- TOC entry 162 (OID 17463)
-- Name: $2; Type: CONSTRAINT; Schema: public; Owner: mail
--

ALTER TABLE ONLY users_info_map_edu
    ADD CONSTRAINT "$2" FOREIGN KEY (val_id) REFERENCES users_info_edu(id) ON UPDATE NO ACTION ON DELETE NO ACTION;


--
-- TOC entry 95 (OID 17469)
-- Name: users_info_map_work_pkey; Type: CONSTRAINT; Schema: public; Owner: mail
--

ALTER TABLE ONLY users_info_map_work
    ADD CONSTRAINT users_info_map_work_pkey PRIMARY KEY (user_id, val_id);


--
-- TOC entry 165 (OID 17471)
-- Name: $1; Type: CONSTRAINT; Schema: public; Owner: mail
--

ALTER TABLE ONLY users_info_map_work
    ADD CONSTRAINT "$1" FOREIGN KEY (user_id) REFERENCES users_info(id) ON UPDATE NO ACTION ON DELETE CASCADE;


--
-- TOC entry 164 (OID 17475)
-- Name: $2; Type: CONSTRAINT; Schema: public; Owner: mail
--

ALTER TABLE ONLY users_info_map_work
    ADD CONSTRAINT "$2" FOREIGN KEY (val_id) REFERENCES users_info_work(id) ON UPDATE NO ACTION ON DELETE NO ACTION;


--
-- TOC entry 97 (OID 17481)
-- Name: users_info_map_ints_pkey; Type: CONSTRAINT; Schema: public; Owner: mail
--

ALTER TABLE ONLY users_info_map_ints
    ADD CONSTRAINT users_info_map_ints_pkey PRIMARY KEY (user_id, val_id);


--
-- TOC entry 167 (OID 17483)
-- Name: $1; Type: CONSTRAINT; Schema: public; Owner: mail
--

ALTER TABLE ONLY users_info_map_ints
    ADD CONSTRAINT "$1" FOREIGN KEY (user_id) REFERENCES users_info(id) ON UPDATE NO ACTION ON DELETE CASCADE;


--
-- TOC entry 166 (OID 17487)
-- Name: $2; Type: CONSTRAINT; Schema: public; Owner: mail
--

ALTER TABLE ONLY users_info_map_ints
    ADD CONSTRAINT "$2" FOREIGN KEY (val_id) REFERENCES users_info_interests(id) ON UPDATE NO ACTION ON DELETE NO ACTION;


--
-- TOC entry 100 (OID 17513)
-- Name: users_info_areas_pkey; Type: CONSTRAINT; Schema: public; Owner: mail
--

ALTER TABLE ONLY users_info_areas
    ADD CONSTRAINT users_info_areas_pkey PRIMARY KEY (id);


--
-- TOC entry 101 (OID 17518)
-- Name: users_info_map_area_pkey; Type: CONSTRAINT; Schema: public; Owner: mail
--

ALTER TABLE ONLY users_info_map_area
    ADD CONSTRAINT users_info_map_area_pkey PRIMARY KEY (user_id, val_id);


--
-- TOC entry 169 (OID 17520)
-- Name: $1; Type: CONSTRAINT; Schema: public; Owner: mail
--

ALTER TABLE ONLY users_info_map_area
    ADD CONSTRAINT "$1" FOREIGN KEY (user_id) REFERENCES users_info(id) ON UPDATE NO ACTION ON DELETE CASCADE;


--
-- TOC entry 168 (OID 17524)
-- Name: $2; Type: CONSTRAINT; Schema: public; Owner: mail
--

ALTER TABLE ONLY users_info_map_area
    ADD CONSTRAINT "$2" FOREIGN KEY (val_id) REFERENCES users_info_areas(id) ON UPDATE NO ACTION ON DELETE NO ACTION;


--
-- TOC entry 103 (OID 17560)
-- Name: users_info_sex_pkey; Type: CONSTRAINT; Schema: public; Owner: mail
--

ALTER TABLE ONLY users_info_sexes
    ADD CONSTRAINT users_info_sex_pkey PRIMARY KEY (id);


--
-- TOC entry 105 (OID 17567)
-- Name: users_info_map_sex_pkey; Type: CONSTRAINT; Schema: public; Owner: mail
--

ALTER TABLE ONLY users_info_map_sex
    ADD CONSTRAINT users_info_map_sex_pkey PRIMARY KEY (user_id, val_id);


--
-- TOC entry 171 (OID 17569)
-- Name: $1; Type: CONSTRAINT; Schema: public; Owner: mail
--

ALTER TABLE ONLY users_info_map_sex
    ADD CONSTRAINT "$1" FOREIGN KEY (user_id) REFERENCES users_info(id) ON UPDATE NO ACTION ON DELETE CASCADE;


--
-- TOC entry 170 (OID 17573)
-- Name: $2; Type: CONSTRAINT; Schema: public; Owner: mail
--

ALTER TABLE ONLY users_info_map_sex
    ADD CONSTRAINT "$2" FOREIGN KEY (val_id) REFERENCES users_info_sexes(id) ON UPDATE NO ACTION ON DELETE NO ACTION;


--
-- TOC entry 108 (OID 17827)
-- Name: foo_iarp_pl_pkey; Type: CONSTRAINT; Schema: public; Owner: mail
--

ALTER TABLE ONLY foo_iarp_pl
    ADD CONSTRAINT foo_iarp_pl_pkey PRIMARY KEY (id);


--
-- TOC entry 107 (OID 17829)
-- Name: foo_iarp_pl_login_key; Type: CONSTRAINT; Schema: public; Owner: mail
--

ALTER TABLE ONLY foo_iarp_pl
    ADD CONSTRAINT foo_iarp_pl_login_key UNIQUE (login);


--
-- TOC entry 109 (OID 17840)
-- Name: foo_iarp_pl_log_pkey; Type: CONSTRAINT; Schema: public; Owner: mail
--

ALTER TABLE ONLY foo_iarp_pl_log
    ADD CONSTRAINT foo_iarp_pl_log_pkey PRIMARY KEY (id);


--
-- TOC entry 110 (OID 17850)
-- Name: foo_iarp_pl_dot_pkey; Type: CONSTRAINT; Schema: public; Owner: mail
--

ALTER TABLE ONLY foo_iarp_pl_dot
    ADD CONSTRAINT foo_iarp_pl_dot_pkey PRIMARY KEY (id);


--
-- TOC entry 172 (OID 17852)
-- Name: foo_iarp_pl_dot_uid_fkey; Type: CONSTRAINT; Schema: public; Owner: mail
--

ALTER TABLE ONLY foo_iarp_pl_dot
    ADD CONSTRAINT foo_iarp_pl_dot_uid_fkey FOREIGN KEY (uid) REFERENCES foo_iarp_pl(id) ON UPDATE CASCADE ON DELETE CASCADE DEFERRABLE;


--
-- TOC entry 174 (OID 17592)
-- Name: users_info_country_up_set; Type: TRIGGER; Schema: public; Owner: mail
--

CREATE TRIGGER users_info_country_up_set
    BEFORE INSERT OR UPDATE ON users_info
    FOR EACH ROW
    EXECUTE PROCEDURE users_info_country_up_set ();


--
-- TOC entry 173 (OID 17656)
-- Name: users_info_city_up_set; Type: TRIGGER; Schema: public; Owner: mail
--

CREATE TRIGGER users_info_city_up_set
    BEFORE INSERT OR UPDATE ON users_info
    FOR EACH ROW
    EXECUTE PROCEDURE users_info_city_up_set ();


--
-- TOC entry 175 (OID 17662)
-- Name: users_info_re_ans_set; Type: TRIGGER; Schema: public; Owner: mail
--

CREATE TRIGGER users_info_re_ans_set
    BEFORE INSERT ON users_info
    FOR EACH ROW
    EXECUTE PROCEDURE users_info_re_ans_set ();


--
-- TOC entry 176 (OID 17670)
-- Name: users_today_add; Type: RULE; Schema: public; Owner: mail
--

CREATE RULE users_today_add AS ON INSERT TO users_info DO SELECT nextval('users_today_add_seq'::text) AS nextval;


--
-- TOC entry 177 (OID 17671)
-- Name: users_today_rm; Type: RULE; Schema: public; Owner: mail
--

CREATE RULE users_today_rm AS ON INSERT TO users_info DO SELECT nextval('users_today_rm_seq'::text) AS nextval;


--
-- TOC entry 178 (OID 17672)
-- Name: users_all_set; Type: RULE; Schema: public; Owner: mail
--

CREATE RULE users_all_set AS ON INSERT TO users_info DO SELECT setval('users_all_seq'::text, (SELECT count(*) AS count FROM users_info)) AS setval;


--
-- TOC entry 179 (OID 17673)
-- Name: users_all_set1; Type: RULE; Schema: public; Owner: mail
--

CREATE RULE users_all_set1 AS ON DELETE TO users_info DO SELECT setval('users_all_seq'::text, (SELECT count(*) AS count FROM users_info)) AS setval;


--
-- TOC entry 55 (OID 17197)
-- Name: domains_id_seq; Type: SEQUENCE SET; Schema: public; Owner: mail
--

SELECT pg_catalog.setval ('domains_id_seq', 1, true);


--
-- TOC entry 56 (OID 17243)
-- Name: domains_all_id_seq; Type: SEQUENCE SET; Schema: public; Owner: mail
--

SELECT pg_catalog.setval ('domains_all_id_seq', 1, true);


--
-- TOC entry 57 (OID 17258)
-- Name: domains_prefixes_id_seq; Type: SEQUENCE SET; Schema: public; Owner: mail
--

SELECT pg_catalog.setval ('domains_prefixes_id_seq', 3, true);


--
-- TOC entry 58 (OID 17286)
-- Name: nameservers_id_seq; Type: SEQUENCE SET; Schema: public; Owner: mail
--

SELECT pg_catalog.setval ('nameservers_id_seq', 1, true);


--
-- TOC entry 59 (OID 17396)
-- Name: users_info_edu_id_seq; Type: SEQUENCE SET; Schema: public; Owner: mail
--

SELECT pg_catalog.setval ('users_info_edu_id_seq', 6, true);


--
-- TOC entry 60 (OID 17408)
-- Name: users_info_interests_id_seq; Type: SEQUENCE SET; Schema: public; Owner: mail
--

SELECT pg_catalog.setval ('users_info_interests_id_seq', 33, true);


--
-- TOC entry 61 (OID 17420)
-- Name: users_info_work_id_seq; Type: SEQUENCE SET; Schema: public; Owner: mail
--

SELECT pg_catalog.setval ('users_info_work_id_seq', 17, true);


--
-- TOC entry 62 (OID 17435)
-- Name: users_info_id_seq; Type: SEQUENCE SET; Schema: public; Owner: mail
--

SELECT pg_catalog.setval ('users_info_id_seq', 44, true);


--
-- TOC entry 64 (OID 17503)
-- Name: users_info_areas_id_seq; Type: SEQUENCE SET; Schema: public; Owner: mail
--

SELECT pg_catalog.setval ('users_info_areas_id_seq', 16, true);


--
-- TOC entry 65 (OID 17550)
-- Name: users_info_sex_id_seq; Type: SEQUENCE SET; Schema: public; Owner: mail
--

SELECT pg_catalog.setval ('users_info_sex_id_seq', 2, true);


--
-- TOC entry 3 (OID 17664)
-- Name: users_all_seq; Type: SEQUENCE SET; Schema: public; Owner: mail
--

SELECT pg_catalog.setval ('users_all_seq', 11, true);


--
-- TOC entry 6 (OID 17666)
-- Name: users_today_add_seq; Type: SEQUENCE SET; Schema: public; Owner: mail
--

SELECT pg_catalog.setval ('users_today_add_seq', 29, true);


--
-- TOC entry 9 (OID 17668)
-- Name: users_today_rm_seq; Type: SEQUENCE SET; Schema: public; Owner: mail
--

SELECT pg_catalog.setval ('users_today_rm_seq', 28, true);


--
-- TOC entry 66 (OID 17816)
-- Name: foo_iarp_pl_id_seq; Type: SEQUENCE SET; Schema: public; Owner: mail
--

SELECT pg_catalog.setval ('foo_iarp_pl_id_seq', 23, true);


--
-- TOC entry 68 (OID 17831)
-- Name: foo_iarp_pl_log_id_seq; Type: SEQUENCE SET; Schema: public; Owner: mail
--

SELECT pg_catalog.setval ('foo_iarp_pl_log_id_seq', 62, true);


--
-- TOC entry 70 (OID 17842)
-- Name: foo_iarp_pl_dot_id_seq; Type: SEQUENCE SET; Schema: public; Owner: mail
--

SELECT pg_catalog.setval ('foo_iarp_pl_dot_id_seq', 37, true);


