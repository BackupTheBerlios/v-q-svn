CREATE TABLE "log" (
		"id" serial,
        "domain" text default '' not null,
        "login" text default '' not null,
		"service" smallint default 0 not null,
		"result" smallint default 0 not null,
        "ip" text default '' not null,
        "msg" text default '' not null,
        "time" timestamp with time zone DEFAULT now() not null,
		primary key(id)
);

create index log_domain_login_idx on log (domain,login);

--
-- TOC Entry ID 48 (OID 288534)
--
-- Name: log Type: ACL Owner:
--

REVOKE ALL on "log" from PUBLIC;
GRANT ALL on "log" to "mail";
